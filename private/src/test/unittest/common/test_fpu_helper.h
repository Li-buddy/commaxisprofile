#pragma once

#include <functional>
#include <vector>

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------
/// @class TestCommonFPUHelper
/// Test class for object factory.
class TestCommonFPUHelper : public xTST_Test2
{
public:
  /// Destructor
  virtual ~TestCommonFPUHelper() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("common.FPUHelper");
  TEST_DECLARE_DESCRIPTION("Test of the FPU helper");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest() override;

private: // types

  /// @brief Return values for the test
  enum class TestResults : std::uint32_t
  {
    STS_OK = 123,                       ///< okay
    ERR_FPU_DIV_BY_ZERO = 0xdead,       ///< FPU error 'division by zero'
    ERR_FPU_INVALID,                    ///< FPU error 'invalid number'
    ERR_FPU_OVERFLOW,                   ///< FPU error 'overflow'
    ERR_FPU_UNKNOWN                     ///< unknown FPU error
  };

  /// @brief structure for test cases
  struct TestCase
  {
    const std::wstring m_name;          ///< name of the test case
    std::function<void(volatile double&)> m_trigger;   ///< test function
    TestResults m_expRes;               ///< expected result
  };

  /// @brief structure for test cases for checkValid
  struct TestCaseValid
  {
    const std::wstring m_name;          ///< name of the test case
    double m_value;                     ///< test value
    std::string m_expRes;               ///< expected result
  };

private: // variables

  static std::vector<TestCase> s_testCases;             ///< all test cases
  static std::vector<TestCaseValid> s_testCasesValid;   ///< all test cases for checkValid

private: // methods

  /// @brief run all test cases
  /// @return  S_OK, when tests were successful
  HRESULT runAllTestCases();

  /// @brief run a single test case
  /// @return  S_OK, when tests were successful
  HRESULT runSingleTestCase(const TestCase& testCase);

  /// @brief run all test cases for check valid
  /// @return  S_OK, when tests were successful
  HRESULT runAllTestCasesValid();
};
