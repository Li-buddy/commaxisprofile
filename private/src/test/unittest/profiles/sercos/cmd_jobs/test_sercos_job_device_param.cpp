#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/cmd_jobs/sercos_job_device_param.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "test_sercos_job_device_param.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestSercosJobDeviceParam);
REGISTER_TEST_TO_GROUP(TestSercosJobDeviceParam, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosJobDeviceParam, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosJobDeviceParam, Profiles_Sercos);


HRESULT TestSercosJobDeviceParam::doInit()
{
  return S_OK;
}

HRESULT TestSercosJobDeviceParam::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the sercos profile cmdJobs SercosJobDeviceParam class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestGetJobData();
  hresult |= TestDeviceParamSoEGetData();
  return hresult;
}

HRESULT TestSercosJobDeviceParam::doEnd()
{
  HRESULT res = {S_OK};
  log(L"\nDeinit test cases...");
  return res;
}


HRESULT TestSercosJobDeviceParam::TestGetJobData()
{
  log(L"\n<b>Test SercosJobDeviceParam::getJobData</b>");
  HRESULT res {S_OK};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  sercos::SercosJobDeviceParam job(profileBasicInfo, &shareHandler);

  const IContainer* data = job.getJobData();
  if (data == nullptr)
  {
    res = E_FAIL;
    error(L"TestGetJobData JobData is nullptr" );
  }
  return res;
}

HRESULT TestSercosJobDeviceParam::TestDeviceParamSoEGetData()
{
  log(L"\n<b>Test JobDeviceParamSoE::SetData</b>");
  ReadWriteType type = ReadWriteType::WRITE;
  std::uint16_t id {0};
  sercos::JobDeviceParamSoE jobDeviceParamSoE;
  std::size_t value1 {0};
  std::size_t value2 {0};
  jobDeviceParamSoE.m_ptrData = &value1;
  jobDeviceParamSoE.setData(id, type, value1, value2);
  return S_OK;
}
