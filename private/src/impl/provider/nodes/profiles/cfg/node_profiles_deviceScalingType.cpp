/// @file
///
/// @brief This file is part of the axisprofile library and implement
/// the node for device scaling type configuration.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "node_profiles_deviceScalingType.h"

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
NodeProfilesDeviceScalingType::NodeProfilesDeviceScalingType(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "profileDeviceScalingType.bfbs";
  this->m_metaDataInfo.m_description = "device scaling type";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesDeviceScalingType::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get manager pointer
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

  comm::axisprofile::AxisProfileDeviceScalingType profileDeviceScalingType{};
  profileDeviceScalingType = profile->getPropertyInterface()->getDeviceScalingType(AxisProfileDeviceValueType::POSITION);
  comm::datalayer::Variant retVariant;
  // Build basic profile flatbuffer
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto typeOffset = generateFlatbufferData(builder, profileDeviceScalingType);
  // Finish
  builder.Finish(typeOffset);
  retVariant.copyFlatbuffers(builder);

  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesDeviceScalingType::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile  
  comm::axisprofile::IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Check the variant type
  comm::axisprofile::AxisProfileDeviceScalingType profileDeviceScalingType{};
  if (aprStatusFailed(extractFlatbufferData(data, profileDeviceScalingType, callback)))
  {
    return;
  }

  // Finish
  AxisProfileScalingStrategyCfg scalingCfg = profile->getProfileScalingStrategyCfg();
  scalingCfg.m_deviceScalingType = profileDeviceScalingType;
  profile->setProfileScalingStrategyCfg(scalingCfg);
  callback(comm::datalayer::DlResult::DL_OK, nullptr);  
}

comm::axisprofile::AxisProfileResult NodeProfilesDeviceScalingType::extractFlatbufferData(const comm::datalayer::Variant* data, comm::axisprofile::AxisProfileDeviceScalingType& profileDeviceScalingType, const ResponseCallback& callback)
{
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::FLATBUFFERS, callback))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Verify the flatbuffer
  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifyprofileDeviceScalingTypeBuffer(verifier))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  //get fbs profile
  auto variantData = fbtypes::GetprofileDeviceScalingType(data->getData());

  //check field present
  if (aprStatusFailed(extractFlatbufferData(variantData, profileDeviceScalingType)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check Field Present failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  return AxisProfileResult::STS_OK;
}

comm::axisprofile::AxisProfileResult NodeProfilesDeviceScalingType::extractFlatbufferData(const comm::axisprofile::fbtypes::profileDeviceScalingType* data, comm::axisprofile::AxisProfileDeviceScalingType& profileDeviceScalingType)
{
  //check field present
  if (!flatbuffers::IsFieldPresent(data, fbtypes::profileDeviceScalingType::VT_SCALINGTYPE))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }
  profileDeviceScalingType = stringToAxisProfileDeviceScalingType(fbtypes::EnumNamedeviceScalingType(data->scalingType()));
  return AxisProfileResult::STS_OK;
}

void NodeProfilesDeviceScalingType::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

flatbuffers::Offset<comm::axisprofile::fbtypes::profileDeviceScalingType> NodeProfilesDeviceScalingType::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, comm::axisprofile::AxisProfileDeviceScalingType scalingType)
{
  //get type offset data
  fbtypes::profileDeviceScalingTypeBuilder ScalingTypeDataBuilder(fbb);
  ScalingTypeDataBuilder.add_scalingType(fbtypes::EnumValuesdeviceScalingType()[static_cast<std::int32_t>(scalingType)+1]);
  return ScalingTypeDataBuilder.Finish();
}
