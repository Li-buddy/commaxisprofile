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

/// @brief Test for the Sercos JobHoming class
class TestSercosJobHoming : public xTST_Test2
{

public:
  /// @brief Constructor
  TestSercosJobHoming() = default;
  /// @brief Destructor
  virtual ~TestSercosJobHoming()noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.cmdJobs.JobHoming");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos profile cmdJobs JobHoming class.");
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

  /// @brief   This method test the private function validateHomingSupport
  /// @return  S_OK, when tests were successful
  HRESULT TestValidateHomingSupport();

  /// @brief   This method test the private function activeHomingByProfile
  /// @return  S_OK, when tests were successful
  HRESULT TestActiveHomingByProfile();

private: //data  
  test::MockAxisProfileTrace m_mockProfileTrace {};             ///< Mockup of the Profile trace interface used in test
  test::MockFieldbusAdapterSoE m_mockFieldbusAdapter {};        ///< Mockup of the fieldbus adapter interface used in test
  test::MockScaling m_mockScaling {};                           ///< Mockup of the scaling interface used in test
  test::MockAxisProfileMappingHandler m_mockMappingHandler {};  ///< Mockup of the Mapping handle interface used in test
  test::MockAxisProfileDeviceParam m_mockDeviceParam {};        ///< Mockup of the device parameter interface used in test 
  test::MockAxisProfileProperty m_mockProperty {};              ///< Mockup of the property interface used in test
  test::MockAxisProfileCyclicData m_mockCyclicData {};          ///< Mockup of the cyclic data interface used in test
  std::unique_ptr<AxisProfileSharingHandler> m_sharingHandlerHolder;  ///< unique pointer to the sharing handler
  IAxisProfileSharingHandler* m_sharingHandler {};               ///< pointer to the data sharing handler
};
