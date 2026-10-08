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

/// @brief Test for the Sercos JobFixedStop class
class TestSercosJobFixedStop : public xTST_Test2
{

public:
  /// @brief Constructor
  TestSercosJobFixedStop() = default;
  /// @brief Destructor
  virtual ~TestSercosJobFixedStop()noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.cmdJobs.FixedStop");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos profile cmdJobs FixedStop class.");
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

  /// @brief   This method test the private function validateConfig
  /// @return  S_OK, when tests were successful
  HRESULT TestvalidateConfig();

private: //data  
  test::MockAxisProfileTrace m_mockProfileTrace {};             ///< Mockup of the Profile trace interface used in test
  test::MockScaling m_mockScaling {};                           ///< Mockup of the scaling interface used in test
  test::MockAxisProfileDeviceParam m_mockDeviceParam {};        ///< Mockup of the device parameter interface used in test 
  std::unique_ptr<AxisProfileSharingHandler> m_sharingHandlerHolder;  ///< unique pointer to the sharing handler
  IAxisProfileSharingHandler* m_sharingHandler {};               ///< pointer to the data sharing handler
};
