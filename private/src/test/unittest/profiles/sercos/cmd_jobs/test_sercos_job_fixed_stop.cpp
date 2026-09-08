#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/cmd_jobs/sercos_job_fixed_stop.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "test_sercos_job_fixed_stop.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestSercosJobFixedStop);
REGISTER_TEST_TO_GROUP(TestSercosJobFixedStop, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosJobFixedStop, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosJobFixedStop, Profiles_Sercos);


HRESULT TestSercosJobFixedStop::doInit()
{
  m_sharingHandlerHolder = std::make_unique<AxisProfileSharingHandler>(&m_mockProfileTrace);
  m_sharingHandler = m_sharingHandlerHolder.get();
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &m_mockScaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  return S_OK;
}

HRESULT TestSercosJobFixedStop::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the sercos profile cmdJobs SercosJobFixedStop class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestSetArgument();
  hresult |= TestvalidateConfig();

  return hresult;
}

HRESULT TestSercosJobFixedStop::doEnd()
{
  HRESULT res = S_OK;
  log(L"\nDeinit test cases...");
  return res;
}


HRESULT TestSercosJobFixedStop::TestSetArgument()
{
  log(L"\n<b>Test SercosJobFixedStop::onSetArgument</b>");

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = m_mockProfileTrace;
  sercos::SercosJobFixedStop testJob(profileBasicInfo, m_sharingHandler);
  log(L"\nTest for wrong argument");
  ArgHoming wrongArg;
  AxisProfileResult res = testJob.onSetArgument(&wrongArg);
  LOG_TEST_RESULT(res, L"onSetArgument should fail when passing wrong argument type", AxisProfileResult::ERR_JOB_DATA_MISMATCH);
  return S_OK;
}

HRESULT TestSercosJobFixedStop::TestvalidateConfig()
{
  log(L"\n<b>Test SercosJobFixedStop::validateConfig</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  sercos::SercosJobFixedStop testJob(profileBasicInfo, m_sharingHandler);

  log(L"\nTest for null pointer of sharing handler");
  testJob.m_shareHandle = nullptr;
  AxisProfileResult res = testJob.validateConfig();
  LOG_TEST_RESULT(res, L"validateConfig should fail when sharing handler is null", AxisProfileResult::ERR_EXEC_NULL);
  testJob.m_shareHandle = m_sharingHandler;

  log(L"\nTest for torque/force target higher than max limit");
  auto& deviceCfg = m_mockDeviceParam.getDeviceCfgData();
  deviceCfg.m_trqLimitMax = 0;
  res = testJob.validateConfig();
  LOG_TEST_RESULT(res, L"validateConfig should reject out-of-range position", AxisProfileResult::ERR_INVALID_ARGUMENT);
  return S_OK;

}

