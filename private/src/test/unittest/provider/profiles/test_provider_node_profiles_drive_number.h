#pragma once

#include "stdafx.h"
#include "provider/provider_helper.h"

//
// test class
//
class TestProviderProfilesDriveNumber : public xTST_Test2
{
public:
  TestProviderProfilesDriveNumber() = default;
  virtual ~TestProviderProfilesDriveNumber() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.DriveNumber");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, used to set/get the drive number.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_XQF);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  HRESULT doInit() final;
  HRESULT doTest() final;

private:

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

  std::string m_profileName = "testProfile";                                                        ///< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_DRIVENUMBER;   ///< Node drive number of the test class
  std::string m_invalidNodeAddress{};                                                               ///< Node address with profile name that does not exist
};
