#pragma once

#include "mockup/mock_job_obj.h"
#include "profiles/axisprofile_cmd.h"
#include "profiles/profile_defs.h"
#include "stdafx.h"
#include <memory>
//
// test class
//
class TestProfileCmd : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestProfileCmd() = default;
  /// @brief Destructor
  virtual ~TestProfileCmd() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Cmd");
  TEST_DECLARE_DESCRIPTION("Test of profile cmd base class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;

private: // methods

  /// @brief Test error case of command handling methods
  /// @return  S_OK, when the test was successful
  HRESULT testErrCaseOfCmdHanlding();

  /// @brief Test normal case of command handling methods
  /// @return  S_OK, when the test was successful
  HRESULT testNormalCaseOfCmdHandling();

  /// @brief Test of command invokers
  /// @return  S_OK, when the test was successful
  HRESULT testOfCmdInvokers();

  /// @brief Test of command invokers
  /// @return  S_OK, when the test was successful
  HRESULT testWrapperCmds();

private: // data

  std::unique_ptr<::comm::axisprofile::AxisProfileCmd> m_testCmd {};  ///< pointer to test command
};
