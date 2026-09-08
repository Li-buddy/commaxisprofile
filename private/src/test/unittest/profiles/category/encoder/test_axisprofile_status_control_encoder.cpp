#include "test_axisprofile_status_control_encoder.h"
#include "helper/result_helper.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileStatusControlEncoder);
REGISTER_TEST_TO_GROUP(TestAxisProfileStatusControlEncoder, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileStatusControlEncoder, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileStatusControlEncoder, 4_EncoderTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestAxisProfileStatusControlEncoder::doInit()
{
  HRESULT res = S_OK;

  log(L"\nInit test data member...");
  m_profileInfo = std::make_shared<ProfileBasicInfo>();
  m_telegramMDT = std::make_shared<MasterDataTelegram>();
  m_telegramAT = std::make_shared<AcknowledgeTelegram>();
  m_category = std::make_unique<EncoderCategoryAdapterCoE>(*m_profileInfo.get(), m_telegramAT.get(), m_telegramMDT.get());
  m_sharingHandler = std::make_unique<AxisProfileSharingHandler>(&m_profileInfo->m_profileTrace);
  m_category->registerSharingITFs(m_sharingHandler.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_category.get());

  return res;
}

HRESULT TestAxisProfileStatusControlEncoder::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test all the TestAxisProfileStatusControlEncoder interface");
  log(L"\n----------------------------------------------------------------\n");

  log(L"\n\t Test of analyzeStatusWord...");
  auto result = m_category->getStatusControlInternal()->analyzeStatusWord();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of booting...");
  result = m_category->getStatusControlInternal()->booting();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of checkError...");
  m_category->getStatusControlInternal()->checkError();

  log(L"\n\t Test of checkOperationMode...");
  AxisProfileOperationMode opMode {};
  m_category->getStatusControlInternal()->checkOperationMode(opMode);

  log(L"\n\t Test of checkOperationModeSupported...");
  OpModeErrorInfo info {};
  m_category->getStatusControlInternal()->checkOperationModeSupported(opMode, info);

  log(L"\n\t Test of checkPowerMode...");
  AxisProfilePowerMode powerMode {};
  m_category->getStatusControlInternal()->checkPowerMode(powerMode);

  log(L"\n\t Test of checkReadyToOperate...");
  m_category->getStatusControlInternal()->checkReadyToOperate();

  log(L"\n\t Test of cyclicStandStillCheck...");
  m_category->getStatusControlInternal()->cyclicStandStillCheck();

  log(L"\n\t Test of deviceHasWarning...");
  m_category->getStatusControlInternal()->deviceHasWarning();

  log(L"\n\t Test of getCInterface...");
  m_category->getStatusControlInternal()->getCInterface();

  log(L"\n\t Test of getDriveHaltMode...");
  bool isHalt {};
  result = m_category->getStatusControlInternal()->getDriveHaltMode(isHalt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getOperationMode...");
  m_category->getStatusControlInternal()->getOperationMode();

  log(L"\n\t Test of getPowerMode...");
  m_category->getStatusControlInternal()->getPowerMode();

  log(L"\n\t Test of getStatusWord...");
  m_category->getStatusControlInternal()->getStatusWord();

  log(L"\n\t Test of isDeviceFollowsControl...");
  m_category->getStatusControlInternal()->isDeviceFollowsControl();

  log(L"\n\t Test of setControlWord...");
  uint16_t controlWord {};
  result = m_category->getStatusControlInternal()->setControlWord(controlWord);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setDriveHaltMode...");
  result = m_category->getStatusControlInternal()->setDriveHaltMode(isHalt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setOperationMode...");
  result = m_category->getStatusControlInternal()->setOperationMode(opMode);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setPowerMode...");
  result = m_category->getStatusControlInternal()->setPowerMode(powerMode);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  return res;
}
