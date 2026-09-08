#pragma once

#include "functiontest/functest_base.h"
#include "stdafx.h"

//
// test class
//
class FuncTestCanopenCmd : public FuncTestBase
{
public: // methods
  /// @brief Constructor
  FuncTestCanopenCmd() = default;
  /// @brief Destructor
  virtual ~FuncTestCanopenCmd() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("FuncTest.Canopen.Cmd");
  TEST_DECLARE_DESCRIPTION("Function test of command of canopen");
  TEST_DECLARE_TIMEOUT_MINUTES(2);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods

  /// @brief Test of reset command of CoE
  /// @return  S_OK, when the test was successful
  HRESULT testCmdReset();

  /// @brief Test of homing command of CoE
  /// @return  S_OK, when the test was successful
  HRESULT testCmdHoming();

private: // data

  std::string m_name = "CoEProfile";  ///< Profile name used in this test
  comm::axisprofile::IAxisProfileInternal* m_profile {};  ///< test profile
};
