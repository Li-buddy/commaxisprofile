#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/cmd_jobs/sercos_job_setabspos.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_category.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "profiles/sercos/drive/sercos_signal_func_handler.h"
#include "test_sercos_job_setabspos.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestSercosJobSetAbsPos);
REGISTER_TEST_TO_GROUP(TestSercosJobSetAbsPos, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosJobSetAbsPos, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosJobSetAbsPos, Profiles_Sercos);


HRESULT TestSercosJobSetAbsPos::doInit()
{
  m_mockProperty.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  m_sharingHandlerHolder = std::make_unique<AxisProfileSharingHandler>(&m_mockProfileTrace);
  m_sharingHandler = m_sharingHandlerHolder.get();
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &m_mockScaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &m_mockMappingHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &m_mockProperty);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &m_mockCategory);
  return S_OK;
}

HRESULT TestSercosJobSetAbsPos::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the sercos profile cmdJobs SercosJobSetAbsPos class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestSetArgument();
  hresult |= TestJobEnterActivating();
  hresult |= TestJobEnterAborting();
  hresult |= TestvalidateConfig();
  hresult |= TestjudgmentHandleBased();
  hresult |= TestSignalFuncCommand();

  return hresult;
}

HRESULT TestSercosJobSetAbsPos::doEnd()
{
  HRESULT res = S_OK;
  log(L"\nDeinit test cases...");
  return res;
}


HRESULT TestSercosJobSetAbsPos::TestSetArgument()
{
  log(L"\n<b>Test SercosJobSetAbsPos::onSetArgument</b>");

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  sercos::SercosJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  log(L"\nTest case: wrong argument type");
  ArgFixedStop wrongArg;
  AxisProfileResult res = testJob.onSetArgument(&wrongArg);
  LOG_TEST_RESULT(res, L"onSetArgument should fail when passing wrong argument type", AxisProfileResult::ERR_JOB_DATA_MISMATCH);
  return S_OK;
}

HRESULT TestSercosJobSetAbsPos::TestJobEnterActivating()
{
  log(L"\n<b>Test SercosJobSetAbsPos::onJobEnterActivating</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  sercos::SercosJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
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

HRESULT TestSercosJobSetAbsPos::TestJobEnterAborting()
{
  log(L"\n<b>Test SercosJobSetAbsPos::onJobEnterAborting</b>");

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  sercos::SercosJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  ArgSetAbsolutePosition arg;
  auto& ref = arg.getRef();
  ref.encoder = AxisProfileEncoder::ENCODER_1;
  for (const auto& tc : m_testCases)
  {
    log(L"\n Test case: %s", tc.name.c_str());
    m_mockCategory.m_profileCategory = tc.nType;
    AxisProfileResult res = testJob.onSetArgument(&arg);
    RETURN_TEST_RESULT(res, L"onSetArgument failed", AxisProfileResult::STS_OK);

    res = testJob.onJobEnterAborting();
    LOG_TEST_RESULT(res, L"onJobEnterAborting unexpected failed", tc.ret);
  }
  //recover the category after test
  m_mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_DRIVE;
  return S_OK;
}

HRESULT TestSercosJobSetAbsPos::TestvalidateConfig()
{
  log(L"\n<b>Test SercosJobSetAbsPos::validateConfig</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  sercos::SercosJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);

  log(L"\nTest case: out of travel limit");
  auto& cfg = m_mockDeviceParam.getDeviceCfgData();
  cfg.m_softwareLimitActive = true;
  cfg.m_positivePosLimit = INT32_MIN;
  cfg.m_negativePosLimit = INT32_MAX;
  AxisProfileResult res = testJob.validateConfig();
  LOG_TEST_RESULT(res, L"validateConfig should reject out-of-range position", AxisProfileResult::ERR_INVALID_ARGUMENT);
  comm::axisprofile::DeviceCfgData dataInit;  //recover
  m_mockDeviceParam.setReturnCfgData(dataInit);

  log(L"\nTest case: undefined encoder");
  testJob.m_encoder = AxisProfileEncoder::UNDEFINED;
  res = testJob.validateConfig();
  LOG_TEST_RESULT(res, L"validateConfig should fail when encoder is undefined", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\nTest case: wrong device state for encoder_2(bit6/bit7)");
  testJob.m_encoder = AxisProfileEncoder::ENCODER_2;
  WordStructure prop;
  prop.m_bits.bit6 = true;
  prop.m_bits.bit7 = true;
  cfg.m_encoder2Property = prop.m_word;
  res = testJob.validateConfig();
  LOG_TEST_RESULT(res, L"validateConfig should reject absolute encoder in relative mode", AxisProfileResult::ERR_WRONG_DEVICE_STATE);

  log(L"\nTest case: wrong device state for encoder_2(bit6/bit8)");
  prop.m_bits.bit6 = false;
  prop.m_bits.bit8 = false;
  cfg.m_encoder2Property = prop.m_word;
  res = testJob.validateConfig();
  LOG_TEST_RESULT(res, L"validateConfig should reject absolute encoder in relative mode", AxisProfileResult::ERR_WRONG_DEVICE_STATE);

  log(L"\nTest case: encoder_2 mode and correct params");
  prop.m_bits.bit8 = true;
  cfg.m_encoder2Property = prop.m_word;
  res = testJob.validateConfig();
  LOG_TEST_RESULT(res, L"validateConfig succeed", AxisProfileResult::STS_OK);
  return S_OK;

}

HRESULT TestSercosJobSetAbsPos::TestjudgmentHandleBased()
{
  log(L"\n<b>Test SercosJobSetAbsPos::judgmentHandleBased</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  sercos::SercosJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);

  log(L"\nTest case: share handle is null");
  testJob.m_shareHandle = nullptr;
  AxisProfileResult res = testJob.judgmentHandleBased();
  LOG_TEST_RESULT(res, L"judgmentHandleBased should fail when share handle is null", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest case: share handle is valid");
  testJob.m_shareHandle = m_sharingHandler;
  res = testJob.judgmentHandleBased();
  LOG_TEST_RESULT(res, L"judgmentHandleBased should pass when share handle is valid", AxisProfileResult::STS_OK);

  log(L"\nTest case: in reference state and set reference with offset");
  auto& assigned = m_mockMappingHandler.getProfileAssignedInfo();
  assigned.m_input.m_isInReference = false;
  assigned.m_output.m_setReference = true;
  assigned.m_output.m_referenceOffset = true;
  res = testJob.judgmentHandleBased();
  LOG_TEST_RESULT(res, L"judgmentHandleBased should handle as control based", AxisProfileResult::STS_OK);
  return S_OK;

}

HRESULT TestSercosJobSetAbsPos::TestSignalFuncCommand()
{
  log(L"\n<b>Test SercosJobSetAbsPos signal function command path</b>");

  test::MockAxisProfileCyclicData mockCyclicData;
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclicData);
  m_mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_DRIVE;

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;

  sercos::SercosSignalFuncHandler signalFuncHandler(profileBasicInfo);
  AxisProfileResult res = signalFuncHandler.init(m_sharingHandler);
  RETURN_TEST_RESULT(res, L"signal function handler init failed", AxisProfileResult::STS_OK);

  sercos::SercosJobSetAbsPos testJob(profileBasicInfo, m_sharingHandler);
  res = testJob.onJobEnterActivating();
  RETURN_TEST_RESULT(res, L"onJobEnterActivating failed", AxisProfileResult::STS_OK);

  testJob.m_signalFuncCmd.initSignalFuncHandler(&signalFuncHandler);
  testJob.m_isSignalFuncCmd = true;
  SignalFunction signalFunc {FunctionSupport::SET_ABS_POS_CMD_CYCLIC};
  signalFunc.statusSupported = true;
  signalFunc.controlSupported = true;
  testJob.m_signalFuncCmd = SignalFuncCmd(signalFunc);
  testJob.m_signalFuncCmd.initSignalFuncHandler(&signalFuncHandler);

  res = testJob.activeSetAbsPosByDevice();
  LOG_TEST_RESULT(res, L"activeSetAbsPosByDevice should use signal function path", AxisProfileResult::STS_PENDING);

  res = testJob.abortSetAbsPosByDevice();
  LOG_TEST_RESULT(res, L"abortSetAbsPosByDevice should use signal function path", AxisProfileResult::STS_OK);

  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA);
  m_mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_DRIVE;
  return S_OK;
}
