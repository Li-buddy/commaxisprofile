#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "profiles/axisprofile_cyclic_data.h"

#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_diag_msg.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_probe.h"
#include "mockup/mock_axisprofile_status_control.h"

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

/// @brief Test for the canopen cyclic data class
class TestCanopenCyclicData : public xTST_Test2
{
public:
  /// @brief Constructor
  TestCanopenCyclicData() = default;

  /// @brief Destructor
  virtual ~TestCanopenCyclicData() noexcept = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.CyclicData");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen profile cyclic data class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: //data

  /// @brief This struct contains the test data of the different test cases.
  struct TestCaseDigitalInput
  {
    const wchar_t* m_name;                                     ///< name of testcase
    comm::axisprofile::DigitalInputValueBitType m_valueType;   ///< test value type of digital input bit
    comm::axisprofile::AxisProfileResult m_expectedResult;     ///< expected result of getDigitalInputValueBit()
  };

private: //method

  /// @brief   test create function
  /// @return  S_OK, when the test was successful
  HRESULT testClass();

  /// @brief Test the axisprofile Interface.
  /// @return  S_OK, when tests were successful
  HRESULT testInterface();

  /// @brief Test for drive category adapter with CoE protocol 
  /// @return  S_OK, when tests were successful
  HRESULT testForFPUError();

  /// @brief Do test cases for getting the safety operation mode.
  /// @param[in] testCyclicDataDrive   Points the tested object.
  /// @param[in] ptrAtData  Points the cyclic data object for testing.
  /// @param[in] mappingHandler        The axis profile mapping handler for testing.
  /// @return  S_OK, when tests were successful
  HRESULT testGettingSafetyOperationMode(AxisProfileCyclicData* testCyclicDataDrive, AcknowledgeTelegram* ptrAtData,
                                         test::MockAxisProfileMappingHandler& mappingHandler);

  test::MockAxisProfileCommand* m_cmdMock {};                   ///< Mockup of the command interface used in test
  test::MockAxisProfileStatusControl* m_statusControlMock {};   ///< Mockup of the status control interface used in test
  test::MockAxisProfileDiagMsg* m_diagMsgMock {};               ///< Mockup of the diagnostic message interface used in test
  test::MockAxisProfileDeviceParam* m_deviceParamMock {};       ///< Mockup of the device parameter interface used in test 
  test::MockAxisProfileProbe* m_probeMock {};                   ///< Mockup of the probe interface used in test
};
