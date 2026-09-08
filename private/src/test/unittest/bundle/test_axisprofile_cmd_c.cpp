
#include "bundle/axisprofile_cmd_c.h"
#include "helper/result_helper.h"
#include "mockup/mock_axisprofile_cmd.h"
#include "test_axisprofile_cmd_c.h"
#include <functional>
#include <memory>

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestAxisprofileCmdC);
REGISTER_TEST_TO_GROUP(TestAxisprofileCmdC, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisprofileCmdC, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisprofileCmdC, BundleIfTest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestAxisprofileCmdC::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestAxisprofileCmdC");
  log(L"\n----------------------------------------------------------------\n");

  res |= testCmdInvoking();
  res |= testCmdHandling();

  return res;
}

HRESULT TestAxisprofileCmdC::testCmdInvoking()
{
  log(L"\nTest axisprofile commands invoking...");

  /// @brief Test case structure
  struct TestCase
  {
    CmdType cmdType;            // Command type
    std::function<AxisProfileResult(void* handle, CmdType& cmdType)> testFunc; // Test function
  };

  std::vector<TestCase> testData = {
    // command type                       test function
    { CmdType::CLEAR_DEV_ERR,             [&](void* handle, CmdType& cmdType) { return AXISPROFILE_clearDeviceErrorCmd(handle, cmdType); }},
    { CmdType::SET_ABS_POS,               [&](void* handle, CmdType& cmdType) { return AXISPROFILE_setAbsolutePositionCmd(handle, 0.0, AxisProfileEncoder::ENCODER_1, cmdType); }},
    { CmdType::REFERENCE_POS,             [&](void* handle, CmdType& cmdType) { return AXISPROFILE_referencingAxisCmd(handle, 0.0, AxisProfileEncoder::ENCODER_1, cmdType); }},
    { CmdType::FIXED_STOP,                [&](void* handle, CmdType& cmdType) { return AXISPROFILE_fixedStopCmd(handle, 0.0, cmdType); }},
    { CmdType::CLEAR_DEV_ERR,             [&](void* handle, CmdType& /*cmdType*/) { return AXISPROFILE_clearDeviceError2Cmd(handle); }},
    { CmdType::SET_NEW_REF_WITHOUT_MOVE,  [&](void* handle, CmdType& cmdType) { return AXISPROFILE_setNewReferenceWithoutMovement(handle, 0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_PROFILE, cmdType); }},
  };
  
  std::unique_ptr<MockAxisProfileCmd> mockCmd = std::make_unique<MockAxisProfileCmd>();
  for (const auto& it : testData)
  {
    CmdType cmdType {};

    log(L"\n\tTest success case of command: %ls...", UTF8TOWIDE(cmdTypeToString(it.cmdType)));
    AxisProfileResult res = it.testFunc(mockCmd.get(), cmdType);
    LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::STS_OK);

    log(L"\n\tTest nullptr handle case of %ls...", UTF8TOWIDE(cmdTypeToString(it.cmdType)));
    res = it.testFunc(nullptr, cmdType);
    LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::ERR_EXEC_NULL);
  }

  return S_OK;
}

HRESULT TestAxisprofileCmdC::testCmdHandling()
{
  log(L"\nTest axisprofile commands handling...");
  std::unique_ptr<MockAxisProfileCmd> mockCmd = std::make_unique<MockAxisProfileCmd>();

  log(L"\n\tTest success case of getCmdState...");
  for (std::int16_t expState = -1; expState <= STATE_MAX + 1; expState++)
  {
    mockCmd->m_cmdState = static_cast<CmdState>(expState);
    log(L"\n\tThe state should be %ls...", UTF8TOWIDE(cmdStateToString(mockCmd->m_cmdState)));
    CmdState actState = AXISPROFILE_getCmdState(mockCmd.get(), CmdType::CLEAR_DEV_ERR);
    RETURN_TEST_STATE(static_cast<CmdState>(expState), L"\nUnexpected state", actState, cmdStateToString);
  }

  log(L"\n\tTest nullptr handle case of getCmdState...");
  CmdState actState = AXISPROFILE_getCmdState(nullptr, CmdType::CLEAR_DEV_ERR);
  RETURN_TEST_STATE(CmdState::STATE_UNKNOWN, L"\nUnexpected state", actState, cmdStateToString);

  log(L"\n\tTest success case of getCmdStatusInDevice...");
  CmdStatusInDevice expStatus = CmdStatusInDevice::DEVICE_NOT_SET;
  mockCmd->m_cmdStatusInDevice = expStatus;
  auto actStatus = AXISPROFILE_getCmdStatusInDevice(mockCmd.get(), CmdType::CLEAR_DEV_ERR);
  RETURN_TEST_RESULT(actStatus, L"\nUnexpected state", expStatus);

  log(L"\n\tTest nullptr handle case of AXISPROFILE_getCmdStatusInDevice...");
  actStatus = AXISPROFILE_getCmdStatusInDevice(nullptr, CmdType::CLEAR_DEV_ERR);
  RETURN_TEST_RESULT(actStatus, L"\nUnexpected state", CmdStatusInDevice::DEVICE_UNKNOWN);

  log(L"\n\tTest success case of AXISPROFILE_getOperationModeCmd...");
  mockCmd->m_operationMode = AxisProfileOperationMode::VELOCITY;
  auto actOperationMode = AXISPROFILE_getOperationModeCmd(mockCmd.get());
  RETURN_TEST_RESULT(actOperationMode, L"\nUnexpected operation mode", mockCmd->m_operationMode);

  log(L"\n\tTest nullptr handle case of AXISPROFILE_getOperationModeCmd...");
  actOperationMode = AXISPROFILE_getOperationModeCmd(nullptr);
  RETURN_TEST_RESULT(actOperationMode, L"\nUnexpected operation mode", AxisProfileOperationMode::UNDEFINED);

  log(L"\n\tTest success case of AXISPROFILE_setOperationModeCmd...");
  AxisProfileResult operationModeResult = AXISPROFILE_setOperationModeCmd(mockCmd.get(), AxisProfileOperationMode::POSITIONING);
  LOG_TEST_RESULT(operationModeResult, L"\nUnexpected return value!", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT(mockCmd->m_operationMode, L"\nUnexpected operation mode", AxisProfileOperationMode::POSITIONING);

  log(L"\n\tTest nullptr handle case of AXISPROFILE_setOperationModeCmd...");
  operationModeResult = AXISPROFILE_setOperationModeCmd(nullptr, AxisProfileOperationMode::POSITIONING);
  LOG_TEST_RESULT(operationModeResult, L"\nUnexpected return value!", AxisProfileResult::ERR_EXEC_NULL);

  /// @brief Test case structure
  struct TestCase
  {
    const std::wstring name;  ///< test name
    std::function<AxisProfileResult(void* handle, CmdType cmdType)> testFunc; ///< Test function
  };

  std::vector<TestCase> testData = {
    // handling name      test function
    {L"reset command",    [&](void* handle, CmdType cmdType) { return AXISPROFILE_resetCmd(handle, cmdType); }},
    {L"abort command",    [&](void* handle, CmdType cmdType) { return AXISPROFILE_abortCmd(handle, cmdType); }},
  };

  for (const auto& it : testData)
  {
    CmdType cmdType {};

    log(L"\n\tTest success case of: %ls...", it.name.c_str());
    AxisProfileResult res = it.testFunc(mockCmd.get(), cmdType);
    LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::STS_OK);

    log(L"\n\tTest nullptr handle case of: %ls...", it.name.c_str());
    res = it.testFunc(nullptr, cmdType);
    LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::ERR_EXEC_NULL);
  }

  return S_OK;
}
