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

/// @brief Test for the Canopen SetAbsPos class
class TestCanopenJobSetAbsPos : public xTST_Test2
{

public:
  /// @brief Constructor
  TestCanopenJobSetAbsPos() = default;
  /// @brief Destructor
  virtual ~TestCanopenJobSetAbsPos()noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.cmdJobs.SetAbsPos");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen profile cmdJobs SetAbsPos class.");
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

  /// @brief   This method test the function onSetArgument
  /// @return  S_OK, when tests were successful
  HRESULT TestSetArgument();

  /// @brief   This method test the function onJobEnterActivating
  /// @return  S_OK, when tests were successful
  HRESULT TestJobEnterActivating();

  /// @brief   This method test the function onJobEnterAborting 
  /// @return  S_OK, when tests were successful
  HRESULT TestJobEnterAborting();

  /// @brief This method test the function onJobActivating
  /// @return  S_OK, when tests were successful
  HRESULT TestJobActivating();

  /// @brief This method test the function onJobAborting
  /// @return  S_OK, when tests were successful
  HRESULT TestJobAborting();

  /// @brief This method test the function setupReadListFromDevice
  /// @return  S_OK, when tests were successful
  HRESULT TestSetupReadListFromDevice();

  /// @brief This method test the function setupWriteListToDevice
  /// @return  S_OK, when tests were successful
  HRESULT TestSetupWriteListToDevice();

  /// @brief This method test the function judgmentHandleBased
  /// @return  S_OK, when tests were successful
  HRESULT TestJudgmentHandleBased();

  /// @brief This method test the function setIoEncoderData
  /// @return  S_OK, when tests were successful
  HRESULT TestSetIoEncoderData();

  /// @brief This method test the function resetIoEncoderData
  /// @return  S_OK, when tests were successful
  HRESULT TestResetIoEncoderData();

  /// @brief This method test the function setCmdStatusInDevice
  /// @return  S_OK, when tests were successful
  HRESULT TestSetCmdStatusInDevice();

private: //data  
  test::MockAxisProfileTrace m_mockProfileTrace {};             ///< Mockup of the Profile trace interface used in test
  test::MockFieldbusAdapterCoE m_mockFieldbusAdapter {};        ///< Mockup of the fieldbus adapter interface used in test
  test::MockScaling m_mockScaling {};                           ///< Mockup of the scaling interface used in test
  test::MockAxisProfileMappingHandler m_mockMappingHandler {};  ///< Mockup of the Mapping handle interface used in test
  test::MockAxisProfileDeviceParam m_mockDeviceParam {};        ///< Mockup of the device parameter interface used in test 
  test::MockAxisProfileProperty m_mockProperty {};              ///< Mockup of the property interface used in test
  test::MockCategory m_mockCategory {};                         ///< Mockup of the category interface used in test
  test::MockAxisProfileCyclicData m_mockCyclicData {};          ///< Mockup of the cyclic data interface used in test
  std::unique_ptr<AxisProfileSharingHandler> m_sharingHandlerHolder;  ///< unique pointer to the sharing handler
  IAxisProfileSharingHandler* m_sharingHandler {};               ///< pointer to the data sharing handler

  struct TestCase      ///<profile Category Test struct            
  {
    std::wstring name;           ///< descriptioon
    AxisProfileCategory nType;   ///< profile Category
    AxisProfileResult ret;       ///< expected result
  };
  std::vector<TestCase> m_testCases =       ///<Test Data for profile Category
  {
    {L"CATEGORY_ENCODER condition test", AxisProfileCategory::CATEGORY_ENCODER, AxisProfileResult::STS_OK},
    {L"CATEGORY_DRIVE condition test", AxisProfileCategory::CATEGORY_DRIVE, AxisProfileResult::STS_OK},
    {L"UNDEFINED condition test", AxisProfileCategory::UNDEFINED, AxisProfileResult::ERR_NOK},
  };
};
