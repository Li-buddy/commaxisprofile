#ifndef _MSC_VER
#include "celix/utils/celix_errno.h"
#else
#include "celix_errno.h"
#endif
#include "comm/datalayer/datalayer_system.h"
#include "comm/datalayer/persistence_helper.h"
#include "component/axisprofile_cmp.h"
#include "motion/test/result_helper.h"
#include "test_axisprofile_cmp.h"

// mockups
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"
#include "mockup/external_bundles/mock_datalayer_factory.h"
#include "mockup/external_bundles/mock_datalayer_memory_user.h"
#include "mockup/external_bundles/mock_diagnosis.h"
#include "mockup/external_bundles/mock_realtime_trace.h"
#include "mockup/external_bundles/mock_scheduler.h"
#include "mockup/external_bundles/mock_system_state.h"
#include "mockup/mock_background_thread.h"
#include <thread>

using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileCmp);
REGISTER_TEST_TO_GROUP(TestAxisProfileCmp, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileCmp, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileCmp, Component);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestAxisProfileCmp::TestAxisProfileCmp()
{
}


TestAxisProfileCmp::~TestAxisProfileCmp(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestAxisProfileCmp::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile component of the axis profile bundle");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= testNullpointers();
  hResult |= testServiceDependencies();
  hResult |= testCelixInterface();
  hResult |= testCelixInterfaceError();
  hResult |= testExternalCelixInterface();
  hResult |= doSimpleTest();
  hResult |= testBackgroundThread();

  return hResult;
}

HRESULT TestAxisProfileCmp::testServiceDependencies() const
{
  log(L"\n<b>Test the service dependency methods of the component.");

  HRESULT hResult = S_OK;
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactory;
  testFactory.setMemoryUser(testUserObject);
  comm::datalayer::IDataLayerFactory* expectedDatalayerFactory = &testFactory;
  comm::datalayer::IDataLayerFactory* actualDatalayerFactory;

  comm::datalayer::DatalayerSystem testDatalayerSystem;
  testDatalayerSystem.start();

  comm::datalayer::PersistenceHelper persistenceHelper{&testDatalayerSystem};

  AxisProfileCmp axisProfileCmp;
  axisProfileCmp.addDataLayer(testDatalayerSystem.factory());
  axisProfileCmp.addPersistenceHelper(persistenceHelper.getCInterface());

  celix_status_t result = axisProfileCmp.init();
  if (result != CELIX_SUCCESS)
  {
    error(L"\nCould not initialize component for this test case.");
    hResult = E_FAIL;
  }

  log(L"\nCheck persistence helper service dependency interface.");
  axisProfileCmp.addPersistenceHelper(persistenceHelper.getCInterface());
  auto receivedPersHelper = axisProfileCmp.getPersistenceHelper();
  if (!receivedPersHelper)
  {
    error(L"\nCould not initialize the persistence helper in the component!");
    hResult = E_FAIL;
  }
  axisProfileCmp.persistenceHelperServiceChanged(persistenceHelper.getCInterface());
  axisProfileCmp.removePersistenceHelper(persistenceHelper.getCInterface());

  log(L"\nCheck datalayer service dependency interface.");
  axisProfileCmp.addDataLayer(expectedDatalayerFactory);
  actualDatalayerFactory = axisProfileCmp.getDataLayer();
  if ( actualDatalayerFactory != expectedDatalayerFactory )
  {
    error(L"Setting the datalayer over dependency method failed.");
    hResult = E_FAIL;
  }
  axisProfileCmp.datalayerServiceChanged(actualDatalayerFactory);
  axisProfileCmp.removeDatalayer(actualDatalayerFactory);

  test::MockScheduler testScheduler;
  common::scheduler::IScheduler2* expectedScheduler = &testScheduler;
  common::scheduler::IScheduler2* actualScheduler;

  log(L"\nCheck scheduler service dependency interface.");
  axisProfileCmp.addScheduler(expectedScheduler);
  actualScheduler = axisProfileCmp.getScheduler();
  if ( actualScheduler != expectedScheduler )
  {
    error(L"Setting the scheduler over dependency method failed.");
    hResult = E_FAIL;
  }
  axisProfileCmp.schedulerServiceChanged(expectedScheduler);
  axisProfileCmp.removeScheduler(actualScheduler);

  log(L"\nCheck trace service dependency interface.");
  test::MockTraceLogRT testTraceLog;
  testTraceLog.setReturnValue(common::log::trace::TraceResult::NOK);
  test::MockTraceRegistrationRT mockTraceRegistration;
  mockTraceRegistration.setTestLog(&testTraceLog);
  axisProfileCmp.addTrace(&mockTraceRegistration);
  axisProfileCmp.traceServiceChanged(&mockTraceRegistration);
  axisProfileCmp.removeTrace(&mockTraceRegistration);

  log(L"\nCheck diagnosis service dependency interface.");
  test::MockDiagnosisRegistration mockDiagRegistration;
  axisProfileCmp.addDiagnosis(&mockDiagRegistration);
  axisProfileCmp.diagnosisServiceChanged(&mockDiagRegistration);
  axisProfileCmp.removeDiagnosis(&mockDiagRegistration);

  log(L"\nCheck system state dependency interface.");
  test::MockSystemState mockSystemState;
  axisProfileCmp.addSystemState(&mockSystemState);
  common::systemhandler::IComponent* systemStateCmp = axisProfileCmp.getSystemStateComponent();
  LOG_TEST_RESULT_SET_PTR(systemStateCmp, L"\nUnexpected pointer.");
  axisProfileCmp.removeSystemState(&mockSystemState);

  log(L"\nCheck set/get cycle time.");
  double expCycleTime {123E-6};
  axisProfileCmp.setCycleTime(expCycleTime);
  double cycleTime = axisProfileCmp.getCycleTime();
  LOG_TEST_RESULT_FLT(cycleTime, L"\nUnexpected cycle time.", expCycleTime, comm::axisprofile::STD_EPS);

  // test datalayer setter with existing axis profile manager
  axisProfileCmp.addDataLayer(expectedDatalayerFactory);
  axisProfileCmp.addPersistenceHelper(persistenceHelper.getCInterface());
  celix_status_t status = axisProfileCmp.init();
  if ( CELIX_SUCCESS != status )
  {
    error(L"Init of axisprofile component failed. 0x%X", status);
    hResult = E_FAIL;
  }
  status = axisProfileCmp.stop();
  testDatalayerSystem.stop();
  (hResult == S_OK) ? log(L"\nTest <DARK_GREEN>succeeded</DARK_GREEN>.") : log(L"\nTest <DARK_RED>failed</DARK_RED>.");

  return hResult;
}

HRESULT TestAxisProfileCmp::testCelixInterface()
{
  log(L"\n<b>Test the celix interface methods of the component...</b>");

  HRESULT hResult = S_OK;
  // Set service dependencies
  test::MockScheduler testScheduler;
  test::MockSystemState mockSystemState;
  test::MockTraceLogRT testLog;
  test::MockTraceRegistrationRT testRegistration;
  testRegistration.setTestLog(&testLog);

  comm::datalayer::DatalayerSystem testDatalayerSystem;
  testDatalayerSystem.start();

  comm::datalayer::PersistenceHelper persistenceHelper{&testDatalayerSystem};

  AxisProfileCmp axisProfileCmp;
  axisProfileCmp.addTrace(&testRegistration);
  axisProfileCmp.addDataLayer(testDatalayerSystem.factory());
  axisProfileCmp.addPersistenceHelper(persistenceHelper.getCInterface());
  axisProfileCmp.addScheduler(&testScheduler);
  axisProfileCmp.addSystemState(&mockSystemState);

  log(L"\nInit axisprofile component...");
  celix_status_t celixRes = axisProfileCmp.init();
  RETURN_TEST_RESULT(celixRes, L"Init of axisprofile component failed.", CELIX_SUCCESS);

  log(L"\nRe-trigger init to check case with existing axisprofile manager...");
  celixRes = axisProfileCmp.init();
  RETURN_TEST_RESULT(celixRes, L"Init of axisprofile component failed.", CELIX_SUCCESS);

  log(L"\nStart axisprofile component...");
  celixRes = axisProfileCmp.start();
  RETURN_TEST_RESULT(celixRes, L"Start of axisprofile component failed.", CELIX_SUCCESS);

  log(L"\nSleep for a while...");
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  log(L"\nStop axisprofile component...");
  celixRes = axisProfileCmp.stop();
  RETURN_TEST_RESULT(celixRes, L"Stop of axisprofile component failed.", CELIX_SUCCESS);

  log(L"\nDeinit axisprofile component...");
  celixRes = axisProfileCmp.deinit();
  RETURN_TEST_RESULT(celixRes, L"Dinit of axisprofile component failed.", CELIX_SUCCESS);
  
  testDatalayerSystem.stop();
  axisProfileCmp.removeTrace(&testRegistration);

  return hResult;
}

HRESULT TestAxisProfileCmp::testCelixInterfaceError()
{
  log(L"\n<b>Test the celix interface methods of the component for error cases</b>");

  HRESULT hResult = S_OK;
  // Set service dependencies
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));

  comm::datalayer::DatalayerSystem testDatalayerSystem;
  testDatalayerSystem.start();
  comm::datalayer::PersistenceHelper persistenceHelper{&testDatalayerSystem};

  test::MockDatalayerFactory testFactory;
  testFactory.setMemoryUser(testUserObject);
  test::MockScheduler testScheduler;
  test::MockSystemState mockSystemState;
  test::MockTraceLogRT testLog;
  test::MockTraceRegistrationRT testRegistration;
  testRegistration.setTestLog(&testLog);

  AxisProfileCmp axisProfileCmp;
  axisProfileCmp.addTrace(&testRegistration);
  axisProfileCmp.addScheduler(&testScheduler);
  axisProfileCmp.addSystemState(&mockSystemState);

  log(L"\nCall init() without datalayer.");
  celix_status_t status = axisProfileCmp.init();
  if (CELIX_BUNDLE_EXCEPTION != status)
  {
    error(L"Start of axisprofile component succeeded but should fail. 0x%X", status);
    hResult = E_FAIL;
  }

  axisProfileCmp.addDataLayer(&testFactory);
  axisProfileCmp.addPersistenceHelper(persistenceHelper.getCInterface());

  log(L"\nCall start() and simulate error in scheduler.");
  testScheduler.setReturnValue(common::scheduler::SchedStatus::SCHED_E_ERROR);
  status = axisProfileCmp.start();
  if ( CELIX_BUNDLE_EXCEPTION != status )
  {
    error(L"Start of axisprofile component succeeded but should fail. 0x%X", status);
    hResult = E_FAIL;
  }

  log(L"\nCall stop() and simulate error in scheduler.");
  status = axisProfileCmp.stop();
  if ( CELIX_BUNDLE_EXCEPTION != status )
  {
    error(L"Stop of axisprofile component succeeded but should fail. 0x%X", status);
    hResult = E_FAIL;
  }

  if ( hResult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  axisProfileCmp.removeTrace(&testRegistration);

  return hResult;
}

HRESULT TestAxisProfileCmp::testExternalCelixInterface()
{
  log(L"\n<b>Test external celix interfaces</b>");

  HRESULT hResult = S_OK;
  AxisProfileResult devStatus {AxisProfileResult::STS_OK};
  // Set service dependencies
  test::MockScheduler testScheduler;
  test::MockTraceLogRT testLog;
  test::MockTraceRegistrationRT testRegistration;
  testRegistration.setTestLog(&testLog);

  comm::datalayer::DatalayerSystem testDatalayerSystem;
  testDatalayerSystem.start();
  comm::datalayer::PersistenceHelper persistenceHelper{&testDatalayerSystem};

  test::MockEthercatMaster testMockEthercat(this, testDatalayerSystem.factory());
  testMockEthercat.init();

  AxisProfileCmp axisProfileCmp;
  axisProfileCmp.addTrace(&testRegistration);
  axisProfileCmp.addDataLayer(testDatalayerSystem.factory());
  axisProfileCmp.addScheduler(&testScheduler);
  axisProfileCmp.addPersistenceHelper(persistenceHelper.getCInterface());

  test::MockSystemState mockSystemState;
  axisProfileCmp.addSystemState(&mockSystemState);

  celix_status_t status = axisProfileCmp.init();
  if ( CELIX_SUCCESS != status )
  {
    error(L"Init of axisprofile component failed. 0x%X", status);
    axisProfileCmp.removeTrace(&testRegistration);
    return E_FAIL;
  }
  status = axisProfileCmp.start();
  if ( CELIX_SUCCESS != status )
  {
    error(L"Start of axisprofile component failed. 0x%X", status);
    axisProfileCmp.removeTrace(&testRegistration);
    return E_FAIL;
  }

  std::int32_t timeout = 50;
  while ( std::string("Configuration") != axisProfileCmp.getAxisProfileState() )
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if ( timeout < 0 )
    {
      error(L"\nDid not reach state configuration!");
      axisProfileCmp.removeTrace(&testRegistration);
      return E_FAIL;
    }
    --timeout;
  }

  log(L"\nTest on several profiles...");
  std::vector<std::string> profileNames {"profile1", "profile2", "profile3", "profile4", "profile5", "profile6", "profile7", "profile8"};
  for (const auto& profileName : profileNames)
  {
    log(L"\n\tCreate axisProfile %ls...", UTF8TOWIDE(profileName));
    devStatus = axisProfileCmp.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName);
    RETURN_TEST_RESULT(devStatus, L"create profile failed!", devStatus);

    log(L"\n\tConnect axisProfile %ls...", UTF8TOWIDE(profileName));
    std::shared_ptr<const IAxisProfileC> profile;
    devStatus = axisProfileCmp.connectAxisProfile(profileName, profile);
    RETURN_TEST_RESULT(devStatus, L"connect profile failed!", devStatus);

    log(L"\n\tCheck axisProfile exit %ls...", UTF8TOWIDE(profileName));
    bool exist = axisProfileCmp.profileExists(profileName);
    RETURN_TEST_RESULT_ANY(exist, L"profile not exist!");

    profile = nullptr;
  }

  log(L"\nCheck profiles total number ...");
  LOG_TEST_RESULT(axisProfileCmp.getConfiguredProfileNumber(), L"Unexpected configured profile number!", profileNames.size());

  log(L"\nDelete all created profiles...");
  for (auto profileName : profileNames)
  {
    log(L"\n\tDelete axisProfile %ls...", UTF8TOWIDE(profileName));
    devStatus = axisProfileCmp.deleteAxisProfile(profileName);
    RETURN_TEST_RESULT(devStatus, L"deleted profile failed!", devStatus);
  }

  std::vector<std::string> stateNames;
  if ( AxisProfileResult::STS_OK != axisProfileCmp.getAxisProfileStateNames(stateNames) )
  {
    error( L"\nCan't read state names!");
    hResult = E_FAIL;
  }
  if ( stateNames.size() != 4 )
  {
    error(L"Unexpected number of states! Expected: 4");
    log(L"\nStates:");
    for (const auto& name : stateNames )
    {
      log(L"\n\t%ls", test::StringHelper::utf8ToWide(name.c_str()).c_str());
    }
  }

  devStatus = axisProfileCmp.requestAxisProfileState("Booting");
  if ( AxisProfileResult::STS_OK != devStatus )
  {
    error(L"switch to running failed. 0x%X", status);
    hResult = E_FAIL;
  }

  log(L"\n\tCreate callable...");
  std::vector<std::string> callableAdpStr({"adaption"});
  std::vector<std::string> callableDsbStr({"distribution"});
  comm::datalayer::Variant callableParam;
  callableParam.setType(comm::datalayer::VariantType::ARRAY_OF_STRING);

  callableParam.setValue(callableAdpStr);
  axisProfileCmp.getCallableFactory()->createCallable(callableParam);

  callableParam.setValue(callableDsbStr);
  axisProfileCmp.getCallableFactory()->createCallable(callableParam);

  timeout = 50;
  common::scheduler::SchedEventType  eventType = ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING;
  common::scheduler::SchedEventPhase eventPhase = ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE;
  comm::datalayer::Variant param;
  while ( std::string("Running") != axisProfileCmp.getAxisProfileState() )
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if ( timeout < 0)
    {
      error(L"\nDid not reach state running!");
      axisProfileCmp.removeTrace(&testRegistration);
      return E_FAIL;
    }
    --timeout;
    axisProfileCmp.getCallableFactory()->getCallable(comm::axisprofile::CallableType::ADAPTION_CALLABLE)->execute(eventType, eventPhase, param);
  }

  devStatus = axisProfileCmp.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "testProfile");
  if ( AxisProfileResult::ERR_INVALID_STATE != devStatus )
  {
    error(L"createAxisProfile succeeded in invalid state. 0x%X", status);
    hResult = E_FAIL;
  }


  devStatus = axisProfileCmp.deleteAxisProfile("testProfile");
  if ( AxisProfileResult::ERR_INVALID_STATE != devStatus )
  {
    error(L"deleteAxisProfile succeeded in invalid state. 0x%X", status);
    hResult = E_FAIL;
  }

  status = axisProfileCmp.stop();
  if ( CELIX_SUCCESS != status )
  {
    error(L"Start of axisprofile component failed. 0x%X", status);
    hResult = E_FAIL;
  }

  status = axisProfileCmp.deinit();
  if ( CELIX_SUCCESS != status )
  {
    error(L"Stop of axisprofile component failed. 0x%X", status);
    hResult = E_FAIL;
  }


  if ( hResult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  testMockEthercat.deInit();
  testDatalayerSystem.stop();
  axisProfileCmp.removeTrace(&testRegistration);

  return hResult;
}

HRESULT TestAxisProfileCmp::doSimpleTest()
{
  HRESULT hresult = S_OK;
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactory;
  testFactory.setMemoryUser(testUserObject);
  test::MockScheduler testScheduler;
  test::MockSystemState mockSystemState;
  test::MockTraceLogRT testLog;
  test::MockTraceRegistrationRT testRegistration;
  testRegistration.setTestLog(&testLog);
  comm::datalayer::DatalayerSystem testDatalayerSystem;
  testDatalayerSystem.start();
  comm::datalayer::PersistenceHelper persistenceHelper{&testDatalayerSystem};

  AxisProfileCmp axisProfileCmp;
  axisProfileCmp.addTrace(&testRegistration);
  axisProfileCmp.addDataLayer(&testFactory);
  axisProfileCmp.addScheduler(&testScheduler);
  axisProfileCmp.addSystemState(&mockSystemState);
  axisProfileCmp.addPersistenceHelper(persistenceHelper.getCInterface());

  log(L"\n<b>Test the internal object getter.");

  celix_status_t status = axisProfileCmp.init();
  if ( CELIX_SUCCESS != status )
  {
    error(L"Init of axisprofile component failed. 0x%X", status);
    axisProfileCmp.removeTrace(&testRegistration);
    return E_FAIL;
  }

  const IAxisProfileManagerInternal* testManagerInternal = axisProfileCmp.getAxisProfileManager();
  if ( nullptr == testManagerInternal )
  {
    error(L"Pointer to IAxisProfileManagerInternal not set.");
    hresult = E_FAIL;
  }
  const IStateMachine* testStatemachine = axisProfileCmp.getStateMachine();
  if ( nullptr == testStatemachine )
  {
    error(L"Pointer to IStateMachine not set.");
    hresult = E_FAIL;
  }
  const IAxisProfileManager* testManager = axisProfileCmp.getAxisProfileManagerInterface();
  if ( nullptr == testManager )
  {
    error(L"Pointer to IAxisProfileManager not set.");
    hresult = E_FAIL;
  }

  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  log(L"\n<b>Test bundle name methods.");
  std::string testBundleName = "AxisProfileTestBundle";
  axisProfileCmp.setBundleName(testBundleName.c_str());
  std::string actualBundleName = axisProfileCmp.getBundleName();
  if ( actualBundleName != testBundleName )
  {
    error(L"Bundle name is not correct.");
  }
  axisProfileCmp.clrBundleName();
  actualBundleName = axisProfileCmp.getBundleName();
  if ( actualBundleName != "" )
  {
    error(L"Bundle name is not empty.");
  }
  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  log(L"\n<b>Test bundle path methods.");
  std::string testBundlePath = "AxisProfileTestPath";
  axisProfileCmp.setBundlePath(testBundlePath.c_str());
  std::string actualBundlePath = axisProfileCmp.getBundlePath();
  if ( actualBundlePath != testBundlePath )
  {
    error(L"Bundle path is not correct.");
  }
  axisProfileCmp.clrBundlePath();
  actualBundlePath = axisProfileCmp.getBundleName();
  if ( actualBundlePath != "" )
  {
    error(L"Bundle path is not empty.");
  }
  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  axisProfileCmp.removeTrace(&testRegistration);
  return hresult;
}

HRESULT TestAxisProfileCmp::testNullpointers()
{
  log(L"\n<b>Test the component when dependencies are null-pointers.");

  HRESULT hresult = S_OK;
  AxisProfileCmp axisProfileCmp;

  log(L"\nCall getAxisProfileState() when the statemachine is null.");
  const char* strResult = axisProfileCmp.getAxisProfileState();
  if (strResult)
  {
    error(L"\nReturn value was not null: %s", test::StringHelper::utf8ToWide(strResult).c_str());
    hresult = E_FAIL;
  }

  log(L"\nCall requestAxisProfileState() when the statemachine is null.");
  AxisProfileResult aprResult = axisProfileCmp.requestAxisProfileState("dummyString");
  if (aprResult != AxisProfileResult::ERR_NOK)
  {
    error(L"\nWrong return code! Expected: 'ERR_NOK ' Actual: 0x%08X", aprResult);
    hresult = E_FAIL;
  }

  log(L"\nCall getAxisProfileStateNames() when the statemachine is null.");
  std::vector<std::string> stateNames;
  aprResult = axisProfileCmp.getAxisProfileStateNames(stateNames);
  if (aprResult != AxisProfileResult::ERR_NOK)
  {
    error(L"\nWrong return code! Expected: 'ERR_NOK ' Actual: 0x%08X", aprResult);
    hresult = E_FAIL;
  }
  if (!stateNames.empty())
  {
    error(L"\nMethod returned state names!");
    hresult = E_FAIL;
  }

  return hresult;
}

HRESULT TestAxisProfileCmp::testBackgroundThread() const
{
  log(L"\n<b>Test the background thread related functions.");
  AxisProfileCmp axisProfileCmp;

  log(L"\nUsing empty background thread...");
  axisProfileCmp.m_bkThread.reset();

  log(L"\nSet cycle time failure...");
  auto aprRetValue = axisProfileCmp.setBkThreadCycleTime(0.0);
  LOG_TEST_RESULT(aprRetValue, L"Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nWakeup background failure...");
  aprRetValue = axisProfileCmp.wakeupBkThread();
  LOG_TEST_RESULT(aprRetValue, L"Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nUsing mockup background thread...");
  axisProfileCmp.m_bkThread.reset(new test::MockBackgroundThread);

  log(L"\nSet cycle time success...");
  aprRetValue = axisProfileCmp.setBkThreadCycleTime(0.005);
  LOG_TEST_RESULT(aprRetValue, L"Unexpected return value", AxisProfileResult::STS_OK);

  log(L"\nWakeup background success...");
  aprRetValue = axisProfileCmp.wakeupBkThread();
  LOG_TEST_RESULT(aprRetValue, L"Unexpected return value", AxisProfileResult::STS_OK);

  log(L"\nReset background thread...");
  axisProfileCmp.m_bkThread.reset();

  return S_OK;
}
