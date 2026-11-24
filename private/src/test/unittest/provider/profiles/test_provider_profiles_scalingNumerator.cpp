/// @file  test_provider_node_profiles_scalingNominator.cpp
///
/// This source file is part of the axisprofile library and implement
/// the node for scaling numerator.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_provider_profiles_scalingNumerator.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_scalingNumerator.h"

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

REGISTER_TEST(TestProviderProfilesScalingNumerator);
REGISTER_TEST_TO_GROUP(TestProviderProfilesScalingNumerator, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesScalingNumerator, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesScalingNumerator, Provider);

HRESULT TestProviderProfilesScalingNumerator::doInit()
{
  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");
  return S_OK;
}

HRESULT TestProviderProfilesScalingNumerator::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to set/get the device address.");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnWrite();
  hResult |= testOnMetadata();

  return hResult;
}

HRESULT TestProviderProfilesScalingNumerator::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesScalingNumerator nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesScalingNumerator node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::CANOPEN_OVER_ETHERCAT, m_profileName);

  std::uint32_t scalingNumerator = 1;
  log(L"\nSet device numerator to %d.", scalingNumerator);
  IAxisProfile* profile = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str());
  if (!profile)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }
  AxisProfileScalingStrategyCfg scalingCfg{};
  scalingCfg.m_scalingStrategy = AxisProfileScalingStrategy::COE_CALC_CONTROL;
  scalingCfg.m_numerator = scalingNumerator;
  profile->setProfileScalingStrategyCfg(scalingCfg);

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData{};
  expData.setValue(scalingNumerator);
  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesScalingNumerator::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesScalingNumerator nodeNoManager(nullptr);
  comm::datalayer::Variant data{};
  data.setType(comm::datalayer::VariantType::UINT32);
  data.setValue(std::uint32_t(1));
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &nodeNoManager, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesScalingNumerator node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onWrite() with invalid variant type.");  
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite() when profile does not exist.");
  data.setType(comm::datalayer::VariantType::UINT32);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_invalidNodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  
  log(L"\nCall onWrite() when value is invalid.");
  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::CANOPEN_OVER_ETHERCAT, m_profileName);
  IAxisProfile* profile = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str());
  if (!profile)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }
  AxisProfileScalingStrategyCfg scalingCfg{};
  scalingCfg.m_scalingStrategy = AxisProfileScalingStrategy::COE_CALC_CONTROL;
  scalingCfg.m_numerator = 0;
  profile->setProfileScalingStrategyCfg(scalingCfg);

  data.setValue(std::uint32_t(0));
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data,false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onWrite() when everything is OK.");
  scalingCfg.m_numerator = 1;
  profile->setProfileScalingStrategyCfg(scalingCfg);
  data.setValue(std::uint32_t(1));
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesScalingNumerator::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onMetadata() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesScalingNumerator node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
