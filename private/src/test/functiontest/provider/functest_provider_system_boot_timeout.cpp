/// @file                functest_provider_system_boot_timeout.cpp
///
/// This source file is part of the axisprofile library and implements the function test code of datalayer max booting timeout
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <thread>
#include "functest_provider_system_boot_timeout.h"
#include "provider/provider_helper.h"
#include "helper/provider_helper_functest.h"
#include "manager/axisprofile_manager.h"
#include "configuration/boot_mgr.h"

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
REGISTER_TEST(FuncTestProviderBootTimeout);
REGISTER_TEST_TO_GROUP(FuncTestProviderBootTimeout, 1_AutoTest);
REGISTER_TEST_TO_GROUP(FuncTestProviderBootTimeout, 3_FuncTest);
REGISTER_TEST_TO_GROUP(FuncTestProviderBootTimeout, Provider);

// Constructor/Destructor
FuncTestProviderBootTimeout::FuncTestProviderBootTimeout()
{}


FuncTestProviderBootTimeout::~FuncTestProviderBootTimeout(void)
{}

HRESULT FuncTestProviderBootTimeout::doInit()
{
  return startTestEnvironment();
}

HRESULT FuncTestProviderBootTimeout::doEnd()
{
  return stopTestEnvironment();
}

HRESULT FuncTestProviderBootTimeout::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile boot timeout datalayer node.");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= testReadBootTimeoutNodes();

  return hResult;
}

HRESULT FuncTestProviderBootTimeout::testReadBootTimeoutNodes()
{
  HRESULT hResult = S_OK;

  log(L"\n<b>Test to read the boot timeout nodes.");

  std::string urlRead = comm::axisprofile::provider::NodeTree.APR_SYSTEM_BOOT_TIMEOUT;
  comm::datalayer::Variant actData;
  comm::datalayer::Variant expData;
  expData.setType(comm::datalayer::VariantType::FLOAT64);
  expData.setValue(MILLISECOND_2_SECOND(static_cast<double>(TIMEOUT_SCHED_IN_RUN_STATE_MS + TIMEOUT_FIELDBUS_DEVICE_OP_MS + TIMEOUT_WAIT_BEFORE_DEVICE_OP)) + 1);
  PROVIDER_CHECK(test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, urlRead.c_str(), &actData, true, &expData));

  log(L"\n");
  return hResult;
}

