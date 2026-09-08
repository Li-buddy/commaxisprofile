#pragma once

#include "stdafx.h"
#include "provider/provider_helper.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

//
// test class
//
class TestProviderNodeProfilesScalingFactor : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestProviderNodeProfilesScalingFactor() = default;
  /// @brief Destructor
  virtual ~TestProviderNodeProfilesScalingFactor() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.NodeProfilesScalingFactorCfg");
  TEST_DECLARE_DESCRIPTION("Test of special scaling factor");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods

  /// @brief Test create function
  /// @return  S_OK, when the test was successful
  HRESULT doTestCase();

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnRead();

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnMetadata();

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnWrite();
    
  /// @brief Create a default scaling factor variant for testing.
  /// @param[in] bFieldEmpty Leave a flatbuffer field empty for error simulation.
  /// @param[in] factor the source scaling factor data.
  /// @return The finished scaling factor flatbuffer as variant.
  comm::datalayer::Variant createFBData(bool bFieldEmpty = false, const comm::axisprofile::ScalingFactor& factor = comm::axisprofile::ScalingFactor());

private: // data

  std::string m_profileName {"testProfile"};                                                                         /// Profile name used in test
  std::string m_nodeAddress {comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_SCALINGINFO_VELOCITYFACTOR};     /// Node basic data of the test class
  std::string m_invalidNodeAddress {};                                                                               /// Node address with profile name that does not exist
};
