#include "test_axisprofile_device_param_encoder.h"
#include "helper/result_helper.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_axisprofile_cyclic_data.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileDeviceParamEncoder);
REGISTER_TEST_TO_GROUP(TestAxisProfileDeviceParamEncoder, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileDeviceParamEncoder, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileDeviceParamEncoder, 4_EncoderTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestAxisProfileDeviceParamEncoder::doInit()
{
  HRESULT res = S_OK;

  log(L"\nInit test data member...");
  m_profileInfo = std::make_shared<ProfileBasicInfo>();
  m_telegramAT = std::make_shared<AcknowledgeTelegram>();
  m_category = std::make_unique<EncoderCategoryAdapterCoE>(*m_profileInfo.get(), m_telegramAT.get(), m_telegramMDT.get());
  m_sharingHandler = std::make_unique<AxisProfileSharingHandler>(&m_profileInfo->m_profileTrace);
  m_category->registerSharingITFs(m_sharingHandler.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_category.get());
  
  return res;
}

HRESULT TestAxisProfileDeviceParamEncoder::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test all the TestAxisProfileDeviceParamEncoder interface");
  log(L"\n----------------------------------------------------------------\n");

  log(L"\n\t Test of booting...");
  test::MockAxisProfileProperty property {};
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  auto result = m_category->getDeviceParamInternal()->booting();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of checkAndSetModulo...");
  double moduloValue {};
  std::string errTxt {};
  result = m_category->getDeviceParamInternal()->checkAndSetModulo(moduloValue, errTxt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of checkBrakeDelaySupport...");
  m_category->getDeviceParamInternal()->checkBrakeDelaySupport();

  log(L"\n\t Test of checkFunctionSupported...");
  FunctionSupport func {};
  FunctionSupportErrorInfo info {};
  bool isSupport = m_category->getDeviceParamInternal()->checkFunctionSupported(func, info);
  RETURN_TEST_RESULT(isSupport, L"\nUnexpected return value", false);

  func = FunctionSupport::HOMING_STATUS_CYCLIC;
  isSupport = m_category->getDeviceParamInternal()->checkFunctionSupported(func, info);
  RETURN_TEST_RESULT(isSupport, L"\nUnexpected return value", false);

  func = FunctionSupport::HOMING_CMD_CYCLIC;
  isSupport = m_category->getDeviceParamInternal()->checkFunctionSupported(func, info);
  RETURN_TEST_RESULT(isSupport, L"\nUnexpected return value", false);

  test::MockAxisProfileMappingHandler mappinghandler {};
  auto& assigned = mappinghandler.getProfileAssignedInfo();
  assigned.m_input.m_isInReference = true;
  assigned.m_output.m_setReference = true;
  assigned.m_output.m_referenceOffset = true;
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappinghandler);

  func = FunctionSupport::HOMING_STATUS_CYCLIC;
  isSupport = m_category->getDeviceParamInternal()->checkFunctionSupported(func, info);
  RETURN_TEST_RESULT(isSupport, L"\nUnexpected return value", true);

  func = FunctionSupport::HOMING_CMD_CYCLIC;
  isSupport = m_category->getDeviceParamInternal()->checkFunctionSupported(func, info);
  RETURN_TEST_RESULT(isSupport, L"\nUnexpected return value", true);

  log(L"\n\t Test of getCInterface...");
  m_category->getDeviceParamInternal()->getCInterface();

  log(L"\n\t Test of getDCSync0ShiftTime...");
  double syncTime {};
  result = m_category->getDeviceParamInternal()->getDCSync0ShiftTime(syncTime);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getDeprecatedCInterface...");
  m_category->getDeviceParamInternal()->getDeprecatedCInterface();

  log(L"\n\t Test of getDeviceCfgData...");
  m_category->getDeviceParamInternal()->getDeviceCfgData();

  log(L"\n\t Test of getErrorBrakeDelayInfo...");
  AxisProfileDeviceErrorType errorType {};
  bool activated {};
  double delayTime {};
  result = m_category->getDeviceParamInternal()->getErrorBrakeDelayInfo(errorType, activated, delayTime);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getErrorBrakeDynLimit...");
  double deceleration {};
  double jerk {};
  result = m_category->getDeviceParamInternal()->getErrorBrakeDynLimit(errorType, deceleration, jerk);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of isLatchParamSupport...");
  m_category->getDeviceParamInternal()->isLatchParamSupport();

  log(L"\n\t Test of getOutputEffectiveDelay...");
  result = m_category->getDeviceParamInternal()->getOutputEffectiveDelay(delayTime);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getPositionDataFormat...");
  AxisProfilePosDataFormat format {};
  result = m_category->getDeviceParamInternal()->getPositionDataFormat(format);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of readDriveSignalStatusWordCfg...");  
  test::MockAxisProfileCyclicData cyclicData {};
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &cyclicData);
  auto signalFuncMgr = m_category->getDeviceParamInternal()->getSignalFuncHandler();
  RETURN_TEST_RESULT_PTR(signalFuncMgr, L"\nUnexpected return value", nullptr);

  log(L"\n\t Test of getOpModeAvailInfo...");
  m_category->getDeviceParamInternal()->getOpModeAvailInfo();

  log(L"\n\t Test of getOpModeAvailInfo...");
  std::vector<std::string> idnList;
  result = m_category->getDeviceParamInternal()->getChecksumIdnList(idnList);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of checkStandstill...");
  bool standStill {};
  m_category->getDeviceParamInternal()->checkStandstill(standStill);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of checkAxisHomed...");
  bool homed {};
  result = m_category->getDeviceParamInternal()->checkAxisHomed(homed);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  return res;
}
