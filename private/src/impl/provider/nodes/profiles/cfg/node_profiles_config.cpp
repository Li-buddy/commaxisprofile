/// @file
///
/// @brief This file is part of the axisprofile library and implements the configuration node for all profiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_config.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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

NodeProfilesConfig::NodeProfilesConfig( ProviderMgr* mgr ) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "profileConfig.bfbs";
  this->m_metaDataInfo.m_description = "Profile configuration node";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesConfig::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get profile manager pointer
  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  // Get profile name
  std::string profileName{};
  ProviderHelper::getProfileNameFromAddress(address, profileName);

  // Get profile pointer
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(profileManager->getProfileByName(profileName.c_str()));
  if (!profile)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_PROFILE_NOT_FOUND, "profile %s not found", profileName.c_str());
    return;
  }
  
  // Collect configuration data
  const char* inBuffer = profileManager->getProfileInputBuffer(profileName.c_str());
  const char* outBuffer = profileManager->getProfileOutputBuffer(profileName.c_str());

  profileConfig profileConfigData{};
  profileConfigData.m_profile            = { profileName, profile->getPropertyInterface()->getProfileType() };
  profileConfigData.m_inputBuffer        = inBuffer  ? inBuffer  : std::string();
  profileConfigData.m_outputBuffer       = outBuffer ? outBuffer : std::string();
  profileConfigData.m_deviceAddress      = profile->getPropertyInterface()->getDeviceAddress();
  profileConfigData.m_deviceName         = profile->getPropertyInterface()->getDeviceName();
  profileConfigData.m_inputValueMapping  = profile->getMappingHandler()->getConfiguredInputs();
  profileConfigData.m_outputValueMapping = profile->getMappingHandler()->getConfiguredOutputs();
  profileConfigData.m_scalingStrategyCfg = profile->getProfileScalingStrategyCfg();
  profileConfigData.m_driveNumber        = profile->getDriveNumber();
  profileConfigData.m_profileCategory    = profile->getPropertyInterface()->getProfileCategory();
  profileConfigData.m_checksumEnable     = profile->getPropertyInterface()->isChecksumEnabled();

  // Convert internal data into flatbuffer variant
  comm::datalayer::Variant retVariant;
  if (aprStatusFailed(generateFlatbufferData(retVariant, profileConfigData, callback)))
  {
    return;
  }

  // Finish
  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesConfig::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile manager pointer
  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  // Get profile name
  std::string profileName;
  ProviderHelper::getProfileNameFromAddress(address, profileName);

  // Get profile pointer
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(profileManager->getProfileByName(profileName.c_str()));
  if (!profile)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_ADDRESS, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_PROFILE_NOT_FOUND, "profile %s not found", profileName.c_str());
    return;
  }

  // Check and extract the received variant
  profileConfig profileConfigData{};
  if (aprStatusFailed(extractFlatbufferData(data, profileConfigData, callback)))
  {
    return;
  }

  // Check consistency of the extracted data
  if ((profileConfigData.m_profile.m_profileName != profileName) || (profileConfigData.m_profile.m_profileType != profile->getPropertyInterface()->getProfileType()))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_SYSTEM_INCONSISTENT, "extract data not consistent", profileName.c_str());
    return;
  }

  // Write data
  profileManager->setProfileInputBuffer(profileName.c_str(), profileConfigData.m_inputBuffer.c_str());
  profileManager->setProfileOutputBuffer(profileName.c_str(), profileConfigData.m_outputBuffer.c_str());
  profile->getPropertyInterface()->setDeviceAddress(profileConfigData.m_deviceAddress);
  profile->getPropertyInterface()->setDeviceName(profileConfigData.m_deviceName.c_str());
  profile->getMappingHandler()->clearInputConfiguration();
  profile->getMappingHandler()->configureInputValues(profileConfigData.m_inputValueMapping);
  profile->getMappingHandler()->clearOutputConfiguration();
  profile->getMappingHandler()->configureOutputValues(profileConfigData.m_outputValueMapping);
  profile->setProfileScalingStrategyCfg(profileConfigData.m_scalingStrategyCfg);
  profile->setDriveNumber(profileConfigData.m_driveNumber);
  profile->setProfileCategory(profileConfigData.m_profileCategory);
  profile->getPropertyInterface()->enableChecksum(profileConfigData.m_checksumEnable);

  // Finish
  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeProfilesConfig::onMetadata(const std::string &address, const ResponseCallback &callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

comm::axisprofile::AxisProfileResult NodeProfilesConfig::extractFlatbufferData(const comm::datalayer::Variant* data, profileConfig& profileConfigData, const ResponseCallback& callback)
{
  // Check the variant type
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::FLATBUFFERS, callback))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Verify the flatbuffer
  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifyprofileConfigBuffer(verifier))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Get the flatbuffer struct
  auto variantData = fbtypes::GetprofileConfig(data->getData());

  // Check fields
  if ( (!flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_DEVICEADDRESS) &&
        !flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_DEVICENAME))     ||
       !flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_INPUTBUFFER)      ||
       !flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_OUTPUTBUFFER)     ||
       !flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_PROFILEBASICDATA) ||
       !flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_INPUTMAPPING)     ||
       !flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_OUTPUTMAPPING)    )
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Extract simple data
  profileConfigData.m_deviceAddress = variantData->deviceAddress();
  profileConfigData.m_inputBuffer   = flatbuffers::GetString(variantData->inputBuffer());
  profileConfigData.m_outputBuffer  = flatbuffers::GetString(variantData->outputBuffer());
  profileConfigData.m_deviceName    = flatbuffers::GetString(variantData->deviceName());
  profileConfigData.m_driveNumber   = variantData->driveNumber();

  // Extract basic profile flatbuffer
  auto profileBasicData = variantData->profileBasicData();
  NodeProfiles::extractFlatbufferData(profileBasicData, profileConfigData.m_profile);

  // Extract input mapping flatbuffer
  auto vecInputMapping = variantData->inputMapping();
  NodeProfilesInputMapping::extractFlatbufferData(vecInputMapping, profileConfigData.m_inputValueMapping);

  // Extract output mapping flatbuffer
  auto vecOutputMapping = variantData->outputMapping();
  NodeProfilesOutputMapping::extractFlatbufferData(vecOutputMapping, profileConfigData.m_outputValueMapping);

  // Extract scaling inforamtion
  if (flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_SCALINGINFO))
  {
    auto scalingInfo = variantData->scalingInfo();
    NodeProfilesScalingStrategyCfg::extractFlatbufferData(scalingInfo, profileConfigData.m_scalingStrategyCfg);      
  }

  // Extract profile category information
  if (flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_CATEGORY))
  {
    auto profileCategory = variantData->category();
    NodeProfilesCategory::extractFlatbufferData(profileCategory, profileConfigData.m_profileCategory);
  }

  // Extract profile checksum state
  if (flatbuffers::IsFieldPresent(variantData, fbtypes::profileConfig::VT_CHECKSUMENABLED))
  {
    profileConfigData.m_checksumEnable = variantData->checksumEnabled();
  }
    
  return AxisProfileResult::STS_OK;
}

// ---------------------------------------------------------------------------
// Private Methods
// ---------------------------------------------------------------------------

comm::axisprofile::AxisProfileResult NodeProfilesConfig::generateFlatbufferData(comm::datalayer::Variant& data, const profileConfig& profileConfigData, const ResponseCallback& /*callback*/)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  // Convert internal strings to flatbuffer strings
  auto inputBufferOffset  = builder.CreateString(profileConfigData.m_inputBuffer);
  auto outputBufferOffset = builder.CreateString(profileConfigData.m_outputBuffer);
  auto deviceNameOffset = builder.CreateString(profileConfigData.m_deviceName);
  // Build basic profile flatbuffer
  auto profileOffset = NodeProfiles::generateFlatbufferData(builder, profileConfigData.m_profile);

  // Build input mapping flatbuffer
  auto inputMappingOffset = NodeProfilesInputMapping::generateFlatbufferData(builder, profileConfigData.m_inputValueMapping);

  // Build output mapping flatbuffer
  auto outputMappingOffset = NodeProfilesOutputMapping::generateFlatbufferData(builder, profileConfigData.m_outputValueMapping);

  //build scaling information
  auto scalingInfoOffset = NodeProfilesScalingStrategyCfg::generateFlatbufferData(builder, profileConfigData.m_scalingStrategyCfg);

  // build usage mode data
  auto categoryOffset = NodeProfilesCategory::generateFlatbufferData(builder, profileConfigData.m_profileCategory);

  // Build profile config flatbuffer
  fbtypes::profileConfigBuilder profileConfigBuilder(builder);
  profileConfigBuilder.add_deviceAddress(profileConfigData.m_deviceAddress);
  profileConfigBuilder.add_deviceName(deviceNameOffset);
  profileConfigBuilder.add_inputBuffer(inputBufferOffset);
  profileConfigBuilder.add_outputBuffer(outputBufferOffset);
  profileConfigBuilder.add_profileBasicData(profileOffset);
  profileConfigBuilder.add_inputMapping(inputMappingOffset);
  profileConfigBuilder.add_outputMapping(outputMappingOffset);
  profileConfigBuilder.add_scalingInfo(scalingInfoOffset);
  profileConfigBuilder.add_driveNumber(profileConfigData.m_driveNumber);
  profileConfigBuilder.add_category(categoryOffset);
  profileConfigBuilder.add_checksumEnabled(profileConfigData.m_checksumEnable);

  // Finish
  builder.Finish(profileConfigBuilder.Finish());
  data.copyFlatbuffers(builder);

  return AxisProfileResult::STS_OK;
}
