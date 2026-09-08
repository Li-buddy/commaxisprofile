#pragma once
/// @file
///
/// This header file is part of the axisprofile library and defines
/// the node for profile category.
///
/// @copyright (C) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


/// ---------------------------------------------------------------------------
/// Include Files
/// ---------------------------------------------------------------------------
#include "test/stdafx.h"
#include "provider/provider_helper.h"

/// ---------------------------------------------------------------------------
/// Declaration of symbolic constants
/// ---------------------------------------------------------------------------

/// ---------------------------------------------------------------------------
/// Declaration of data types
/// ---------------------------------------------------------------------------

/// ---------------------------------------------------------------------------
/// Forward references
/// ---------------------------------------------------------------------------

/// ---------------------------------------------------------------------------
/// Test Class
/// ---------------------------------------------------------------------------

/// @class TestProviderNodeProfilesCategory
/// Test datalayer node to set/get profile category data.
class TestProviderNodeProfilesCategory : public xTST_Test2
{
public:
  /// Constructor
  explicit TestProviderNodeProfilesCategory() = default;
  /// Destructor
  virtual ~TestProviderNodeProfilesCategory() noexcept override = default;

  ///
  /// interface functions
  ///
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.NodeProfilesCategory");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, used to set/get the profile category.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief Init function of the test.
  /// @return S_OK, when initialization successful. 
  HRESULT doInit();

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: /// Methods

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnRead();

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnMetadata();

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnWrite();

  /// @brief Create a default profile basicdata variant for testing.
  /// @param[in]   leaveFieldEmpty   Leave a flatbuffer field empty for error simulation.
  /// @return      The finished basicdata flatbuffer as variant.
  comm::datalayer::Variant createFBData(bool bLeaveFieldEmpty = false);

private: /// data

  std::string m_profileName = "testProfile";                                                                   /// Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_SCALINGINFO_STRATEGY;     /// Node basic data of the test class
  std::string m_invalidNodeAddress {};                                                                          /// Node address with profile name that does not exist
};
