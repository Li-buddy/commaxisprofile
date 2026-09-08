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

//! @class FuncTestProfilesSercos
//! Test class for plc open state machine.
class FuncTestProfilesSercos : public FuncTestBase
{
public:
  //! Constructor
  FuncTestProfilesSercos() = default;
  //! Destructor
  virtual ~FuncTestProfilesSercos() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("FuncTest.Profiles.Sercos");
  TEST_DECLARE_DESCRIPTION("Functiontest of axisprofile SERCOS.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

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

  //! @brief This test validates, that the transmission between mdt and at is functional.
  //! @return   S_OK, if test was successful 
  HRESULT doDataTransmitReceiveTest();

  //! @brief This test validates that all functionality of the axisprofile SERCOS behaves as expected.
  //! @return   S_OK, if test was successful 
  HRESULT doSingleMethodTest();

  /// @brief This test runs device parameter access
  /// @return   S_OK, if test was successful 
  HRESULT doDeviceParam();

private: // data

  std::string m_profileName = "testProfile";              //!< Profile name used in this test
  comm::axisprofile::IAxisProfileInternal* m_profile {};  //!< test profile
  comm::axisprofile::AxisProfileType m_testProfileType = comm::axisprofile::AxisProfileType::UNDEFINED;             //!< Profile type used in test
  comm::axisprofile::NetworkTechnology m_testNetworkTechnology = comm::axisprofile::NetworkTechnology::UNDEFINED;   //!< Network technology used in test
};
