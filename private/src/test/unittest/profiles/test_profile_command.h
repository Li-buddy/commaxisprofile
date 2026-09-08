#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// Mockups
#include "mockup/mock_profile_trace.h"

// Test Class
#include "profiles/category/drive/axisprofile_command_drive.h"


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

//! @class TestProfileCommand
//! Test class for the profile command base class.
class TestProfileCommand : public xTST_Test2
{
public:
  //! Constructor
  TestProfileCommand();
  //! Destructor
  virtual ~TestProfileCommand() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Command");
  TEST_DECLARE_DESCRIPTION("Unittest of the profile command base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  //! @brief Test the command C-Interface.
  //! @return  S_OK, when tests were successful
  HRESULT testCInterface();

private: // Data

  std::uint32_t m_deviceAddress = 0;             ///< Device address used in this test
  std::string m_profileName = "testProfile";     ///< Profile name used in this test

private: // Types

  //! @class AxisProfileCommandUnittest
  //! Specialization of the test class just to overwrite the pure virtual methods and make it creatable. 
  class AxisProfileCommandUnittest : public comm::axisprofile::AxisProfileCommandDrive
  {
  public: // Methods

    explicit AxisProfileCommandUnittest(comm::axisprofile::ProfileBasicInfo& profileBasicInfo) :
    comm::axisprofile::AxisProfileCommandDrive(profileBasicInfo) {};
    virtual ~AxisProfileCommandUnittest() = default;
    comm::axisprofile::AxisProfileResult clearDeviceError() final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult setAndCheckPositiveStop(const comm::axisprofile::AxisProfilePositiveStop /*mode*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult setAndCheckClampingTorque(const double /*clampingTorque*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult getClampingTorque(double* const /*clampingTorque*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult checkStandStill(bool* const /*standStill*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult setAndCheckStandStillWindow(const double /*standStillWindow*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult getStandStillWindow(double* const /*standStillWindow*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult activateCyclicStandStillCheck() final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult deactivateCyclicStandStillCheck() final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult setAbsolutePosition(const double /*position*/, const comm::axisprofile::AxisProfileEncoder /*encoder*/, const bool /*ifDriveEnabled*/) override {return comm::axisprofile::AxisProfileResult::STS_OK;};
    comm::axisprofile::AxisProfileResult resetAbsolutePosition() override {return comm::axisprofile::AxisProfileResult::STS_OK;};
    comm::axisprofile::AxisProfileResult referencingAxis(const double /* newRefPos */, const comm::axisprofile::AxisProfileEncoder /* encoder */) override {return comm::axisprofile::AxisProfileResult::STS_OK;};
    comm::axisprofile::AxisProfileResult resetReferencingAxis() override {return comm::axisprofile::AxisProfileResult::STS_OK;};
    comm::axisprofile::AxisProfileResult setReferencingPosition(const double /* refPos */, const comm::axisprofile::AxisProfileEncoder /* encoder */, comm::axisprofile::RefMode/*method*/) override {return comm::axisprofile::AxisProfileResult::STS_OK;};
    comm::axisprofile::AxisProfileResult addAuxCyclicJobs() override {return comm::axisprofile::AxisProfileResult::STS_OK;};
    comm::axisprofile::AxisProfileResult checkIfDeviceHalt(bool& /*isHalted*/) override { return comm::axisprofile::AxisProfileResult::STS_OK;};
    void loadBootingTasks() override {}
  };
};
