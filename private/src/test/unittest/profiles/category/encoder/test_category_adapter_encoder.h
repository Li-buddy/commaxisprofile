#pragma once

#include "stdafx.h"


//
// test class
//
class TestCategoryAdapterEncoder : public xTST_Test2
{
public:
  TestCategoryAdapterEncoder() = default;
  virtual ~TestCategoryAdapterEncoder() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.CategoryAdapterEncoder");
  TEST_DECLARE_DESCRIPTION("Test of CategoryAdapterEncoder class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;

private:

  /// @brief   test create function
  /// @return  S_OK, when the test was successful
  HRESULT testClass();

  /// @brief Test the axisprofile Interface.
  /// @return  S_OK, when tests were successful
  HRESULT testInterface();
};
