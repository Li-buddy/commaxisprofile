/// @file
///
/// @brief This file is part of the axisprofile library and defines the node information for enabling checksum node.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_use_checksum.h"
#include "profiles/i_axisprofile_internal.h"
#include "common/debug_trace.h"

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

NodeProfilesUseChecksum::NodeProfilesUseChecksum(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_description = "use checksum";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesUseChecksum::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if ( profile == nullptr )
  {
    return;
  }

  // Check and extract the received variant
  if ( !ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::BOOL8, callback) )
  {
    return;
  }
  profile->getPropertyInterface()->enableChecksum(*data);
  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeProfilesUseChecksum::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if ( !profile )
  {
    return;
  }
  
  bool enable = profile->getPropertyInterface()->isChecksumEnabled();
  comm::datalayer::Variant returnVal;
  returnVal.setValue(enable);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeProfilesUseChecksum::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBasicType(address, callback, comm::datalayer::VariantType::BOOL8);
}
