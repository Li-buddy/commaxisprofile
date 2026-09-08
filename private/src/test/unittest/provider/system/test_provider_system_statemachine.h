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

//! @class TestProviderSystemStatemachine
//! Test class of the system statemachine datalayer node
class TestProviderSystemStatemachine : public xTST_Test2
{
public:
  //! Constructor
  TestProviderSystemStatemachine();
  //! Destructor
  virtual ~TestProviderSystemStatemachine() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.System.Statemachine");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile statemachine provider.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  //! @brief This method tests the methods for create.
  //! @return  S_OK, when the test was successful.
  HRESULT testNodeMethodsCmdCreateFail();

  //! @brief This method tests the methods for browse.
  //! @return  S_OK, when the test was successful.
  HRESULT testNodeMethodsCmdBrowseFail();

  //! @brief This method tests the methods for read of the statemachine
  //! @return  S_OK, when the test was successful.
  HRESULT testNodeMethodsActReadFail();

  //! @brief This method tests the methods for reading the metadata.
  //! @return  S_OK, when the test was successful.
  HRESULT testNodeMethodsOnMetadata();

private: // Data

  std::string m_nodeAddressCmd = comm::axisprofile::provider::NodeTree.APR_SYSTEM_OPSTATE_CMD;      //!< Node address of the test class for commanding
  std::string m_nodeAddressAct = comm::axisprofile::provider::NodeTree.APR_SYSTEM_OPSTATE_ACTUAL;   //!< Node address of the test class for actual state
};
