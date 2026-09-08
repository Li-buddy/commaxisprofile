#include "comm/axisprofile/interfaces/i_axisprofile_cyclic_data_c.h"
#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/drive/sercos_scaling_strategy_drive.h"
#include "test_profile_cyclic_data.h"

#include "mockup/external_bundles/mock_datalayer_client_ex.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestProfileCyclicData);
REGISTER_TEST_TO_GROUP(TestProfileCyclicData, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileCyclicData, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileCyclicData, Profiles);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProfileCyclicData::TestProfileCyclicData()
{}


TestProfileCyclicData::~TestProfileCyclicData(void)
{}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProfileCyclicData::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the base class for profile cyclic data.");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testCInterface();
  hResult |= testCheckIfTelegramItemAssigned();
  hResult |= testForcheckMasterStatus();
  hResult |= testForcheckSlaveStatus();
  hResult |= testForCheckActualValueAvailability();

  return hResult;
}

HRESULT TestProfileCyclicData::testCInterface() const
{
  HRESULT hResult = S_OK;
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};

  ProfileTrace traceHelper;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  test::MockAxisProfileTrace profileTraceMock {};

  auto pProfileCategory = std::make_unique<DriveCategoryAdapterSoE>(profileBasicInfo, &atData, &mdtData);
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_profileName = "testProfile";
  AxisProfileCyclicDataUnittest testClass(profileBasicInfo, &atData, &mdtData);

  log(L"\n<b>Test of the cyclic data C-Interface</b>");

  log(L"\nCall getCInterface()...");
  const IAxisProfileCyclicDataC* cyclicDataItf = testClass.getCInterface();
  if (!cyclicDataItf)
  {
    error(L"\nCould not get the C-Interface (received nullptr)!");
    return E_FAIL;
  }

  log(L"\nCall AXISPROFILE_getActualValuesDevice() with nullptr handle...");
  CommandValuesDevice cmdValues {};
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult aprResult = cyclicDataItf->setCommandValuesDevice(nullptr, cmdValues);
  LOG_TEST_RESULT(expResult, L"\n Method did not fail with expected error code!", aprResult);

  expResult = AxisProfileResult::STS_OK;
  aprResult = cyclicDataItf->setCommandValuesDevice(cyclicDataItf->handle, cmdValues);
  LOG_TEST_RESULT(expResult, L"\n Method did not fail with expected error code!", aprResult);

  log(L"\nCall AXISPROFILE_getActualValuesDevice() with nullptr handle...");
  ActualValuesDevice actValues {};
  expResult = AxisProfileResult::ERR_EXEC_NULL;
  aprResult = cyclicDataItf->getActualValuesDevice(nullptr, &actValues);
  LOG_TEST_RESULT(expResult, L"\n Method did not fail with expected error code!", aprResult);

  log(L"\nCall AXISPROFILE_getNRTActualValue()...");
  AxisProfileValueTypeID type = AxisProfileValueTypeID::ACTUAL_POSITION;
  DataAccessErrorInfo errorInfo {};
  cyclicDataItf->checkActualValueAvailabilityNRT(cyclicDataItf->handle, type, errorInfo);

  log(L"\nCall AXISPROFILE_getNRTActualValue() with nullptr handle...");
  bool boolAprResult = cyclicDataItf->checkActualValueAvailabilityNRT(nullptr, type, errorInfo);
  LOG_TEST_RESULT_ANY(boolAprResult == false, L"Method did not fail with expected error code");

  log(L"\nCall AXISPROFILE_getDigitalInputValueBit() with nullptr handle...");
  bool digitalInputValueBit {};
  aprResult = cyclicDataItf->getDigitalInputValueBit(nullptr, DigitalInputValueBitType::REFERENCE_SWITCH_BIT, digitalInputValueBit);
  LOG_TEST_RESULT(aprResult, L"Got an unexpected result", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall AXISPROFILE_getDigitalInputValueBit()...");
  aprResult = cyclicDataItf->getDigitalInputValueBit(cyclicDataItf->handle, DigitalInputValueBitType::REFERENCE_SWITCH_BIT, digitalInputValueBit);
  LOG_TEST_RESULT(aprResult, L"Got an unexpected result", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nPrepare booting config share handler...");
  test::MockAxisProfileDeviceParam mockDeviceParam;
  auto sharingHandler = AxisProfileSharingHandler(&profileBasicInfo.m_profileTrace);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &mockDeviceParam);
  testClass.setHandler(&sharingHandler);

  log(L"\nCall AXISPROFILE_getLatchTimeOfActInput()...");
  double absSync0Time {123456789.123};
  double cycleTime {0.002};
  double expInputTime {absSync0Time - 2.0 * cycleTime}; // always delay 2 cycles
  testClass.setActSyncTime(absSync0Time, cycleTime);
  double resInTime = cyclicDataItf->getLatchTimeOfActInput(cyclicDataItf->handle);
  LOG_TEST_RESULT_FLT(resInTime, L"Get an unexpected getLatchTimeOfActInput", expInputTime, 1E-9);

  log(L"\nCall AXISPROFILE_getEffectiveTimeOfActOutput() when device not have output effective delay...");
  mockDeviceParam.m_returnValue = comm::axisprofile::AxisProfileResult::ERR_NOT_SUPPORTED;
  double expOutputTime {absSync0Time + cycleTime}; // fixed 1 cycle delay when no output effective delay
  testClass.setActSyncTime(absSync0Time, cycleTime);
  double resOutTime = cyclicDataItf->getEffectiveTimeOfActOutput(cyclicDataItf->handle);
  LOG_TEST_RESULT_FLT(resOutTime, L"Get an unexpected getEffectiveTimeOfActOutput", expOutputTime, 1E-9);

  log(L"\nCall AXISPROFILE_getEffectiveTimeOfActOutput() when device output effective delay...");
  double outputDelay {21.123};
  mockDeviceParam.m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;
  mockDeviceParam.m_outputEffectiveDelay = outputDelay;
  expOutputTime = absSync0Time + outputDelay + cycleTime;
  testClass.setActSyncTime(absSync0Time, cycleTime);
  resOutTime = cyclicDataItf->getEffectiveTimeOfActOutput(cyclicDataItf->handle);
  LOG_TEST_RESULT_FLT(resOutTime, L"Get an unexpected getEffectiveTimeOfActOutput", expOutputTime, 1E-9);

  log(L"\nCall AXISPROFILE_getLatchTimeOfActInput() with nullptr handle...");
  resInTime = cyclicDataItf->getLatchTimeOfActInput(nullptr);
  LOG_TEST_RESULT_FLT(resInTime, L"Get an unexpected getLatchTimeOfActInput", 0.0, 1E-9);

  log(L"\nCall AXISPROFILE_getEffectiveTimeOfActOutput() with nullptr handle...");
  resOutTime = cyclicDataItf->getEffectiveTimeOfActOutput(nullptr);
  LOG_TEST_RESULT_FLT(resOutTime, L"Get an unexpected getEffectiveTimeOfActOutput", 0.0, 1E-9);

  log(L"\nCall AXISPROFILE_AXISPROFILE_getSafetyOperationMode()...");
  comm::axisprofile::SafetyOperationMode mode;
  auto actualRes = cyclicDataItf->getSafetyOperationMode(cyclicDataItf->handle, mode);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);
  actualRes = cyclicDataItf->getSafetyOperationMode(nullptr, mode);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall AXISPROFILE_AXISPROFILE_getSingleUserPDODataFromDevice()...");
  comm::axisprofile::UserPDODataInfo userpdoinfo;
  actualRes = cyclicDataItf->getSingleUserPDODataFromDevice(cyclicDataItf->handle, 0, userpdoinfo);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);
  actualRes = cyclicDataItf->getSingleUserPDODataFromDevice(nullptr, 0, userpdoinfo);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall AXISPROFILE_AXISPROFILE_setSingleUserPDODataToDevice()...");
  actualRes = cyclicDataItf->setSingleUserPDODataToDevice(cyclicDataItf->handle, 0, userpdoinfo);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);
  actualRes = cyclicDataItf->setSingleUserPDODataToDevice(nullptr, 0, userpdoinfo);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall AXISPROFILE_AXISPROFILE_checkSingleUserPDOActiveFromDevice()...");
  bool expRes = false;
  bool res = cyclicDataItf->checkSingleUserPDOActiveFromDevice(cyclicDataItf->handle, 0);
  LOG_TEST_RESULT(res, L"Got an unexpected result", expRes);

  expRes = false;
  res = cyclicDataItf->checkSingleUserPDOActiveFromDevice(nullptr, 0);
  LOG_TEST_RESULT(res, L"Got an unexpected result", expRes);

  log(L"\nCall AXISPROFILE_AXISPROFILE_checkSingleUserPDOActiveToDevice()...");
  expRes = false;
  res = cyclicDataItf->checkSingleUserPDOActiveToDevice(cyclicDataItf->handle, 0);
  LOG_TEST_RESULT(res, L"Got an unexpected result", expRes);

  res = cyclicDataItf->checkSingleUserPDOActiveToDevice(nullptr, 0);
  expRes = false;
  LOG_TEST_RESULT(res, L"Got an unexpected result", expRes);

  log(L"\n");
  return hResult;
}

HRESULT TestProfileCyclicData::testCheckIfTelegramItemAssigned() const
{
  log(L"\n\n<b>Test all class member function</b>");

  HRESULT hResult = S_OK;
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  auto scaling = std::make_unique<sercos::ScalingStrategySoEDrive>(sercos::ScalingStrategySoEDrive {profileBasicInfo});
  IScaling* ptrScaling = scaling.get();
  test::MockAxisProfileTrace profileTraceMock {};

  auto pProfileCategory = std::make_unique<DriveCategoryAdapterSoE>(profileBasicInfo, &atData, &mdtData);
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
  AxisProfileCyclicDataUnittest testClass(profileBasicInfo, &atData, &mdtData);

  // setup sharing handler for cyclic data object
  AxisProfileMappingHandler handler(profileBasicInfo);
  auto sharingHandler = AxisProfileSharingHandler(&profileBasicInfo.m_profileTrace);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &handler);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, pProfileCategory.get());
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, ptrScaling);
  testClass.setHandler(&sharingHandler);

  log(L"\nTest checkIfTelegramItemAssigned");

  // test for position data unassigned
  AxisProfileValueTypeID type = AxisProfileValueTypeID::ACTUAL_POSITION;
  DataAccessErrorInfo errorInfo {};
  testClass.checkIfTelegramItemAssigned(type, errorInfo);
  DataAccessCommonErrorType ExpectedErrorType {DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR};
  std::string expectedAdditionInfo = "POSITION : " + std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType));
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  // test for position data unassigned
  type = AxisProfileValueTypeID::ACTUAL_VELOCITY;
  testClass.checkIfTelegramItemAssigned(type, errorInfo);
  ExpectedErrorType = DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR;
  expectedAdditionInfo = "VELOCITY : " + std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType));
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  // test for velocity data unassigned
  type = AxisProfileValueTypeID::ACTUAL_VELOCITY;
  testClass.checkIfTelegramItemAssigned(type, errorInfo);
  ExpectedErrorType = DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR;
  expectedAdditionInfo = "VELOCITY : " + std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType));
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  type = AxisProfileValueTypeID::ACTUAL_TORQUE;
  testClass.checkIfTelegramItemAssigned(type, errorInfo);
  ExpectedErrorType = DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR;
  expectedAdditionInfo = "TORQUE : " + std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType));
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  // test for other unprovided data unassigned
  type = AxisProfileValueTypeID::ACTUAL_ACCELERATION;
  testClass.checkIfTelegramItemAssigned(type, errorInfo);
  ExpectedErrorType = DataAccessCommonErrorType::ERROR_NOT_PROVIDED;
  expectedAdditionInfo = std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType));
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  return hResult;
}

HRESULT TestProfileCyclicData::testForcheckMasterStatus() const
{
  log(L"\n<b>Test checkMasterStatus method of the cyclic data.</b>");
  HRESULT hresult = S_OK;

  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  test::MockAxisProfileDeviceParam deviceParamMockup {};
  test::MockAxisProfileCommand commandIf {};

  ProfileTrace traceHelper;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  auto usageMode = std::make_unique<DriveCategoryAdapterSoE>(profileBasicInfo, &atData, &mdtData);
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_profileName = "testProfile";
  AxisProfileCyclicDataUnittest testMasterStatus(profileBasicInfo, &atData, &mdtData);

  DataAccessErrorInfo errorInfo {};
  errorInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_NONE;
  errorInfo.m_additionInfo = DataAccessCommonErrorTypeStrings.ERROR_NONE;

  // test for no ethercat instance
  log(L"\ntest for no ethercat instance");
  fieldbusAdapterMockUp.setFieldbusMasterAddress(DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES);
  bool expectRes {false};
  bool statusRes = testMasterStatus.checkFieldbusStatus(errorInfo);
  LOG_TEST_RESULT(statusRes, L"Got an unexpected result", expectRes);
  DataAccessCommonErrorType ExpectedErrorType {DataAccessCommonErrorType::ERROR_MASTER_STATUS};
  std::string expectedAdditionInfo = std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType)) + " : no master instance exist";
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  log(L"\ntest for read master status error");
  fieldbusAdapterMockUp.setFieldbusMasterAddress(DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + "/ethercatMaster");
  fieldbusAdapterMockUp.setReturnValue(AxisProfileResult::ERR_DATALAYER_READ_NODE);
  statusRes = testMasterStatus.checkFieldbusStatus(errorInfo);
  LOG_TEST_RESULT(statusRes, L"Got an unexpected result", expectRes);
  ExpectedErrorType = DataAccessCommonErrorType::ERROR_MASTER_STATUS;
  expectedAdditionInfo = std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType)) + " : failed to read master status";
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  // prepare for mock data
  test::FlatbuffersHelperCommon fbHelper;
  test::MockDatalayerClientEx MockDatalayerClientEx {};
  comm::datalayer::Variant masterResp {};

  log(L"\ntest for read master status not in op");
  fbHelper.m_masterStatus.m_status = 0;
  fbHelper.m_masterStatus.m_statusBits.NotInStateOp = 1;
  fbHelper.generateMasterStatusResponseFlatbuffer(&masterResp);
  MockDatalayerClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  MockDatalayerClientEx.m_callbackVariant = masterResp;
  fieldbusAdapterMockUp.setDatalayer(&MockDatalayerClientEx);
  fieldbusAdapterMockUp.setReturnValue(AxisProfileResult::STS_OK);
  statusRes = testMasterStatus.checkFieldbusStatus(errorInfo);
  LOG_TEST_RESULT(statusRes, L"Got an unexpected result", expectRes);
  ExpectedErrorType = DataAccessCommonErrorType::ERROR_MASTER_STATUS;
  expectedAdditionInfo = " master name: " + std::string("ethercatMaster ") + std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType)) +
    " master status: " + std::to_string(fbHelper.m_masterStatus.m_status);
  getMasterHighestErrorInfoString(fbHelper.m_masterStatus);
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  log(L"\ntest for read master status ok, but error status");
  fbHelper.m_masterStatus.m_status = 0;
  fbHelper.m_masterStatus.m_statusBits.NoEthernetLink = 1;
  fbHelper.generateMasterStatusResponseFlatbuffer(&masterResp);
  MockDatalayerClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  MockDatalayerClientEx.m_callbackVariant = masterResp;
  fieldbusAdapterMockUp.setDatalayer(&MockDatalayerClientEx);
  fieldbusAdapterMockUp.setReturnValue(AxisProfileResult::STS_OK);
  statusRes = testMasterStatus.checkFieldbusStatus(errorInfo);
  LOG_TEST_RESULT(statusRes, L"Got an unexpected result", expectRes);
  ExpectedErrorType = DataAccessCommonErrorType::ERROR_MASTER_STATUS;
  expectedAdditionInfo = " master name: " + std::string("ethercatMaster ") + std::string(dataAccessCommonErrorTypeToString(ExpectedErrorType)) +
    " master status: " + getMasterHighestErrorInfoString(fbHelper.m_masterStatus);
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  //ok
  fbHelper.m_masterStatus.m_status = 0;
  fbHelper.m_masterStatus.m_statusBits.NoEthernetLink = 0;
  fbHelper.m_masterStatus.m_statusBits.SettingsInconsistent = 0;
  fbHelper.m_masterStatus.m_statusBits.PortError = 0;
  fbHelper.m_masterStatus.m_statusBits.LicenseError = 0;

  fbHelper.generateMasterStatusResponseFlatbuffer(&masterResp);
  MockDatalayerClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  MockDatalayerClientEx.m_callbackVariant = masterResp;
  fieldbusAdapterMockUp.setDatalayer(&MockDatalayerClientEx);
  statusRes = testMasterStatus.checkFieldbusStatus(errorInfo);
  expectRes = true;
  LOG_TEST_RESULT(statusRes, L"Got an unexpected result", expectRes);

  MockDatalayerClientEx.m_vecCallBackVariant.clear();
  MockDatalayerClientEx.m_nCalledTimes = 0;
  return hresult;
}

HRESULT TestProfileCyclicData::testForcheckSlaveStatus() const
{
  log(L"\n<b>Test checkSlaveStatus method of the cyclic data.</b>");
  HRESULT hresult = S_OK;

  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  test::MockAxisProfileDeviceParam deviceParamMockup {};
  test::MockAxisProfileCommand commandIf {};

  ProfileTrace traceHelper;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  auto usageMode = std::make_unique<DriveCategoryAdapterSoE>(profileBasicInfo, &atData, &mdtData);
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_profileName = "testProfile";
  AxisProfileCyclicDataUnittest testSlaveStatus(profileBasicInfo, &atData, &mdtData);

  DataAccessErrorInfo errorInfo {};
  errorInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_NONE;
  errorInfo.m_additionInfo = DataAccessCommonErrorTypeStrings.ERROR_NONE;

  log(L"\ntest for fail to read");
  fieldbusAdapterMockUp.setFieldbusMasterAddress(DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + "/ethercatMaster");

  bool expectRes {false};
  fieldbusAdapterMockUp.setReturnValue(AxisProfileResult::ERR_DATALAYER_READ_NODE);
  bool actualRes = testSlaveStatus.checkForSlaveStatus(errorInfo);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", expectRes);
  DataAccessCommonErrorType expectedErrorType {DataAccessCommonErrorType::ERROR_MASTER_STATUS};
  std::string expectedAdditionInfo = std::string(dataAccessCommonErrorTypeToString(expectedErrorType)) + " : read slave status failed";
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  test::MockDatalayerClientEx MockDatalayerClientEx {};
  test::FlatbuffersHelperCommon fbHelper;
  comm::datalayer::Variant slaveVar {};

  log(L"\ntest for error slave status");
  expectRes = false;
  expectedErrorType = DataAccessCommonErrorType::ERROR_SLAVE_STATUS;
  fbHelper.m_slaveStatus.m_statusBits.NotOnline = 1;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&slaveVar);
  MockDatalayerClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  MockDatalayerClientEx.m_callbackVariant = slaveVar;
  fieldbusAdapterMockUp.setDatalayer(&MockDatalayerClientEx);
  fieldbusAdapterMockUp.setReturnValue(AxisProfileResult::STS_OK);
  actualRes = testSlaveStatus.checkForSlaveStatus(errorInfo);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", expectRes);
  expectedAdditionInfo = std::string(dataAccessCommonErrorTypeToString(expectedErrorType)) + " : slave not online";
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);

  log(L"\ntest for not in state op");
  fieldbusAdapterMockUp.setReturnValue(AxisProfileResult::ERR_DATALAYER_READ_NODE);
  fbHelper.m_slaveStatus.m_status = 0;
  fbHelper.m_slaveStatus.m_statusBits.NotInStateOp = 1;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&slaveVar);
  MockDatalayerClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  MockDatalayerClientEx.m_callbackVariant = slaveVar;
  fieldbusAdapterMockUp.setDatalayer(&MockDatalayerClientEx);
  fieldbusAdapterMockUp.setReturnValue(AxisProfileResult::STS_OK);
  actualRes = testSlaveStatus.checkForSlaveStatus(errorInfo);
  LOG_TEST_RESULT(actualRes, L"Got an unexpected result", expectRes);
  expectedAdditionInfo = std::string(dataAccessCommonErrorTypeToString(expectedErrorType)) + " : not in op state";
  LOG_TEST_RESULT_STRING(errorInfo.m_additionInfo, L"Got an unexpected result", expectedAdditionInfo);
  getSlaveHighestErrorInfoString(fbHelper.m_slaveStatus);

  return hresult;
}

HRESULT TestProfileCyclicData::testForCheckActualValueAvailability() const
{
  HRESULT hresult = S_OK;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  test::MockAxisProfileDeviceParam deviceParamMockup {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};

  ProfileTrace traceHelper;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  auto scaling = std::make_unique<sercos::ScalingStrategySoEDrive>(sercos::ScalingStrategySoEDrive {profileBasicInfo});
  IScaling* ptrScaling = scaling.get();
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  auto usageMode = std::make_unique<DriveCategoryAdapterSoE>(profileBasicInfo, &atData, &mdtData);
  ICategoryAdapter* profileCategory = usageMode.get();
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;
  profileBasicInfo.m_profileName = "testProfile";
  AxisProfileCyclicDataUnittest cyclicData(profileBasicInfo, &atData, &mdtData);

  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &cyclicData);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, profileCategory);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, ptrScaling);

  // test for check assigned items
  DataAccessErrorInfo errorInfo {};
  errorInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_NONE;
  auto& assgined = mappingHandlerMockup.getProfileAssignedInfo();
  assgined.m_input.m_position = true;

  //test for master status
  test::FlatbuffersHelperCommon fbHelper;
  test::MockDatalayerClientEx MockDatalayerClientEx {};
  comm::datalayer::Variant masterResp {};

  fbHelper.m_masterStatus.m_status = 0;

  fbHelper.generateMasterStatusResponseFlatbuffer(&masterResp);
  MockDatalayerClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  MockDatalayerClientEx.m_vecCallBackVariant.push_back(masterResp);
  fieldbusAdapterMockUp.setDatalayer(&MockDatalayerClientEx);
  fieldbusAdapterMockUp.setFieldbusMasterAddress(DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + "/ethercatMaster");
  fieldbusAdapterMockUp.setReturnValue(AxisProfileResult::STS_OK);

  //test for slave status
  comm::datalayer::Variant slaveVar {};
  fbHelper.m_slaveStatus.m_status = 0;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&slaveVar);
  MockDatalayerClientEx.m_returnValue = comm::datalayer::DlResult::DL_OK;
  MockDatalayerClientEx.m_vecCallBackVariant.push_back(slaveVar);

  //check for limitation
  DeviceCfgData testData {};
  deviceParamMockup.setReturnCfgData(testData);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  bool res = cyclicData.checkActualValueAvailabilityNRT(AxisProfileValueTypeID::ACTUAL_POSITION, errorInfo);
  LOG_TEST_RESULT(res, L"Got an unexpected result", true);

  return hresult;
}
