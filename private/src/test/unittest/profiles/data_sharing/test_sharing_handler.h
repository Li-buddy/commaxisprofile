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

/// @brief Test class for the axisprofile sharing handler.
class TestSharingHandler : public xTST_Test2
{
public: // methods

  /// constructor
  TestSharingHandler() = default;

  /// Destructor
  virtual ~TestSharingHandler() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.SharingHandler");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile sharing handler.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_JG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PRZ);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: // methods

  /// @brief   Test all Interface method cases.
  /// @return  S_OK, when tests was successful
  HRESULT testInterface();
};

