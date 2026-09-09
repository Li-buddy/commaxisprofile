/// @file
///
/// @brief This file is part of the axisprofile library and defines the node information for enabling checksum node of all profiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/debug_trace.h"
#include "node_system_use_checksum.h"
#include "profiles/i_axisprofile_internal.h"
#include "provider/provider_helper.h"
#include "provider/provider_mgr.h"
#include "manager/i_axisprofile_manager_internal.h"

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

NodeSystemUseChecksum::NodeSystemUseChecksum(ProviderMgr* mgr) : NodeBase(mgr)
{
  this->m_metaDataInfo.m_description = "use checksum for all profiles";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeSystemUseChecksum::onWrite(const std::string& /*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  auto profileManager = m_manager->getAxisProfileCmp()->getAxisProfileManager();
  std::vector<std::string> profileNames;
  profileManager->getProfileNames(profileNames);
  if (profileNames.empty())
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR,
                                        DETAILED_DIAG_PROFILE_NOT_FOUND, "profile not existed");
    return;
  }

  // Check and extract the received variant
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::BOOL8, callback))
  {
    return;
  }

  for (const auto& name : profileNames)
  {
    IAxisProfileInternal* profile = getProfileInternal(name, callback);
    if (nullptr == profile)
    {
      continue;
    }
    profile->getPropertyInterface()->enableChecksum(*data);
  }
  callback(comm::datalayer::DlResult::DL_OK, nullptr);  
}

void NodeSystemUseChecksum::onRead(const std::string& /*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  comm::datalayer::Variant returnVal;
  returnVal.setValue(false);
  auto profileManager = m_manager->getAxisProfileCmp()->getAxisProfileManager();
  std::vector<std::string> profileNames;
  profileManager->getProfileNames(profileNames);
  if (profileNames.empty())
  {
    callback(comm::datalayer::DlResult::DL_OK, &returnVal);
    return;
  }

  bool enable = true;
  for (const auto& name : profileNames)
  {
    IAxisProfileInternal* profile = getProfileInternal(name, callback);
    if (nullptr == profile)
    {
      continue;
    }
    enable = enable && profile->getPropertyInterface()->isChecksumEnabled();
    if (!enable)
    {
      break;
    }
  }
  returnVal.setValue(enable);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeSystemUseChecksum::onMetadata(const std::string& /*address*/, const ResponseCallback& callback)
{
  ProviderHelper::createMetaData(comm::datalayer::DATALAYER_TYPE_BOOL8, this->m_metaDataInfo, callback);
}

comm::axisprofile::IAxisProfileInternal* NodeSystemUseChecksum::getProfileInternal(const std::string& name, const ResponseCallback& callback)
{
  auto profileManager = m_manager->getAxisProfileCmp()->getAxisProfileManager();
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(profileManager->getProfileByName(name.c_str()));
  if (nullptr == profile)
  {
    APR_TRACE_WARNING("%s, not been found.", name.c_str());
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_PROFILE_NOT_FOUND, "profile %s not found", name.c_str());
  }
  return profile;
}
