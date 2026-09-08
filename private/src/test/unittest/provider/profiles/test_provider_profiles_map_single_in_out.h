#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider/provider_helper.h"

// Mockups
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_profile_trace.h"

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

//! @class TestProviderProfilesMapSingleInputOutput
//! Test datalayer node to map single inputs or outputs.
class TestProviderProfilesMapSingleInputOutput : public xTST_Test2
{
public:
  //! Constructor
  TestProviderProfilesMapSingleInputOutput();
  //! Destructor
  virtual ~TestProviderProfilesMapSingleInputOutput() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.MapSingleInputOutput");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, used to map single inputs and outputs.");
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

  std::string m_profileName = "testProfile";                                                                                                         //!< Profile name used in test
  std::string m_nodeAddressIn = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_INMAPPING + comm::axisprofile::provider::NodeTree.POSITION;   //!< Node address of the test class for inputs
  std::string m_nodeAddressOut = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_OUTMAPPING + comm::axisprofile::provider::NodeTree.POSITION; //!< Node address of the test class for outputs

private: // Types

  //! @class MockAxisProfileMappingHandlerSpecial
  //! Specialization of the axis profile maping handler mockup, needed for this test.
  class MockAxisProfileMappingHandlerSpecial : public test::MockAxisProfileMappingHandler
  {
  public: // methods

    //! @brief Constructor
    MockAxisProfileMappingHandlerSpecial() : test::MockAxisProfileMappingHandler() {}

    const char* getConfiguredInputURI(comm::axisprofile::InputMappingValueID /*valueID*/) const override { return m_testURI.c_str(); }
    const char* getConfiguredOutputURI(comm::axisprofile::OutputMappingValueID /*valueID*/) const override { return m_testURI.c_str(); }

  public: // data

    const std::string m_testURI = "axisprofile/profiles/testProfile/testURI";   //!< URI that is returned by the class methods

  };
};
