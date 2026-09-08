#pragma once

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

//! @class TestAxisProfileFactory
//! Test class for plc open state machine.
class TestAxisProfileFactory : public xTST_Test2
{
public:
  //! c'tor
  explicit TestAxisProfileFactory() = default;

  //! Destructor
  virtual ~TestAxisProfileFactory() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Factory");
  TEST_DECLARE_DESCRIPTION("Unittest of axisprofile factory.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_DS);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief Simple state machine test that verifies if all states and transitions are reached.
  //! @return   S_OK, if test was successful 
  HRESULT doFactoryTest();

  //! @brief test for the checkRequestedProfile function.
  //! @return   S_OK, if test was successful 
  HRESULT doCheckRequestedProfileTest();
};
