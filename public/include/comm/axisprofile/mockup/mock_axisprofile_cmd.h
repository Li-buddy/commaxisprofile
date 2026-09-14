#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axisprofile command interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_cmd_c.h"

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

  /// @brief Mockup handle of axisprofile command interface.
  class AxisProfileCmdMockHandle
  {
  public: // data

    CmdState m_cmdState = CmdState::STATE_UNKNOWN;            ///< State of the command
    CmdStatusInDevice m_cmdStatusInDevice = CmdStatusInDevice::DEVICE_UNKNOWN;  ///< Status of command in the device
    AxisProfileResult m_retValue = AxisProfileResult::STS_OK; ///< Return value of C-Methods
    double m_homeNewPos {0.0};                                ///< New position for homing command
  };

  /// @brief Mockup of getting acutal state of axisprofile command
  /// 
  /// @param[in]  handle        pointer to the instantiated object
  /// @param[in]  cmdType       command type
  /// @return state of command
  extern "C" CmdState AXISPROFILE_getCmdStateMock(void* handle, CmdType cmdType);

  /// @brief Mockup resetting a axisprofile command
  /// 
  /// @param[in]  handle        pointer to the instantiated object
  /// @param[in]  cmdType       command type
  /// @return Status of the function call, STS_OK or an error code on failure
  extern "C" AxisProfileResult AXISPROFILE_resetCmdMock(void* handle, CmdType cmdType);

  /// @brief Mockup of aborting a axisprofile command
  ///
  /// @param[in]  handle        pointer to the instantiated object
  /// @param[in]  cmdType       command type
  /// @return Status of the function call, STS_OK or an error code on failure
  extern "C" AxisProfileResult AXISPROFILE_abortCmdMock(void* handle, CmdType cmdType);

  /// @brief Mockup of getting command status in device
  ///
  /// @param[in]  handle        pointer to the instantiated object
  /// @param[in]  cmdType       command type
  /// @return Status of command in the device
  extern "C" CmdStatusInDevice AXISPROFILE_getCmdStatusInDeviceMock(void* handle, CmdType cmdType);

  /// @brief Mockup of command of clearing device error
  ///
  /// @param[in]  handle        pointer to the instantiated object
  /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
  /// @return Status of the function call, STS_OK or an error code on failure
  extern "C" AxisProfileResult AXISPROFILE_clearDeviceErrorMockCmd(void* handle, CmdType& cmdType);

  /// @brief Mockup of command of clearing device error wrapper that can be cycle invoked.
  ///
  /// @param[in]  handle        pointer to the instantiated object
  /// @return Status of the function call, STS_OK or an error code on failure
  extern "C" AxisProfileResult AXISPROFILE_clearDeviceError2MockCmd(void* handle);

  /// @brief Mockup of command of setting new absolute position to device
  ///
  /// @param[in]  handle        pointer to the instantiated object
  /// @param[in]  newPos        new absolute position
  /// @param[in]  encoder       index of encoder to be set
  /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
  /// @return Status of the function call, STS_OK or an error code on failure
  extern "C" AxisProfileResult AXISPROFILE_setAbsolutePositionMockCmd(void* handle, double newPos, AxisProfileEncoder encoder, CmdType& cmdType);

  /// @brief Mockup of command of triggering searching new reference of device
  ///
  /// @param[in]  handle        pointer to the instantiated object
  /// @param[in]  newPos        new position to be set when referencing has done
  /// @param[in]  encoder       index of encoder to be set
  /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
  /// @return Status of the function call, STS_OK or an error code on failure
  extern "C" AxisProfileResult AXISPROFILE_referencingAxisMockCmd(void* handle, double newPos, AxisProfileEncoder encoder, CmdType& cmdType);

  /// @brief Mockup of command of triggering fixed stop
  ///
  /// @param[in]  handle          pointer to the instantiated object
  /// @param[in]  clampingTorque  Clamping torque of fixedstop
  /// @param[out] cmdType         type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
  /// @return Status of the function call, STS_OK or an error code on failure
  extern "C" AxisProfileResult AXISPROFILE_fixedStopMockCmd(void* handle, double clampingTorque, CmdType& cmdType);

  /// @brief Command of triggering setting new reference without movement.
  /// @param[in]  handle        pointer to the instantiated object
  /// @param[in]  newPos        new position to be set when referencing has done
  /// @param[in]  encoder       index of encoder to be set
  /// @param[in]  refMode       reference mode, could be set new reference to device or control or do nothing after homing.
  /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
  /// @return Status of the function call, STS_OK or an error code on failure
  extern "C" AxisProfileResult AXISPROFILE_setNewReferenceWithoutMovementMockCmd(void* handle, double newPos, AxisProfileEncoder encoder, RefMode refMode, CmdType& cmdType);
}
