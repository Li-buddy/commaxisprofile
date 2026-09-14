/// @file                test_provider_system_boot_timeout.cpp
///
/// This source file is part of the axisprofile library and implements the test code of datalayer boot max timeout
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_provider_system_boot_timeout.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/system/node_system_boot_timeout.h"

#include "comm/axisprofile/datatypes/aprBootState_generated.h"


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
REGISTER_TEST(TestProviderBootTimeout);
REGISTER_TEST_TO_GROUP(TestProviderBootTimeout, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderBootTimeout, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderBootTimeout, Provider);

// Constructor/Destructor
TestProviderBootTimeout::TestProviderBootTimeout()
{}


TestProviderBootTimeout::~TestProviderBootTimeout(void)
{}

HRESULT TestProviderBootTimeout::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile boot timeout provider");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= testOnRead();
  hResult |= testOnMetadata();

  return hResult;
}

HRESULT TestProviderBootTimeout::testOnRead()
{
  HRESULT hResult = S_OK;

  log(L"\n<b>Test error cases for the onRead node.");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockComponent;
  provider::NodeSystemBootTimeout node(mockComponent.getProviderManager());

  log(L"\nCreate profile for testing.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  log(L"\n<b>Test the onMetadata method");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, false, comm::datalayer::DlResult::DL_OK));

  log(L"\nCall onRead() when profile does not exist.");
  comm::datalayer::Variant data;
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &data, true, comm::datalayer::DlResult::DL_OK, &data, fbtypes::VerifyAPRBootStateBuffer));

  mockComponent.setBootManager(false, mockComponent.getBootManager());
  log(L"\nCall onRead() with invalid variant type.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TIMEOUT));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderBootTimeout::testOnMetadata()
{
  HRESULT hResult = S_OK;

  log(L"\n<b>Test the onMetadata method");

  test::MockAxisProfileCmp testProfileCmp;
  comm::axisprofile::provider::NodeSystemBootTimeout nodeBootTimeout(testProfileCmp.getProviderManager());

  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &nodeBootTimeout, m_nodeAddress, true, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}

