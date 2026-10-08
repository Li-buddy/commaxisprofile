#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "test_canopen_job_device_param.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenJobDeviceParam);
REGISTER_TEST_TO_GROUP(TestCanopenJobDeviceParam, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobDeviceParam, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobDeviceParam, Profiles_Canopen);


HRESULT TestCanopenJobDeviceParam::doInit()
{
  m_sharingHandlerHolder = std::make_unique<AxisProfileSharingHandler>(&m_mockProfileTrace);
  m_sharingHandler = m_sharingHandlerHolder.get();
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &m_mockScaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  return S_OK;
}

HRESULT TestCanopenJobDeviceParam::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the canopen profile cmdJobs CanopenJobDeviceParam class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestGetJobData();
  hresult |= TestGetJobDataRef();
  hresult |= TestJobEnterActivating();
  hresult |= TestJobActivating();
  hresult |= TestJobEnterAborting();
  hresult |= TestCompareIdAndType();
  hresult |= TestGetData();
  hresult |= TestSetData();

  return hresult;
}

HRESULT TestCanopenJobDeviceParam::doEnd()
{
  HRESULT res = {S_OK};
  log(L"\nDeinit test cases...");
  return res;
}

HRESULT TestCanopenJobDeviceParam::TestGetJobData()
{
  log(L"\n<b>Test CanopenJobDeviceParam::getJobData</b>");
  HRESULT res {S_OK};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  canopen::CanopenJobDeviceParam job(profileBasicInfo, m_sharingHandler);

  const IContainer* data = job.getJobData();
  if (data == nullptr)
  {
    res = E_FAIL;
    error(L"TestGetJobData JobData is nullptr" );
  }
  return res;
}

HRESULT TestCanopenJobDeviceParam::TestGetJobDataRef()
{
  log(L"\n<b>Test CanopenJobDeviceParam::getJobDataRef</b>");
  HRESULT res {S_OK};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  canopen::CanopenJobDeviceParam testJob(profileBasicInfo, m_sharingHandler);
  const IContainer* data = testJob.getJobDataRef();
  if (data == nullptr)
  {
    res = E_FAIL;
    error(L"TestGetJobDataRef JobData is nullptr" );
  }
  return res;
}

HRESULT TestCanopenJobDeviceParam::TestJobEnterActivating()
{
  log(L"\n<b>Test CanopenJobDeviceParam::onJobEnterActivating</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  CanopenJobDeviceParamEx testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.onJobEnterActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT TestCanopenJobDeviceParam::TestJobActivating()
{
  log(L"\n<b>Test CanopenJobDeviceParam::onJobActivating readAndCheckParamAsync</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenParamAccess paramAccess(profileBasicInfo);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &paramAccess);
  CanopenJobDeviceParamEx testJob(profileBasicInfo, m_sharingHandler);  
  AxisProfileResult res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_PENDING);

  log(L"\nTest writeAndCheckParamAsync");
  auto jobDataContainer = testJob.getJobDataRef();
  auto& jobData = canopen::DatJobDeviceParamCoE::safeCast(jobDataContainer)->getRef();
  jobData.m_readWriteType = ReadWriteType::WRITE;
  res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_PENDING);
  return S_OK;
}

HRESULT TestCanopenJobDeviceParam::TestJobEnterAborting()
{
  log(L"\n<b>Test CanopenJobDeviceParam::onJobEnterAborting</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  CanopenJobDeviceParamEx testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.onJobEnterAborting();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);
  return S_OK;
}


HRESULT TestCanopenJobDeviceParam::TestCompareIdAndType()
{
  log(L"\n<b>Test JobDeviceParamCoE::CompareIdAndType</b>");
  canopen::JobDeviceParamCoE jobDeviceParamCoE;
  ReadWriteType type = ReadWriteType::READ;
  ObjectDirEntry id;
  bool ret = jobDeviceParamCoE.compareIdAndType(id, type);
  LOG_TEST_RESULT(ret, L"Get an unexpected result", true);
  return S_OK;
}

HRESULT TestCanopenJobDeviceParam::TestGetData()
{
  log(L"\n<b>Test JobDeviceParamCoE::GetData</b>");
  HRESULT res {S_OK};
  canopen::JobDeviceParamCoE jobDeviceParamCoE;
  std::size_t value {};
  jobDeviceParamCoE.m_ptrData = &value;
  jobDeviceParamCoE.getData(value);
  return res;
}

HRESULT TestCanopenJobDeviceParam::TestSetData()
{
  log(L"\n<b>Test JobDeviceParamCoE::SetData</b>");
  ReadWriteType type = ReadWriteType::READ;
  ObjectDirEntry id;
  canopen::JobDeviceParamCoE jobDeviceParamCoE;
  std::size_t value1 {};
  std::size_t value2 {};
  jobDeviceParamCoE.m_ptrData = &value1;
  jobDeviceParamCoE.setData(id, type, value1, value2);

  log(L"\n Test for write type");
  type = ReadWriteType::WRITE;
  jobDeviceParamCoE.setData(id, type, value1, value2);

  return S_OK;
}

