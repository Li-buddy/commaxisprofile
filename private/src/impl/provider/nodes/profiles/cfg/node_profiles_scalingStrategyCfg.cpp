/// @file  node_profiles_scalingStrategyCfg.cpp
///
/// This source file is part of the axisprofile library and implement
/// the node for scaling strategy configuration.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_scalingStrategyCfg.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

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

NodeProfilesScalingStrategyCfg::NodeProfilesScalingStrategyCfg(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "profileScalingCfg.bfbs";
  this->m_metaDataInfo.m_description = "Profile scaling information";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesScalingStrategyCfg::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
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

  comm::axisprofile::AxisProfileScalingStrategyCfg profileScalingStrategyCfg{};
  profileScalingStrategyCfg = profile->getProfileScalingStrategyCfg();

  comm::datalayer::Variant retVariant;
  // Build basic profile flatbuffer
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto typeOffset = generateFlatbufferData(builder, profileScalingStrategyCfg);
  // Finish
  builder.Finish(typeOffset);
  retVariant.copyFlatbuffers(builder);

  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesScalingStrategyCfg::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile
  comm::axisprofile::IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Check the variant type
  comm::axisprofile::AxisProfileScalingStrategyCfg profileScalingStrategyCfg{};
  if (aprStatusFailed(extractFlatbufferData(data, profileScalingStrategyCfg, callback)))
  {
    return;
  }

  // Finish
  // if strategy is equal to before
  AxisProfileResult aprResult = profile->setProfileScalingStrategyCfg(profileScalingStrategyCfg);
  if (aprStatusSucceeded(aprResult))
  {
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE, "set profie scaling strategy %d failed", profileScalingStrategyCfg.m_scalingStrategy);
  }
}

comm::axisprofile::AxisProfileResult NodeProfilesScalingStrategyCfg::extractFlatbufferData(const comm::datalayer::Variant* data, comm::axisprofile::AxisProfileScalingStrategyCfg& profileScalingCfg, const ResponseCallback& callback)
{
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::FLATBUFFERS, callback))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Verify the flatbuffer
  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifyprofileScalingCfgBuffer(verifier))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  //get fbs profile
  auto variantData = fbtypes::GetprofileScalingCfg(data->getData());

  //check field present
  if (aprStatusFailed(extractFlatbufferData(variantData, profileScalingCfg)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check Field Present failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }
  //0 is invalid value
  if (0 == variantData->resolution())
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE, "set m_resolution %d failed", variantData->resolution());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  return AxisProfileResult::STS_OK;
}

comm::axisprofile::AxisProfileResult NodeProfilesScalingStrategyCfg::extractFlatbufferData(const comm::axisprofile::fbtypes::profileScalingCfg* data, comm::axisprofile::AxisProfileScalingStrategyCfg& profileScalingCfg)
{
  //check field present
  if (!flatbuffers::IsFieldPresent(data, fbtypes::profileScalingCfg::VT_SCALINGSTRATEGY) ||
    !flatbuffers::IsFieldPresent(data, fbtypes::profileScalingCfg::VT_SCALINGTYPE) ||
    !flatbuffers::IsFieldPresent(data, fbtypes::profileScalingCfg::VT_NUMERATOR) ||
    !flatbuffers::IsFieldPresent(data, fbtypes::profileScalingCfg::VT_DENOMINATOR) ||
    !flatbuffers::IsFieldPresent(data, fbtypes::profileScalingCfg::VT_RESOLUTION))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }
  auto ScalingStrategy = data->scalingStrategy();
  NodeProfilesScalingStrategy::extractFlatbufferData(ScalingStrategy, profileScalingCfg.m_scalingStrategy);
  auto scalingType = data->scalingType();
  NodeProfilesDeviceScalingType::extractFlatbufferData(scalingType, profileScalingCfg.m_deviceScalingType);
  profileScalingCfg.m_denominator = data->denominator();
  profileScalingCfg.m_numerator = data->numerator();
  profileScalingCfg.m_resolution = data->resolution();
  if (flatbuffers::IsFieldPresent(data, fbtypes::profileScalingCfg::VT_VELOCITYFACTOR))
  {
    auto factorCfg = data->velocityFactor();
    NodeProfilesScalingFactorCfg::extractFlatbufferData(factorCfg, profileScalingCfg.m_velFactor);
  }
  return AxisProfileResult::STS_OK;
}

void NodeProfilesScalingStrategyCfg::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

flatbuffers::Offset<comm::axisprofile::fbtypes::profileScalingCfg> NodeProfilesScalingStrategyCfg::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, const comm::axisprofile::AxisProfileScalingStrategyCfg& scalingStrategyCfg)
{
  auto scalingMethodOffset = NodeProfilesScalingStrategy::generateFlatbufferData(fbb, scalingStrategyCfg.m_scalingStrategy);
  auto scalingTypeOffset = NodeProfilesDeviceScalingType::generateFlatbufferData(fbb, scalingStrategyCfg.m_deviceScalingType);
  auto factorOffset = NodeProfilesScalingFactorCfg::generateFlatbufferData(fbb, scalingStrategyCfg.m_velFactor);
  
  fbtypes::profileScalingCfgBuilder ScalingCfgDataBuilder(fbb);
  //construct scaling method data
  ScalingCfgDataBuilder.add_scalingStrategy(scalingMethodOffset);
  ScalingCfgDataBuilder.add_scalingType(scalingTypeOffset);
  ScalingCfgDataBuilder.add_denominator(scalingStrategyCfg.m_denominator);
  ScalingCfgDataBuilder.add_numerator(scalingStrategyCfg.m_numerator);
  ScalingCfgDataBuilder.add_resolution(scalingStrategyCfg.m_resolution);
  ScalingCfgDataBuilder.add_velocityFactor(factorOffset);

  return ScalingCfgDataBuilder.Finish();
}
