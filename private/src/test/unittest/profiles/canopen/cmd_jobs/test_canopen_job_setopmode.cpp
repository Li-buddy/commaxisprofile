#include "helper/result_helper.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_status_control.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_profile_trace.h"
#include "profiles/canopen/cmd_jobs/canopen_job_setopmode.h"
#include "profiles/canopen/canopen_cmd.h"
#include "profiles/canopen/canopen_param_access.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "test_canopen_job_setopmode.h"
#include <algorithm>

using namespace comm::axisprofile;

namespace
{
  class TestableCanopenJobSetOpMode final : public canopen::CanopenJobSetOpMode
  {
  public:
    using canopen::CanopenJobSetOpMode::CanopenJobSetOpMode;
    using canopen::CanopenJobSetOpMode::onJobAborting;
    using canopen::CanopenJobSetOpMode::onJobActivating;
    using canopen::CanopenJobSetOpMode::onJobEnterActivating;
    using canopen::CanopenJobSetOpMode::onSetArgument;
  };

  class TestableCanopenCmd final : public canopen::CanopenCmd
  {
  public:
    using canopen::CanopenCmd::CanopenCmd;
    const JobLookupTable& jobTable() const { return m_jobLookupTable; }
    const std::vector<CmdType>& resetCommands() const { return m_resetCmds; }
  };
}

REGISTER_TEST(TestCanopenJobSetOpMode);
REGISTER_TEST_TO_GROUP(TestCanopenJobSetOpMode, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobSetOpMode, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobSetOpMode, Profiles_Canopen);

HRESULT TestCanopenJobSetOpMode::doTest()
{
  test::MockAxisProfileTrace trace {};
  test::MockFieldbusAdapterCoE fieldbus {};
  test::MockDatalayerClient client {};
  fieldbus.setDatalayer(&client);

  ProfileBasicInfo profileInfo {};
  profileInfo.m_profileName = "set-operation-mode-test";
  profileInfo.m_driveNumber = 1;
  profileInfo.m_deviceAddress = 1;
  profileInfo.m_fieldbusAdapter = &fieldbus;
  profileInfo.m_profileTrace = trace;

  canopen::CanopenParamAccess paramAccess(profileInfo);
  test::MockAxisProfileDeviceParam deviceParam {};
  test::MockAxisProfileMappingHandler mapping {};
  test::MockAxisProfileStatusControl statusControl {};
  OpModeAvailInfo availability {true};
  ProfileAssignedInfo assigned {true};
  deviceParam.setReturnValue(&availability);
  mapping.setReturnValue(assigned);

  AxisProfileSharingHandler sharing(&profileInfo.m_profileTrace);
  sharing.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParam);
  sharing.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mapping);
  sharing.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &statusControl);
  sharing.registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);

  log(L"\n<b>Test CANopen set-operation-mode argument validation and mode support</b>");
  TestableCanopenJobSetOpMode argJob(profileInfo, &sharing);
  ArgFixedStop wrongArg {};
  auto result = argJob.onSetArgument(&wrongArg);
  LOG_TEST_RESULT(result, L"wrong argument type must be rejected", AxisProfileResult::ERR_JOB_DATA_MISMATCH);
  result = argJob.onSetArgument(nullptr);
  LOG_TEST_RESULT(result, L"null argument must be rejected", AxisProfileResult::ERR_JOB_DATA_MISMATCH);

  ArgSetOperationMode modeArg({AxisProfileOperationMode::VELOCITY});
  modeArg.getRef().mode = AxisProfileOperationMode::POSITIONING;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"assigned position mode must be supported", AxisProfileResult::STS_OK);
  modeArg.getRef().mode = AxisProfileOperationMode::VELOCITY;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"assigned velocity mode must be supported", AxisProfileResult::STS_OK);
  modeArg.getRef().mode = AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"assigned torque mode must be supported", AxisProfileResult::STS_OK);
  modeArg.getRef().mode = AxisProfileOperationMode::HOMING;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"homing mode must be accepted by the CiA-402 conversion", AxisProfileResult::STS_OK);

  modeArg.getRef().mode = AxisProfileOperationMode::UNDEFINED;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"undefined mode must fail conversion", AxisProfileResult::ERR_NOT_SUPPORTED);
  modeArg.getRef().mode = static_cast<AxisProfileOperationMode>(99);
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"unknown mode must fail conversion", AxisProfileResult::ERR_NOT_SUPPORTED);

  modeArg.getRef().mode = AxisProfileOperationMode::VELOCITY;
  availability.velocityAvail = AvailabilityInfo(false, "drive does not support velocity");
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"unsupported drive mode must be rejected", AxisProfileResult::ERR_NOK);
  availability.velocityAvail = AvailabilityInfo(false);
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"unsupported mode without detail text must be rejected", AxisProfileResult::ERR_NOK);
  availability.velocityAvail.clear();

  mapping.getProfileAssignedInfo().m_input.m_velocity = false;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing velocity input mapping must be rejected", AxisProfileResult::ERR_NOK);
  mapping.getProfileAssignedInfo().m_input.m_velocity = true;
  mapping.getProfileAssignedInfo().m_output.m_velocity = false;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing velocity output mapping must be rejected", AxisProfileResult::ERR_NOK);
  mapping.getProfileAssignedInfo().m_output.m_velocity = true;

  mapping.getProfileAssignedInfo().m_input.m_torqueForceLimSts = false;
  modeArg.getRef().mode = AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing torque-limit status input must be rejected", AxisProfileResult::ERR_NOK);
  mapping.getProfileAssignedInfo().m_input.m_torqueForceLimSts = true;
  mapping.getProfileAssignedInfo().m_output.m_torqueVelLimHigh = false;
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing torque velocity-limit output must be rejected", AxisProfileResult::ERR_NOK);
  mapping.getProfileAssignedInfo().m_output.m_torqueVelLimHigh = true;

  modeArg.getRef().mode = AxisProfileOperationMode::VELOCITY;
  deviceParam.setReturnValue(nullptr);
  result = argJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing operation-mode availability data must be rejected", AxisProfileResult::ERR_NOK);
  deviceParam.setReturnValue(&availability);

  AxisProfileSharingHandler missingMappingSharing(&profileInfo.m_profileTrace);
  missingMappingSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParam);
  missingMappingSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &statusControl);
  TestableCanopenJobSetOpMode missingMappingJob(profileInfo, &missingMappingSharing);
  result = missingMappingJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing mapping handler must reject the mode", AxisProfileResult::ERR_NOK);

  AxisProfileSharingHandler missingDeviceSharing(&profileInfo.m_profileTrace);
  missingDeviceSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mapping);
  missingDeviceSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &statusControl);
  TestableCanopenJobSetOpMode missingDeviceJob(profileInfo, &missingDeviceSharing);
  result = missingDeviceJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing device parameter interface must reject the mode", AxisProfileResult::ERR_NOK);

  log(L"\n<b>Test CANopen PDO set, feedback verification, and confirmed-mode cache update</b>");
  AcknowledgeTelegram at {};
  MasterDataTelegram mdt {};
  TestableCanopenJobSetOpMode pdoJob(profileInfo, &sharing, &at, &mdt);
  result = pdoJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"PDO job argument should be accepted", AxisProfileResult::STS_OK);
  result = pdoJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"PDO job should enter activation", AxisProfileResult::STS_OK);
  result = pdoJob.onJobActivating();
  LOG_TEST_RESULT(result, L"PDO write should advance asynchronously", AxisProfileResult::STS_PENDING);
  LOG_TEST_RESULT(mdt.m_mdtData.m_opMode, L"PDO output should contain CiA-402 velocity mode", canopen::CANOPEN_VELOCITY_MODE);
  result = pdoJob.onJobActivating();
  LOG_TEST_RESULT(result, L"mismatched PDO feedback should remain pending", AxisProfileResult::STS_PENDING);
  at.m_atData.m_opModeDisplay = static_cast<std::uint8_t>(canopen::CANOPEN_VELOCITY_MODE);
  result = pdoJob.onJobActivating();
  LOG_TEST_RESULT(result, L"matching PDO feedback should complete the job", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(statusControl.getOperationMode(), L"successful PDO feedback should update the confirmed mode", AxisProfileOperationMode::VELOCITY);

  log(L"\n<b>Test CANopen PDO write with SDO verification fallback</b>");
  mapping.getProfileAssignedInfo().m_output.m_opMode = true;
  mapping.getProfileAssignedInfo().m_input.m_opModeDisplay = false;
  statusControl.setOperationMode(AxisProfileOperationMode::POSITIONING);
  fieldbus.m_returnValue = AxisProfileResult::STS_OK;
  client.m_callbackReturnValue = comm::datalayer::DlResult::DL_OK;
  TestableCanopenJobSetOpMode mixedMappingJob(profileInfo, &sharing, &at, &mdt);
  result = mixedMappingJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"mixed-mapping job argument should be accepted", AxisProfileResult::STS_OK);
  result = mixedMappingJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"mixed-mapping job should enter activation", AxisProfileResult::STS_OK);
  result = mixedMappingJob.onJobActivating();
  LOG_TEST_RESULT(result, L"PDO write should advance to the SDO verification step", AxisProfileResult::STS_PENDING);
  LOG_TEST_RESULT(mdt.m_mdtData.m_opMode, L"mixed-mapping job should still write the CiA-402 velocity mode to PDO", canopen::CANOPEN_VELOCITY_MODE);

  fieldbus.m_fbsHelper.writeParam(canopen::MODES_OF_OPERATION_DISPLAY.index,
                                  canopen::MODES_OF_OPERATION_DISPLAY.subIndex,
                                  static_cast<std::uint8_t>(canopen::CANOPEN_VELOCITY_MODE));
  fieldbus.m_fbsHelper.generateSDOResponseFlatbuffer(&client.m_callbackVariant,
                                                     canopen::MODES_OF_OPERATION_DISPLAY.index,
                                                     canopen::MODES_OF_OPERATION_DISPLAY.subIndex);
  result = mixedMappingJob.onJobActivating();
  LOG_TEST_RESULT(result, L"SDO verification request should remain pending", AxisProfileResult::STS_PENDING);
  result = mixedMappingJob.onJobActivating();
  LOG_TEST_RESULT(result, L"SDO verification response wait should remain pending", AxisProfileResult::STS_PENDING);
  result = mixedMappingJob.onJobActivating();
  LOG_TEST_RESULT(result, L"SDO verification should complete without overrunning the job-function vector", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(statusControl.getOperationMode(), L"mixed-mapping verification should update the confirmed mode", AxisProfileOperationMode::VELOCITY);
  mapping.getProfileAssignedInfo().m_input.m_opModeDisplay = true;

  log(L"\n<b>Test already-confirmed mode and abort cleanup</b>");
  statusControl.setOperationMode(AxisProfileOperationMode::VELOCITY);
  TestableCanopenJobSetOpMode alreadySetJob(profileInfo, &sharing, &at, &mdt);
  result = alreadySetJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"already-set job argument should be accepted", AxisProfileResult::STS_OK);
  result = alreadySetJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"already-confirmed mode should skip the write path", AxisProfileResult::STS_OK);
  result = alreadySetJob.onJobActivating();
  LOG_TEST_RESULT(result, L"already-confirmed mode should complete", AxisProfileResult::STS_OK);
  result = alreadySetJob.onJobAborting();
  LOG_TEST_RESULT(result, L"abort cleanup should succeed", AxisProfileResult::STS_OK);

  log(L"\n<b>Test null PDO telegram guard</b>");
  TestableCanopenJobSetOpMode noTelegramJob(profileInfo, &sharing);
  result = noTelegramJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"null-telegram job argument should be accepted", AxisProfileResult::STS_OK);
  result = noTelegramJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"activation without telegrams must fail safely", AxisProfileResult::ERR_EXEC_NULL);

  TestableCanopenJobSetOpMode noMdtJob(profileInfo, &sharing, &at, nullptr);
  result = noMdtJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"null-MDT job argument should be accepted", AxisProfileResult::STS_OK);
  result = noMdtJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"activation without an MDT telegram must fail safely", AxisProfileResult::ERR_EXEC_NULL);

  missingMappingSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
  TestableCanopenJobSetOpMode missingMappingActivationJob(profileInfo, &missingMappingSharing, &at, &mdt);
  result = missingMappingActivationJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing-mapping job argument must be rejected", AxisProfileResult::ERR_NOK);
  result = missingMappingActivationJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"activation without a mapping handler must fail safely", AxisProfileResult::ERR_EXEC_NULL);

  AxisProfileSharingHandler missingParamSharing(&profileInfo.m_profileTrace);
  missingParamSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParam);
  missingParamSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mapping);
  missingParamSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &statusControl);
  TestableCanopenJobSetOpMode missingParamJob(profileInfo, &missingParamSharing, &at, &mdt);
  result = missingParamJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"missing-param-access job argument should be accepted", AxisProfileResult::STS_OK);
  result = missingParamJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"activation without parameter access must fail safely", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n<b>Test CANopen SDO async verification and confirmation</b>");
  mapping.getProfileAssignedInfo().m_output.m_opMode = false;
  mapping.getProfileAssignedInfo().m_input.m_opModeDisplay = false;
  statusControl.setOperationMode(AxisProfileOperationMode::POSITIONING);
  fieldbus.m_returnValue = AxisProfileResult::STS_OK;
  client.m_callbackReturnValue = comm::datalayer::DlResult::DL_OK;
  fieldbus.m_fbsHelper.writeParam(canopen::MODES_OF_OPERATION_DISPLAY.index,
                                  canopen::MODES_OF_OPERATION_DISPLAY.subIndex,
                                  static_cast<std::uint8_t>(canopen::CANOPEN_POSITION_MODE));
  fieldbus.m_fbsHelper.generateSDOResponseFlatbuffer(&client.m_callbackVariant,
                                                     canopen::MODES_OF_OPERATION_DISPLAY.index,
                                                     canopen::MODES_OF_OPERATION_DISPLAY.subIndex);
  TestableCanopenJobSetOpMode sdoJob(profileInfo, &sharing, &at, &mdt);
  result = sdoJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"SDO job argument should be accepted", AxisProfileResult::STS_OK);
  result = sdoJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"SDO job should enter activation", AxisProfileResult::STS_OK);
  for (int cycle = 0; cycle < 6; ++cycle)
  {
    result = sdoJob.onJobActivating();
    if (result != AxisProfileResult::STS_PENDING)
    {
      break;
    }
  }
  LOG_TEST_RESULT(result, L"SDO write and mismatched readback should remain pending", AxisProfileResult::STS_PENDING);

  fieldbus.m_fbsHelper.writeParam(canopen::MODES_OF_OPERATION_DISPLAY.index,
                                  canopen::MODES_OF_OPERATION_DISPLAY.subIndex,
                                  static_cast<std::uint8_t>(canopen::CANOPEN_VELOCITY_MODE));
  fieldbus.m_fbsHelper.generateSDOResponseFlatbuffer(&client.m_callbackVariant,
                                                     canopen::MODES_OF_OPERATION_DISPLAY.index,
                                                     canopen::MODES_OF_OPERATION_DISPLAY.subIndex);
  result = sdoJob.onJobActivating();
  LOG_TEST_RESULT(result, L"SDO read request should remain pending", AxisProfileResult::STS_PENDING);
  result = sdoJob.onJobActivating();
  LOG_TEST_RESULT(result, L"SDO response wait should remain pending", AxisProfileResult::STS_PENDING);
  result = sdoJob.onJobActivating();
  LOG_TEST_RESULT(result, L"matching SDO readback should complete the job", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(statusControl.getOperationMode(), L"successful SDO feedback should update the confirmed mode", AxisProfileOperationMode::VELOCITY);

  log(L"\n<b>Test CoE fieldbus error handling for SDO write and readback</b>");
  statusControl.setOperationMode(AxisProfileOperationMode::POSITIONING);
  TestableCanopenJobSetOpMode failedWriteJob(profileInfo, &sharing, &at, &mdt);
  result = failedWriteJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"failed-write job argument should be accepted", AxisProfileResult::STS_OK);
  result = failedWriteJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"failed-write job should enter activation", AxisProfileResult::STS_OK);
  fieldbus.m_returnValue = AxisProfileResult::ERR_FIELDBUS;
  result = failedWriteJob.onJobActivating();
  LOG_TEST_RESULT(result, L"SDO write errors should fail the command", AxisProfileResult::ERR_JOB_EXECUTION);
  fieldbus.m_returnValue = AxisProfileResult::STS_OK;

  statusControl.setOperationMode(AxisProfileOperationMode::POSITIONING);
  client.m_callbackReturnValue = comm::datalayer::DlResult::DL_OK;
  TestableCanopenJobSetOpMode failedReadJob(profileInfo, &sharing, &at, &mdt);
  result = failedReadJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"failed-read job argument should be accepted", AxisProfileResult::STS_OK);
  result = failedReadJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"failed-read job should enter activation", AxisProfileResult::STS_OK);
  for (int cycle = 0; cycle < 3; ++cycle)
  {
    result = failedReadJob.onJobActivating();
    if (result != AxisProfileResult::STS_PENDING)
    {
      break;
    }
  }
  LOG_TEST_RESULT(result, L"SDO write should remain pending", AxisProfileResult::STS_PENDING);
  client.m_callbackReturnValue = comm::datalayer::DlResult::DL_FAILED;
  result = failedReadJob.onJobActivating();
  LOG_TEST_RESULT(result, L"SDO read request should remain pending", AxisProfileResult::STS_PENDING);
  result = failedReadJob.onJobActivating();
  LOG_TEST_RESULT(result, L"failed SDO read response should fail the command", AxisProfileResult::ERR_JOB_EXECUTION);
  client.m_callbackReturnValue = comm::datalayer::DlResult::DL_OK;

  log(L"\n<b>Test operation-mode job without a status-control interface</b>");
  AxisProfileSharingHandler noStatusSharing(&profileInfo.m_profileTrace);
  noStatusSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParam);
  noStatusSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mapping);
  noStatusSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
  noStatusSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &statusControl);
  mapping.getProfileAssignedInfo().m_output.m_opMode = true;
  mapping.getProfileAssignedInfo().m_input.m_opModeDisplay = true;
  at.m_atData.m_opModeDisplay = static_cast<std::uint8_t>(canopen::CANOPEN_VELOCITY_MODE);
  TestableCanopenJobSetOpMode noStatusJob(profileInfo, &noStatusSharing, &at, &mdt);
  result = noStatusJob.onSetArgument(&modeArg);
  LOG_TEST_RESULT(result, L"job without status control should accept a supported mode", AxisProfileResult::STS_OK);
  result = noStatusJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"job without status control should enter activation", AxisProfileResult::STS_OK);
  result = noStatusJob.onJobActivating();
  LOG_TEST_RESULT(result, L"PDO write should remain pending for its verification step", AxisProfileResult::STS_PENDING);
  result = noStatusJob.onJobActivating();
  LOG_TEST_RESULT(result, L"job execution should finish even without a status-control interface", AxisProfileResult::STS_OK);

  log(L"\n<b>Test CANopen command registration and mode-setting dispatch</b>");
  TestableCanopenCmd command(profileInfo);
  const auto modeCommand = ENUM_TO_U16(CmdType::SET_OPERATION_MODE_WITH_CHECK);
  LOG_TEST_RESULT(command.jobTable()[modeCommand] == canopen::CanopenJobSetOpMode::jobName(),
                  L"CANopen command must resolve to the operation-mode job", true);
  const auto& resetCommands = command.resetCommands();
  LOG_TEST_RESULT(std::find(resetCommands.begin(), resetCommands.end(), CmdType::SET_OPERATION_MODE_WITH_CHECK) != resetCommands.end(),
                  L"operation-mode job should be in the command reset list", true);

  return S_OK;
}
