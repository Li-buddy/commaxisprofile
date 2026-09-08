#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "profiles/scaling/scaling_strategy_factory.h"
#include <vector>

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

//! @class TestScalingFactory
//! Test class for the sercos scaling class.
class TestScalingFactory : public xTST_Test2
{
public: // Methods

  //! Constructor
  TestScalingFactory();
  //! Destructor
  virtual ~TestScalingFactory() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Scaling.ScalingFactory");
  TEST_DECLARE_DESCRIPTION("Unittest of the ScalingFactory class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);

  //! @brief Init function of the test.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief End function of the test.
  //! @return S_OK, when deinitialization successful.
  HRESULT doEnd();

private: // Methods

  //! @brief    Test of method to check scaling compatiblity.
  //! @return   S_OK, if test was successful 
  HRESULT testForCheckCompatible();

  //! @brief    Test of method to create strategy.
  //! @return   S_OK, if test was successful 
  HRESULT testForCreateStrategy();

  //! @brief    Test of method to get and set.
  //! @return   S_OK, if test was successful
  HRESULT testForGetterSetter();

private: // Data
  const std::string m_profileName = "testProfile";   //!< Name of the test axis profile
};
