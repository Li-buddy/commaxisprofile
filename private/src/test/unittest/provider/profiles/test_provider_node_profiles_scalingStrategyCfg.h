#pragma once
/// @file  test_provider_node_profiles_scalingStrategyCfg.h
///
/// This header file is part of the axisprofile library and defines
/// the node for scaling strategy.
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
#include "comm/axisprofile/defines/axisprofile_defs.h"


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

/// @class TestProviderNodeProfilesScalingStrategyCfg
/// Test class of the datalayer profile scaling strategy node
class TestProviderNodeProfilesScalingStrategyCfg : public xTST_Test2
{
public:
  /// Constructor
  explicit TestProviderNodeProfilesScalingStrategyCfg() = default;
  /// Destructor
  virtual ~TestProviderNodeProfilesScalingStrategyCfg() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.NodeProfilesScalingStrategyCfg");
  TEST_DECLARE_DESCRIPTION("Unittest of the datalayer node for the configuration of scaling.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

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

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnWrite();

  //! @brief Create a default profile basicdata variant for testing.
  //! @param[in]   leaveFieldEmpty   Leave a flatbuffer field empty for error simulation.
  //! @return      The finished basicdata flatbuffer as variant.

  /// @brief Create a default profile basicdata variant for testing.
  /// @param[in] bFieldEmpty      Leave a flatbuffer field empty for error simulation.
  /// @param[in] bIsScalingFactor Is scaling factor data exist.
  /// @param[in] strategy detail strategy.
  /// @return The finished scaling factor flatbuffer as variant.
  comm::datalayer::Variant createFBData(bool bFieldEmpty = false, bool bIsScalingFactor = false, comm::axisprofile::AxisProfileScalingStrategy strategy = comm::axisprofile::AxisProfileScalingStrategy::SOE_STANDARD);

private: // Data

  std::string m_profileName = "testProfile";                                            ///< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_SCALINGINFO;   ///< Node address of the test class
};
