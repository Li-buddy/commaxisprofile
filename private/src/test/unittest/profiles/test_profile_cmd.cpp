
#include "helper/result_helper.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_job_mgr.h"
#include "mockup/mock_axisprofile_category.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "test_profile_cmd.h"
#include "comm/axisprofile/interfaces/i_axisprofile_cmd_c.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestProfileCmd);
REGISTER_TEST_TO_GROUP(TestProfileCmd, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileCmd, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileCmd, Profiles);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProfileCmd::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestProfileCmd");
  log(L"\n----------------------------------------------------------------\n");

  res |= testErrCaseOfCmdHanlding();
  res |= testNormalCaseOfCmdHandling();
  res |= testOfCmdInvokers();
  res |= testWrapperCmds();

  log(L"\n");
  return res;
}

HRESULT TestProfileCmd::testErrCaseOfCmdHanlding()
{
  log(L"\n\nTest case of error happens in AxisprofileCmd class...");
  test::MockFieldbusAdapterSoE adapter {};
  test::MockAxisProfileTrace trace {};
  ProfileBasicInfo profileInfo {AxisProfileType::SERVODRIVE_OVER_ETHERCAT,"dummy", 0, 0, &adapter, trace};
  m_testCmd.reset(new AxisProfileCmd(profileInfo));

  log(L"\nError case when shareHandle is empty...");
  m_testCmd->setSharingHandler(nullptr);

  log(L"\n  Error should happen at getCmdState...");
  auto cmdState = m_testCmd->getCmdState(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_STATE(CmdState::STATE_UNKNOWN, L"\nGet an unexpected state!", cmdState, cmdStateToString);

  log(L"\n  Error should happen at resetCmd...");
  auto aprRes = m_testCmd->resetCmd(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n  Error should happen at abortCmd...");
  aprRes = m_testCmd->abortCmd(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n  Error should happen at sendJobCmd...");
  aprRes = m_testCmd->startCmd(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n  Error should happen at getCmdStatusInDevice...");
  auto cmdStatusInDevice = m_testCmd->getCmdStatusInDevice(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(cmdStatusInDevice, L"\nGet an unexpected result!", CmdStatusInDevice::DEVICE_UNKNOWN);

  log(L"\nError case when shareHandle is exit...");
  log(L"\nCreate and set share handler and job manager...");
  auto sharingHandler = AxisProfileSharingHandler(&profileInfo.m_profileTrace);
  test::MockJobMgr mockJobMgr;
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_JOB_MGR, &mockJobMgr);
  m_testCmd->setSharingHandler(&sharingHandler);

  log(L"\n  Error when job manger can't send a new command...");
  mockJobMgr.m_retValue = AxisProfileResult::ERR_COMMAND_EXECUTION;
  aprRes = m_testCmd->startCmd(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\n  getCmdStatusInDevice error, when job manger doesn't contain the jobObj...");
  mockJobMgr.m_jobObjPtr = nullptr;
  cmdStatusInDevice = m_testCmd->getCmdStatusInDevice(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(cmdStatusInDevice, L"\nGet an unexpected result!", CmdStatusInDevice::DEVICE_UNKNOWN);

  log(L"\n  getCmdStatusInDevice error, when jobObj is not in executing state...");
  mockJobMgr.m_jobObjPtr = &mockJobMgr.m_jobObj;
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_IDLE;
  cmdStatusInDevice = m_testCmd->getCmdStatusInDevice(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(cmdStatusInDevice, L"\nGet an unexpected result!", CmdStatusInDevice::DEVICE_UNKNOWN);

  log(L"\n  getCmdStatusInDevice error, when jobObj doesn't contain job data...");
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_ACTIVE;
  cmdStatusInDevice = m_testCmd->getCmdStatusInDevice(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(cmdStatusInDevice, L"\nGet an unexpected result!", CmdStatusInDevice::DEVICE_UNKNOWN);

  return S_OK;
}

HRESULT TestProfileCmd::testNormalCaseOfCmdHandling()
{
  log(L"\n\nTest case of normal in AxisprofileCmd class...");
  log(L"\nCreate profile information...");
  test::MockFieldbusAdapterSoE adapter {};
  test::MockAxisProfileTrace trace {};
  ProfileBasicInfo profileInfo {AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "dummy", 0, 0, &adapter, trace};
  m_testCmd.reset(new AxisProfileCmd(profileInfo));

  log(L"\nCreate share handler and job manger...");
  auto sharingHandler = AxisProfileSharingHandler(&profileInfo.m_profileTrace);
  test::MockJobMgr mockJobMgr;
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_JOB_MGR, &mockJobMgr);
  m_testCmd->setSharingHandler(&sharingHandler);

  log(L"\n  Everything works fine in getCInterface...");
  auto cmdCItf = m_testCmd->getCInterface();
  LOG_TEST_RESULT_SET_PTR(cmdCItf, L"\nGet an unexpected result!");

  log(L"\n  Everything works fine in getCmdState...");
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_ACTIVATING;
  auto cmdState = m_testCmd->getCmdState(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_STATE(mockJobMgr.m_jobObj.m_objState, L"\nGet an unexpected state!", cmdState, cmdStateToString);

  log(L"\n  Everything works fine in resetCmd...");
  mockJobMgr.m_retValue = AxisProfileResult::STS_OK;
  auto aprRes = m_testCmd->resetCmd(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\n  Everything works fine in abortCmd...");
  aprRes = m_testCmd->abortCmd(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\n  Everything works fine in sendJobCmd...");
  aprRes = m_testCmd->startCmd(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\n  Everything works fine in getCmdStatusInDevice...");
  CmdStatusInDevice expCmdStatusInDevice = CmdStatusInDevice::DEVICE_IN_PROCESS;
  JobDataDeviceStatus m_deviceStatus {};
  m_deviceStatus.getRef().store(expCmdStatusInDevice);
  mockJobMgr.m_jobObj.m_jobData = &m_deviceStatus;
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_ACTIVE;
  auto cmdStatusInDevcie = m_testCmd->getCmdStatusInDevice(CmdType::CLEAR_DEV_ERR);
  LOG_TEST_RESULT(cmdStatusInDevcie, L"\nGet an unexpected result!", expCmdStatusInDevice);

  return S_OK;
}

HRESULT TestProfileCmd::testOfCmdInvokers()
{
  log(L"\n\nTest case of command invokers...");
  log(L"\nCreate profile information...");
  test::MockFieldbusAdapterSoE adapter {};
  test::MockAxisProfileTrace trace {};
  ProfileBasicInfo profileInfo {AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "dummy", 0, 0, &adapter, trace};
  m_testCmd.reset(new AxisProfileCmd(profileInfo));

  log(L"\nCreate share handler and job manger...");
  auto sharingHandler = AxisProfileSharingHandler(&profileInfo.m_profileTrace);
  test::MockJobMgr mockJobMgr;
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_JOB_MGR, &mockJobMgr);
  m_testCmd->setSharingHandler(&sharingHandler);
  mockJobMgr.m_retValue = AxisProfileResult::STS_OK;

  CmdType cmdType {};
  log(L"\n  Test of clearDeviceError...");
  auto aprRes = m_testCmd->clearDeviceError(cmdType);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\n  Test of setAbsolutePosition...");
  aprRes = m_testCmd->setAbsolutePosition(0.0, AxisProfileEncoder::ENCODER_1, cmdType);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\n  Test of referencingAxis...");
  aprRes = m_testCmd->referencingAxis(0.0, AxisProfileEncoder::ENCODER_1, cmdType);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\n  Test of setNewReferenceWithoutMovement...");
  aprRes = m_testCmd->setNewReferenceWithoutMovement(0.0, AxisProfileEncoder::ENCODER_1, RefMode::SET_NONE, cmdType);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\n  Test of setOperationMode...");
  aprRes = m_testCmd->setOperationMode(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  return S_OK;
}

HRESULT TestProfileCmd::testWrapperCmds()
{
  log(L"\n\nTest case of command invokers...");
  log(L"\nCreate profile information...");
  test::MockFieldbusAdapterSoE adapter {};
  test::MockAxisProfileTrace trace {};
  ProfileBasicInfo profileInfo {AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "dummy", 0, 0, &adapter, trace};
  m_testCmd.reset(new AxisProfileCmd(profileInfo));

  log(L"\nCreate share handler and job manger...");
  auto sharingHandler = AxisProfileSharingHandler(&profileInfo.m_profileTrace);
  test::MockJobMgr mockJobMgr;
  test::MockCategory mockCategory;
  mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_DRIVE;
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_JOB_MGR, &mockJobMgr);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
  m_testCmd->setSharingHandler(&sharingHandler);
  mockJobMgr.m_retValue = AxisProfileResult::STS_OK;

  log(L"\nTest normal case of wrapper of clearDeviceError...");
  log(L"\n  1st cycle: command is triggered successfully...");
  auto aprRes = m_testCmd->clearDeviceError2();
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::STS_PENDING);

  log(L"\n  2nd cycle: command state is active but device not completed...");
  JobDataDeviceStatus deviceStatus;
  mockJobMgr.m_jobObj.m_jobData = &deviceStatus;
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_ACTIVE;
  deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  aprRes = m_testCmd->clearDeviceError2();
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::STS_PENDING);

  log(L"\n  3nd cycle: command in device completed...");
  deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
  aprRes = m_testCmd->clearDeviceError2();
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", mockJobMgr.m_retValue);

  log(L"\nTest error case of wrapper of clearDeviceError...");
  log(L"\n  Error case when trigger failed...");
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_IDLE;
  mockJobMgr.m_retValue = AxisProfileResult::ERR_COMMAND_EXECUTION;
  aprRes = m_testCmd->clearDeviceError2();
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::ERR_COMMAND_EXECUTION);

  log(L"\n  Error case get an error command state...");
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_ERROR;
  aprRes = m_testCmd->clearDeviceError2();
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::ERR_INVALID_STATE);

  log(L"\n  Error case get an unknown command state...");
  mockJobMgr.m_jobObj.m_objState = static_cast<CmdState>(-45);
  aprRes = m_testCmd->clearDeviceError2();
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::ERR_INVALID_STATE);

  log(L"\nTest operation-mode cyclic wrapper: start, wait, confirm, and error paths...");
  mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_DRIVE;
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_IDLE;
  mockJobMgr.m_retValue = AxisProfileResult::STS_OK;
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should start and return pending", AxisProfileResult::STS_PENDING);

  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_ACTIVE;
  deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should wait for device confirmation", AxisProfileResult::STS_PENDING);

  deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should complete after confirmation", AxisProfileResult::STS_OK);

  deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_ERROR);
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should reject device errors", AxisProfileResult::ERR_INVALID_STATE);

  deviceStatus.getRef().store(static_cast<CmdStatusInDevice>(99));
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should reject unknown device status", AxisProfileResult::ERR_INVALID_STATE);

  mockJobMgr.m_jobObj.m_jobData = nullptr;
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should reject missing device-status data", AxisProfileResult::ERR_INVALID_STATE);
  mockJobMgr.m_jobObj.m_jobData = &deviceStatus;

  deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_ERROR;
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should reject an error command state", AxisProfileResult::ERR_INVALID_STATE);

  mockJobMgr.m_jobObj.m_objState = static_cast<CmdState>(-45);
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should reject an unknown command state", AxisProfileResult::ERR_INVALID_STATE);

  mockJobMgr.m_jobObj.m_objState = CmdState::STATE_IDLE;
  mockJobMgr.m_retValue = AxisProfileResult::ERR_COMMAND_EXECUTION;
  aprRes = m_testCmd->switchOperationModeWithCheck(AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"operation-mode wrapper should report a failed start", AxisProfileResult::ERR_COMMAND_EXECUTION);

  log(L"\nTest the operation-mode wrapper through its C interface...");
  mockJobMgr.m_retValue = AxisProfileResult::STS_OK;
  const auto* cmdC = m_testCmd->getCInterface();
  aprRes = cmdC->switchOperationModeWithCheck(cmdC->handle, AxisProfileOperationMode::VELOCITY);
  LOG_TEST_RESULT(aprRes, L"C interface should call the operation-mode cyclic wrapper", AxisProfileResult::STS_PENDING);

  log(L"\n  test for encoder category...");
  mockCategory.m_profileCategory = AxisProfileCategory::CATEGORY_ENCODER;
  aprRes = m_testCmd->clearDeviceError2();
  LOG_TEST_RESULT(aprRes, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  return S_OK;
}
