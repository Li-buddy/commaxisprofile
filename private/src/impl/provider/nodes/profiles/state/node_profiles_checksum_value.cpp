/// @file
///
/// This header file is part of the axisprofile library and defines the node information for checksum value.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_checksum_value.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "profiles/i_device_boot_param_snapshot.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::provider;

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
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Implementation of classes and structures
// ---------------------------------------------------------------------------

NodeProfilesChecksumValue::NodeProfilesChecksumValue( ProviderMgr* mgr ) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_description = "checksum value";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ;
}

void NodeProfilesChecksumValue::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if ( !profile )
  {
    return;
  }
  IDeviceBootParamSnapshot* snapshot = profile->getDeviceBootParmSnapshotInterface();
  if (!snapshot)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR,
                                        DETAILED_DIAG_SYSTEM_INCONSISTENT, "boot parameter snapshot interface not found");
    return;
  }
  std::uint32_t value = snapshot->getBootParamSnapshot().checksumValue;
  
  // Finish
  comm::datalayer::Variant returnVal;
  returnVal.setValue(value);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeProfilesChecksumValue::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBasicType(address, callback, comm::datalayer::VariantType::UINT32);
}
