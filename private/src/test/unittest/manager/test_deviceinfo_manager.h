#pragma once
// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "manager/deviceinfo_manager.h"

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

/// @class TestDeviceInfoMgr
/// Test class for the fieldbus adapter base class.
class TestDeviceInfoMgr : public xTST_Test2
{
public: //method

  /// c'tor
  TestDeviceInfoMgr() = default;
  /// Destructor
  virtual ~TestDeviceInfoMgr() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.DeviceInfo.Manager");
  TEST_DECLARE_DESCRIPTION("Unittest of the device information manager class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief Init function of the test.
  /// @return S_OK, when initialization successful. 
  HRESULT doInit();

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

  /// @brief End function of the test.
  /// @return S_OK, when deinitialization successful.
  HRESULT doEnd();

private: // Methods

  /// @brief   this test validates the set operation of datalayer
  /// @return  S_OK, when tests were successful
  HRESULT TestSetDataLayer();

  /// @brief   this test validates the set operation of profiles
  /// @return  S_OK, when tests were successful
  HRESULT TestSetProfiles();

  /// @brief   this test validates the initialization of the available devices information
  /// @return  S_OK, when tests were successful
  HRESULT TestInitAvailableDeviceInfo();

  /// @brief   this test validates the update procedure of all profile address
  /// @return  S_OK, when tests were successful
  HRESULT TestUpdateProfileAddress();

  /// @brief   this test validates the update procedure of profile address with abnormal branch
  /// @return  S_OK, when tests were successful
  HRESULT TestUpdateProfileAddressSimpleCase();

  /// @brief   this test validates the function of CheckForDeviceAddressAndName
  /// @return  S_OK, when tests were successful
  HRESULT TestCheckForDeviceAddressAndName();

  /// @brief   this test validates the protocol type between device and profile.
  /// @return  S_OK, when tests were successful 
  HRESULT TestValidateProtocolInfo();

  /// @brief This method used to generate unique pointer to device manager
  /// @return unique_ptr pointer to the new object
  std::unique_ptr<comm::axisprofile::DeviceInfoManager> getNewDeviceInforMgr();
};
