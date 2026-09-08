/// @file  node_profiles_scalingStrategy.cpp
///
/// This source file is part of the axisprofile library and implement
/// the node for scaling strategy.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_scalingStrategy.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace provider;

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
NodeProfilesScalingStrategy::NodeProfilesScalingStrategy(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "profileScalingStrategy.bfbs";
  this->m_metaDataInfo.m_description = "Profile scaling strategy";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesScalingStrategy::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
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

  AxisProfileScalingStrategy profileScalingStrategy{};
  profileScalingStrategy = profile->getProfileScalingStrategyCfg().m_scalingStrategy;

  comm::datalayer::Variant retVariant;
  // Build basic profile flatbuffer
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto typeOffset = generateFlatbufferData(builder, profileScalingStrategy);
  // Finish
  builder.Finish(typeOffset);
  retVariant.copyFlatbuffers(builder);

  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesScalingStrategy::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Check the variant type
  AxisProfileScalingStrategy profileScalingStrategy{};
  if (aprStatusFailed(extractFlatbufferData(data, profileScalingStrategy, callback)))
  {
    return;
  }

  // Finish
  AxisProfileScalingStrategyCfg scalingCfg = profile->getProfileScalingStrategyCfg();
  scalingCfg.m_scalingStrategy = profileScalingStrategy;  
  AxisProfileResult aprResult = profile->setProfileScalingStrategyCfg(scalingCfg);
  if (aprStatusSucceeded(aprResult))
  {
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, "set profie scaling strategy %d failed", profileScalingStrategy);
  }
}

AxisProfileResult NodeProfilesScalingStrategy::extractFlatbufferData(const comm::datalayer::Variant* data, AxisProfileScalingStrategy& profileScalingStrategy, const ResponseCallback& callback)
{
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::FLATBUFFERS, callback))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Verify the flatbuffer
  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifyprofileScalingStrategyBuffer(verifier))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  //get fbs profile
  auto variantData = fbtypes::GetprofileScalingStrategy(data->getData());

  //check field present
  if (aprStatusFailed(extractFlatbufferData(variantData, profileScalingStrategy)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check Field Present failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult NodeProfilesScalingStrategy::extractFlatbufferData(const fbtypes::profileScalingStrategy* data, AxisProfileScalingStrategy& profileScalingStrategy)
{
  //check field present
  if (!flatbuffers::IsFieldPresent(data, fbtypes::profileScalingStrategy::VT_STRATEGY))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }
  profileScalingStrategy = stringToAxisProfileScalingStrategy(fbtypes::EnumNameScalingStrategy(data->strategy()));
  return AxisProfileResult::STS_OK;
}

void NodeProfilesScalingStrategy::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

flatbuffers::Offset<fbtypes::profileScalingStrategy> NodeProfilesScalingStrategy::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, AxisProfileScalingStrategy ScalingStrategy)
{
  //get type offset data
  fbtypes::profileScalingStrategyBuilder ScalingMethodDataBuilder(fbb);
  ScalingMethodDataBuilder.add_strategy(fbtypes::EnumValuesScalingStrategy()[static_cast<std::uint32_t>(ScalingStrategy)]);
  return ScalingMethodDataBuilder.Finish();
}

