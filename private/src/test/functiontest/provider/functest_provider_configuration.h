#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "functiontest/functest_base.h"

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

//! @class FuncTestProviderConfiguration
//! Functiontest of the provider manager for datalayer nodes.
class FuncTestProviderConfiguration : public FuncTestBase
{
public:
  //! Constructor
  FuncTestProviderConfiguration();
  //! Destructor
  virtual ~FuncTestProviderConfiguration() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("FuncTest.Provider.Configuration");
  TEST_DECLARE_DESCRIPTION("Functiontest for profile configuration via datalayer.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief Init necessary motion system objects on test call.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief Free memory of allocated objects in doInit() test function.
  //! @return S_OK, when deletion successful.
  HRESULT doEnd();

private: //methods

  //! @brief This test creates profiles and try´s to adress them in the datalayer.
  //! @return  S_OK, when the test was successful.
  HRESULT testCreateOfProfiles();
};
