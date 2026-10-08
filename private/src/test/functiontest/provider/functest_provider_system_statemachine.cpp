#include <thread>
#include "functest_provider_system_statemachine.h"
#include "provider/provider_helper.h"
#include "helper/provider_helper_functest.h"
#include "manager/axisprofile_manager.h"

// Mockups (External Bundles)
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"


using namespace comm::axisprofile;

REGISTER_TEST(FuncTestProviderSystemStatemachine);
REGISTER_TEST_TO_GROUP(FuncTestProviderSystemStatemachine, 1_AutoTest);
REGISTER_TEST_TO_GROUP(FuncTestProviderSystemStatemachine, 3_FuncTest);
REGISTER_TEST_TO_GROUP(FuncTestProviderSystemStatemachine, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
FuncTestProviderSystemStatemachine::FuncTestProviderSystemStatemachine()
{
}


FuncTestProviderSystemStatemachine::~FuncTestProviderSystemStatemachine(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT FuncTestProviderSystemStatemachine::doInit()
{
  return startTestEnvironment();
}

HRESULT FuncTestProviderSystemStatemachine::doEnd()
{
  return stopTestEnvironment();
}

HRESULT FuncTestProviderSystemStatemachine::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile statemachine datalayer node.");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= testReadStatemachineNodes();
  hResult |= testTransitionToRunning();

  return hResult;
}

HRESULT FuncTestProviderSystemStatemachine::testReadStatemachineNodes()
{
  log(L"\n<b>Test to read the statemachine nodes.");

  HRESULT hResult = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  if (FAILED(hResult))
  {
    return hResult;
  }

  log(L"\nTry to read the actual state via datalayer.");
  std::string urlRead = comm::axisprofile::provider::NodeTree.APR_SYSTEM_OPSTATE_ACTUAL;
  comm::datalayer::Variant actData;
  comm::datalayer::Variant expData;
  expData.setType(comm::datalayer::VariantType::STRING);
  expData.setValue("Configuration");
  PROVIDER_CHECK(test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, urlRead.c_str(), &actData, true, &expData));

  log(L"\nRead all system state strings via datalayer.");
  std::string urlBrowse = comm::axisprofile::provider::NodeTree.APR_SYSTEM_OPSTATE_CMD;
  std::vector<std::string> expStates = { "Booting", "Configuration" };
  expData.setValue(expStates);
  PROVIDER_CHECK(test::ProviderHelperFunctest::browseNodeSync(this, m_datalayerTestClient, urlBrowse.c_str(), &actData, true, &expData));

  log(L"\n");
  return hResult;
}

HRESULT FuncTestProviderSystemStatemachine::testTransitionToRunning()
{
  log(L"\n<b>Test different cases when switching the statemachine to 'Running'.");

  HRESULT hResult = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  if (FAILED(hResult))
  {
    return hResult;
  }

  log(L"\nDeinitialize EthercatMaster mockup for next test case.");
  hResult = m_mockEthercatMaster->deInit();
  if (FAILED(hResult))
  {
    return hResult;
  }

  log(L"\n\nTry state switch without EthercatMaster instance and without any profile (should succeed).");
  hResult = checkTransition(StateIDType::BOOTING, StateIDType::RUNNING, 2000);
  if (FAILED(hResult))
  {
    return hResult;
  }

  const std::string profileName = "testProfile";
  IAxisProfileInternal* testProfile{};
  hResult = addProfileToSystem(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, 1, profileName.c_str(), &testProfile);
  if (FAILED(hResult))
  {
    return hResult;
  }
  m_axisProfileComponent->getAxisProfileManager()->setProfileInputBuffer(profileName.c_str(), MOCK_ECAT_INPUTBUFFER_STRING);
  m_axisProfileComponent->getAxisProfileManager()->setProfileOutputBuffer(profileName.c_str(), MOCK_ECAT_OUTPUTBUFFER_STRING);

  testProfile->getPropertyInterface()->setEtherCATName(MOCK_ECAT_MASTER_INSTANCE_NAME);
  testProfile->getPropertyInterface()->setDeviceName(MOCK_ECAT_SLAVE_DEVICE_NAME);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::STATUSWORD, MOCK_ECAT_INBUFFER_STATUSWORD);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::POSITION, MOCK_ECAT_INBUFFER_POSITION);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::CONTROLWORD, MOCK_ECAT_OUTBUFFER_CMDWORD);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::POSITION, MOCK_ECAT_OUTBUFFER_POSITION);

  log(L"\n\nTry state switch without EthercatMaster instance but with profile (should fail).");
  hResult = checkTransition(StateIDType::BOOTING, StateIDType::BOOTING, 2000);
  if (FAILED(hResult))
  {
    return hResult;
  }

  log(L"\nInitialize EthercatMaster mockup for next test case.");
  hResult = m_mockEthercatMaster->init();
  if (FAILED(hResult))
  {
    return hResult;
  }
  hResult = m_mockEthercatMaster->waitUntilRtAvailable(10000);
  if (FAILED(hResult))
  {
    return hResult;
  }

  log(L"\nTry state switch with EthercatMaster instance and with profile (should succeed).");
  hResult = checkTransition(StateIDType::BOOTING, StateIDType::RUNNING, 5000);
  if (FAILED(hResult))
  {
    return hResult;
  }

  log(L"\n");
  return hResult;
}

HRESULT FuncTestProviderSystemStatemachine::checkTransition(const comm::axisprofile::StateIDType cmdState, const comm::axisprofile::StateIDType expState,
                                                            const std::uint32_t timeoutMilliseconds, const bool backToConfig)
{
  HRESULT hResult = S_OK;

  log(L"\nRequest switch system state to '%ls'.", UTF8TOWIDE(stateTypeToString(cmdState)));
  std::string urlWrite = comm::axisprofile::provider::NodeTree.APR_SYSTEM_OPSTATE_CMD;
  comm::datalayer::Variant writeData;
  writeData.setValue(stateTypeToString(cmdState));
  PROVIDER_CHECK(test::ProviderHelperFunctest::createNodeSync(this, m_datalayerTestClient, urlWrite.c_str(), &writeData, true));

  log(L"\nTry to read the actual state via datalayer.");
  StateIDType state = StateIDType::ERROR_STATE;
  std::uint8_t timeoutCycles = timeoutMilliseconds / 10; // One cycle = 10ms
  while (state != expState)
  {
    state = m_axisProfileComponent->getStateMachine()->getState();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeoutCycles--;
    if (timeoutCycles == 0)
    {
      break;
    }
  }

  std::string urlRead = comm::axisprofile::provider::NodeTree.APR_SYSTEM_OPSTATE_ACTUAL;
  comm::datalayer::Variant actData;
  comm::datalayer::Variant expData;
  expData.setValue(stateTypeToString(expState));
  PROVIDER_CHECK(test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, urlRead.c_str(), &actData, true, &expData));

  if (backToConfig)
  {
    hResult = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  }

  return hResult;
}
