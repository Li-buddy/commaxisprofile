#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_axisprofile_command.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2020
//
// ---------------------------------------------------------------------------
//
//! This file is part of the axisprofile library and implements a mockup for
//! the axis profile command interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_command_internal.h"


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockAxisProfileCommand
  //! @brief This class implements a mockup for the axis profile command interface.
  class MockAxisProfileCommand : public comm::axisprofile::IAxisProfileCommandInternal
  {
  public: // methods

    //! @brief Constructor
    MockAxisProfileCommand() = default;
    //! @brief Destructor
    virtual ~MockAxisProfileCommand() noexcept override = default;

    comm::axisprofile::AxisProfileResult clearDeviceError() override {return m_returnValue;}
    const IAxisProfileCommandC* getCInterface() const override {return nullptr;}
    comm::axisprofile::AxisProfileResult setAndCheckPositiveStop(const comm::axisprofile::AxisProfilePositiveStop /*mode*/) override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult setAndCheckClampingTorque(const double /*clampingTorque*/) override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult getClampingTorque(double* const /*clampingTorque*/) override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult checkStandStill(bool* const /*standStill*/) override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult setAndCheckStandStillWindow(const double /*standStillWindow*/) override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult getStandStillWindow(double* const /*standStillWindow*/)  override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult activateCyclicStandStillCheck() override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult deactivateCyclicStandStillCheck() override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult resetAllAxisprofileCommands() override {return m_returnValue;};
    comm::axisprofile::AxisProfileResult setAbsolutePosition(const double /*position*/, const comm::axisprofile::AxisProfileEncoder /*encoder*/, const bool /*ifDriveEnabled*/) override {return m_returnValue;};
    comm::axisprofile::AxisProfileResult resetAbsolutePosition() override {return m_returnValue;};
    comm::axisprofile::AxisProfileResult referencingAxis(const double /* newRefPos */, const comm::axisprofile::AxisProfileEncoder /* encoder */) override {return m_returnValue;};
    comm::axisprofile::AxisProfileResult resetReferencingAxis() override {return m_returnValue;};
    comm::axisprofile::AxisProfileResult checkAxisHomed(bool& /*isHomed*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult booting() override {return m_returnValue;};
    comm::axisprofile::AxisProfileResult resetAxisProfileCommandState(const comm::axisprofile::AxisProfileCommandType /*type*/) override {return m_returnValue;};
    comm::axisprofile::AxisProfileResult abortAxisProfileCommand(const comm::axisprofile::AxisProfileCommandType /*type*/) override {return m_returnValue;};
    comm::axisprofile::AxisprofileCommandState getAxisProfileCommandState(const comm::axisprofile::AxisProfileCommandType /*type*/) override { return m_commandState;};
    comm::axisprofile::AxisProfileResult setReferencingPosition(const double /* refPos */, const comm::axisprofile::AxisProfileEncoder /* encoder */, comm::axisprofile::RefMode/*mode*/) override {return m_returnValue;};
    comm::axisprofile::AxisProfileResult getCommandsAtDeterminedState(comm::axisprofile::AxisprofileCommandState /*cmdState*/, std::vector<comm::axisprofile::AxisProfileCommandType>& /*activeCmds*/) override { return m_returnValue;};
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/)override {};
    void loadBootingTasks() override {}
    comm::axisprofile::AxisProfileResult addAuxCyclicJobs() override{ return m_returnValue; };
    comm::axisprofile::AxisProfileResult checkIfDeviceHalt(bool& /*isHalted*/) override { return m_returnValue; };

    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }  
  
  public: //data
    std::vector<std::function<comm::axisprofile::AxisProfileResult()>> m_initCommands{};  //!< mockup of the vector which contains init methods
    comm::axisprofile::AxisprofileCommandState m_commandState {comm::axisprofile::AxisprofileCommandState::CMD_STATE_IDLE};

  protected: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   //!< Result of member functions
  };
}
