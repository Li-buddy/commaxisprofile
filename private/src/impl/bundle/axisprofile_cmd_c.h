#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the command handle.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
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

/// @brief Get acutal state of axisprofile command
/// 
/// @param[in]  handle        pointer to the instantiated object
/// @param[in]  cmdType       command type
/// @return state of command
extern "C" ::comm::axisprofile::CmdState AXISPROFILE_getCmdState(void* handle, ::comm::axisprofile::CmdType cmdType);

/// @brief Reset a axisprofile command
/// 
/// @param[in]  handle        pointer to the instantiated object
/// @param[in]  cmdType       command type
/// @return Status of the function call, STS_OK or an error code on failure
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_resetCmd(void* handle, ::comm::axisprofile::CmdType cmdType);

/// @brief Abort a axisprofile command
///
/// @param[in]  handle        pointer to the instantiated object
/// @param[in]  cmdType       command type
/// @return Status of the function call, STS_OK or an error code on failure
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_abortCmd(void* handle, ::comm::axisprofile::CmdType cmdType);

/// @brief Get command status in device
///
/// @param[in]  handle        pointer to the instantiated object
/// @param[in]  cmdType       command type
/// @return Status of command in device
extern "C" ::comm::axisprofile::CmdStatusInDevice AXISPROFILE_getCmdStatusInDevice(void* handle, ::comm::axisprofile::CmdType cmdType);

/// @brief Command of clearing device error
///
/// @param[in]  handle        pointer to the instantiated object
/// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
/// @return Status of the function call, STS_OK or an error code on failure
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_clearDeviceErrorCmd(void* handle, ::comm::axisprofile::CmdType& cmdType);

/// @brief Command of setting new absolute position to device
///
/// @param[in]  handle        pointer to the instantiated object
/// @param[in]  newPos        new absolute position
/// @param[in]  encoder       index of encoder to be set
/// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
/// @return Status of the function call, STS_OK or an error code on failure
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_setAbsolutePositionCmd(void* handle, double newPos, ::comm::axisprofile::AxisProfileEncoder encoder, ::comm::axisprofile::CmdType& cmdType);

/// @brief Command of triggering searching new reference of device
///
/// @param[in]  handle        pointer to the instantiated object
/// @param[in]  newPos        new position to be set when referencing has done
/// @param[in]  encoder       index of encoder to be set
/// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_referencingAxisCmd(void* handle, double newPos, ::comm::axisprofile::AxisProfileEncoder encoder, ::comm::axisprofile::CmdType& cmdType);

/// @brief Command of triggering fixed stop
///
/// @param[in]  handle          pointer to the instantiated object
/// @param[in]  clampingTorque  Clamping torque of fixedstop
/// @param[out] cmdType         type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_fixedStopCmd(void* handle, double clampingTorque, ::comm::axisprofile::CmdType& cmdType);

/// @brief Command of clearing device error
///
/// @param[in]  handle        pointer to the instantiated object
/// @return Status of the function call, STS_OK or an error code on failure
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_clearDeviceError2Cmd(void* handle);

/// @brief Command of setting new reference of device without movement.
///
/// @param[in]  handle        pointer to the instantiated object
/// @param[in]  newPos        new position to be set when referencing has done
/// @param[in]  encoder       index of encoder to be set
/// @param[in]  refMode       reference mode, could be set new reference to device or control or do nothing after homing.
/// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
/// @return Status of the function call, STS_OK or an error code on failure
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_setNewReferenceWithoutMovement(void* handle, double newPos, ::comm::axisprofile::AxisProfileEncoder encoder, ::comm::axisprofile::RefMode refMode, ::comm::axisprofile::CmdType& cmdType);

/// @brief Command of switching operation mode with check
/// @param[in] handle pointer to the instantiated object
/// @param[in] opMode operation mode to be set
/// @return Status of the function call, STS_OK or an error code on failure
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_switchOperationModeWithCheckCmd(void* handle, ::comm::axisprofile::AxisProfileOperationMode opMode);
