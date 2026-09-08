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

//! @class TestDebugTrace
//! Test class for the trace adaption in the axis profile bundle
class TestDebugTrace : public xTST_Test2
{
public:
  //! Constructor
  TestDebugTrace();
  //! Destructor
  virtual ~TestDebugTrace() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Common.Trace");
  TEST_DECLARE_DESCRIPTION("Unittest of the trace adaption for the axis profile bundle.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

};
