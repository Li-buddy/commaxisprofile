#include "test_provider_system_statemachine.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/system/node_system_act_statemachine.h"
#include "provider/nodes/system/node_system_cmd_statemachine.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderSystemStatemachine);
REGISTER_TEST_TO_GROUP(TestProviderSystemStatemachine, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderSystemStatemachine, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderSystemStatemachine, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderSystemStatemachine::TestProviderSystemStatemachine()
{
}


TestProviderSystemStatemachine::~TestProviderSystemStatemachine(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderSystemStatemachine::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile statemachine provider");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= testNodeMethodsCmdCreateFail();
  hResult |= testNodeMethodsCmdBrowseFail();
  hResult |= testNodeMethodsActReadFail();
  hResult |= testNodeMethodsOnMetadata();

  return hResult;
}

HRESULT TestProviderSystemStatemachine::testNodeMethodsCmdCreateFail()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test error cases for the onCreate node.");

  test::MockAxisProfileCmp testProfileCmp;
  comm::axisprofile::provider::NodeSystemCmdStateMachine nodeCmdStatemachine(testProfileCmp.getProviderManager());

  comm::datalayer::Variant data;
  data.setType(comm::datalayer::VariantType::FLOAT64);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onCreate(this, &nodeCmdStatemachine, m_nodeAddressCmd, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  data.setType(comm::datalayer::VariantType::STRING);
  data.setValue("Booting");
  testProfileCmp.setReturnValue(comm::axisprofile::AxisProfileResult::ERR_NOK);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onCreate(this, &nodeCmdStatemachine, m_nodeAddressCmd, &data, false, comm::datalayer::DlResult::DL_SUBMODULE_FAILURE));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderSystemStatemachine::testNodeMethodsCmdBrowseFail()
{
  HRESULT hResult = S_OK;

  log(L"\n<b>Test error cases for the onBrowse cmd node (always expect DL_OK with empty state list).");

  test::MockAxisProfileCmp testProfileCmp;
  comm::axisprofile::provider::NodeSystemCmdStateMachine nodeCmdStatemachine(testProfileCmp.getProviderManager());
  testProfileCmp.setReturnValue(comm::axisprofile::AxisProfileResult::ERR_NOK);

  std::vector<std::string> emptyStateNames {"Booting", "Configuration"};
  comm::datalayer::Variant expData;
  expData.setValue(emptyStateNames);

  PROVIDER_CHECK(test::ProviderHelperUnittest::onBrowse(this, &nodeCmdStatemachine, m_nodeAddressCmd, true, &expData, comm::datalayer::DlResult::DL_OK));
  PROVIDER_CHECK(test::ProviderHelperUnittest::onBrowse(this, &nodeCmdStatemachine, m_nodeAddressCmd + "/", true, &expData, comm::datalayer::DlResult::DL_OK));
  PROVIDER_CHECK(test::ProviderHelperUnittest::onBrowse(this, &nodeCmdStatemachine, m_nodeAddressCmd + "/*", true, &expData, comm::datalayer::DlResult::DL_OK));

  emptyStateNames.clear();
  expData.setValue(emptyStateNames);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onBrowse(this, &nodeCmdStatemachine, m_nodeAddressCmd + "/Booting", true, &expData, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderSystemStatemachine::testNodeMethodsActReadFail()
{
  HRESULT hResult = S_OK;

  log(L"\n<b>Test error cases for the onRead node.");

  test::MockAxisProfileCmp testProfileCmp;
  comm::axisprofile::provider::NodeSystemActStateMachine nodeActStatemachine(testProfileCmp.getProviderManager());
  testProfileCmp.setStatemachine(false);
  testProfileCmp.setReturnValue(comm::axisprofile::AxisProfileResult::ERR_NOK);

  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeActStatemachine, m_nodeAddressAct, nullptr, false, comm::datalayer::DlResult::DL_INVALID_OPERATION_MODE));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderSystemStatemachine::testNodeMethodsOnMetadata()
{
  HRESULT hResult = S_OK;

  log(L"\n<b>Test the onMetadata method");

  test::MockAxisProfileCmp testProfileCmp;
  comm::axisprofile::provider::NodeSystemActStateMachine nodeActStatemachine(testProfileCmp.getProviderManager());
  comm::axisprofile::provider::NodeSystemCmdStateMachine nodeCmdStatemachine(testProfileCmp.getProviderManager());

  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &nodeActStatemachine, m_nodeAddressAct, true, true, comm::datalayer::DlResult::DL_OK));
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &nodeCmdStatemachine, m_nodeAddressCmd, true, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
