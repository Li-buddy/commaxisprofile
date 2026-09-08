
#include "helper/result_helper.h"
#include "profiles/category/category_mapping_helper.h"
#include "test_category_mapping_helper.h"
#include <vector>

using namespace comm::axisprofile;

REGISTER_TEST(TestCategoryMappingHelper);
REGISTER_TEST_TO_GROUP(TestCategoryMappingHelper, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCategoryMappingHelper, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCategoryMappingHelper, Profiles);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestCategoryMappingHelper::doInit()
{
  HRESULT res = S_OK;

  log(L"\n<BLUE><b>Init test cases...</b></BLUE>");
  LOG_TEST_RESULT(res, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  return res;
}

HRESULT TestCategoryMappingHelper::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestCategoryMappingHelper");
  log(L"\n----------------------------------------------------------------\n");

  res |= testGetAvailableInputMappingItems();
  res |= testGetAvailableOutputMappingItems();

  return res;
}

HRESULT TestCategoryMappingHelper::doEnd()
{
  HRESULT res = S_OK;

  log(L"\n<BLUE><b>Deinit test cases...</b></BLUE>");
  LOG_TEST_RESULT(res, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  return res;
}

HRESULT TestCategoryMappingHelper::testGetAvailableInputMappingItems()
{
  HRESULT res = S_OK;
  log(L"\n<b>----- Test method getAvailableInputMappingItems() -----</b>");

  std::vector<InputMappingValueID> vecInputItems;

  log(L"\nTest category mapping helper CATEGORY_ENCODER.");
  AxisProfileResult aprResult = CategoryMappingHelper::getAvailableInputMappingItems(AxisProfileCategory::CATEGORY_ENCODER, vecInputItems);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  log(L"\nTest category mapping helper UNDEFINED.");
  aprResult = CategoryMappingHelper::getAvailableInputMappingItems(AxisProfileCategory::UNDEFINED, vecInputItems);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\n");
  return res;
}

HRESULT TestCategoryMappingHelper::testGetAvailableOutputMappingItems()
{
  HRESULT res = S_OK;
  log(L"\n<b>----- Test method getAvailableOutputMappingItems() -----</b>");

  std::vector<OutputMappingValueID> vecOutputItems;

  log(L"\nTest category mapping helper CATEGORY_ENCODER.");
  AxisProfileResult aprResult = CategoryMappingHelper::getAvailableOutputMappingItems(AxisProfileCategory::CATEGORY_ENCODER, vecOutputItems);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  log(L"\nTest category mapping helper UNDEFINED.");
  aprResult = CategoryMappingHelper::getAvailableOutputMappingItems(AxisProfileCategory::UNDEFINED, vecOutputItems);
  LOG_TEST_RESULT(aprResult, L"\nGet an unexpected result!", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\n");
  return res;
}
