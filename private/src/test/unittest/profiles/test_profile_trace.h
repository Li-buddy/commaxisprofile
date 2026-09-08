#pragma once

// ---------------------------------------------------------------------------
//
//! @file       test_profile_trace.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This file is part of the axisprofile library and defines the test for the 
//! profile trace helper class.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestProfileTrace
//! Test class for the unit test of the profile trace helper class.
class TestProfileTrace : public xTST_Test2
{
public:
  //! @brief Constructor
  TestProfileTrace() = default;
  //! @brief Destructor
  virtual ~TestProfileTrace() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Trace");
  TEST_DECLARE_DESCRIPTION("Test of profile trace helper class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief Init necessary motion system objects on test call.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function
  //! @return  S_OK, when the test was successful
  HRESULT doTest();

  //! @brief Free memory of allocated objects in doInit() test function.
  //! @return S_OK, when deletion successful.
  HRESULT doEnd();

private: // method

  //! @brief Execute all error test cases.
  //! @return  S_OK, when the test was successful
  HRESULT testErrors();

  //! @brief Set various data in the trace class and check the resulting trace messages.
  //! @return  S_OK, when the test was successful
  HRESULT testDifferentTraceMessages();

  //! @brief Create trace messages for all possible error codes.
  //! @return  S_OK, when the test was successful
  HRESULT testReturnCodeTraceMessages();

private: // data

  const std::string m_testProfileName = "testProfile";   //!< Profile name used in this test
};
