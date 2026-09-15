#include "helper/result_helper.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/canopen/canopen_param_access.h"
#include "profiles/profile_trace.h"
#include "profiles/canopen/drive/canopen_scaling_strategy_control.h"
#include "profiles/canopen/drive/canopen_status_control_drive.h"
#include "test_canopen_status_control.h"

// Mockups
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_profile_trace.h"
#include <thread>
#include <vector>

using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenStatusControl);
REGISTER_TEST_TO_GROUP(TestCanopenStatusControl, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenStatusControl, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenStatusControl, Profiles_Canopen);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestCanopenStatusControl::TestCanopenStatusControl()
{
}

TestCanopenStatusControl::~TestCanopenStatusControl(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestCanopenStatusControl::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the CiA402 profile status- / controlword class");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testStatusword();
  hResult |= testSetOperationMode();
  hResult |= testSetPowerMode();
  hResult |= testSetDriveHalt(); 
  hResult |= testCheckDeviceFollowsControl();
  hResult |= testCheckOperationModeSupported();
  hResult |= testDetermineCanopenOpMode();
  hResult |= testMics();

  return hResult;
}

HRESULT TestCanopenStatusControl::testStatusword()
{
  log(L"\n<b>Test interpretation of the statusword.");

  TestCaseStatusword testcases[] =
  {
    {L"checkError",            0x8,    true,  AxisProfileResult::STS_OK},
    {L"cyclicStandStillCheck", 0xC080, false, AxisProfileResult::STS_OK},
  };

  HRESULT hresult = S_OK;
  for (auto &elem : testcases)
  {
    hresult |= runTestcaseStatusword(&elem);
  }
  ProfileTrace traceHelper{};
  test::MockFieldbusAdapterCoE mockFieldbusAdapter{};
  std::uint32_t deviceAddress = 1;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  canopen::CanopenStatusControlDrive testStatusword(profileBasicInfo, nullptr, nullptr);
  if (comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL != testStatusword.analyzeStatusWord())
  {
    error(L"Analyze of statusword does not return expected status");
    hresult |= E_FAIL;
  }

  return hresult;
}

HRESULT TestCanopenStatusControl::runTestcaseStatusword(TestCaseStatusword *testcase)const
{
  log(L"\n\tRun testcase: %ls", testcase->m_name);

  HRESULT hresult = S_OK;
  ProfileTrace traceHelper{};
  AcknowledgeTelegram atData{};
  MasterDataTelegram mdtData{};
  atData.m_statusWord.m_word = testcase->m_statusWord;
  test::MockFieldbusAdapterCoE mockFieldbusAdapter{};
  std::uint32_t deviceAddress = 1;
  atData.m_atData.m_opModeDisplay = 8;
  mdtData.m_mdtData.m_opMode = 8;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  canopen::CanopenStatusControlDrive testStatuswordInstance(profileBasicInfo , &atData, &mdtData);

  AxisProfileResult result = testStatuswordInstance.analyzeStatusWord();
  if (testcase->m_expectedResult != result)
  {
    error(L"Analyze of statusword does not return expected status. Expected: 0x%X, Actual: 0x%X", testcase->m_expectedResult, result);
    hresult = E_FAIL;
  }

  std::uint16_t actualStatusword = testStatuswordInstance.getStatusWord();
  if (actualStatusword != testcase->m_statusWord)
  {
    error(L"Value of statusword is not as expected. Expected: %d, Actual: 0x%X", testcase->m_statusWord, actualStatusword);
    hresult = E_FAIL;
  }

  bool actualError = testStatuswordInstance.checkError();
  if (actualError != testcase->m_expectedErrorValue)
  {
    error(L"Error state not as expected. Expected: %d, Actual: %d", testcase->m_expectedErrorValue, actualError);
    hresult = E_FAIL;
  }

  (void)testStatuswordInstance.cyclicStandStillCheck();
  (void)testStatuswordInstance.setControlWord(1);
  testStatuswordInstance.checkReadyToOperate();
  if (hresult == S_OK)
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}

HRESULT TestCanopenStatusControl::testSetOperationMode()
{
  log(L"\n<b>Test set operation mode.");
  HRESULT hresult = S_OK;
  ProfileTrace traceHelper;
  test::MockFieldbusAdapterCoE mockFieldbusAdapter{};
  std::uint32_t deviceAddress = 1;
  test::MockDatalayerClient mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient *>(&mockClient));
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  atData.m_atData.m_opModeDisplay = 9;
  mdtData.m_mdtData.m_opMode = 9;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  canopen::CanopenStatusControlDrive testControlword(profileBasicInfo, &atData, &mdtData);
  std::vector<TestCaseControlword> testcases
  {
    // name                     m_expectedResult                            m_commandedOpMode                         dataResponse
    {L"Set velocity mode.",     AxisProfileResult::STS_OK,                  AxisProfileOperationMode::VELOCITY,       canopen::CANOPEN_VELOCITY_MODE},
    {L"Set position mode.",     AxisProfileResult::STS_OK,                  AxisProfileOperationMode::POSITIONING,    canopen::CANOPEN_POSITION_MODE},
    {L"Set torque mode.",       AxisProfileResult::STS_OK,                  AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT, canopen::CANOPEN_FORCE_VEL_LIMIT},
    {L"Set invalid mode.",      AxisProfileResult::ERR_NOT_SUPPORTED,       AxisProfileOperationMode::UNDEFINED,      0},    
  };

  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  comm::axisprofile::ProfileAssignedInfo profileAssigned {true};
  test::MockAxisProfileDeviceParam deviceParamMockup {};
  comm::axisprofile::OpModeAvailInfo m_opModeAvailInfo {true};
  deviceParamMockup.setReturnValue(&m_opModeAvailInfo);
  mappingHandlerMockup.setReturnValue(profileAssigned);
  
  comm::axisprofile::canopen::CanopenParamAccess  canopenParamAccess(profileBasicInfo);
  test::MockAxisProfileCommand commandIf {};
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testControlword);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &commandIf);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &canopenParamAccess);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  for (const auto &elem : testcases)
  {
    hresult |= runTestSetOperationMode(&elem, testControlword, mockClient);
  }

  //test operation mode not mapping PDO
  std::vector<TestCaseControlword> testcasesNotMapping
  {
    // name                     m_expectedResult                            m_commandedOpMode                         dataResponse
    {L"Set velocity mode.",     AxisProfileResult::STS_OK,                  AxisProfileOperationMode::VELOCITY,       canopen::CANOPEN_VELOCITY_MODE},
    {L"Set position mode.",     AxisProfileResult::STS_OK,                  AxisProfileOperationMode::POSITIONING,    canopen::CANOPEN_POSITION_MODE},
    {L"Set torque mode.",       AxisProfileResult::STS_OK,                  AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT, canopen::CANOPEN_FORCE_VEL_LIMIT},
    {L"Set invalid mode.",      AxisProfileResult::ERR_NOT_SUPPORTED,       AxisProfileOperationMode::UNDEFINED,      0},
  };

  profileAssigned.m_input.m_opModeDisplay = false ;
  profileAssigned.m_output.m_opMode = false;
  mappingHandlerMockup.setReturnValue(profileAssigned);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);

  for (const auto& elem : testcasesNotMapping)
  {
    hresult |= runTestSetOperationMode(&elem, testControlword, mockClient);
  }

  return hresult;
}

HRESULT TestCanopenStatusControl::runTestSetOperationMode(const TestCaseControlword *testcase, comm::axisprofile::canopen::CanopenStatusControlDrive &testControlword, test::MockDatalayerClient &mockClient)
{
  HRESULT hresult = S_OK;
  AxisProfileResult result;
  setResponseVariant(&mockClient, testcase->dataResponse);
  do
  {
    result = testControlword.setOperationMode(testcase->m_commandedOpMode);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if (aprStatusFailed(result))
    {
      break;
    }
  } while (AxisProfileResult::STS_PENDING == result);

  if (testcase->m_expectedResult != result)
  {
    error(L"set Canopen operation mode -> %s failed.", testcase->m_name);
    return E_FAIL;
  }
  if (hresult == S_OK)
  {
    log(L"\n\tRun testcase: %ls... <DARK_GREEN>OK</DARK_GREEN>", testcase->m_name);
  }

  return hresult;
}

HRESULT TestCanopenStatusControl::testSetPowerMode()
{
  log(L"\n<b>Test set power mode.</b>");
  HRESULT hresult = S_OK;
  ProfileTrace traceHelper;
  AcknowledgeTelegram atData{};
  MasterDataTelegram mdtData{};
  test::MockFieldbusAdapterCoE mockFieldbusAdapter{};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = 1;
  profileBasicInfo.m_driveNumber = 1;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  canopen::ScalingStrategyCoEControl scalingObj {profileBasicInfo};
  canopen::CanopenStatusControlDrive testStatuswords(profileBasicInfo, &atData, &mdtData);
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&traceHelper});
  sharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testStatuswords);
  sharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &scalingObj);

  std::vector<TestPowerMode> testcases
  {
    {L"PowerOn: everything works fine",   {0x40, 0x21, 0x23, 0x27}, AxisProfileResult::STS_OK,            AxisProfilePowerMode::POWER_ON},
    {L"PowerOn: invalid state",           {0x00},                   AxisProfileResult::ERR_INVALID_STATE, AxisProfilePowerMode::POWER_ON},
    {L"PowerOn: invalid state",           {0x07},                   AxisProfileResult::ERR_INVALID_STATE, AxisProfilePowerMode::POWER_ON},
    {L"PowerOff: everything works fine",  {0x27, 0x23, 0x21},       AxisProfileResult::STS_OK,            AxisProfilePowerMode::POWER_OFF},
    {L"Unknown power mode",               {0x00},                   AxisProfileResult::ERR_NOK,           AxisProfilePowerMode::POWER_OFF_TORQUE_FREE},
  };
  for (const auto &testcase : testcases)
  {
    hresult |= runTestSetPowerMode(&testcase, testStatuswords, atData);
  }

  std::uint16_t timeoutCycle = canopen::CANOPEN_SWITCH_TIMEOUT_THRESHOLD + 5;
  TestPowerMode timeoutPowerOn {L"Error case of timeout of power on", {}, AxisProfileResult::ERR_TIMEOUT, AxisProfilePowerMode::POWER_ON};
  timeoutPowerOn.m_statusWords.assign(timeoutCycle, 0x23);
  hresult |= runTestSetPowerMode(&timeoutPowerOn, testStatuswords, atData);

  TestPowerMode timeoutPowerOff {L"Error case of timeout of power off", {}, AxisProfileResult::ERR_TIMEOUT, AxisProfilePowerMode::POWER_OFF};
  timeoutPowerOff.m_statusWords.assign(timeoutCycle, 0x27);
  hresult |= runTestSetPowerMode(&timeoutPowerOff, testStatuswords, atData);

  return hresult;
}

HRESULT TestCanopenStatusControl::runTestSetPowerMode(const TestPowerMode* testcase,
                                                      canopen::CanopenStatusControlDrive& testControlword,
                                                      AcknowledgeTelegram& atData) const
{
  log(L"\n  Test case of %ls...", testcase->m_name);

  AxisProfileResult result {};
  for (const auto& statusWord : testcase->m_statusWords)
  {
    atData.m_statusWord.m_word = statusWord;
    testControlword.analyzeStatusWord();
    result = testControlword.setPowerMode(testcase->m_commandedPowerMode);
    if (aprStatusFailed(result))
    {
      break;
    }
  }

  RETURN_TEST_RESULT(result, L"\nUnexpected return value", testcase->m_expectedResult);
  return S_OK;
}

void TestCanopenStatusControl::setResponseVariant(test::MockDatalayerClient *const client, const std::uint16_t &data)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  std::uint8_t buffer[1];
  std::size_t dataSize = 1;
  (void)memcpy(buffer, &data, dataSize);
  auto vec = builder.CreateVector(buffer, dataSize);
  comm::ethercat::master::fbs::SDOResponseBuilder sdoRponseBuilder(builder);
  if (data)
  {
    sdoRponseBuilder.add_data(vec);
  }
  auto exchangeDataResponse = sdoRponseBuilder.Finish();
  comm::ethercat::master::fbs::SDOBuilder paramBuilder(builder);
  paramBuilder.add_response(exchangeDataResponse);
  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);
  comm::datalayer::Variant variant;
  variant.copyFlatbuffers(builder);
  client->m_callbackVariant = variant;
}

HRESULT TestCanopenStatusControl::testSetDriveHalt() const
{
  /////////////////////////////////////////////////////Test Scenario/////////////////////////////////////////////////////
  log(L"\n<b>Test drive halt.</b>");
  struct DriveHaltTestCase
  {
    std::wstring m_testName;                //!< test name
    std::function<AxisProfileResult(canopen::CanopenStatusControlDrive& statusCtrl)> m_testFun; 
    std::uint16_t m_statusWord;             //!< test value for statusword
    AxisProfileResult m_expectedResult;     //!< set expected return value
    AxisProfileResult m_cmdReturnValue;     //!< command return value
  };
  std::vector<DriveHaltTestCase> testCase {
    {L"Set drive halt success ",                [](canopen::CanopenStatusControlDrive& statusCtrl){return statusCtrl.setDriveHaltMode(true);},                    0x0406, AxisProfileResult::STS_OK                , AxisProfileResult::STS_OK},
    {L"Set drive halt success ",                [](canopen::CanopenStatusControlDrive& statusCtrl){return statusCtrl.setDriveHaltMode(false);},                   0x0406, AxisProfileResult::STS_OK                , AxisProfileResult::STS_OK},
    {L"Set drive halt but device in Ab",        [](canopen::CanopenStatusControlDrive& statusCtrl){return statusCtrl.setDriveHaltMode(true);},                    0x0000, AxisProfileResult::ERR_WRONG_DEVICE_STATE, AxisProfileResult::STS_OK},
    {L"Get drive halt status",                  [](canopen::CanopenStatusControlDrive& statusCtrl){bool isHalted; return statusCtrl.getDriveHaltMode(isHalted);}, 0x0406, AxisProfileResult::STS_OK                , AxisProfileResult::STS_OK},
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
  canopen::CanopenStatusControlDrive testStatusControl(profileBasicInfo, &atData, &mdtData);

  //register interface for sharing handler
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testStatusControl);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &commandIf);

  int maxTestTimes = 2;
  AxisProfileResult testRes {AxisProfileResult::STS_PENDING};
  for (const auto& oneTest : testCase)
  {
    log(L"\n  Test case of %s ...", oneTest.m_testName.c_str());
    atData.m_statusWord.m_word = oneTest.m_statusWord;
    testStatusControl.analyzeStatusWord();
    commandIf.setReturnValue(oneTest.m_cmdReturnValue);

    while (maxTestTimes--)
    {
      testRes = oneTest.m_testFun(testStatusControl);
    }
    RETURN_TEST_RESULT(testRes, L"\n  unexpected return value.", oneTest.m_expectedResult);
    maxTestTimes = 2;
  }
    
  return S_OK;
}

HRESULT TestCanopenStatusControl::testCheckDeviceFollowsControl() const
{
  HRESULT res = S_OK;
  log(L"\n<b>Test of checking whether device follows command value....</b>");

  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo basicInfo = {AxisProfileType::CANOPEN_OVER_ETHERCAT, "TestBuddyTesting", 0, 0};
  canopen::CanopenStatusControlDrive testStatusControl(basicInfo, &atData, &mdtData);
  test::MockAxisProfileTrace profileTraceMockup; 
  AxisProfileSharingHandler sharingHandler(&profileTraceMockup); 
  test::MockAxisProfileMappingHandler mappingHandlerMockup;
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testStatusControl);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  testStatusControl.setSharingHandler(&sharingHandler);

  log(L"\nTest isDeviceFollowsControl()...");
  RETURN_TEST_RESULT_ANY(testStatusControl.isDeviceFollowsControl() == false, L"\nGet an unexpected result!");

  log(L"\nTest device follows command value (without opmode-display input)...");
  auto& assignedInfo = mappingHandlerMockup.getProfileAssignedInfo();
  assignedInfo.m_input.m_opModeDisplay = false;
  RETURN_TEST_RESULT_ANY(testStatusControl.isDeviceFollowsControl() == false, L"\nGet an unexpected result!");

  log(L"\nTest device follows command value (CANOPEN_POSITION_MODE)...");
  assignedInfo.m_input.m_opModeDisplay = true;
  atData.m_atData.m_opModeDisplay = canopen::CANOPEN_POSITION_MODE;
  atData.m_statusWord.m_bits.bit12 = 1;
  testStatusControl.analyzeStatusWord();
  RETURN_TEST_RESULT_ANY(testStatusControl.isDeviceFollowsControl() == true, L"\nGet an unexpected result!");

  log(L"\nTest device follows command value (CANOPEN_VELOCITY_MODE)...");
  assignedInfo.m_input.m_opModeDisplay = true;
  atData.m_atData.m_opModeDisplay = canopen::CANOPEN_VELOCITY_MODE;
  atData.m_statusWord.m_bits.bit12 = 0;
  testStatusControl.analyzeStatusWord();
  RETURN_TEST_RESULT_ANY(testStatusControl.isDeviceFollowsControl() == false, L"\nGet an unexpected result!");

  log(L"\nTest device follows command value (CANOPEN_TORQUE_FORCE_VEL_LIMIT)...");
  assignedInfo.m_input.m_opModeDisplay = true;
  atData.m_atData.m_opModeDisplay = canopen::CANOPEN_FORCE_VEL_LIMIT;
  atData.m_statusWord.m_bits.bit12 = 0;
  testStatusControl.analyzeStatusWord();
  RETURN_TEST_RESULT_ANY(testStatusControl.isDeviceFollowsControl() == false, L"\nGet an unexpected result!");

  return res;
}
  
HRESULT TestCanopenStatusControl::testCheckOperationModeSupported()
{
  HRESULT res = S_OK;
  log(L"\n<b>Test of the checking about the supported operation mode....</b>");

  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData; 
  ProfileBasicInfo basicInfo = {AxisProfileType::CANOPEN_OVER_ETHERCAT, "TestBuddyTesting", 0, 0};
  canopen::CanopenStatusControlDrive testStatusControl(basicInfo, &atData, &mdtData);
  test::MockAxisProfileTrace profileTraceMockup; 
  AxisProfileSharingHandler sharingHandler(&profileTraceMockup); 
  test::MockAxisProfileDeviceParam   deviceParamMockup;
  test::MockAxisProfileMappingHandler mappingHandlerMockup;
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  testStatusControl.setSharingHandler(&sharingHandler);
  comm::axisprofile::OpModeAvailInfo opModeAvailInfo {true};
  deviceParamMockup.setReturnValue(&opModeAvailInfo);

  log(L"\nTest checkOperationModeSupported()...");
  comm::axisprofile::OpModeErrorInfo opModeErrorInfo{};
  bool result = testStatusControl.checkOperationModeSupported(comm::axisprofile::AxisProfileOperationMode::UNDEFINED, opModeErrorInfo);
  RETURN_TEST_RESULT_ANY(result == false, L"\nGet an unexpected result!");

  log(L"\nTest checkOperationModeSupported() of unknown opmode...");
  result = testStatusControl.checkOperationModeSupported(static_cast<AxisProfileOperationMode>(123), opModeErrorInfo);
  RETURN_TEST_RESULT_ANY(result == false, L"\nGet an unexpected result!");

  log(L"\nTest if supported velocity mode (without target velocity output)...");
  auto& assignedInfo = mappingHandlerMockup.getProfileAssignedInfo();
  assignedInfo.m_output.m_velocity = false;
  result = testStatusControl.checkOperationModeSupported(comm::axisprofile::AxisProfileOperationMode::VELOCITY, opModeErrorInfo);
  RETURN_TEST_RESULT_ANY(result == false, L"\nGet an unexpected result!");

  log(L"\nTest if supported velocity mode (with velocity input/output)...");
  assignedInfo.m_input.m_position  = true;
  assignedInfo.m_output.m_position = true;
  result = testStatusControl.checkOperationModeSupported(comm::axisprofile::AxisProfileOperationMode::POSITIONING, opModeErrorInfo);
  RETURN_TEST_RESULT_ANY(result == true, L"\nGet an unexpected result!");

  log(L"\nTest if supported Torque with velocity limit mode...");
  assignedInfo.m_input.m_torqueForce = true;
  assignedInfo.m_input.m_torqueForceLimSts = true;
  assignedInfo.m_output.m_torqueForce = true;
  assignedInfo.m_output.m_torqueVelLimHigh = true;
  assignedInfo.m_output.m_torqueVelLimLow = true;
  result = testStatusControl.checkOperationModeSupported(comm::axisprofile::AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT, opModeErrorInfo);
  RETURN_TEST_RESULT_ANY(result == true, L"\nGet an unexpected result!");

  log(L"\nTest if supported Torque with velocity limit mode...");
  assignedInfo.m_input.m_torqueForce = true;
  assignedInfo.m_input.m_torqueForceLimSts = true;
  assignedInfo.m_output.m_torqueForce = true;
  assignedInfo.m_output.m_torqueVelLimHigh = true;
  assignedInfo.m_output.m_torqueVelLimLow = true;
  result = testStatusControl.checkOperationModeSupported(comm::axisprofile::AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT, opModeErrorInfo);
  RETURN_TEST_RESULT_ANY(result == true, L"\nGet an unexpected result!");

  log(L"\ncheck operation mode with op mode display");
  assignedInfo.m_input.m_opModeDisplay = true;
  AxisProfileResult checkRes = testStatusControl.checkOperationMode();
  RETURN_TEST_RESULT_ANY(checkRes == AxisProfileResult::STS_OK, L"\nGet an unexpected result!");

  log(L"\ntest for checkOpModeAvailable");
  AvailabilityInfo support {false};
  OpModeErrorInfo errInfos {};
  result = testStatusControl.checkOpModeAvailable(support, true, false, errInfos);
  RETURN_TEST_RESULT_ANY(result == false, L"\nGet an unexpected result!");

  support.available = true;
  result = testStatusControl.checkOpModeAvailable(support, true, false, errInfos);
  RETURN_TEST_RESULT_ANY(result == false, L"\nGet an unexpected result!");

  return res;
}

HRESULT TestCanopenStatusControl::testDetermineCanopenOpMode()
{
  HRESULT res = S_OK;
  log(L"\n<b>Test of the determineCanopenOpMode()....</b>");
  AcknowledgeTelegram atData;
  MasterDataTelegram mdtData;
  ProfileBasicInfo basicInfo = {AxisProfileType::CANOPEN_OVER_ETHERCAT, "TestBuddyTesting", 0, 0};
  canopen::CanopenStatusControlDrive testStatusControl(basicInfo, &atData, &mdtData);
  test::MockAxisProfileTrace profileTraceMockup;
  AxisProfileSharingHandler sharingHandler(&profileTraceMockup);
  test::MockAxisProfileDeviceParam   deviceParamMockup;
  test::MockAxisProfileMappingHandler mappingHandlerMockup;
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  testStatusControl.setSharingHandler(&sharingHandler);
  comm::axisprofile::OpModeAvailInfo opModeAvailInfo {true};
  deviceParamMockup.setReturnValue(&opModeAvailInfo);
  log(L"\nTest determineCanopenOpMode()...");
  std::uint8_t mode = 0;
  AxisProfileResult result = testStatusControl.determineCanopenOpMode(comm::axisprofile::AxisProfileOperationMode::UNDEFINED, mode);
  RETURN_TEST_RESULT_ANY(result == AxisProfileResult::ERR_NOT_SUPPORTED, L"\nGet an unexpected result!");
  log(L"\nTest determineCanopenOpMode()...");
  result = testStatusControl.determineCanopenOpMode(comm::axisprofile::AxisProfileOperationMode::POSITIONING, mode);
  RETURN_TEST_RESULT_ANY(result == AxisProfileResult::ERR_NOK, L"\nGet an unexpected result!");
  log(L"\nTest determineCanopenOpMode()...");
  result = testStatusControl.determineCanopenOpMode(comm::axisprofile::AxisProfileOperationMode::VELOCITY, mode);
  RETURN_TEST_RESULT_ANY(result == AxisProfileResult::ERR_NOK, L"\nGet an unexpected result!");
  log(L"\nTest determineCanopenOpMode()...");
  result = testStatusControl.determineCanopenOpMode(comm::axisprofile::AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT, mode);

  return res;
}

HRESULT TestCanopenStatusControl::testMics()
{
  AcknowledgeTelegram atData;
  MasterDataTelegram mdtData;
  ProfileBasicInfo basicInfo = {AxisProfileType::CANOPEN_OVER_ETHERCAT, "TestBuddyTesting", 0, 0};
  canopen::CanopenStatusControlDrive testStatusControl(basicInfo, &atData, &mdtData);

  log(L"\n<b>Test of getFsaStateFromStatusWord()....</b>");
  // structure of test case of FAS state
  struct TestFasStateCase
  {
    const wchar_t* name;          // name of test case
    std::uint16_t statusWord;     // test value for statusword
    canopen::FsaState expState;   // expected FSA state
    ReadyToOperate expReadyToOP;  // expected ready to operate state
  };

  std::vector<TestFasStateCase> testCases =
  {
    {L"Not ready to switch on",      0x0000, canopen::FsaState::NOT_READY_TO_SWITCH_ON, ReadyToOperate::DRIVE_NOT_READY},
    {L"Switch on disabled",          0x0040, canopen::FsaState::SWITCH_ON_DISABLED,     ReadyToOperate::DRIVE_READY_FOR_MAIN_POWER_ON},
    {L"Ready to switch on",          0x0021, canopen::FsaState::READY_TO_SWITCH_ON,     ReadyToOperate::DRIVE_READY_FOR_MAIN_POWER_ON},
    {L"Switched on",                 0x0023, canopen::FsaState::SWITCHED_ON,            ReadyToOperate::DRIVE_READY_AND_MAIN_POWER_APPLIED},
    {L"Operation enabled",           0x0027, canopen::FsaState::OPERATION_ENABLED,      ReadyToOperate::DRIVE_ENABLED},
    {L"Quick stop active",           0x0007, canopen::FsaState::QUICK_STOP_ACTIVE,      ReadyToOperate::DRIVE_ENABLED},
    {L"Fault reaction active",       0x000F, canopen::FsaState::FAULT_REACTION_ACTIVE,  ReadyToOperate::DRIVE_NOT_READY},
    {L"Fault",                       0x0008, canopen::FsaState::FAULT,                  ReadyToOperate::DRIVE_NOT_READY},
  };
  for (const auto& it : testCases)
  {
    log(L"\n  Test of %ls, statusword: <BLUE>0x%04X</BLUE>, expected FAS state <BLUE>%ls</BLUE>...",
        it.name, it.statusWord, UTF8TOWIDE(canopen::FasStateToString(it.expState)));

    auto fsaState = canopen::getFsaStateFromStatusWord(it.statusWord);
    LOG_TEST_STATE(fsaState, L"\nUnexpected state", it.expState, canopen::FasStateToString);

    log(L"\n  Test of %ls, statusword: <BLUE>0x%04X</BLUE>, expected ReadyToOP <BLUE>%ls</BLUE>...",
        it.name, it.statusWord, UTF8TOWIDE(axisProfileReadyToOpTypeToString(it.expReadyToOP)));
    atData.m_statusWord.m_word = it.statusWord;
    testStatusControl.analyzeStatusWord();
    auto readyToOperate = testStatusControl.checkReadyToOperate();
    LOG_TEST_STATE(readyToOperate, L"\nUnexpected state", it.expReadyToOP, axisProfileReadyToOpTypeToString);
  }

  return S_OK;
}
