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

//! @class TestProviderManager
//! Test class the provider manager
class TestProviderManager : public xTST_Test2
{
public:
  //! Constructor
  TestProviderManager();
  //! Destructor
  virtual ~TestProviderManager() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Manager");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile provider manager.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  //! @brief Do some simple tests for the provider manager.
  //! @return  S_OK, when the test was successful.
  HRESULT doSimpleTest();

  //! @brief Do some tests that should fail.
  //! @return  S_OK, when the test was successful.
  HRESULT doFailTest();

};
