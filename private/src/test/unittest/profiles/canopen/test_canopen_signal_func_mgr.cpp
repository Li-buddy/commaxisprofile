#include "test_canopen_signal_func_mgr.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenSignalFuncMgr);
REGISTER_TEST_TO_GROUP(TestCanopenSignalFuncMgr, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenSignalFuncMgr, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenSignalFuncMgr, Profiles_Canopen);

HRESULT TestCanopenSignalFuncMgr::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Canopen signal function manager class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testSupportedSignalFunctions();
  hresult |= testUnsupportedSignalFunctions();
  hresult |= testSignalWordMappingRequired();
  hresult |= testReadParamListUsesActualReadLength();
  hresult |= testConfigurationReadErrors();
  hresult |= testIndraDriveSkipsCtrlxSignalWordCfg();

  return hresult;
}

HRESULT TestCanopenSignalFuncMgr::testSupportedSignalFunctions()
{
  log(L"\n<b>Test supported Canopen signal function objects.</b>");

  TestContext context {};
  context.setSupportedSignalWordResponses();

  AxisProfileResult result = context.testClass.init(&context.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);

  FunctionSupportErrorInfo errorInfo {};
  SignalFunction homingStatusFunc {FunctionSupport::HOMING_STATUS_CYCLIC};
  bool isSupported = context.testClass.isFuncInSignalStatusWord(homingStatusFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing status should be supported from 0x2508:01", true);
  LOG_TEST_RESULT(homingStatusFunc.statusBitOffset, L"homing status bit offset mismatch", 1);

  TestContext commandContext {};
  commandContext.setControlWordResponses({signalWordParam(comm::axisprofile::canopen::SET_ABSOLUTE_POSITION_CONTROL),
                                          signalWordParam(comm::axisprofile::canopen::HOMING_DRIVE_CONTROLLED_CMD)},
                                         {0, 0});
  commandContext.setStatusWordResponses({signalWordParam(comm::axisprofile::canopen::HOMING_DRIVE_CONTROLLED_CMD),
                                         signalWordParam(comm::axisprofile::canopen::HOMING_FEEDBACK_VALUE_STATUS)},
                                        {0, 0});
  result = commandContext.testClass.init(&commandContext.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);

  errorInfo = {};
  SignalFunction homingCommandFunc {FunctionSupport::HOMING_CMD_CYCLIC};
  isSupported = commandContext.testClass.isSignalFuncSupported(homingCommandFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing command should be supported from 0x2501:01", true);
  LOG_TEST_RESULT(homingCommandFunc.statusBitOffset, L"homing command status bit offset mismatch", 0);
  LOG_TEST_RESULT(homingCommandFunc.controlBitOffset, L"homing command control bit offset mismatch", 1);

  using namespace comm::axisprofile::canopen;
  LOG_TEST_RESULT(context.fieldbusAdapter.getReadCount(context.getObj(SIGNAL_CONTROL_WORD_CONFIG_LIST)),
                  L"signal control word config list was not read",
                  1);
  LOG_TEST_RESULT(context.fieldbusAdapter.getReadCount(context.getObj(SIGNAL_CONTROL_WORD_ASSIGN_LIST)),
                  L"signal control word assign list was not read",
                  1);
  LOG_TEST_RESULT(context.fieldbusAdapter.getReadCount(context.getObj(SIGNAL_STATUS_WORD_CONFIG_LIST)),
                  L"signal status word config list was not read",
                  1);
  LOG_TEST_RESULT(context.fieldbusAdapter.getReadCount(context.getObj(SIGNAL_STATUS_WORD_ASSIGN_LIST)),
                  L"signal status word assign list was not read",
                  1);

  return S_OK;
}

HRESULT TestCanopenSignalFuncMgr::testUnsupportedSignalFunctions()
{
  log(L"\n<b>Test unsupported Canopen signal function objects.</b>");

  using namespace comm::axisprofile::canopen;

  TestContext wrongObjectContext {};
  wrongObjectContext.setControlWordResponses();
  wrongObjectContext.setStatusWordResponses({signalWordParam(STATUS_WORD_CTRLX_DRIVE)}, {0});

  AxisProfileResult result = wrongObjectContext.testClass.init(&wrongObjectContext.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);

  FunctionSupportErrorInfo errorInfo {};
  SignalFunction homingStatusFunc {FunctionSupport::HOMING_STATUS_CYCLIC};
  bool isSupported = wrongObjectContext.testClass.isFuncInSignalStatusWord(homingStatusFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing status should not be supported when 0x2508:01 is missing", false);
  LOG_TEST_RESULT(errorInfo.errorType, L"unexpected error type", FuncSupportErrorType::ERROR_DEVICE_NOT_SUPPORT);

  TestContext wrongBitContext {};
  wrongBitContext.setControlWordResponses();
  wrongBitContext.setStatusWordResponses({signalWordParam(HOMING_FEEDBACK_VALUE_STATUS)}, {1});

  result = wrongBitContext.testClass.init(&wrongBitContext.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);

  errorInfo = {};
  homingStatusFunc = {FunctionSupport::HOMING_STATUS_CYCLIC};
  isSupported = wrongBitContext.testClass.isFuncInSignalStatusWord(homingStatusFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing status should require assign bit 0", false);
  LOG_TEST_RESULT(errorInfo.errorType, L"unexpected error type", FuncSupportErrorType::ERROR_DEVICE_NOT_SUPPORT);

  errorInfo = {};
  SignalFunction homingCommandFunc {FunctionSupport::HOMING_CMD_CYCLIC};
  isSupported = wrongBitContext.testClass.isFuncInSignalControlWord(homingCommandFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"Canopen homing command should not be supported by wrong signal control word", false);
  LOG_TEST_RESULT(errorInfo.errorType, L"unexpected error type", FuncSupportErrorType::ERROR_DEVICE_NOT_SUPPORT);

  return S_OK;
}

HRESULT TestCanopenSignalFuncMgr::testSignalWordMappingRequired()
{
  log(L"\n<b>Test signal word mapping preconditions.</b>");

  TestContext context {};
  context.setSupportedSignalWordResponses();

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

HRESULT TestCanopenSignalFuncMgr::testReadParamListUsesActualReadLength()
{
  log(L"\n<b>Test CoE list read uses actual response length.</b>");

  using namespace comm::axisprofile::canopen;

  TestContext context {};
  const auto obj = context.getObj(SIGNAL_STATUS_WORD_CONFIG_LIST);
  const std::vector<std::uint32_t> expectedParams {signalWordParam(STATUS_WORD_CTRLX_DRIVE), 0U, signalWordParam(HOMING_FEEDBACK_VALUE_STATUS)};
  context.fieldbusAdapter.setListResponse<std::uint32_t>(obj, expectedParams);

  constexpr std::size_t maxCfgParams = 16;
  std::vector<std::uint8_t> rawBuffer(maxCfgParams * sizeof(std::uint32_t), 0);
  std::size_t readLen = 0;
  ParamReqDataCoE requestData {context.profileBasicInfo.m_deviceAddress, obj};
  requestData.maxLen = static_cast<std::uint32_t>(rawBuffer.size());

  AxisProfileResult result = context.fieldbusAdapter.readSingleParamSync(requestData, rawBuffer.data(), rawBuffer.size(), readLen);
  LOG_TEST_RESULT(result, L"read raw parameter list failed", AxisProfileResult::STS_OK);

  std::vector<std::uint32_t> outList {};
  result = FieldbusHelper::rawToParamList<std::uint32_t>(rawBuffer.data(), readLen, outList);
  LOG_TEST_RESULT(result, L"parse parameter list failed", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(outList.size(), L"read parameter list should only contain returned elements", expectedParams.size());
  LOG_TEST_RESULT_FLT_ARRY(outList.data(), L"unexpected parameter list values", expectedParams.data(), outList.size(), 1E-7);

  return S_OK;
}

HRESULT TestCanopenSignalFuncMgr::testConfigurationReadErrors()
{
  log(L"\n<b>Test signal word configuration read errors.</b>");

  using namespace comm::axisprofile::canopen;

  TestContext missingStatusAssignContext {};
  missingStatusAssignContext.setControlWordResponses();
  missingStatusAssignContext.fieldbusAdapter.setListResponse<std::uint32_t>(missingStatusAssignContext.getObj(SIGNAL_STATUS_WORD_CONFIG_LIST),
                                                                            {signalWordParam(HOMING_FEEDBACK_VALUE_STATUS)});

  AxisProfileResult result = missingStatusAssignContext.testClass.init(&missingStatusAssignContext.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(missingStatusAssignContext.fieldbusAdapter.getReadCount(missingStatusAssignContext.getObj(SIGNAL_STATUS_WORD_ASSIGN_LIST)),
                  L"signal status word assign list was not read",
                  1);

  FunctionSupportErrorInfo errorInfo {};
  SignalFunction homingStatusFunc {FunctionSupport::HOMING_STATUS_CYCLIC};
  bool isSupported = missingStatusAssignContext.testClass.isFuncInSignalStatusWord(homingStatusFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing status should stay unsupported when assign list read fails", false);
  LOG_TEST_RESULT(errorInfo.errorType, L"unexpected error type", FuncSupportErrorType::ERROR_DEVICE_NOT_SUPPORT);

  TestContext missingControlAssignContext {};
  missingControlAssignContext.fieldbusAdapter.setListResponse<std::uint32_t>(missingControlAssignContext.getObj(SIGNAL_CONTROL_WORD_CONFIG_LIST),
                                                                             {signalWordParam(SET_ABSOLUTE_POSITION_CONTROL)});
  missingControlAssignContext.setStatusWordResponses({signalWordParam(HOMING_FEEDBACK_VALUE_STATUS)}, {0});

  result = missingControlAssignContext.testClass.init(&missingControlAssignContext.sharingHandler);
  LOG_TEST_RESULT(result, L"signal function manager init failed", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(missingControlAssignContext.fieldbusAdapter.getReadCount(missingControlAssignContext.getObj(SIGNAL_CONTROL_WORD_ASSIGN_LIST)),
                  L"signal control word assign list was not read",
                  1);

  errorInfo = {};
  homingStatusFunc = {FunctionSupport::HOMING_STATUS_CYCLIC};
  isSupported = missingControlAssignContext.testClass.isFuncInSignalStatusWord(homingStatusFunc, errorInfo);
  LOG_TEST_RESULT(isSupported, L"homing status should still use status word data when control word read fails", true);

  return S_OK;
}

HRESULT TestCanopenSignalFuncMgr::testIndraDriveSkipsCtrlxSignalWordCfg()
{
  log(L"\n<b>Test IndraDrive skips ctrlX DRIVE signal word configuration.</b>");

  using namespace comm::axisprofile::canopen;

  TestContext context {};

  context.property.setDeviceSeries(DeviceSeries::INDRADRIVE);

  // Intentionally don't provide responses for the ctrlX DRIVE
  // signal word configuration objects.
  AxisProfileResult result = context.testClass.init(&context.sharingHandler);

  LOG_TEST_RESULT(result, L"signal function manager init should succeed for IndraDrive", AxisProfileResult::STS_OK);

  LOG_TEST_RESULT(context.fieldbusAdapter.getReadCount(context.getObj(SIGNAL_CONTROL_WORD_CONFIG_LIST)),
                  L"IndraDrive should not read ctrlX signal control word config list",
                  0);

  LOG_TEST_RESULT(context.fieldbusAdapter.getReadCount(context.getObj(SIGNAL_CONTROL_WORD_ASSIGN_LIST)),
                  L"IndraDrive should not read ctrlX signal control word assign list",
                  0);

  LOG_TEST_RESULT(context.fieldbusAdapter.getReadCount(context.getObj(SIGNAL_STATUS_WORD_CONFIG_LIST)),
                  L"IndraDrive should not read ctrlX signal status word config list",
                  0);

  LOG_TEST_RESULT(context.fieldbusAdapter.getReadCount(context.getObj(SIGNAL_STATUS_WORD_ASSIGN_LIST)),
                  L"IndraDrive should not read ctrlX signal status word assign list",
                  0);

  return S_OK;
}
