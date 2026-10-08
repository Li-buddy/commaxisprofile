/// @file  node_profiles_scalingUint.cpp
///
/// This source file is part of the axisprofile library and implement
/// the node for uint32_t scaling  member.
///
/// @copyright (C) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_scalingUint.h"
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

NodeProfilesScalingUint::NodeProfilesScalingUint(ProviderMgr* mgr, ScalingUintMember Member, const MetaDataInfo& metaDataInfo): NodeProfilesBase(mgr), m_fieldPtr(Member)
{
  this->m_metaDataInfo = metaDataInfo;
  m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesScalingUint::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  comm::axisprofile::IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Finish
  auto cfg = profile->getProfileScalingStrategyCfg();
  std::uint32_t scalingValue = cfg.*m_fieldPtr;
  if (0 == scalingValue)
  {
    return;
  }
  comm::datalayer::Variant returnVal;
  returnVal.setValue(scalingValue);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeProfilesScalingUint::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Check and extract the received variant
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::UINT32, callback))
  {
    return;
  }

  const std::uint32_t scalingValue = *(reinterpret_cast<std::uint32_t*>(data->getData()));
  if (0 == scalingValue)
  {
    return;
  }
  // Get profile
  comm::axisprofile::IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Finish
  AxisProfileScalingStrategyCfg scalingCfg = profile->getProfileScalingStrategyCfg();
  scalingCfg.*m_fieldPtr = scalingValue;
  profile->setProfileScalingStrategyCfg(scalingCfg);
  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeProfilesScalingUint::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  std::string name;
  ProviderHelper::getProfileNameFromAddress(address, name);

  if (!name.empty())
  {
    // The address string contains a profile name. Check if it exists in the system.
    if (!getProfileByAddress(address, callback))
    {
      return;
    }
  }

  std::string typeString{};
  if (!createBasicTypeString(name, comm::datalayer::VariantType::UINT32, typeString, callback))
  {
    return;
  }
  ProviderHelper::createMetaData(typeString, m_metaDataInfo, callback);
}
