#include "test_canopen_device_param.h"
#include "helper/result_helper.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/canopen/canopen_boot_params.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/profile_trace.h"
#include "profiles/canopen/drive/canopen_device_param_drive.h"

//Mockups
#include "helper/flatbuffers_helper_coe.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/external_bundles/mock_datalayer_client_ex.h"
#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_job_mgr.h"  

using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenDeviceParam);
REGISTER_TEST_TO_GROUP(TestCanopenDeviceParam, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenDeviceParam, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenDeviceParam, Profiles_Canopen);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestCanopenDeviceParam::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Canopen profile DeviceParam class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testChecksum();
  hresult |= testCheckNeedReadBootParams();
  hresult |= testFirmwareInfo();
  hresult |= testOperationModeInfo();
  hresult |= testDetermineOpModeAvailable();
  hresult |= testNotAvailableMethods();
  hresult |= testCheckFunctionSupported();
  hresult |= testCheckStandstill();
  hresult |= testLoadBootingTasks();
  hresult |= testGetDeviceParamJobObj();
  hresult |= testCheckAxisHomed();
  hresult |= testConstructDeviceParamCoE();
  hresult |= testReadEncoderTypes();
  
  return hresult;
}

HRESULT TestCanopenDeviceParam::testFirmwareInfo()
{
  log(L"\n<b>Test firmware information related functions.</b>");
  HRESULT hresult = S_OK;
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  std::uint32_t deviceAddress = 1;
  std::uint8_t driveNumber = 0;

  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockEthercatMasterSDORequestNodeCoE sdoRequestNodeMockCoE {};
  mockClient.m_testNode = &sdoRequestNodeMockCoE;

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, &atData, &mdtData};

  test::MockAxisProfileProperty property {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  testClass.setSharingHandler(&shareHandler);

  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  log(L"\n<b>Test ctrlX firmware information related functions.</b>");
  testClass.checkDeviceQuickStopCfg();

  log(L"\n<b>Test Indra drive firmware information related functions.</b>");
  property.setDeviceSeries(DeviceSeries::INDRADRIVE);
  testClass.checkDeviceQuickStopCfg();

  log(L"\n<b>Test KNOWN_GENERAL_3RD_DRIVE firmware information related functions.</b>");
  property.setDeviceSeries(DeviceSeries::KNOWN_GENERAL_3RD_DRIVE);
  testClass.checkDeviceQuickStopCfg();
  return hresult;
}

HRESULT TestCanopenDeviceParam::testOperationModeInfo()
{
  log(L"\n<b>Test read operation mode information.</b>");
  HRESULT hresult = S_OK;
  test::MockFieldbusAdapterCoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  std::uint32_t deviceAddress = 1;
  std::uint8_t driveNumber = 0;

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  testClass.getOpModeAvailInfo();

  return hresult;
}

HRESULT TestCanopenDeviceParam::testDetermineOpModeAvailable()
{
  log(L"\n<b>Test determination of the supported operation modes.</b>");
  HRESULT hresult = S_OK;
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClientEx mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  std::uint32_t deviceAddress = 1;
  std::uint8_t driveNumber = 0;
  test::MockScaling mockScaling;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};

  comm::axisprofile::ProfileAssignedInfo profileAssigned {false};
  mappingHandlerMockup.setReturnValue(profileAssigned);

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  AcknowledgeTelegram atData;
  MasterDataTelegram mdtData;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, &atData, &mdtData};

  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);

  auto& assignedInfo = mappingHandlerMockup.getProfileAssignedInfo();
  assignedInfo.m_output.m_opMode = true;

  log(L"\n  Test the failure reading of the supported operation mode...");
  mockFieldbusAdapter.setReturnValue(AxisProfileResult::ERR_DATALAYER);
  AxisProfileResult result = testClass.determineOpModeAvailable();
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::ERR_DATALAYER);

  log(L"\n  Test the reading of the supported operation mode...");
  mockFieldbusAdapter.setReturnValue(AxisProfileResult::STS_OK);
  mockClient.resetClient();
  comm::datalayer::Variant var;
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&var, canopen::SUPPORTED_DRIVE_MODE.index, canopen::SUPPORTED_DRIVE_MODE.subIndex);
  mockClient.m_vecCallBackVariant.push_back(var);
  mockClient.m_vecReturnValue.push_back(comm::datalayer::DlResult::DL_OK);
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&var, canopen::TORQUE_FORCE_OPTIONS_CODE.index, canopen::TORQUE_FORCE_OPTIONS_CODE.subIndex);
  mockClient.m_vecCallBackVariant.push_back(var);
  mockClient.m_vecReturnValue.push_back(comm::datalayer::DlResult::DL_COMMUNICATION_ERROR);
  result = testClass.determineOpModeAvailable();
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);

  return hresult;
}

HRESULT TestCanopenDeviceParam::testNotAvailableMethods()
{
  log(L"\n<b>Test not available methods in CoE.</b>");
  ProfileBasicInfo profileBasicInfo;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  log(L"\n  Test the read delay brake parameters...");
  auto result = testClass.readDelayBrakeParam();
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);
  if (result != AxisProfileResult::STS_OK)
  {
    error(L"Not able to set operation mode positioning! Return code: 0x%08X", result);
    return E_FAIL;
  }

  log(L"\n  Test the get delay brake information...");
  testClass.getDelayBrakeInfo();
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);
  if (result != AxisProfileResult::STS_OK)
  {
    error(L"Not able to set operation mode positioning! Return code: 0x%08X", result);
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);
    return E_FAIL;
  }

  log(L"\n  Test the get delay dynamic limits...");
  testClass.getBrakeDynLimit();
  RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);
  if (result != AxisProfileResult::STS_OK)
  {
    error(L"Not able to set operation mode positioning! Return code: 0x%08X", result);
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::STS_OK);
    return E_FAIL;
  }

  return S_OK;
}

HRESULT TestCanopenDeviceParam::testCheckFunctionSupported()
{
  log(L"\n<b>Test the checking of the supported functionality.</b>");
  HRESULT hresult = S_OK;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  ProfileAssignedInfo profileAssigned {false};
  mappingHandlerMockup.setReturnValue(profileAssigned);
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileProperty property {};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, nullptr);

  auto& assignedInfo = mappingHandlerMockup.getProfileAssignedInfo();
  assignedInfo.m_output.m_opMode = false;
  assignedInfo.m_input.m_opModeDisplay = false;
  testClass.prepareSignalFuncHandler();
  log(L"\n  Test the failure checking...");
  FunctionSupportErrorInfo errorInfo;
  bool result = testClass.checkFunctionSupported(static_cast<FunctionSupport>(0xFF), errorInfo);
  RETURN_TEST_RESULT_ANY(result == false, L"\n  unexpected return value.");

  assignedInfo.m_output.m_opMode = true;
  assignedInfo.m_input.m_opModeDisplay = true;
  log(L"\n  Test the success checking...");
  result = testClass.checkFunctionSupported(FunctionSupport::OPMODE_CYCLIC, errorInfo);
  RETURN_TEST_RESULT_ANY(result == true, L"\n  unexpected return value.");

  assignedInfo.m_output.m_opMode = true;
  assignedInfo.m_input.m_opModeDisplay = false;
  log(L"\n  Test the success checking...");
  result = testClass.checkFunctionSupported(FunctionSupport::OPMODE_CYCLIC, errorInfo);
  RETURN_TEST_RESULT_ANY(result == false, L"\n  unexpected return value.");

  log(L"\n  Test the failure checking of HOMING_STATUS_CYCLIC...");
  result = testClass.checkFunctionSupported(FunctionSupport::HOMING_STATUS_CYCLIC, errorInfo);
  LOG_TEST_RESULT_ANY(!result && errorInfo.errorType == FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, L"\n  unexpected return value.");

  log(L"\n  Test the failure checking of ERROR_CODE_CYCLIC...");
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  result = testClass.checkFunctionSupported(FunctionSupport::ERROR_CODE_CYCLIC, errorInfo);
  LOG_TEST_RESULT_ANY(!result && errorInfo.errorType == FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, L"\n  unexpected return value.");

  log(L"\n  Test the failure checking of invalid function enumeration...");
  result = testClass.checkFunctionSupported(FunctionSupport::MAX_VALUE, errorInfo);
  LOG_TEST_RESULT_ANY(!result && errorInfo.errorType == FuncSupportErrorType::ERROR_NOT_DEFINED, L"\n  unexpected return value.");
  return hresult;
}

HRESULT TestCanopenDeviceParam::testChecksum()
{
  log(L"\n<b>Test checksum related functions.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = 1001;
  profileBasicInfo.m_driveNumber = 0;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  log(L"\nGetChecksum test...");
  comm::datalayer::Variant var;
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&var, canopen::CHECKSUM_VALUE.index, canopen::CHECKSUM_VALUE.subIndex);
  mockClient.m_callbackVariant = var;
  std::uint32_t checksum {};
  test::CoEValue coeValue;
  mockFieldbusAdapter.m_fbsHelper.readParam(canopen::CHECKSUM_VALUE.index, canopen::CHECKSUM_VALUE.subIndex, coeValue);
  AxisProfileResult res = testClass.readChecksumValueFromDevice(checksum);
  RETURN_TEST_RESULT(res, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_ANY(std::get<std::uint32_t>(coeValue) == checksum, L"\n  unexpected return value.");

  log(L"\nReadChecksumIdnList test, read data error...");
  std::vector<ObjectDirEntry> idnList {};
  comm::datalayer::Variant idnListVar;
  mockClient.m_callbackVariant = idnListVar;
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  res = testClass.readChecksumIdnListFromDevice(idnList);
  RETURN_TEST_RESULT(res, L"\nUnexpected return value.", AxisProfileResult::ERR_FIELDBUS);
  RETURN_TEST_RESULT_ANY(testClass.checkBootIdnListModified(), L"\nUnexpected return value.");

  log(L"\nReadChecksumIdnList test, normal case...");
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&idnListVar, canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex);
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockClient.m_callbackVariant = idnListVar;
  res = testClass.readChecksumIdnListFromDevice(idnList);
  mockFieldbusAdapter.m_fbsHelper.readParam(canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex, coeValue);
  bool equal = (std::get<std::vector<ObjectDirEntry>>(coeValue).size() == idnList.size()) &&
    std::equal(idnList.begin(), idnList.end(), std::get<std::vector<ObjectDirEntry>>(coeValue).begin(), [] (const ObjectDirEntry& a, const ObjectDirEntry& b) {
                 return a.index == b.index && a.subIndex == b.subIndex;
               });
  RETURN_TEST_RESULT_ANY(equal, L"\nUnexpected return value.");
  RETURN_TEST_RESULT(res, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_ANY(testClass.checkBootIdnListModified(), L"\nUnexpected return value.");
  std::vector<std::string> idnStrList;
  RETURN_TEST_RESULT(testClass.getChecksumIdnList(idnStrList), L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nReadChecksumIdnList test, normal case, return default idn list...");
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockFieldbusAdapter.m_fbsHelper.writeParam(canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex, canopen::COE_BOOT_PARAM_LIST);
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&idnListVar, canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex);
  mockClient.m_callbackVariant = idnListVar;
  idnList.clear();
  res = testClass.readChecksumIdnListFromDevice(idnList);
  RETURN_TEST_RESULT(res, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_ANY(idnList.size() == canopen::COE_BOOT_PARAM_LIST.size(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT_ANY(!testClass.checkBootIdnListModified(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT(testClass.getChecksumIdnList(idnStrList), L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nReadChecksumIdnList test, empty list...");
  mockFieldbusAdapter.m_fbsHelper.writeParam(canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex, std::vector<ObjectDirEntry>{}); // empty IDN list
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&idnListVar, canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex);
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockClient.m_callbackVariant = idnListVar;
  res = testClass.readChecksumIdnListFromDevice(idnList);
  RETURN_TEST_RESULT(res, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_ANY(idnList.empty(), L"\nUnexpected return value.");

  log(L"\nSetChecksumIDNList test, set a empty list...");
  res = testClass.writeChecksumIdnListToDevice(idnList);//request data is empty
  RETURN_TEST_RESULT(res, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nSetChecksumIDNList test, set a list with a normal case...");
  idnList = {{0x6091, 2}, {0x6091, 1}, {0x2390, 1}, {0x24A8, 1}};
  res = testClass.writeChecksumIdnListToDevice(idnList);
  RETURN_TEST_RESULT(res, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nSetChecksumIDNList test, set a list but data layer write error...");
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  res = testClass.writeChecksumIdnListToDevice(idnList);
  RETURN_TEST_RESULT(res, L"\nUnexpected return value.", AxisProfileResult::ERR_FIELDBUS);

  return S_OK;
}

HRESULT TestCanopenDeviceParam::testCheckNeedReadBootParams()
{
  log(L"\n<b>Test the checkBootIdnListModified function.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClientEx mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  ProfileAssignedInfo profileAssigned {false};
  mappingHandlerMockup.setReturnValue(profileAssigned);
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileProperty property {};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);

  log(L"\nCheckBootIdnListModified test, drive not support...");
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  property.setSoftwareVersion(5, 1);
  RETURN_TEST_RESULT(testClass.checkNeedReadBootParams(), L"\nUnexpected return value.", true);

  property.setSoftwareVersion(5, 2);
  log(L"\nCheckBootIdnListModified test, checksum read error...");
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  RETURN_TEST_RESULT(testClass.checkNeedReadBootParams(), L"\nUnexpected return value.", true);

  mockClient.resetClient();
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_OK;

  log(L"\nCheckBootIdnListModified test, checksum value different...");
  comm::datalayer::Variant var;
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&var, canopen::CHECKSUM_VALUE.index, canopen::CHECKSUM_VALUE.subIndex);
  mockClient.m_vecCallBackVariant.push_back(var);
  comm::datalayer::Variant idnListVar;
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&idnListVar, canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex);
  mockClient.m_vecCallBackVariant.push_back(idnListVar);
  RETURN_TEST_RESULT(testClass.checkNeedReadBootParams(), L"\nUnexpected return value.", true);

  mockClient.m_nCalledTimes = 0;
  log(L"\nCheckBootIdnListModified test, checksum value same, idn list different to the deault booting idn list...");
  //after checkNeedReadBootParams, the checksum has been updated
  RETURN_TEST_RESULT(testClass.checkNeedReadBootParams(), L"\nUnexpected return value.", true);

  log(L"\nCheckBootIdnListModified test, checksum value same, idn list same to the deault booting idn list...");
  mockClient.resetClient();
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&var, canopen::CHECKSUM_VALUE.index, canopen::CHECKSUM_VALUE.subIndex);
  mockClient.m_vecCallBackVariant.push_back(var);
  mockFieldbusAdapter.m_fbsHelper.writeParam(canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex, canopen::COE_BOOT_PARAM_LIST);
  mockFieldbusAdapter.m_fbsHelper.generateSDOResponseFlatbuffer(&idnListVar, canopen::CHECKSUM_PARAMETERS.index, canopen::CHECKSUM_PARAMETERS.subIndex);
  mockClient.m_vecCallBackVariant.push_back(idnListVar);
  RETURN_TEST_RESULT(testClass.checkNeedReadBootParams(), L"\nUnexpected return value.", false);

  return S_OK;
}

HRESULT TestCanopenDeviceParam::testCheckStandstill()
{
  log(L"\n<b>Test the CheckStandstill function.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileProperty property {};
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);
  bool standstill {false};
  AxisProfileResult res = testClass.checkStandstill(standstill);
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n<b>Test KNOWN_GENERAL_3RD_DRIVE firmware information related functions.</b>");
  property.setDeviceSeries(DeviceSeries::KNOWN_GENERAL_3RD_DRIVE);
  res = testClass.checkStandstill(standstill);
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);
  return S_OK;
}

HRESULT TestCanopenDeviceParam::testLoadBootingTasks()
{
  log(L"\n<b>Test the LoadBootingTasks function.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};
  testClass.m_needReadBootParams = false;
  testClass.loadBootingTasks();
  LOG_TEST_RESULT(testClass.m_initDeviceParam.size(), L"\nUnexpected return value", 5);

  return S_OK;
}

HRESULT TestCanopenDeviceParam::testGetDeviceParamJobObj()
{
  log(L"\n<b>Test the getDeviceParamJobObj function.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};
  AxisProfileResult res = testClass.getDeviceParamJobObj();
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n Test for jobobj is nullptr");
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockJobMgr mockJobMgr {};
  mockJobMgr.m_jobObjPtr = nullptr;
  AxisProfileSharingHandler sharingHandler(&profileTraceMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_JOB_MGR, &mockJobMgr);
  testClass.setSharingHandler(&sharingHandler);
  res = testClass.getDeviceParamJobObj();
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  return S_OK;
}

HRESULT TestCanopenDeviceParam::testCheckAxisHomed()
{
  log(L"\n<b>Test the CheckAxisHomed function.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};
  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileProperty property {};
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);

  bool homed {false};
  AxisProfileResult res = testClass.checkAxisHomed(homed);
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);
  return S_OK;
}

HRESULT TestCanopenDeviceParam::testConstructDeviceParamCoE()
{
  log(L"\n<b>Test the Construct DeviceParamCoE.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  ReadWriteType type = ReadWriteType::READ;
  ObjectDirEntry id {};
  std::uint16_t value {};
  AxisProfileResult res = canopen::CanopenDeviceParamDrive::deviceParamCoE(profileBasicInfo, nullptr, type, id, value, std::size_t(value));
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest for jobobj CmdState::STATE_IDLE");
  test::MockJobObj jobObj {};
  jobObj.m_jobData = nullptr;
  jobObj.m_objState = CmdState::STATE_IDLE;
  res = canopen::CanopenDeviceParamDrive::deviceParamCoE(profileBasicInfo, &jobObj, type, id, value, std::size_t(value));
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest for jobobj CmdState::STATE_ACTIVE");
  jobObj.m_objState = CmdState::STATE_ACTIVE;
  res = canopen::CanopenDeviceParamDrive::deviceParamCoE(profileBasicInfo, &jobObj, type, id, value, std::size_t(value));
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest for jobobj CmdState::STATE_ERROR");
  jobObj.m_objState = CmdState::STATE_ERROR;
  res = canopen::CanopenDeviceParamDrive::deviceParamCoE(profileBasicInfo, &jobObj, type, id, value, std::size_t(value));
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::ERR_JOB_EXECUTION);
  return S_OK;
}

HRESULT TestCanopenDeviceParam::testReadEncoderTypes()
{
  log(L"\n<b>Test the readEncoderTypes function.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  canopen::CanopenDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileProperty property {};
  property.setDeviceSeries(DeviceSeries::KNOWN_GENERAL_3RD_DRIVE);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);
  AxisProfileResult res = testClass.readEncoderTypes();
  LOG_TEST_RESULT(res, L"\n Unexpected return value", AxisProfileResult::STS_OK);

  return S_OK;
}
