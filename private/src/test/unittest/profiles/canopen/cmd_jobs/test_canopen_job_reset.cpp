#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/canopen/cmd_jobs/canopen_job_reset.h"
#include "profiles/canopen/drive/canopen_status_control_drive.h"
#include "profiles/canopen/drive/canopen_signal_func_handler.h"

//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "test_canopen_job_reset.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenJobReset);
REGISTER_TEST_TO_GROUP(TestCanopenJobReset, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobReset, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenJobReset, Profiles_Canopen);


HRESULT TestCanopenJobReset::doInit()
{
  m_sharingHandlerHolder = std::make_unique<AxisProfileSharingHandler>(&m_mockProfileTrace);
  m_sharingHandler = m_sharingHandlerHolder.get();
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &m_mockScaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &m_mockDeviceParam);
  return S_OK;
}

HRESULT TestCanopenJobReset::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the canopen profile cmdJobs CanopenJobReset class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= TestJobEnterActivating();
  hresult |= TestJobActivating();

  return hresult;
}

HRESULT TestCanopenJobReset::doEnd()
{
  HRESULT res = {S_OK};
  log(L"\nDeinit test cases...");
  return res;
}

HRESULT TestCanopenJobReset::TestJobEnterActivating()
{
  log(L"\n<b>Test CanopenJobReset::onJobEnterActivating for empty cyclicData</b>");
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  canopen::CanopenJobReset testJob(profileBasicInfo, m_sharingHandler);
  AxisProfileResult res = testJob.onJobEnterActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::ERR_EXEC_NULL);
  return S_OK;
}

HRESULT TestCanopenJobReset::TestJobActivating()
{
  log(L"\n<b>Test CanopenJobReset::onJobActivating</b>");
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  atData.m_atData.m_opModeDisplay = 9;
  atData.m_statusWord.m_word = 8;  //bit3
  mdtData.m_mdtData.m_opMode = 9;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &m_mockFieldbusAdapter;
  canopen::CanopenStatusControlDrive testControlword(profileBasicInfo, &atData, &mdtData);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, &testControlword);
  canopen::CanopenJobReset testJob(profileBasicInfo, m_sharingHandler);
  testJob.m_asyncSm.switchToCmdStartStep();
  log(L"\nTest CanopenJobReset::onJobActivating for CommandStep::CMD_START");
  AxisProfileResult res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_PENDING);

  log(L"\nTest CanopenJobReset::onJobActivating for CommandStep::CMD_SET");
  res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_PENDING);

  log(L"\nTest CanopenJobReset::onJobActivating for CommandStep::CMD_CHECK_STATUS");
  atData.m_statusWord.m_bits.bit3 = 0;
  res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_PENDING);

  log(L"\nTest CanopenJobReset::onJobActivating for CommandStep::CMD_DONE");
  res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nTest CanopenJobReset::onJobActivating for signal func command");
  testJob.m_isSignalFuncCmd = true;
  testJob.m_asyncSm.switchToCmdStartStep();
  res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_PENDING);

  log(L"\nTest CanopenJobReset::onJobActivating for CommandStep::CMD_SET");
  res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_PENDING);

  log(L"\nTest CanopenJobReset::onJobActivating for CommandStep::CMD_CHECK_STATUS");
  atData.m_statusWord.m_bits.bit3 = 0;
  test::MockAxisProfileCyclicData mockCyclic {};
  testJob.m_shareHandle->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &mockCyclic);
  canopen::CanopenSignalFuncHandler signalHandler{profileBasicInfo};
  signalHandler.init(testJob.m_shareHandle);
  testJob.m_signalFuncHandler = &signalHandler;
  testJob.m_signalFunc.statusSupported = true;
  res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_PENDING);

  log(L"\nTest CanopenJobReset::onJobActivating for CommandStep::CMD_DONE");
  res = testJob.onJobActivating();
  LOG_TEST_RESULT(res, L"Get an unexpected result", AxisProfileResult::STS_OK);

  return S_OK;
}
