/// @file
///
/// This source file is part of the axisprofile library and implement the node information for device name node.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_device_name.h"
#include "profiles/i_axisprofile_internal.h"

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

NodeProfilesDeviceName::NodeProfilesDeviceName(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "profileConfig.bfbs";
  this->m_metaDataInfo.m_description = "device name";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesDeviceName::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if ( !profile )
  {
    return;
  }

  // allow empty name
  const char* cNewDeviceName = *data;
  if ( strlen(cNewDeviceName) == 0 )
  {
    profile->getPropertyInterface()->setDeviceName(std::string().c_str());
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
    return;
  }

  // Check and extract the received variant
  if ( !ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::STRING, callback) )
  {
    return;
  }  

  // Write variant data
  // log original name
  std::string cOriginalDeviceName = profile->getPropertyInterface()->getDeviceName();
  // set new name
  AxisProfileResult aprReturn = profile->getPropertyInterface()->setDeviceName(cNewDeviceName);

  // Finish
  if ( aprStatusSucceeded(aprReturn))
  {
    // after success set then update the address according to device name
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    //when error or discard, need clean the name to original one.
    profile->getPropertyInterface()->setDeviceName(cOriginalDeviceName.c_str());
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_DEVICENAME, "Set device name failed!");
  }
}

void NodeProfilesDeviceName::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if ( !profile )
  {
    return;
  }

  // Get configured output URI
  std::string strDeviceName = profile->getPropertyInterface()->getDeviceName();
  
  // Finish
  comm::datalayer::Variant returnVal;
  strDeviceName.empty() ? returnVal.setValue("") : returnVal.setValue(strDeviceName);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeProfilesDeviceName::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}
