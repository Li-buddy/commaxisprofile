#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// Mockups
#include "mockup/mock_profile_trace.h"

// Test Class
#include "profiles/category/drive/axisprofile_status_control_drive.h"


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

//! @class TestProfileStatusControl
//! Test class for the profile status control base class.
class TestProfileStatusControl : public xTST_Test2
{
public:
  //! Constructor
  TestProfileStatusControl();
  //! Destructor
  virtual ~TestProfileStatusControl() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.StatusControl");
  TEST_DECLARE_DESCRIPTION("Unittest of the profile status control base class.");
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

  std::string m_profileName = "testProfile";     //!< Profile name used in this test

private: // Types

  //! @class AxisProfileStatusControlUnittest
  //! Specialization of the test class just to overwrite the pure virtual methods and make it creatable. 
  class AxisProfileStatusControlUnittest : public comm::axisprofile::AxisProfileStatusControlDrive
  {
  public: // Methods

    explicit AxisProfileStatusControlUnittest(comm::axisprofile::ProfileBasicInfo& profileBasicInfo) : comm::axisprofile::AxisProfileStatusControlDrive(profileBasicInfo) {};
    virtual ~AxisProfileStatusControlUnittest() = default;

    // --------------------------------------------- Statusword ----------------------------------------------- //

    std::uint16_t getStatusWord() const final { return m_testStatusWord; }
    bool checkError() const final { return false; }
    bool cyclicStandStillCheck() const final { return false; }
    bool checkOperationModeSupported(const comm::axisprofile::AxisProfileOperationMode /*opMode*/, comm::axisprofile::OpModeErrorInfo& /*errorInfo*/) final { return false; }
    comm::axisprofile::AxisProfileOperationMode getOperationMode() const final { return comm::axisprofile::AxisProfileOperationMode::POSITIONING; };
    bool isDeviceFollowsControl() const final { return false; }
    bool deviceHasWarning() const final { return false;}
    comm::axisprofile::ReadyToOperate checkReadyToOperate() final { return comm::axisprofile::ReadyToOperate::DRIVE_NOT_READY; }
    
    // -------------------------------------------- Controlword ----------------------------------------------- //

    std::uint16_t getControlWord() const final { return m_testControlWord; }
    comm::axisprofile::AxisProfileResult setControlWord(std::uint16_t /*controlWord*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult setOperationMode(const comm::axisprofile::AxisProfileOperationMode /*opMode*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult setPowerMode(const comm::axisprofile::AxisProfilePowerMode /*powerMode*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }

    // -------------------------------------------- Drive Halt ----------------------------------------------- //

    comm::axisprofile::AxisProfileResult setDriveHaltMode(const bool /*haltEnable*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult getDriveHaltMode(bool& /*isHaltEnabled*/) const override { return comm::axisprofile::AxisProfileResult::STS_OK; }

    // ----------------------------------------- Internal Methods --------------------------------------------- //

    comm::axisprofile::AxisProfileResult analyzeStatusWord() final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    void loadBootingTasks() override {}

  public: // Data

    const std::uint16_t m_testStatusWord = 0x1234;   ///< Statusword used in test
    const std::uint16_t m_testControlWord = 0x1234;  ///< Controlword used in test
  };
};
