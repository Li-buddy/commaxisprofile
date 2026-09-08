#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "profiles/canopen/cmd_jobs/canopen_job_device_param.h"
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
class CanopenJobDeviceParamEx : public comm::axisprofile::canopen::CanopenJobDeviceParam
{
public:
  /// @brief Constructor
  explicit CanopenJobDeviceParamEx(ProfileBasicInfo& profileInfo, comm::axisprofile::IAxisProfileSharingHandler* shareHandle = nullptr)
    : CanopenJobDeviceParam(profileInfo, shareHandle)
  {
  }
  /// @brief Destructor
  virtual ~CanopenJobDeviceParamEx()noexcept override = default;

  using CanopenJobDeviceParam::onJobEnterActivating;
  using CanopenJobDeviceParam::onJobActivating;
  using CanopenJobDeviceParam::onJobEnterAborting;
};

/// @brief Test for the Canopen DeviceParam class
class TestCanopenJobDeviceParam : public xTST_Test2
{

public:
  /// @brief Constructor
  TestCanopenJobDeviceParam() = default;
  /// @brief Destructor
  virtual ~TestCanopenJobDeviceParam()noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.cmdJobs.DeviceParam");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen profile cmdJobs DeviceParam class.");
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

  /// @brief   This method test the function getJobDataRef
  /// @return  S_OK, when tests were successful
  HRESULT TestGetJobDataRef();

  /// @brief   This method test the function onJobEnterActivating
  /// @return  S_OK, when tests were successful
  HRESULT TestJobEnterActivating();

  /// @brief   This method test the function onJobActivating
  /// @return  S_OK, when tests were successful
  HRESULT TestJobActivating();

  /// @brief   This method test the function onJobEnterAborting
  /// @return  S_OK, when tests were successful
  HRESULT TestJobEnterAborting();

  /// @brief   This method test the function compareIdAndType
  /// @return  S_OK, when tests were successful
  HRESULT TestCompareIdAndType();

  /// @brief   This method test the function getData
  /// @return  S_OK, when tests were successful
  HRESULT TestGetData();

  /// @brief   This method test the function setData
  /// @return  S_OK, when tests were successful
  HRESULT TestSetData();

private: //data  
  test::MockAxisProfileTrace m_mockProfileTrace {};             ///< Mockup of the Profile trace interface used in test
  test::MockFieldbusAdapterSoE m_mockFieldbusAdapter {};        ///< Mockup of the fieldbus adapter interface used in test
  test::MockScaling m_mockScaling {};                           ///< Mockup of the scaling interface used in test
  test::MockAxisProfileDeviceParam m_mockDeviceParam {};        ///< Mockup of the device parameter interface used in test
  std::unique_ptr<AxisProfileSharingHandler> m_sharingHandlerHolder;  ///< unique pointer to the sharing handler
  IAxisProfileSharingHandler* m_sharingHandler {};               ///< pointer to the data sharing handler
};

