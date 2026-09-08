#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <vector>
#include <map>
#include "stdafx.h"

#include "profiles/scaling/unit_helper.h"

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
//! @class TestUnitHelper
//! Unit test functions for the UnitHelper class
class TestUnitHelper : public xTST_Test2
{
public:
  //! @brief Constructor
  TestUnitHelper() = default;
  //! @brief Destructor
  virtual ~TestUnitHelper() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("UnitHelper");
  TEST_DECLARE_DESCRIPTION("Test of UnitHelper class");
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);

  //! @brief   main test function
  //! @return  S_OK, when the test was successful
  HRESULT doTest();

private: // methods

  //! @brief Run valid test case where everything should work as expected
  //! @return Indicates success or error
  HRESULT runValidTestCase();

  //! @brief Tests errerneous test cases
  //! @return Indicates success or error
  HRESULT runErrorTestCase();


  //! @brief Run test case to convert linear velocities
  //! @return Indicates success or error
  HRESULT runTestCaseVelocityLinear();

  //! @brief Run test case to convert rotational velocities
  //! @return Indicates success or error
  HRESULT runTestCaseVelocityRotational();

  //! @brief Run test case to convert strings to units and back
  //! @return Indicates success or error
  HRESULT runTestCaseStringConvert();

  //! @brief Run test case to browse all types for linear jerk
  //! @return Indicates success or error
  HRESULT runTestCaseBrowse();

  //! @brief Run test case to init the general configuration with default values
  //! @return Indicates success or error
  HRESULT runTestCaseDefaults();

  //! @brief check a double value with an epsilon
  //! @param[in] value      value to check
  //! @param[in] comment    comment that is used for error text
  //! @param[in] expected   expected value for comparison
  //! @param[in] epsilon    epsilon value that is used for the check
  //! @return indication success or error
  bool checkDoubleValue(double value, const WCHAR* comment, double expected, double epsilon);

  //! @brief check a bool value
  //! @param[in] value      value to check
  //! @param[in] comment    comment that is used for error text
  //! @param[in] expected   expected value for comparison
  //! @return indication success or error
  bool checkBoolResult(bool value, const WCHAR* comment, bool expected);

  //! @brief check a unit value
  //! @param[in] value      value to check
  //! @param[in] comment    comment that is used for error text
  //! @param[in] expected   expected unit for comparison
  //! @return indication success or error
  bool checkUnit(comm::axisprofile::Unit value, const WCHAR* comment, comm::axisprofile::Unit expected);

private: // types

  //! @brief dummy class to test protected methods
  class DummyUnit : public comm::axisprofile::UnitHelper
  {
  public:
    //! @brief destructor
    virtual ~DummyUnit() noexcept override = default;

    //! @brief run the test of the protected methods
    //! @param[in]  base  pointer to the test for log output
    //! @return Indicates success or error
    HRESULT runTest(xTST_Test* base);
  };


};
