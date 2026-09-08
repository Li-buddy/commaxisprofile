#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the main
///        interface for all axisprofile commands. This is a sub-interface of 'IAxisProfile'.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
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
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for all axisprofile commands.
  class IAxisProfileCommand
  {
  public: // methods

    virtual ~IAxisProfileCommand() noexcept = default;

    /// @brief This method triggers a error clear on the device.
    ///
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult clearDeviceError() = 0;

    /// @brief This method enables positive stop on the device.
    ///
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setAndCheckPositiveStop(const comm::axisprofile::AxisProfilePositiveStop mode) = 0;

    /// @brief This method sets Clamping Torque
    /// @param[in]   clampingTorque     clampingTorque value to set
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setAndCheckClampingTorque(const double clampingTorque) = 0;

    /// @brief This method gets Clamping Torque
    /// @param[out]  clampingTorque     the address to store clampingTorque value 
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult getClampingTorque(double* const clampingTorque) = 0;

    /// @brief This method checks stand still
    /// @param[out]  standStill         the address to store standstill result true: standstill, false: moving
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult checkStandStill(bool* const standStill) = 0;

    /// @brief This method sets standstill window
    /// @param[in]   standStillWindow   StandStillWindow value to set (m/s or degree/s)
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setAndCheckStandStillWindow(const double standStillWindow) = 0;

    /// @brief This method gets standstill window
    /// @param[out]  standStillWindow   The address to store the standstill window value (m/s or degree/s)
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult getStandStillWindow(double* const standStillWindow) = 0;

    /// @brief This method activates cyclicStandStillCheck() method declared in i_axisprofile_status_control.h
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult activateCyclicStandStillCheck() = 0;

    /// @brief This method deactivates cyclicStandStillCheck() method declared in i_axisprofile_status_control.h
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult deactivateCyclicStandStillCheck() = 0;

    /// @brief This method set absolute position of the drive
    /// @param[in]   position           The position value to set (degree, or meter)
    /// @param[in]   encoder            The encoder value to set  (encoder 1 or encoder 2)
    /// @param[in]   ifDriveEnabled     If the user wants to trigger the command while drive in enabled state (for sercos drive, i.e. AF state)
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setAbsolutePosition(const double position, const AxisProfileEncoder encoder, const bool ifDriveEnabled) = 0;

    /// @brief This method resets absolute position command
    /// @return      Return value indicating success or error
    virtual AxisProfileResult resetAbsolutePosition() = 0;
      
    /// @brief This method references the drive
    /// @param[in]   newRefPos          Referencing position after homing
    /// @param[in]   encoder            The encoder value to set  (encoder 1 or encoder 2)
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult referencingAxis(const double newRefPos, const AxisProfileEncoder encoder) = 0;

    /// @brief This method resets referencingAxis
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult resetReferencingAxis() = 0;
      
    /// @brief This method get whether the axis is homed
    /// @param[out]  isHomed            indicate if axis homed  
    /// @return      Returns whether the axis is homed.
    virtual AxisProfileResult checkAxisHomed(bool& isHomed) = 0;

    /// @brief This method reset the axis profile command state
    /// @param[in]   type               Type of the command
    /// @return                         Return value indicating success or error.
    virtual AxisProfileResult resetAxisProfileCommandState(const AxisProfileCommandType type) = 0;

    /// @brief This method abort the axis profile command
    /// @param[in]   type               Type of the command
    /// @return                         Return value indicating success or error.
    virtual AxisProfileResult abortAxisProfileCommand(const AxisProfileCommandType type) = 0;

    /// @brief This method get state of the command
    /// @param[in]   type               Type of the command
    /// @return                         Return value indicating success or error.
    virtual AxisprofileCommandState getAxisProfileCommandState(const AxisProfileCommandType type) = 0;

    /// @brief This method set referencing position the drive
    /// @param[in]   refPos          Set referencing position homing
    /// @param[in]   encoder         The encoder value to set  (encoder 1 or encoder 2)
    /// @param[in]   mode            Use which mode to set new position
    /// @return                      Return value indicating success or error.
    virtual AxisProfileResult setReferencingPosition(const double refPos, const AxisProfileEncoder encoder, RefMode mode) = 0;

    /// @brief This method get determined axis profile command state
    /// @param[in]   cmdState        Command state
    /// @param[out]  vecCmdType      Commands in determined state
    /// @return                      Return value indicating success or error.
    virtual AxisProfileResult getCommandsAtDeterminedState(const comm::axisprofile::AxisprofileCommandState cmdState, std::vector<comm::axisprofile::AxisProfileCommandType>& vecCmdType) = 0;

  };
}
