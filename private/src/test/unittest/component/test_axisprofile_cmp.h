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

//! @class TestAxisProfileCmp
//! Test class for the axisprofile component.
class TestAxisProfileCmp : public xTST_Test2
{
public:
  //! Constructor
  TestAxisProfileCmp();
  //! Destructor
  virtual ~TestAxisProfileCmp() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Component");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile component for the axis profile bundle.");
  TEST_DECLARE_TIMEOUT_MINUTES(2);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  //! @brief   this test validates the methods used for the service dependencies by celix
  //! @return  S_OK, when tests were successful
  HRESULT testServiceDependencies() const ;

  //! @brief   this test validates the methods that are used by celix bundle logic
  //! @return  S_OK, when tests were successful
  HRESULT testCelixInterface();

  //! @brief   this test validates error cases of the methods that are used by celix bundle logic
  //! @return  S_OK, when tests were successful
  HRESULT testCelixInterfaceError();

  //! @brief   this test validates the interfaces that are registered as external interfaces for celix
  //! @return  S_OK, when tests were successful
  HRESULT testExternalCelixInterface();

  //! @brief Do some simple tests for the device profile component
  //! @return  S_OK, when the test was successful.
  HRESULT doSimpleTest();

  //! @brief Test the behavior of the class when some dependencies are null.
  //! @return  S_OK, when the test was successful.
  HRESULT testNullpointers();

  //! @brief Test background thread.
  //! @return  S_OK, when the test was successful.
  HRESULT testBackgroundThread() const;
};
