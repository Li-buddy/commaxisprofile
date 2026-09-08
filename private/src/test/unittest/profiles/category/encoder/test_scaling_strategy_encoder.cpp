#include "helper/result_helper.h"
#include "mockup/mock_axisprofile_property.h"
#include "profiles/category/encoder/scaling_strategy_encoder.h"
#include "test_scaling_strategy_encoder.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestScalingStrategyEncoder);
REGISTER_TEST_TO_GROUP(TestScalingStrategyEncoder, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestScalingStrategyEncoder, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestScalingStrategyEncoder, 4_EncoderTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestScalingStrategyEncoder::doInit()
{
  HRESULT res = S_OK;

  log(L"\nInit test data member...");
  m_profileInfo = std::make_shared<ProfileBasicInfo>();
  m_telegramMDT = std::make_shared<MasterDataTelegram>();
  m_telegramAT = std::make_shared<AcknowledgeTelegram>();
  m_category = std::make_unique<EncoderCategoryAdapterCoE>(*m_profileInfo.get(), m_telegramAT.get(), m_telegramMDT.get());
  m_mapping = std::make_shared<MockAxisProfileMappingHandler>();
  m_sharingHandler = std::make_unique<AxisProfileSharingHandler>(&m_profileInfo->m_profileTrace);
  m_category->registerSharingITFs(m_sharingHandler.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_category.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, m_mapping.get());
  return res;
}

HRESULT TestScalingStrategyEncoder::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test all the TestScalingStrategyEncoder interface");
  log(L"\n----------------------------------------------------------------\n");

  log(L"\n\t Test of booting...");
  test::MockAxisProfileProperty property {};
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  auto result = m_category->getScalingInternal()->booting();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  auto& assign = m_mapping->getProfileAssignedInfo();
  assign.m_input.m_velocity = true;
  result = m_category->getScalingInternal()->booting();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of calcScalingLSBWeights...");
  result = static_cast<ScalingStrategyEncoder*>(m_category->getScalingInternal())->calcScalingLSBWeights();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of calcPosScalingLSBWeight...");
  result = static_cast<ScalingStrategyEncoder*>(m_category->getScalingInternal())->calcPosScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of calcVelScalingLSBWeight...");
  result = static_cast<ScalingStrategyEncoder*>(m_category->getScalingInternal())->calcVelScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of calcAccScalingLSBWeight...");
  result = static_cast<ScalingStrategyEncoder*>(m_category->getScalingInternal())->calcAccScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of calcTorqueScalingLSBWeight...");
  result = static_cast<ScalingStrategyEncoder*>(m_category->getScalingInternal())->calcTorqueScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  m_category->removeSharingITFs();
  return res;
}

HRESULT TestScalingStrategyEncoder::doEnd()
{
  HRESULT res = S_OK;
  log(L"\nDeinit test data member...");
  m_category.reset();
  m_mapping.reset();
  m_sharingHandler.reset();
  m_telegramMDT.reset();
  m_telegramAT.reset();
  m_profileInfo.reset();
  return res;
}
