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

//! @class TestProviderNodeProfilesBase
//! Test of the base class for all profile datalayer nodes.
class TestProviderNodeProfilesBase : public xTST_Test2
{
public:
  //! Constructor
  TestProviderNodeProfilesBase();
  //! Destructor
  virtual ~TestProviderNodeProfilesBase() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.NodeProfilesBase");
  TEST_DECLARE_DESCRIPTION("Unittest of the base class for all profile datalayer nodes.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);


  //! @brief Init function of the test.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  //! @brief Check behavior of different error cases.
  //! @return  S_OK, when the test was successful.
  HRESULT testErrors();

  //! @brief Check behavior of a successfull node request.
  //! @return  S_OK, when the test was successful.
  HRESULT testNode();

private: // data

  std::string m_profileName = "testProfile";                                                                                         //!< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES + comm::axisprofile::provider::NodeTree.WILDCARD;   //!< Node address of the test class
};
