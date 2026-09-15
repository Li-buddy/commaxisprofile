#include "helper/result_helper.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_scaling.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "test_profile_device_param.h"

using namespace test;


REGISTER_TEST(TestProfileDeviceParam);
REGISTER_TEST_TO_GROUP(TestProfileDeviceParam, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileDeviceParam, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileDeviceParam, Profiles);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProfileDeviceParam::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile base class.");
  log(L"\n----------------------------------------------------------------");

  log(L"\n");
  hResult |= testCInterface();
  hResult |= testForGetPostionDataFormat();
  hResult |= testDcConfigRelatedMethods();
  hResult |= testFuncInSignalStatusWord();
  hResult |= testMiscellaneous();
  hResult |= testIsDriveSupportChecksum();

  log(L"\n");
  return hResult;
}

HRESULT TestProfileDeviceParam::testCInterface() const
{
  HRESULT hResult = S_OK;
  test::MockAxisProfileTrace profileTraceMock{};
  test::MockScaling scalingMock;
  std::uint8_t driveNumber = 0;

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = m_deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = nullptr;
  profileBasicInfo.m_profileName = "testProfile";

  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  AxisProfileDeviceParamUinttest testClassSoe(profileBasicInfo, &atData, &mdtData);
  AxisProfileSharingHandler shareHandler {&profileTraceMock};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClassSoe);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &scalingMock);
  testClassSoe.setSharingHandler(&shareHandler);

  log(L"\nCall getCInterface()...");
  const IAxisProfileDeviceParamC* deviceParamItf = testClassSoe.getCInterface();
  if (!deviceParamItf)
  {
    error(L"\nThe mapping handler is a null-pointer!");
    hResult = E_FAIL;
    return hResult;
  }

  log(L"\nCall checkBrakeDelaySupport()...");
  bool expectedJudge = true;
  testClassSoe.m_brakeDelySupport = true;
  bool checkSupport = deviceParamItf->checkBrakeDelaySupport(deviceParamItf->handle);
  LOG_TEST_RESULT(checkSupport, L"Got an unexpected result", expectedJudge);

  log(L"\nCall AXISPROFILE_checkBrakeDelaySupports() with nullptr handle......");
  expectedJudge = false;
  checkSupport = deviceParamItf->checkBrakeDelaySupport(nullptr);
  LOG_TEST_RESULT(checkSupport, L"Got an unexpected result", expectedJudge);
  
  log(L"\nCall AXISPROFILE_checkgetErrorBrakeDelayInfo() with error not defined");
  AxisProfileResult expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  bool isActivated = false;
  double delayTime = 0;
  AxisProfileResult result = deviceParamItf->getErrorBrakeDelayInfo(deviceParamItf->handle, AxisProfileDeviceErrorType::ERROR_NOT_DEFINED, isActivated , delayTime);
  LOG_TEST_RESULT(result, L"Got an unexpected result", expectedResult);
  
  log(L"\nCall AXISPROFILE_checkgetErrorBrakeDelayInfo() with error not mapped...");
  result = deviceParamItf->getErrorBrakeDelayInfo(deviceParamItf->handle, AxisProfileDeviceErrorType::ERROR_LIGHT, isActivated , delayTime);
  LOG_TEST_RESULT(result, L"Got an unexpected result", expectedResult);

  log(L"\nCall AXISPROFILE_checkgetErrorBrakeDelayInfo() with error mapped...");
  testClassSoe.m_delayInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_LIGHT, testClassSoe.m_brakeDelayInfo));
  result = deviceParamItf->getErrorBrakeDelayInfo(deviceParamItf->handle, AxisProfileDeviceErrorType::ERROR_LIGHT, isActivated , delayTime);
  expectedResult = AxisProfileResult::STS_OK;
  LOG_TEST_RESULT(result, L"Got an unexpected result", expectedResult);
    
  log(L"\nCall getErrorBrakeDelayInfo() with nullptr handle......");
  result = deviceParamItf->getErrorBrakeDelayInfo(nullptr, AxisProfileDeviceErrorType::ERROR_LIGHT, isActivated , delayTime);
  expectedResult = AxisProfileResult::ERR_EXEC_NULL;
  LOG_TEST_RESULT(result, L"Got an unexpected result", expectedResult);

  log(L"\nCall AXISPROFILE_getErrorBrakeDynLimit() with error not defined...");
  double deceleration = 0;
  double jerk = 0;
  result = deviceParamItf->getErrorBrakeDynLimit(deviceParamItf->handle, AxisProfileDeviceErrorType::ERROR_NOT_DEFINED, deceleration, jerk);
  expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  LOG_TEST_RESULT(result, L"Got an unexpected result", expectedResult);

  log(L"\nCall AXISPROFILE_getErrorBrakeDynLimit() with error not mapped...");
  result = deviceParamItf->getErrorBrakeDynLimit(deviceParamItf->handle, AxisProfileDeviceErrorType::ERROR_LIGHT, deceleration, jerk);
  LOG_TEST_RESULT(result, L"Got an unexpected result", expectedResult);

  log(L"\nCall AXISPROFILE_getErrorBrakeDynLimit() with error mapped...");
  testClassSoe.m_decelerationLimitSupport = true;
  testClassSoe.m_reactionInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_LIGHT, testClassSoe.m_brakeReaction));
  result = deviceParamItf->getErrorBrakeDynLimit(deviceParamItf->handle, AxisProfileDeviceErrorType::ERROR_LIGHT, deceleration, jerk);
  expectedResult = AxisProfileResult::STS_OK;
  LOG_TEST_RESULT(result, L"Got an unexpected result", expectedResult);

  log(L"\nCall AXISPROFILE_getErrorBrakeDynLimit() with nullptr handle...");
  result = deviceParamItf->getErrorBrakeDynLimit(nullptr, AxisProfileDeviceErrorType::ERROR_LIGHT, deceleration, jerk);
  expectedResult = AxisProfileResult::ERR_EXEC_NULL;
  LOG_TEST_RESULT(result, L"Got an unexpected result", expectedResult);

  log(L"\n Call isLatchParamSupport()...");
  testClassSoe.isLatchParamSupport();

  log(L"\nCall AXISPROFILE_checkAndSetModulo() with nullptr handle...");
  std::string errTxt {};
  result = deviceParamItf->checkAndSetModulo(nullptr, 0.0, errTxt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall AXISPROFILE_checkAndSetModulo() with real handle...");
  test::MockAxisProfileDeviceParam mockBootCfg;
  mockBootCfg.setReturnValue(static_cast<AxisProfileResult>(-1));
  result = deviceParamItf->checkAndSetModulo(&mockBootCfg, 0.0, errTxt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", - 1);

  log(L"\nCall checkAndSetModulo of AxisProfileDeviceParam...");
  scalingMock.m_returnValue = static_cast<AxisProfileResult>(-1);
  result = testClassSoe.checkAndSetModulo(0.0, errTxt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", -1);

  log(L"\nRead position positive limit of AxisProfileDeviceParam...");
  double posLmit = testClassSoe.getDeviceCfgData().m_positivePosLimit;
  LOG_TEST_RESULT_FLT(posLmit, L"\nUnexpected return value", INT32_MAX, STD_EPS);

  log(L"\nCall AXISPROFILE_AXISPROFILE_checkFunctionSupported()...");
  comm::axisprofile::FunctionSupportErrorInfo errorInfo;
  bool actualRes = deviceParamItf->checkFunctionSupported(nullptr, comm::axisprofile::FunctionSupport::OPMODE_CYCLIC, errorInfo);
  LOG_TEST_RESULT_ANY(actualRes == false && !errorInfo.addInfo.empty(), L"Got an unexpected result");
  testClassSoe.setFuncSupportResult(true);
  actualRes = deviceParamItf->checkFunctionSupported(deviceParamItf->handle, comm::axisprofile::FunctionSupport::OPMODE_CYCLIC, errorInfo);
  LOG_TEST_RESULT_ANY(actualRes == true, L"Got an unexpected result");


  log(L"\nCall AXISPROFILE_AXISPROFILE_checkStandstill() with nullptr handle...");
  bool stanstill {};
  result = deviceParamItf->checkStandstill(nullptr, stanstill);
  LOG_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall AXISPROFILE_AXISPROFILE_checkStandstill() with valid handle...");
  testClassSoe.m_standstill = true;
  result = deviceParamItf->checkStandstill(deviceParamItf->handle, stanstill);
  LOG_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT_ANY(testClassSoe.m_standstill == stanstill, L"Got an unexpected result");

  log(L"\nCall AXISPROFILE_getRateTorqueForce() with valid handle...");
  scalingMock.m_rateTorqueForce = 12.34;
  double rateToruqeForce {};
  result = deviceParamItf->getRateTorqueForce(deviceParamItf->handle, rateToruqeForce, errTxt);
  LOG_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT_FLT(rateToruqeForce, L"Got an unexpected result", scalingMock.m_rateTorqueForce, SCALING_EPS);

  log(L"\nCall AXISPROFILE_checkAxisHomed() with nullptr handle...");
  bool homed {};
  result = deviceParamItf->checkAxisHomed(nullptr, homed);
  LOG_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall AXISPROFILE_checkAxisHomed() with valid handle...");
  result = deviceParamItf->checkAxisHomed(deviceParamItf->handle, homed);
  LOG_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOTIMPLEMENTED); // base class returns ERR_NOTIMPLEMENTED

  log(L"\n");
  return hResult;
}

HRESULT TestProfileDeviceParam::testForGetPostionDataFormat() const
{
  HRESULT hResult = S_OK;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = m_deviceAddress;
  profileBasicInfo.m_driveNumber = 0;
  profileBasicInfo.m_fieldbusAdapter = nullptr;
  profileBasicInfo.m_profileName = "testProfile";

  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  AxisProfileDeviceParamUinttest testClassSoe(profileBasicInfo, &atData, &mdtData);
  AxisProfileResult expectedResult {AxisProfileResult::ERR_INVALID_ARGUMENT};
  AxisProfileResult actualResult {};

  test::MockAxisProfileTrace profileTraceMock {};
  AxisProfileSharingHandler shareHandler {&profileTraceMock};
  test::MockScaling scalingMock;
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClassSoe);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &scalingMock);
  testClassSoe.setSharingHandler(&shareHandler);

  log(L"\nCall getCInterface()...");
  const IAxisProfileDeviceParamC* deviceParamItf = testClassSoe.getCInterface();
  // test for interface "AXISPROFILE_getPositionDataFormat"
  log(L"\nCall AXISPROFILE_getPositionDataFormat() with nullptr handle......");
  AxisProfilePosDataFormat format {AxisProfilePosDataFormat::FORMAT_ABSOLUTE};
  actualResult = deviceParamItf->getPositionDataFormat(nullptr, format);
  expectedResult = AxisProfileResult::ERR_EXEC_NULL;
  LOG_TEST_RESULT(actualResult, L"Got an unexpected result", expectedResult);

  // test for normal case
  actualResult = deviceParamItf->getPositionDataFormat(deviceParamItf->handle, format);
  expectedResult = AxisProfileResult::STS_OK;
  LOG_TEST_RESULT(actualResult, L"Got an unexpected result", expectedResult);

  return hResult;
}

HRESULT TestProfileDeviceParam::testDcConfigRelatedMethods() const
{
  MockFieldbusAdapterSoE fieldbusAdapter;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";
  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  AxisProfileDeviceParamUinttest testClassSoe(profileBasicInfo, &atData, &mdtData);

  log(L"\nTest readDcConfigInfo read DC information failed...");
  fieldbusAdapter.m_returnValue = AxisProfileResult::ERR_DATALAYER;
  auto result = testClassSoe.readDcConfigInfo();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\nTest readDcConfigInfo DC not support...");
  fieldbusAdapter.m_returnValue = AxisProfileResult::STS_OK;
  fieldbusAdapter.m_dcConfig.m_dcSupported = false;
  result = testClassSoe.readDcConfigInfo();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);
  double syn0 {};
  result = testClassSoe.getDCSync0ShiftTime(syn0);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\nTest readDcConfigInfo every works fine...");
  std::uint32_t expSync0 {123456};
  fieldbusAdapter.m_returnValue = AxisProfileResult::STS_OK;
  fieldbusAdapter.m_dcConfig.m_dcSupported = true;
  fieldbusAdapter.m_dcConfig.m_shiftTimeNs = expSync0;
  result = testClassSoe.readDcConfigInfo();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);
  result = testClassSoe.getDCSync0ShiftTime(syn0);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT_FLT(syn0, L"\nUnexpected return value", NANOSECOND_2_SECOND(expSync0), STD_EPS);

  return S_OK;
}

HRESULT TestProfileDeviceParam::testFuncInSignalStatusWord() const
{
  MockFieldbusAdapterSoE fieldbusAdapter;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";
  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  AxisProfileDeviceParamUinttest testDeviceParam(profileBasicInfo, &atData, &mdtData);

  log(L"\nTest checking signal status word without share handler...");
  FunctionSupportErrorInfo errInfo;
  SignalStatusWordFunction func;
  testDeviceParam.setSharingHandler(nullptr);
  auto res = testDeviceParam.isFuncInSignalStatusWord(func, errInfo);
  LOG_TEST_RESULT_ANY(res == false, L"\n  unexpected value.");

  log(L"\nTest the checking of undefined functionality.");
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);

  auto& assign = mappingHandlerMockup.getProfileAssignedInfo();
  assign.m_input.m_signalStatusWord = true;
  testDeviceParam.m_signalStatusWordFuncs.clear();
  func.function = FunctionSupport::HOMING_STATUS_CYCLIC;
  res = testDeviceParam.isFuncInSignalStatusWord(func, errInfo);
  LOG_TEST_RESULT_ANY(res == false, L"\n  unexpected value.");

  log(L"\nTest the checking of the unsupported functionality.");
  func.function = FunctionSupport::HOMING_STATUS_CYCLIC;
  testDeviceParam.m_signalStatusWordFuncs.emplace_back(FunctionSupport::HOMING_STATUS_CYCLIC,
                                                      [](const std::uint16_t& /*param*/, const std::uint16_t& /*bit*/) -> bool { return false; },
                                                      false,
                                                      0);
  res = testDeviceParam.isFuncInSignalStatusWord(func, errInfo);
  LOG_TEST_RESULT_ANY(res == false, L"\n  unexpected value.");

  return S_OK;
}

HRESULT TestProfileDeviceParam::testMiscellaneous() const
{
  ProfileBasicInfo profileBasicInfo;
  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  AxisProfileDeviceParamUinttest testClassSoe(profileBasicInfo, &atData, &mdtData);
  double resRateTorqueForce {};
  std::string errTxt {};

  log(L"\nTest error case of getRateTorqueForce...");
  log(L"\n ErrorCase1: invalid sharing handler...");
  auto aprRes = testClassSoe.getRateTorqueForce(resRateTorqueForce, errTxt);
  LOG_TEST_RESULT(aprRes, L"\nUnexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  return S_OK;
}

HRESULT TestProfileDeviceParam::testIsDriveSupportChecksum()
{
  log(L"\n<b>Test the isDriveSupportChecksum function.</b>");
  ProfileBasicInfo profileBasicInfoSoe;
  profileBasicInfoSoe.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  AxisProfileDeviceParamUinttest testClassSoe {profileBasicInfoSoe, &atData, &mdtData};

  ProfileBasicInfo profileBasicInfoCoe;
  profileBasicInfoCoe.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  AxisProfileDeviceParamUinttest testClassCoe {profileBasicInfoCoe, &atData, &mdtData};

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileProperty property {};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClassSoe);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClassCoe);

  log(L"\nIsDriveSupportChecksum test, ctrlx drive with version 5.2...");
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  property.setSoftwareVersion(5, 2);
  RETURN_TEST_RESULT_ANY(testClassSoe.isDriveSupportChecksum(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT_ANY(testClassCoe.isDriveSupportChecksum(), L"\nUnexpected return value.");

  log(L"\nIsDriveSupportChecksum test, ctrlx drive with version 5.1...");
  property.setSoftwareVersion(5, 1);
  RETURN_TEST_RESULT_ANY(!testClassSoe.isDriveSupportChecksum(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT_ANY(!testClassCoe.isDriveSupportChecksum(), L"\nUnexpected return value.");

  log(L"\nIsDriveSupportChecksum test, ctrlx drive with version 4.3...");
  property.setSoftwareVersion(4, 3);
  RETURN_TEST_RESULT_ANY(!testClassSoe.isDriveSupportChecksum(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT_ANY(!testClassCoe.isDriveSupportChecksum(), L"\nUnexpected return value.");

  log(L"\nIsDriveSupportChecksum test, indra drive with version 5.1...");
  property.setSoftwareVersion(5, 1);
  property.setDeviceSeries(DeviceSeries::INDRADRIVE);
  RETURN_TEST_RESULT_ANY(testClassSoe.isDriveSupportChecksum(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT_ANY(!testClassCoe.isDriveSupportChecksum(), L"\nUnexpected return value.");

  log(L"\nIsDriveSupportChecksum test, xc20 drive...");
  property.setSoftwareVersion(5, 2);
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE_XC20);
  RETURN_TEST_RESULT_ANY(!testClassSoe.isDriveSupportChecksum(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT_ANY(!testClassCoe.isDriveSupportChecksum(), L"\nUnexpected return value.");

  log(L"\nIsDriveSupportChecksum test, other 3rd company drive...");
  property.setDeviceSeries(DeviceSeries::NOTINDICATES);
  RETURN_TEST_RESULT_ANY(!testClassSoe.isDriveSupportChecksum(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT_ANY(!testClassCoe.isDriveSupportChecksum(), L"\nUnexpected return value.");

  return S_OK;
}
