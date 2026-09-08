#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider/provider_helper.h"


//
// test class
//
class TestProviderSystemUseChecksum : public xTST_Test2
{
public:
  /// Constructor
  TestProviderSystemUseChecksum() = default;;
  /// Destructor
  virtual ~TestProviderSystemUseChecksum() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.System.UseChecksum");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, used to set/get checksum switch of all profiles.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_WSY);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnRead();

  /// @brief Check behavior of a successfull node request.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnWrite();

  /// @brief Check behavior of a successfull node request.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnMetadata();

private: // data

  std::string m_profileName1 = "testProfile1";                                                        ///< Profile name used in test
  std::string m_profileName2 = "testProfile2";                                                        ///< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_SYSTEM_USE_CHECKSUM;          ///< Node address of the test class
};
