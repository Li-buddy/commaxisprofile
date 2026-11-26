#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines template for simple uint profile nodes.
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
#include <functional>
#include <type_traits>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
{
  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief Configuration node template for simple data types.
  /// @tparam SIMPLE_TYPE   Type of the configuration value.
  /// @tparam DATA_OBJ      Interface providing the getter and setter.
  /// @tparam SET_RET_TYPE  Return type of the setter (bool or AxisProfileResult).
  template <typename SIMPLE_TYPE, typename DATA_OBJ, typename SET_RET_TYPE = bool>
  class NodeCfgSimple : public NodeProfilesBase
  {
  public: // methods

    /// Function pointer to getter of the configuration interface.
    typedef std::function<SIMPLE_TYPE(const DATA_OBJ&)> GetterProfile;

    /// Function pointer to setter of the configuration interface.
    typedef std::function<SET_RET_TYPE(DATA_OBJ&, SIMPLE_TYPE)> SetterProfile;

    /// @brief Constructor.
    /// @param[in] mgr           Pointer to the provider manager.
    /// @param[in] getter        Getter of the configuration value.
    /// @param[in] setter        Setter of the configuration value.
    /// @param[in] metaDataInfo  Metadata information of this node.
    NodeCfgSimple(ProviderMgr* mgr, const GetterProfile& getter, const SetterProfile& setter, const MetaDataInfo& metaDataInfo)
      : NodeProfilesBase(mgr), m_cfgGetterFnPtr(getter), m_cfgSetterFnPtr(setter)
    {
      this->m_metaDataInfo = metaDataInfo;
      m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
      m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
    }

    /// Destructor.
    virtual ~NodeCfgSimple() noexcept override = default;

    /// @brief Read the configuration value.
    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

    /// @brief Write the configuration value.
    void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

    /// @brief Return metadata of the configuration node.
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

  protected: // data

    GetterProfile m_cfgGetterFnPtr;  ///< Function pointer to getter cfg interface class.
    SetterProfile m_cfgSetterFnPtr;  ///< Function pointer to setter cfg interface class.
  };

  // -------------------------------------------------------------------------------
  // -------------------------------Implementation----------------------------------
  // ------------------------------------------------------------------------------
  template <typename SIMPLE_TYPE, typename DATA_OBJ, typename SET_RET_TYPE>
  void NodeCfgSimple<SIMPLE_TYPE, DATA_OBJ, SET_RET_TYPE>::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
  {
    // Get object data.
    DATA_OBJ* profile {};
    const auto dlResult = NodeProfilesBase::getObjectData(address, profile, callback);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      return;
    }

    auto value = m_cfgGetterFnPtr(*profile);
    comm::datalayer::Variant returnVal;
    returnVal.setValue(value);
    callback(comm::datalayer::DlResult::DL_OK, &returnVal);
  }

  template <typename SIMPLE_TYPE, typename DATA_OBJ, typename SET_RET_TYPE>
  void NodeCfgSimple<SIMPLE_TYPE, DATA_OBJ, SET_RET_TYPE>::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
  {
    auto type = comm::datalayer::getVariantTypeID<SIMPLE_TYPE>();
    if (!ProviderHelper::checkVariantType(data, type, callback))
    {
      return;
    }

    const SIMPLE_TYPE value = *data;

    // Get object data.
    DATA_OBJ* profile {};
    const auto dlResult = NodeProfilesBase::getObjectData(address, profile, callback);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      return;
    }

    SET_RET_TYPE ret = m_cfgSetterFnPtr(*profile, value);
    bool result = false;
    if constexpr (std::is_same_v<SET_RET_TYPE, comm::axisprofile::AxisProfileResult>)
    {
      result = aprStatusSucceeded(ret);
    }
    else
    {
      result = static_cast<bool>(ret);
    }

    if (result)
    {
      callback(comm::datalayer::DlResult::DL_OK, nullptr);
    }
    else
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE,
                                          MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE,
                                          "Write failed");
    }
  }

  template <typename SIMPLE_TYPE, typename DATA_OBJ, typename SET_RET_TYPE>
  void NodeCfgSimple<SIMPLE_TYPE, DATA_OBJ, SET_RET_TYPE>::onMetadata(const std::string& address, const ResponseCallback& callback)
  {
    std::string name;
    ProviderHelper::getProfileNameFromAddress(address, name);

    if (!name.empty())
    {
      // The address string contains a profile name. Check if it exists in the system.
      if (!getProfileByAddress(address, callback))
      {
        return;
      }
    }

    auto type = comm::datalayer::getVariantTypeID<SIMPLE_TYPE>();
    std::string typeString {};
    if (!createBasicTypeString(name, type, typeString, callback))
    {
      return;
    }
    ProviderHelper::createMetaData(typeString, m_metaDataInfo, callback);
  }
}
