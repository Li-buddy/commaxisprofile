#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/interfaces/i_axisprofile_command_c.h"
#include "helper/result_helper.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_scaling.h"
#include "test_profile_command.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "mockup/mock_axisprofile_cmd.h"
#include "mockup/mock_axisprofile_device_param.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestProfileCommand);
REGISTER_TEST_TO_GROUP(TestProfileCommand, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileCommand, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileCommand, Profiles);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProfileCommand::TestProfileCommand()
{
}


TestProfileCommand::~TestProfileCommand(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProfileCommand::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the base class for all profile commands.");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testCInterface();

  return hResult;
}


HRESULT TestProfileCommand::testCInterface()
{
  HRESULT hResult = S_OK;
  test::MockAxisProfileTrace profileTraceMock{};
  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = m_deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = nullptr;
  profileBasicInfo.m_profileName = "testProfile";

  AxisProfileCommandUnittest testClass(profileBasicInfo);
  AxisProfileSharingHandler sharingHandler(nullptr);
  testClass.setSharingHandler(&sharingHandler);

  log(L"\n<b>Test of the command C-Interface</b>");
  log(L"\nCall getCInterface()...");
  const IAxisProfileCommandC* commandItf = testClass.getCInterface();
  if (!commandItf)
  {
    error(L"\nCould not get the C-Interface (received nullptr)!");
    return E_FAIL;
  }

  log(L"\nCall AXISPROFILE_clearDeviceError() with nullptr handle...");
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult aprResult = commandItf->clearDeviceError(nullptr);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_clearDeviceError()...");
  aprResult = commandItf->clearDeviceError(commandItf->handle);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  const comm::axisprofile::AxisProfilePositiveStop mode = AxisProfilePositiveStop::ENABLE;
  log(L"\nCall AXISPROFILE_setAndCheckPositiveStop(const comm::axisprofile::AxisProfilePositiveStop mode)...");
  aprResult = commandItf->setAndCheckPositiveStop(commandItf->handle, mode);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->setAndCheckPositiveStop(nullptr, mode);
  log(L"\nCall AXISPROFILE_setAndCheckPositiveStop(comm::axisprofile::AxisProfilePositiveStop mode) with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  double value = 10.0;
  log(L"\nCall AXISPROFILE_setAndCheckClampingTorque(const double clampingTorque, const AxisProfileDeviceScalingType scalingType, const bool metrication)...");
  aprResult = commandItf->setAndCheckClampingTorque(commandItf->handle, value, true);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->setAndCheckClampingTorque(nullptr, value, true);
  log(L"\nCall AXISPROFILE_setAndCheckClampingTorque(const double clampingTorque, const AxisProfileDeviceScalingType scalingType, const bool metrication) with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  double currentValue;
  log(L"\nCall AXISPROFILE_getClampingTorque(double* const clampingTorque, const AxisProfileDeviceScalingType scalingType, const bool metrication)...");
  aprResult = commandItf->getClampingTorque(commandItf->handle, &currentValue, true);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->getClampingTorque(nullptr, &currentValue, true);
  log(L"\nCall AXISPROFILE_getClampingTorque(double *clampingTorque, const AxisProfileDeviceScalingType scalingType, const bool metrication) with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  bool standStill;
  aprResult = commandItf->checkStandStill(commandItf->handle, &standStill);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->checkStandStill(nullptr, &standStill);
  log(L"\nCall AXISPROFILE_checkStandStill(bool* const standStill) with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  double standStillWindow  = 100.0;
  aprResult = commandItf->setAndCheckStandStillWindow(commandItf->handle, standStillWindow);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->setAndCheckStandStillWindow(nullptr, standStillWindow);
  log(L"\nCall setAndCheckStandStillWindow(const double standStillWindow) with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }


  double currentStandStillWindow;
  aprResult = commandItf->getStandStillWindow(commandItf->handle, &currentStandStillWindow);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->getStandStillWindow(nullptr, &currentStandStillWindow);
  log(L"\nCall getStandStillWindow(double* const standStillWindow) with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->activateCyclicStandStillCheck(commandItf->handle);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->activateCyclicStandStillCheck(nullptr);
  log(L"\nCall activateCyclicStandStillCheck() with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->deactivateCyclicStandStillCheck(commandItf->handle);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->deactivateCyclicStandStillCheck(nullptr);
  log(L"\nCall deactivateCyclicStandStillCheck() with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->setAbsolutePosition(commandItf->handle, 10.0, comm::axisprofile::AxisProfileEncoder::ENCODER_1, true);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->setAbsolutePosition(nullptr, 10.0, comm::axisprofile::AxisProfileEncoder::ENCODER_1, true);
  log(L"\nCall setAbsolutePosition() with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->resetAbsolutePosition(commandItf->handle);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->resetAbsolutePosition(nullptr);
  log(L"\nCall resetAbsolutePosition() with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  double refPos{};
  aprResult = commandItf->referencingAxis(commandItf->handle, refPos, comm::axisprofile::AxisProfileEncoder::ENCODER_1);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->referencingAxis(nullptr, refPos, comm::axisprofile::AxisProfileEncoder::ENCODER_1);
  log(L"\nCall referencingAxis() with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->resetReferencingAxis(commandItf->handle);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  aprResult = commandItf->resetReferencingAxis(nullptr);
  log(L"\nCall resetReferencingAxis() with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  test::MockAxisProfileDeviceParam mockDevParam{};
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &mockDevParam);

  bool isHomed = false;
  aprResult = commandItf->checkAxisHomed(commandItf->handle, isHomed);
  log(L"\nCall AXISPROFILE_checkAxisHomed with handle...");
  LOG_TEST_RESULT(aprResult, L"Unexpected return value!", AxisProfileResult::ERR_NOTIMPLEMENTED);

  aprResult = commandItf->checkAxisHomed(nullptr, isHomed);
  log(L"\nCall AXISPROFILE_checkAxisHomed with nullptr handle...");
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }
  log(L"\n");

  log(L"\nCall resetAxisProfileCommandState() with normal case...");
  aprResult = commandItf->resetAxisProfileCommandState(commandItf->handle, AxisProfileCommandType::CMD_MIN);
  LOG_TEST_RESULT(aprResult, L"Return OK!", AxisProfileResult::STS_OK);

  log(L"\nCall resetAxisProfileCommandState() with unexpected command type...");
  aprResult = commandItf->resetAxisProfileCommandState(commandItf->handle, static_cast<AxisProfileCommandType>(-1));
  LOG_TEST_RESULT(aprResult, L"Unexpected return value!", AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND);

  log(L"\nCall resetAxisProfileCommandState() with nullptr handle...");
  aprResult = commandItf->resetAxisProfileCommandState(nullptr, AxisProfileCommandType::CMD_MIN);
  LOG_TEST_RESULT(aprResult, L"Unexpected return value!", AxisProfileResult::ERR_EXEC_NULL);
  log(L"\n");


  log(L"\nCall abortAxisProfileCommand() with normal case...");
  aprResult = commandItf->abortAxisProfileCommand(commandItf->handle, comm::axisprofile::AxisProfileCommandType::CMD_MIN);
  LOG_TEST_RESULT(aprResult, L"Return OK!", AxisProfileResult::STS_OK);

  log(L"\nCall abortAxisProfileCommand() with unexpected command type...");
  aprResult = commandItf->abortAxisProfileCommand(commandItf->handle, static_cast<comm::axisprofile::AxisProfileCommandType>(-1));
  LOG_TEST_RESULT(aprResult, L"Unexpected return value!", AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND);

  log(L"\nCall abortAxisProfileCommand() with nullptr handle...");
  aprResult = commandItf->abortAxisProfileCommand(nullptr, comm::axisprofile::AxisProfileCommandType::CMD_MIN);
  LOG_TEST_RESULT(aprResult, L"Unexpected return value!", AxisProfileResult::ERR_EXEC_NULL);
  log(L"\n");


  log(L"\nCall getAxisProfileCommandState() with normal case...");
  AxisprofileCommandState state = commandItf->getAxisProfileCommandState(commandItf->handle, comm::axisprofile::AxisProfileCommandType::CMD_MIN);
  LOG_TEST_RESULT(state, L"Unexpected return value!", AxisprofileCommandState::CMD_STATE_IDLE);

  log(L"\nCall getAxisProfileCommandState() with unexpected command type...");
  state = commandItf->getAxisProfileCommandState(commandItf->handle, static_cast<comm::axisprofile::AxisProfileCommandType>(-1));
  LOG_TEST_RESULT(state, L"Unexpected return value!", AxisprofileCommandState::CMD_STATE_UNDEFINED);

  log(L"\nCall getAxisProfileCommandState() with nullptr handle...");
  state = commandItf->getAxisProfileCommandState(nullptr, comm::axisprofile::AxisProfileCommandType::CMD_MIN);
  LOG_TEST_RESULT(state, L"Unexpected return value!", AxisprofileCommandState::CMD_STATE_UNDEFINED);
  log(L"\n");

  log(L"\nCall setReferencingPosition() with nullptr handle...");
  aprResult = commandItf->setReferencingPosition(nullptr, refPos, comm::axisprofile::AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_DEVICE);
  LOG_TEST_RESULT(aprResult, L"Unexpected return value!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall setReferencingPosition() with  handle...");
  aprResult = commandItf->setReferencingPosition(commandItf->handle, refPos, comm::axisprofile::AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_DEVICE);
  LOG_TEST_RESULT(aprResult, L"Return OK!", AxisProfileResult::STS_OK);
  log(L"\n");

  test::MockAxisProfileCmd mockCmd {};
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CMD, &mockCmd);

  log(L"\nCall setReferencingPosition() with  handle...");
  std::vector<AxisProfileCommandType> vecCmdType;
  aprResult = commandItf->getCommandsAtDeterminedState(commandItf->handle, AxisprofileCommandState::CMD_STATE_IDLE, vecCmdType);
  LOG_TEST_RESULT(aprResult, L"Return OK!", AxisProfileResult::STS_OK);

  log(L"\nCall getAxisProfileCommandState() with nullptr handle...");
  aprResult = commandItf->getCommandsAtDeterminedState(nullptr, AxisprofileCommandState::CMD_STATE_IDLE, vecCmdType);
  LOG_TEST_RESULT(aprResult, L"Unexpected return value!", AxisProfileResult::ERR_EXEC_NULL);
  log(L"\n");

  return hResult;
}
