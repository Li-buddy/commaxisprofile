#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "profiles/canopen/canopen_profile.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace test
{
  class MockDatalayerClient;
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestCanopenProfile
//! Test class for can commands of in the axisprofile.
class TestCanopenProfile : public xTST_Test2
{
public: // methods

  //! Constructor
  TestCanopenProfile() = default;

  //! Destructor
  virtual ~TestCanopenProfile() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.Profile");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen profile class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_SD);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //method

 HRESULT testForSetScalingStrategy();

private: //data

  const std::string m_profileName = "testProfile";   //!< Name of the test axis profile

};
