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

//! @class TestProviderNodeSystemBootState
//! Test class of the datalayer profile configuration node
class TestProviderNodeSystemBootState : public xTST_Test2
{
public:
  //! Constructor
  TestProviderNodeSystemBootState();
  //! Destructor
  virtual ~TestProviderNodeSystemBootState() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.System.node_system_boot_state");
  TEST_DECLARE_DESCRIPTION("Unittest of the datalayer node for the configuration of one axisprofile.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_SD);

  //! @brief Init function of the test.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  //! @brief Check behavior of different error cases.
  //! @return  S_OK, when the test was successful.
  HRESULT testErrors();

  //! @brief Check behavior of a successfull node request.
  //! @return  S_OK, when the test was successful.
  HRESULT testNode();

  //! @brief This method tests the methods for read of the statemachine
  //! @return  S_OK, when the test was successful.
  HRESULT testNodeMethodsActReadFail();

private: // Data

  std::string m_profileName = "testProfile";                                               //!< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_SYSTEM_BOOT_STATE; //!< Node address of the test class
};
