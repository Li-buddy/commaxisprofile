#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "profiles/sercos/drive/sercos_device_param_drive.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace test
{
  class MockDatalayerClient;
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @brief Test for the sercos device parameter class
class TestSercosDeviceParam : public xTST_Test2
{
public: // Types
  /// @brief This struct contains the test data for a variant that is returned by the datalayer
  struct TestDataVariantU16
  {
    const wchar_t* m_name;          ///< name of the variant type
    std::uint16_t m_data;           ///< data to write in the variant
    bool m_addData;                 ///< add data to variant?
  };

  struct TestDelayInformation
  {
    const wchar_t *m_name;                                      ///< Name of the variant type
    bool m_delaySupport{};                                      ///< Indicates whether the delay break support
    std::uint16_t m_multiBrakeDelaySupportCase{};               ///< The deceleration method
    comm::axisprofile::AxisProfileResult m_expectedResult;      ///< Expected result for clear error call
  };

public: // methods

  /// Constructor
  TestSercosDeviceParam();
  /// Destructor
  virtual ~TestSercosDeviceParam() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.DeviceParam");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos profile DeviceParam class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_JS);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private:  // test methods
  /// @brief   This test validates the delay brake informations available.
  /// @return       S_OK, if test was successful 
  HRESULT testDelayBrakeInfo();

  /// @brief   This test validates whether the dynamic brake limits available.
  /// @return       S_OK, if test was successful 
  HRESULT testMappingBrakeInfo();

  /// @brief    Test booting methods
  /// @return   S_OK, if test was successful 
  HRESULT testBootingMethods();

  /// @brief   This method is the main test routine to validate delay brake information with several different testcase.
  /// @param[in]    testDeviceParam    sercos device parameter object
  /// @param[in]    testCase           test data
  /// @return       S_OK, if test was successful 
  HRESULT executeTestDelayBrakeInfo(comm::axisprofile::sercos::SercosDeviceParamDrive& testDeviceParam, const TestDelayInformation &testCase);

  /// @brief   This method sets a test variant for the client response (async calls).
  /// @param[in]    client      data layer client to execute datalayer access
  /// @param[in]    data        test data for the variant
  void setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16& data);

  /// @brief   This test validates checking of supported functionality.
  /// @return       S_OK, if test was successful 
  HRESULT testCheckFunctionSupported();

  /// @brief Test the readPosProcessMode method
  /// @return S_OK, if test was successful
  HRESULT testreadPosProcessMode();

  /// @brief Test the checksum related method
  /// @return S_OK, if test was successful
  HRESULT testChecksum();

  /// @brief Test the checkNeedReadBootParams function
  /// @return S_OK, if test was successful
  HRESULT testCheckNeedReadBootParams();

  /// @brief Test the checkAxisHomed function
  /// @return S_OK, if test was successful
  HRESULT testCheckAxisHomed();

  /// @brief Test the loadBootingTasks function
  /// @return S_OK, if test was successful
  HRESULT testLoadBootingTasks();

  /// @brief Test the getDeviceParamJobObj function
  /// @return S_OK, if test was successful
  HRESULT testGetDeviceParamJobObj();
private: // data

  const std::string m_profileName = "testProfile";   ///< Name of the test axis profile
};
