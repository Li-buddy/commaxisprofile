#include "test_boot_manager.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/external_bundles/mock_datalayer_factory.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"
#include "mockup/external_bundles/mock_scheduler.h"
#include "mockup/external_bundles/mock_system_state.h"
#include "mockup/external_bundles/mock_datalayer_client_ex.h"

// Test Class
#include "configuration/boot_mgr.h"

// Helper Class
#include "motion/test/result_helper.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestConfigurationBootManager);
REGISTER_TEST_TO_GROUP(TestConfigurationBootManager, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestConfigurationBootManager, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestConfigurationBootManager, Configuration);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestConfigurationBootManager::TestConfigurationBootManager()
{
}


TestConfigurationBootManager::~TestConfigurationBootManager(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestConfigurationBootManager::doInit()
{
  return S_OK;
}

HRESULT TestConfigurationBootManager::doTest()
{
  HRESULT hresult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the boot manager of the axis profile bundle");
  log(L"\n----------------------------------------------------------------\n");

  hresult |= testMiscellaneous();
  hresult |= testInitialization();
  hresult |= testCheckFieldbusStatus();
  hresult |= testCheckDeviceStatus();
  hresult |= testCheckFieldbusAndDeviceStatus();
  hresult |= testSchedulerCallableInRunState();  
  hresult |= testInternalBootup();
  hresult |= testGetMaxBootingTimeout();
  hresult |= testResetAllProfiles();

  return hresult;
}

HRESULT TestConfigurationBootManager::doEnd()
{
  return S_OK;
}

HRESULT TestConfigurationBootManager::testMiscellaneous()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of miscellaneous methods of the class.</b>");

  BootMgr testClass{};

  log(L"\nCall abortBooting() when everything is OK.");
  testClass.abortBooting();


  std::uint32_t actStep;
  std::uint32_t maxSteps;
  std::string txt;
  log(L"\nCall getBootState when everything is OK.");
  testClass.getBootingState(actStep, maxSteps, txt);

  log(L"\n");
  return hResult;
}

HRESULT TestConfigurationBootManager::testInitialization()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test the initialization of the class.</b>");

  BootMgr testClass{};
  test::MockAxisProfileCmp mockComponent{};
  test::MockDatalayerFactory mockDatalayerFactory{};

  log(L"\nCall init() without component.");
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult actResult = testClass.init(nullptr);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall init() with component which has no statemachine.");
  mockComponent.setStatemachine(false);
  actResult = testClass.init(&mockComponent);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall init() with component which has no backup/restore manager.");
  mockComponent.setStatemachine(true);
  mockComponent.setBackupRestoreManager(false);
  actResult = testClass.init(&mockComponent);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall init() with component and simulate create datalayer client error.");
  mockComponent.setBackupRestoreManager(true);
  mockComponent.addDataLayer(&mockDatalayerFactory);
  mockDatalayerFactory.simulateCreateClientError(true);
  actResult = testClass.init(&mockComponent);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall init() with component and simulate statemachine addCallback() error.");
  mockDatalayerFactory.simulateCreateClientError(false);
  static_cast<test::MockStateMachine*>(mockComponent.getStateMachine())->m_retResult = GlobalStateResult::ERR_NOK;
  expResult = AxisProfileResult::ERR_STATEMACHINE_ADD_CALLBACK;
  actResult = testClass.init(&mockComponent);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall init() when everything is OK.");
  static_cast<test::MockStateMachine*>(mockComponent.getStateMachine())->m_retResult = GlobalStateResult::STS_OK;
  expResult = AxisProfileResult::STS_OK;
  actResult = testClass.init(&mockComponent);
  if (actResult != expResult)
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", actResult);
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestConfigurationBootManager::testCheckFieldbusStatus()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test the method which checks the fieldbus status.</b>");

  BootMgr testClass{};
  test::MockAxisProfileCmp mockComponent{};
  test::MockDatalayerFactory mockDatalayerFactory{};
  mockComponent.addDataLayer(&mockDatalayerFactory);
  testClass.init(&mockComponent);

  const std::string masterName = "mockMaster";
  test::MockFieldbusAdapterSoE adapter;
  adapter.setDatalayer(testClass.m_datalayerClient);

  test::FlatbuffersHelperCommon fbHelper;

  log(L"\nCall checkFieldbusStatus() when datalayer returns an error...");
  static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  AxisProfileResult actResult = testClass.checkFieldbusStatus(masterName, &adapter);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS_READ_VALUE);

  static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_returnValue = comm::datalayer::DlResult::DL_OK;

  log(L"\nCall checkFieldbusStatus() when SettingsInconsistent...");
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_PREOP;     // in pre-op
  fbHelper.m_masterStatus.m_statusBits.SettingsInconsistent = TRUE; // everything is find but SettingsInconsistent
  fbHelper.generateMasterStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkFieldbusStatus(masterName, &adapter);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS);

  log(L"\nCall checkFieldbusStatus() when PortError...");
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_PREOP;
  fbHelper.m_masterStatus.m_statusBits.PortError = TRUE; // everything is find but PortError
  fbHelper.generateMasterStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkFieldbusStatus(masterName, &adapter);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS);

  log(L"\nCall checkFieldbusStatus() when LicenseError...");
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_PREOP;
  fbHelper.m_masterStatus.m_statusBits.LicenseError = TRUE; // everything is find LicenseError
  fbHelper.generateMasterStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkFieldbusStatus(masterName, &adapter);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS);

  log(L"\nCall checkFieldbusStatus() when not in OP...");
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_PREOP;
  fbHelper.m_masterStatus.m_statusBits.DcNotInSync = TRUE; // DC not ready
  fbHelper.generateMasterStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkFieldbusStatus(masterName, &adapter);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_PENDING);

  log(L"\nCall checkFieldbusStatus() when not initialized...");
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_NOT_INIT;
  fbHelper.generateMasterStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkFieldbusStatus(masterName, &adapter);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_FIELDBUS_NOT_INIT);

  log(L"\nCall checkFieldbusStatus() when in OP...");
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_INOP;
  fbHelper.generateMasterStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkFieldbusStatus(masterName, &adapter);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n");
  return hResult;
}

HRESULT TestConfigurationBootManager::testCheckDeviceStatus()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test the method which checks the device states.</b>");

  BootMgr testClass{};
  test::MockAxisProfileCmp mockComponent{};
  test::MockDatalayerFactory mockDatalayerFactory{};
  mockComponent.addDataLayer(&mockDatalayerFactory);
  testClass.init(&mockComponent);
  const std::string profileName = "mockProfile";
  test::FlatbuffersHelperCommon fbHelper;
  test::MockAxisProfileManager* profileMgr = static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager());

  log(L"\nCall checkDeviceStatus() when AxisProfileManager has inconsistent data...");
  profileMgr->m_mapAxisProfiles[profileName] = nullptr;
  AxisProfileResult actResult = testClass.checkDeviceStatus(profileName);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_PROFILE_NOT_FOUND);

  log(L"\nCreate a profile for further testing.");
  profileMgr->m_mapAxisProfiles.clear();
  mockComponent.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName);
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(profileMgr->getProfileByName(profileName.c_str()));

  test::MockFieldbusAdapterSoE* adapter = static_cast<test::MockFieldbusAdapterSoE*>(profile->getFieldbusAdapter());
  adapter->setDatalayer(testClass.m_datalayerClient);

  log(L"\nCall checkDeviceStatus() when datalayer returns an error...");
  static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  actResult = testClass.checkDeviceStatus(profileName);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS_READ_VALUE);

  log(L"\nCall checkDeviceStates() when device status error...");
  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.m_slaveStatus.m_statusBits.Error = TRUE;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_returnValue = comm::datalayer::DlResult::DL_OK;
  actResult = testClass.checkDeviceStatus(profileName);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS);

  log(L"\nCall checkDeviceStates() when device NotOnline...");
  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.m_slaveStatus.m_statusBits.NotOnline = TRUE;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkDeviceStatus(profileName);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS);

  log(L"\nCall checkDeviceStates() when device NotInStateOp...");
  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.m_slaveStatus.m_statusBits.NotInStateOp = TRUE;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkDeviceStatus(profileName);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_PENDING);

  log(L"\nCall checkDeviceStates() when device in OP...");
  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&static_cast<test::MockDatalayerClient*>(testClass.m_datalayerClient)->m_callbackVariant);
  actResult = testClass.checkDeviceStatus(profileName);
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n");
  return hResult;
}

HRESULT TestConfigurationBootManager::testCheckFieldbusAndDeviceStatus()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test the method which checks fieldbus and device status.</b>");

  BootMgr testClass {};
  test::MockAxisProfileCmp mockComponent {};
  test::MockDatalayerFactory mockDatalayerFactory {};
  mockComponent.addDataLayer(&mockDatalayerFactory);
  testClass.init(&mockComponent);
  const std::string masterName = "mockMaster";
  const std::string profileName = "mockProfile";
  test::FlatbuffersHelperCommon fbHelper;
  test::MockAxisProfileManager* profileMgr = static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager());

  log(L"\nCall checkFieldbusAndDeviceStatus() when AxisProfileManager has no profile...");
  profileMgr->m_mapAxisProfiles.clear();
  AxisProfileResult actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nCreate a profile for further testing.");
  mockComponent.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName);

  log(L"\nCall checkFieldbusAndDeviceStatus() when AxisProfileManager has no field master...");
  actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS_NO_MASTER);

  log(L"\nCreate a fieldbus master for further testing.");
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(profileMgr->getProfileByName(profileName.c_str()));
  profileMgr->m_deviceInfoMgr.linkProfileToFieldbusInstance(masterName.c_str(), profile);
  test::MockFieldbusAdapterSoE* adapter = static_cast<test::MockFieldbusAdapterSoE*>(profile->getFieldbusAdapter());
  adapter->setDatalayer(testClass.m_datalayerClient);

  log(L"\nCall checkFieldbusAndDeviceStatus() when endless loop gets aborted...");
  testClass.abortBooting();
  actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_BOOTING_ABORT);
  testClass.m_abortBooting = false; // bring back

  log(L"\nCall checkFieldbusAndDeviceStatus() when timeout...");
  testClass.m_timeoutFieldBusDeviceOP = 1; // trigger in 1st calling
  actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_TIMEOUT);
  testClass.m_timeoutFieldBusDeviceOP = TIMEOUT_FIELDBUS_DEVICE_OP_MS / POLLING_SLEEP_DEVICE_OP_MS; // bring back

  log(L"\nUsing more powerful datalayer client mockup...");
  test::MockDatalayerClientEx dlClientEx;
  adapter->setDatalayer(&dlClientEx);
  comm::datalayer::Variant tmpVar;
  dlClientEx.resetClient();
  log(L"\nCall checkFieldbusAndDeviceStatus() when everything okay...");  
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_INOP;
  fbHelper.generateMasterStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  dlClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nCall checkFieldbusAndDeviceStatus() when field no initialized for ...");
  dlClientEx.resetClient();
  for (size_t i = 0; i < TIMEOUT_FIELDBUS_DEVICE_OP_MS / POLLING_SLEEP_DEVICE_OP_MS+1; i++)
  {
    fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_NOT_INIT;
    fbHelper.generateMasterStatusResponseFlatbuffer(&tmpVar);
    dlClientEx.m_vecCallBackVariant.push_back(tmpVar);
  }
  dlClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_TIMEOUT);

  log(L"\nCall checkFieldbusAndDeviceStatus() when Fieldbus/Device get error in 10 seconds...");
  dlClientEx.resetClient();
  for (size_t i = 0; i < 10; i++)
  {
    fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_PREOP;
    fbHelper.m_masterStatus.m_statusBits.NoEthernetLink = TRUE; // no link on master
    fbHelper.generateMasterStatusResponseFlatbuffer(&tmpVar);
    dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

    fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
    fbHelper.m_slaveStatus.m_statusBits.Error = TRUE; // slave has error
    fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
    dlClientEx.m_vecCallBackVariant.push_back(tmpVar);
  }

  dlClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::ERR_FIELDBUS);

  log(L"\nCall checkFieldbusAndDeviceStatus() when checkFieldbusStatus get pending...");
  dlClientEx.resetClient();
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_PREOP;
  fbHelper.m_masterStatus.m_statusBits.DcNotInSync = TRUE; // not in DC, should check next cycle
  fbHelper.generateMasterStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_INOP; // everything is okay
  fbHelper.generateMasterStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  dlClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nCall checkFieldbusAndDeviceStatus() when several profiles...");
  const std::string profileName1 = "mockProfile1";
  const std::string profileName2 = "mockProfile2";
  log(L"\nCreate a profile1...");
  mockComponent.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName1);
  profile = static_cast<IAxisProfileInternal*>(profileMgr->getProfileByName(profileName1.c_str()));
  adapter = static_cast<test::MockFieldbusAdapterSoE*>(profile->getFieldbusAdapter());
  adapter->setDatalayer(&dlClientEx);

  log(L"\nCreate a profile2...");
  mockComponent.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName2);
  profile = static_cast<IAxisProfileInternal*>(profileMgr->getProfileByName(profileName2.c_str()));
  adapter = static_cast<test::MockFieldbusAdapterSoE*>(profile->getFieldbusAdapter());
  adapter->setDatalayer(&dlClientEx);

  dlClientEx.resetClient();
  fbHelper.m_masterStatus.m_status = test::MASTER_STATUS_INOP;
  fbHelper.generateMasterStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.m_slaveStatus.m_statusBits.NotInStateOp = 1;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);
  fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  fbHelper.m_slaveStatus.m_status = test::DEVICE_STATUS_INOP;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);
  fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);
  fbHelper.generateSlaveStatusResponseFlatbuffer(&tmpVar);
  dlClientEx.m_vecCallBackVariant.push_back(tmpVar);

  dlClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  actResult = testClass.checkFieldbusAndDeviceStatus();
  LOG_TEST_RESULT(actResult, L"unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n");
  return hResult;
}

HRESULT TestConfigurationBootManager::testSchedulerCallableInRunState()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test the method which checks scheduler state.</b>");

  BootMgr testClass{};
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  log(L"\nCall isSchedulerCallableInRunState() when component not exist...");
  AxisProfileResult actResult = testClass.isSchedulerCallableInRunState();
  LOG_TEST_RESULT(actResult, L"Method did not fail with expected error code.", expResult);

  log(L"\nCall isSchedulerCallableInRunState() when adaption callable not ready...");
  test::MockAxisProfileCmp mockComponent{};
  test::MockDatalayerFactory mockDatalayerFactory{};
  test::MockScheduler testScheduler{};
  mockComponent.addDataLayer(&mockDatalayerFactory);
  mockComponent.addScheduler(&testScheduler);
  mockComponent.setCallableFactory(std::make_shared<AxisprofileCallableFactory>(&mockComponent));
  testClass.init(&mockComponent);
  expResult = AxisProfileResult::ERR_INVALID_STATE;
  testClass.m_timeoutSchedRunState = 5;
  actResult = testClass.isSchedulerCallableInRunState();
  LOG_TEST_RESULT(actResult, L"Method did not fail with expected error code.", expResult);
  return hResult;
}

HRESULT TestConfigurationBootManager::testInternalBootup()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test the method which checks internal bootup.</b>");

  BootMgr testClass{};
  test::MockScheduler testScheduler{};
  test::MockAxisProfileCmp mockComponent{};
  test::MockDatalayerFactory mockDatalayerFactory{};
  mockComponent.addDataLayer(&mockDatalayerFactory);
  mockComponent.addScheduler(&testScheduler);
  std::shared_ptr<comm::axisprofile::AxisprofileCallableFactory> mockDprCallableFactory = std::make_shared<AxisprofileCallableFactory>(&mockComponent);
  mockComponent.setCallableFactory(mockDprCallableFactory); 
  testScheduler.registerCallableFactory(static_cast<const std::shared_ptr<common::scheduler::ICallableFactory2>&>(mockDprCallableFactory), "axisprofile");

  log(L"\nCreate callable...");
  std::vector<std::string> callableAdpStr({"adaption"});
  std::vector<std::string> callableDsbStr({"distribution"});
  comm::datalayer::Variant callableParam;
  callableParam.setType(comm::datalayer::VariantType::ARRAY_OF_STRING);

  callableParam.setValue(callableAdpStr);
  mockDprCallableFactory->createCallable(callableParam);

  callableParam.setValue(callableDsbStr);
  mockDprCallableFactory->createCallable(callableParam);

  common::scheduler::SchedEventType  eventType = ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING;
  common::scheduler::SchedEventPhase eventPhase = ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE;
  comm::datalayer::Variant param;
  testClass.init(&mockComponent);
  mockDprCallableFactory->getCallable(comm::axisprofile::CallableType::ADAPTION_CALLABLE)->execute(eventType, eventPhase, param);
  test::MockSystemState mockSystemState;
  mockComponent.addSystemState(&mockSystemState);
  
  log(L"\nTrigger internalBootup() semaphore and simulate an error in the AxisProfileManager.");
  StateIDType targetState = StateIDType::CONFIG;
  testClass.m_initialBootup = false; // not 1st booting by cmp
  static_cast<test::MockStateMachine*>(mockComponent.getStateMachine())->m_stateID = StateIDType::ERROR_STATE; // ensure that initial state is not equal to target state
  static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager())->m_returnValue = AxisProfileResult::ERR_NOK;
  testClass.wakeup();
  if (FAILED(waitForStateSwitch(&mockComponent, targetState, 200)))
  {
    error(L"State transition timeout! State '%ls' not reached.", UTF8TOWIDE(stateTypeToString(targetState)));
    hResult = E_FAIL;
  }
  std::uint32_t actStep;
  std::uint32_t maxSteps;
  std::string txt;
  testClass.getBootingState(actStep, maxSteps, txt);
  if("Booting failed" != txt)
  {
    error(L"State get error! expected state 'Booting failed' not reached, actual state is '%ls'.", UTF8TOWIDE(txt.c_str()));
    hResult = E_FAIL;
  }

  // test of bugfix 231947
  log(L"\nTrigger internalBootup() semaphore and simulate an error of STS_NO_CONFIG_FILES_FOUND during system booting.");
  targetState = StateIDType::CONFIG;
  testClass.m_initialBootup = true; // initial bootup again
  static_cast<test::MockStateMachine*>(mockComponent.getStateMachine())->m_stateID = StateIDType::ERROR_STATE; // ensure that initial state is not equal to target state
  static_cast<test::MockBackupRestoreManager*>(mockComponent.getBackupRestoreManager())->setReturnValue(AxisProfileResult::STS_NO_CONFIG_FILES_FOUND);
  testClass.wakeup();
  if (FAILED(waitForStateSwitch(&mockComponent, targetState, 200)))
  {
    error(L"State transition timeout! State '%ls' not reached.", UTF8TOWIDE(stateTypeToString(targetState)));
    hResult = E_FAIL;
  }
  if ("Booting failed" != txt)
  {
    error(L"State get error! expected state 'Booting failed' not reached, actual state is '%ls'.", UTF8TOWIDE(txt.c_str()));
    hResult = E_FAIL;
  }

  log(L"\nTrigger internalBootup() semaphore and abort by self booting.");
  targetState = StateIDType::CONFIG;
  testClass.m_initialBootup = true;
  static_cast<test::MockStateMachine*>(mockComponent.getStateMachine())->m_stateID = StateIDType::ERROR_STATE; // ensure that initial state is not equal to target state
  static_cast<test::MockBackupRestoreManager*>(mockComponent.getBackupRestoreManager())->setReturnValue(AxisProfileResult::STS_BOOTING_ABORT);
  testClass.wakeup();
  if (FAILED(waitForStateSwitch(&mockComponent, targetState, 200)))
  {
    error(L"State transition timeout! State '%ls' not reached.", UTF8TOWIDE(stateTypeToString(targetState)));
    hResult = E_FAIL;
  }
  testClass.getBootingState(actStep, maxSteps, txt);
  if ("Booting aborted" != txt)
  {
    error(L"State get error! expected state 'Booting abort' not reached, actual state is '%ls'.", UTF8TOWIDE(txt.c_str()));
    hResult = E_FAIL;
  }

  log(L"\nTrigger internalBootup() semaphore when everything is OK.");
  targetState = StateIDType::RUNNING;
  static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager())->m_returnValue = AxisProfileResult::STS_OK;
  testClass.wakeup();
  if (FAILED(waitForStateSwitch(&mockComponent, targetState, 200)))
  {
    error(L"State transition timeout! State '%ls' not reached.", UTF8TOWIDE(stateTypeToString(targetState)));
    hResult = E_FAIL;
  }

  testClass.getBootingState(actStep, maxSteps, txt);
  if("Booting finished" != txt)
  {
    error(L"State get error! expected State 'Booting finished' not reached.");
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestConfigurationBootManager::waitForStateSwitch(IAxisProfileComponent* cmp, StateIDType expState, std::uint32_t timeoutMS)
{
  const std::uint32_t SLEEP_TIME_MS = 10;
  std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
  std::uint32_t timeOut = timeoutMS / SLEEP_TIME_MS;
  while (timeOut > 0)
  {
    if (cmp->getStateMachine()->getState() == expState)
    {
      return S_OK;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
    --timeOut;
  }
  return E_FAIL;
}

void TestConfigurationBootManager::generateMasterStateResponseFlatbuffer(comm::ethercat::master::fbs::EthercatState state, comm::datalayer::Variant* variant)
{
  comm::datalayer::Variant retValue{};
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  auto masterStateResponseOffset = CreateMasterStateResponse(builder, state);
  comm::ethercat::master::fbs::MasterStateBuilder masterStateBuilder(builder);
  masterStateBuilder.add_response(masterStateResponseOffset);
  auto slaveStateBuilderOffset = masterStateBuilder.Finish();
  builder.Finish(slaveStateBuilderOffset);

  variant->copyFlatbuffers(builder);
}

HRESULT TestConfigurationBootManager::testGetMaxBootingTimeout()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test the method getMaxBootingTimeout.</b>");

  BootMgr testClass {};
  test::MockAxisProfileCmp mockComponent {};
  testClass.init(&mockComponent);

  log(L"\nCreate a profiles...");  
  mockComponent.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "profile1");
  mockComponent.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "profile2");
  mockComponent.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "profile3");

  std::int32_t expTimeoutMs = 0;
  expTimeoutMs += mockComponent.getAxisProfileManager()->getConfiguredProfileNumber() * TIMEOUT_READ_DEVICE_PRAMETERS;
  expTimeoutMs += TIMEOUT_SCHED_IN_RUN_STATE_MS + TIMEOUT_FIELDBUS_DEVICE_OP_MS + TIMEOUT_WAIT_BEFORE_DEVICE_OP;

  double expTimeout = MILLISECOND_2_SECOND(static_cast<double>(expTimeoutMs)) + 1;
  double rstTimeout = testClass.getMaxBootingTimeout();

  LOG_TEST_RESULT_FLT(rstTimeout, L"Unexpected booting timeout value.", expTimeout, STD_EPS);

  log(L"\n");
  return hResult;
}


HRESULT TestConfigurationBootManager::testResetAllProfiles()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test the method resetAllProfiles.</b>");

  BootMgr testClass {};
  test::MockAxisProfileCmp mockComponent {};
  test::MockAxisProfileManager* profileMgr = static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager());
  const std::string profileName = "mockProfile";
  testClass.init(&mockComponent);

  log(L"\nCreate a profiles...");
  mockComponent.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName);
  test::MockAxisProfile* profile = static_cast<test::MockAxisProfile*>(profileMgr->getProfileByName(profileName.c_str()));

  log(L"\nTest everything works fine...");
  profile->setReturnValue(AxisProfileResult::STS_OK);
  AxisProfileResult result = testClass.resetAllProfiles();
  LOG_TEST_RESULT(result, L"Method did not succeed.", AxisProfileResult::STS_OK);

  log(L"\nTest reset profile failed...");
  profile->setReturnValue(AxisProfileResult::ERR_NOK);
  result = testClass.resetAllProfiles();
  LOG_TEST_RESULT(result, L"Method did not fail with expected error code.", AxisProfileResult::ERR_PROFILE_RESET_FAILED);

  log(L"\n");
  return hResult;
}
