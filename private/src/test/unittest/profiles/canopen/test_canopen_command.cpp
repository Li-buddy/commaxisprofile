#include "helper/result_helper.h"

#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_scalingstrategy.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "mockup/mock_axisprofile_category.h"

#include "profiles/aux_job/aux_job_handler.h"
#include "profiles/canopen/canopen_param_access.h"
#include "profiles/canopen/drive/canopen_status_control_drive.h"
#include "profiles/canopen/drive/canopen_command_drive.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "test_canopen_command.h"
#include <future>

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenCommand);
REGISTER_TEST_TO_GROUP(TestCanopenCommand, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenCommand, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenCommand, Profiles_Canopen);


HRESULT TestCanopenCommand::doInit()
{
  m_mockObjs.m_client = new MockDatalayerClient;
  m_mockObjs.m_profileInfo.m_driveNumber = 1;
  m_mockObjs.m_profileInfo.m_fieldbusAdapter = new MockFieldbusAdapterCoE;
  m_mockObjs.m_profileInfo.m_profileName = m_profileName;
  m_mockObjs.m_scaling = new MockScalingStrategy(m_mockObjs.m_profileInfo);
  m_mockObjs.m_profileInfo.m_fieldbusAdapter->setDatalayer(m_mockObjs.m_client);
  m_mockObjs.m_profileInfo.m_deviceAddress = 1001;
  return S_OK;
}

HRESULT TestCanopenCommand::doEnd()
{
  if (m_mockObjs.m_client)
  {
    delete m_mockObjs.m_client;
    m_mockObjs.m_client = nullptr;
  }
  if (m_mockObjs.m_scaling)
  {
    delete m_mockObjs.m_scaling;
    m_mockObjs.m_scaling = nullptr;
  }
  if (m_mockObjs.m_profileInfo.m_fieldbusAdapter)
  {
    delete m_mockObjs.m_profileInfo.m_fieldbusAdapter;
    m_mockObjs.m_profileInfo.m_fieldbusAdapter = nullptr;
  }
  return S_OK;
}

HRESULT TestCanopenCommand::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Canopen profile command class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testMiscellaneous();
  hresult |= testSimpleCommands();
  hresult |= testReferencingCommand(); 
  hresult |= testRisingAndTimeoutError();
  hresult |= testCommandAssignedOpMode();
  hresult |= testSetReferencingPosition();
  hresult |= testAuxJobsHandling();
  hresult |= testCheckIfDriveHalted();
  log(L"\n");
  return hresult;
}

HRESULT TestCanopenCommand::testMiscellaneous() 
{
  log(L"\n<b>Test miscellaneous.</b>");
  canopen::CanopenCommandDrive testCommand {m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt};
  AxisProfileResult result {AxisProfileResult::STS_OK};
  AsyncCommandStatemachine* asyncState {nullptr};

  comm::axisprofile::canopen::CanopenParamAccess paramAccess(m_mockObjs.m_profileInfo);

  log(L"\n  Test accessAndCheckParamListAsync error...");
  testCommand.m_paramAccess = &paramAccess;
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  std::list<canopen::CanopenParamStruct> paramList;
  result = testCommand.m_paramAccess->accessAndCheckParamListAsync(paramList, *asyncState);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n");
  return S_OK;
}

HRESULT TestCanopenCommand::testSimpleCommands()
{
  log(L"\n<b>Test simple commands.</b>");
  struct CommandsTestCase
  {
    std::wstring m_testName;              // test name
    std::function<AxisProfileResult(canopen::CanopenCommandDrive& cmd)> m_testFun; // test function
    std::uint16_t m_setDataState;              // set of datalayer value
    AxisProfileResult m_expectedResult;   // expected return value
  };

  std::vector<CommandsTestCase> testCase {
    {L"ClearDeviceError",                   [](canopen::CanopenCommandDrive& cmd){               return cmd.clearDeviceError();},                                                      0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setAndCheckPositiveStop",            [](canopen::CanopenCommandDrive& cmd){               return cmd.setAndCheckPositiveStop(AxisProfilePositiveStop::ENABLE);},                0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setAndCheckClampingTorque",          [](canopen::CanopenCommandDrive& cmd){               return cmd.setAndCheckClampingTorque(0.0);},                                          0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getClampingTorque",                  [](canopen::CanopenCommandDrive& cmd){double value;  return cmd.getClampingTorque(&value);},                                               0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"checkStandStill",                    [](canopen::CanopenCommandDrive& cmd){bool value;    return cmd.checkStandStill(&value);},                                                 0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setAndCheckStandStillWindow",        [](canopen::CanopenCommandDrive& cmd){               return cmd.setAndCheckStandStillWindow(0.0);},                                        0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getStandStillWindow",                [](canopen::CanopenCommandDrive& cmd){double value;  return cmd.getStandStillWindow(&value);},                                             0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"activateCyclicStandStillCheck",      [](canopen::CanopenCommandDrive& cmd){               return cmd.activateCyclicStandStillCheck();},                                         0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setAbsolutePosition",                [](canopen::CanopenCommandDrive& cmd){               return cmd.setAbsolutePosition(0.0, AxisProfileEncoder::ENCODER_1, true);},           0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"deactivateCyclicStandStillCheck",    [](canopen::CanopenCommandDrive& cmd){               return cmd.deactivateCyclicStandStillCheck();},                                       0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"checkAxisHomed",                     [](canopen::CanopenCommandDrive& cmd){bool value;    return cmd.checkAxisHomed(value);},                                                   0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"resetAbsolutePosition",              [](canopen::CanopenCommandDrive& cmd){               return cmd.resetAbsolutePosition();},                                                 0x0000,         AxisProfileResult::ERR_NOTIMPLEMENTED},
  };

  test::MockAxisProfileCyclicData mockCyclicData;
  AuxJobHandler auxJobHandler {};
  test::MockAxisProfileProperty proper {};
  proper.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  AxisProfileResult result {AxisProfileResult::STS_OK};
  for (const auto& oneTest : testCase)
  {
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    test::MockAxisProfileDeviceParam testDeviceParam{};
    //testDeviceParam.m_deviceFirmwareInfo.m_deviceSeries = DeviceSeries::CTRLXDRIVE;

    canopen::CanopenCommandDrive testCommand {m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt};
    m_mockObjs.m_at.m_statusWord.m_word = 0;
    testCommand.resetAllAxisprofileCommands();
    canopen::CanopenParamAccess paramAccess(m_mockObjs.m_profileInfo);
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(static_cast<test::MockDatalayerClient*>(m_mockObjs.m_client), data);

    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&m_mockObjs.m_profileInfo.m_profileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclicData);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &proper);

    testCommand.booting();
  
    std::uint16_t cycleExpired = 1000; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testCommand);
      if (result != AxisProfileResult::STS_PENDING)
      {
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
  }
  log(L"\n");
  return S_OK;
}

HRESULT TestCanopenCommand::testReferencingCommand()
{
  log(L"\n<b>Test referencingAxis.");
  struct CommandsTestCase
  {
    std::wstring m_testName;              // test name
    std::function<AxisProfileResult(canopen::CanopenCommandDrive& cmd)> m_testFun; // test function
    std::uint16_t m_setStatusWord;             // set status word
    std::uint16_t m_setDataState;              // set of datalayer value
    AxisProfileResult m_expectedResult;   // expected return value
    AxisProfileCommandType m_cmdType;     // command type
    AxisprofileCommandState m_cmdState = AxisprofileCommandState::CMD_STATE_ACTIVE;     // command state
    bool m_inputassignedInfo{true};
    bool m_outputassignedInfo{true};
   };

  std::vector<CommandsTestCase> testCase {
    {L"ResetRefAxs",                          [](canopen::CanopenCommandDrive& cmd){return cmd.resetReferencingAxis();},                                         0x0000,        0x0000,      AxisProfileResult::ERR_NOTIMPLEMENTED,   AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS},
    {L"ResetRefAxs error op mode not mapped", [](canopen::CanopenCommandDrive& cmd){return cmd.resetReferencingAxis();},                                         0x0000,        0x0000,      AxisProfileResult::ERR_NOTIMPLEMENTED,   AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS,  AxisprofileCommandState::CMD_STATE_INIT, false},
    {L"RefAxs normal case encoder1",          [](canopen::CanopenCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_1);},            0x1400,        0x0000,      AxisProfileResult::ERR_NOTIMPLEMENTED,   AxisProfileCommandType::CMD_REFERENCING_AXIS},
    {L"RefAxs clear",                         [](canopen::CanopenCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_1);},            0x1400,        0x0000,      AxisProfileResult::ERR_NOTIMPLEMENTED,   AxisProfileCommandType::CMD_REFERENCING_AXIS,        AxisprofileCommandState::CMD_STATE_ABORTING},
    {L"RefAxs error position overflow",       [](canopen::CanopenCommandDrive& cmd){return cmd.referencingAxis(UINT32_MAX, AxisProfileEncoder::ENCODER_1);},     0x1400,        0x0000,      AxisProfileResult::ERR_NOTIMPLEMENTED,   AxisProfileCommandType::CMD_REFERENCING_AXIS},
    {L"RefAxs error power off",               [](canopen::CanopenCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_1);},            0x1401,        0x0000,      AxisProfileResult::ERR_NOTIMPLEMENTED,   AxisProfileCommandType::CMD_REFERENCING_AXIS},
    {L"RefAxs op mode not mapped",            [](canopen::CanopenCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_1);},            0x1400,        0x0000,      AxisProfileResult::ERR_NOTIMPLEMENTED,   AxisProfileCommandType::CMD_REFERENCING_AXIS,        AxisprofileCommandState::CMD_STATE_INIT, false, false},

  };

  test::MockCategory mockCategory;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  comm::axisprofile::canopen::CanopenParamAccess paramAccess(m_mockObjs.m_profileInfo);
  test::MockAxisProfileDeviceParam testDeviceParam{};
  testDeviceParam.m_deviceCfgData.m_positivePosLimit = INT32_MAX / 2;
  testDeviceParam.m_deviceCfgData.m_negativePosLimit = INT32_MIN / 2;
  test::MockAxisProfileCyclicData mockCyclicData;
  AuxJobHandler auxJobHandler {};
  canopen::CanopenStatusControlDrive testStatusControl(m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt);
  test::MockAxisProfileProperty property {};

  AxisProfileResult result {AxisProfileResult::STS_OK};  
  for (const auto& oneTest : testCase)
  {
    log(L"\n  Test of %s ...", oneTest.m_testName.c_str());
    comm::axisprofile::ProfileAssignedInfo assignedInfo{oneTest.m_inputassignedInfo, oneTest.m_outputassignedInfo};
    assignedInfo.m_input.m_position = true;
    assignedInfo.m_output.m_position = true;
    mappingHandlerMockup.setReturnValue(assignedInfo);
    comm::axisprofile::OpModeAvailInfo opModeAvailInfo{true, true, true}; 
    testDeviceParam.m_opModeAvailInfo = &opModeAvailInfo;
    canopen::CanopenCommandDrive testCommand {m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&m_mockObjs.m_profileInfo.m_profileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclicData);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testStatusControl);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, m_mockObjs.m_scaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);

    testCommand.booting();
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(static_cast<test::MockDatalayerClient*>(m_mockObjs.m_client), data);
    m_mockObjs.m_at.m_statusWord.m_word = oneTest.m_setStatusWord;

    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testCommand);
      if (!aprStatusFailed(result))
      {
        if ((oneTest.m_cmdType == AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS) )
        {
          result = testCommand.executeResetReferencingAxis();
        }
        if ((oneTest.m_cmdType == AxisProfileCommandType::CMD_REFERENCING_AXIS))
        {
          if (oneTest.m_cmdState == AxisprofileCommandState::CMD_STATE_ABORTING && !m_specialStepFlag)
          {
            m_specialStepFlag = true; 
            testCommand.abortAxisProfileCommand(AxisProfileCommandType::CMD_REFERENCING_AXIS);
          }

          auto stm = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_REFERENCING_AXIS);
          auto currentStep = stm->getCommandStep();
          if (currentStep  == AsyncCommandStatemachine::CommandStep::CMD_START)
          {
            testStatusControl.setOperationMode(AxisProfileOperationMode::HOMING);
            // set current operation mode: homing mode
            if (mappingHandlerMockup.getProfileAssignedInfo().m_input.m_opModeDisplay)
            {
              // homing mode: 6
              m_mockObjs.m_at.m_atData.m_opModeDisplay = 6;
            }
            else
            { 
              // homing mode: 6
              TestDataVariantU16 dat {L"Add homing mode", 6, true};
              setResponseVariant(static_cast<test::MockDatalayerClient*>(m_mockObjs.m_client), dat);
            }
          }
          if (currentStep == AsyncCommandStatemachine::CommandStep::CMD_SET)
          { 
            testStatusControl.setOperationMode(AxisProfileOperationMode::POSITIONING);
            // set current operation mode: position mode
            if (mappingHandlerMockup.getProfileAssignedInfo().m_input.m_opModeDisplay)
            {
              // position mode: 8
              m_mockObjs.m_at.m_atData.m_opModeDisplay = 8;
            }
            else
            { 
              // position mode: 8
              TestDataVariantU16 dat {L"reset position mode", 8, true};
              setResponseVariant(static_cast<test::MockDatalayerClient*>(m_mockObjs.m_client), dat);
            }
            // status word (0b1000110111, bit12/10 is 0) indicates homing command is executing.
            m_mockObjs.m_at.m_statusWord.m_word = 0b1000110111;
          }
          result = testCommand.executeReferencingAxis();
          m_mockObjs.m_at.m_statusWord.m_word = oneTest.m_setStatusWord;
        }
      }

      if (result != AxisProfileResult::STS_PENDING)
      {
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
  }
  m_specialStepFlag = false;
  log(L"\n");
  return S_OK;
}

HRESULT TestCanopenCommand::testRisingAndTimeoutError()
{
  log(L"\n<b>Test rising failed and timeout case</b>");
  struct CommandsTestCase
  {
    std::wstring m_cmdName;             // test name
    AxisProfileCommandType m_cmdType;   // command type
    std::function<AxisProfileResult(canopen::CanopenCommandDrive& cmd)> m_testFun;
  };

  std::vector<CommandsTestCase> testCase {
    {L"clearDeviceError",         AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR,       [](canopen::CanopenCommandDrive& cmd)  {return cmd.clearDeviceError();}},
    {L"referencingAxis",          AxisProfileCommandType::CMD_REFERENCING_AXIS,         [](canopen::CanopenCommandDrive& cmd)  {return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_1);}}
  };

  AcknowledgeTelegram atData {};
  MasterDataTelegram mdtData {};
  MockAxisProfileTrace mockProfileTrace{};
  MockFieldbusAdapterCoE mockFieldbusAdapter{};
  MockDatalayerClient mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  std::uint32_t deviceAddress = 1;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_deviceAddress = deviceAddress;

  test::MockScalingStrategy strategy(profileBasicInfo);
  
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&m_mockObjs.m_profileInfo.m_profileTrace});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  canopen::CanopenCommandDrive testCommand {profileBasicInfo, &atData, &mdtData};
  test::MockAxisProfileDeviceParam testDeviceParam{};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockAxisProfileCyclicData mockCyclicData;
  test::MockCategory mockCategory;
  AuxJobHandler auxJobHandler {};
  const comm::axisprofile::ProfileAssignedInfo assignedInfo {true};
  mappingHandlerMockup.setReturnValue(assignedInfo);
  test::MockAxisProfileProperty property;

  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclicData);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &strategy);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);

  log(L"\n<b>  Test of error case of rising failed: </b>");
  for (const auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};

    log(L"\n    Test of rising failed on %s ...", oneTest.m_cmdName.c_str());

    testCommand.resetAllAxisprofileCommands();
    result = oneTest.m_testFun(testCommand);
    RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);
  }
  log(L"\n");

  log(L"\n<b>  Test error case of timeout: </b>");
  for (const auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};

    // homing will has disable timeout check
    if (oneTest.m_cmdType == AxisProfileCommandType::CMD_REFERENCING_AXIS)
    {
      break;
    }

    log(L"\n    Test of timeout on %s ...", oneTest.m_cmdName.c_str());
    profileBasicInfo.m_deviceAddress = deviceAddress;
    TestDataVariantU16 data {L"Add invalid data", 0x0000, true};
    setResponseVariant(&mockClient, data);
    testCommand.resetAllAxisprofileCommands();

    AsyncCommandStatemachine* asyncState = testCommand.getAsyncStatemachine(oneTest.m_cmdType);
    asyncState->setCommandTimeout(1);
    std::uint16_t cycleExpired = 100;  // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testCommand);
      if (result != AxisProfileResult::STS_PENDING)
      {
        RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n    Execute timeout.");
    }
  }
  log(L"\n");

  log(L"\n<b>  Test error case of thread locker: </b>");
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};

    log(L"\n    Test of thread locker on %s ...", oneTest.m_cmdName.c_str());
    TestDataVariantU16 data {L"Add invalid data", 0x0000, true};
    setResponseVariant(&mockClient, data);
    testCommand.resetAllAxisprofileCommands();

    result = oneTest.m_testFun(testCommand);
    RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);

    auto lambdaFun = [&testCommand, &oneTest]
    {
      return oneTest.m_testFun(testCommand);
    };
    std::future<AxisProfileResult> anotherThread = std::async(std::launch::async, lambdaFun);
    result = anotherThread.get();
    RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);
  }
  log(L"\n");

  return S_OK;
}

void TestCanopenCommand::setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16& data)
{
  flatbuffers::FlatBufferBuilder builder {};
  builder.ForceDefaults(true);
  std::uint8_t buffer[4];
  std::size_t dataSize = 4;
  (void)memcpy(buffer, &data.m_data, dataSize);
  auto vec = builder.CreateVector(buffer, dataSize);
  comm::ethercat::master::fbs::SDOResponseBuilder paramResponseBuilder(builder);
  if (data.m_addData)
  {
    paramResponseBuilder.add_data(vec);
  }
  auto exchangeDataResponse = paramResponseBuilder.Finish();
  comm::ethercat::master::fbs::SDOBuilder paramBuilder(builder);
  paramBuilder.add_response(exchangeDataResponse);
  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);
  comm::datalayer::Variant variant;
  variant.copyFlatbuffers(builder);
  client->m_callbackVariant = variant;
}

HRESULT TestCanopenCommand::testCommandAssignedOpMode()
{
  log(L"\n<b>Test commands assigned op mode.");
  struct CommandsTestCase
  {
    std::wstring m_testName;                // test name
    std::function<AxisProfileResult(canopen::CanopenCommandDrive& cmd)> m_testFun;
    std::uint16_t m_setStatusWord;               // set status word
    std::uint16_t m_setDataState;                // set of datalayer value
    AxisProfileResult m_expectedResult;     // expected return value
    AxisProfileCommandType m_cmdType;       // command type
    AxisprofileCommandState m_cmdState = AxisprofileCommandState::CMD_STATE_ACTIVE;     // command state
   };

  std::vector<CommandsTestCase> testCase {
    {L"ResetReAxs",                         [](canopen::CanopenCommandDrive& cmd){return cmd.resetReferencingAxis();},                                         0x0000,        0x0000,      AxisProfileResult::STS_OK,               AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS},
    {L"RefAxs normal case encoder1",        [](canopen::CanopenCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_1);},            0x1400,        0x0000,      AxisProfileResult::STS_OK,               AxisProfileCommandType::CMD_REFERENCING_AXIS},
    {L"RefAxs clear",                       [](canopen::CanopenCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_1);},            0x1400,        0x0000,      AxisProfileResult::STS_OK,               AxisProfileCommandType::CMD_REFERENCING_AXIS,  AxisprofileCommandState::CMD_STATE_ABORTING},
  };

  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  comm::axisprofile::ProfileAssignedInfo profileAssigned {true};
  mappingHandlerMockup.setReturnValue(profileAssigned);
  test::MockAxisProfileProperty property {};
  comm::axisprofile::canopen::CanopenParamAccess paramAccess(m_mockObjs.m_profileInfo);

  AxisProfileResult result {AxisProfileResult::STS_OK};  
  for (const auto& oneTest : testCase)
  {
    log(L"\n  Test of %s ...", oneTest.m_testName.c_str());
    if (oneTest.m_cmdType == AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS)
    {
      m_mockObjs.m_at.m_atData.m_opModeDisplay = 8;
      m_mockObjs.m_mdt.m_mdtData.m_opMode = 8;
    }

    if (oneTest.m_cmdType == AxisProfileCommandType::CMD_REFERENCING_AXIS)
    {
      m_mockObjs.m_at.m_atData.m_opModeDisplay = 6;
      m_mockObjs.m_mdt.m_mdtData.m_opMode = 6;
    }

    canopen::CanopenCommandDrive testCommand {m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt};
    canopen::CanopenStatusControlDrive testStatusControl(m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt);
    test::MockAxisProfileDeviceParam testDeviceParam{};
    test::MockCategory mockCategory;
    AuxJobHandler auxJobHandler {};
    comm::axisprofile::OpModeAvailInfo opModeAvailInfo{true, true, true}; 
    testDeviceParam.m_opModeAvailInfo = &opModeAvailInfo;

    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&m_mockObjs.m_profileInfo.m_profileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testStatusControl);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, m_mockObjs.m_scaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);

    testCommand.booting();
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(static_cast<test::MockDatalayerClient*>(m_mockObjs.m_client), data);
    m_mockObjs.m_at.m_statusWord.m_word = oneTest.m_setStatusWord;

    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testCommand);
      if ((oneTest.m_cmdType == AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS))
      {
        result = testCommand.executeResetReferencingAxis();
      }
      if ((oneTest.m_cmdType == AxisProfileCommandType::CMD_REFERENCING_AXIS))
      {
        if (cycleExpired<80)
        {
          m_mockObjs.m_at.m_atData.m_opModeDisplay = 8;
          m_mockObjs.m_mdt.m_mdtData.m_opMode = 8;
          testCommand.booting();
          setResponseVariant(static_cast<test::MockDatalayerClient*>(m_mockObjs.m_client), data);
          m_mockObjs.m_at.m_statusWord.m_word = oneTest.m_setStatusWord;
        }
        if (oneTest.m_cmdState == AxisprofileCommandState::CMD_STATE_ABORTING && !m_specialStepFlag)
        {
          m_specialStepFlag = true; 
          testCommand.abortAxisProfileCommand(AxisProfileCommandType::CMD_REFERENCING_AXIS);
        }
        result = testCommand.executeReferencingAxis();
      }

      if (result != AxisProfileResult::STS_PENDING)
      {
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
  }
  m_specialStepFlag = false;
  log(L"\n");
  return S_OK;
}

HRESULT TestCanopenCommand::testSetReferencingPosition()
{
  log(L"\n<b>Test setReferencePosition command...</b>");

log(L"\n<b>Test referencingAxis.");
  struct CommandsTestCase
  {
    std::wstring m_testName;              // test name
    std::function<AxisProfileResult(canopen::CanopenCommandDrive& cmd)> m_testFun; // test function
    std::uint16_t m_setStatusWord;             // set status word
    std::uint16_t m_setDataState;              // set of datalayer value
    AxisProfileResult m_expectedResult;   // expected return value
    AxisProfileCommandType m_cmdType;     // command type
    DeviceSeries m_deviceSeries;          // device series
    AxisprofileCommandState m_cmdState = AxisprofileCommandState::CMD_STATE_ACTIVE;     // command state
    bool m_inputassignedInfo{true};
    bool m_outputassignedInfo{true};
   };

  std::vector<CommandsTestCase> testCase 
  {
    /* m_testName           */  {L"Set Reference Position into device via indra drive",
    /* m_testFun            */  [](canopen::CanopenCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_DEVICE);},
    /* m_setStatusWord      */  0x1400,
    /* m_setDataState       */  0x0000,
    /* m_expectedResult     */  AxisProfileResult::ERR_NOTIMPLEMENTED,
    /* m_cmdType            */  AxisProfileCommandType::CMD_SET_REFERENCING_POS,
    /* m_deviceSeries       */  DeviceSeries::INDRADRIVE
    /* m_cmdState           */
    /* m_inputassignedInfo  */
    /* m_outputassignedInfo */  },

    /* m_testName           */  {L"Set Reference Position into device via ctrlX drive",
    /* m_testFun            */  [](canopen::CanopenCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_DEVICE);},
    /* m_setStatusWord      */  0x1400,
    /* m_setDataState       */  0x0000,
    /* m_expectedResult     */  AxisProfileResult::ERR_NOTIMPLEMENTED,
    /* m_cmdType            */  AxisProfileCommandType::CMD_SET_REFERENCING_POS,
    /* m_deviceSeries       */  DeviceSeries::CTRLXDRIVE
    /* m_cmdState           */
    /* m_inputassignedInfo  */
    /* m_outputassignedInfo */  },
    /* m_testName           */  {L"Set Reference Position into device via 3rd party drive",
    /* m_testFun            */  [](canopen::CanopenCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_DEVICE);},
    /* m_setStatusWord      */  0x1400,
    /* m_setDataState       */  0x0000,
    /* m_expectedResult     */  AxisProfileResult::ERR_NOTIMPLEMENTED,
    /* m_cmdType            */  AxisProfileCommandType::CMD_SET_REFERENCING_POS,
    /* m_deviceSeries       */  DeviceSeries::NOTINDICATES
    /* m_cmdState           */
    /* m_inputassignedInfo  */
    /* m_outputassignedInfo */  },

    /* m_testName           */  {L"Set Reference Position into control",
    /* m_testFun            */  [](canopen::CanopenCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_PROFILE);},
    /* m_setStatusWord      */  0x1400,
    /* m_setDataState       */  0x0000,
    /* m_expectedResult     */  AxisProfileResult::ERR_NOTIMPLEMENTED,
    /* m_cmdType            */  AxisProfileCommandType::CMD_SET_REFERENCING_POS,
    /* m_deviceSeries       */  DeviceSeries::CTRLXDRIVE
    /* m_cmdState           */
    /* m_inputassignedInfo  */
    /* m_outputassignedInfo */  },

    /* m_testName           */  {L"Set Reference Position with none",
    /* m_testFun            */  [](canopen::CanopenCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_NONE);},
    /* m_setStatusWord      */  0x1400,
    /* m_setDataState       */  0x0000,
    /* m_expectedResult     */  AxisProfileResult::ERR_NOTIMPLEMENTED,
    /* m_cmdType            */  AxisProfileCommandType::CMD_SET_REFERENCING_POS,
    /* m_deviceSeries       */  DeviceSeries::CTRLXDRIVE
    /* m_cmdState           */
    /* m_inputassignedInfo  */
    /* m_outputassignedInfo */  },

    /* m_testName           */  {L"Set Reference Position with unknown method",
    /* m_testFun            */  [](canopen::CanopenCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_1, RefMode::UNDEF_METHOD);},
    /* m_setStatusWord      */  0x1400,
    /* m_setDataState       */  0x0000,
    /* m_expectedResult     */  AxisProfileResult::ERR_NOTIMPLEMENTED,
    /* m_cmdType            */  AxisProfileCommandType::CMD_SET_REFERENCING_POS,
    /* m_deviceSeries       */  DeviceSeries::CTRLXDRIVE
    /* m_cmdState           */
    /* m_inputassignedInfo  */
    /* m_outputassignedInfo */  },

    /* m_testName           */  {L"Set Reference Position but op mode not assigned",
    /* m_testFun            */  [](canopen::CanopenCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_NONE);},
    /* m_setStatusWord      */  0x1400,
    /* m_setDataState       */  0x0000,
    /* m_expectedResult     */  AxisProfileResult::ERR_NOTIMPLEMENTED,
    /* m_cmdType            */  AxisProfileCommandType::CMD_SET_REFERENCING_POS,
    /* m_deviceSeries       */  DeviceSeries::CTRLXDRIVE,
    /* m_cmdState           */  AxisprofileCommandState::CMD_STATE_ACTIVE,
    /* m_inputassignedInfo  */  true,
    /* m_outputassignedInfo */  false},
  };

  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  comm::axisprofile::canopen::CanopenParamAccess paramAccess(m_mockObjs.m_profileInfo);
  test::MockAxisProfileDeviceParam testDeviceParam{};
  test::MockCategory mockCategory;
  AuxJobHandler auxJobHandler {};
  test::MockAxisProfileProperty property {};

  AxisProfileResult result {AxisProfileResult::STS_OK};  
  for (const auto& oneTest : testCase)
  {
    log(L"\n  Test of %s ...", oneTest.m_testName.c_str());
    const comm::axisprofile::ProfileAssignedInfo assignedInfo{oneTest.m_inputassignedInfo, oneTest.m_outputassignedInfo};
    property.setDeviceSeries(oneTest.m_deviceSeries);
    mappingHandlerMockup.setReturnValue(assignedInfo);
    canopen::CanopenCommandDrive testCommand {m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&m_mockObjs.m_profileInfo.m_profileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, m_mockObjs.m_scaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);

    testCommand.booting();
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(static_cast<test::MockDatalayerClient*>(m_mockObjs.m_client), data);
    m_mockObjs.m_at.m_statusWord.m_word = oneTest.m_setStatusWord;

    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testCommand);
      if (result != AxisProfileResult::STS_PENDING)
      {
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
  }

  log(L"\n"); 
  return S_OK;
}

HRESULT TestCanopenCommand::testAuxJobsHandling()
{
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  comm::axisprofile::ProfileAssignedInfo profileAssigned {true};
  mappingHandlerMockup.setReturnValue(profileAssigned);

  AuxJobHandler auxJobHandler;
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&m_mockObjs.m_profileInfo.m_profileTrace});
  sharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);
  sharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);

  canopen::CanopenCommandDrive testCommand {m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt};
  testCommand.setSharingHandler(sharing.get());

  log(L"\n<b>Test adding aux jobs...</b>");
  auto result = testCommand.addAuxCyclicJobs();
  RETURN_TEST_RESULT(result, L"\nAdding aux job failed.", AxisProfileResult::STS_OK);

  log(L"\n<b>Test running aux jobs...</b>");
  result = auxJobHandler.runAllAuxJobs();
  RETURN_TEST_RESULT(result, L"\nAdding aux job failed.", AxisProfileResult::STS_OK);

  log(L"\n");
  return S_OK;
}

HRESULT TestCanopenCommand::testCheckIfDriveHalted()
{
  log(L"\n<b>Test checkIfAxisHalted.");
  struct CommandsTestCase
  {
    std::wstring m_testName;              // test name
    std::function<AxisProfileResult(canopen::CanopenCommandDrive& cmd)> m_testFun; // test function
    std::uint16_t m_setControlWord;            // set control word
    std::uint16_t m_setDataState;              // set of datalayer value
    DeviceSeries m_deviceSeries;          // set device series
    AxisProfileResult m_expectedResult;   // expected return value
   };

   std::vector<CommandsTestCase> testCase {
    {L"checkIfDeviceHalt with undefined series", [](canopen::CanopenCommandDrive& cmd){bool value;    return cmd.checkIfDeviceHalt(value);},  0x010F, 0x0000, DeviceSeries::UNDEFINED,     AxisProfileResult::STS_OK},
    {L"checkIfDeviceHalt with indra drive",      [](canopen::CanopenCommandDrive& cmd){bool value;    return cmd.checkIfDeviceHalt(value);},  0x010F, 0x0010, DeviceSeries::INDRADRIVE,    AxisProfileResult::STS_OK },
    {L"checkIfDeviceHalt with ctrlX drive",      [](canopen::CanopenCommandDrive& cmd){bool value;    return cmd.checkIfDeviceHalt(value);},  0x010F, 0x0010, DeviceSeries::CTRLXDRIVE,    AxisProfileResult::STS_OK },
    {L"checkIfDeviceHalt with other drive",      [](canopen::CanopenCommandDrive& cmd){bool value;    return cmd.checkIfDeviceHalt(value);},  0x010F, 0x0000, DeviceSeries::NOTINDICATES,  AxisProfileResult::STS_OK },
  
  };
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  comm::axisprofile::canopen::CanopenParamAccess paramAccsss(m_mockObjs.m_profileInfo);
  test::MockAxisProfileDeviceParam deviceParamMockup{};
  test::MockCategory mockCategory;
  AuxJobHandler auxJobHandler {};
  test::MockAxisProfileProperty property {};

  AxisProfileResult result {AxisProfileResult::STS_OK};  
  for (const auto& oneTest : testCase)
  {
    log(L"\n  Test of %s ...", oneTest.m_testName.c_str());
    canopen::CanopenCommandDrive testCommand {m_mockObjs.m_profileInfo, &m_mockObjs.m_at, &m_mockObjs.m_mdt};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&m_mockObjs.m_profileInfo.m_profileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccsss);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, m_mockObjs.m_scaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
    testCommand.booting();
    property.setDeviceSeries(oneTest.m_deviceSeries);
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(static_cast<test::MockDatalayerClient*>(m_mockObjs.m_client), data);
    m_mockObjs.m_mdt.m_controlWord.m_word = oneTest.m_setControlWord;
    m_mockObjs.m_at.m_statusWord.m_word = 0x140F;

    std::uint16_t cycleExpired = 100; // max cycle for the test

    while (true)
    {
      result = oneTest.m_testFun(testCommand);
      if (result != AxisProfileResult::STS_PENDING)
      {
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
  }

  return S_OK;  
}
