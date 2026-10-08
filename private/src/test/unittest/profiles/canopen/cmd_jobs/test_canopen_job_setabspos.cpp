#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/canopen/cmd_jobs/canopen_job_setabspos.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_category.h"
#include "test_canopen_job_setabspos.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenJobSetAbsPos);
REGISTER_TEST_TO_GROUP(TestCanopenJobSetAbsPos, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobSetAbsPos, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobSetAbsPos, Profiles_Canopen);


HRESULT TestCanopenJobSetAbsPos::doInit()
{
  m_mockProperty.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  m_sharingHandlerHolder = std::make_unique<AxisProfileSharingHandler>(&m_mockProfileTrace);
  m_sharingHandler = m_sharingHandlerHolder.get();
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &m_mockScaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &m_mockMappingHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &m_mockProperty);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &m_mockCategory);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &m_mockCyclicData);
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Canopen profile cmdJobs CanopenJobSetAbsPos class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestSetArgument();
  hresult |= TestJobEnterActivating();
  hresult |= TestJobEnterAborting();
  hresult |= TestJobActivating();
  hresult |= TestJobAborting();
  hresult |= TestSetupReadListFromDevice();
  hresult |= TestSetupWriteListToDevice();
  hresult |= TestJudgmentHandleBased();
  hresult |= TestSetIoEncoderData();
  hresult |= TestResetIoEncoderData();
  hresult |= TestSetCmdStatusInDevice();

  return hresult;
}

HRESULT TestCanopenJobSetAbsPos::doEnd()
{
  HRESULT res = S_OK;
  log(L"\nDeinit test cases...");
  return res;
}

HRESULT TestCanopenJobSetAbsPos::TestSetArgument()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::onSetArgument</b>");

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;  
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);

  log(L"\nTest case: wrong argument type");
  ArgFixedStop wrongArg;
  AxisProfileResult res = testJob.onSetArgument(&wrongArg);
  LOG_TEST_RESULT(res, L"onSetArgument should fail when passing wrong argument type", AxisProfileResult::ERR_JOB_DATA_MISMATCH);

  log(L"\nTest for out of travel limit");
  ArgSetAbsolutePosition arg;
  auto& ref = arg.getRef();
  auto& cfg = m_mockDeviceParam.getDeviceCfgData();
  cfg.m_softwareLimitActive = true;
  cfg.m_positivePosLimit = INT32_MIN;
  cfg.m_negativePosLimit = INT32_MAX;
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::ERR_INVALID_ARGUMENT);
  comm::axisprofile::DeviceCfgData dataInit;
  m_mockDeviceParam.setReturnCfgData(dataInit);

  log(L"\nTest for category check");
  m_mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_ENCODER;
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  m_mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_DRIVE;

  log(L"\nTest for device series check");
  m_mockProperty.setDeviceSeries(DeviceSeries::INDRADRIVE);
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::ERR_INVALID_ARGUMENT);
  m_mockProperty.setDeviceSeries(DeviceSeries::CTRLXDRIVE);

  log(L"\nTest case: wrong device state for encoder_2(bit6/bit7)");
  ref.encoder = AxisProfileEncoder::ENCODER_2;
  WordStructure prop;
  prop.m_bits.bit6 = true;
  prop.m_bits.bit7 = true;
  cfg.m_encoder2Property = prop.m_word;
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"validateConfig should reject absolute encoder in relative mode error", AxisProfileResult::ERR_WRONG_DEVICE_STATE);

  log(L"\nTest case: wrong device state for encoder_2(bit6/bit8)");
  prop.m_bits.bit6 = false;
  prop.m_bits.bit8 = false;
  cfg.m_encoder2Property = prop.m_word;
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"validateConfig should reject absolute encoder in relative mode error", AxisProfileResult::ERR_WRONG_DEVICE_STATE);

  log(L"\nTest for encoder validation type");
  prop.m_bits.bit8 = true;
  cfg.m_encoder2Property = prop.m_word;
  cfg.m_encoder1Property = prop.m_word;
  struct EncoderCase
  {
    AxisProfileEncoder encoder;
    AxisProfileResult expected;
    const wchar_t* msg{};
  };
  constexpr std::array<EncoderCase, 3> encoderCases {{
    {AxisProfileEncoder::ENCODER_1, AxisProfileResult::STS_OK, L"encoder 1 should be accepted"},
    {AxisProfileEncoder::ENCODER_2, AxisProfileResult::STS_OK, L"encoder 2 should be accepted"},
    {AxisProfileEncoder::UNDEFINED, AxisProfileResult::ERR_INVALID_ARGUMENT, L"undefined encoder should be rejected"}
    }};

  for (const auto& tc : encoderCases)
  {
    ref.encoder = tc.encoder;
    res = testJob.onSetArgument(&arg);
    LOG_TEST_RESULT(res, tc.msg, tc.expected);
  }

  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestJobEnterActivating()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::onJobEnterActivating</b>");

  ProfileBasicInfo profileBasicInfo; 
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  ArgSetAbsolutePosition arg;
  auto& ref = arg.getRef();
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  for (const auto& tc : m_testCases)
  {
    log(L"\n Test case: %s", tc.name.c_str());
    m_mockCategory.m_profileCategory = tc.nType;
    AxisProfileResult res = testJob.onSetArgument(&arg);
    RETURN_TEST_RESULT(res, L"onSetArgument failed", AxisProfileResult::STS_OK);

    res = testJob.onJobEnterActivating();
    LOG_TEST_RESULT(res, L"onJobEnterActivating unexpected failed", tc.ret);
  }
  //recover the category after test
  m_mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_DRIVE;
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestJobEnterAborting()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::onJobEnterAborting</b>");

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  ArgSetAbsolutePosition arg;
  auto& ref = arg.getRef();
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  for (const auto& tc : m_testCases)
  {
    log(L"\n Test case: %s", tc.name.c_str());
    m_mockCategory.m_profileCategory = tc.nType;
    AxisProfileResult res = testJob.onSetArgument(&arg);
    LOG_TEST_RESULT(res, L"onSetArgument failed", AxisProfileResult::STS_OK);

    res = testJob.onJobEnterAborting();
    LOG_TEST_RESULT(res, L"onJobEnterAborting unexpected failed", tc.ret);
  }
  //recover the category after test
  m_mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_DRIVE;
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestJobActivating()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::onJobActivating</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestJobAborting()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::onJobAborting</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.onJobAborting();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestSetupReadListFromDevice()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::setupReadListFromDevice</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.setupReadListFromDevice();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestSetupWriteListToDevice()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::setupWriteListToDevice</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.setupWriteListToDevice();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestJudgmentHandleBased()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::judgmentHandleBased</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.judgmentHandleBased();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestSetIoEncoderData()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::setIoEncoderData</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.setIoEncoderData();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestResetIoEncoderData()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::resetIoEncoderData</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.setIoEncoderData();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobSetAbsPos::TestSetCmdStatusInDevice()
{
  log(L"\n<b>Test CanopenJobSetAbsPos::setCmdStatusInDevice</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  CmdStatusInDevice status = CmdStatusInDevice::DEVICE_NOT_SET;
  AxisProfileResult res = testJob.setCmdStatusInDevice(status);
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

