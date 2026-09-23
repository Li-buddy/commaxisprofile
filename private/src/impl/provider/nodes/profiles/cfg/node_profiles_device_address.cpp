/// @file
///
/// @brief This file is part of the axisprofile library and implements the node
/// to set the device address of a specific profile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "manager/i_axisprofile_manager_internal.h"
#include "node_profiles_device_address.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::provider;


// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------


NodeProfilesDeviceAddress::NodeProfilesDeviceAddress(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  m_metaDataInfo.m_description = "device address";
  m_metaDataInfo.m_descriptionUrl = "";
  m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesDeviceAddress::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  comm::axisprofile::IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Finish
  comm::datalayer::Variant returnVal;
  std::uint32_t deviceAddress = profile->getPropertyInterface()->getDeviceAddress();
  returnVal.setValue(deviceAddress);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeProfilesDeviceAddress::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Check and extract the received variant
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::UINT32, callback))
  {
    return;
  }

  const std::uint32_t deviceAddress = *(reinterpret_cast<std::uint32_t*>(data->getData()));

  // Get profile
  comm::axisprofile::IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  AxisProfileResult aprResult = profile->getPropertyInterface()->setDeviceAddress(deviceAddress);
  if (aprStatusSucceeded(aprResult))
  {
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, "set device address %d failed", deviceAddress);
  }
}

void NodeProfilesDeviceAddress::onMetadata(const std::string& address, const ResponseCallback& callback)
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

  std::string typeString{};
  if (!createBasicTypeString(name, comm::datalayer::VariantType::UINT32, typeString, callback))
  {
    return;
  }
  ProviderHelper::createMetaData(typeString, m_metaDataInfo, callback);
}
