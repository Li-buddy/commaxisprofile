#pragma once
// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test/stdafx.h"
#include "provider/provider_helper.h"


// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------
class TestProviderProfilesBootParams : public xTST_Test2
{
public: // method
  /// Constructor
  TestProviderProfilesBootParams() = default;
  /// Destructor
  virtual ~TestProviderProfilesBootParams() noexcept override = default;

  // interface functions
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.BootParams");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, used to get/set persisted boot parameters.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_WSY);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  /// @brief Init function of the test.
  /// @return S_OK, when initialization successful.
  HRESULT doInit();

  /// @brief Main test function for running all tests.
  /// @return S_OK, when tests were successful.
  HRESULT doTest();

private: // Methods

  /// @brief Check behavior of different read error cases and success case.
  /// @return S_OK, when the test was successful.
  HRESULT testOnRead();

  /// @brief Check conversion for all boot parameter value branches.
  /// @return S_OK, when all value conversions are as expected.
  HRESULT testToReadableValue();

  /// @brief Check behavior of metadata request.
  /// @return S_OK, when the test was successful.
  HRESULT testOnMetadata();

private: // data

  std::string m_profileName = "testProfile";                                                      ///< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_BOOTPARAMS;  ///< Node address of the test class: boot params
  std::string m_invalidNodeAddress{};                                                             ///< Node address with profile name that does not exist
};
