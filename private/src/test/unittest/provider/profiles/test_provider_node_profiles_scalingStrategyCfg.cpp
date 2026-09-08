/// @file  test_provider_node_profiles_scalingstrategyCfg.cpp
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
#include "test_provider_node_profiles_scalingStrategyCfg.h"
#include "helper/provider_helper_unittest.h"
#include "comm/axisprofile/datatypes/profileScalingCfg_generated.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_scalingStrategyCfg.h"
#include "provider/nodes/profiles/cfg/node_profiles_deviceScalingType.h"
#include "provider/nodes/profiles/cfg/node_profiles_scalingStrategy.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
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

REGISTER_TEST(TestProviderNodeProfilesScalingStrategyCfg);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesScalingStrategyCfg, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesScalingStrategyCfg, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesScalingStrategyCfg, Provider);

HRESULT TestProviderNodeProfilesScalingStrategyCfg::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  return S_OK;
}

HRESULT TestProviderNodeProfilesScalingStrategyCfg::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the general datalayer node of all existing axis profiles. ");
  log(L"\n----------------------------------------------------------------------\n");

  
  hResult |= testOnRead();
  hResult |= testOnMetadata();
  hResult |= testOnWrite();

  return hResult;
}

HRESULT TestProviderNodeProfilesScalingStrategyCfg::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesScalingStrategyCfg nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesScalingStrategyCfg node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  IAxisProfile* profile = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str());
  if (!profile)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData{};
  comm::axisprofile::AxisProfileScalingStrategyCfg profileScalingStrategyCfg{};
  profileScalingStrategyCfg = profile->getProfileScalingStrategyCfg();

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto strategyCfgOffset = node.generateFlatbufferData(builder, profileScalingStrategyCfg);

  builder.Finish(strategyCfgOffset);
  expData.copyFlatbuffers(builder);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData, fbtypes::VerifyprofileScalingCfgBuffer));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeProfilesScalingStrategyCfg::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesScalingStrategyCfg node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeProfilesScalingStrategyCfg::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");
  test::MockAxisProfileCmp mockAxisProfileCmp{};

  log(L"\nCall onRead() when provider manager does not exist.");
  provider::NodeProfilesScalingStrategyCfg nodeNullptr(nullptr);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &nodeNullptr, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  provider::NodeProfilesScalingStrategyCfg node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onWrite() when profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile for further testing.");
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);
  log(L"\nCall onWrite() with invalid variant type.");
  comm::datalayer::Variant data{};
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite with flatbuffer that has a missing field (isFieldPresent fails).");
  const IAxisProfile* profile = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str());
  if (!profile)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }

  log(L"\nCall onWrite() when everything is OK.");  
  data = createFBData();
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\nCall onWrite() when everything is OK. without special scalig factor");
  data = createFBData(false, true, AxisProfileScalingStrategy::COE_CALC_CONTROL);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\nCall onWrite() when everything is OK. with COE_CALC_CONTROL strategy");
  data = createFBData(false, false, AxisProfileScalingStrategy::COE_CALC_CONTROL);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\nCall onWrite() when everything is OK. with DEVICE_IO strategy");
  data = createFBData(false, false, AxisProfileScalingStrategy::DEVICE_IO);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\nCall onWrite() when everything is OK. with COE_CALC_DRIVE strategy");
  data = createFBData(false, false, AxisProfileScalingStrategy::COE_CALC_DRIVE);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\n");
  return hResult;
}

comm::datalayer::Variant TestProviderNodeProfilesScalingStrategyCfg::createFBData(bool bLeaveFieldEmpty, bool bIsScalingFactor, AxisProfileScalingStrategy strategy)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  //create profile data
  auto scalingMethodOffset = comm::axisprofile::provider::NodeProfilesScalingStrategy::generateFlatbufferData(builder, strategy);
  auto scalingTypeOffset = comm::axisprofile::provider::NodeProfilesDeviceScalingType::generateFlatbufferData(builder, AxisProfileDeviceScalingType::LINEAR);
  auto scalingFactorOffset = comm::axisprofile::provider::NodeProfilesScalingFactorCfg::generateFlatbufferData(builder, {true,1,1});
  //get type offset data
  fbtypes::profileScalingCfgBuilder ScalingCfgDataBuilder(builder);
  //construct scaling method data
  if (!bLeaveFieldEmpty)
  {
    ScalingCfgDataBuilder.add_scalingStrategy(scalingMethodOffset);
    ScalingCfgDataBuilder.add_scalingType(scalingTypeOffset);
    ScalingCfgDataBuilder.add_denominator(10000);
    ScalingCfgDataBuilder.add_numerator(8388608);
    ScalingCfgDataBuilder.add_resolution(8388608);
    if (!bIsScalingFactor)
    {
      ScalingCfgDataBuilder.add_velocityFactor(scalingFactorOffset);
    }
  }

  builder.Finish(ScalingCfgDataBuilder.Finish());
  //get variant data
  comm::datalayer::Variant variantData;
  variantData.copyFlatbuffers(builder);
  return variantData;
}
