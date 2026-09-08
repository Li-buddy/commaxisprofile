#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"


// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @class TestDeviceIdHelper
class TestDeviceIdHelper : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestDeviceIdHelper() = default;
  /// @brief Destructor
  virtual ~TestDeviceIdHelper() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Helper.DeviceIdHelper");
  TEST_DECLARE_DESCRIPTION("Unittest of the device judge helper.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_WSY);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  /// @brief This method tests the IsRexrothDrive function
  /// @return  S_OK, when the test was successful.
  HRESULT testIsRexrothDrive();

  /// @brief This method tests the calculateDeviceSeries function
  /// @return  S_OK, when the test was successful.
  HRESULT testGetDeviceSeries();

  /// @brief This method tests the deviceSupportCheckSumParam function
  /// @return  S_OK, when the test was successful.
  HRESULT testDeviceSupportCheckSumParam();

  /// @brief This method tests the extractFirmwareByStr function
  /// @return  S_OK, when the test was successful.
  HRESULT testExtractFirmwareByStr();

  /// @brief This method tests the convertSoftwareVersion function
  /// @return  S_OK, when the test was successful.
  HRESULT testConvertSoftwareVersion();

  /// @brief This method help to make a product to test case
  /// @param   highWord   high word of the product code
  /// @param   lowWord    low word of the product code
  /// @return  product code
  uint32_t makeProductCode(uint16_t highWord, uint16_t lowWord = 0x0001);
};
