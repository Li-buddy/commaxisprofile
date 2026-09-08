#pragma once

#include "stdafx.h"


//
// test class
//
class TestAxisprofileCmdC : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestAxisprofileCmdC() = default;
  /// @brief Destructor
  virtual ~TestAxisprofileCmdC() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("BundleIf.AxisprofileCmdC");
  TEST_DECLARE_DESCRIPTION("Test of axisprofile command bundle interface");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;

private: // methods

  /// @brief Test commands invoking
  /// @return  S_OK, when the test was successful
  HRESULT testCmdInvoking();

  /// @brief Test command handling
  /// @return  S_OK, when the test was successful
  HRESULT testCmdHandling();
};
