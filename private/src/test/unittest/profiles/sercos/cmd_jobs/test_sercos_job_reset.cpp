#include "helper/result_helper.h"
#include "test_sercos_job_reset.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestSercosJobReset);
REGISTER_TEST_TO_GROUP(TestSercosJobReset, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosJobReset, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosJobReset, Profiles_Sercos);


HRESULT TestSercosJobReset::doInit()
{
  m_sharingHandler = std::make_unique<AxisProfileSharingHandler>(&m_mockProfileTrace);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &m_mockScaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &m_mockMappingHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &m_mockCyclicData);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &m_mockStatusControl);
  return S_OK;
}

HRESULT TestSercosJobReset::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the sercos profile cmdJobs SercosJobReset class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestJobEnterActivating();
  hresult |= TestRTCommandActivating();
  hresult |= TestJobAborting();
  hresult |= TestUpdateStatusInDevice();

  return hresult;
}

HRESULT TestSercosJobReset::doEnd()
{
  HRESULT result = S_OK;
  log(L"\nDeinit test cases...");
  return result;
}


HRESULT TestSercosJobReset::TestJobEnterActivating()
{
  log(L"\n<b>Test SercosJobReset::onJobEnterActivating</b>");

  ProfileBasicInfo profileBasicInfo = createProfileBasicInfo(m_mockProfileTrace);
  AxisProfileSharingHandler emptySharingHandler {&m_mockProfileTrace};
  TestableSercosJobReset jobWithoutDeviceParam(profileBasicInfo, &emptySharingHandler);
  log(L"\nTest case: device parameter interface is missing");
  AxisProfileResult result = jobWithoutDeviceParam.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"onJobEnterActivating should switch to normal command handle", AxisProfileResult::STS_OK);

  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  TestableSercosJobReset jobWithoutSignalFuncMgr(profileBasicInfo, m_sharingHandler.get());
  log(L"\nTest case: signal function manager is missing");
  result = jobWithoutSignalFuncMgr.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"onJobEnterActivating should switch to normal command handle", AxisProfileResult::STS_OK);

  RTTestContext rtContext {m_mockProfileTrace};
  result = prepareRTTestContext(m_sharingHandler.get(), m_mockMappingHandler, rtContext, true);
  RETURN_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);
  TestableSercosJobReset rtJob(rtContext.profileBasicInfo, m_sharingHandler.get());
  log(L"\nTest case: reset command is available in signal word");
  result = rtJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"onJobEnterActivating should succeed with signal function support", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(rtJob.isRTCmd(), L"reset job should use realtime command when signal function is supported", true);
  LOG_TEST_RESULT(commandStepValue(rtJob.asyncStateMachine().getCommandStep()), L"async command should start from CMD_START", commandStepValue(AsyncCommandStatemachine::CommandStep::CMD_START));

  RTTestContext nonRTContext {m_mockProfileTrace};
  result = prepareRTTestContext(m_sharingHandler.get(), m_mockMappingHandler, nonRTContext, false);
  RETURN_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);
  TestableSercosJobReset nonRTJob(nonRTContext.profileBasicInfo, m_sharingHandler.get());
  log(L"\nTest case: reset command is not available in signal word");
  result = nonRTJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"onJobEnterActivating should still succeed without realtime support", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(nonRTJob.isRTCmd(), L"reset job should use sercos procedure command when signal function is unsupported", false);

  return S_OK;
}

HRESULT TestSercosJobReset::TestRTCommandActivating()
{
  log(L"\n<b>Test SercosJobReset::onJobActivating for realtime command</b>");

  RTTestContext rtContext {m_mockProfileTrace};
  AxisProfileResult result = prepareRTTestContext(m_sharingHandler.get(), m_mockMappingHandler, rtContext, true);
  RETURN_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);
  TestableSercosJobReset testJob(rtContext.profileBasicInfo, m_sharingHandler.get());
  result = testJob.onJobEnterActivating();
  RETURN_TEST_RESULT(result, L"onJobEnterActivating failed", AxisProfileResult::STS_OK);

  log(L"\nTest case: clear signal bit before setting command");
  result = testJob.onJobActivating();
  LOG_TEST_RESULT(result, L"CMD_START should keep command pending", AxisProfileResult::STS_PENDING);
  LOG_TEST_RESULT(rtContext.cyclicData.m_resetCallCount, L"resetSignalFuncBit should be called once", 1);
  LOG_TEST_RESULT(commandStepValue(testJob.asyncStateMachine().getCommandStep()), L"command step should switch to CMD_SET", commandStepValue(AsyncCommandStatemachine::CommandStep::CMD_SET));

  log(L"\nTest case: activate signal bit to start reset command");
  result = testJob.onJobActivating();
  LOG_TEST_RESULT(result, L"CMD_SET should keep command pending", AxisProfileResult::STS_PENDING);
  LOG_TEST_RESULT(rtContext.cyclicData.m_activeCallCount, L"activeSignalFunc should be called once", 1);
  LOG_TEST_RESULT(commandStepValue(testJob.asyncStateMachine().getCommandStep()), L"command step should switch to CMD_CHECK_STATUS", commandStepValue(AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS));

  log(L"\nTest case: command remains pending while status bit is not set");
  rtContext.cyclicData.m_signalState = false;
  result = testJob.onJobActivating();
  LOG_TEST_RESULT(result, L"CMD_CHECK_STATUS should stay pending while status bit is false", AxisProfileResult::STS_PENDING);
  LOG_TEST_RESULT(rtContext.cyclicData.m_stateCallCount, L"getSignalFuncState should be called once", 1);
  LOG_TEST_RESULT(commandStepValue(testJob.asyncStateMachine().getCommandStep()), L"command step should stay in CMD_CHECK_STATUS", commandStepValue(AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS));

  log(L"\nTest case: command finishes after status bit is set and command bit is cleared");
  // cppcheck-suppress redundantAssignment
  rtContext.cyclicData.m_signalState = true;  
  result = testJob.onJobActivating();
  LOG_TEST_RESULT(result, L"CMD_DONE should finish realtime reset command", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(testJob.deviceStatus().get().load(), L"device status should be in process after command start", CmdStatusInDevice::DEVICE_IN_PROCESS);

  return S_OK;
}

HRESULT TestSercosJobReset::TestJobAborting()
{
  log(L"\n<b>Test SercosJobReset::onJobAborting for realtime command</b>");

  RTTestContext rtContext {m_mockProfileTrace};
  AxisProfileResult result = prepareRTTestContext(m_sharingHandler.get(), m_mockMappingHandler, rtContext, true);
  RETURN_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);
  TestableSercosJobReset testJob(rtContext.profileBasicInfo, m_sharingHandler.get());
  result = testJob.onJobEnterActivating();
  RETURN_TEST_RESULT(result, L"onJobEnterActivating failed", AxisProfileResult::STS_OK);
  testJob.deviceStatus().getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);

  result = testJob.onJobAborting();
  LOG_TEST_RESULT(result, L"onJobAborting should finish immediately for realtime command", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(rtContext.cyclicData.m_resetCallCount, L"resetSignalFuncBit should be called during abort", 1);
  LOG_TEST_RESULT(testJob.deviceStatus().get().load(), L"device status should be reset after abort", CmdStatusInDevice::DEVICE_NOT_SET);

  return S_OK;
}

HRESULT TestSercosJobReset::TestUpdateStatusInDevice()
{
  log(L"\n<b>Test SercosJobReset::onUpdateStatusInDevice</b>");

  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &m_mockStatusControl);
  ProfileBasicInfo profileBasicInfo = createProfileBasicInfo(m_mockProfileTrace);
  TestableSercosJobReset testJob(profileBasicInfo, m_sharingHandler.get());
  testJob.deviceStatus().getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);

  log(L"\nTest case: device still reports error");
  m_mockStatusControl.m_deviceError = true;
  AxisProfileResult result = testJob.onUpdateStatusInDevice();
  LOG_TEST_RESULT(result, L"onUpdateStatusInDevice should succeed while device still has error", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(testJob.deviceStatus().get().load(), L"device status should stay in process while error is active", CmdStatusInDevice::DEVICE_IN_PROCESS);

  log(L"\nTest case: device error flag is cleared");
  m_mockStatusControl.m_deviceError = false;
  result = testJob.onUpdateStatusInDevice();
  LOG_TEST_RESULT(result, L"onUpdateStatusInDevice should succeed after device error is cleared", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(testJob.deviceStatus().get().load(), L"device status should become completed after error is cleared", CmdStatusInDevice::DEVICE_COMPLETED);

  testJob.deviceStatus().getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  result = testJob.onUpdateStatusInDevice();
  LOG_TEST_RESULT(result, L"onUpdateStatusInDevice should ignore status when command is not in process", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(testJob.deviceStatus().get().load(), L"device status should stay unchanged when command is not in process", CmdStatusInDevice::DEVICE_NOT_SET);

  return S_OK;
}
