#include "test_axisprofile_probe_encoder.h"
#include "helper/result_helper.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileProbeEncoder);
REGISTER_TEST_TO_GROUP(TestAxisProfileProbeEncoder, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileProbeEncoder, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileProbeEncoder, 4_EncoderTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestAxisProfileProbeEncoder::doInit()
{
  HRESULT res = S_OK;

  log(L"\nInit test data member...");
  m_profileInfo = std::make_shared<ProfileBasicInfo>();
  m_category = std::make_unique<EncoderCategoryAdapterCoE>(*m_profileInfo.get(), m_telegramAT.get(), m_telegramMDT.get());
  m_sharingHandler = std::make_unique<AxisProfileSharingHandler>(&m_profileInfo->m_profileTrace);
  m_category->registerSharingITFs(m_sharingHandler.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_category.get());

  return res;
}

HRESULT TestAxisProfileProbeEncoder::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test all the TestAxisProfileProbeEncoder interface");
  log(L"\n----------------------------------------------------------------\n");
  log(L"\n\t Test of abortAndResetProbe...");
  auto result = m_category->getProbeInternal()->abortAndResetProbe();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of booting...");
  result = m_category->getProbeInternal()->booting();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of getCInterface...");
  m_category->getProbeInternal()->getCInterface();

  log(L"\n\t Test of getProbeActualValuesRT...");
  ProbeIndex probeIndex {};
  ProbeActualValues values {};
  result = m_category->getProbeInternal()->getProbeActualValuesRT(probeIndex, &values);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getProbeValues...");
  ProbeBaseConfig baseCfg {};
  ProbeExtConfig extCfg {};
  result = m_category->getProbeInternal()->getProbeValues(probeIndex, &baseCfg, &extCfg);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of isProbeSupported...");
  result = m_category->getProbeInternal()->isProbeSupported();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of loadBootingTasks...");
  m_category->getProbeInternal()->loadBootingTasks();

  log(L"\n\t Test of onlineChangeParams...");
  ProbeUpdateMeasurement updateMeasure {};
  result = m_category->getProbeInternal()->onlineChangeParams(probeIndex, updateMeasure);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of probeBaseConfig...");
  result = m_category->getProbeInternal()->probeBaseConfig(probeIndex, baseCfg, true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of probeExtConfig...");
  result = m_category->getProbeInternal()->probeExtConfig(probeIndex, extCfg, true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of probeStatusCtrl...");
  result = m_category->getProbeInternal()->probeStatusCtrl(probeIndex, true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);
  return res;
}


