#include "comm/axisprofile/interfaces/i_axisprofile_status_control_c.h"
#include "test_profile_status_control.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProfileStatusControl);
REGISTER_TEST_TO_GROUP(TestProfileStatusControl, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileStatusControl, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileStatusControl, Profiles);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProfileStatusControl::TestProfileStatusControl()
{
}


TestProfileStatusControl::~TestProfileStatusControl(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProfileStatusControl::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the base class for profile status control.");
  log(L"\n----------------------------------------------------------------\n");
  
  hResult |= testCInterface();

  return hResult;
}


HRESULT TestProfileStatusControl::testCInterface()
{
  HRESULT hResult = S_OK;
  test::MockAxisProfileTrace profileTraceMock{};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testAxis";

  AxisProfileStatusControlUnittest testClass(profileBasicInfo);
  log(L"\n<b>Test of the status control C-Interface</b>");

  log(L"\nCall getCInterface()...");
  const IAxisProfileStatusControlC* statusControlItf = testClass.getCInterface();
  if (!statusControlItf)
  {
    error(L"\nCould not get the C-Interface (received nullptr)!");
    return E_FAIL;
  }

  log(L"\nCall AXISPROFILE_getStatusWord() with nullptr handle...");
  statusControlItf->getStatusWord(nullptr);

  log(L"\nCall AXISPROFILE_getStatusWord()...");
  std::uint16_t statusWord = statusControlItf->getStatusWord(statusControlItf->handle);
  if (statusWord != testClass.m_testStatusWord)
  {
    error(L"Received wrong status word! Expected: 0x%04X, Actual: 0x%04X", testClass.m_testStatusWord, statusWord);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_checkOperationMode() with nullptr handle...");
  AxisProfileOperationMode opMode{};
  statusControlItf->checkOperationMode(nullptr, opMode);

  log(L"\nCall AXISPROFILE_checkOperationMode()...");
  statusControlItf->checkOperationMode(statusControlItf->handle, opMode);

  OpModeErrorInfo opModeErrorInfo{};
  log(L"\nCall AXISPROFILE_checkOperationModeSupported() with nullptr handle...");
  statusControlItf->checkOperationModeSupported(nullptr, opMode, opModeErrorInfo);

  log(L"\nCall AXISPROFILE_checkOperationModeSupported()...");
  statusControlItf->checkOperationModeSupported(statusControlItf->handle, opMode, opModeErrorInfo);

  log(L"\nCall AXISPROFILE_getOperationMode() with nullptr handle...");
  statusControlItf->getOperationMode(nullptr);

  log(L"\nCall AXISPROFILE_getOperationMode()...");
  statusControlItf->getOperationMode(statusControlItf->handle);

  log(L"\nCall AXISPROFILE_checkPowerMode() with nullptr handle...");
  AxisProfilePowerMode powerMode{};
  statusControlItf->checkPowerMode(nullptr, powerMode);

  log(L"\nCall AXISPROFILE_checkPowerMode()...");
  statusControlItf->checkPowerMode(statusControlItf->handle, powerMode);

  log(L"\nCall AXISPROFILE_getPowerMode() with nullptr handle...");
  statusControlItf->getPowerMode(nullptr);

  log(L"\nCall AXISPROFILE_getPowerMode()...");
  statusControlItf->getPowerMode(statusControlItf->handle);

  log(L"\nCall AXISPROFILE_checkError() with nullptr handle...");
  statusControlItf->checkError(nullptr);

  log(L"\nCall AXISPROFILE_checkError()...");
  statusControlItf->checkError(statusControlItf->handle);

  log(L"\nCall AXISPROFILE_cyclicStandStillCheck() with nullptr handle...");
  statusControlItf->cyclicStandStillCheck(nullptr);

  log(L"\nCall AXISPROFILE_cyclicStandStillCheck()...");
  statusControlItf->cyclicStandStillCheck(statusControlItf->handle);

  log(L"\nCall AXISPROFILE_setControlWord() with nullptr handle...");
  std::uint16_t controlWord = 0x1234;
  statusControlItf->setControlWord(nullptr, controlWord);

  log(L"\nCall AXISPROFILE_setControlWord()...");
  statusControlItf->setControlWord(statusControlItf->handle, controlWord);

  log(L"\nCall AXISPROFILE_setOperationMode() with nullptr handle...");
  statusControlItf->setOperationMode(nullptr, opMode);

  log(L"\nCall AXISPROFILE_setOperationMode()...");
  statusControlItf->setOperationMode(statusControlItf->handle, opMode);

  log(L"\nCall AXISPROFILE_setPowerMode() with nullptr handle...");
  statusControlItf->setPowerMode(nullptr, powerMode);

  log(L"\nCall AXISPROFILE_setPowerMode()...");
  statusControlItf->setPowerMode(statusControlItf->handle, powerMode);

  log(L"\nCall AXISPROFILE_isDeviceFollowsControl() with nullptr handle...");
  statusControlItf->isDeviceFollowsControl(nullptr);

  log(L"\nCall AXISPROFILE_isDeviceFollowsControl()...");
  statusControlItf->isDeviceFollowsControl(statusControlItf->handle);

  log(L"\nCall AXISPROFILE_setDriveHaltMode() with nullptr handle...");
  statusControlItf->setDriveHaltMode(nullptr, true);

  log(L"\nCall AXISPROFILE_setDriveHaltMode()...");
  statusControlItf->setDriveHaltMode(statusControlItf->handle, true);

  log(L"\nCall AXISPROFILE_getDriveHaltMode() with nullptr handle...");
  bool isHaltEnabled = true;
  statusControlItf->getDriveHaltMode(nullptr, isHaltEnabled);

  log(L"\nCall AXISPROFILE_getDriveHaltMode()...");
  statusControlItf->getDriveHaltMode(statusControlItf->handle, isHaltEnabled);

  log(L"\nCall AXISPROFILE_deviceHasWarning() with nullptr handle...");
  statusControlItf->deviceHasWarning(nullptr);

  log(L"\nCall AXISPROFILE_deviceHasWarning()...");
  statusControlItf->deviceHasWarning(statusControlItf->handle);

  log(L"\nCall AXISPROFILE_checkReadyToOperate() with nullptr handle...");
  statusControlItf->checkReadyToOperate(nullptr);

  log(L"\nCall AXISPROFILE_checkReadyToOperate()...");
  statusControlItf->checkReadyToOperate(statusControlItf->handle);

  log(L"\n");
  return hResult;
}
