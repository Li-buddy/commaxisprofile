#include "test_canopen_probe.h"
#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/canopen/canopen_param_access.h"

//Mockups
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_category.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenProbe);
REGISTER_TEST_TO_GROUP(TestCanopenProbe, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenProbe, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenProbe, Profiles_Canopen);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestCanopenProbe::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Canopen profile Probe class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= SimpleTests();
  hresult |= TestProbeBaseConfig();
  hresult |= TestProbeStatusCtrl();
  hresult |= TestGetProbeValues();
  hresult |= TestGetProbeValuesRT();
  hresult |= TestProbeCmdFailed();

  return hresult;
}

HRESULT TestCanopenProbe::SimpleTests()
{
  HRESULT hresult = S_OK;
  log(L"\n<b>Simple tests.</b>");
  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  AcknowledgeTelegram atData;
  std::uint32_t devAddress = 1;
  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_profileTrace = mockProfileTrace;

  canopen::CanopenProbeDrive testProbe {profileBasicInfo, &atData};
  log(L"\nTest case of onlineChangeParams().");
  ProbeIndex probeIdx {};
  ProbeUpdateMeasurement updateValue {};
  AxisProfileResult result = testProbe.onlineChangeParams(probeIdx, updateValue);
  m_expectedResult = AxisProfileResult::ERR_NOTIMPLEMENTED;
  if (m_expectedResult != result)
  {
    error(L"Function of onlineChangeParams() does not return expected status. Expected: 0x%X, Actual: 0x%X", m_expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\n<b>Test case of abortAndResetProbe().");
  result = testProbe.abortAndResetProbe();
  m_expectedResult = AxisProfileResult::ERR_NOTIMPLEMENTED;
  if (m_expectedResult != result)
  {
    error(L"Function of abortAndResetProbe() does not return expected status. Expected: 0x%X, Actual: 0x%X", m_expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\n<b>Test case of isProbeSupported().");
  result = testProbe.isProbeSupported();
  m_expectedResult = AxisProfileResult::STS_OK;
  if (m_expectedResult != result)
  {
    error(L"Function of isProbeSupported() does not return expected status. Expected: 0x%X, Actual: 0x%X", m_expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\n<b>Test probe extend controlword config.</b>");
  ProbeExtConfig probeExtendConfig {};
  result = testProbe.probeExtConfig(probeIdx, probeExtendConfig, true);
  m_expectedResult = AxisProfileResult::STS_OK;
  if (m_expectedResult != result)
  {
    error(L"Function of probeExtConfig() does not return expected status. Expected: 0x%X, Actual: 0x%X", m_expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\n");
  return hresult;
}

HRESULT TestCanopenProbe::TestProbeBaseConfig()
{
  log(L"\n<b>Test probe ctrlword config.</b>");
  HRESULT hresult = S_OK;
  struct ProbeTestCase
  {
    std::wstring m_testName;             // test name
    std::function<AxisProfileResult(canopen::CanopenProbeDrive& probe)> m_testFun;  // test function
    std::uint16_t m_setDataState;             // set of datalayer value
    AxisProfileResult m_expectedResult;  // expected return value
  };

  std::vector<ProbeTestCase> testCase;
  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterCoE mockFiledbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFiledbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockAxisProfileDeviceParam deviceParamMockup {};
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFiledbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_profileTrace = mockProfileTrace;

  comm::axisprofile::canopen::CanopenParamAccess canopenParamAccess(profileBasicInfo);

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  ProbeTestCase testCase1;
  testCase1.m_testName = L"probeBaseConfig normal case of probe 1.";
  testCase1.m_testFun = [] (canopen::CanopenProbeDrive& probe)
    {
      ProbeIndex m_probeIdx = ProbeIndex::PROBE_1;
      ProbeBaseConfig m_probeConfig {};
      return probe.probeBaseConfig(m_probeIdx, m_probeConfig, false);
    };
  testCase1.m_setDataState = 0x0000;
  testCase1.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase1);

  ProbeTestCase testCase2;
  testCase2.m_testName = L"probeBaseConfig normal case of probe 2.";
  testCase2.m_testFun = [] (canopen::CanopenProbeDrive& probe)
    {
      ProbeIndex m_probeIdx = ProbeIndex::PROBE_2;
      ProbeBaseConfig m_probeConfig {};
      return probe.probeBaseConfig(m_probeIdx, m_probeConfig, false);
    };
  testCase2.m_setDataState = 0x0000;
  testCase2.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase2);

  ProbeTestCase testCase3;
  testCase3.m_testName = L"probeBaseConfig case of probe undefine with no command.";
  testCase3.m_testFun = [] (canopen::CanopenProbeDrive& probe)
    {
      ProbeIndex m_probeIdx = ProbeIndex::PROBE_UNDEFINED;
      ProbeBaseConfig m_probeConfig {};
      return probe.probeBaseConfig(m_probeIdx, m_probeConfig, false);
    };
  testCase3.m_setDataState = 0x0000;
  testCase3.m_expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  testCase.emplace_back(testCase3);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n Test normal case of %s ...", oneTest.m_testName.c_str());
    canopen::CanopenProbeDrive testProbe {profileBasicInfo, &atData};

    // set sharing handler for mapping
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &canopenParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);

    testProbe.resetProbeCommand();
    testProbe.booting();
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(&mockClient, data);
    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testProbe);
      if (result != AxisProfileResult::STS_PENDING)
      {
        RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
  }

  return hresult;
}

HRESULT TestCanopenProbe::TestProbeStatusCtrl()
{
  log(L"\n<b>Test probe status control.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;             // test name
    std::function<AxisProfileResult(canopen::CanopenProbeDrive& probe)> m_testFun;  // test function
    std::uint16_t m_setDataState;             // set of datalayer value
    AxisProfileResult m_expectedResult;  // expected return value
  };

  ////////////////////////////////////////Create Tests cases//////////////////////////////////////////////////////// 
  std::vector<ProbeTestCase> testCase {
    {L"Enable probe normal case",           [] (canopen::CanopenProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 0; probe.m_probeParam.m_probe2Enable = 0; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, true);}, 0x0000, AxisProfileResult::STS_OK},
    {L"Enable probe2 and probe1 activated", [] (canopen::CanopenProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 0; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_2, true);}, 0x0000, AxisProfileResult::STS_OK},
    {L"Disable probe1 and probe2 disabled", [] (canopen::CanopenProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 0; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, true);}, 0x0000, AxisProfileResult::STS_OK},
    {L"Disable probe2 and probe1 enabled",  [] (canopen::CanopenProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, false);},0x0000, AxisProfileResult::STS_OK},
    {L"Invalid probe",                      [] (canopen::CanopenProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_UNDEFINED, true);}, 0x0000, AxisProfileResult::ERR_INVALID_ARGUMENT},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_profileTrace = mockProfileTrace;

  comm::axisprofile::canopen::CanopenParamAccess canopenParamAccess(profileBasicInfo);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n Test normal case of %s ...", oneTest.m_testName.c_str());
    canopen::CanopenProbeDrive testProbe {profileBasicInfo, &atData};
    // set sharing handler for mapping
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &canopenParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);

    testProbe.booting();
    testProbe.resetProbeCommand();
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(&mockClient, data);
    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testProbe);
      if (result != AxisProfileResult::STS_PENDING)
      {
        RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
  }

  return S_OK;
}

HRESULT TestCanopenProbe::TestGetProbeValues()
{
  log(L"\n<b>Test get probe value.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(canopen::CanopenProbeDrive& probe)> m_testFun; // test function 
    std::uint16_t m_setDataState;                    // set of datalayer value
    AxisProfileResult m_expectedResult;         // expected return value
  };

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  std::vector<ProbeTestCase> testCase {
    {L"Get probe 1 values.",      [] (canopen::CanopenProbeDrive& probe){ProbeBaseConfig probeConfig {}; ProbeExtConfig probeExtendConfig; return probe.getProbeValues(ProbeIndex::PROBE_1, &probeConfig, &probeExtendConfig);}, 0x0000, AxisProfileResult::STS_OK},
    {L"Get probe 2 values.",      [] (canopen::CanopenProbeDrive& probe){ProbeBaseConfig probeConfig {}; ProbeExtConfig probeExtendConfig; return probe.getProbeValues(ProbeIndex::PROBE_2, &probeConfig, &probeExtendConfig);}, 0x0000, AxisProfileResult::STS_OK},
    {L"Get values but undefine.", [] (canopen::CanopenProbeDrive& probe){ProbeBaseConfig probeConfig {}; ProbeExtConfig probeExtendConfig; return probe.getProbeValues(ProbeIndex::PROBE_UNDEFINED, &probeConfig, &probeExtendConfig);}, 0x0000, AxisProfileResult::ERR_INVALID_ARGUMENT},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_profileTrace = mockProfileTrace;

  comm::axisprofile::canopen::CanopenParamAccess canopenParamAccess(profileBasicInfo);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    canopen::CanopenProbeDrive testProbe {profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &canopenParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    testProbe.resetProbeCommand();
    testProbe.booting();
    TestDataVariantU16 data {L"Add invalid data", oneTest.m_setDataState, true};
    setResponseVariant(&mockClient, data);
    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testProbe);
      if (result != AxisProfileResult::STS_PENDING)
      {
        RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
  }

  return S_OK;
}

HRESULT TestCanopenProbe::TestGetProbeValuesRT()
{
  log(L"\n<b>Test get probe real time values.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(canopen::CanopenProbeDrive& probe)> m_testFun; // test function 
    AxisProfileResult m_expectedResult;         // expected return value
  };

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  std::vector<ProbeTestCase> testCase {
    {L"Get probe1 actual values with position.",     [] (canopen::CanopenProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_1, &actualValueProbe);}, AxisProfileResult::STS_OK},
    {L"Get probe1 actual values with fien time.",    [] (canopen::CanopenProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_1, &actualValueProbe);}, AxisProfileResult::STS_OK},
    {L"Get probe2 actual values with position.",     [] (canopen::CanopenProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_2, &actualValueProbe);}, AxisProfileResult::STS_OK},
    {L"Get probe2 actual values with fien time.",    [] (canopen::CanopenProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_2, &actualValueProbe);}, AxisProfileResult::STS_OK},
    {L"Get probe actual values but probe undefine.", [] (canopen::CanopenProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_UNDEFINED, &actualValueProbe);}, AxisProfileResult::ERR_INVALID_ARGUMENT},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_profileTrace = mockProfileTrace;

  comm::axisprofile::canopen::CanopenParamAccess canopenParamAccess(profileBasicInfo);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    canopen::CanopenProbeDrive testProbe {profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &canopenParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    testProbe.resetProbeCommand();
    testProbe.booting();

    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testProbe);
      if (result != AxisProfileResult::STS_PENDING)
      {
        RETURN_TEST_RESULT(result, L"\n  unexpected return value.", oneTest.m_expectedResult);
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n  Execute timeout.");
    }
  }

  return S_OK;
}

HRESULT TestCanopenProbe::TestProbeCmdFailed()
{
  log(L"\n<b>Test probe commands failed situations.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(canopen::CanopenProbeDrive& probe)> m_testFun; // test function 
    AxisProfileResult m_expectedResult;         // expected return value
  };

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  std::vector<ProbeTestCase> testCase {
    {L"Set probe control word failed.", [] (canopen::CanopenProbeDrive& probe){ProbeBaseConfig probeConfig; return probe.probeBaseConfig(ProbeIndex::PROBE_1, probeConfig, false);}, AxisProfileResult::ERR_EXEC_NULL},
    {L"Set probe status control failed.", [] (canopen::CanopenProbeDrive& probe){return probe.probeStatusCtrl(ProbeIndex::PROBE_1, true);}, AxisProfileResult::ERR_EXEC_NULL},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = 0;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_profileTrace = mockProfileTrace;

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n Test normal case of %s ...", oneTest.m_testName.c_str());
    canopen::CanopenProbeDrive testProbe {profileBasicInfo, &atData};
    testProbe.resetProbeCommand();
    std::uint16_t cycleExpired = 100; // max cycle for the test
    while (true)
    {
      result = oneTest.m_testFun(testProbe);
      if (result != AxisProfileResult::STS_PENDING)
      {
        RETURN_TEST_RESULT(result, L"\n unexpected return value.", oneTest.m_expectedResult);
        break;
      }
      RETURN_TEST_FAILED(cycleExpired--, L"\n Execute timeout.");
    }
  }

  return S_OK;
}

//////////////////////////////////////////////Helper Methods////////////////////////////////////////////////////////////
void TestCanopenProbe::setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16& data)
{
  flatbuffers::FlatBufferBuilder builder {};
  builder.ForceDefaults(true);
  std::uint8_t buffer[2];
  std::size_t dataSize = 2;
  (void)memcpy(buffer, &data.m_data, dataSize);
  auto vec = builder.CreateVector(buffer, dataSize);
  comm::ethercat::master::fbs::SDOResponseBuilder paramResponseBuilder(builder);
  if (data.m_addData)
  {
    paramResponseBuilder.add_data(vec);
  }
  auto exchangeDataResponse = paramResponseBuilder.Finish();
  comm::ethercat::master::fbs::SDOBuilder paramBuilder(builder);
  paramBuilder.add_response(exchangeDataResponse);
  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);
  comm::datalayer::Variant variant;
  variant.copyFlatbuffers(builder);
  client->m_callbackVariant = variant;
}
