
#include "test_template.h"

#include "helper/result_helper.h"


using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestTemplateClass);
REGISTER_TEST_TO_GROUP(TestTemplateClass, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestTemplateClass, 2_UnitTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestTemplateClass::doInit()
{
  HRESULT res = S_OK;

  log(L"\nInit test cases...");
  LOG_TEST_RESULT(res, L"\nGet an unexpected result!", S_OK);

  return res;
}

HRESULT TestTemplateClass::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestTemplateClass");
  log(L"\n----------------------------------------------------------------\n");

  res |= doTestCase();
  // ... add new cases

  return res;
}


HRESULT TestTemplateClass::doEnd()
{
  HRESULT res = S_OK;

  log(L"\nDeinit test cases...");
  LOG_TEST_RESULT(res, L"\nGet an unexpected result!", S_OK);

  return res;
}

HRESULT TestTemplateClass::doTestCase()
{
  HRESULT res = S_OK;

  log(L"\nDo my test case...");
  LOG_TEST_RESULT(res, L"\nGet an unexpected result!", S_OK);

  return res;
}
