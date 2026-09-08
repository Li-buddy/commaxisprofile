/// @file
///
/// @brief This file is part of the axisprofile library and defines the node for profile basic data configuration.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_basicData.h"
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

NodeProfilesBasicData::NodeProfilesBasicData(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "profile.bfbs";
  this->m_metaDataInfo.m_description = "Profile basic data node";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ;
}

void NodeProfilesBasicData::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get mangr pointer
  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  std::string profileName{};
  ProviderHelper::getProfileNameFromAddress(address, profileName);

  // Get profile pointer
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(profileManager->getProfileByName(profileName.c_str()));
  if (!profile)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_PROFILE_NOT_FOUND, "profile %s not found", profileName.c_str());
    return;
  }

  NodeProfiles::profileFbData profileBasicData{};
  profileBasicData.m_profileName = profileName;
  profileBasicData.m_profileType = profile->getPropertyInterface()->getProfileType();

  // Finish
  comm::datalayer::Variant retVariant;
  if (aprStatusFailed(generateFlatbufferData(retVariant, profileBasicData)))
  {
    return;
  }
  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesBasicData::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

comm::axisprofile::AxisProfileResult NodeProfilesBasicData::generateFlatbufferData(comm::datalayer::Variant& data, const NodeProfiles::profileFbData& profileBasicData)
{
  // Build basic profile flatbuffer
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  
  auto profileOffset = NodeProfiles::generateFlatbufferData(builder, profileBasicData);

  // Finish
  builder.Finish(profileOffset);
  data.copyFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}
