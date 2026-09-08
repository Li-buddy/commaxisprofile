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

//! @class FuncTestProviderSystemStatemachine
//! Functiontest of the statemachine datalayer node.
class FuncTestProviderSystemStatemachine : public FuncTestBase
{
public:
  //! Constructor
  FuncTestProviderSystemStatemachine();
  //! Destructor
  virtual ~FuncTestProviderSystemStatemachine() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("FuncTest.Provider.Statemachine");
  TEST_DECLARE_DESCRIPTION("Functiontest of the statemachine datalayer node.");
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

  //! @brief Do some simple tests for the statemachine provider.
  //! @return  S_OK, when the test was successful.
  HRESULT testReadStatemachineNodes();

  //! @brief Test different cases when switching the statemachine to 'Running'.
  //! @return  S_OK, when the test was successful.
  HRESULT testTransitionToRunning();

  //! @brief This method requests a new statemachine transition via datalayer, checks the expected
  //!        result and switches the statemachine back to 'Configuration' if needed.
  //! @return  S_OK, when the test was successful.
  HRESULT checkTransition(const comm::axisprofile::StateIDType cmdState, const comm::axisprofile::StateIDType expState, const std::uint32_t timeoutMilliseconds, const bool backToConfig = true);
};
