#include "profiles/canopen/canopen_profile.h"
#include "profiles/scaling/scaling_strategy_factory.h"
#include "profiles/sercos/sercos_defs.h"
#include "profiles/sercos/sercos_profile.h"
#include "test_scaling_factory.h"

// Mockups
#include "test/mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"
#include "test/mockup/external_bundles/mock_datalayer_client.h"
#include "test/mockup/mock_fieldbus_adapter_coe.h"
#include "test/mockup/mock_fieldbus_adapter_soe.h"
#include "test/mockup/mock_profile_trace.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestScalingFactory);
REGISTER_TEST_TO_GROUP(TestScalingFactory, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestScalingFactory, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestScalingFactory, Profiles_Scaling);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestScalingFactory::TestScalingFactory()
{
}


TestScalingFactory::~TestScalingFactory()
{}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestScalingFactory::doInit()
{
  return S_OK;
}

HRESULT TestScalingFactory::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the ScalingFactory class");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testForCheckCompatible();
  hResult |= testForCreateStrategy();
  hResult |= testForGetterSetter();
  return hResult;
}

HRESULT TestScalingFactory::doEnd()
{
  return S_OK;
}

HRESULT TestScalingFactory::testForCheckCompatible()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test of check scaling compatiblity.</b>");
  sercos::SercosProfile soeprofile{ m_profileName, comm::axisprofile::NetworkTechnology::ETHERCAT };
  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapterSoE{};
  test::MockDatalayerClient mockClientSoE{};
  mockFieldbusAdapterSoE.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClientSoE));

  ScalingStrategyFactory::checkCompatiblityForStrategy(&mockProfileTrace, AxisProfileScalingStrategy::SOE_STANDARD, comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT);  //for code coverage
  ScalingStrategyFactory::checkCompatiblityForStrategy(&mockProfileTrace, AxisProfileScalingStrategy::COE_CALC_CONTROL, comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT);

  //for CoE
  canopen::CanopenProfile profileCoE{ m_profileName, comm::axisprofile::NetworkTechnology::ETHERCAT };
  test::MockFieldbusAdapterCoE mockFieldbusAdapterCoE{};
  test::MockDatalayerClient mockClientCoE{};
  test::MockEthercatMasterSDORequestNodeCoE sdoRequestNodeMockCoE{};
  mockFieldbusAdapterCoE.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClientCoE));

  ScalingStrategyFactory::checkCompatiblityForStrategy(&mockProfileTrace, AxisProfileScalingStrategy::SOE_STANDARD, comm::axisprofile::AxisProfileType::CANOPEN_OVER_ETHERCAT);  //for code coverage
  ScalingStrategyFactory::checkCompatiblityForStrategy(&mockProfileTrace, AxisProfileScalingStrategy::COE_CALC_CONTROL, comm::axisprofile::AxisProfileType::CANOPEN_OVER_ETHERCAT);
  ScalingStrategyFactory::checkCompatiblityForStrategy(&mockProfileTrace, AxisProfileScalingStrategy::UNKNOWN_STRATEGY, AxisProfileType::UNDEFINED);

  return hResult;
}

HRESULT TestScalingFactory::testForCreateStrategy()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test of check scaling strategy creation.</b>");
  sercos::SercosProfile soeprofile{ m_profileName, comm::axisprofile::NetworkTechnology::ETHERCAT };
  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  std::uint32_t deviceAddress = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;

  IScaling* pScalingSoE = ScalingStrategyFactory::createScalingStrategy(profileBasicInfo, AxisProfileScalingStrategy::SOE_STANDARD);
  delete pScalingSoE;
  pScalingSoE = nullptr;
  IScaling* pScalingCoEObj = ScalingStrategyFactory::createScalingStrategy(profileBasicInfo, AxisProfileScalingStrategy::COE_CALC_DRIVE);
  delete pScalingCoEObj;
  pScalingCoEObj = nullptr;
  IScaling* pScalingCoEEleGear = ScalingStrategyFactory::createScalingStrategy(profileBasicInfo, AxisProfileScalingStrategy::COE_CALC_CONTROL);
  delete pScalingCoEEleGear;
  pScalingCoEEleGear = nullptr;
  ScalingStrategyFactory::createScalingStrategy(profileBasicInfo, AxisProfileScalingStrategy::UNKNOWN_STRATEGY);
  return hResult;
}

HRESULT TestScalingFactory::testForGetterSetter()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test of check scaling getter.</b>");
  sercos::SercosProfile soeprofile{ m_profileName, comm::axisprofile::NetworkTechnology::ETHERCAT };
  AxisProfileScalingStrategyCfg actualCfg = soeprofile.getProfileScalingStrategyCfg();
  AxisProfileScalingStrategyCfg expCfg;
  expCfg.m_scalingStrategy = AxisProfileScalingStrategy::SOE_STANDARD;
  if (expCfg.m_denominator != actualCfg.m_denominator ||
      expCfg.m_deviceScalingType != actualCfg.m_deviceScalingType||
      expCfg.m_numerator != actualCfg.m_numerator||
      expCfg.m_resolution != actualCfg.m_resolution||
      expCfg.m_scalingStrategy != actualCfg.m_scalingStrategy)
  {
    return E_FAIL;
  }
  return hResult;
}
