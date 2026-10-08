#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
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

/// @brief Test for the Sercos DeviceParam class
class TestSercosJobDeviceParam : public xTST_Test2
{

public:
  /// @brief Constructor
  TestSercosJobDeviceParam() = default;
  /// @brief Destructor
  virtual ~TestSercosJobDeviceParam()noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.cmdJobs.DeviceParam");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos profile cmdJobs DeviceParam class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LST);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief Init necessary motion system objects on test call.
  /// @return S_OK, when initialization successful. 
  HRESULT doInit();

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

  /// @brief Free memory of allocated objects in doInit() test function.
  /// @return S_OK, when deletion successful.
  HRESULT doEnd();

private:  //test methods

  /// @brief   This method test the function getJobData
  /// @return  S_OK, when tests were successful
  HRESULT TestGetJobData();

  /// @brief  This method test the function JobDeviceParamSoE::setData
  /// @return  S_OK, when tests were successful
  HRESULT TestDeviceParamSoEGetData();

};

