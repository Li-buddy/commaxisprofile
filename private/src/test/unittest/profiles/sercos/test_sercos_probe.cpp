#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/signal_func/signal_func_handler.h"
#include "profiles/sercos/sercos_param_access.h"
#include "profiles/sercos/drive/sercos_probe_drive.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_category.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "mockup/mock_axisprofile_device_param.h"

#include "test_sercos_probe.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestSercosProbe);
REGISTER_TEST_TO_GROUP(TestSercosProbe, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosProbe, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosProbe, Profiles_Sercos);

HRESULT TestSercosProbe::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the sercos profile Probe class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= SimpleTests();
  hresult |= TestProbeBaseConfig();
  hresult |= TestProbeExtConfig();
  hresult |= TestProbeStatusCtrl();
  hresult |= TestOnlineChangeParams();
  hresult |= TestGetProbeValues();
  hresult |= TestGetProbeValuesRT();
  hresult |= TestProbeCmdFailed();
  hresult |= TestMeasuredSignalConvert();
  hresult |= TestisProbeSupported();
  hresult |= TestProbeStatusCtrlSignalFunction();

  return hresult;
}

HRESULT TestSercosProbe::SimpleTests()
{
  log(L"\n<b>Simple tests.</b>");
  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = "testProfile";


  sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};
  log(L"\nTest case of time measurement exceed...");

  // 0.06s is already beyond the time limit.
  double exceedTime = 0.06;
  testProbe.timeUnitAdapteDevice(exceedTime);

  return S_OK;
}

HRESULT TestSercosProbe::TestProbeBaseConfig()
{
  log(L"\n<b>Test probe ctrlword config.</b>");
  HRESULT hresult = S_OK;
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosProbeDrive& probe)> m_testFun; // test function
    std::uint16_t m_setDataState {0};                    // set of datalayer value
    AxisProfileResult m_expectedResult {AxisProfileResult::STS_OK};         // expected return value
  };

  std::vector<ProbeTestCase> testCase;
  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  test::MockAxisProfileTrace profileTraceMockup{};
  test::MockAxisProfileMappingHandler mappingHandlerMockup{};
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  ProbeTestCase testCase1;
  testCase1.m_testName = L"probeBaseConfig normal case of probe 1.";
  testCase1.m_testFun = [] (sercos::SercosProbeDrive& probe)
  {
    ProbeIndex m_probeIdx = ProbeIndex::PROBE_1;
    ProbeBaseConfig m_probeConfig {};
    return probe.probeBaseConfig(m_probeIdx, m_probeConfig, true);
  };
  testCase1.m_setDataState = 0x0000;
  testCase1.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase1);

  ProbeTestCase testCase2;
  testCase2.m_testName = L"probeBaseConfig normal case of probe 2.";
  testCase2.m_testFun = [] (sercos::SercosProbeDrive& probe)
  {
    ProbeIndex m_probeIdx = ProbeIndex::PROBE_2;
    ProbeBaseConfig m_probeConfig {};
    return probe.probeBaseConfig(m_probeIdx, m_probeConfig, true);
  };
  testCase2.m_setDataState = 0x0000;
  testCase2.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase2);

  ProbeTestCase testCase3;
  testCase3.m_testName = L"probeBaseConfig case of probe undefine with no command.";
  testCase3.m_testFun = [] (sercos::SercosProbeDrive& probe)
  {
    ProbeIndex m_probeIdx = ProbeIndex::PROBE_UNDEFINED;
    ProbeBaseConfig m_probeConfig {};
    return probe.probeBaseConfig(m_probeIdx, m_probeConfig, true);
  };
  testCase3.m_setDataState = 0x0000;
  testCase3.m_expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  testCase.emplace_back(testCase3);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};

    // set sharing handler for mapping
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
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

HRESULT TestSercosProbe::TestProbeExtConfig()
{
  log(L"\n<b>Test probe extend controlword config.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosProbeDrive& probe)> m_testFun; // test function
    std::uint16_t m_setDataState {0};                    // set of datalayer value
    AxisProfileResult m_expectedResult {AxisProfileResult::STS_OK};         // expected return value
  };

  std::vector<ProbeTestCase> testCase;
  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  // set sharing handler for mapping
  test::MockAxisProfileTrace profileTraceMockup{};
  test::MockAxisProfileMappingHandler mappingHandlerMockup{};
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  ProbeTestCase testCase1;
  testCase1.m_testName = L"probeExtConfig normal case of probe 1";
  testCase1.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeIndex probeIdx = ProbeIndex::PROBE_1;
    ProbeExtConfig probeExtendConfig{};
    probeExtendConfig.m_measuredSignal = MeasuringSignalSel::POS_FEEDBACK_VAL_ENCODER_1;
    probeExtendConfig.m_isTimeMeasureActivated = false;
    probeExtendConfig.m_isExpWindowActivated = true;
    return probe.probeExtConfig(probeIdx, probeExtendConfig, true);
  };
  testCase1.m_setDataState = 0x0000;
  testCase1.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase1);

  ProbeTestCase testCase2;
  testCase2.m_testName = L"probeExtConfig normal case of probe 2";
  testCase2.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeIndex probeIdx = ProbeIndex::PROBE_2;
    ProbeExtConfig probeExtendConfig{};
    probeExtendConfig.m_measuredSignal = MeasuringSignalSel::POS_FEEDBACK_VAL_ENCODER_2;
    return probe.probeExtConfig(probeIdx, probeExtendConfig, true);
  };
  testCase2.m_setDataState = 0x0000;
  testCase2.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase2);

  ProbeTestCase testCase3;
  testCase3.m_testName = L"probeExtConfig normal case of probe 1 with system fine time";
  testCase3.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeIndex probeIdx = ProbeIndex::PROBE_1;
    ProbeExtConfig probeExtendConfig{};
    probeExtendConfig.m_measuredSignal = MeasuringSignalSel::SYSTEM_FINE_TIME;
    return probe.probeExtConfig(probeIdx, probeExtendConfig, true);
  };
  testCase3.m_setDataState = 0x0000;
  testCase3.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase3);

  ProbeTestCase testCase4;
  testCase4.m_testName = L"probeExtConfig case of probe undefine with no command";
  testCase4.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeIndex probeIdx = ProbeIndex::PROBE_UNDEFINED;
    ProbeExtConfig probeExtendConfig{};
    probeExtendConfig.m_measuredSignal = MeasuringSignalSel::NO_COMMAND_ACTIVE;
    return probe.probeExtConfig(probeIdx, probeExtendConfig, true);
  };
  testCase4.m_setDataState = 0x0000;
  testCase4.m_expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  testCase.emplace_back(testCase4);

  ProbeTestCase testCase5;
  testCase5.m_testName = L"probeExtConfig case of probe1 with signal no define";
  testCase5.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeIndex probeIdx = ProbeIndex::PROBE_1;
    ProbeExtConfig probeExtendConfig{};
    probeExtendConfig.m_measuredSignal = MeasuringSignalSel::SIGNAL_UNDEFINED;
    return probe.probeExtConfig(probeIdx, probeExtendConfig, true);
  };
  testCase5.m_setDataState = 0x0000;
  testCase5.m_expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  testCase.emplace_back(testCase5);

  ProbeTestCase testCase6;
  testCase6.m_testName = L"probeExtConfig case of probe1 with no command";
  testCase6.m_testFun = [] (sercos::SercosProbeDrive& probe)
    {
      ProbeIndex probeIdx = ProbeIndex::PROBE_1;
      ProbeExtConfig probeExtendConfig {};
      probeExtendConfig.m_measuredSignal = MeasuringSignalSel::NO_COMMAND_ACTIVE;
      return probe.probeExtConfig(probeIdx, probeExtendConfig, true);
    };
  testCase6.m_setDataState = 0x0000;
  testCase6.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase6);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};

    // set sharing handler for mapping
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
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

HRESULT TestSercosProbe::TestProbeStatusCtrl()
{
  log(L"\n<b>Test probe status control.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosProbeDrive& probe)> m_testFun; // test function
    std::uint16_t m_setDataState{};                  // set of datalayer value
    AxisProfileResult m_expectedResult;         // expected return value
  };

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  std::vector<ProbeTestCase> testCase {
    {L"Enable probe normal case",           [](sercos::SercosProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 0; probe.m_probeParam.m_probe2Enable = 0; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, true);},         0x0000, AxisProfileResult::STS_OK},
    {L"Enable probe2 and probe1 activated", [](sercos::SercosProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 0; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_2, true);},         0x0000, AxisProfileResult::STS_OK},
    {L"Disable probe1 and probe2 disabled", [](sercos::SercosProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 0; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, true);},         0x0000, AxisProfileResult::STS_OK},
    {L"Disable probe2 and probe1 enabled",  [](sercos::SercosProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, false);},        0x0000, AxisProfileResult::STS_OK},
    {L"Invalid probe",                      [](sercos::SercosProbeDrive& probe){probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_UNDEFINED, true);}, 0x0000, AxisProfileResult::ERR_INVALID_ARGUMENT},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
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

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);


  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};
    // set sharing handler for mapping
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
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

HRESULT TestSercosProbe::TestOnlineChangeParams()
{
  log(L"\n<b>Test probe online change parameters.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosProbeDrive& probe)> m_testFun; // test function
    std::uint16_t m_setDataState {0};                    // set of datalayer value
    AxisProfileResult m_expectedResult {AxisProfileResult::STS_OK};         // expected return value
  };

  std::vector<ProbeTestCase> testCase;
  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockScaling mockCategory;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  ProbeTestCase testCase1;
  testCase1.m_testName = L"online change probe 1 without time measurement.";
  testCase1.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeUpdateMeasurement updateValue{};
    updateValue.m_isExpWindowActivated =true;
    return probe.onlineChangeParams(ProbeIndex::PROBE_1, updateValue);
  };
  testCase1.m_setDataState = 0x0000;
  testCase1.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase1);

  ProbeTestCase testCase2;
  testCase2.m_testName = L"online change probe 1 with time measurement.";
  testCase2.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeUpdateMeasurement updateValue{};
    probe.m_probeBitProcess.m_extendedCtrlWord.m_bits.bit2 = true;
    return probe.onlineChangeParams(ProbeIndex::PROBE_1, updateValue);
  };
  testCase2.m_setDataState = 0x0000;
  testCase2.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase2);

  ProbeTestCase testCase3;
  testCase3.m_testName = L"online change probe 2 without time measurement.";
  testCase3.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeUpdateMeasurement updateValue{};
    updateValue.m_isExpWindowActivated = true;
    return probe.onlineChangeParams(ProbeIndex::PROBE_2, updateValue);
  };
  testCase3.m_setDataState = 0x0000;
  testCase3.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase3);

  ProbeTestCase testCase4;
  testCase4.m_testName = L"online change probe 2 with time measurement.";
  testCase4.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeUpdateMeasurement updateValue{};
    probe.m_probeBitProcess.m_extendedCtrlWord.m_bits.bit6 = true;
    return probe.onlineChangeParams(ProbeIndex::PROBE_2, updateValue);
  };
  testCase4.m_setDataState = 0x0000;
  testCase4.m_expectedResult = AxisProfileResult::STS_OK;
  testCase.emplace_back(testCase4);

  ProbeTestCase testCase5;
  testCase5.m_testName = L"online change probe undefine.";
  testCase5.m_testFun = [](sercos::SercosProbeDrive& probe)
  {
    ProbeUpdateMeasurement updateValue{};
    probe.m_timeMeasure = false;
    probe.m_probeParam.m_isExpWindowActivated =true;
    return probe.onlineChangeParams(ProbeIndex::PROBE_UNDEFINED, updateValue);
  };
  testCase5.m_setDataState = 0x0000;
  testCase5.m_expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  testCase.emplace_back(testCase5);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());

    sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
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

HRESULT TestSercosProbe::TestGetProbeValues()
{
  log(L"\n<b>Test get probe value.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosProbeDrive& probe)> m_testFun; // test function
    std::uint16_t m_setDataState{};                  // set of datalayer value
    AxisProfileResult m_expectedResult;         // expected return value
  };

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  std::vector<ProbeTestCase> testCase {
    {L"Get probe 1 values.",      [] (sercos::SercosProbeDrive& probe){ProbeBaseConfig probeConfig{}; ProbeExtConfig probeExtendConfig; return probe.getProbeValues(ProbeIndex::PROBE_1, &probeConfig, &probeExtendConfig);},        0x0000, AxisProfileResult::STS_OK},
    {L"Get probe 2 values.",      [] (sercos::SercosProbeDrive& probe){ProbeBaseConfig probeConfig{}; ProbeExtConfig probeExtendConfig; return probe.getProbeValues(ProbeIndex::PROBE_2, &probeConfig, &probeExtendConfig);},        0x0000, AxisProfileResult::STS_OK},
    {L"Get values but undefine.", [] (sercos::SercosProbeDrive& probe){ProbeBaseConfig probeConfig{}; ProbeExtConfig probeExtendConfig; return probe.getProbeValues(ProbeIndex::PROBE_UNDEFINED, &probeConfig, &probeExtendConfig);}, 0x0000, AxisProfileResult::ERR_INVALID_ARGUMENT},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
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

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
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

HRESULT TestSercosProbe::TestGetProbeValuesRT()
{
  log(L"\n<b>Test get probe real time values.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosProbeDrive& probe)> m_testFun; // test function
    std::uint16_t commandIDN{};        // measured signal
    AxisProfileResult m_expectedResult;         // expected return value
  };

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  std::vector<ProbeTestCase> testCase {
    {L"Get probe1 actual values with position.",      [] (sercos::SercosProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_1, &actualValueProbe);}, 51,       AxisProfileResult::STS_OK},
    {L"Get probe1 actual values with fien time.",     [] (sercos::SercosProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_1, &actualValueProbe);}, 35523,    AxisProfileResult::STS_OK},
    {L"Get probe2 actual values with position.",      [] (sercos::SercosProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_2, &actualValueProbe);}, 53,       AxisProfileResult::STS_OK},
    {L"Get probe2 actual values with fien time.",     [] (sercos::SercosProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_2, &actualValueProbe);}, 35523,    AxisProfileResult::STS_OK},
    {L"Get probe actual values but probe undefine.",  [] (sercos::SercosProbeDrive& probe){ProbeActualValues actualValueProbe; return probe.getProbeActualValuesRT(ProbeIndex::PROBE_UNDEFINED, &actualValueProbe);}, 0, AxisProfileResult::ERR_INVALID_ARGUMENT},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
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

  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    testProbe.m_probeParam.m_selectedSignal = oneTest.commandIDN;
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

HRESULT TestSercosProbe::TestProbeCmdFailed()
{
  log(L"\n<b>Test probe commands failed situations.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosProbeDrive& probe)> m_testFun; // test function
    AxisProfileResult m_expectedResult;         // expected return value
  };

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  std::vector<ProbeTestCase> testCase {
    {L"Set probe control word failed.",           [](sercos::SercosProbeDrive& probe){ProbeBaseConfig probeConfig; return probe.probeBaseConfig(ProbeIndex::PROBE_1, probeConfig, false);},      AxisProfileResult::ERR_EXEC_NULL},
    {L"Set probe extended control word failed.",  [](sercos::SercosProbeDrive& probe){ProbeExtConfig probeConfig; return probe.probeExtConfig(ProbeIndex::PROBE_1, probeConfig, false);},        AxisProfileResult::ERR_EXEC_NULL},
    {L"Set probe status control failed.",         [](sercos::SercosProbeDrive& probe){return probe.probeStatusCtrl(ProbeIndex::PROBE_1, true);},                                                 AxisProfileResult::ERR_EXEC_NULL},
    {L"Update probe value failed.",               [](sercos::SercosProbeDrive& probe){ProbeUpdateMeasurement updateValues; return probe.onlineChangeParams(ProbeIndex::PROBE_1, updateValues);}, AxisProfileResult::ERR_EXEC_NULL},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  test::MockCategory mockCategory;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = 0;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};
    testProbe.resetProbeCommand();
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

  log(L"\n  Test error case of probe without sharing handler ...");
  sercos::SercosProbeDrive testProbe2 {profileBasicInfo, &atData};
  auto result = testProbe2.booting();
  if (result != AxisProfileResult::ERR_EXEC_NULL)
  {
    RETURN_TEST_RESULT(result, L"\n  unexpected return value.", AxisProfileResult::ERR_EXEC_NULL);
  }
  return S_OK;
}

HRESULT TestSercosProbe::TestMeasuredSignalConvert()
{
  log(L"\n<b>Test signal convert.</b>");
  HRESULT hResult = S_OK;
  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};

  log(L"\nTest case of POS_FEEDBACK_VAL_ENCODER_1...");
  MeasuringSignalSel expSignal = MeasuringSignalSel::POS_FEEDBACK_VAL_ENCODER_1;
  testProbe.m_probeParam.m_selectedSignal = 51;
  MeasuringSignalSel signal = testProbe.measuredSignalConvert();

  if (signal != expSignal)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expSignal, signal);
    hResult = E_FAIL;
  }

  log(L"\nTest case of POS_FEEDBACK_VAL_ENCODER_2...");
  expSignal = MeasuringSignalSel::POS_FEEDBACK_VAL_ENCODER_2;
  testProbe.m_probeParam.m_selectedSignal = 53;
  signal = testProbe.measuredSignalConvert();

  if (signal != expSignal)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expSignal, signal);
    hResult = E_FAIL;
  }

  log(L"\nTest case of SYSTEM_FINE_TIME...");
  expSignal = MeasuringSignalSel::SYSTEM_FINE_TIME;
  testProbe.m_probeParam.m_selectedSignal = 35523;
  signal = testProbe.measuredSignalConvert();

  if (signal != expSignal)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expSignal, signal);
    hResult = E_FAIL;
  }

  log(L"\nTest case of signal undefine...");
  expSignal = MeasuringSignalSel::SIGNAL_UNDEFINED;
  testProbe.m_probeParam.m_selectedSignal = 1;
  signal = testProbe.measuredSignalConvert();

  if (signal != expSignal)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expSignal, signal);
    hResult = E_FAIL;
  }

  return hResult;
}

HRESULT TestSercosProbe::TestisProbeSupported()
{
  log(L"\n<b>Test check if probe supported.</b>");
  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  test::MockScaling mockScaling;
  std::uint32_t devAddress = 1;
  AcknowledgeTelegram atData;
  std::uint8_t driveNumber = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = devAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  test::MockAxisProfileMappingHandler mappingHandlerMockup{};
  test::MockAxisProfileTrace profileTraceMockup{};

  sercos::SercosProbeDrive testProbe{profileBasicInfo, &atData};

  // set sharing handler for mapping
  auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&profileTraceMockup});
  IAxisProfileSharingHandler* ptrSharing = sharing.get();
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
  ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &mockScaling);

  AxisProfileResult expRes = AxisProfileResult::STS_OK;
  AxisProfileResult res = testProbe.isProbeSupported();
  if (res != expRes)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expRes, res);
    return E_FAIL;
  }

  return S_OK;
}

//////////////////////////////////////////////Helper Methods////////////////////////////////////////////////////////////

void TestSercosProbe::setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16 &data)
{
  flatbuffers::FlatBufferBuilder builder{};
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

HRESULT TestSercosProbe::TestProbeStatusCtrlSignalFunction()
{
  log(L"\n<b>Test probe status control.</b>");
  struct ProbeTestCase
  {
    std::wstring m_testName;                    // test name
    std::function<AxisProfileResult(sercos::SercosProbeDrive& probe)> m_testFun; // test function
    std::uint16_t m_setDataState{};                  // set of datalayer value
    AxisProfileResult m_expectedResult;         // expected return value
  };

  ////////////////////////////////////////Create Tests cases////////////////////////////////////////////////////////
  std::vector<ProbeTestCase> testCase {
    {L"Enable probe normal case", [] (sercos::SercosProbeDrive& probe)
    {
      probe.m_probeParam.m_probe1Enable = 0; probe.m_probeParam.m_probe2Enable = 0; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, true);
    }, 0x0000, AxisProfileResult::STS_OK},
    {L"Enable probe2 and probe1 activated", [] (sercos::SercosProbeDrive& probe)
    {
      probe.m_probeParam.m_probe1Enable = 0; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_2, true);
    }, 0x0000, AxisProfileResult::STS_OK},
    {L"Disable probe1 and probe2 disabled", [] (sercos::SercosProbeDrive& probe)
    {
      probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 0; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, true);
    }, 0x0000, AxisProfileResult::STS_OK},
    {L"Disable probe2 and probe1 enabled", [] (sercos::SercosProbeDrive& probe)
    {
      probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_1, false);
    }, 0x0000, AxisProfileResult::STS_OK},
    {L"Invalid probe", [] (sercos::SercosProbeDrive& probe)
    {
      probe.m_probeParam.m_probe1Enable = 1; probe.m_probeParam.m_probe2Enable = 1; return probe.probeStatusCtrl(ProbeIndex::PROBE_UNDEFINED, true);
    }, 0x0000, AxisProfileResult::ERR_INVALID_ARGUMENT},
  };

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  mappingHandlerMockup.getProfileAssignedInfo().m_input.m_signalStatusWord = true;
  mappingHandlerMockup.getProfileAssignedInfo().m_output.m_signalControlWord = true;
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

  ProbeSignalFuncDeviceParam mockDeviceParam;
  ProbeSignalFuncHandler handler {profileBasicInfo};
  handler.setProbeSignalsSupported(true);
  mockDeviceParam.setSignalFuncHandler(&handler);
  RecordingProbeCyclicData cyclicData;
  comm::axisprofile::sercos::SercosParamAccess sercosParamAccess(profileBasicInfo);


  //////////////////////////////////////////Run tests cases/////////////////////////////////////////////////////////
  for (auto& oneTest : testCase)
  {
    AxisProfileResult result = AxisProfileResult::STS_OK;
    log(L"\n  Test normal case of %s ...", oneTest.m_testName.c_str());
    sercos::SercosProbeDrive testProbe {profileBasicInfo, &atData};
    testProbe.m_isSignalFuncCmd = true;
    // set sharing handler for mapping
    auto sharing = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {&mockProfileTrace});
    IAxisProfileSharingHandler* ptrSharing = sharing.get();

    
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, &testProbe);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS, &sercosParamAccess);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &mockScaling);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategory);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &mockDeviceParam);
    ptrSharing->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &cyclicData);

    handler.init(ptrSharing);
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
