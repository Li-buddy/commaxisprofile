#include "profiles/canopen/drive/canopen_scaling_strategy_control.h"
#include "profiles/category/drive/drive_category_adapter_coe.h"
#include "test_canopen_scaling_strategy_control.h"

using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

REGISTER_TEST(TestCanopenScalingStrategyControl);
REGISTER_TEST_TO_GROUP(TestCanopenScalingStrategyControl, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenScalingStrategyControl, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenScalingStrategyControl, Profiles_Canopen);

TestCanopenScalingStrategyControl::TestCanopenScalingStrategyControl()
{
  //prepare for test object  
  log(L"\n<b>create test object.</b>");
  std::uint32_t devAddress = 1;
  m_mockClient = std::make_shared< test::MockDatalayerClient>();
  m_mockFieldbusAdapter = std::make_shared<test::MockFieldbusAdapterCoE>();

  m_mockFieldbusAdapter->setDatalayer(static_cast<comm::datalayer::IClient*>(m_mockClient.get()));
  m_mockFieldbusAdapter->m_returnValue = AxisProfileResult::STS_OK;
  m_sdoRequestNodeMockCoE = std::make_shared<test::MockEthercatMasterSDORequestNodeCoE>();
  m_mockClient->m_testNode = m_sdoRequestNodeMockCoE.get();
  
  m_profileBasicInfo.m_fieldbusAdapter = m_mockFieldbusAdapter.get();
  m_profileBasicInfo.m_deviceAddress = devAddress;
  m_testObject = std::make_unique<ScalingStrategyCoEControl>(ScalingStrategyCoEControl{m_profileBasicInfo});
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  m_mockCategory = std::make_unique<DriveCategoryAdapterCoE>(m_profileBasicInfo, &atData, &mdtData);

  m_psharingHandler = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {m_mockProfileTrace.get()});
  m_psharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_mockCategory.get());
  m_psharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, m_testObject.get());
}

HRESULT TestCanopenScalingStrategyControl::doInit()
{
  HRESULT tResult = S_OK;
  //create test data for position linear
  AxisProfileScalingStrategyCfg testCfgForPosLinear{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                                     AxisProfileDeviceScalingType::LINEAR, std::uint32_t(8388608), std::uint32_t(10000), std::uint32_t(8388608) };

  //create test data for position rotary
  AxisProfileScalingStrategyCfg testCfgForPosRotary{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                                     AxisProfileDeviceScalingType::ROTATIONAL, std::uint32_t(8388608), std::uint32_t(10000), std::uint32_t(8388608) };

  //create test data for position unknown
  AxisProfileScalingStrategyCfg testCfgForPosUnknown{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                                     AxisProfileDeviceScalingType::UNDEFINED, std::uint32_t(8388608), std::uint32_t(10000), std::uint32_t(8388608) };
  //create test data for velocity
  AxisProfileScalingStrategyCfg testCfgForVel{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                               AxisProfileDeviceScalingType::LINEAR, std::uint32_t(8388608), std::uint32_t(10000), std::uint32_t(8388608) };

  AxisProfileScalingStrategyCfg testCfgForErrorVel{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                                    AxisProfileDeviceScalingType::LINEAR, std::uint32_t(8388608), std::uint32_t(0), std::uint32_t(8388608) };

  //create test data for special velocity
  AxisProfileScalingStrategyCfg testCfgForSpecialLinearVel{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                                            AxisProfileDeviceScalingType::LINEAR, std::uint32_t(8388608), std::uint32_t(10000), std::uint32_t(8388608), ScalingFactor{true, 8388608, 10000} };

  AxisProfileScalingStrategyCfg testCfgForSpecialRotaryVel{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                                            AxisProfileDeviceScalingType::ROTATIONAL, std::uint32_t(10000), std::uint32_t(10000), std::uint32_t(262144), ScalingFactor{true, 1, 262144} };

  // test for invalid special velocity scaling settings
  AxisProfileScalingStrategyCfg errorInvalidSpecialVelCfg{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                                           AxisProfileDeviceScalingType::ROTATIONAL, std::uint32_t(10000), std::uint32_t(10000), std::uint32_t(262144), ScalingFactor{true, 0, 262144} };

  AxisProfileScalingStrategyCfg errorAxisTypeCfg { AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                                   AxisProfileDeviceScalingType::PERCENTAGE, std::uint32_t(10000), std::uint32_t(10000), std::uint32_t(262144), ScalingFactor{true, 1, 262144} };

  //create test data for acceleration
  AxisProfileScalingStrategyCfg testCfgForAcc{ AxisProfileScalingStrategy::COE_CALC_CONTROL,
                                               AxisProfileDeviceScalingType::LINEAR, std::uint32_t(8388608), std::uint32_t(10000), std::uint32_t(8388608) };

  m_vecTestData.reserve(10);
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcPosScalingLSBWeight, this, std::placeholders::_1),
                                        testCfgForPosLinear,AxisProfileResult::STS_OK,L"test For Linear Position Type" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcPosScalingLSBWeight, this, std::placeholders::_1),
                                        testCfgForPosRotary,AxisProfileResult::STS_OK,L"test For Rotary Position Type" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcPosScalingLSBWeight, this, std::placeholders::_1),
                                        testCfgForPosUnknown,AxisProfileResult::ERR_INVALID_ARGUMENT,L"test For Unknown Position Type" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                        testCfgForVel,AxisProfileResult::STS_OK,L"test For velocity" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                        testCfgForErrorVel,AxisProfileResult::ERR_INVALID_ARGUMENT,L"test For velocity" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                        testCfgForSpecialLinearVel,AxisProfileResult::STS_OK,L"test For special linear velocity" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                        testCfgForSpecialRotaryVel,AxisProfileResult::STS_OK,L"test For special rotary velocity" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                        errorInvalidSpecialVelCfg,AxisProfileResult::ERR_INVALID_ARGUMENT,L"test For invalid special velocity scaling settings" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                        errorAxisTypeCfg,AxisProfileResult::ERR_INVALID_ARGUMENT,L"test For error axis type" }));
  m_vecTestData.emplace_back(TestData({ std::bind(&TestCanopenScalingStrategyControl::testCalcAccScalingLSBWeight, this, std::placeholders::_1),
                                        testCfgForAcc,AxisProfileResult::STS_OK,L"test For acceleration" }));

  return tResult;
}

HRESULT TestCanopenScalingStrategyControl::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the CoE Scaling Control Strategy");
  log(L"\n----------------------------------------------------------------\n");

  for (const auto& item : m_vecTestData)
  {
    item.m_testFunctional(&item);
  }

  hResult |= testLoadLSBWeightsMethods();
  hResult |= testCalcScalingLSBWeights();
  hResult |= testCalcTorqueScalingLSBWeight();
  hResult |= testFor_setter();
  hResult |= testFor_supportAxis();
  log(L"\n");
  return hResult;
}

HRESULT TestCanopenScalingStrategyControl::doEnd()
{
  return S_OK;
}

HRESULT TestCanopenScalingStrategyControl::testCalcScalingLSBWeights()
{
  HRESULT tResult = S_OK;
  m_testObject->calcScalingLSBWeights();
  return tResult;
}

HRESULT TestCanopenScalingStrategyControl::testCalcPosScalingLSBWeight(const TestData* testData)
{
  HRESULT tResult = S_OK;
  m_testObject->m_scalingStrategyCfg = testData->m_scalingStrategyCfg;

  AxisProfileResult result = m_testObject->calcPosScalingLSBWeight();
  if (result != testData->m_expResult)
  {
    error(L"\n\t %ls test fails! Actual: %d, Expected: %d", testData->m_testName, result, testData->m_expResult);
    tResult = E_FAIL;
  }
  else
  {
    log(L"\n\t %ls test... <DARK_GREEN>OK</DARK_GREEN>", testData->m_testName);
  }
  return tResult;
}

HRESULT TestCanopenScalingStrategyControl::testCalcVelScalingLSBWeight(const TestData* testData)
{
  HRESULT tResult = S_OK;
  m_testObject->m_scalingStrategyCfg = testData->m_scalingStrategyCfg;

  AxisProfileResult result = m_testObject->calcVelScalingLSBWeight();
  if (result != testData->m_expResult)
  {
    error(L"\n\t %ls test fails! Actual: %d, Expected: %d", testData->m_testName, result, testData->m_expResult);
    tResult = E_FAIL;
  }
  else
  {
    log(L"\n\t %ls test... <DARK_GREEN>OK</DARK_GREEN>", testData->m_testName);
  }
  return tResult;
}

HRESULT TestCanopenScalingStrategyControl::testCalcAccScalingLSBWeight(const TestData* testData)
{
  HRESULT tResult = S_OK;
  m_testObject->m_scalingStrategyCfg = testData->m_scalingStrategyCfg;

  AxisProfileResult result = m_testObject->calcAccScalingLSBWeight();
  if (result != testData->m_expResult)
  {
    error(L"\n\t %ls test fails! Actual: %d, Expected: %d", testData->m_testName, result, testData->m_expResult);
    tResult = E_FAIL;
  }
  else
  {
    log(L"\n\t %ls test... <DARK_GREEN>OK</DARK_GREEN>", testData->m_testName);
  }
  return tResult;
}

HRESULT TestCanopenScalingStrategyControl::testCalcTorqueScalingLSBWeight()
{
  HRESULT tResult = S_OK;
  m_testObject->calcAccScalingLSBWeight();
  return tResult;
}

HRESULT TestCanopenScalingStrategyControl::testLoadLSBWeightsMethods()
{
  HRESULT tResult = S_OK;
  m_testObject->loadBootingTasks();
  return tResult;
}

HRESULT TestCanopenScalingStrategyControl::testFor_setter()
{
  HRESULT tResult = S_OK;
  m_testObject->setDeviceScalingType(AxisProfileDeviceScalingType::LINEAR);
  m_testObject->setDeviceScalingType(AxisProfileDeviceScalingType::ROTATIONAL);
  m_testObject->setEncoderResolution(8388608);
  m_testObject->setScalingDenominator(8388608);
  m_testObject->setScalingNumerator(10000);
  return tResult;
}

HRESULT TestCanopenScalingStrategyControl::testFor_supportAxis()
{
  HRESULT tResult = S_OK;
  double modValue = 360;
  std::int32_t rawValue = 10000;
  double scaledValue = 1;

  const wchar_t* name = L"test For support absolute axis";
  m_testObject->convertPositionRawToScaled(rawValue);
  m_testObject->convertPositionScaledToRaw(scaledValue);

  scaledValue = static_cast<double>(INT64_MAX - 100);
  auto value = m_testObject->convertPositionScaledToRaw(scaledValue);
  if (value != INT32_MAX)
  {
    error(L"\n\t %ls test fails! Actual: %d, Expected: %d", name, value, INT32_MAX);
    tResult = E_FAIL;
  }
  else
  {
    log(L"\n\t %ls test... <DARK_GREEN>OK</DARK_GREEN>", name);
  }

  scaledValue = static_cast<double>(INT64_MIN + 100);
  value = m_testObject->convertPositionScaledToRaw(scaledValue);
  if (value != INT32_MIN)
  {
    error(L"\n\t %ls test fails! Actual: %d, Expected: %d", name, value, INT32_MIN);
    tResult = E_FAIL;
  }
  else
  {
    log(L"\n\t %ls test... <DARK_GREEN>OK</DARK_GREEN>", name);
  }
  name = L"test For support modulo axis";
  std::string errTxt {};
  auto result = m_testObject->checkAndSetModulo(modValue, errTxt);
  m_testObject->convertPositionRawToScaled(rawValue);
  m_testObject->convertPositionScaledToRaw(scaledValue);
  m_testObject->initModuloPosCmp(rawValue);
  if (result != AxisProfileResult::STS_OK)
  {
    error(L"\n\t %ls test fails! Actual: %d, Expected: %d", name, result, AxisProfileResult::STS_OK);
    tResult = E_FAIL;
  }
  else
  {
    log(L"\n\t %ls test... <DARK_GREEN>OK</DARK_GREEN>", name);
  }

  return tResult;
}


