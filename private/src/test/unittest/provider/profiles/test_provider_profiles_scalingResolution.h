#pragma once
/// @file  test_provider_profiles_scalingResolution.h
///
/// This header file is part of the axisprofile library and defines
/// the node for device resolution.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test/stdafx.h"
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

//! @class TestProviderProfilesScalingResolution
//! Test datalayer node to set/get the device resolution of a profile.
class TestProviderProfilesScalingResolution : public xTST_Test2
{
public:
  //! Constructor
  explicit TestProviderProfilesScalingResolution() = default;
  //! Destructor
  virtual ~TestProviderProfilesScalingResolution() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.ScalingResolution");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, used to set/get the device resolution.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);


  //! @brief Init function of the test.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  //! @brief Check behavior of different error cases.
  //! @return  S_OK, when the test was successful.
  HRESULT testOnRead();

  //! @brief Check behavior of a successfull node request.
  //! @return  S_OK, when the test was successful.
  HRESULT testOnWrite();

  /// @brief Check behavior of a successfull node request.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnMetadata();

private: // data

  std::string m_profileName = "testProfile";                                                      //!< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_SCALINGINFO_RESOLUTION;   //!< Node address of the test class
  std::string m_invalidNodeAddress{};                                                             //!< Node address with profile name that does not exist
};
