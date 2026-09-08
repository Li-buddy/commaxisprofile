#include "profiles/data_sharing/sharing_handler.h"
#include "test_category_adapter_drive.h"
#include "profiles/category/category_adapter_factory.h"
#include "helper/result_helper.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_soe.h"

REGISTER_TEST(TestCategoryAdapterDrive);
REGISTER_TEST_TO_GROUP(TestCategoryAdapterDrive, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCategoryAdapterDrive, 2_UnitTest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestCategoryAdapterDrive::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the DriveCategoryAdapterSoE");
  log(L"\n----------------------------------------------------------------\n");

  res |= testInterface();
  res |= testClass();

  return res;
}

HRESULT TestCategoryAdapterDrive::testInterface()
{
  m_cmdMock = new test::MockAxisProfileCommand();
  m_statusControlMock = new test::MockAxisProfileStatusControl();
  m_diagMsgMock = new test::MockAxisProfileDiagMsg();
  m_deviceParamMock = new test::MockAxisProfileDeviceParam();
  m_probeMock = new test::MockAxisProfileProbe();

  HRESULT hResult = S_OK;
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;

  DriveCategoryAdapterSoE testCategoryAdapterDrive(profileBasicInfo, &atData, &mdtData);
  // just for coverage
  testCategoryAdapterDrive.updateScalingStrategy(AxisProfileScalingStrategy::COE_CALC_DRIVE);
  
  delete m_cmdMock;
  m_cmdMock = nullptr;
  delete m_statusControlMock;
  m_statusControlMock = nullptr;
  delete m_diagMsgMock;
  m_diagMsgMock = nullptr;
  delete m_deviceParamMock;
  m_deviceParamMock = nullptr;
  delete m_probeMock;
  m_probeMock = nullptr;

  log(L"\n");
  return hResult;
}

HRESULT TestCategoryAdapterDrive::testClass()
{
  HRESULT result = S_OK;
  AcknowledgeTelegram  atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;
  profileBasicInfo.m_axisProfileType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  auto adapterPtr = CategoryAdapterFactory::createCategoryAdapter(profileBasicInfo, &atData, &mdtData, AxisProfileCategory::CATEGORY_DRIVE);
  LOG_TEST_RESULT(adapterPtr->getProfileCategory(), L"\nGet an unexpected result!", AxisProfileCategory::CATEGORY_DRIVE);

  delete adapterPtr;
  adapterPtr = nullptr;

  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
  adapterPtr = CategoryAdapterFactory::createCategoryAdapter(profileBasicInfo, &atData, &mdtData, AxisProfileCategory::CATEGORY_DRIVE);
  LOG_TEST_RESULT(adapterPtr->getProfileCategory(), L"\nGet an unexpected result!", AxisProfileCategory::CATEGORY_DRIVE);

  AxisProfileSharingHandler handler {&profileBasicInfo.m_profileTrace};
  adapterPtr->registerSharingITFs(&handler);
  adapterPtr->removeSharingITFs();

  delete adapterPtr;
  adapterPtr = nullptr;

  log(L"\n");
  return result;
}
