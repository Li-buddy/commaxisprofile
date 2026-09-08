#pragma once
#include "stdafx.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_diag_msg.h"
#include "mockup/mock_axisprofile_probe.h"
#include "mockup/mock_axisprofile_status_control.h"

using namespace comm::axisprofile;

//
// test class
//
class TestCategoryAdapterDrive : public xTST_Test2
{
public:
  TestCategoryAdapterDrive() = default;
  virtual ~TestCategoryAdapterDrive() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.CategoryAdapterDrive");
  TEST_DECLARE_DESCRIPTION("Test of CategoryAdapterDrive class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;

private:

  /// @brief   test create function
  /// @return  S_OK, when the test was successful
  HRESULT testClass();

  /// @brief Test the axisprofile Interface.
  /// @return  S_OK, when tests were successful
  HRESULT testInterface();

  test::MockAxisProfileCommand* m_cmdMock {};                   ///< Mockup of the command interface used in test
  test::MockAxisProfileStatusControl* m_statusControlMock {};   ///< Mockup of the status control interface used in test
  test::MockAxisProfileDiagMsg* m_diagMsgMock {};               ///< Mockup of the diagnostic message interface used in test
  test::MockAxisProfileDeviceParam* m_deviceParamMock {};       ///< Mockup of the device parameter interface used in test 
  test::MockAxisProfileProbe* m_probeMock {};                   ///< Mockup of the probe interface used in test
};
