#pragma once

#include "stdafx.h"


//
// test class
//
class TestCategoryMappingHelper : public xTST_Test2
{
public:
  TestCategoryMappingHelper() = default;
  virtual ~TestCategoryMappingHelper() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.TestCategoryMappingHelper");
  TEST_DECLARE_DESCRIPTION("Test of CategoryMappingHelper class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private:  //test methods

  /// @brief   This test validates the access of get available input mapping items related information 
  /// @return       S_OK, if test was successful 
  HRESULT testGetAvailableInputMappingItems();

  /// @brief   This test validates the access of get available output mapping items related information 
  /// @return       S_OK, if test was successful 
  HRESULT testGetAvailableOutputMappingItems();
};
