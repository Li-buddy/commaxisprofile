#pragma once

/// @file          functest_provider_system_boot_timeout.h
///
/// This header file is part of the axisprofile library and define function test code of datalayer max booting timeout
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "functiontest/functest_base.h"


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

/// @brief Test class of the boot timeout
/// Functiontest of the BootTimeout datalayer node.
class FuncTestProviderBootTimeout : public FuncTestBase
{
public:
  /// Constructor
  FuncTestProviderBootTimeout();
  /// Destructor
  virtual ~FuncTestProviderBootTimeout() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("FuncTest.Provider.BootTimeout");
  TEST_DECLARE_DESCRIPTION("Functiontest of the boot Timeout datalayer node.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_YSK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  /// @brief Init necessary motion system objects on test call.
  /// @return S_OK, when initialization successful. 
  HRESULT doInit();

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

  /// @brief Free memory of allocated objects in doInit() test function.
  /// @return S_OK, when deletion successful.
  HRESULT doEnd();

private: //methods

  /// @brief Do some simple tests for the maxtimeout provider.
  /// @return  S_OK, when the test was successful.
  HRESULT testReadBootTimeoutNodes();
};
