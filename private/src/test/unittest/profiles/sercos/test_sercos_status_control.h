#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "profiles/sercos/sercos_defs.h"
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

//! @class TestSercosStatusControl
//! Test for the sercos status-/controlword class
class TestSercosStatusControl : public xTST_Test2
{
public:
  //! Constructor
  TestSercosStatusControl() = default;
  //! Destructor
  virtual ~TestSercosStatusControl() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.StatusControl");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos profile status- / controlword class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //data

  /// @brief This structure make a pair for profile operation mode and sercos operation mode
  struct TestCaseOpModePair
  {
    comm::axisprofile::sercos::OperationMode m_sercosOpMode;      ///< sercos operation mode
    comm::axisprofile::AxisProfileOperationMode m_profileOpMode;  ///< profile operation mode
  };

  /// @brief This structure contains operation mode supported information.
  struct TestCaseOpModeSupported
  {
    bool m_opModeAvailable;      ///< operation mode available information from drive
    bool m_opModeInputAssigned;  ///< operation mode input assigned information from profile variable
    bool m_opModeOutputAssigned; ///< operation mode output assigned information from profile variable
  };

  //! @struct TestCaseStatusword
  //! @brief This structure contains the test data of the different test cases.
  struct TestCaseStatusword
  {
    const wchar_t *m_name;                                          //!< name of testcase
    std::uint16_t m_statusWord;                                          //!< test value for statusword
    comm::axisprofile::AxisProfileResult m_expectedResult;          //!< expected result of analyzeStatusWord
    bool m_expectedErrorValue;                                      //!< execute datalayer callback
    bool m_expectedStandStill;                                      //!< execute standstill state
    bool m_isDeviceFollowsControl;                                  //!< execute is device follows control
    TestCaseOpModeSupported m_opModeSupported;                      //!< expected support operation mode
    comm::axisprofile::sercos::OperationMode m_sercosOpMode;        //!< expected sercos operation mode
    comm::axisprofile::AxisProfileOperationMode m_expectedOpMode;   //!< expected operation mode
    comm::axisprofile::AxisProfilePowerMode m_expectedPowerMode;    //!< expected power mode
  };

  //! @struct TestCaseControlword
  //! @brief This structure contains the test data of the different test cases.
  struct TestCaseControlword
  {
    const wchar_t *m_name;                                           //!< name of testcase
    std::uint16_t m_statusWord;                                      //!< test value for statusword
    comm::axisprofile::AxisProfileResult m_expectedResult;           //!< expected result of called method
    TestCaseOpModeSupported m_opModeSupported;                       //!< expected support operation mode
    TestCaseOpModePair m_opModePair;                                 //!< available operation mode pair
    comm::axisprofile::AxisProfileOperationMode m_commandedOpMode;   //!< operation mode to set
    bool m_testPowerMode;                                            //!< execute setPowerMode
    comm::axisprofile::AxisProfilePowerMode m_commandedPowerMode;    //!< power mode to set

  };

private: //method
  
  //! @brief   This test validates the statusword functionality.
  //! @return   S_OK, if test was successful 
  HRESULT testStatusword();

  //! @brief   This method runs a testcase for the statusword.
  //! @return   S_OK, if test was successful 
  HRESULT runTestcaseStatusword(TestCaseStatusword *testcase);

  //! @brief   This test validates the controlword functionality.
  //! @return   S_OK, if test was successful 
  HRESULT testControlword();

  //! @brief   This method runs a testcase for the statusword.
  //! @return   S_OK, if test was successful 
  HRESULT runTestcaseControlword(TestCaseControlword *testcase);

  //! @brief   This test set the axis to halt state.
  //! @return   S_OK, if test was successful 
  HRESULT testSetDriveHalt();

};
