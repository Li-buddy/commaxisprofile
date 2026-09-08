#include "test_canopen_profile.h"
#include "helper/result_helper.h"

// Mockups
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestCanopenProfile);
REGISTER_TEST_TO_GROUP(TestCanopenProfile, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenProfile, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenProfile, Profiles_Canopen);


HRESULT TestCanopenProfile::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the CiA402 profile command class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;

  hresult |= testForSetScalingStrategy();
  hresult |= testProcessDataAtCopiesHomedStatusAsSingleBit();
  log(L"\n");
  return hresult;
}

 HRESULT TestCanopenProfile::testForSetScalingStrategy()
 {
   HRESULT hresult = S_OK;
   canopen::CanopenProfile profile{ m_profileName, comm::axisprofile::NetworkTechnology::ETHERCAT };
   test::MockAxisProfileTrace mockProfileTrace{};
   test::MockFieldbusAdapterCoE mockFieldbusAdapter{};
   test::MockDatalayerClient mockClient{};
   test::MockEthercatMasterSDORequestNodeCoE sdoRequestNodeMockCoE{};
   mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
   mockClient.m_testNode = &sdoRequestNodeMockCoE;
   mockFieldbusAdapter.m_returnValue = AxisProfileResult::STS_OK;

   AxisProfileScalingStrategyCfg scalingCfgCoEUnitObject{};
   scalingCfgCoEUnitObject.m_scalingStrategy = AxisProfileScalingStrategy::COE_CALC_DRIVE;
   profile.setProfileScalingStrategyCfg(scalingCfgCoEUnitObject);

   AxisProfileScalingStrategyCfg scalingCfgCoEEleGear{};
   scalingCfgCoEEleGear.m_scalingStrategy = AxisProfileScalingStrategy::COE_CALC_CONTROL;
   profile.setProfileScalingStrategyCfg(scalingCfgCoEEleGear);

   AxisProfileScalingStrategyCfg scalingCfgCoEWrong{};
   scalingCfgCoEWrong.m_scalingStrategy = AxisProfileScalingStrategy::SOE_STANDARD;
   profile.setProfileScalingStrategyCfg(scalingCfgCoEWrong);

   AxisProfileScalingStrategyCfg scalingCfgCoEUnknown{};
   scalingCfgCoEUnknown.m_scalingStrategy = AxisProfileScalingStrategy::UNKNOWN_STRATEGY;
   profile.setProfileScalingStrategyCfg(scalingCfgCoEUnknown);

   profile.getProfileScalingStrategyCfg();
   //test for base
   AxisProfileScalingStrategyCfg cfg = profile.AxisProfile::getProfileScalingStrategyCfg();
   profile.AxisProfile::setProfileScalingStrategyCfg(cfg);

   return hresult;
 }

HRESULT TestCanopenProfile::testProcessDataAtCopiesHomedStatusAsSingleBit()
{
  log(L"\n<b>Test HOMED_STATUS process data copy.</b>");
  canopen::CanopenProfile profile{m_profileName, comm::axisprofile::NetworkTechnology::ETHERCAT};
  MockAxisProfileMappingHandler mappingHandler{};
  mappingHandler.setInputRealtimeInfo({InputMappingInfoRT{0, 16, InputMappingValueID::HOMED_STATUS}});
  IAxisProfileMappingHandler* const originalMappingHandler = profile.m_mappingHandler;
  profile.m_mappingHandler = &mappingHandler;

  std::uint8_t dataByteArray[2] {0xFE, 0xFF};
  profile.m_telegramAT.m_bitStatusHomed.m_byte = 0xFF;
  profile.m_telegramMDT.m_controlWord.m_word = 0x55AA;

  const AxisProfileResult result = profile.processDataAt(dataByteArray);
  const bool homedStatusIsBit0 = profile.m_telegramAT.m_bitStatusHomed.m_byte == 0U;
  const bool controlWordUnchanged = profile.m_telegramMDT.m_controlWord.m_word == 0x55AA;
  profile.m_mappingHandler = originalMappingHandler;

  RETURN_TEST_RESULT(result, L"\nUnexpected processDataAt result.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_ANY(homedStatusIsBit0, L"\nHomed status should only reflect bit 0 of the source value.");
  RETURN_TEST_RESULT_ANY(controlWordUnchanged, L"\nHomed status input copy must not overwrite the MDT controlword.");
  return S_OK;
}
