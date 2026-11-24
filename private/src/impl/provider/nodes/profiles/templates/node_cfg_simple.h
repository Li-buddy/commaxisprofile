#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and declare datalayer node of vendor adaption configuration.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
{
  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the config node template class for simple data types.
  /// @tparam SIMPLE_TYPE   The simple data type (e.g., double, bool, ...)
  /// @tparam PROFILE_IF    The profile interface type (e.g., IAxisProfilePropertyInternal, IAxisProfileInternal, ...)
  template <typename SIMPLE_TYPE, typename PROFILE_IF>
  class NodeCfgSimple : public NodeProfilesBase
  {
  public: // methods

    /// function pointer to getter of profile interface interface
    typedef std::function<SIMPLE_TYPE(const PROFILE_IF&)> GetterAxs;

    /// function pointer to setter of profile interface interface
    typedef std::function<bool(PROFILE_IF&, SIMPLE_TYPE)> SetterAxs;

    /// @brief function pointer to a function, that is called after the write operation
    using PostWriteFct = std::function<bool(const std::string& address, SIMPLE_TYPE value, const ResponseCallback& callback)>;

    /// @brief Constructor
    /// @param[in]   mgr             pointer to the provider manager
    /// @param[in]   getter          function pointer for getter to return config values
    /// @param[in]   setter          function pointer for setter to set config values
    /// @param[in]   metaDataInfo    metadata info of this node
    NodeCfgSimple(ProviderMgr* mgr, const GetterAxs& getter, const SetterAxs& setter, const MetaDataInfo& metaDataInfo, const PostWriteFct& postWriteFct = {})
      : NodeProfilesBase(mgr), m_cfgGetterFnPtr(getter), m_cfgSetterFnPtr(setter), m_postWriteFct(postWriteFct)
    {
      this->m_metaDataInfo = metaDataInfo;
      this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
      this->m_metaDataInfo.nodeActions = node_actions::WRITE | node_actions::READ;
    };

    /// @brief Destructor
    virtual ~NodeCfgSimple() noexcept override = default;

    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

  protected: // data

    GetterAxs m_cfgGetterFnPtr;  ///< function pointer to getter cfg interface class
    SetterAxs m_cfgSetterFnPtr;  ///< function pointer to setter cfg interface class
    PostWriteFct m_postWriteFct; ///< function pointer to a function, that is called after the write operation
  };

  // -------------------------------------------------------------------------------
  // -------------------------------Implementation----------------------------------
  // -------------------------------------------------------------------------------

  template <typename SIMPLE_TYPE, typename PROFILE_IF>
  void NodeCfgSimple<SIMPLE_TYPE, PROFILE_IF>::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
  {
    comm::datalayer::Variant retVariant;
    comm::datalayer::DlResult dlResult;

    PROFILE_IF* profileIf {};
    dlResult = NodeProfilesBase::getObjectData(address, profileIf, callback);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      return;
    }

    // return variant
    retVariant.setValue(m_cfgGetterFnPtr(*profileIf));
    callback(comm::datalayer::DlResult::DL_OK, &retVariant);
  }

  template <typename SIMPLE_TYPE, typename PROFILE_IF>
  void NodeCfgSimple<SIMPLE_TYPE, PROFILE_IF>::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
  {
    if (!data)
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE,
                                          MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE, "Can not write address: %s", address.c_str());
      return;
    }

    auto type = comm::datalayer::getVariantTypeID<SIMPLE_TYPE>();
    if constexpr (std::is_same_v<SIMPLE_TYPE, std::vector<std::string>>)
    {
      type = comm::datalayer::VariantType::ARRAY_OF_STRING;
    }
    if (data->getType() != type)
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH,
                                          MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE, "Wrong type of data");
      return;
    }

    PROFILE_IF* profileIf {};
    comm::datalayer::DlResult dlResult;

    dlResult = NodeProfilesBase::getObjectData(address, profileIf, callback);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      return;
    }

    SIMPLE_TYPE value {};
    if constexpr (std::is_same_v<SIMPLE_TYPE, std::string>)
    {
      // specialization for std::string
      const char* txt = *data;
      value = txt;       // cppcheck-suppress AssignmentAddressToInteger    - false positive: value is a std::string and no integer
    }
    else
    {
      if constexpr (std::is_same_v<SIMPLE_TYPE, std::vector<std::string>>)
      {
        // specialization for std::vector<std::string>
        const char** txt = data->operator const char** ();
        for (std::uint32_t i = 0; i < data->getCount(); i++)
        {
          value.push_back(txt[i]);
        }
      }
      else
      {
        value = *data;
      }
    }

    if (!m_cfgSetterFnPtr(*profileIf, value))
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_SUBMODULE_FAILURE,
                                          MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE, "Write failed");
      return;
    }

    if (m_postWriteFct && !m_postWriteFct(address, value, callback))
    {
      return;
    }

    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }

  template <typename SIMPLE_TYPE, typename PROFILE_IF>
  void NodeCfgSimple<SIMPLE_TYPE, PROFILE_IF>::onMetadata(const std::string& address, const ResponseCallback& callback)
  {
    comm::datalayer::VariantType variantTypeID = comm::datalayer::getVariantTypeID<SIMPLE_TYPE>();
    if constexpr (std::is_same_v<SIMPLE_TYPE, std::vector<std::string>>)
    {
      variantTypeID = comm::datalayer::VariantType::ARRAY_OF_STRING;
    }
    std::string typeString;
    if (!createBasicTypeString(address, variantTypeID, typeString, callback))
    {
      return;
    }
    ProviderHelper::createMetaData3(address, typeString, this->m_metaDataInfo, callback);
  }
}
