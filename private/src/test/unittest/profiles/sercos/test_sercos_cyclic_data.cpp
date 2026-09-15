#include "test_sercos_cyclic_data.h"

#include "profiles/sercos/drive/sercos_cyclic_data_drive.h"
#include "profiles/profile_trace.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/drive/sercos_scaling_strategy_drive.h"

#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"

#include "helper/result_helper.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestSercosCyclicData);
REGISTER_TEST_TO_GROUP(TestSercosCyclicData, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosCyclicData, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosCyclicData, Profiles_Sercos);


HRESULT TestSercosCyclicData::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the DriveCategoryAdapterSoE");
  log(L"\n----------------------------------------------------------------\n");

  res |= testInterface();
  res |= testClass();
  res |= testForFPUError();

  return res;
}

HRESULT TestSercosCyclicData::testInterface()
{
  m_cmdMock = new test::MockAxisProfileCommand();
  m_statusControlMock = new test::MockAxisProfileStatusControl();
  m_diagMsgMock = new test::MockAxisProfileDiagMsg();
  m_deviceParamMock = new test::MockAxisProfileDeviceParam();
  m_probeMock = new test::MockAxisProfileProbe();

  HRESULT hResult = S_OK;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  sercos::SercosCyclicDataDrive testCyclicDataDrive(profileBasicInfo, &atData, &mdtData);
  log(L"\n<b>Test of the DriveCategoryAdapterSoE Interface</b>");

  delete m_cmdMock;
  m_cmdMock = nullptr;
  delete m_statusControlMock;
  m_statusControlMock = nullptr;
  delete m_diagMsgMock;
  m_diagMsgMock = nullptr;
  delete m_deviceParamMock;
  m_deviceParamMock = nullptr;
  delete m_probeMock;
  m_probeMock = nullptr;

  log(L"\n");
  return hResult;
}

HRESULT TestSercosCyclicData::testClass()
{
  HRESULT result = S_OK;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  sercos::SercosCyclicDataDrive testCyclicDataDrive(profileBasicInfo, &atData, &mdtData);
  DataAccessErrorInfo dataAccessErrorInfo;
  dataAccessErrorInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_NONE;
  dataAccessErrorInfo.m_additionInfo = "";

  log(L"\nTest setCommandValuesDevice nullptr...");
  CommandValuesDevice commandValuesDevice;
  commandValuesDevice.m_commandPosition = 10;
  commandValuesDevice.m_commandTorqueForce = 10;
  commandValuesDevice.m_commandVelocity = 10;
  commandValuesDevice.m_torqueVelLimitHigh = 10;
  commandValuesDevice.m_torqueVelLimitLow = 10;
  AxisProfileResult aprResult = testCyclicDataDrive.booting();
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  bool inputValue;
  log(L"\nTest getDigitalInputValueBit nullptr...");
  aprResult = testCyclicDataDrive.getDigitalInputValueBit(DigitalInputValueBitType::UNDEFINED, inputValue);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest getActualValuesDevice nullptr...");
  ActualValuesDevice actualValuesDevice;
  actualValuesDevice.m_actualPosition = 10.0;
  actualValuesDevice.m_actualTorqueForce = 10.0;
  actualValuesDevice.m_actualVelocity = 10.0;
  actualValuesDevice.m_dataValid = false;

  log(L"\nTest checkActualValueAvailabilityNRT nullptr...");
  bool checkResult = testCyclicDataDrive.checkActualValueAvailabilityNRT(AxisProfileValueTypeID::ACTUAL_POSITION, dataAccessErrorInfo);
  LOG_TEST_RESULT(checkResult, L"\nGet an unexpected result!", false);

  test::MockAxisProfileDeviceParam deviceParamMockup {};
  test::MockAxisProfileCommand commandMockup {};
  ProfileTrace traceHelper;
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockAxisProfileProperty peroperty {};

  std::uint32_t devAddress = 1001;
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_profileName = "testProfile";

  log(L"\nTest getDigitalInputValueBit...");
  aprResult = testCyclicDataDrive.getDigitalInputValueBit(DigitalInputValueBitType::UNDEFINED, inputValue);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest checkActualValueAvailabilityNRT...");
  auto scaling = std::make_unique<sercos::ScalingStrategySoEDrive>(profileBasicInfo);
  IScaling* ptrScaling = scaling.get();
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &testCyclicDataDrive);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &commandMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, ptrScaling);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &peroperty);

  log(L"\nTest setCommandValuesDevice...");
  commandMockup.m_commandState = AxisprofileCommandState::CMD_STATE_ACTIVE;
  testCyclicDataDrive.booting();
  aprResult = testCyclicDataDrive.setCommandValuesDevice(commandValuesDevice);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  commandMockup.m_commandState = AxisprofileCommandState::CMD_STATE_IDLE;
  aprResult = testCyclicDataDrive.setCommandValuesDevice(commandValuesDevice);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  DataAccessErrorInfo errorInfo {};
  errorInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_NONE;
  auto& assgined = mappingHandlerMockup.getProfileAssignedInfo();
  assgined.m_input.m_position = true;
  assgined.m_input.m_velocity = true;

  log(L"\nTest checkActualValueAvailabilityNRT with limitation...");
  DeviceCfgData testData {};
  testData.m_softwareLimitActive = true;
  deviceParamMockup.setReturnCfgData(testData);
  fieldbusAdapterMockUp.m_returnValue = AxisProfileResult::STS_OK;
  fieldbusAdapterMockUp.setFieldbusMasterAddress(DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + "instance");
  checkResult = testCyclicDataDrive.checkIfMatchLimitation(AxisProfileValueTypeID::ACTUAL_POSITION, dataAccessErrorInfo);
  LOG_TEST_RESULT(checkResult, L"\nGet an expected result!", true);

  assgined.m_input.m_velocity = true;
  checkResult = testCyclicDataDrive.checkIfMatchLimitation(AxisProfileValueTypeID::ACTUAL_VELOCITY, dataAccessErrorInfo);
  LOG_TEST_RESULT(checkResult, L"\nGet an expected result!", true);

  log(L"\nTest checkActualValueAvailabilityNRT with position beyond limitation...");
  testData.m_positivePosLimit = -10;
  deviceParamMockup.setReturnCfgData(testData);
  checkResult = testCyclicDataDrive.checkIfMatchLimitation(AxisProfileValueTypeID::ACTUAL_POSITION, dataAccessErrorInfo);
  DataAccessCommonErrorType expectErrorType = DataAccessCommonErrorType::ERROR_BEYOND_THE_LIMIT;
  std::string expectError = "POSITION : " + std::string(dataAccessCommonErrorTypeToString(expectErrorType));
  LOG_TEST_RESULT(checkResult, L"\nGet an expected result!", false);
  LOG_TEST_RESULT_STRING(dataAccessErrorInfo.m_additionInfo, L"\nGet an expected result!", expectError);

  log(L"\nTest checkActualValueAvailabilityNRT with velocity beyond limitation...");
  testData.m_positiveVelLimit = -10;
  deviceParamMockup.setReturnCfgData(testData);
  checkResult = testCyclicDataDrive.checkIfMatchLimitation(AxisProfileValueTypeID::ACTUAL_VELOCITY, dataAccessErrorInfo);
  expectError = "VELOCITY : " + std::string(dataAccessCommonErrorTypeToString(expectErrorType));
  LOG_TEST_RESULT(checkResult, L"\nGet an expected result!", false);
  LOG_TEST_RESULT_STRING(dataAccessErrorInfo.m_additionInfo, L"\nGet an expected result!", expectError);

  log(L"\nTest checkActualValueAvailabilityNRT with torque ...");
  assgined.m_input.m_torqueForce = true;
  checkResult = testCyclicDataDrive.checkIfMatchLimitation(AxisProfileValueTypeID::ACTUAL_TORQUE, dataAccessErrorInfo);
  LOG_TEST_RESULT(checkResult, L"\nGet an expected result!", true);

  log(L"\nTest checkActualValueAvailabilityNRT with undefined variable");
  checkResult = testCyclicDataDrive.checkActualValueAvailabilityNRT(AxisProfileValueTypeID::LAST_ID, dataAccessErrorInfo);
  expectError = std::string(dataAccessCommonErrorTypeToString(DataAccessCommonErrorType::ERROR_NOT_PROVIDED));
  LOG_TEST_RESULT(checkResult, L"\nGet an expected result!", false);
  LOG_TEST_RESULT_STRING(dataAccessErrorInfo.m_additionInfo, L"\nGet an expected result!", expectError);

  log(L"\nTest getDigitalInputValueBit...");
  auto& assignedInfo = mappingHandlerMockup.getProfileAssignedInfo();
  assignedInfo.m_input.m_switchRef = true;
  bool input {true};
  AxisProfileResult digitRes = testCyclicDataDrive.getDigitalInputValueBit(DigitalInputValueBitType::REFERENCE_SWITCH_BIT, input);
  LOG_TEST_RESULT(digitRes, L"\nGet an expected result!", comm::axisprofile::AxisProfileResult::STS_OK);

  assignedInfo.m_input.m_switchRef = false;
  assignedInfo.m_input.m_switchNegLim = true;
  digitRes = testCyclicDataDrive.getDigitalInputValueBit(DigitalInputValueBitType::NEGATIVE_LIMIT_SWITCH_BIT, input);
  LOG_TEST_RESULT(digitRes, L"\nGet an expected result!", comm::axisprofile::AxisProfileResult::STS_OK);

  assignedInfo.m_input.m_switchNegLim = false;
  assignedInfo.m_input.m_switchPosLim = true;
  digitRes = testCyclicDataDrive.getDigitalInputValueBit(DigitalInputValueBitType::POSITIVE_LIMIT_SWITCH_BIT, input);
  LOG_TEST_RESULT(digitRes, L"\nGet an expected result!", comm::axisprofile::AxisProfileResult::STS_OK);

  assignedInfo.m_input.m_switchPosLim = false;
  digitRes = testCyclicDataDrive.getDigitalInputValueBit(DigitalInputValueBitType::UNDEFINED, input);
  LOG_TEST_RESULT(digitRes, L"\nGet an expected result!", comm::axisprofile::AxisProfileResult::ERR_NOK);

  log(L"\nTest getSafetyOperationMode...");
  StateMachineState stateMachine = StateMachineState::RUNNING;
  testCyclicDataDrive.setStateMachineStatusIF(&stateMachine);
  testGettingSafetyOperationMode(&testCyclicDataDrive, &atData, mappingHandlerMockup);

  log(L"\nTest getSingleUserPDODataFromDevice...");
  UserPDODataInfo destData {};
  aprResult = testCyclicDataDrive.getSingleUserPDODataFromDevice(6, destData);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_INVALID_ARGUMENT);
  aprResult = testCyclicDataDrive.getSingleUserPDODataFromDevice(0, destData);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_ERR_DATA_NOT_MAPPED);

  assignedInfo.m_input.m_userPDOs.m_userPDO[0] = true;
  aprResult = testCyclicDataDrive.getSingleUserPDODataFromDevice(0, destData);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);
  testCyclicDataDrive.setSharingHandler(nullptr);
  aprResult = testCyclicDataDrive.getSingleUserPDODataFromDevice(0, destData);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest setSingleUserPDODataToDevice...");
  testCyclicDataDrive.setSharingHandler(ptrSharing);
  aprResult = testCyclicDataDrive.setSingleUserPDODataToDevice(6, destData);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_INVALID_ARGUMENT);
  aprResult = testCyclicDataDrive.setSingleUserPDODataToDevice(0, destData);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_ERR_DATA_NOT_MAPPED);

  assignedInfo.m_output.m_userPDOs.m_userPDO[0] = true;
  aprResult = testCyclicDataDrive.setSingleUserPDODataToDevice(0, destData);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);
  testCyclicDataDrive.setSharingHandler(nullptr);
  aprResult = testCyclicDataDrive.setSingleUserPDODataToDevice(0, destData);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest checkSingleUserPDOActiveFromDevice...");
  testCyclicDataDrive.setSharingHandler(ptrSharing);
  testCyclicDataDrive.checkSingleUserPDOActiveFromDevice(0);
  testCyclicDataDrive.checkSingleUserPDOActiveFromDevice(6);
  testCyclicDataDrive.setSharingHandler(nullptr);
  testCyclicDataDrive.checkSingleUserPDOActiveFromDevice(0);

  log(L"\nTest checkSingleUserPDOActiveToDevice...");
  testCyclicDataDrive.setSharingHandler(ptrSharing);
  testCyclicDataDrive.checkSingleUserPDOActiveToDevice(0);
  testCyclicDataDrive.checkSingleUserPDOActiveToDevice(6);
  testCyclicDataDrive.setSharingHandler(nullptr);
  testCyclicDataDrive.checkSingleUserPDOActiveToDevice(0);

  log(L"\nTest getActualValuesDevice...");
  testCyclicDataDrive.setSharingHandler(ptrSharing);
  testCyclicDataDrive.booting();
  aprResult = testCyclicDataDrive.getActualValuesDevice(&actualValuesDevice);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  log(L"\n\t Test of setNewRefPosition...");
  std::int32_t iNewRefPos {};
  testCyclicDataDrive.setNewRefPosition(iNewRefPos);

  log(L"\n\t Test of activeNewRefOperation...");
  bool active = true;
  testCyclicDataDrive.activeNewRefOperation(active);

  log(L"\n");
  return result;
}

HRESULT TestSercosCyclicData::testForFPUError()
{
  HRESULT hResult = S_OK;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  sercos::SercosCyclicDataDrive testCyclicDataDrive(profileBasicInfo, &atData, &mdtData);
  ActualValuesDevice actValue {};

  // just for nullptr coverage
  testCyclicDataDrive.setSharingHandler(nullptr);
  bool input {true};
  testCyclicDataDrive.getDigitalInputValueBit(DigitalInputValueBitType::UNDEFINED, input);
  SafetyOperationMode mode = SafetyOperationMode::SOM_SAFE_HOLD;
  testCyclicDataDrive.getSafetyOperationMode(mode);

  test::MockAxisProfileDeviceParam deviceParamMockup {};
  test::MockAxisProfileCommand commandMockup {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockAxisProfileProperty peroperty {};

  auto scaling = std::make_unique<sercos::ScalingStrategySoEDrive>(sercos::ScalingStrategySoEDrive {profileBasicInfo});
  IScaling* ptrScaling = scaling.get();
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &testCyclicDataDrive);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &commandMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, ptrScaling);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &peroperty);

  StateMachineState stateMachine = StateMachineState::RUNNING;
  testCyclicDataDrive.setStateMachineStatusIF(&stateMachine);
  testCyclicDataDrive.booting();
  auto aprResult = testCyclicDataDrive.getActualValuesDevice(&actValue);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  peroperty.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  peroperty.setSoftwareVersion(06, 06);
  testCyclicDataDrive.booting();
  aprResult = testCyclicDataDrive.getActualValuesDevice(&actValue);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  log(L"\n");
  return hResult;
}

HRESULT TestSercosCyclicData::testGettingSafetyOperationMode(
  AxisProfileCyclicData* testCyclicDataDrive,
  AcknowledgeTelegram* ptrAtData,
  test::MockAxisProfileMappingHandler& mappingHandler)
{
  HRESULT res = S_OK;
  log(L"\n<b>do test of getting safety operation mode...</b>");

  // Make cyclic data indicating safety function selection was selected or not selected.
  std::vector<std::pair<SafeMotionDataPackage, SafetyOperationMode> > cases
    = {
    // Basic cases.
      {{MASK1(32, 25, 0), 0x0}, SafetyOperationMode::SOM_NORMAL_MODE},
      {{MASK1(32, 25, 0), MASK1(32, 1, 4)}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},
      {{MASK1(32, 25, 0), MASK1(32, 4, 14)}, SafetyOperationMode::SOM_SAFE_HOLD},
      {{MASK1(32, 25, 0) & (~MASK1(32, 1, 2)), MASK1(32, 1, 2)}, SafetyOperationMode::SOM_SAFE_HOLD},
      {{MASK1(32, 25, 0) & (~MASK1(32, 1, 3)), MASK1(32, 1, 3)}, SafetyOperationMode::SOM_SAFE_HOLD},
      {{MASK1(32, 25, 0) & (~MASK1(32, 1, 4)), 0x0}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},
      {{MASK1(32, 25, 0) & (~MASK1(32, 4, 14)), 0x0}, SafetyOperationMode::SOM_SAFE_HOLD},
      // These test cases is from [Bug #834105](https://rb-msvs-alm.de.bosch.com/tfs/Col-DCIATE3_EDC/PD_EDC/_workitems/edit/861644)
      {{0x877BFFFF, 0x00800004}, SafetyOperationMode::SOM_SAFE_HOLD},
      {{0x877FFFFB, 0x04802010}, SafetyOperationMode::SOM_SAFE_HOLD},
      {{0x837FFFFF, 0x00800000}, SafetyOperationMode::SOM_NORMAL_MODE},         // from line#4 (SLS.trace file)
      {{0x837FFFFF, 0x00800000}, SafetyOperationMode::SOM_NORMAL_MODE},         // from line#11
      {{0x877BFFFF, 0x00840000}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},  // from line #12
      {{0x877BFFFF, 0x00840000}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},  // to line #495
      {{0x877FDFEF, 0x04802010}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},  // from line#496
      {{0x877FDFEF, 0x04802010}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},  // to line#505
      {{0x877FFFFB, 0x00800000}, SafetyOperationMode::SOM_SAFE_HOLD},           // from line#506
      {{0x877FFFFB, 0x00800000}, SafetyOperationMode::SOM_SAFE_HOLD},           // to line#509
      {{0x877FFFFB, 0x00800004}, SafetyOperationMode::SOM_SAFE_HOLD},           // from line#510
      {{0x877FFFFB, 0x00800004}, SafetyOperationMode::SOM_SAFE_HOLD},           // to line#1004

      {{0x837FFFFF, 0x00800000}, SafetyOperationMode::SOM_NORMAL_MODE},            // from line#2  (SLS.scope file)
      {{0x837FFFFF, 0x00800000}, SafetyOperationMode::SOM_NORMAL_MODE},            // from line#1828
      {{0x877BFFFF, 0x00840000}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},     // from line#1829
      {{0x877BFFFF, 0x00840000}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},     // from line#2329
      {{0x877FFFEF, 0x04800010}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},     // from line#2330
      {{0x877FFFEF, 0x04800010}, SafetyOperationMode::SOM_SAFE_LIMITED_SPEED},     // from line#3876
  };

  auto& assignedInfo = mappingHandler.getProfileAssignedInfo();
  assignedInfo.m_input.m_safetySelection = true;
  assignedInfo.m_input.m_safetyStatus = true;

  for (auto c : cases)
  {
    ptrAtData->m_smoData.m_safetySelection = c.first.m_safetySelection;
    ptrAtData->m_smoData.m_safetyStatus = c.first.m_safetyStatus;
    comm::axisprofile::SafetyOperationMode mode;
    auto aprResult = testCyclicDataDrive->getSafetyOperationMode(mode);
    log(L"\nCyclic data:[P-0-3264:%lu (0x%08X), P-0-3264.0.1:%lu (0x%08X)]",
        c.first.m_safetySelection, c.first.m_safetySelection, c.first.m_safetyStatus, c.first.m_safetyStatus);
    LOG_TEST_RESULT_ANY(aprResult == AxisProfileResult::STS_OK && mode == c.second, L"\nGet an unexpected result!");
  }

  return res;
}
