#pragma once

#include "stdafx.h"


//
// test class
//
class TestTemplateClass : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestTemplateClass() = default;
  /// @brief Destructor
  virtual ~TestTemplateClass() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Base.TemplateClass");
  TEST_DECLARE_DESCRIPTION("Test of template class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods

  /// @brief Test create function
  /// @return  S_OK, when the test was successful
  HRESULT doTestCase();
};
