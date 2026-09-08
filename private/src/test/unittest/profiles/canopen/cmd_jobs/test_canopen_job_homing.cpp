#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/canopen/cmd_jobs/canopen_job_homing.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_category.h"
#include "mockup/mock_axisprofile_status_control.h"
#include "test_canopen_job_homing.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenJobHoming);
REGISTER_TEST_TO_GROUP(TestCanopenJobHoming, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobHoming, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobHoming, Profiles_Canopen);


HRESULT TestCanopenJobHoming::doInit()
{
  m_mockProperty.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  m_sharingHandlerHolder = std::make_unique<AxisProfileSharingHandler>(&m_mockProfileTrace);
  m_sharingHandler = m_sharingHandlerHolder.get();
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &m_mockScaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &m_mockMappingHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &m_mockProperty);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &m_mockCategory);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &m_mockStatusControl);
  return S_OK;
}

HRESULT TestCanopenJobHoming::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Canopen profile cmdJobs CanopenJobHoming class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestSetArgument();
  hresult |= TestJobEnterActivating();
  hresult |= TestJobEnterAborting();
  hresult |= TestActiveHomingByProfile();
  hresult |= TestCheckHomingProcedure();

  return hresult;
}

HRESULT TestCanopenJobHoming::doEnd()
{
  HRESULT res = S_OK;
  log(L"\nDeinit test cases...");
  return res;
}


HRESULT TestCanopenJobHoming::TestSetArgument()
{
  log(L"\n<b>Test CanopenJobHoming::onSetArgument</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  canopen::CanopenJobHoming testJob(profileBasicInfo, m_sharingHandler);

  log(L"\nTest case: wrong argument type");
  ArgFixedStop wrongArg;
  AxisProfileResult res = testJob.onSetArgument(&wrongArg);
  LOG_TEST_RESULT(res, L"onSetArgument should fail when passing wrong argument type", AxisProfileResult::ERR_JOB_DATA_MISMATCH);

  log(L"\nTest for out of travel limit");
  ArgHoming arg;
  auto& ref = arg.getRef();
  auto& cfg = m_mockDeviceParam.getDeviceCfgData();
  cfg.m_softwareLimitActive = true;
  cfg.m_positivePosLimit = INT32_MIN;
  cfg.m_negativePosLimit = INT32_MAX;
  ref.mode = RefMode::SET_POS_DEVICE;
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::ERR_INVALID_ARGUMENT);
  comm::axisprofile::DeviceCfgData dataInit;
  m_mockDeviceParam.setReturnCfgData(dataInit);

  log(L"\nTest for encoder type");
  ref.encoder = AxisProfileEncoder::ENCODER_2;
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\nTest for mode type");
  ref.mode = RefMode::SET_POS_PROFILE;
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobHoming::TestJobEnterActivating()
{
  log(L"\n<b>Test CanopenJobHoming::onJobEnterActivating</b>");
  struct TestCase
  {
    std::wstring name;
    DeviceSeries deviceSeries;
    RefMode mode;
    std::size_t expectedJobCount;
  };

  std::vector<TestCase> testCases =
  {
    {L"SET_POS_DEVICE generates ctrlX jobs", DeviceSeries::CTRLXDRIVE, RefMode::SET_POS_DEVICE, 12},
    {L"SET_POS_DEVICE generates non ctrlX jobs", DeviceSeries::INDRADRIVE, RefMode::SET_POS_DEVICE, 9},
    {L"SET_POS_PROFILE generates job", DeviceSeries::CTRLXDRIVE, RefMode::SET_POS_PROFILE, 2},
    {L"UNDEFINED generates jobs", DeviceSeries::CTRLXDRIVE, RefMode::UNDEF_METHOD, 0}
  };
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobHoming testJob(profileBasicInfo, m_sharingHandler);
  ArgHoming arg;
  auto& ref = arg.getRef();
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  for (const auto& tc : testCases)
  {
    log(L"\n Test case: %s", tc.name.c_str());
    m_mockProperty.setDeviceSeries(tc.deviceSeries);
    ref.mode = tc.mode;
    AxisProfileResult res = testJob.onSetArgument(&arg);
    RETURN_TEST_RESULT(res, L"onSetArgument failed", AxisProfileResult::STS_OK);

    res = testJob.onJobEnterActivating();
    LOG_TEST_RESULT(res, L"onJobEnterActivating failed", AxisProfileResult::STS_OK);
    LOG_TEST_RESULT(testJob.m_jobFuns.size(), L"unexpected top-level job count", tc.expectedJobCount);
  }

  return S_OK;
}

HRESULT TestCanopenJobHoming::TestJobEnterAborting()
{
  log(L"\n<b>Test CanopenJobHoming::onJobEnterAborting</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobHoming job(profileBasicInfo, m_sharingHandler);
  ArgHoming arg;
  auto& ref = arg.getRef();
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  log(L"\n Test case SET_POS_PROFILE abort has 0 job");
  ref.mode = RefMode::SET_POS_PROFILE;
  AxisProfileResult res = job.onSetArgument(&arg);
  RETURN_TEST_RESULT(res, L"onSetArgument failed", AxisProfileResult::STS_OK);
  res = job.onJobEnterAborting();
  LOG_TEST_RESULT(res, L"onJobEnterAborting failed", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobHoming::TestActiveHomingByProfile()
{
  log(L"\n<b>Test activeHomingByProfile - read actual value failure cases</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &m_mockCyclicData);
  canopen::CanopenJobHoming job(profileBasicInfo, m_sharingHandler);
  log(L"\ngetActualValuesDevice fail test");
  m_mockCyclicData.setReturnValue(AxisProfileResult::ERR_NOK);
  AxisProfileResult res = job.activeHomingByProfile();
  LOG_TEST_RESULT(res, L"activeHomingByProfile failed", AxisProfileResult::ERR_NOK);

  log(L"\nActualValuesDevice is invalid test");
  m_mockCyclicData.setReturnValue(AxisProfileResult::STS_OK);
  res = job.activeHomingByProfile();
  LOG_TEST_RESULT(res, L"activeHomingByProfile m_dataValid is false", AxisProfileResult::ERR_WRONG_DEVICE_STATE);
  return S_OK;
}

HRESULT TestCanopenJobHoming::TestCheckHomingProcedure()
{
  log(L"\n<b>Test CanopenJobHoming::checkHomingProcedure</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  canopen::CanopenJobHoming job(profileBasicInfo, m_sharingHandler);

  log(L"\nTest non-device homing completes immediately");
  job.m_cmdData.mode = RefMode::SET_POS_PROFILE;
  AxisProfileResult res = job.onUpdateStatusInDevice();
  LOG_TEST_RESULT(res, L"profile homing status update failed", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(job.m_deviceStatus.get().load(), L"profile homing should be completed", CmdStatusInDevice::DEVICE_COMPLETED);

  log(L"\nTest device homing reports error");
  job.m_cmdData.mode = RefMode::SET_POS_DEVICE;
  job.m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  WordStruct statusWord {};
  statusWord.m_bits.bit13 = true;
  m_mockStatusControl.m_statusWord = statusWord.m_word;
  res = job.onUpdateStatusInDevice();
  LOG_TEST_RESULT(res, L"device homing error should fail", AxisProfileResult::ERR_JOB_EXECUTION);
  LOG_TEST_RESULT(job.m_deviceStatus.get().load(), L"device homing should report error", CmdStatusInDevice::DEVICE_ERROR);

  log(L"\nTest device homing completes");
  job.m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  m_mockDeviceParam.m_homed = false;
  statusWord.m_word = 0;
  statusWord.m_bits.bit10 = true;
  statusWord.m_bits.bit12 = true;
  m_mockStatusControl.m_statusWord = statusWord.m_word;
  res = job.onUpdateStatusInDevice();
  LOG_TEST_RESULT(res, L"device homing complete status update failed", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(job.m_deviceStatus.get().load(), L"device homing should be completed", CmdStatusInDevice::DEVICE_COMPLETED);
  LOG_TEST_RESULT(m_mockDeviceParam.m_homed, L"axis should be marked homed", true);

  log(L"\nTest signal function homing remains in process");
  job.m_isSignalFuncCmd = true;
  job.m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  statusWord.m_word = 0;
  m_mockStatusControl.m_statusWord = statusWord.m_word;
  m_mockDeviceParam.m_homed = false;
  m_mockDeviceParam.setReturnValue(AxisProfileResult::ERR_NOK);
  res = job.onUpdateStatusInDevice();
  LOG_TEST_RESULT(res, L"signal function homing status update failed", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(job.m_deviceStatus.get().load(), L"signal function homing should stay in process", CmdStatusInDevice::DEVICE_IN_PROCESS);

  log(L"\nTest signal function homing completes");
  m_mockDeviceParam.setReturnValue(AxisProfileResult::STS_OK);
  job.m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  m_mockDeviceParam.m_homed = true;
  res = job.onUpdateStatusInDevice();
  LOG_TEST_RESULT(res, L"signal function homing complete status update failed", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(job.m_deviceStatus.get().load(), L"signal function homing should be completed", CmdStatusInDevice::DEVICE_COMPLETED);
  LOG_TEST_RESULT(m_mockDeviceParam.m_homed, L"axis should be marked homed by signal function", true);
  return S_OK;
}
