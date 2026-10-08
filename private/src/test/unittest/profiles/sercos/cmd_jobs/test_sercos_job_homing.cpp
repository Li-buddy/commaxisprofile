#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/cmd_jobs/sercos_job_homing.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "test_sercos_job_homing.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestSercosJobHoming);
REGISTER_TEST_TO_GROUP(TestSercosJobHoming, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosJobHoming, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosJobHoming, Profiles_Sercos);


HRESULT TestSercosJobHoming::doInit()
{
  m_mockProperty.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  m_sharingHandlerHolder = std::make_unique<AxisProfileSharingHandler>(&m_mockProfileTrace);
  m_sharingHandler = m_sharingHandlerHolder.get();
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &m_mockScaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &m_mockMappingHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &m_mockProperty);
  return S_OK;
}

HRESULT TestSercosJobHoming::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the sercos profile cmdJobs JobHoming class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestSetArgument();
  hresult |= TestJobEnterActivating();
  hresult |= TestJobEnterAborting();
  hresult |= TestValidateHomingSupport();
  hresult |= TestActiveHomingByProfile();

  return hresult;
}

HRESULT TestSercosJobHoming::doEnd()
{
  HRESULT res = S_OK;
  log(L"\nDeinit test cases...");
  return res;
}


HRESULT TestSercosJobHoming::TestSetArgument()
{
  log(L"\n<b>Test SercosJobHoming::onSetArgument</b>");

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  sercos::SercosJobHoming testJob(profileBasicInfo, m_sharingHandler);
  log(L"\nTest for wrong argument type");
  ArgFixedStop wrongArg;
  AxisProfileResult res = testJob.onSetArgument(&wrongArg);
  RETURN_TEST_RESULT(res, L"onSetArgument should fail when passing wrong argument type", AxisProfileResult::ERR_JOB_DATA_MISMATCH);
  log(L"\nTest for encoder validation type");
  ArgHoming arg;
  auto& ref = arg.getRef();
  ref.mode = RefMode::SET_POS_DEVICE;
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
  log(L"\nTest for out of travel limit");
  auto& cfg = m_mockDeviceParam.getDeviceCfgData();
  cfg.m_softwareLimitActive = true;
  cfg.m_positivePosLimit = INT32_MIN;
  cfg.m_negativePosLimit = INT32_MAX;
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  res = testJob.onSetArgument(&arg);
  LOG_TEST_RESULT(res, L"onSetArgument failed for invalid mode", AxisProfileResult::ERR_INVALID_ARGUMENT);
  comm::axisprofile::DeviceCfgData dataInit;
  m_mockDeviceParam.setReturnCfgData(dataInit);
  return S_OK;
}

HRESULT TestSercosJobHoming::TestJobEnterActivating()
{
  log(L"\n<b>Test SercosJobHoming::onJobEnterActivating</b>");
  struct TestCase
  {
    std::wstring name;
    RefMode mode;
  };

  std::vector<TestCase> testCases =
  {
    {L"SET_POS_DEVICE generates 7 jobs", RefMode::SET_POS_DEVICE},
    {L"SET_POS_PROFILE generates 1 job", RefMode::SET_POS_PROFILE},
    {L"UNDEFINED generates 0 jobs", RefMode::UNDEF_METHOD}
  };
  ProfileBasicInfo profileBasicInfo;
  std::uint32_t devAddress = 1001;
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  sercos::SercosJobHoming testJob(profileBasicInfo, m_sharingHandler);
  ArgHoming arg;
  auto& ref = arg.getRef();
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  for (const auto& tc : testCases)
  {
    log(L"\n Test case: %s", tc.name.c_str());
    ref.mode = tc.mode;
    AxisProfileResult res = testJob.onSetArgument(&arg);
    RETURN_TEST_RESULT(res, L"onSetArgument failed", AxisProfileResult::STS_OK);

    res = testJob.onJobEnterActivating();
    LOG_TEST_RESULT(res, L"onJobEnterActivating failed", AxisProfileResult::STS_OK);
  }

  return S_OK;
}

HRESULT TestSercosJobHoming::TestJobEnterAborting()
{
  log(L"\n<b>Test SercosJobHoming::onJobEnterAborting</b>");
  struct TestCase
  {
    std::wstring name;
    RefMode mode;
  };

  std::vector<TestCase> cases = {
    {L"SET_POS_DEVICE abort has 1 job", RefMode::SET_POS_DEVICE},
    {L"SET_POS_PROFILE abort has 0 job", RefMode::SET_POS_PROFILE},
  };
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  sercos::SercosJobHoming job(profileBasicInfo, m_sharingHandler);

  ArgHoming arg;
  auto& ref = arg.getRef();
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  for (const auto& tc : cases)
  {
    log(L"\n Test case: %s", tc.name.c_str());

    ref.mode = tc.mode;
    AxisProfileResult res = job.onSetArgument(&arg);
    RETURN_TEST_RESULT(res, L"onSetArgument failed", AxisProfileResult::STS_OK);

    res = job.onJobEnterAborting();
    LOG_TEST_RESULT(res, L"onJobEnterAborting failed", AxisProfileResult::STS_OK);
  }

  return S_OK;
}


HRESULT TestSercosJobHoming::TestValidateHomingSupport()
{
  log(L"\n<b>Test validateHomingSupport invalid device state</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  sercos::SercosJobHoming job(profileBasicInfo, m_sharingHandler);
  ///withoutMove needs to be true for testing validateHomingSupport
  auto& cmd = job.m_cmdData;
  cmd.withoutMove = true;
  struct CtrlCase
  {
    bool bit5;
    bool bit6;
    bool bit9;
    bool bit10;
  };

  // very special case when reset new reference without any movement or switch searching
  constexpr CtrlCase cases[] = {
    {false, true, false, false},   // homing switch evaluation: disable
    {true, false, false, false},   // reference mark evaluation: disable
    {true, true, true, false},     // range evaluation: enable
    {true, true, false, true},     // axis blocking evaluation: enable
  };
  auto& ctrl = job.m_devData.ctrlWordS147.m_bits;
  log(L"\nTest for error cases");
  for (auto it: cases)
  {
    ctrl.bit5 = it.bit5;
    ctrl.bit6 = it.bit6;
    ctrl.bit9 = it.bit9;
    ctrl.bit10 = it.bit10;
    AxisProfileResult res = job.validateHomingSupport();
    LOG_TEST_RESULT(res, L"validateHomingSupport should detect invalid device state", AxisProfileResult::ERR_WRONG_DEVICE_STATE);
  }

  log(L"\nTest for correct case");
  ctrl.bit5 = true;
  ctrl.bit6 = true; 
  ctrl.bit9 = false;  
  ctrl.bit10 = false;  
  AxisProfileResult res = job.validateHomingSupport();
  LOG_TEST_RESULT(res, L"ValidateHomingSupport succeed", AxisProfileResult::STS_OK);
  return S_OK;

}

HRESULT TestSercosJobHoming::TestActiveHomingByProfile()
{
  log(L"\n<b>Test activeHomingByProfile - read actual value failure cases</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &m_mockCyclicData);
  sercos::SercosJobHoming job(profileBasicInfo, m_sharingHandler);
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
