#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "functiontest/functest_base.h"

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

//! @class FuncTestProfilesCanopen
//! Test class for plc open state machine.
class FuncTestProfilesCanopen : public FuncTestBase
{
public:
  //! Constructor
  FuncTestProfilesCanopen() = default;
  //! Destructor
  virtual ~FuncTestProfilesCanopen() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("FuncTest.Profiles.Canopen");
  TEST_DECLARE_DESCRIPTION("Functiontest of axisprofile CiA402.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_SD);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_SD);

  //! @brief Init necessary motion system objects on test call.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief Free memory of allocated objects in doInit() test function.
  //! @return S_OK, when deletion successful.
  HRESULT doEnd();

private: // methods

  //! @brief   This test validates, that the transmission between mdt and at is functional.
  //! @return   S_OK, if test was successful 
  HRESULT doDataTransmitReceiveTest();

  //! @brief This test validates that all functionality of the axisprofile CiA402 behaves as expected.
  //! @return   S_OK, if test was successful 
  HRESULT doSingleMethodTest();

private: // data

  std::string m_profileName = "testProfile";                                                                        //!< Profile name used in this test
  comm::axisprofile::AxisProfileType m_testProfileType = comm::axisprofile::AxisProfileType::UNDEFINED;             //!< Profile type used in test
  comm::axisprofile::NetworkTechnology m_testNetworkTechnology = comm::axisprofile::NetworkTechnology::UNDEFINED;   //!< Network technology used in test
};
