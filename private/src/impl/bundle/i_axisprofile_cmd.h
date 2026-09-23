#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and define the interface of command.
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is interface for axisprofile command
  class IAxisProfileCmd
  {
  public: // methods

    /// @brief Destructor
    virtual ~IAxisProfileCmd() noexcept = default;

    /// @brief Get command state
    /// @param[in] cmdType      command type
    /// @return state of command
    virtual CmdState getCmdState(CmdType cmdType) = 0;

    /// @brief Reset a axisprofile command
    /// @param[in]  cmdType     command type
    /// @return Status of the function call, STS_OK or an error code on failure
    virtual AxisProfileResult resetCmd(CmdType cmdType) = 0;

    /// @brief Abort a axisprofile command
    /// @param[in]  cmdType     command type
    /// @return Status of the function call, STS_OK or an error code on failure
    virtual AxisProfileResult abortCmd(CmdType cmdType) = 0;

    /// @brief Get command status in device
    /// @param[in]  cmdType     command type
    /// @return Status of command in device
    virtual CmdStatusInDevice getCmdStatusInDevice(CmdType cmdType) = 0;

    /// @brief Command of clearing device error
    /// @param[out] cmdType     command type
    /// @return Status of the function call, STS_OK or an error code on failure
    virtual AxisProfileResult clearDeviceError(CmdType& cmdType) = 0;

    /// @brief Mockup of command of clearing device error wrapper that can be cycle invoked
    /// @return Status of the function call, STS_OK or an error code on failure
    virtual AxisProfileResult clearDeviceError2() = 0;

    /// @brief Command of set absolute position
    /// @param[in]  newPos      new position
    /// @param[in]  encoder     index of encoder to be set
    /// @param[out] cmdType     type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    virtual AxisProfileResult setAbsolutePosition(double newPos, AxisProfileEncoder encoder, CmdType& cmdType) = 0;

    /// @brief Command of triggering searching new reference of device
    /// @param[in]  newPos      new position
    /// @param[in]  encoder     index of encoder to be set
    /// @param[out] cmdType     type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    virtual AxisProfileResult referencingAxis(double newPos, AxisProfileEncoder encoder, CmdType& cmdType) = 0;

    /// @brief Command of triggering fixed stop
    ///
    /// @param[in]  handle          pointer to the instantiated object
    /// @param[in]  clampingTorque  Clamping torque of fixedstop
    /// @param[out] cmdType         type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    virtual AxisProfileResult fixedStop(double clampingTorque, CmdType& cmdType) = 0;

    /// @brief Command of triggering setting new reference without movement.
    /// @param[in]  newPos        new position to be set when referencing has done
    /// @param[in]  encoder       index of encoder to be set
    /// @param[in]  refMode       reference mode, could be set new reference to device or control or do nothing after homing.
    /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    virtual AxisProfileResult setNewReferenceWithoutMovement(double newPos, AxisProfileEncoder encoder, RefMode refMode, CmdType& cmdType) = 0;
    
    /// @brief This function sets the given operation mode to the device, and then check if the operation mode is set to the target value.
    ///        It combines the set operation mode command and the check of the operation mode in one function, which is useful for cyclic calls.
    /// note:  This function is intended for use in real-time control loops where non-blocking behavior is required.
    ///        why need cyclic:
    ///        In CoE protocol, the operation mode is set by writing to the operation mode object in the device.
    ///        This progress can be done by SDO or PDO, so this interface is provided to support both methods.
    ///        The SDO progress need more than one cycle to finish, before finish, will return STS_PENDING.
    ///        if the operation mode data is set by PDO, this function will return STS_OK with much less cycles.
    ///        
    /// @param[in] opMode The target operation mode to be set on the device. This should be a valid AxisProfileOperationMode value.
    /// @return STS_OK will be returned if the operation mode is already set to the desired value, allowing for efficient cyclic calls.
    ///         STS_OK will be returned if the operation mode already in target mode before command execution.
    ///         STS_PENDING if the operation mode is not set to the target value yet.
    virtual AxisProfileResult switchOperationModeWithCheck(AxisProfileOperationMode opMode) = 0;
  };
}
