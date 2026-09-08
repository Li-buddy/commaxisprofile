#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/sercos_param_access.h"
#include "profiles/aux_job/aux_job_handler.h"
#include "test_sercos_command.h"
#include "profiles/sercos/drive/sercos_command_drive.h"

// Mockups
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_status_control.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_axisprofile_category.h"
#include "mockup/mock_scaling.h"

#include <functional>
#include <future>
#include <memory>
#include <vector>

using namespace comm::axisprofile;
using namespace test;

REGISTER_TEST(TestSercosCommand);
REGISTER_TEST_TO_GROUP(TestSercosCommand, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosCommand, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosCommand, Profiles_Sercos);


HRESULT TestSercosCommand::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Sercos profile command class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testMiscellaneous();
  hresult |= testCmdGernalProcuder();
  hresult |= testSimpleCommands();
  hresult |= testParameterCommands();
  hresult |= testRisingAndTimeoutError();
  hresult |= testInitSetAbs();
  log(L"\n");
  return hresult;
}

HRESULT TestSercosCommand::testMiscellaneous() const
{
  log(L"\n<b>Test miscellaneous.</b>");
  MockAxisProfileTrace mockProfileTrace {};
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";

  sercos::SercosCommandDrive testCommand {profileBasicInfo, &atData};
  AxisProfileResult result {AxisProfileResult::STS_OK};
  AsyncCommandStatemachine* asyncState {nullptr};
  AxisprofileCommandState state {AxisprofileCommandState::CMD_STATE_UNDEFINED};

  log(L"\n  Test resetAllAxisprofileCommands ...");
  result = testCommand.resetAllAxisprofileCommands();
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n  Test resetAxisProfileCommandState ...");
  testCommand.resetAllAxisprofileCommands();
  result = testCommand.resetAxisProfileCommandState(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n  Test resetAxisProfileCommandState error case ...");
  testCommand.resetAllAxisprofileCommands();
  result = testCommand.resetAxisProfileCommandState(static_cast<AxisProfileCommandType>(-1));
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND);

  log(L"\n  Test getAsyncStatemachine ...");
  testCommand.resetAllAxisprofileCommands();
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  LOG_TEST_RESULT_SET_PTR(asyncState, L"\n  should valid pointer.");

  log(L"\n  Test getAsyncStatemachine error case...");
  testCommand.resetAllAxisprofileCommands();
  asyncState = testCommand.getAsyncStatemachine(static_cast<AxisProfileCommandType>(-1));
  LOG_TEST_RESULT_PTR(asyncState, L"\n  unexpected return value.", nullptr);

  log(L"\n  Test abortAxisProfileCommand ...");
  testCommand.resetAllAxisprofileCommands();
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  asyncState->switchToCertainStep(AsyncCommandStatemachine::CommandStep::PARAM_INIT);
  result = testCommand.abortAxisProfileCommand(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n  Test abortAxisProfileCommand error case ...");
  testCommand.resetAllAxisprofileCommands();
  result = testCommand.abortAxisProfileCommand(static_cast<AxisProfileCommandType>(-1));
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND);

  log(L"\n  Test getAxisProfileCommandState idle...");
  testCommand.resetAllAxisprofileCommands();
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  asyncState->switchToCertainStep(AsyncCommandStatemachine::CommandStep::STEP_IDLE);
  state = testCommand.getAxisProfileCommandState(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  RETURN_TEST_RESULT(state, L"\n  unexpected return value.", AxisprofileCommandState::CMD_STATE_IDLE);

  log(L"\n  Test getAxisProfileCommandState aborted...");
  testCommand.resetAllAxisprofileCommands();
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  asyncState->switchToAbortStep();
  state = testCommand.getAxisProfileCommandState(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  RETURN_TEST_RESULT(state, L"\n  unexpected return value.", AxisprofileCommandState::CMD_STATE_ABORTING);

  log(L"\n  Test getAxisProfileCommandState active...");
  testCommand.resetAllAxisprofileCommands();
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  asyncState->switchToCertainStep(AsyncCommandStatemachine::CommandStep::PARAM_INIT);
  state = testCommand.getAxisProfileCommandState(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  RETURN_TEST_RESULT(state, L"\n  unexpected return value.", AxisprofileCommandState::CMD_STATE_ACTIVE);

  log(L"\n  Test raiseAndGetStatemachine...");
  testCommand.resetAllAxisprofileCommands();
  result = testCommand.raiseAndGetStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR, asyncState);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n  Test raiseAndGetStatemachine error case...");
  testCommand.resetAllAxisprofileCommands();
  result = testCommand.raiseAndGetStatemachine(static_cast<AxisProfileCommandType>(-1), asyncState);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND);

  log(L"\n  Test resultHandlingCycle STS_OK...");
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  AxisProfileResult tmpResult = AxisProfileResult::STS_OK;
  result = testCommand.resultHandlingCycle(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR, *asyncState, tmpResult);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);

  log(L"\n  Test resultHandlingCycle STS_PENDING...");
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  tmpResult = AxisProfileResult::STS_PENDING;
  result = testCommand.resultHandlingCycle(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR, *asyncState, tmpResult);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_PENDING);

  log(L"\n  Test resultHandlingCycle error...");
  asyncState = testCommand.getAsyncStatemachine(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR);
  tmpResult = AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  result = testCommand.resultHandlingCycle(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR, *asyncState, tmpResult);
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::ERR_WRONG_DEVICE_STATE);

  log(L"\n");
  return S_OK;
}

HRESULT TestSercosCommand::testCmdGernalProcuder()
{
  log(L"\n<b>Test general command procedures.</b>");
  struct MiscellaneousTestCase
  {
    std::wstring m_testName;                // test name
    std::function<AxisProfileResult(sercos::SercosParamAccess& paramAccess, const sercos::ProcedureCommands paramIDN, AsyncCommandStatemachine& asyncState)> m_testFun;
    std::uint16_t m_setDataState;           // set datalayer data
    AxisProfileResult m_expectedResult;     // set expected return value
  };

  std::vector<MiscellaneousTestCase> testCase {
    {L"executeSercosProcedureCommand",   [](sercos::SercosParamAccess& paramAccess, const sercos::ProcedureCommands paramIDN, AsyncCommandStatemachine& asyncState){return paramAccess.executeSercosProcedureCommandAsync(paramIDN, asyncState);},   sercos::SERCOS_COMMAND_STATUS_FINISH,     AxisProfileResult::STS_OK},
    {L"setSercosProcedureCommand",       [](sercos::SercosParamAccess& paramAccess, const sercos::ProcedureCommands paramIDN, AsyncCommandStatemachine& asyncState){return paramAccess.setSercosProcedureCommandAsync(paramIDN, asyncState);},       sercos::SERCOS_COMMAND_STATUS_FINISH,     AxisProfileResult::STS_OK},
    {L"interruptSercosProcedureCommand", [](sercos::SercosParamAccess& paramAccess, const sercos::ProcedureCommands paramIDN, AsyncCommandStatemachine& asyncState){return paramAccess.interruptSercosProcedureCommandAsync(paramIDN, asyncState);}, sercos::SERCOS_COMMAND_STATUS_INTERRUPT,  AxisProfileResult::STS_OK},
    {L"clearSercosProcedureCommand",     [](sercos::SercosParamAccess& paramAccess, const sercos::ProcedureCommands paramIDN, AsyncCommandStatemachine& asyncState){return paramAccess.clearSercosProcedureCommandAsync(paramIDN, asyncState);},     sercos::SERCOS_COMMAND_STATUS_NOT_SET,    AxisProfileResult::STS_OK},
  };

  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  std::uint32_t devAddress = 1;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);

  MockAxisProfileStatusControl mockStatusControl {};
  mockStatusControl.m_statusWord |= (1 << comm::axisprofile::sercos::SERCOS_STSWORD_PROCEDURE_CMDBIT_OFFSET); // set procedure command bit)
  AxisProfileSharingHandler shareHandler {&profileBasicInfo.m_profileTrace};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &mockStatusControl);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);

  AsyncCommandStatemachine asyncState {};
  for (const auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};

    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(&mockClient, data);

    asyncState.resetCommandState();
    asyncState.switchToCmdStartStep();

    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(sercosParamAccess, sercos::ProcedureCommands::S_0_0099_RESET_CLASS_1_DIAGNOSTIC, asyncState);
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

HRESULT TestSercosCommand::testSimpleCommands()
{
  log(L"\n<b>Test simple commands.</b>");
  struct CommandsTestCase
  {
    std::wstring m_testName;              // test name
    AxisProfileCommandType m_cmdType;     // command type
    std::function<AxisProfileResult(sercos::SercosCommandDrive& cmd)> m_testFun;
    std::uint16_t m_encoderOneType;       // set of encoder type1
    std::uint16_t m_encoderTwoType;       // set of encoder type2    
    std::uint16_t m_setDataState;         // set of datalayer value
    AxisProfileResult m_expectedResult;   // expected return value
   };

  std::vector<CommandsTestCase> testCase {
    {L"ClearDeviceError",                          AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR,      [](sercos::SercosCommandDrive& cmd){return cmd.clearDeviceError();},                                                                   0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
                                                                                                                                                                                                                                          
    {L"Enable setAndCheckPositiveStop",            AxisProfileCommandType::CMD_SET_CHECK_POSITIVE_STOP, [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckPositiveStop(AxisProfilePositiveStop::ENABLE);},                             0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"Disable setAndCheckPositiveStop",           AxisProfileCommandType::CMD_SET_CHECK_POSITIVE_STOP, [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckPositiveStop(AxisProfilePositiveStop::DISABLE);},                            0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_NOT_SET,     AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"Abort setAndCheckPositiveStop",             AxisProfileCommandType::CMD_SET_CHECK_POSITIVE_STOP, [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckPositiveStop(AxisProfilePositiveStop::ABORT);},                              0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_INTERRUPT,   AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"Undefined type of fixedstop ",              AxisProfileCommandType::CMD_SET_CHECK_POSITIVE_STOP, [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckPositiveStop(AxisProfilePositiveStop::UNDEFINED);},                          0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_NOT_SET,     AxisProfileResult::ERR_NOTIMPLEMENTED},
                                                                                                                                                                                                                                          
    {L"SetAbPos normal power on",                  AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION,   [](sercos::SercosCommandDrive& cmd){return cmd.setAbsolutePosition(0.0, AxisProfileEncoder::ENCODER_1, true);},                        0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetAbPos normal power off",                 AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION,   [](sercos::SercosCommandDrive& cmd){return cmd.setAbsolutePosition(0.0, AxisProfileEncoder::ENCODER_1, false);},                       0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetAbPos normal encoder2",                  AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION,   [](sercos::SercosCommandDrive& cmd){return cmd.setAbsolutePosition(0.0, AxisProfileEncoder::ENCODER_2, true);},                        0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetAbPos error encoder type",               AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION,   [](sercos::SercosCommandDrive& cmd){return cmd.setAbsolutePosition(0.0, AxisProfileEncoder::UNDEFINED, false);},                       0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetAbPos error encoder one token",          AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION,   [](sercos::SercosCommandDrive& cmd){return cmd.setAbsolutePosition(0.0, AxisProfileEncoder::ENCODER_1, true);},                          0x0,        0x0,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetAbPos error encoder two token",          AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION,   [](sercos::SercosCommandDrive& cmd){return cmd.setAbsolutePosition(0.0, AxisProfileEncoder::ENCODER_2, true);},                          0x0,        0x0,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"ResetAbPos",                                AxisProfileCommandType::CMD_RESET_ABSOLUTE_POSITION, [](sercos::SercosCommandDrive& cmd){return cmd.resetAbsolutePosition();},                                                              0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_NOT_SET,     AxisProfileResult::ERR_NOTIMPLEMENTED},
                                                                                                                                                                                                                                          
    {L"ResetRefAxs",                               AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS,  [](sercos::SercosCommandDrive& cmd){return cmd.resetReferencingAxis();},                                                               0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_NOT_SET,     AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"RefAxs normal case encoder1",               AxisProfileCommandType::CMD_REFERENCING_AXIS,        [](sercos::SercosCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_1);},                                  0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"RefAxs normal case encoder2",               AxisProfileCommandType::CMD_REFERENCING_AXIS,        [](sercos::SercosCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::ENCODER_2);},                                  0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"RefAxs error encoder type",                 AxisProfileCommandType::CMD_REFERENCING_AXIS,        [](sercos::SercosCommandDrive& cmd){return cmd.referencingAxis(0.0, AxisProfileEncoder::UNDEFINED);},                                  0x140,      0x140,      sercos::SERCOS_COMMAND_STATUS_FINISH,      AxisProfileResult::ERR_NOTIMPLEMENTED},
                                                
    {L"setReferencePosition to device",            AxisProfileCommandType::CMD_SET_REFERENCING_POS,     [](sercos::SercosCommandDrive& cmd){return cmd.setReferencingPosition(100, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_DEVICE);},  0x140,      0x140,      0x0060,                                    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setReferencePosition to profile",           AxisProfileCommandType::CMD_SET_REFERENCING_POS,     [](sercos::SercosCommandDrive& cmd){return cmd.setReferencingPosition(600, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_PROFILE);}, 0x140,      0x140,      0x0060,                                    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setReferencePosition to none",              AxisProfileCommandType::CMD_SET_REFERENCING_POS,     [](sercos::SercosCommandDrive& cmd){return cmd.setReferencingPosition(600, AxisProfileEncoder::ENCODER_1, RefMode::SET_NONE);},        0x140,      0x140,      0x0060,                                    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setReferencePosition with unclear method",  AxisProfileCommandType::CMD_SET_REFERENCING_POS,     [](sercos::SercosCommandDrive& cmd){return cmd.setReferencingPosition(600, AxisProfileEncoder::ENCODER_1, RefMode::UNDEF_METHOD);},    0x140,      0x140,      0x0060,                                    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setReferencePosition normal case encoder2", AxisProfileCommandType::CMD_SET_REFERENCING_POS,     [](sercos::SercosCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_2, RefMode::SET_POS_DEVICE);},  0x140,      0x140,      0x0060,                                    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setReferencePosition wrong configuration",  AxisProfileCommandType::CMD_SET_REFERENCING_POS,     [](sercos::SercosCommandDrive& cmd){return cmd.setReferencingPosition(0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_DEVICE);},  0x140,      0x140,      0x00A4,                                    AxisProfileResult::ERR_NOTIMPLEMENTED},

    {L"checkAxisHomed with none",                  AxisProfileCommandType::CMD_CHECK_AXIS_HOMED,        [](sercos::SercosCommandDrive& cmd){bool isHomed; return cmd.checkAxisHomed(isHomed);},                                                0x140,      0x140,      0x0060,                                    AxisProfileResult::ERR_NOTIMPLEMENTED},

  };

  MockAxisProfileTrace mockProfileTrace{};
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  std::uint32_t deviceAddress = 1;
  AcknowledgeTelegram atData;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);
  test::MockAxisProfileDeviceParam mockSercosDevcieParam;
  test::MockAxisProfileCyclicData mockCyclicData;
  test::MockCategory mockCategory;
  MockAxisProfileStatusControl mockStatusControl {};
  mockStatusControl.m_statusWord |= (1 << comm::axisprofile::sercos::SERCOS_STSWORD_PROCEDURE_CMDBIT_OFFSET); // set procedure command bit)
  AuxJobHandler auxJobHandler {};
  for (const auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};

    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    DeviceCfgData testCfgData;
    testCfgData.m_encoder1Property = oneTest.m_encoderOneType;
    testCfgData.m_encoder2Property = oneTest.m_encoderTwoType;
    testCfgData.m_softwareLimitActive = true;
    mockSercosDevcieParam.setReturnCfgData(testCfgData);

    sercos::SercosCommandDrive testCommand {profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &mockSercosDevcieParam);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclicData);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &mockStatusControl);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);

    testCommand.booting();
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(&mockClient, data);

    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      auto asyncState = testCommand.getAsyncStatemachine(oneTest.m_cmdType);
      asyncState->getCommandStep();
      if ((oneTest.m_cmdType == AxisProfileCommandType::CMD_SET_REFERENCING_POS) && (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS))
      {
        TestDataVariantU16 dataVariantU16 {L"Add invalid data", sercos::SERCOS_COMMAND_STATUS_FINISH, true};
        setResponseVariant(&mockClient, dataVariantU16);
      }
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

HRESULT TestSercosCommand::testParameterCommands()
{
  log(L"\n<b>Test parameter commands.</b>");
  struct CommandsTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosCommandDrive& cmd)> m_testFun;
    TorqueScalingType m_torqueScalingType;      // set of torque scaling type
    std::uint16_t m_setDataState;               // set of datalayer value
    AxisProfileResult m_expectedResult;         // expected return value
   };

  std::vector<CommandsTestCase> testCase {
    {L"SetClampingToruqe normal case Linear 1",           [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(1.0);},                     TorqueScalingType::LINEAR,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe normal case Linear 2",           [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(1.0);},                     TorqueScalingType::PERCENTAGE,        0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe normal case Rotary 1",           [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(1.0);},                     TorqueScalingType::ROTARY,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe normal case Rotary 2",           [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(1.0);},                     TorqueScalingType::PERCENTAGE,        0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe normal case Percentage 1",       [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(1.0);},                     TorqueScalingType::LINEAR,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe normal case Percentage 2",       [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(1.0);},                     TorqueScalingType::ROTARY,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe normal case Percentage 3",       [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(1.0);},                     TorqueScalingType::PERCENTAGE,        0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe error input value",              [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(0.0);},                     TorqueScalingType::LINEAR,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe error scale type",               [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(0.0);},                     TorqueScalingType::LINEAR,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"SetClampingToruqe error scale type inconsistent",  [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckClampingTorque(0.0);},                     TorqueScalingType::ROTARY,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},

    {L"getClampingTorque normal case Linear 1",           [](sercos::SercosCommandDrive& cmd){double value;  return cmd.getClampingTorque(&value);},           TorqueScalingType::LINEAR,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getClampingTorque normal case Linear 2",           [](sercos::SercosCommandDrive& cmd){double value;  return cmd.getClampingTorque(&value);},           TorqueScalingType::PERCENTAGE,        0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getClampingTorque normal case Rotary 1",           [](sercos::SercosCommandDrive& cmd){double value;  return cmd.getClampingTorque(&value);},           TorqueScalingType::ROTARY,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getClampingTorque normal case Rotary 2",           [](sercos::SercosCommandDrive& cmd){double value;  return cmd.getClampingTorque(&value);},           TorqueScalingType::PERCENTAGE,        0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getClampingTorque normal case Percentage 1",       [](sercos::SercosCommandDrive& cmd){double value;  return cmd.getClampingTorque(&value);},           TorqueScalingType::LINEAR,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getClampingTorque normal case Percentage 2",       [](sercos::SercosCommandDrive& cmd){double value;  return cmd.getClampingTorque(&value);},           TorqueScalingType::ROTARY,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getClampingTorque normal case Percentage 3",       [](sercos::SercosCommandDrive& cmd){double value;  return cmd.getClampingTorque(&value);},           TorqueScalingType::PERCENTAGE,        0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getClampingTorque error null input pointer",       [](sercos::SercosCommandDrive& cmd){               return cmd.getClampingTorque(nullptr);},          TorqueScalingType::LINEAR,            0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},

    {L"checkStandStill normal case standstill",           [](sercos::SercosCommandDrive& cmd){bool value;    return cmd.checkStandStill(&value);},             TorqueScalingType::UNDEFINED,         0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"checkStandStill normal case not standstill",       [](sercos::SercosCommandDrive& cmd){bool value;    return cmd.checkStandStill(&value);},             TorqueScalingType::UNDEFINED,         0x0001,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"checkStandStill error null input pointer",         [](sercos::SercosCommandDrive& cmd){               return cmd.checkStandStill(nullptr);},            TorqueScalingType::UNDEFINED,         0x0001,    AxisProfileResult::ERR_NOTIMPLEMENTED},

    {L"setAndCheckStandStillWindow normal case",          [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckStandStillWindow(1.0);},                   TorqueScalingType::UNDEFINED,         0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"setAndCheckStandStillWindow error input value",    [](sercos::SercosCommandDrive& cmd){return cmd.setAndCheckStandStillWindow(0.0);},                   TorqueScalingType::UNDEFINED,         0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},

    {L"getStandStillWindow normal case",                  [](sercos::SercosCommandDrive& cmd){double value;  return cmd.getStandStillWindow(&value);},         TorqueScalingType::UNDEFINED,         0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"getStandStillWindow error null input pointer",     [](sercos::SercosCommandDrive& cmd){               return cmd.getStandStillWindow(nullptr);},        TorqueScalingType::UNDEFINED,         0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},

    {L"activateCyclicStandStillCheck normal case",        [](sercos::SercosCommandDrive& cmd){return cmd.activateCyclicStandStillCheck();},                    TorqueScalingType::UNDEFINED,         0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
    {L"deactivateCyclicStandStillCheck normal case",      [](sercos::SercosCommandDrive& cmd){return cmd.deactivateCyclicStandStillCheck();},                  TorqueScalingType::UNDEFINED,         0x0000,    AxisProfileResult::ERR_NOTIMPLEMENTED},
  };

  MockAxisProfileTrace mockProfileTrace{};
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t deviceAddress = 1;
  AcknowledgeTelegram atData;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  AuxJobHandler auxJobHandler {};
  MockAxisProfileStatusControl mockStatusControl {};
  mockStatusControl.m_statusWord |= (1 << comm::axisprofile::sercos::SERCOS_STSWORD_PROCEDURE_CMDBIT_OFFSET); // set procedure command bit)

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);

  for (auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};

    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    sercos::SercosCommandDrive testCommand{profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &mockStatusControl);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);

    testCommand.booting();
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(&mockClient, data);
    mockScaling.m_torqueType = oneTest.m_torqueScalingType;
    testCommand.m_cmdData.m_cycSSCheckActivated = true;

    std::uint16_t cycleExpired = 200; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testCommand);
      if (result != AxisProfileResult::STS_PENDING)
      {
        RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
  }
  log(L"\n");
  return S_OK;
}

HRESULT TestSercosCommand::testRisingAndTimeoutError()
{
  log(L"\n<b>Test rising failed and timeout case</b>");
  struct CommandsTestCase
  {
    std::wstring m_cmdName;             // test name
    AxisProfileCommandType m_cmdType;   // command type
    std::function<void(sercos::SercosCommandDrive& cmd)> m_preConditionFun; // pre-condition function
    std::function<AxisProfileResult(sercos::SercosCommandDrive& cmd)> m_testFun;
  };

  std::vector<CommandsTestCase> testCase {
    {L"clearDeviceError",                       AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR,           [](sercos::SercosCommandDrive& /*cmd*/){},                                                 [](sercos::SercosCommandDrive& cmd)  {               return cmd.clearDeviceError();}},
    {L"setAndCheckPositiveStop",                AxisProfileCommandType::CMD_SET_CHECK_POSITIVE_STOP,      [](sercos::SercosCommandDrive& /*cmd*/){},                                                 [](sercos::SercosCommandDrive& cmd)  {               return cmd.setAndCheckPositiveStop(AxisProfilePositiveStop::ENABLE);}},
    {L"setAndCheckClampingTorque",              AxisProfileCommandType::CMD_SET_CHECK_CLAMPING_TORQUE,    [](sercos::SercosCommandDrive& /*cmd*/){},                                                 [](sercos::SercosCommandDrive& cmd)  {               return cmd.setAndCheckClampingTorque(1.0);}},
    {L"getClampingTorque",                      AxisProfileCommandType::CMD_GET_CLAMPING_TORQUE,          [](sercos::SercosCommandDrive& /*cmd*/){},                                                 [](sercos::SercosCommandDrive& cmd)  {double value;  return cmd.getClampingTorque(&value);}},
    {L"checkStandStill",                        AxisProfileCommandType::CMD_CHECK_STANDSTILL,             [](sercos::SercosCommandDrive& /*cmd*/){},                                                 [](sercos::SercosCommandDrive& cmd)  {bool value;    return cmd.checkStandStill(&value);}},
    {L"setAndCheckStandStillWindow",            AxisProfileCommandType::CMD_SET_CHECK_STANDSTILL_WINDOW,  [](sercos::SercosCommandDrive& /*cmd*/){},                                                 [](sercos::SercosCommandDrive& cmd)  {               return cmd.setAndCheckStandStillWindow(10);}},
    {L"getStandStillWindow",                    AxisProfileCommandType::CMD_GET_STANDSTILL_WINDOW,        [](sercos::SercosCommandDrive& /*cmd*/){},                                                 [](sercos::SercosCommandDrive& cmd)  {double value;  return cmd.getStandStillWindow(&value);}},
    {L"activateCyclicStandStillCheck",          AxisProfileCommandType::CMD_ACTIVATE_STANDSTILL_CHECK,    [](sercos::SercosCommandDrive& cmd){cmd.m_cmdData.m_cycSSCheckActivated = false;},         [](sercos::SercosCommandDrive& cmd)  {               return cmd.activateCyclicStandStillCheck();}},
    {L"deactivateCyclicStandStillCheck",        AxisProfileCommandType::CMD_DEACTIVATE_STANDSTILL_CHECK,  [](sercos::SercosCommandDrive& cmd){cmd.m_cmdData.m_cycSSCheckActivated = true; },         [](sercos::SercosCommandDrive& cmd)  {               return cmd.deactivateCyclicStandStillCheck();}},
    {L"checkIfDriveHalted",                     AxisProfileCommandType::CMD_CHECK_DEVICE_HALT,            [](sercos::SercosCommandDrive& /*cmd*/){},                                                 [](sercos::SercosCommandDrive& cmd)  {bool value;    return cmd.checkIfDeviceHalt(value);}},
  };

  MockAxisProfileTrace mockProfileTrace {};
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockAxisProfileDeviceParam mockSercosDeviceParam;
  test::MockAxisProfileCyclicData mockCyclicData;
  MockAxisProfileStatusControl mockStatusControl {};
  AuxJobHandler auxJobHandler {};
  mockStatusControl.m_statusWord |= (1 << comm::axisprofile::sercos::SERCOS_STSWORD_PROCEDURE_CMDBIT_OFFSET); // set procedure command bit)

  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);

  log(L"\n<b>  Test of error case of rising failed: </b>");
  for (const auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};

    log(L"\n    Test of rising failed on %s ...", oneTest.m_cmdName.c_str());
    sercos::SercosCommandDrive testCommand{profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclicData);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &mockStatusControl);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);

    testCommand.booting();
    oneTest.m_preConditionFun(testCommand);
    testCommand.resetAllAxisprofileCommands();
    result = oneTest.m_testFun(testCommand);
    if (oneTest.m_cmdType == AxisProfileCommandType::CMD_CHECK_DEVICE_HALT)
    {
      RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::STS_PENDING);
    }
    else
    {
      RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);
    }
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
    DeviceCfgData testCfgData;
    testCfgData.m_encoder1Property = 0x140;
    testCfgData.m_encoder2Property = 0x140;
    mockSercosDeviceParam.setReturnCfgData(testCfgData);
    profileBasicInfo.m_deviceAddress = devAddress;

    sercos::SercosCommandDrive testCommand {profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &mockSercosDeviceParam);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclicData);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);

    testCommand.booting();
    oneTest.m_preConditionFun(testCommand);
    TestDataVariantU16 data {L"Add invalid data", 0x0000, true};
    setResponseVariant(&mockClient, data);
    mockScaling.m_torqueType = TorqueScalingType::LINEAR;
    oneTest.m_preConditionFun(testCommand);
    testCommand.resetAllAxisprofileCommands();

    AsyncCommandStatemachine* asyncState = testCommand.getAsyncStatemachine(oneTest.m_cmdType);
    asyncState->setCommandTimeout(1);
    std::uint16_t cycleExpired = 100;  // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testCommand);
      if (result != AxisProfileResult::STS_PENDING)
      {
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n    Execute timeout.");
    }
    if (oneTest.m_cmdType == AxisProfileCommandType::CMD_CHECK_DEVICE_HALT)
    {
      RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_TIMEOUT);
    }
    else
    {
      RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);
    }
  }
  log(L"\n");

  log(L"\n<b>  Test error case of thread locker: </b>");
  for (const auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};
    log(L"\n    Test of thread locker on %s ...", oneTest.m_cmdName.c_str());
    DeviceCfgData testCfgData;
    testCfgData.m_encoder1Property = 0x140;
    testCfgData.m_encoder2Property = 0x140;
    mockSercosDeviceParam.setReturnCfgData(testCfgData);
    profileBasicInfo.m_deviceAddress = devAddress;;

    sercos::SercosCommandDrive testCommand {profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &mockSercosDeviceParam);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclicData);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);

    testCommand.booting();
    TestDataVariantU16 data {L"Add invalid data", 0x0000, true};
    setResponseVariant(&mockClient, data);
    mockScaling.m_torqueType = TorqueScalingType::LINEAR;
    oneTest.m_preConditionFun(testCommand);

    result = oneTest.m_testFun(testCommand);
    if (oneTest.m_cmdType == AxisProfileCommandType::CMD_CHECK_DEVICE_HALT)
    {
      RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::STS_PENDING);
    }
    else
    {
      RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);
    }

    auto lambdaFun = [&testCommand, &oneTest]
    {
      return oneTest.m_testFun(testCommand);
    };
    std::future<AxisProfileResult> anotherThread = std::async(std::launch::async, lambdaFun);
    result = anotherThread.get();
    if (oneTest.m_cmdType == AxisProfileCommandType::CMD_CHECK_DEVICE_HALT)
    {
      RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_COMMAND_LOCKED);
    }
    else
    {
      RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);
    }
  }
  log(L"\n");

  return S_OK;
}

void TestSercosCommand::setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16 &data)
{
  flatbuffers::FlatBufferBuilder builder{};
  builder.ForceDefaults(true);
  std::uint8_t buffer[2];
  std::size_t dataSize = 2;
  (void)memcpy(buffer, &data.m_data, dataSize);
  auto vec = builder.CreateVector(buffer, dataSize);
  comm::ethercat::master::fbs::ElementFlags respElementFlags = static_cast<comm::ethercat::master::fbs::ElementFlags>(comm::ethercat::master::fbs::ElementFlags::ElementFlags_value);
  comm::ethercat::master::fbs::ParameterResponseBuilder paramResponseBuilder(builder);
  if (data.m_addData)
  {
    paramResponseBuilder.add_elementFlags(respElementFlags);
    paramResponseBuilder.add_data(vec);
  }
  auto exchangeDataResponse = paramResponseBuilder.Finish();
  comm::ethercat::master::fbs::ParameterBuilder paramBuilder(builder);
  paramBuilder.add_response(exchangeDataResponse);
  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);
  comm::datalayer::Variant variant;
  variant.copyFlatbuffers(builder);
  client->m_callbackVariant = variant;
}

void TestSercosCommand::setResponseVariantFourBytes(test::MockDatalayerClient* const client, const TestDataVariantU32 &Bigdata)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  std::uint8_t buffer[4] {};
  std::size_t dataSize = 4;
  (void)memcpy(&buffer[0], &Bigdata.m_data, dataSize);
  auto vec = builder.CreateVector(buffer, dataSize);
  comm::ethercat::master::fbs::ElementFlags respElementFlags = static_cast<comm::ethercat::master::fbs::ElementFlags>(comm::ethercat::master::fbs::ElementFlags::ElementFlags_value);
  comm::ethercat::master::fbs::ParameterResponseBuilder paramResponseBuilder(builder);
  if (Bigdata.m_addData)
  {
    paramResponseBuilder.add_elementFlags(respElementFlags);
    paramResponseBuilder.add_data(vec);
  }
  auto exchangeDataResponse = paramResponseBuilder.Finish();
  comm::ethercat::master::fbs::ParameterBuilder paramBuilder(builder);
  paramBuilder.add_response(exchangeDataResponse);
  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);
  comm::datalayer::Variant variant{};
  variant.copyFlatbuffers(builder);
  client->m_callbackVariant = variant;
}

HRESULT TestSercosCommand::testInitSetAbs()
{
  MockAxisProfileTrace mockProfileTrace {};
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockAxisProfileDeviceParam devcieParamMockup {};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t deviceAddress = 1;
  AcknowledgeTelegram atData;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  AuxJobHandler auxJobHandler {};
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  sercos::SercosCommandDrive testCommand {profileBasicInfo, &atData};

  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &devcieParamMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &testCommand);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, &auxJobHandler);

  testCommand.booting();
  DeviceCfgData cfgData {};
  cfgData.m_softwareLimitActive = true;
  cfgData.m_positivePosLimit = -10;
  cfgData.m_negativePosLimit = -200;
  devcieParamMockup.setReturnCfgData(cfgData);

  comm::axisprofile::AxisProfileResult result = testCommand.initSetAbsolutePosition(66, AxisProfileEncoder::ENCODER_1, true);
  RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_INVALID_ARGUMENT);

  result = testCommand.initReferencingScaling(66, AxisProfileEncoder::ENCODER_1, RefMode::SET_POS_DEVICE);
  RETURN_TEST_RESULT(result, L"\n    unexpected return value.", AxisProfileResult::ERR_INVALID_ARGUMENT);

  return S_OK;
}
