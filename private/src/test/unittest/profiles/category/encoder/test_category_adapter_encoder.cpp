#include "profiles/category/category_adapter_factory.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "test_category_adapter_encoder.h"
#include "helper/result_helper.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_soe.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestCategoryAdapterEncoder);
REGISTER_TEST_TO_GROUP(TestCategoryAdapterEncoder, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCategoryAdapterEncoder, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCategoryAdapterEncoder, 4_EncoderTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestCategoryAdapterEncoder::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the CategoryAdapterEncoder");
  log(L"\n----------------------------------------------------------------\n");

  res |= testInterface();
  res |= testClass();

  return res;
}


HRESULT TestCategoryAdapterEncoder::testInterface()
{
  HRESULT hResult = S_OK;
  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;
  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;

  auto adapterPtr = CategoryAdapterFactory::createCategoryAdapter(profileBasicInfo, &atData, &mdtData, AxisProfileCategory::CATEGORY_ENCODER);
  std::unique_ptr<ICategoryAdapter> testCategoryAdapterEncoder(adapterPtr);
  log(L"\n<b>Test of the CategoryAdapterEncoder Interface</b>");  

  // coverage for factory
  adapterPtr = CategoryAdapterFactory::createCategoryAdapter(profileBasicInfo, &atData, &mdtData, AxisProfileCategory::UNDEFINED);
  LOG_TEST_RESULT_PTR(adapterPtr, L"\nGet an unexpected result!", nullptr);  

  profileBasicInfo.m_axisProfileType = AxisProfileType::UNDEFINED;
  adapterPtr = CategoryAdapterFactory::createCategoryAdapter(profileBasicInfo, &atData, &mdtData, AxisProfileCategory::CATEGORY_ENCODER);
  LOG_TEST_RESULT_PTR(adapterPtr, L"\nGet an unexpected result!", nullptr);

  profileBasicInfo.m_axisProfileType = AxisProfileType::UNDEFINED;
  adapterPtr = CategoryAdapterFactory::createCategoryAdapter(profileBasicInfo, &atData, &mdtData, AxisProfileCategory::CATEGORY_DRIVE);
  LOG_TEST_RESULT_PTR(adapterPtr, L"\nGet an unexpected result!", nullptr);

  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  adapterPtr = CategoryAdapterFactory::createCategoryAdapter(profileBasicInfo, &atData, &mdtData, AxisProfileCategory::CATEGORY_ENCODER);
  LOG_TEST_RESULT(adapterPtr->getProfileCategory(), L"\nGet an unexpected result!", AxisProfileCategory::CATEGORY_ENCODER);

  AxisProfileSharingHandler handler {&profileTraceMockup};
  adapterPtr->registerSharingITFs(&handler);
  adapterPtr->removeSharingITFs();
  adapterPtr->updateScalingStrategy(AxisProfileScalingStrategy::COE_CALC_CONTROL);
  std::vector<std::function<AxisProfileResult()>> methods;
  adapterPtr->loadInitMethod(methods);

  delete adapterPtr;
  adapterPtr = nullptr;

  log(L"\n");
  return hResult;
}

HRESULT TestCategoryAdapterEncoder::testClass()
{
  HRESULT result = S_OK;
  
  log(L"\n");
  ProfileBasicInfo info {};
  AcknowledgeTelegram at {};
  MasterDataTelegram mdt {};
  std::unique_ptr<EncoderCategoryAdapterCoE> encoderCategory = std::make_unique<EncoderCategoryAdapterCoE>(info, &at, &mdt);
  std::vector<std::function<AxisProfileResult()>> methods {};
  encoderCategory->loadInitMethod(methods);
  encoderCategory->getProfileCategory();
  encoderCategory->getStatemachine();
  ProfileTrace trace {};
  AxisProfileSharingHandler handler {&trace};
  encoderCategory->registerSharingITFs(&handler);
  encoderCategory->updateScalingStrategy(AxisProfileScalingStrategy::COE_CALC_DRIVE);

  return result;
}
