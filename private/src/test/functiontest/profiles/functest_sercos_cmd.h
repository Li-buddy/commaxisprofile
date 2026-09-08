#pragma once
#include "functiontest/functest_base.h"

//
// test class
//
class FuncTestSercosCmd : public FuncTestBase
{
public: // types

  /// @brief Structure test cases
  struct TestCase
  {
    std::wstring testName;  ///< test name
    comm::axisprofile::CmdType cmdType; ///< command type
    std::function<HRESULT(FuncTestSercosCmd*)> prepareFunc; ///< pre-condition function
    std::function<HRESULT(FuncTestSercosCmd*)> changeStatusFunc; ///< change status function
    std::function<HRESULT(FuncTestSercosCmd*)> testFunc;    ///< test function
  };

public: // methods

  /// @brief Constructor
  FuncTestSercosCmd() = default;
  /// @brief Destructor
  virtual ~FuncTestSercosCmd() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("FuncTest.Sercos.Cmd");
  TEST_DECLARE_DESCRIPTION("Function test of command of sercos");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods

  /// @brief Command method to test SoE command
  /// @param[in]  testCase  Case data of test
  /// @return  S_OK, when the test was successful
  HRESULT testSoECmd(const TestCase& testCase);

  /// @brief Method that triggers clear device error command
  /// @return  S_OK, when the test was successful
  HRESULT runClearDevError();

  /// @brief Method that prepare the test of reference axis
  /// @return  S_OK, when the test was successful
  HRESULT prepareReferencingAxis();

  /// @brief Method that run reference axis
  /// @return  S_OK, when the test was successful
  HRESULT runReferencingAxis();

  /// @brief Method that prepare the test of set absolute position
  /// @return  S_OK, when the test was successful
  HRESULT prepareSetAbsPos();

  /// @brief Method that run set absolute position
  /// @return  S_OK, when the test was successful
  HRESULT runSetAbsPos();

  /// @brief Method that run set absolute position
  /// @return  S_OK, when the test was successful
  HRESULT runFixedStop();

  /// @brief Method set SoE command completed
  /// @return  S_OK, when the test was successful
  HRESULT statusChangeSoECmdCompleted();

  /// @brief Method set error cleared
  /// @param[in]  hasError   true to set error, false to clear error
  /// @return  S_OK, when the test was successful
  HRESULT setErrorState(bool hasError);

private: // data

  std::string m_name = "testProfile";   ///< Profile name used in this test
  comm::axisprofile::IAxisProfileInternal* m_profile {};  ///< test profile
  static const std::vector<TestCase> m_testCases; ///< test cases
};
