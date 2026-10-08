#include "test_sercos_signal_func_mgr.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestSercosSignalFuncHandler);
REGISTER_TEST_TO_GROUP(TestSercosSignalFuncHandler, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosSignalFuncHandler, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosSignalFuncHandler, Profiles_Sercos);

HRESULT TestSercosSignalFuncHandler::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Sercos signal function manager class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testSupportedSignalFunctions();
  hresult |= testSignalControlWordNotInMdt();
  hresult |= testSignalWordMappingRequired();
  hresult |= testErrorHandling();
  return hresult;
}

HRESULT TestSercosSignalFuncHandler::testSupportedSignalFunctions()
{
  log(L"\n<b>Test supported Sercos signal function IDNs.</b>");

  TestContext context {};
  context.setSignalWordResponses(true);

  AxisProfileResult result = context.testClass.init(&context.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);

  FunctionSupportErrorInfo errorInfo {};
  SignalFunction homingStatusFunc {FunctionSupport::HOMING_STATUS_CYCLIC};
  bool isSupported = context.testClass.isFuncInSignalStatusWord(homingStatusFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing status should be supported from S-0-0403.0", true);
  LOG_TEST_RESULT(homingStatusFunc.statusBitOffset, L"homing status bit offset mismatch", 0);

  SignalFunction homingCommandFunc {FunctionSupport::HOMING_CMD_CYCLIC};
  isSupported = context.testClass.isFuncInSignalControlWord(homingCommandFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing command should be supported from S-0-0148.0", true);
  LOG_TEST_RESULT(homingCommandFunc.controlBitOffset, L"homing command bit offset mismatch", 0);

  SignalFunction clearDeviceErrorFunc {FunctionSupport::CLEAR_DEVERR_CMD_CYCLIC};
  isSupported = context.testClass.isSignalFuncSupported(clearDeviceErrorFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"clear device error should be supported from S-0-0099.0", true);
  LOG_TEST_RESULT(clearDeviceErrorFunc.statusBitOffset, L"clear device error status bit offset mismatch", 1);
  LOG_TEST_RESULT(clearDeviceErrorFunc.controlBitOffset, L"clear device error control bit offset mismatch", 1);

  return S_OK;
}

HRESULT TestSercosSignalFuncHandler::testSignalControlWordNotInMdt()
{
  log(L"\n<b>Test signal control word omitted from MDT.</b>");

  TestContext context {};
  context.setSignalWordResponses(false);

  AxisProfileResult result = context.testClass.init(&context.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);

  FunctionSupportErrorInfo errorInfo {};
  SignalFunction homingCommandFunc {FunctionSupport::HOMING_CMD_CYCLIC};
  bool isSupported = context.testClass.isFuncInSignalControlWord(homingCommandFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing command should not be supported when S-0-0145 is not in MDT", false);
  LOG_TEST_RESULT(errorInfo.errorType, L"unexpected error type", FuncSupportErrorType::ERROR_DEVICE_NOT_SUPPORT);

  return S_OK;
}

HRESULT TestSercosSignalFuncHandler::testSignalWordMappingRequired()
{
  log(L"\n<b>Test signal word mapping preconditions.</b>");

  TestContext context {};
  context.setSignalWordResponses(true);

  AxisProfileResult result = context.testClass.init(&context.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);

  FunctionSupportErrorInfo errorInfo {};
  auto& profileAssignedInfo = context.mappingHandler.getProfileAssignedInfo();

  profileAssignedInfo.m_input.m_signalStatusWord = false;
  SignalFunction homingStatusFunc {FunctionSupport::HOMING_STATUS_CYCLIC};
  bool isSupported = context.testClass.isFuncInSignalStatusWord(homingStatusFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing status should require signal status word mapping", false);
  LOG_TEST_RESULT(errorInfo.errorType, L"unexpected error type", FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT);

  profileAssignedInfo.m_input.m_signalStatusWord = true;
  profileAssignedInfo.m_output.m_signalControlWord = false;
  SignalFunction homingCommandFunc {FunctionSupport::HOMING_CMD_CYCLIC};
  isSupported = context.testClass.isFuncInSignalControlWord(homingCommandFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing command should require signal control word mapping", false);
  LOG_TEST_RESULT(errorInfo.errorType, L"unexpected error type", FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT);

  return S_OK;
}

HRESULT TestSercosSignalFuncHandler::testErrorHandling()
{
  log(L"\n<b>Test error handling for unsupported functions.</b>");
  TestContext context {};
  AxisProfileResult result = context.testClass.init(&context.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);

  return S_OK;
}
