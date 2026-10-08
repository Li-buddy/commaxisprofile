#include "helper/result_helper.h"
#include "mockup/mock_axisprofile_status_control.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/cmd_jobs/sercos_job_setopmode.h"
#include "profiles/sercos/sercos_cmd.h"
#include "profiles/sercos/sercos_param_access.h"
#include "test_sercos_job_setopmode.h"
#include <algorithm>

using namespace comm::axisprofile;

namespace
{
  class TestableSercosJobSetOpMode final : public sercos::SercosJobSetOpMode
  {
  public:
    using sercos::SercosJobSetOpMode::SercosJobSetOpMode;
    using sercos::SercosJobSetOpMode::onJobActivating;
    using sercos::SercosJobSetOpMode::onJobEnterAborting;
    using sercos::SercosJobSetOpMode::onJobEnterActivating;
    using sercos::SercosJobSetOpMode::onSetArgument;
  };

  class UnsupportedStatusControl final : public test::MockAxisProfileStatusControl
  {
  public:
    bool checkOperationModeSupported(const AxisProfileOperationMode,
                                     OpModeErrorInfo& errorInfo) override
    {
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_NOT_SUPPORT;
      errorInfo.m_addInfo = "not supported by test drive";
      return false;
    }
  };

  class TestableSercosCmd final : public sercos::SercosCmd
  {
  public:
    using sercos::SercosCmd::SercosCmd;
    const JobLookupTable& jobTable() const { return m_jobLookupTable; }
    const std::vector<CmdType>& resetCommands() const { return m_resetCmds; }
  };
}

REGISTER_TEST(TestSercosJobSetOpMode);
REGISTER_TEST_TO_GROUP(TestSercosJobSetOpMode, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosJobSetOpMode, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosJobSetOpMode, Profiles_Sercos);

HRESULT TestSercosJobSetOpMode::doTest()
{
  test::MockAxisProfileTrace trace {};
  test::MockFieldbusAdapterSoE fieldbus {};
  ProfileBasicInfo profileInfo {};
  profileInfo.m_profileName = "set-operation-mode-sercos-test";
  profileInfo.m_fieldbusAdapter = &fieldbus;
  profileInfo.m_profileTrace = trace;

  sercos::SercosParamAccess paramAccess(profileInfo);
  test::MockAxisProfileStatusControl statusControl {};
  AxisProfileSharingHandler sharing(&profileInfo.m_profileTrace);
  sharing.registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
  sharing.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &statusControl);

  log(L"\n<b>Test Sercos argument validation and unsupported mode</b>");
  TestableSercosJobSetOpMode job(profileInfo, &sharing);
  ArgFixedStop wrongArg {};
  auto result = job.onSetArgument(&wrongArg);
  LOG_TEST_RESULT(result, L"wrong argument type must be rejected", AxisProfileResult::ERR_JOB_DATA_MISMATCH);
  result = job.onSetArgument(nullptr);
  LOG_TEST_RESULT(result, L"null argument must be rejected", AxisProfileResult::ERR_JOB_DATA_MISMATCH);

  UnsupportedStatusControl unsupportedStatusControl;
  AxisProfileSharingHandler unsupportedSharing(&profileInfo.m_profileTrace);
  unsupportedSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
  unsupportedSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &unsupportedStatusControl);
  TestableSercosJobSetOpMode unsupportedJob(profileInfo, &unsupportedSharing);
  ArgSetOperationMode velocityArg({AxisProfileOperationMode::VELOCITY});
  result = unsupportedJob.onSetArgument(&velocityArg);
  LOG_TEST_RESULT(result, L"unsupported operation mode must be rejected", AxisProfileResult::ERR_NOK);

  log(L"\n<b>Test SoE mode write, pending feedback, completion, and confirmed cache</b>");
  result = job.onSetArgument(&velocityArg);
  LOG_TEST_RESULT(result, L"supported mode argument should be accepted", AxisProfileResult::STS_OK);
  result = job.onJobActivating();
  LOG_TEST_RESULT(result, L"activation must wait while feedback has not reached the requested mode", AxisProfileResult::STS_PENDING);

  statusControl.setReturnValue(AxisProfileResult::ERR_NOK);
  result = job.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"failed StatusControl write must be propagated", AxisProfileResult::ERR_NOK);

  statusControl.setReturnValue(AxisProfileResult::STS_OK);
  result = job.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"successful StatusControl write should enter activation", AxisProfileResult::STS_OK);
  result = job.onJobActivating();
  LOG_TEST_RESULT(result, L"matching mode feedback should finish the job", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(statusControl.getOperationMode(), L"confirmed mode should be retained by StatusControl", AxisProfileOperationMode::VELOCITY);
  auto jobData = JobDataDeviceStatus::safeCast(job.getJobData());
  LOG_TEST_RESULT(jobData->get().load(), L"completed job should publish device completion", CmdStatusInDevice::DEVICE_COMPLETED);

  result = job.onJobEnterAborting();
  LOG_TEST_RESULT(result, L"abort should mark the job interrupted and finish", AxisProfileResult::STS_OK);
  jobData = JobDataDeviceStatus::safeCast(job.getJobData());
  LOG_TEST_RESULT(jobData->get().load(), L"abort should publish an interrupted device status", CmdStatusInDevice::DEVICE_NOT_SET);

  log(L"\n<b>Test missing status-control interface guards</b>");
  AxisProfileSharingHandler missingStatusSharing(&profileInfo.m_profileTrace);
  missingStatusSharing.registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
  TestableSercosJobSetOpMode missingStatusJob(profileInfo, &missingStatusSharing);
  result = missingStatusJob.onSetArgument(&velocityArg);
  LOG_TEST_RESULT(result, L"missing status control should be reported on argument setup", AxisProfileResult::ERR_EXEC_NULL);
  result = missingStatusJob.onJobEnterActivating();
  LOG_TEST_RESULT(result, L"missing status control should be reported on activation", AxisProfileResult::ERR_EXEC_NULL);
  result = missingStatusJob.onJobActivating();
  LOG_TEST_RESULT(result, L"missing status control should be reported while checking feedback", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n<b>Test Sercos command registration for the set-operation-mode job</b>");
  TestableSercosCmd command(profileInfo);
  const auto modeCommand = ENUM_TO_U16(CmdType::SET_OPERATION_MODE_WITH_CHECK);
  LOG_TEST_RESULT(command.jobTable()[modeCommand] == sercos::SercosJobSetOpMode::jobName(),
                  L"Sercos command must resolve to the operation-mode job", true);
  const auto& resetCommands = command.resetCommands();
  LOG_TEST_RESULT(std::find(resetCommands.begin(), resetCommands.end(), CmdType::SET_OPERATION_MODE_WITH_CHECK) != resetCommands.end(),
                  L"operation-mode job should be in the command reset list", true);

  return S_OK;
}
