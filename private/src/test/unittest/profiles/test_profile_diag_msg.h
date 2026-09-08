#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// Mockups
#include "mockup/mock_profile_trace.h"

// Test Class
#include "profiles/axisprofile_diag_msg.h"


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

//! @class TestProfileDiagMsg
//! Test class for the profile diagnostic message base class.
class TestProfileDiagMsg : public xTST_Test2
{
public:
  //! Constructor
  TestProfileDiagMsg();
  //! Destructor
  virtual ~TestProfileDiagMsg() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.DiagMsg");
  TEST_DECLARE_DESCRIPTION("Unittest of the profile diagnostic message base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PRZ);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_JG);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // types

  /// @struct TestCaseDiagMsg
  /// @brief This struct contains the test data of the different test cases.
  struct TestCaseDiagMsg
  {
    const wchar_t *m_name;                                     ///< name of testcase
    std::uint32_t m_diagMsg;                                        ///< test value for diagnostic message
    comm::axisprofile::AxisProfileResult m_expectedResult;     ///< expected result of analyzeDiagMsg
    comm::axisprofile::AxisProfileDeviceErrorType m_errorType; ///< expected error type
  };

private: // methods

  //! @brief Test the command C-Interface.
  //! @return  S_OK, when tests were successful
  HRESULT testCInterface();

  /// @brief   This test validates the diagnostic message functionality.
  /// @return   S_OK, if test was successful 
  HRESULT testDiagMsg();

  /// @brief   This method runs a testcase for the diagnostic message.
  /// @return   S_OK, if test was successful 
  HRESULT runTestcaseDiagMsg(TestCaseDiagMsg* testcase);

private: // data

  std::string m_profileName = "testProfile";     //!< Profile name used in this test
};
