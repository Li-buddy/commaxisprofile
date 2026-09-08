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

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the state node class for simple data types.
  template <typename SIMPLE_TYPE, typename DATA_OBJ, typename SET_RET_TYPE>
  class NodeCfgSimple : public NodeProfilesBase
  {
  public: // methods

    /// Declaration of data types
    /// deduce function pointer type from template
    /// function pointer to getter cfg interface
    typedef std::function<SIMPLE_TYPE(const DATA_OBJ&)> GetterProfile;

    /// function pointer to setter cfg interface
    typedef std::function<SET_RET_TYPE(DATA_OBJ&, SIMPLE_TYPE)> SetterProfile;

  public:
    /// @brief construct
    /// @param mgr           pointer to the provider manager
    /// @param getter        get value function pointer
    /// @param setter        set value function pointer
    /// @param metaDataInfo   metadata info of this node
    NodeCfgSimple(ProviderMgr* mgr, const GetterProfile& getter, const SetterProfile& setter, const MetaDataInfo& metaDataInfo)
      : NodeProfilesBase(mgr), m_cfgGetterFnPtr(getter), m_cfgSetterFnPtr(setter)
    {
      this->m_metaDataInfo = metaDataInfo;
      m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
      m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
    }

    /// Destructor
    virtual ~NodeCfgSimple() noexcept override = default;

    /// Read function of a node. Function will be called whenever a node should be read.
    /// @param[in]  address   Address of the node to read
    /// @param[in]  data      Data of the object
    /// @param[in]  callback  Callback to call when function is finished
    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

    /// Read function of a node. Function will be called whenever a node should be write.
    /// @param[in]  address   Address of the node to write
    /// @param[in]  data      Data of the object
    /// @param[in]  callback  Callback to call when function is finished
    void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

    /// Read function of a node. Function will be called whenever a node need metadata.
    /// @param[in]  address   Address of the node to get metadata
    /// @param[in]  callback  Callback to call when function is finished
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

  protected: // data
    GetterProfile m_cfgGetterFnPtr;  ///< function pointer to getter cfg interface class
    SetterProfile m_cfgSetterFnPtr;  ///< function pointer to setter cfg interface class
  };

  // -------------------------------------------------------------------------------
  // -------------------------------Implementation----------------------------------
  // ------------------------------------------------------------------------------
  template <typename SIMPLE_TYPE, typename DATA_OBJ, typename SET_RET_TYPE>
  void NodeCfgSimple<SIMPLE_TYPE, DATA_OBJ, SET_RET_TYPE>::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
  {
    // get object data
    DATA_OBJ* profile = getProfileByAddress(address, callback);
    if (!profile)
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

    const SIMPLE_TYPE value = *(reinterpret_cast<SIMPLE_TYPE*>(data->getData()));
    auto* profile = getProfileByAddress(address, callback);
    if (!profile)
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
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, "%d Write failed", value);
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
