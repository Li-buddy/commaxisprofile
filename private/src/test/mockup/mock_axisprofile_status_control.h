#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_axisprofile_status_control.h
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
//! the axis profile status/control interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_status_control_internal.h"


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

  //! @class MockAxisProfileStatusControl
  //! @brief This class implements a mockup for the axis profile status/control interface.
  class MockAxisProfileStatusControl : public comm::axisprofile::IAxisProfileStatusControlInternal
  {
  public: // methods

    //! @brief Constructor
    MockAxisProfileStatusControl() = default;
    //! @brief Destructor
    virtual ~MockAxisProfileStatusControl() noexcept override = default;

    const IAxisProfileStatusControlC* getCInterface() const override { return nullptr; }
    comm::axisprofile::AxisProfileResult booting() override { return m_returnValue; }

    // --------------------------------------------- Statusword -----------------------------------------------

    std::uint16_t getStatusWord() const override { return m_statusWord; }
    bool checkOperationMode(const comm::axisprofile::AxisProfileOperationMode /*opMode*/) const override { return true; }
    bool checkOperationModeSupported(const comm::axisprofile::AxisProfileOperationMode /*opMode*/, comm::axisprofile::OpModeErrorInfo& /*errorInfo*/) override { return true; }
    comm::axisprofile::AxisProfileOperationMode getOperationMode() const override { return m_operationMode; }
    comm::axisprofile::AxisProfileResult readOperationMode(comm::axisprofile::AxisProfileOperationMode& operationMode) override
    {
      operationMode = m_operationMode;
      return m_returnValue;
    }
    bool checkPowerMode(const comm::axisprofile::AxisProfilePowerMode /*powerMode*/) const override { return true; }
    comm::axisprofile::AxisProfilePowerMode getPowerMode() const override { return m_powerMode; }
    bool checkError() const override { return m_deviceError; }
    bool cyclicStandStillCheck() const override { return m_deviceError; }
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/)override {};
    void loadBootingTasks() override {}
    bool isDeviceFollowsControl() const override { return true; }
    bool deviceHasWarning() const override {return false;} 
    comm::axisprofile::ReadyToOperate checkReadyToOperate() override {return comm::axisprofile::ReadyToOperate::DRIVE_ENABLED;}
    comm::axisprofile::AxisProfileResult analyzeStatusWord() override {return m_returnValue;}
    
    // -------------------------------------------- Controlword -----------------------------------------------

    std::uint16_t getControlWord() const override { return m_controlWord; }
    comm::axisprofile::AxisProfileResult setControlWord(std::uint16_t /*controlWord*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult setOperationMode(const comm::axisprofile::AxisProfileOperationMode opMode) override
    {
      if (aprStatusSucceeded(m_returnValue)) m_operationMode = opMode;
      return m_returnValue;
    }
    comm::axisprofile::AxisProfileResult setPowerMode(const comm::axisprofile::AxisProfilePowerMode powerMode) override
    {
      if (aprStatusSucceeded(m_returnValue)) m_powerMode = powerMode;
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult setDriveHaltMode(const bool /*haltEnable*/) override {return m_returnValue;} 
    comm::axisprofile::AxisProfileResult getDriveHaltMode(bool& /*isHaltEnabled*/) const override {return m_returnValue;} 

    // -------------------------------------------- Test Methods ----------------------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

  public: // data

    bool m_deviceError = false;   //!< Flag that indicates, if a device error is currently simulated
    bool m_haltEnable = false;    //!< Flag that indicates, if halt enable is activated
    std::uint16_t m_statusWord {0}; //!< Status word of the device, used for testing purposes
    std::uint16_t m_controlWord {0}; ///< Control word of the device, used for testing purposes

  protected: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   //!< Result of member functions

  private: // data

    comm::axisprofile::AxisProfilePowerMode m_powerMode = comm::axisprofile::AxisProfilePowerMode::POWER_ON;                  //!< Current power mode
    comm::axisprofile::AxisProfileOperationMode m_operationMode = comm::axisprofile::AxisProfileOperationMode::POSITIONING;   //!< Current operation mode
  };
}
