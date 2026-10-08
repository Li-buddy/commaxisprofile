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
  LOG_TEST_RESULT_ANY(!m_category->getStatusControlInternal()->checkError(), L"\nUnexpected checkError value. Expected false.");

  log(L"\n\t Test of checkOperationMode...");
  AxisProfileOperationMode opMode { AxisProfileOperationMode::POSITIONING };
  LOG_TEST_RESULT_ANY(!m_category->getStatusControlInternal()->checkOperationMode(opMode),
                      L"\nUnexpected checkOperationMode value. Expected false.");

  log(L"\n\t Test of checkOperationModeSupported...");
  OpModeErrorInfo info {};
  LOG_TEST_RESULT_ANY(!m_category->getStatusControlInternal()->checkOperationModeSupported(opMode, info),
                      L"\nUnexpected checkOperationModeSupported value. Expected false.");

  log(L"\n\t Test of checkPowerMode...");
  AxisProfilePowerMode powerMode { AxisProfilePowerMode::POWER_ON };
  LOG_TEST_RESULT_ANY(!m_category->getStatusControlInternal()->checkPowerMode(powerMode),
                      L"\nUnexpected checkPowerMode value. Expected false.");

  log(L"\n\t Test of checkReadyToOperate...");
  LOG_TEST_RESULT_ANY(m_category->getStatusControlInternal()->checkReadyToOperate() == ReadyToOperate::UNDEF_OPERATION,
                      L"\nUnexpected checkReadyToOperate value.");

  log(L"\n\t Test of cyclicStandStillCheck...");
  LOG_TEST_RESULT_ANY(!m_category->getStatusControlInternal()->cyclicStandStillCheck(),
                      L"\nUnexpected cyclicStandStillCheck value. Expected false.");

  log(L"\n\t Test of deviceHasWarning...");
  LOG_TEST_RESULT_ANY(!m_category->getStatusControlInternal()->deviceHasWarning(),
                      L"\nUnexpected deviceHasWarning value. Expected false.");

  log(L"\n\t Test of getCInterface...");
  LOG_TEST_RESULT_ANY(m_category->getStatusControlInternal()->getCInterface(), L"\nUnexpected nullptr C interface.");

  log(L"\n\t Test of getDriveHaltMode...");
  bool isHalt {};
  result = m_category->getStatusControlInternal()->getDriveHaltMode(isHalt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getOperationMode...");
  LOG_TEST_RESULT_ANY(m_category->getStatusControlInternal()->getOperationMode() == AxisProfileOperationMode::UNDEFINED,
                      L"\nUnexpected getOperationMode value.");

  log(L"\n\t Test of getPowerMode...");
  LOG_TEST_RESULT_ANY(m_category->getStatusControlInternal()->getPowerMode() == AxisProfilePowerMode::UNDEFINED,
                      L"\nUnexpected getPowerMode value.");

  log(L"\n\t Test of getStatusWord...");
  LOG_TEST_RESULT_ANY(m_category->getStatusControlInternal()->getStatusWord() == 0U,
                      L"\nUnexpected getStatusWord value. Expected 0.");

  log(L"\n\t Test of isDeviceFollowsControl...");
  LOG_TEST_RESULT_ANY(!m_category->getStatusControlInternal()->isDeviceFollowsControl(),
                      L"\nUnexpected isDeviceFollowsControl value. Expected false.");

  log(L"\n\t Test of setControlWord...");
  uint16_t controlWord {};
  result = m_category->getStatusControlInternal()->setControlWord(controlWord);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setDriveHaltMode...");
  result = m_category->getStatusControlInternal()->setDriveHaltMode(isHalt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getDriveHaltMode output behavior...");
  isHalt = true;
  result = m_category->getStatusControlInternal()->getDriveHaltMode(isHalt);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);
  if (!isHalt)
  {
    error(L"\ngetDriveHaltMode should not modify output argument for unsupported encoder profile.");
    res = E_FAIL;
  }

  log(L"\n\t Test of setOperationMode...");
  result = m_category->getStatusControlInternal()->setOperationMode(opMode);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setPowerMode...");
  result = m_category->getStatusControlInternal()->setPowerMode(powerMode);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  return res;
}
