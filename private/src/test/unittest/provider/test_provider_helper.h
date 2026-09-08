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

//! @class TestProviderHelper
//! Test class the the provider helper
class TestProviderHelper : public xTST_Test2
{
public:
  //! Constructor
  TestProviderHelper();
  //! Destructor
  virtual ~TestProviderHelper() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Helper");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile provider helper.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  //! @brief This method tests the metadata method of the provider helper
  //! @return  S_OK, when the test was successful.
  HRESULT testMetadata();

  //! @brief This miscellaneous methods of the provider helper.
  //! @return  S_OK, when the test was successful.
  HRESULT testMiscellaneous();
};
