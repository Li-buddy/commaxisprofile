#pragma once

#include "stdafx.h"


//
// test class
//
class TestJobMgr : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestJobMgr() = default;
  /// @brief Destructor
  virtual ~TestJobMgr() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Jobs.JobMgr");
  TEST_DECLARE_DESCRIPTION("Test of axisprofile job manager");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;

private: // methods

  /// @brief Test create function
  /// @return  S_OK, when the test was successful
  HRESULT doTestCase();
};
