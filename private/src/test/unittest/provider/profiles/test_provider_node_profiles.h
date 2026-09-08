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

//! @class TestProviderNodeProfiles
//! Test of the general datalayer node of all existing axis profiles.
class TestProviderNodeProfiles : public xTST_Test2
{
public:
  //! Constructor
  TestProviderNodeProfiles();
  //! Destructor
  virtual ~TestProviderNodeProfiles() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.NodeProfiles");
  TEST_DECLARE_DESCRIPTION("Unittest of the general datalayer node of all existing axis profiles.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // methods

  //! @brief Check behavior of a onCreate().
  //! @return  S_OK, when the test was successful.
  HRESULT testOnCreate();

  //! @brief Check behavior of a onRemove().
//! @return  S_OK, when the test was successful.
  HRESULT testOnRemove();

  //! @brief Check behavior of a onBrowse().
  //! @return  S_OK, when the test was successful.
  HRESULT testOnBrowse();

  //! @brief Check behavior of onMetadata().
  //! @return  S_OK, when the test was successful.
  HRESULT testOnMetaData();

private: // data

  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES;   //!< Node address of the test class
};
