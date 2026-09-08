/// @file
///
/// @brief This file is part of the axisprofile library and implements the operation of scaling factor configuration.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_scalingFactor.h"
#include "manager/i_axisprofile_manager_internal.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::provider;
using namespace comm::axisprofile;

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

NodeProfilesScalingFactorCfg::NodeProfilesScalingFactorCfg(ProviderMgr* mgr): NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "scalingFactor.bfbs";
  this->m_metaDataInfo.m_description = "Standalone scaling factor for velocity, only take effect when the property 'isEnabled' set to true";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesScalingFactorCfg::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get mangr pointer
  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  std::string profileName {};
  ProviderHelper::getProfileNameFromAddress(address, profileName);

  // Get profile pointer
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(profileManager->getProfileByName(profileName.c_str()));
  if (!profile)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_PROFILE_NOT_FOUND, "profile %s not found", profileName.c_str());
    return;
  }

  ScalingFactor factorData {
    profile->getProfileScalingStrategyCfg().m_velFactor.m_enabled,
    profile->getProfileScalingStrategyCfg().m_velFactor.m_numerator,
    profile->getProfileScalingStrategyCfg().m_velFactor.m_denominator
  };

  comm::datalayer::Variant retVariant;

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto factorOffset = generateFlatbufferData(builder, factorData);
  // Finish
  builder.Finish(factorOffset);
  retVariant.copyFlatbuffers(builder);

  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesScalingFactorCfg::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Check the variant type
  ScalingFactor scalingFactor {};
  if (aprStatusFailed(extractFlatbufferData(data, scalingFactor, callback)))
  {
    return;
  }

  // Finish
  AxisProfileScalingStrategyCfg scalingCfg = profile->getProfileScalingStrategyCfg();
  scalingCfg.m_velFactor = scalingFactor;
  AxisProfileResult aprResult = profile->setProfileScalingStrategyCfg(scalingCfg);
  if (aprStatusSucceeded(aprResult))
  {
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, "set additional scaling factor failed");
  }
}

void NodeProfilesScalingFactorCfg::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

flatbuffers::Offset<comm::axisprofile::fbtypes::ScalingFactor> NodeProfilesScalingFactorCfg::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, const ScalingFactor& factorData)
{
  return fbtypes::CreateScalingFactor(fbb, factorData.m_enabled, factorData.m_numerator, factorData.m_denominator);
}

AxisProfileResult NodeProfilesScalingFactorCfg::extractFlatbufferData(const comm::datalayer::Variant* data, ScalingFactor& scalingFactorCfg, const ResponseCallback& callback)
{
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::FLATBUFFERS, callback))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Verify the flatbuffer
  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifyScalingFactorBuffer(verifier))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  //get scaling factor fbs data
  auto variantData = fbtypes::GetScalingFactor(data->getData());    
  if (aprStatusFailed(extractFlatbufferData(variantData, scalingFactorCfg)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check Field Present failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }
  
  return AxisProfileResult::STS_OK;
}

AxisProfileResult NodeProfilesScalingFactorCfg::extractFlatbufferData(const comm::axisprofile::fbtypes::ScalingFactor* data, ScalingFactor& factorCfg)
{
  //check field present
  if (!flatbuffers::IsFieldPresent(data, fbtypes::ScalingFactor::VT_ISENABLED)||
      !flatbuffers::IsFieldPresent(data, fbtypes::ScalingFactor::VT_NUMERATOR)||
      !flatbuffers::IsFieldPresent(data, fbtypes::ScalingFactor::VT_DENOMINATOR))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }
  factorCfg.m_enabled = data->isEnabled();
  factorCfg.m_numerator = data->numerator();
  factorCfg.m_denominator = data->denominator();

  return AxisProfileResult::STS_OK;
}
