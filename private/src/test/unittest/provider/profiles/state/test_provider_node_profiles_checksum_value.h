#pragma once
// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test/stdafx.h"
#include "provider/provider_helper.h"


// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------
class TestProviderProfilesChecksumValue : public xTST_Test2
{
public: // method
  /// Constructor
  TestProviderProfilesChecksumValue() = default;
  /// Destructor
  virtual ~TestProviderProfilesChecksumValue() noexcept override = default;
    
  // interface functions
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.ChecksumValue");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, used to get checksum value.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_WSY);

  /// @brief Init function of the test.
  /// @return S_OK, when initialization successful. 
  HRESULT doInit();

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnRead();

  /// @brief Check behavior of a successfull node request.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnMetadata();

private: // data

  std::string m_profileName = "testProfile";                                                        ///< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CHECKSUM_VALUE;    ///< Node address of the test class : device offset
  std::string m_invalidNodeAddress{};                                                               ///< Node address with profile name that does not exist
};
