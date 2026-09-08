#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider/provider_helper.h"

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

//! @class TestProviderNodeBase
//! Test datalayer node to set profile related URIs.
class TestProviderNodeBase : public xTST_Test2
{
public:
  //! Constructor
  TestProviderNodeBase();
  //! Destructor
  virtual ~TestProviderNodeBase() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.NodeBase");
  TEST_DECLARE_DESCRIPTION("Unittest of the base class for all axisprofile datalayer nodes.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  //! @brief Check behavior of a successfull node request.
  //! @return  S_OK, when the test was successful.
  HRESULT testClass();

  /// @brief Check behavior of a successfull node request.
  /// @return  S_OK, when the test was successful.
  HRESULT testCreateBasicTypeString();
};
