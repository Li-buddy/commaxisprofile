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

//! @class TestMappingHandler
//! Test class for the axisprofile mapping handler.
class TestMappingHandler : public xTST_Test2
{
public:
  //! c'tor
  TestMappingHandler();

  //! Destructor
  virtual ~TestMappingHandler() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.MappingHandler");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile mapping handler.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_DS);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private:

  //! @brief   Test all error cases of all methods.
  //! @return  S_OK, when tests was successful
  HRESULT testErrors();

  //! @brief   Test all methods of the mapping handler.
  //! @return  S_OK, when tests was successful
  HRESULT testHandler();

  //! @brief   Test methods of the mapping handler that uses a vector.
  //! @return  S_OK, when tests was successful
  HRESULT testVectorConfig();

  //! @brief   Test methods to convert strings.
  //! @return  S_OK, when tests was successful
  HRESULT testStringConvertMethods();
};

