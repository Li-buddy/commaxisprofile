#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "profiles/aux_job/aux_job_handler.h"
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

/// @brief Test for the AuxJobHandler class
class TestAuxJobHandler : public xTST_Test2
{
public:
  /// Constructor
  TestAuxJobHandler();
  /// Destructor
  virtual ~TestAuxJobHandler() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.AuxJobHandler");
  TEST_DECLARE_DESCRIPTION("Unittest of the profile AuxJobHandler class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_XQF);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

    HRESULT doTest() final;

private: //method

  /// @brief   This test runAllAuxJobs functionality.
  /// @return   S_OK, if test was successful 
  HRESULT testrunAllAuxJobs();
};
