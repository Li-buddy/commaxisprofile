#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider/provider_helper.h"

// Mockups
#include "mockup/mock_axisprofile_manager.h"

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

//! @class TestProviderProfilesSetURI
//! Test datalayer node to set profile related URIs.
class TestProviderProfilesSetURI : public xTST_Test2
{
public:
  //! Constructor
  TestProviderProfilesSetURI();
  //! Destructor
  virtual ~TestProviderProfilesSetURI() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.SetURI");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, used to set profile related URIs.");
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

private: // Data

  std::string m_profileName = "testProfile";                                                        //!< Profile name used in test
  std::string m_nodeAddressIn = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_INBUFFER;    //!< Node address of the test class for inputs
  std::string m_nodeAddressOut = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_OUTBUFFER;  //!< Node address of the test class for outputs

private: // Types

  //! @class MockAxisProfileManagerSpecial
  //! Specialization of the axis profile manager mockup, needed for this test.
  class MockAxisProfileManagerSpecial : public test::MockAxisProfileManager
  {
  public: // methods

    const char* getProfileInputBuffer(const char* /*profileName*/) override { return testURI.c_str(); }
    const char* getProfileOutputBuffer(const char* /*profileName*/) override { return testURI.c_str(); }

  public: // data

    std::string testURI = "axisprofile/profiles/testProfile/testURI";   //!< URI that is returned by the class methods
  };
};
