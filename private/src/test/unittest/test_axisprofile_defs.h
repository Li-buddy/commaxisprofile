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

//! @class TestAxisProfileDefs
//! Test class for the axisprofile defines header
class TestAxisProfileDefs : public xTST_Test2
{
public:
  //! c'tor
  TestAxisProfileDefs();

  //! Destructor
  virtual ~TestAxisProfileDefs() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.ProfileDefs");
  TEST_DECLARE_DESCRIPTION("Unittest of axisprofile defines.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  //! @brief Test the public (external) axisprofile defines.
  //! @return  S_OK, when tests were successful
  HRESULT testPublicDefs();

  //! @brief Test the internal axisprofile defines.
  //! @return  S_OK, when tests were successful
  HRESULT testInternalDefs();

  /// @brief Test the axisprofile defines.
  /// @return  S_OK, when tests were successful
  HRESULT testProfileDefs();

  /// @brief Test the axisprofile mapping handle defines.
  /// @return  S_OK, when tests were successful
  HRESULT testMappingHandleDefs();
};

