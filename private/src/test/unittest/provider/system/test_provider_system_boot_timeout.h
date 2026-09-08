#pragma once

/// @file          test_provider_system_boot_timeout.h
///
/// This header file is part of the axisprofile library and define the test code of datalayer max booting timeout
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider/provider_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of classes and structures
// ---------------------------------------------------------------------------

/// @brief Test class of the system boot timeout datalayer node
/// Test class of the system boot timeout datalayer node
class TestProviderBootTimeout : public xTST_Test2
{
public:
  //! Constructor
  TestProviderBootTimeout();
  //! Destructor
  virtual ~TestProviderBootTimeout() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.System.BootTimeout");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile boot timeout provider.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_YSK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  /// @brief This method tests the methods for read of the max booting timeout
  /// @return  S_OK, when the test was successful.
  HRESULT testOnRead();

  /// @brief This method tests the methods for reading the metadata.
  /// @return  S_OK, when the test was successful.
  HRESULT testOnMetadata();

private: // Data
  std::string m_profileName = "testProfile";                                                      //!< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_SYSTEM_BOOT_TIMEOUT;      //!< Node address of the test class for max timeout
};
