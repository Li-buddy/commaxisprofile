#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
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

//! @class TestCanopenStatusControl
//! Test for the can status-/controlword class
class TestCanopenStatusControl : public xTST_Test2
{
public:
  //! Constructor
  TestCanopenStatusControl();
  //! Destructor
  virtual ~TestCanopenStatusControl() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.StatusControl");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen profile status- / controlword class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_SD);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_SD);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //data
  //! @struct TestCaseStatusword
  //! @brief This struct contains the test data of the different test cases.
  struct TestCaseStatusword
  {
    const wchar_t *m_name{};                               //!< name of testcase
    std::uint16_t m_statusWord{};                          //!< test value for statusword
    bool m_expectedErrorValue{};                           //!< execute datalayer callback
    comm::axisprofile::AxisProfileResult m_expectedResult; //!< expected result of called method
  };

  //! @struct TestCaseControlword
  //! @brief This struct contains the test data of the different test cases.
  struct TestCaseControlword
  {
    const wchar_t *m_name{};                                       //!< name of testcase
    comm::axisprofile::AxisProfileResult m_expectedResult;         //!< expected result of called method
    comm::axisprofile::AxisProfileOperationMode m_commandedOpMode; //!< operation mode to set
    std::uint16_t dataResponse{};                                  //!< datalayer response
  };

  //! @struct TestCaseControlword
  //! @brief This struct contains the test data of the different test cases.
  struct TestPowerMode
  {
    const wchar_t* m_name{};                                      //!< name of testcase
    std::vector<std::uint16_t> m_statusWords;                     //!< test value for statusword
    comm::axisprofile::AxisProfileResult m_expectedResult;        //!< expected result of called method
    comm::axisprofile::AxisProfilePowerMode m_commandedPowerMode; //!< power mode to set
  };

  //! @struct TestCaseFeedbackStatus
  //! @brief This struct contains the test data of the different test cases.
  struct TestFeedbackStatus
  {
    std::uint8_t m_opModeDisplay{};                             //!< operation mode display.
    std::uint8_t m_opMode{};                                    //!< operation mode.
    comm::axisprofile::AxisProfileResult m_expectedResult;      //!< expected result of called method
  };

private: //method
  //! @brief   This test validates the statusword functionality.
  //! @return   S_OK, if test was successful
  HRESULT testStatusword();

  //! @brief   This method runs a testcase for the statusword.
  //! @param[in]   testcase  test case
  //! @return   S_OK, if test was successful
  HRESULT runTestcaseStatusword(TestCaseStatusword *testcase) const;

  //! @brief   This test validates the set operation mode functionality
  //! @return   S_OK, if test was successful
  HRESULT testSetOperationMode();

  //! @brief   This test validates the set power mode functionality.
  //! @return   S_OK, if test was successful
  HRESULT testSetPowerMode();

  //! @brief   This test executes testSetPowerMode
  //! @param[in]   testcase  test case
  //! @param[in]   testControlword     control word
  //! @param[in]   atData      at telegram
  //! @return   S_OK, if test was successful
  HRESULT runTestSetPowerMode(const TestPowerMode *testcase, comm::axisprofile::canopen::CanopenStatusControlDrive &testControlword, comm::axisprofile::AcknowledgeTelegram &atData) const;

  //! @brief   This method runs a testcase for the testSetOperationMode.
  //! @param[in]   testcase  test case
  //! @param[in]   testControlword     control word
  //! @param[in]   mockClient     datalayer client
  //! @return   S_OK, if test was successful
  HRESULT runTestSetOperationMode(const TestCaseControlword *testcase, comm::axisprofile::canopen::CanopenStatusControlDrive &testControlword, test::MockDatalayerClient &mockClient);

  //! @brief   This test set the axis to halt state.
  //! @return   S_OK, if test was successful 
  HRESULT testSetDriveHalt() const;

  //! @brief   This method sets a test variant for the client response (async calls).
  //!
  //! @param[in]    client      data layer client to execute datalayer access
  //! @param[in]    data        test data for the variant
  void setResponseVariant(test::MockDatalayerClient *const client, const std::uint16_t &data);

  /// @brief   Testing of checking whether device follows command value functionality.
  /// @return  S_OK, if test was successful
  HRESULT testCheckDeviceFollowsControl() const;

  /// @brief   Testing of the checking about the supported operation mode.
  /// @return  S_OK, if test was successful
  HRESULT testCheckOperationModeSupported();

  /// @brief   Testing of the determine canopen operation mode.
  /// @return  S_OK, if test was successful
  HRESULT testDetermineCanopenOpMode();

  /// @brief   Testing of misc methods
  /// @return  S_OK, if test was successful
  HRESULT testMics();

private: // data

  const std::string m_profileName = "testProfile";   //!< Name of the test axis profile
};
