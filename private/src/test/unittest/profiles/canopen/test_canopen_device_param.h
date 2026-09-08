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

/// @brief Test for the canopen booting configuration class
class TestCanopenDeviceParam : public xTST_Test2
{
public:
  /// @brief Constructor
  TestCanopenDeviceParam() = default;
  /// @brief Destructor
  virtual ~TestCanopenDeviceParam() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.DeviceParam");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen profile DeviceParam class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_JG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private:  //test methods

  /// @brief This method test for the firmware information related function.
  /// @return       S_OK, if test was successful
  HRESULT testFirmwareInfo();

  /// @brief   This test validates the access of operation mode related information 
  /// @return       S_OK, if test was successful 
  HRESULT testOperationModeInfo();

  /// @brief   This test validates determination of operation mode 
  /// @return       S_OK, if test was successful 
  HRESULT testDetermineOpModeAvailable();
  
  /// @brief   This test validates all not available methods in CoE
  /// @return       S_OK, if test was successful 
  HRESULT testNotAvailableMethods();

  /// @brief   This test validates checking of supported functionality.
  /// @return       S_OK, if test was successful 
  HRESULT testCheckFunctionSupported();

  /// @brief   This test validates checking of checksum related functionality.
  /// @return       S_OK, if test was successful 
  HRESULT testChecksum();

  /// @brief Test the checkNeedReadBootParams function
  /// @return S_OK, if test was successful
  HRESULT testCheckNeedReadBootParams();

  /// @brief Test the checkStandstill function
  /// @return S_OK, if test was successful 
  HRESULT testCheckStandstill();
  /// @brief Test the loadBootingTasks function
  /// @return S_OK, if test was successful
  HRESULT testLoadBootingTasks();

  /// @brief Test the getDeviceParamJobObj function
  /// @return S_OK, if test was successful
  HRESULT testGetDeviceParamJobObj();

  /// @brief Test the checkAxisHomed function
  /// @return S_OK, if test was successful
  HRESULT testCheckAxisHomed();

  /// @brief Test the deviceParamCoE function
  /// @return S_OK, if test was successful
  HRESULT testConstructDeviceParamCoE();

  /// @brief Test the readEncoderTypes function
/// @return S_OK, if test was successful
  HRESULT testReadEncoderTypes();
private: //data

  const std::string m_profileName = "testProfile";   ///< Name of the test axis profile
};
