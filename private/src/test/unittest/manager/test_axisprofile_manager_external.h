#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "manager/axisprofile_manager.h"

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


//! @class TestAxisProfileManagerExternal
//! Test class for the external interface of the axisprofile manager
class TestAxisProfileManagerExternal : public xTST_Test2
{
public:
  //! c'tor
  TestAxisProfileManagerExternal();

  //! Destructor
  virtual ~TestAxisProfileManagerExternal() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Manager.ExternalInterface");
  TEST_DECLARE_DESCRIPTION("Unittest of the external axisprofile manager interface.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  //! @brief Test the method 'createAxisProfile()'.
  //! @return  S_OK, when tests were successful
  HRESULT testCreateProfile();

  //! @brief Test the method 'deleteAxisProfile()'.
  //! @return  S_OK, when tests were successful
  HRESULT testDeleteProfile();

  //! @brief Test the method 'connectAxisProfile()'.
  //! @return  S_OK, when tests were successful
  HRESULT testConnectProfile();

  //! @brief Test all other external interface methods.
  //! @return  S_OK, when tests were successful
  HRESULT testMiscellaneous();

private: // Data

  std::string m_profileName = "testProfile";     //!< Profile name used in this test
  std::string m_profileName1 = "testProfile1";   //!< Profile name used in this test
};

