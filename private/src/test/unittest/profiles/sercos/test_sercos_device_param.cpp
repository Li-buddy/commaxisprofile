#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/drive/sercos_device_param_drive.h"
#include "profiles/sercos/sercos_boot_params.h"
#include "test_sercos_device_param.h"

//Mockups
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/external_bundles/mock_datalayer_client_ex.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_job_mgr.h"
#include "mockup/mock_axisprofile_cyclic_data.h"

using namespace comm::axisprofile;
using namespace test;

REGISTER_TEST(TestSercosDeviceParam);
REGISTER_TEST_TO_GROUP(TestSercosDeviceParam, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosDeviceParam, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosDeviceParam, Profiles_Sercos);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestSercosDeviceParam::TestSercosDeviceParam()
{
}


TestSercosDeviceParam::~TestSercosDeviceParam(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestSercosDeviceParam::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Sercos profile DeviceParam class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testChecksum();
  hresult |= testCheckNeedReadBootParams();
  hresult |= testDelayBrakeInfo();
  hresult |= testMappingBrakeInfo();
  hresult |= testBootingMethods();
  hresult |= testCheckFunctionSupported();
  hresult |= testreadPosProcessMode();
  hresult |= testLoadBootingTasks();
  hresult |= testCheckAxisHomed();
  hresult |= testGetDeviceParamJobObj();
  return hresult;
}

HRESULT TestSercosDeviceParam::testDelayBrakeInfo()
{
  const std::vector<TestDelayInformation> testCases
  {
    {L"Test case of brake delay information not support", false, 0x0001, AxisProfileResult::STS_OK  },
    {L"Test case of light error activated",               true,  0x0001, AxisProfileResult::STS_OK  },
    {L"Test case of light error inactivated",             true,  0x0010, AxisProfileResult::STS_OK  },
    {L"Test case of fieldbus error inactivated",          true,  0x0004, AxisProfileResult::STS_OK  },
    {L"Test case of fieldbus error inactivated",          true,  0x0010, AxisProfileResult::STS_OK  },
    {L"Test case of critical error inactivated",          true,  0x0008, AxisProfileResult::STS_OK  },
    {L"Test case of critical error inactivated",          true,  0x0010, AxisProfileResult::STS_OK  },
  };
  HRESULT hresult = S_OK;
  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  std::uint32_t deviceAddress = 1;

  ProfileBasicInfo profileBasicInfo;
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";

  sercos::SercosDeviceParamDrive testClass{profileBasicInfo, nullptr, nullptr};
  mockClient.m_simulateTimeout = false;

  for (const auto& testCase : testCases)
  {
    hresult |= executeTestDelayBrakeInfo(testClass, testCase);
  }
  return hresult;
}

HRESULT TestSercosDeviceParam::executeTestDelayBrakeInfo(comm::axisprofile::sercos::SercosDeviceParamDrive& testDeviceParam, const TestDelayInformation& testCase)
{
  HRESULT hresult = S_OK;
  testDeviceParam.m_brakeDelySupport = testCase.m_delaySupport;
  testDeviceParam.m_bootingDataMultiBrake.m_multiBrakeDelaySupport = testCase.m_multiBrakeDelaySupportCase;;

  AxisProfileResult result = testDeviceParam.getDelayBrakeInfo();

  if (testCase.m_expectedResult != result)
  {
    hresult = E_FAIL;
    error(L"%ls test failed, Actual state Cmd: %d, Req: %d", testCase.m_name, result, testCase.m_expectedResult);
  }

  return hresult;
}

HRESULT TestSercosDeviceParam::testMappingBrakeInfo()
{
  log(L"\n<b>Test MappingBrakeInfo.</b>");
  HRESULT hresult = S_OK;
  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  std::uint32_t deviceAddress = 1;
  ProfileBasicInfo profileBasicInfo;
  std::uint8_t driveNumber = 0;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";

  sercos::SercosDeviceParamDrive testClass{profileBasicInfo, nullptr, nullptr};
  mockClient.m_simulateTimeout = false;

  log(L"\nTest sharingHandler is nullptr.");
  bool checkDecelerationLimitSupport = false;
  AxisProfileResult res = testClass.mappingBrakeInfo(checkDecelerationLimitSupport);
  LOG_TEST_RESULT(res, L"\n sharingHandle is null fail", AxisProfileResult::ERR_EXEC_NULL);

  mockClient.m_simulateTimeout = false;

  AxisProfileResult expectedResult = AxisProfileResult::STS_OK;
  testClass.m_bootingDataMultiBrake.m_controlUnitReactionBestDeceleration = 0x0042;

  AxisProfileSharingHandler shareHandler {&mockProfileTrace};
  test::MockScaling mockScaling;
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
  testClass.setSharingHandler(&shareHandler);

  AxisProfileResult result = testClass.mappingBrakeInfo(checkDecelerationLimitSupport);
  if (expectedResult != result)
  {
    hresult = E_FAIL;
    error(L"%ls test failed, Actual state Cmd: %d, Req: %d", result, expectedResult);
  }

  testClass.m_bootingDataMultiBrake.m_controlUnitReactionBestDeceleration = 0x0003;
  result = testClass.mappingBrakeInfo(checkDecelerationLimitSupport);

  if (expectedResult != result)
  {
    hresult = E_FAIL;
    error(L"%ls test failed, Actual state Cmd: %d, Req: %d", result, expectedResult);
  }

  testClass.m_bootingDataMultiBrake.m_controlUnitReactionBestDeceleration = 0x0004;
  testClass.mappingBrakeInfo(checkDecelerationLimitSupport);

  if (expectedResult != result)
  {
    hresult = E_FAIL;
    error(L"%ls test failed, Actual state Cmd: %d, Req: %d", result, expectedResult);
  }

  return hresult;
}

HRESULT TestSercosDeviceParam::testBootingMethods()
{
  log(L"\n<b>Test booting methods.</b>");
  struct CommandsTestCase
  {
    std::wstring m_testName;                // test name
    std::function<AxisProfileResult(sercos::SercosDeviceParamDrive& cmd)> m_testFun; // test function
    std::uint16_t m_setData;                     // set datalayer data
    AxisProfileResult m_adapterRetVal;      // set expected fieldbus return value
    AxisProfileResult m_expectedResult;     // set expected result
  };

  std::vector<CommandsTestCase> testCase {
    {L"readEncodersTypes normal case 1", [] (sercos::SercosDeviceParamDrive& cmd)
    {
      return cmd.readDriveLimits();
  }, 0x0000, AxisProfileResult::STS_OK, AxisProfileResult::STS_OK},
  {L"readEncodersTypes normal case 2", [] (sercos::SercosDeviceParamDrive& cmd)
   {
     return cmd.readDriveLimits();
  }, 0x0100, AxisProfileResult::STS_OK, AxisProfileResult::STS_OK},
  {L"readEncodersTypes field failed", [] (sercos::SercosDeviceParamDrive& cmd)
   {
     return cmd.readDriveLimits();
  }, 0x0000, AxisProfileResult::ERR_NOK, AxisProfileResult::ERR_NOK},

  {L"readEncodersTypes normal case 1", [] (sercos::SercosDeviceParamDrive& cmd)
   {
     return cmd.readEncodersTypes();
  }, 0x0000, AxisProfileResult::STS_OK, AxisProfileResult::STS_OK},
  {L"readEncodersTypes field failed", [] (sercos::SercosDeviceParamDrive& cmd)
   {
     return cmd.readEncodersTypes();
  }, 0x0000, AxisProfileResult::ERR_NOK, AxisProfileResult::STS_OK},
  };

  MockAxisProfileTrace mockProfileTrace {};
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  std::uint32_t devAddress = 1;
  std::uint8_t driveNumber = 0;

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";

  for (auto& oneTest : testCase)
  {
    AxisProfileResult result {AxisProfileResult::STS_OK};

    // add assign item
    test::MockAxisProfileTrace profileTraceMockup {};
    test::MockAxisProfileMappingHandler mappingHandlerMockup {};
    //mock for position
    auto& assign = mappingHandlerMockup.getProfileAssignedInfo();
    assign.m_input.m_position = true;
    mappingHandlerMockup.setReturnValue(assign);

    sercos::SercosDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};
    AxisProfileSharingHandler sharingHandler(&profileTraceMockup);
    sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &testClass);
    sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);

    log(L"\n  Test of %s ...", oneTest.m_testName.c_str());
    mockFieldbusAdapter.m_returnValue = oneTest.m_adapterRetVal;
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setData, true};
    setResponseVariant(&mockClient, data);

    result = oneTest.m_testFun(testClass);
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
  }
  log(L"\n");
  return S_OK;
}

void TestSercosDeviceParam::setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16& data)
{
  flatbuffers::FlatBufferBuilder builder {};
  builder.ForceDefaults(true);
  std::uint8_t buffer[2];
  std::size_t dataSize = 2;
  (void)memcpy(buffer, &data.m_data, dataSize);
  auto vec = builder.CreateVector(buffer, dataSize);
  comm::ethercat::master::fbs::ElementFlags respElementFlags = static_cast<comm::ethercat::master::fbs::ElementFlags>(comm::ethercat::master::fbs::ElementFlags::ElementFlags_value);
  comm::ethercat::master::fbs::ParameterResponseBuilder paramResponseBuilder(builder);
  if (data.m_addData)
  {
    paramResponseBuilder.add_elementFlags(respElementFlags);
    paramResponseBuilder.add_data(vec);
  }
  auto exchangeDataResponse = paramResponseBuilder.Finish();
  comm::ethercat::master::fbs::ParameterBuilder paramBuilder(builder);
  paramBuilder.add_response(exchangeDataResponse);
  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);
  comm::datalayer::Variant variant;
  variant.copyFlatbuffers(builder);
  client->m_callbackVariant = variant;
}

HRESULT TestSercosDeviceParam::testCheckFunctionSupported()
{
  log(L"\n<b>Test the checking of the supported functionality.</b>");
  HRESULT hresult = S_OK;

  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  FlatbuffersHelperSoE fbsHelper{};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = 1;
  profileBasicInfo.m_driveNumber = 0;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";

  sercos::SercosDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  ProfileAssignedInfo profileAssigned {false};
  mappingHandlerMockup.setReturnValue(profileAssigned);

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, nullptr);

  auto& assignedInfo = mappingHandlerMockup.getProfileAssignedInfo();
  assignedInfo.m_output.m_controlWord = false;
  assignedInfo.m_input.m_statusWord = false;

  testClass.prepareSignalFuncHandler();

  log(L"\n  Test the failure checking...");
  FunctionSupportErrorInfo errorInfo;
  bool result = testClass.checkFunctionSupported(FunctionSupport::OPMODE_CYCLIC, errorInfo);
  LOG_TEST_RESULT_ANY(result == false, L"\n  unexpected return value.");

  assignedInfo.m_output.m_controlWord = true;
  assignedInfo.m_input.m_statusWord = true;
  log(L"\n  Test the success checking...");
  result = testClass.checkFunctionSupported(FunctionSupport::OPMODE_CYCLIC, errorInfo);
  LOG_TEST_RESULT_ANY(result == true, L"\n  unexpected return value.");

  log(L"\n  Test the failure checking of HOMING_STATUS_CYCLIC...");
  result = testClass.checkFunctionSupported(FunctionSupport::HOMING_STATUS_CYCLIC, errorInfo);
  LOG_TEST_RESULT_ANY(!result && errorInfo.errorType == FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, L"\n  unexpected return value.");

  log(L"\n  Test the failure checking of ERROR_CODE_CYCLIC...");
  result = testClass.checkFunctionSupported(FunctionSupport::ERROR_CODE_CYCLIC, errorInfo);
  LOG_TEST_RESULT_ANY(!result && errorInfo.errorType == FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, L"\n  unexpected return value.");

  log(L"\n  Test the failure checking of invalid function enumeration...");
  result = testClass.checkFunctionSupported(FunctionSupport::MAX_VALUE, errorInfo);
  LOG_TEST_RESULT_ANY(!result && errorInfo.errorType == FuncSupportErrorType::ERROR_NOT_DEFINED, L"\n  unexpected return value.");

  return hresult;
}

HRESULT TestSercosDeviceParam::testreadPosProcessMode()
{
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = 1;
  profileBasicInfo.m_driveNumber = 0;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";
  sercos::SercosDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  double outputDelay;
  testClass.m_dcSync0CycleTime = 0.005;
  TestDataVariantU16 data {L"PosPrcMode", 0xFFFF, true};

  log(L"\nPosition process mode 0...");
  data.m_data = 0;
  setResponseVariant(&mockClient, data);
  auto result = testClass.readPosProcessMode();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  result = testClass.getOutputEffectiveDelay(outputDelay);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  log(L"\nOutput delay is 1 cycle in mode 0...");
  LOG_TEST_RESULT_FLT(outputDelay, L"\nUnexpected output delay time.", testClass.m_dcSync0CycleTime, 1E-9);

  log(L"\nPosition process mode 2...");
  data.m_data = 2;
  setResponseVariant(&mockClient, data);
  result = testClass.readPosProcessMode();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  result = testClass.getOutputEffectiveDelay(outputDelay);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  log(L"\nOutput delay is 2 cycle in mode 2...");
  LOG_TEST_RESULT_FLT(outputDelay, L"\nUnexpected output delay time.", 2 * testClass.m_dcSync0CycleTime, 1E-9);

  log(L"\nPosition process incorrect mode...");
  data.m_data = 100;
  setResponseVariant(&mockClient, data);
  result = testClass.readPosProcessMode();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_BOOTING_CONTINUE);

  log(L"\nRequest datalayer failed...");
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  result = testClass.readPosProcessMode();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_BOOTING_CONTINUE);

  return S_OK;
}

HRESULT TestSercosDeviceParam::testChecksum()
{
  log(L"\n<b>Test the checksum related functions.</b>");
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = 1;
  profileBasicInfo.m_driveNumber = 0;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";
  sercos::SercosDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};

  test::MockAxisProfileProperty property {};
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testClass);

  test::FlatbuffersHelperSoE fbsHelper {};
  comm::datalayer::Variant responseFB {};

  log(L"\nGetChecksum test...");
  fbsHelper.generateParamResponseFlatbuffer(&responseFB, IDN_S(0, 326));
  mockClient.m_callbackVariant = responseFB;
  std::uint32_t checksum {};
  auto result = testClass.readChecksumValueFromDevice(checksum);
  SoEValue expChecksum {};
  fbsHelper.readParam(IDN_S(0, 326), expChecksum);
  RETURN_TEST_RESULT_ANY(std::get<std::uint32_t>(expChecksum) == checksum, L"\nUnexpected return value.");
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nReadChecksumIdnList test, read data error...");
  std::vector<std::uint16_t> IdnList {};
  fbsHelper.generateParamResponseFlatbuffer(&responseFB, IDN_S(0, 327));
  mockClient.m_callbackVariant = responseFB;
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  result = testClass.readChecksumIdnListFromDevice(IdnList);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::ERR_FIELDBUS);
  RETURN_TEST_RESULT_ANY(testClass.checkBootIdnListModified(), L"\nUnexpected return value.");

  log(L"\nReadChecksumIdnList test, normal case...");
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  result = testClass.readChecksumIdnListFromDevice(IdnList);
  SoEValue expIDNList {};
  fbsHelper.readParam(IDN_S(0, 327), expIDNList);
  RETURN_TEST_RESULT_ANY(std::get<std::vector<std::uint16_t>>(expIDNList) == IdnList, L"\nUnexpected return value.");
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_ANY(testClass.checkBootIdnListModified(), L"\nUnexpected return value.");
  std::vector<std::string> idnStrList;
  RETURN_TEST_RESULT(testClass.getChecksumIdnList(idnStrList), L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nReadChecksumIdnList test, return default idn list...");
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  fbsHelper.writeParam(IDN_S(0, 327), sercos::SOE_CTRLX_DRIVE_BOOT_PARAM_LIST);
  fbsHelper.generateParamResponseFlatbuffer(&responseFB, IDN_S(0, 327));
  mockClient.m_callbackVariant = responseFB;
  IdnList.clear();
  result = testClass.readChecksumIdnListFromDevice(IdnList);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_ANY(IdnList.size() == sercos::SOE_CTRLX_DRIVE_BOOT_PARAM_LIST.size(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT_ANY(!testClass.checkBootIdnListModified(), L"\nUnexpected return value.");
  RETURN_TEST_RESULT(testClass.getChecksumIdnList(idnStrList), L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nReadChecksumIdnList test, empty list...");
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  fbsHelper.writeParam(IDN_S(0, 327), std::vector<std::uint16_t>{});
  fbsHelper.generateParamResponseFlatbuffer(&responseFB, IDN_S(0, 327));
  mockClient.m_callbackVariant = responseFB;
  IdnList.clear();
  result = testClass.readChecksumIdnListFromDevice(IdnList);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_ANY(IdnList.empty(), L"\nUnexpected return value.");

  log(L"\nWriteChecksumIdnList test, set a empty list...");
  result = testClass.writeChecksumIdnListToDevice(IdnList);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nWriteChecksumIdnList test, set a list with a normal case...");
  IdnList = {IDN_S(0, 121), IDN_S(0, 122)};
  result = testClass.writeChecksumIdnListToDevice(IdnList);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nWriteChecksumIdnList test, set a list but data layer write error...");
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  result = testClass.writeChecksumIdnListToDevice(IdnList);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::ERR_FIELDBUS);

  return S_OK;
}

HRESULT TestSercosDeviceParam::testCheckNeedReadBootParams()
{
  log(L"\n<b>Test the checkBootIdnListModified function.</b>");
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClientEx mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  sercos::SercosDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};

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
  test::FlatbuffersHelperSoE fbsHelper{};
  comm::datalayer::Variant responseFB{};

  log(L"\nCheckBootIdnListModified test, checksum value different...");
  fbsHelper.generateParamResponseFlatbuffer(&responseFB, IDN_S(0, 326));
  mockClient.m_vecCallBackVariant.push_back(responseFB);
  fbsHelper.generateParamResponseFlatbuffer(&responseFB, IDN_S(0, 327));
  mockClient.m_vecCallBackVariant.push_back(responseFB);
  RETURN_TEST_RESULT(testClass.checkNeedReadBootParams(), L"\nUnexpected return value.", true);

  mockClient.m_nCalledTimes = 0;// resetClient();
  log(L"\nCheckBootIdnListModified test, checksum value same, idn list different to the deault booting idn list...");
  //after checkNeedReadBootParams, the checksum has been updated
  RETURN_TEST_RESULT(testClass.checkNeedReadBootParams(), L"\nUnexpected return value.", true);

  log(L"\nCheckBootIdnListModified test, checksum value same, idn list same to the deault booting idn list...");
  mockClient.resetClient();
  mockClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  fbsHelper.generateParamResponseFlatbuffer(&responseFB, IDN_S(0, 326));
  mockClient.m_vecCallBackVariant.push_back(responseFB);
  fbsHelper.writeParam(IDN_S(0, 327), sercos::SOE_CTRLX_DRIVE_BOOT_PARAM_LIST);
  fbsHelper.generateParamResponseFlatbuffer(&responseFB, IDN_S(0, 327));
  mockClient.m_vecCallBackVariant.push_back(responseFB);
  RETURN_TEST_RESULT(testClass.checkNeedReadBootParams(), L"\nUnexpected return value.", false);
  return S_OK;
}

HRESULT TestSercosDeviceParam::testLoadBootingTasks()
{
  log(L"\n<b>Test the LoadBootingTasks function for non-required BootParams.</b>");
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  sercos::SercosDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};
  testClass.m_needReadBootParams = false;
  testClass.loadBootingTasks();
  RETURN_TEST_RESULT(testClass.m_initDeviceParam.size(), L"\nUnexpected return value", 2);
  return S_OK;
}

HRESULT TestSercosDeviceParam::testCheckAxisHomed()
{
  log(L"\n<b>Test the CheckAxisHomed function.</b>");
  HRESULT hResult = S_OK;

  // Prepare profile info and telegrams
  test::MockFieldbusAdapterSoE mockFieldbus{};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbus;
  AcknowledgeTelegram atData;
  MasterDataTelegram mdtData;

  // Create test instance
  sercos::SercosDeviceParamDrive testDeviceParam(profileBasicInfo, &atData, &mdtData);

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  comm::axisprofile::ProfileAssignedInfo profileAssigned {false,false};
  mappingHandlerMockup.setReturnValue(profileAssigned);
  test::MockAxisProfileProperty property {};
  test::MockAxisProfileCyclicData cyclicData {};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &cyclicData);

  // not mapped homed
  property.setDeviceSeries(DeviceSeries::INNOVANCE);
  bool homed {};
  testDeviceParam.checkAxisHomed(homed);
  RETURN_TEST_RESULT(homed, L"Expected not homed", false);

  // Test: Not homed (m_bitStatusHomed = 0)
  mappingHandlerMockup.getProfileAssignedInfo().m_input.m_homed = true;
  atData.m_bitStatusHomed.m_byte = {0};
  testDeviceParam.checkAxisHomed(homed);
  RETURN_TEST_RESULT(homed, L"Expected homed but not same as expect", false);

  // Test: Homed (m_bitStatusHomed = 1)
  atData.m_bitStatusHomed.m_byte = {1};
  testDeviceParam.checkAxisHomed(homed);
  RETURN_TEST_RESULT(homed, L"Expected homed", true);

  testDeviceParam.prepareSignalFuncHandler();
  auto mgr = testDeviceParam.getSignalFuncHandler();
  SignalFunction func {};
  func.controlSupported = true;
  func.statusSupported = true;
  auto result = mgr->resetSignalFuncBit(func);
  RETURN_TEST_RESULT(result, L"failed to reset", AxisProfileResult::STS_OK);

  result = mgr->activeSignalFunc(func);
  RETURN_TEST_RESULT(result, L"failed to reset", AxisProfileResult::STS_OK);
  bool value {};
  result = mgr->getSignalFuncState(func, value);
  RETURN_TEST_RESULT(result, L"failed to reset", AxisProfileResult::STS_OK);

  return hResult;
}

HRESULT TestSercosDeviceParam::testGetDeviceParamJobObj()
{
  log(L"\n<b>Test the getDeviceParamJobObj function.</b>");
  MockFieldbusAdapterSoE mockFieldbusAdapter {};
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  sercos::SercosDeviceParamDrive testClass {profileBasicInfo, nullptr, nullptr};
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
