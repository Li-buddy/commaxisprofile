#include "test_sercos_status_control.h"
#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/profile_trace.h"
#include "profiles/sercos/sercos_defs.h"
#include "profiles/sercos/drive/sercos_status_control_drive.h"

// Mockup
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_axisprofile_category.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestSercosStatusControl);
REGISTER_TEST_TO_GROUP(TestSercosStatusControl, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosStatusControl, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosStatusControl, Profiles_Sercos);


HRESULT TestSercosStatusControl::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Sercos profile status- / controlword class");
  log(L"\n----------------------------------------------------------------\n");

  testStatusword();
  testControlword();
  testSetDriveHalt();

  return S_OK;
}

HRESULT TestSercosStatusControl::testStatusword()
{
  log(L"\n<b>Test interpretation of the statusword.");

  TestCaseStatusword testcases[] =
  {
    { L"Statusword is zero (error)",                         0x0000, AxisProfileResult::STS_OK,   false,  false,  false, { false, false, false },  sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::UNDEFINED,   AxisProfilePowerMode::UNDEFINED },
    { L"PowerOff, Positioning, Supported",                   0x0080, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  true  },  sercos::OperationMode::PRIMARY ,   AxisProfileOperationMode::POSITIONING, AxisProfilePowerMode::POWER_OFF },
    { L"PowerOff, Positioning, Drive not config",            0x0080, AxisProfileResult::STS_OK,   false,  false,  false, { false, true,  true  },  sercos::OperationMode::PRIMARY ,   AxisProfileOperationMode::POSITIONING, AxisProfilePowerMode::POWER_OFF },
    { L"PowerOff, Positioning, Cyclic input missing",        0x0080, AxisProfileResult::STS_OK,   false,  false,  false, { true,  false, true  },  sercos::OperationMode::PRIMARY ,   AxisProfileOperationMode::POSITIONING, AxisProfilePowerMode::POWER_OFF },
    { L"PowerOff, Positioning, Cyclic output missing",       0x0080, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  false },  sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::POSITIONING, AxisProfilePowerMode::POWER_OFF },
    { L"PowerOn,  Positioning, Supported",                   0xC080, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  true  },  sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::POSITIONING, AxisProfilePowerMode::POWER_ON  },
    { L"PowerOn,  Positioning, Supported, Error",            0xE080, AxisProfileResult::STS_OK,   true,   false,  false, { true,  true,  true  },  sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::POSITIONING, AxisProfilePowerMode::POWER_ON  },
    { L"PowerOn,  Velocity, Supported, secondary1",          0xC180, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  true  },  sercos::OperationMode::SECONDARY1, AxisProfileOperationMode::VELOCITY,    AxisProfilePowerMode::POWER_ON  },
    { L"PowerOn,  Velocity, Supported, secondary4",          0xC400, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  true  },  sercos::OperationMode::SECONDARY4, AxisProfileOperationMode::VELOCITY,    AxisProfilePowerMode::POWER_ON  },
    { L"PowerOn,  Velocity, Supported, secondary5",          0xC500, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  true  },  sercos::OperationMode::SECONDARY5, AxisProfileOperationMode::VELOCITY,    AxisProfilePowerMode::POWER_ON  },
    { L"PowerOn,  Velocity, Supported, secondary6",          0xC600, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  true  },  sercos::OperationMode::SECONDARY6, AxisProfileOperationMode::VELOCITY,    AxisProfilePowerMode::POWER_ON  },
    { L"PowerOn,  Velocity, Supported, secondary7",          0xC700, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  true  },  sercos::OperationMode::SECONDARY7, AxisProfileOperationMode::VELOCITY,    AxisProfilePowerMode::POWER_ON  },
    { L"PowerOn,  Torque/force, Supported, secondary2",      0xC280, AxisProfileResult::STS_OK,   false,  false,  false, { true,  true,  true  },  sercos::OperationMode::SECONDARY2, AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT,AxisProfilePowerMode::POWER_ON  },
  };

  HRESULT hresult = S_OK;
  for ( auto &elem : testcases )
  {
    hresult |= runTestcaseStatusword(&elem);
  }
  return hresult;
}

HRESULT TestSercosStatusControl::runTestcaseStatusword(TestCaseStatusword *testcase)
{
  log(L"\n\tRun testcase: %ls", testcase->m_name);

  HRESULT hresult = S_OK;
  ProfileTrace traceHelper;
  AcknowledgeTelegram atData;
  MasterDataTelegram mdtData;

  test::MockAxisProfileTrace profileTraceMockup{};
  test::MockAxisProfileDeviceParam deviceParamMockup{};
  test::MockAxisProfileMappingHandler mappingHandlerMockup{};
  test::MockAxisProfileCommand commandIf {};
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler{ &profileTraceMockup });
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  atData.m_statusWord.m_word = testcase->m_statusWord;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testAxis";

  sercos::SercosStatusControlDrive testStatuswordInstance(profileBasicInfo, &atData, &mdtData);

  // register interface for sharing handler
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &commandIf);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  if ( !testcase->m_statusWord )
  {
    testStatuswordInstance.setSharingHandler(nullptr);
  }
  else
  {
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testStatuswordInstance);
  }

  comm::axisprofile::sercos::OpModeAvailInfoSercos opModeAvailable{ testcase->m_opModeSupported.m_opModeAvailable };
  opModeAvailable.setOpModeAvailInfoSercos(testcase->m_sercosOpMode, testcase->m_expectedOpMode);
  comm::axisprofile::ProfileAssignedInfo profileAssigned{ testcase->m_opModeSupported.m_opModeInputAssigned, testcase->m_opModeSupported.m_opModeOutputAssigned };
  deviceParamMockup.setReturnValue(&opModeAvailable);
  mappingHandlerMockup.setReturnValue(profileAssigned);
  testStatuswordInstance.updateOpModeSupportedInfo();

  AxisProfileResult result = testStatuswordInstance.analyzeStatusWord();
  if ( testcase->m_expectedResult != result )
  {
    error(L"Analyze of statusword does not return expected status. Expected: 0x%X, Actual: 0x%X", testcase->m_expectedResult, result);
    hresult = E_FAIL;
  }

  std::uint16_t actualStatusword = testStatuswordInstance.getStatusWord();
  if ( actualStatusword != testcase->m_statusWord )
  {
    error(L"Value of statusword is not as expected. Expected: %d, Actual: 0x%X", testcase->m_statusWord, actualStatusword);
    hresult = E_FAIL;
  }

  bool actualError = testStatuswordInstance.checkError();
  if ( actualError != testcase->m_expectedErrorValue )
  {
    error(L"Error state not as expected. Expected: %d, Actual:%d", testcase->m_expectedErrorValue, actualError);
    hresult = E_FAIL;
  }

  bool actualStandStill = testStatuswordInstance.cyclicStandStillCheck();
  if ( actualStandStill != testcase->m_expectedStandStill )
  {
    error(L"standstill state not as expected. Expected: %d, Actual: %d", testcase->m_expectedStandStill, actualStandStill);
    hresult = E_FAIL;
  }

  bool actualIsDeviceFollowsControl = testStatuswordInstance.isDeviceFollowsControl();
  if (actualIsDeviceFollowsControl != testcase->m_isDeviceFollowsControl)
  {
    error(L"Command value processing is not as expected. Expected: %d, Actual: %d", testcase->m_isDeviceFollowsControl, actualIsDeviceFollowsControl);
    hresult = E_FAIL;
  }

  bool actualOpModeCheck = testStatuswordInstance.checkOperationMode(testcase->m_expectedOpMode);
  AxisProfileOperationMode actualOpMode = testStatuswordInstance.getOperationMode();
  if ( (actualOpMode != testcase->m_expectedOpMode) || !actualOpModeCheck )
  {
    error(L"Actual opmode not as expected. Expected: %d, Actual: 0x%X", testcase->m_expectedOpMode, actualOpMode);
    hresult = E_FAIL;
  }

  comm::axisprofile::OpModeErrorInfo opModeErrorInfo{};
  bool actualOpModeSupportedCheck = testStatuswordInstance.checkOperationModeSupported(testcase->m_expectedOpMode, opModeErrorInfo);
  bool expectOpModeSupported = (testcase->m_opModeSupported.m_opModeAvailable && testcase->m_opModeSupported.m_opModeInputAssigned && testcase->m_opModeSupported.m_opModeOutputAssigned);
  if (actualOpModeSupportedCheck != expectOpModeSupported)
  {
    error(L"Actual opmode supported not as expected. Expected: %d, Actual:  %d", expectOpModeSupported, actualOpModeSupportedCheck);
    hresult = E_FAIL;
  }

  bool actualPowerCheck = testStatuswordInstance.checkPowerMode(testcase->m_expectedPowerMode);
  AxisProfilePowerMode actualPowerMode = testStatuswordInstance.getPowerMode();
  if ( ( actualPowerMode != testcase->m_expectedPowerMode ) || !actualPowerCheck )
  {
    error(L"Actual power mode not as expected. Expected: %d, Actual: 0x%X", testcase->m_expectedPowerMode, actualPowerMode);
    hresult = E_FAIL;
  }


  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}

HRESULT TestSercosStatusControl::testControlword()
{
  log(L"\n<b>Test interpretation of the controlword.");

  TestCaseControlword testcases[] =
  {
    { L"Set invalid operation mode.",               0xC080,     AxisProfileResult::ERR_NOK,                   {true,  true,  true },  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::UNDEFINED},   AxisProfileOperationMode::UNDEFINED,   false,  AxisProfilePowerMode::UNDEFINED },
    { L"Set position mode, primary.",               0xC080,     AxisProfileResult::STS_OK,                    {true,  true,  true },  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::POSITIONING}, AxisProfileOperationMode::POSITIONING, false,  AxisProfilePowerMode::UNDEFINED },
    { L"Set position mode, drive not configured.",  0xC080,     AxisProfileResult::ERR_NOK,                   {false, true,  true },  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::POSITIONING}, AxisProfileOperationMode::POSITIONING, false,  AxisProfilePowerMode::UNDEFINED },
    { L"Set position mode, cyclic data missing.",   0xC080,     AxisProfileResult::ERR_NOK,                   {true,  false, false},  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::POSITIONING}, AxisProfileOperationMode::POSITIONING, false,  AxisProfilePowerMode::UNDEFINED },
    { L"Set velocity mode, secondary1.",            0xC180,     AxisProfileResult::STS_OK,                    {true,  true,  true },  {sercos::OperationMode::SECONDARY1, AxisProfileOperationMode::VELOCITY},    AxisProfileOperationMode::VELOCITY,    false,  AxisProfilePowerMode::UNDEFINED },
    { L"Set velocity mode, secondary4, undefined.", 0xC400,     AxisProfileResult::ERR_NOK,                   {true,  true,  true },  {sercos::OperationMode::SECONDARY4, AxisProfileOperationMode::UNDEFINED},   AxisProfileOperationMode::VELOCITY,    false,  AxisProfilePowerMode::UNDEFINED },
    { L"Set invalid power mode",                    0x0080,     AxisProfileResult::ERR_NOK,                   {false, false, false},  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::UNDEFINED},   AxisProfileOperationMode::UNDEFINED,   true,   AxisProfilePowerMode::UNDEFINED },
    { L"Set power on, drive in bb",                 0x4080,     AxisProfileResult::ERR_WRONG_DEVICE_STATE,    {false, false, false},  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::UNDEFINED},   AxisProfileOperationMode::UNDEFINED,   true,   AxisProfilePowerMode::POWER_ON  },
    { L"Set power on, drive in Ab",                 0x8080,     AxisProfileResult::STS_OK,                    {false, false, false},  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::UNDEFINED},   AxisProfileOperationMode::UNDEFINED,   true,   AxisProfilePowerMode::POWER_ON  },
    { L"Set power off, drive in AF",                0xC088,     AxisProfileResult::STS_OK,                    {false, false, false},  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::UNDEFINED},   AxisProfileOperationMode::UNDEFINED,   true,   AxisProfilePowerMode::POWER_OFF },
    { L"Set power off, drive in Ab",                0x8080,     AxisProfileResult::STS_OK,                    {false, false, false},  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::UNDEFINED},   AxisProfileOperationMode::UNDEFINED,   true,   AxisProfilePowerMode::POWER_OFF },
    { L"Set power off torque free, drive in Ab",    0x8080,     AxisProfileResult::STS_OK,                    {false, false, false},  {sercos::OperationMode::PRIMARY,    AxisProfileOperationMode::UNDEFINED},   AxisProfileOperationMode::UNDEFINED,   true,   AxisProfilePowerMode::POWER_OFF_TORQUE_FREE },
  };

  HRESULT hresult = S_OK;
  for ( auto &elem : testcases )
  {
    hresult |= runTestcaseControlword(&elem);
  }

  ProfileBasicInfo profileInfo {};
  AcknowledgeTelegram atData {};
  MasterDataTelegram mdtData {};
  sercos::SercosStatusControlDrive confirmedModeControl(profileInfo, &atData, &mdtData);
  confirmedModeControl.confirmOperationModeFromDevice(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(confirmedModeControl.getOperationMode(), L"confirmed Sercos mode should be cached", AxisProfileOperationMode::VELOCITY);
  confirmedModeControl.confirmOperationModeFromDevice(AxisProfileOperationMode::UNDEFINED);
  LOG_TEST_RESULT(confirmedModeControl.getOperationMode(), L"undefined feedback must not erase the confirmed Sercos mode", AxisProfileOperationMode::VELOCITY);

  return hresult;
}

HRESULT TestSercosStatusControl::runTestcaseControlword(TestCaseControlword *testcase)
{
  log(L"\n\tRun testcase: %ls", testcase->m_name);

  bool testOpMode = testcase->m_opModeSupported.m_opModeAvailable || testcase->m_opModeSupported.m_opModeInputAssigned || testcase->m_opModeSupported.m_opModeOutputAssigned;
  if ( testOpMode && testcase->m_testPowerMode )
  {
    error(L"Only one method should be tested at a time.");
    return E_FAIL;
  }

  if ( !testOpMode && !testcase->m_testPowerMode )
  {
    warning(L"Nothing to test.");
    return S_OK;
  }

  HRESULT hresult = S_OK;
  ProfileTrace traceHelper;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;

  test::MockAxisProfileTrace profileTraceMockup{};
  test::MockAxisProfileDeviceParam deviceParamMockup{};
  test::MockAxisProfileMappingHandler mappingHandlerMockup{};
  test::MockAxisProfileCommand commandIf {};
  test::MockCategory mockCategory;
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  atData.m_statusWord.m_word = testcase->m_statusWord;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testAxis";

  sercos::SercosStatusControlDrive testControlwordInstance(profileBasicInfo, &atData, &mdtData);

  //register interface for sharing handler
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &commandIf);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
  if ( !testcase->m_statusWord )
  {
    testControlwordInstance.setSharingHandler(nullptr);
  }
  else
  {
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testControlwordInstance);
  }

  comm::axisprofile::sercos::OpModeAvailInfoSercos opModeAvailable{ testcase->m_opModeSupported.m_opModeAvailable };
  opModeAvailable.setOpModeAvailInfoSercos(testcase->m_opModePair.m_sercosOpMode, testcase->m_opModePair.m_profileOpMode);
  opModeAvailable.setOpModeAvailInfoSercos(testcase->m_opModePair.m_sercosOpMode, testcase->m_opModePair.m_profileOpMode);
  comm::axisprofile::ProfileAssignedInfo profileAssigned{ testcase->m_opModeSupported.m_opModeInputAssigned, testcase->m_opModeSupported.m_opModeOutputAssigned };
  deviceParamMockup.setReturnValue(&opModeAvailable);
  mappingHandlerMockup.setReturnValue(profileAssigned);
  testControlwordInstance.booting();

  AxisProfileResult result = testControlwordInstance.analyzeStatusWord();
  if ( AxisProfileResult::STS_OK !=  result )
  {
    error(L"Initial analyze of statusword failed.");
    return E_FAIL;
  }

  if ( testOpMode )
  {
    result = testControlwordInstance.setOperationMode(testcase->m_commandedOpMode);
  }
  else if ( testcase->m_testPowerMode )
  {
    result = testControlwordInstance.setPowerMode(testcase->m_commandedPowerMode);
  }

  if ( testcase->m_expectedResult != result )
  {
    error(L"Call of %ls did not return expected status. Expected: 0x%X Actual: 0x%X", testOpMode ? L"setOperationMode":L"setPowerMode", testcase->m_expectedResult, result);
    hresult = E_FAIL;
  }

  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}

HRESULT TestSercosStatusControl::testSetDriveHalt()
{
  /////////////////////////////////////////////////////Test Scenario/////////////////////////////////////////////////////
  log(L"\n<b>Test drive halt.</b>");
  struct DriveHaltTestCase
  {
    std::wstring m_testName;                //!< test name
    std::function<AxisProfileResult(sercos::SercosStatusControlDrive& statusCtrl)> m_testFun; 
    std::uint16_t m_statusWord;             //!< test value for statusword
    AxisProfileResult m_expectedResult;     //!< set expected return value
    AxisProfileResult m_cmdReturnValue;     //!< command return value
  };
  std::vector<DriveHaltTestCase> testCase {
    {L"Set drive halt success ",                [](sercos::SercosStatusControlDrive& statusCtrl){return statusCtrl.setDriveHaltMode(true);},                    0xC080, AxisProfileResult::STS_OK                , AxisProfileResult::STS_OK},
    {L"Get drive halt status",                  [](sercos::SercosStatusControlDrive& statusCtrl){bool isHalted; return statusCtrl.getDriveHaltMode(isHalted);}, 0x8080, AxisProfileResult::STS_OK                , AxisProfileResult::STS_OK},
    {L"Clear drive halt success ",              [](sercos::SercosStatusControlDrive& statusCtrl){return statusCtrl.setDriveHaltMode(false);},                   0xC080, AxisProfileResult::STS_OK                , AxisProfileResult::STS_OK},
    {L"Sharing handle not exist for get halt",  [](sercos::SercosStatusControlDrive& statusCtrl){bool isHalted; return statusCtrl.getDriveHaltMode(isHalted);}, 0x8080, AxisProfileResult::ERR_WRONG_DEVICE_STATE, AxisProfileResult::ERR_WRONG_DEVICE_STATE},

  };
  ////////////////////////////////////////////////////Test Implement/////////////////////////////////////////////////////
  ProfileTrace traceHelper;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  test::MockAxisProfileTrace profileTraceMockup{};
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileDeviceParam deviceParamMockup{};
  test::MockAxisProfileMappingHandler mappingHandlerMockup{};
  test::MockAxisProfileCommand commandIf {};
  profileBasicInfo.m_profileName = "testAxis";
  sercos::SercosStatusControlDrive testStatusControl(profileBasicInfo, &atData, &mdtData);
  
  //get drive halt command should not been called without sharing handle
  bool isHalted = false;
  auto result = testStatusControl.getDriveHaltMode(isHalted);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::ERR_EXEC_NULL);

  //register interface for sharing handler
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testStatusControl);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &commandIf);
  
  comm::axisprofile::sercos::OpModeAvailInfoSercos opModeAvailable{ true };
  opModeAvailable.setOpModeAvailInfoSercos(sercos::OperationMode::PRIMARY, AxisProfileOperationMode::POSITIONING);
  comm::axisprofile::ProfileAssignedInfo profileAssigned{ true, true };
  deviceParamMockup.setReturnValue(&opModeAvailable);
  mappingHandlerMockup.setReturnValue(profileAssigned);
  testStatusControl.updateOpModeSupportedInfo();

  for (const auto& oneTest : testCase)
  {
    log(L"\n  Test case of %s ...", oneTest.m_testName.c_str());
    atData.m_statusWord.m_word = oneTest.m_statusWord;
    testStatusControl.analyzeStatusWord();
    commandIf.setReturnValue(oneTest.m_cmdReturnValue);
    result = oneTest.m_testFun(testStatusControl);
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
  }
    
  return S_OK;
}
