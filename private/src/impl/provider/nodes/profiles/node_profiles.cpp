// ---------------------------------------------------------------------------
//
//! @file       node_system_act_statemachine.cpp
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This file is part of the axisprofile library and defines the nodes for 
//! the actual value of the axisprofile state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles.h"
#include "component/i_axisprofile_component.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "provider/provider_mgr.h"


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


NodeProfiles::NodeProfiles(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "profile.bfbs";
  this->m_metaDataInfo.m_description = "name of profile";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Resource;
  this->m_metaDataInfo.nodeActions = node_actions::CREATE | node_actions::REMOVE;
}

void NodeProfiles::onCreate(const std::string& /*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::FLATBUFFERS, callback))
  {
    return;
  }

  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifyprofileBuffer(verifier))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return;
  }

  profileFbData profileFbData{};
  auto variantData = fbtypes::Getprofile(data->getData());
  if (aprStatusFailed(extractFlatbufferData(variantData, profileFbData)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "extract flatbuffer failed");
    return;
  }

  std::string errTxt;
  if (!ProviderHelper::checkProfileName(profileFbData.m_profileName, errTxt))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_PROFILE_NAME, "%s", errTxt.c_str());
    return;
  }

  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  if (aprStatusFailed(profileManager->createAxisProfile(profileFbData.m_profileType, profileFbData.m_profileName)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_CREATION_FAILED, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_CREATE_PROFILE_FAILED, "create profile failed");
    return;
  }

  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeProfiles::onRemove(const std::string& address, const ResponseCallback& callback)
{
  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  std::string profileName;
  ProviderHelper::getProfileNameFromAddress(address, profileName);

  if (aprStatusFailed(profileManager->deleteAxisProfile(profileName)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_FAILED, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_DELETE_PROFILE_FAILED, "delete profile name %s failed", profileName.c_str());
    return;
  }

  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeProfiles::onBrowse(const std::string& /*address*/, const ResponseCallback& callback)
{
  if (!m_manager)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_SYSTEM_INCONSISTENT, "no provider manager");
    return;
  }

  IAxisProfileManagerInternal* profileManager = m_manager->getAxisProfileCmp()->getAxisProfileManager();
  if( !profileManager )
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_SYSTEM_INCONSISTENT, "no profile manager");
    return;
  }

  std::vector<std::string> profileNames;
  profileManager->getProfileNames(profileNames);

  comm::datalayer::Variant result;
  result.setValue(profileNames);
  callback(comm::datalayer::DlResult::DL_OK, &result);
}

void NodeProfiles::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

comm::axisprofile::AxisProfileResult NodeProfiles::extractFlatbufferData(const comm::axisprofile::fbtypes::profile* data, profileFbData& profileFbData)
{
  if (!flatbuffers::IsFieldPresent(data, fbtypes::profile::VT_NAME) ||
      !flatbuffers::IsFieldPresent(data, fbtypes::profile::VT_TYPE))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  profileFbData.m_profileName = flatbuffers::GetString(data->name());
  profileFbData.m_profileType = stringToAxisProfileType(fbtypes::EnumNameProfileType(data->type()));

  return AxisProfileResult::STS_OK;
}

flatbuffers::Offset<comm::axisprofile::fbtypes::profile> NodeProfiles::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, const profileFbData& data)
{
  auto profileNameOffset = fbb.CreateString(data.m_profileName);

  fbtypes::profileBuilder profileBuilder(fbb);
  profileBuilder.add_name(profileNameOffset);
  profileBuilder.add_type(fbtypes::EnumValuesProfileType()[static_cast<std::uint32_t>(data.m_profileType)]);

  return profileBuilder.Finish();
}
