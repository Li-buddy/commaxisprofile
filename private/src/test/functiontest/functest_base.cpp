#include "functest_base.h"
#include "provider/provider_helper.h"
#include <thread>
// Interfaces
#include "manager/i_axisprofile_manager_internal.h"
#ifndef _MSC_VER
#include "celix/utils/celix_errno.h"
#else
#include "celix_errno.h"
#endif

// Mockups
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"
#include "mockup/external_bundles/mock_system_state.h"

using namespace comm::axisprofile;


FuncTestBase::FuncTestBase()
{
}

FuncTestBase::~FuncTestBase(void)
{
  if (m_datalayerTestClient) { delete m_datalayerTestClient; }
  if (m_mockEthercatMaster)  { delete m_mockEthercatMaster; }
  if (m_persistenceHelper) { delete m_persistenceHelper; }
}

HRESULT FuncTestBase::startTestEnvironment()
{
  stopTestEnvironment();

  log(L"\n\t<GREY>Start the axisprofile system...</GREY>");

  log(L"\n\t<GREY>Start the datalayer system...</GREY>");
  m_datalayerSystem = new comm::datalayer::DatalayerSystem();
  m_datalayerSystem->start();
  m_persistenceHelper = new comm::datalayer::PersistenceHelper(m_datalayerSystem);

  log(L"\n\t<GREY>Add service dependencies to the axisprofile system...</GREY>");
  m_axisProfileComponent = new AxisProfileCmp();
  m_axisProfileComponent->addDataLayer(m_datalayerSystem->factory());
  m_axisProfileComponent->addPersistenceHelper(m_persistenceHelper->getCInterface());

  log(L"\n\t<GREY>Create and run mockup of the ethercat master bundle...</GREY>");
  m_mockEthercatMaster = new test::MockEthercatMaster(this, m_datalayerSystem->factory());
  log(L"\n\t<GREY>Create and run mockup of the scheduler bundle...</GREY>");
  m_axisProfileComponent->addScheduler(&m_mockScheduler);
  HRESULT result = m_mockEthercatMaster->init();
  RETURN_TEST_FAILED(result, L"\nInit EtherCAT master failed.");

  test::MockSystemState mockSystemState;
  m_axisProfileComponent->addSystemState(&mockSystemState);

  std::uint32_t timeoutMs = 1000;
  result = m_mockEthercatMaster->waitUntilRtAvailable(timeoutMs);
  RETURN_TEST_FAILED(result, L"\nWait realtime data available failed.");
  
  log(L"\n\t<GREY>Create a datalayer client for testing...</GREY>");
  m_datalayerTestClient = m_datalayerSystem->factory()->createClient();

  log(L"\n\t<GREY>Call AxisProfileCmp::init()...</GREY>");
  celix_status_t celixResult = m_axisProfileComponent->init();
  RETURN_TEST_RESULT(celixResult, L"\nInitial axisprofile bundle failed.", CELIX_SUCCESS);

  log(L"\n\t<GREY>Call AxisProfileCmp::start()...</GREY>");
  celixResult = m_axisProfileComponent->start();
  RETURN_TEST_RESULT(celixResult, L"\nStart axisprofile bundle failed.", CELIX_SUCCESS);

  log(L"\n\t<GREY>Create callable...</GREY>");
  std::vector<std::string> callableAdpStr({"adaption"});
  std::vector<std::string> callableDsbStr({"distribution"});
  comm::datalayer::Variant callableParam;
  callableParam.setType(comm::datalayer::VariantType::ARRAY_OF_STRING);

  callableParam.setValue(callableAdpStr);
  auto adpCallable = m_axisProfileComponent->getCallableFactory()->createCallable(callableParam);

  callableParam.setValue(callableDsbStr);
  auto dspCallable = m_axisProfileComponent->getCallableFactory()->createCallable(callableParam);

  common::scheduler::SchedEventType  eventType = ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING;
  common::scheduler::SchedEventPhase eventPhase = ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE;
  comm::datalayer::Variant param;
  adpCallable->execute(eventType, static_cast<::common::scheduler::SchedEventPhase>(eventPhase), param);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  m_axisProfileComponent->getStateMachine()->requestSwitchToState(StateIDType::CONFIG);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  log(L"\n\t<GREY>Wait until the booting process is finished...</GREY>");
  StateIDType state = StateIDType::BOOTING;
  std::uint8_t timeoutCycles = 100; // One cycle = 10ms --> Timeout = 10seconds
  while (state != StateIDType::CONFIG)
  {
    state = m_axisProfileComponent->getStateMachine()->getState();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeoutCycles--;
    if (timeoutCycles == 0)
    {
      break;
    }
  }

  if (timeoutCycles == 0)
  {
    // Note: This could happen because of another jenkins build, running at the same time. The datalayer RT is using interprocess
    // communication. If the datalayer is currently online in another process that blocks the needed ports, the realtime memory could
    // not be found in this test. Therefore AxisprofileManager::initMemory() stuck in an infinite loop.
    m_axisProfileComponent->requestAxisProfileState("Configuration");
    error(L"\nTimeout in AxisProfileManager initialization! Aborting...");
    return E_FAIL;
  }

  log(L"\n\t<GREY>AxisProfile system started successfully...</GREY>");
  m_systemRunning = true;

  return S_OK;
}

HRESULT FuncTestBase::stopTestEnvironment()
{
  if (m_axisProfileComponent)
  {
    log(L"\n\t<GREY>Switch axisprofile system to CONFIG state...</GREY>");
    auto result = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
    RETURN_TEST_FAILED(result, L"\nSwitch CONFIG state failed");

    log(L"\n\t<GREY>Stop axisprofile component...</GREY>");
    celix_status_t celixResult = m_axisProfileComponent->stop();
    RETURN_TEST_RESULT(celixResult, L"\nStop axisprofile bundle failed.", CELIX_SUCCESS);

    log(L"\n\t<GREY>Deinit axisprofile component...</GREY>");
    celixResult = m_axisProfileComponent->deinit();
    RETURN_TEST_RESULT(celixResult, L"\nDeinit axisprofile bundle failed.", CELIX_SUCCESS);

    log(L"\n\t<GREY>Remove services...</GREY>");
    m_axisProfileComponent->removeDatalayer(nullptr);
    m_axisProfileComponent->removePersistenceHelper(nullptr);
    delete m_axisProfileComponent;
    m_axisProfileComponent = nullptr;
  }

  log(L"\n\t<GREY>Stop datalayer component...</GREY>");
  if (m_datalayerTestClient)
  {
    delete m_datalayerTestClient;
    m_datalayerTestClient = nullptr;
  }
  if (m_mockEthercatMaster)
  {
    delete m_mockEthercatMaster;
    m_mockEthercatMaster = nullptr;
  }
  
  if (m_datalayerSystem)
  {
    m_datalayerSystem->stop();
    delete m_datalayerSystem;
    m_datalayerSystem = nullptr;
  }

  log(L"\n\t<GREY>Axisprofile component has shutdown...</GREY>");
  m_systemRunning = false;

  return S_OK;
}

HRESULT FuncTestBase::executeSingleCycle()
{
  std::vector<std::string> profileNames{};
  m_axisProfileComponent->getAxisProfileManager()->getProfileNames(profileNames);

  HRESULT res = m_mockEthercatMaster->copyMdtToAt(static_cast<std::uint8_t>(profileNames.size()));
  RETURN_TEST_FAILED(res, L"\nEtherCAT wait realtime data available failed.");

  comm::axisprofile::AxisProfileResult aprResult = m_axisProfileComponent->getAxisProfileManager()->processDataAt();
  RETURN_TEST_FAILED(aprResult, L"\nProfile process AT data failed.");

  res = m_mockEthercatMaster->copyAtToMdt(static_cast<std::uint8_t>(profileNames.size()));
  RETURN_TEST_FAILED(res, L"\nEtherCAT copy At to Mdt failed.");

  aprResult = m_axisProfileComponent->getAxisProfileManager()->processDataMdt();
  RETURN_TEST_FAILED(aprResult, L"\nProfile process MDT failed.");

  return S_OK;
}

HRESULT FuncTestBase::addProfileToSystem(const comm::axisprofile::AxisProfileType type, const std::uint32_t deviceAddress, const std::string& profileName, comm::axisprofile::IAxisProfileInternal** axisProfile)
{
  RETURN_TEST_RESULT_ANY(!profileName.empty(), L"\nCould not add profile to the system. Invalid argument!");
  RETURN_TEST_RESULT_S(m_systemRunning, L"\nAxisprofile system is not running!", true);

  if (StateIDType::CONFIG != m_axisProfileComponent->getStateMachine()->getState())
  {
    error(L"\nSystem is not in state 'Configuration'! Could not add new axisprofile.");
    return E_FAIL;
  }

  log(L"\n\t<GREY>Add new axisprofile to the system. Name: %ls, Type: %ls, Addr: %d </GREY>", UTF8TOWIDE(profileName), UTF8TOWIDE(axisProfileTypeToString(type)), deviceAddress);

  AxisProfileResult aprResult = m_axisProfileComponent->getAxisProfileManager()->createAxisProfile(type, profileName);
  RETURN_TEST_FAILED(aprResult, L"\nAdd new axisprofile failed.");

  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(m_axisProfileComponent->getAxisProfileManager()->getProfileByName(profileName.c_str()));
  RETURN_TEST_RESULT_SET_PTR(profile, L"\nThe AxisProfileManager could not find the new profile!");

  aprResult = profile->setDeviceAddress(deviceAddress);
  RETURN_TEST_FAILED(aprResult, L"\nSet device address failed.");

  updateProtocolRelatedMockData(type);

  if (axisProfile)
  {
    *axisProfile = profile;
  }

  return S_OK;
}

void FuncTestBase::updateProtocolRelatedMockData(const comm::axisprofile::AxisProfileType type)
{
  std::uint32_t protocol {};
  switch (type)
  {
    case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
      protocol = 26;
      break;
    case AxisProfileType::CANOPEN_OVER_ETHERCAT:
      protocol = 6;
      break;
    default:
      break;
  }
  m_mockEthercatMaster->m_ecatSlaveCfgInfo.m_devInfo.m_supportedProtocol = protocol;
}

HRESULT FuncTestBase::connectProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& axisProfile)
{
  RETURN_TEST_RESULT_ANY(!profileName.empty(), L"\nCould not add profile to the system. Invalid argument!");
  RETURN_TEST_RESULT_S(m_systemRunning, L"\nAxisprofile system is not running!", true);
  if (StateIDType::CONFIG != m_axisProfileComponent->getStateMachine()->getState())
  {
    error(L"\nSystem is not in state 'Configuration'! Could not connect the axisprofile.");
    return E_FAIL;
  }

  const IAxisProfile* profile = m_axisProfileComponent->getAxisProfileManager()->getProfileByName(profileName.c_str());
  RETURN_TEST_RESULT_SET_PTR(profile, L"\nThe AxisProfileManager could not find the new profile!");

  log(L"\n\t<GREY>Connect existing axisprofile. Name: %ls</GREY>", UTF8TOWIDE(profileName));

  AxisProfileResult aprResult = m_axisProfileComponent->getAxisProfileManager()->connectAxisProfile(profileName, axisProfile);
  RETURN_TEST_FAILED(aprResult, L"\nConnect axisprofile failed.");
  return S_OK;
}

HRESULT FuncTestBase::switchSystemStatemachine(comm::axisprofile::StateIDType cmdState, comm::axisprofile::StateIDType targetState)
{
  RETURN_TEST_RESULT_S(m_systemRunning, L"\nAxisprofile system is not running!", true);

  IStateMachine* stateMachine = m_axisProfileComponent->getStateMachine();
  if (targetState != stateMachine->getState())
  {
    // We need to use the datalayer node here, because a direct statemachine function call would skip some logic which can result in different errors...
    log(L"\n\t<GREY>Switch system statemachine to state '%ls'.</GREY>", UTF8TOWIDE(stateMachine->getStateName(targetState)));
    comm::datalayer::Variant data{};
    std::string cmdStateString(stateTypeToString(cmdState));
    data.setValue(cmdStateString);
    comm::datalayer::DlResult dlResult = m_datalayerTestClient->createSync(provider::NodeTree.APR_SYSTEM_OPSTATE_CMD, &data);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      error(L"\nRequest new system state failed! Result: %ls", UTF8TOWIDE(dlResult.toString()));
      return E_FAIL;
    }

    StateIDType state = StateIDType::ERROR_STATE;
    std::uint8_t timeoutCycles = 150; // One cycle = 100ms --> Timeout = 15.5seconds
    while (state != targetState)
    {
      state = stateMachine->getState();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      timeoutCycles--;
      if (timeoutCycles == 0)
      {
        break;
      }
    }
    if (timeoutCycles == 0)
    {
      error(L"\nCould not switch system state to '%ls'! Current state: %ls", UTF8TOWIDE(stateMachine->getStateName(targetState)), UTF8TOWIDE(stateMachine->getStateName(stateMachine->getState())));
      return E_FAIL;
    }
  }

  return S_OK;
}

HRESULT FuncTestBase::waitAndChecker(std::function<AxisProfileResult(void)> invoker, AxisProfileResult expRet, std::uint32_t waitMs, uint32_t timeoutMs)
{
  std::uint32_t cnt = timeoutMs / waitMs;

  while (cnt > 0)
  {
    auto retValue = invoker();
    if (retValue == expRet)
    {
      log(L"<DARK_GREEN><b>success.</b></DARK_GREEN>");
      return S_OK;
    }
    else if (aprStatusFailed(retValue))
    {
      RETURN_TEST_RESULT(retValue, L"\nUnexpected return value.", expRet);
      return E_FAIL;
    }

    cnt--;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  error(L"\n%ls:%d Timeout.", LITERALWIDECHAR(__FILE__), __LINE__);
  return S_OK;
}
