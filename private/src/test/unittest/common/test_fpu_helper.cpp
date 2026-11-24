#include "stdafx.h"
#include <string.h>
#include <limits>
#include "common/fpu_helper.h"
#include "common/axisprofile_fpu_helper.h"
#include "test_fpu_helper.h"
#include "helper/result_helper.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestCommonFPUHelper);
REGISTER_TEST_TO_GROUP(TestCommonFPUHelper, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCommonFPUHelper, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCommonFPUHelper, Common);

#ifdef WIN32
#define RUNNING_ON_VALGRIND 0
#else
#include "../thirdparty/valgrind.h"     // required for macro RUNNING_ON_VALGRIND
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

std::vector<TestCommonFPUHelper::TestCase> TestCommonFPUHelper::s_testCases {
  {std::wstring(L"no error"), [] (volatile double& b)
  {
    b = 25 * 3.5;
  }, TestResults::STS_OK},
  {std::wstring(L"divide by zero error"), [] (volatile double& b)
  {
    double a = sin(0.0); b = 3.0 / a;
  }, TestResults::ERR_FPU_DIV_BY_ZERO},
  {std::wstring(L"inexact error"), [] (volatile double&)
  {
    float a = 1.1f; float b = 1.2f; volatile float c = a * b * 1.3f; (void)c;
  }, TestResults::STS_OK},
  {std::wstring(L"invalid error"), [] (volatile double& b)
  {
    double a = -1; b = std::sqrt(a);
  }, TestResults::ERR_FPU_INVALID},
  {std::wstring(L"underflow error"), [] (volatile double& b)
  {
    b = DBL_MIN; volatile float c = static_cast<float>(b); (void)c;
  }, TestResults::STS_OK},
  {std::wstring(L"overflow error"), [] (volatile double& b)
  {
    b = DBL_MAX; volatile float c = static_cast<float>(b); (void)c;
  }, TestResults::ERR_FPU_OVERFLOW}
};

std::vector<TestCommonFPUHelper::TestCaseValid> TestCommonFPUHelper::s_testCasesValid {
  {std::wstring(L"no error"), 1.0, ""},
  {std::wstring(L"too great"), fpu::DEFAULT_NUMERICAL_LIMIT + 1.0, "greater than default numerical limit"},
  {std::wstring(L"too less"), -fpu::DEFAULT_NUMERICAL_LIMIT - 1.0, "less than default numerical limit"},
  {std::wstring(L"positive infinite"), std::numeric_limits<double>::infinity(), "is infinite"},
  {std::wstring(L"negative infinite"), -std::numeric_limits<double>::infinity(), "is infinite"},
  {std::wstring(L"is not a number"), sqrt(-1.0), "is not a number"},
};

HRESULT TestCommonFPUHelper::doTest()
{
  xTST_Test::log(L"\n\n----------------------------------------------------------------");
  xTST_Test::log(L"\n  Test the FPU Helper");
  xTST_Test::log(L"\n----------------------------------------------------------------");

  fpu::clearFPUFlags();   //Clear all FPU exceptions

  HRESULT res = S_OK;

#ifndef WIN32
  if (RUNNING_ON_VALGRIND == 0)
#endif
  {
    // can't run these test cases, because Valgrind clears the FPU errors
    res |= runAllTestCases();
    res |= runAllTestCasesValid();
    res |= runTestOfMacros();
  }

  fpu::clearFPUFlags();   //Clear all FPU exceptions

  return res;
}

HRESULT TestCommonFPUHelper::runAllTestCases()
{
  HRESULT res = S_OK;

  for (const auto& testCase : s_testCases)
  {
    res |= runSingleTestCase(testCase);
  }
  return res;
}

HRESULT TestCommonFPUHelper::runSingleTestCase(const TestCase& testCase)
{
  HRESULT res = S_OK;

  log(L"\n run test case \'%ls\' ... ", testCase.m_name.c_str());
  volatile double b {};
  testCase.m_trigger(b);
  TestResults testRes = fpu::checkForFPUError(TestResults::STS_OK, TestResults::ERR_FPU_DIV_BY_ZERO);
  LOG_TEST_RESULT(testRes, L"\nUnexpected result!", testCase.m_expRes);

  return res;
}

HRESULT TestCommonFPUHelper::runAllTestCasesValid()
{
  HRESULT res = S_OK;

  for (const auto& testCase : s_testCasesValid)
  {
    log(L"\n run test case \'%ls\' ... ", testCase.m_name.c_str());
    auto resFPU = fpu::checkValid(testCase.m_value);
    std::string resStr = resFPU ? resFPU : "";
    LOG_TEST_RESULT_STRING(resStr, L"\nUnexpected result!", testCase.m_expRes);
  }
  return res;
}

HRESULT TestCommonFPUHelper::runTestOfMacros()
{
  volatile double b {};
  auto fpuFunc = [] (volatile double& b)
    {
      double a = sin(0.0);
      b = 3.0 / a;
    };

  fpu::clearFPUFlags(); // Clear all FPU exceptions
  log(L"\n test APR_CHECK_FPU_ERR_GEN when no FPU error");
  APR_CHECK_FPU_ERR_GEN(MAIN_DIAG_SYSTEM_ERR, DETAILED_DIAG_FPU_ERR_BEFORE_AXISPROFILE, "before manual trigger");
  fpuFunc(b);
  log(L"\n test APR_CHECK_FPU_ERR_GEN when exit FPU error");
  APR_CHECK_FPU_ERR_GEN(MAIN_DIAG_SYSTEM_ERR, DETAILED_DIAG_FPU_ERR_BEFORE_AXISPROFILE, "after manual trigger");

  fpu::clearFPUFlags(); // Clear all FPU exceptions
  fpu::g_FPUError = false;
  log(L"\n test APR_CHECK_FPU_ERR_PROFILE when no FPU error");
  APR_CHECK_FPU_ERR_PROFILE(MAIN_DIAG_SYSTEM_ERR, DETAILED_DIAG_FPU_ERR_BEFORE_AXISPROFILE, "profile1", "before manual trigger");
  fpuFunc(b);
  log(L"\n test APR_CHECK_FPU_ERR_PROFILE when exit FPU error");
  APR_CHECK_FPU_ERR_PROFILE(MAIN_DIAG_SYSTEM_ERR, DETAILED_DIAG_FPU_ERR_BEFORE_AXISPROFILE, "profile1", "after manual trigger");

  return S_OK;
}
