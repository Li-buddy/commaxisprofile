#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements
///        C/C++ method mapping for the base class of all axis profiles.
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

// ---------------------------------------------------------------------------
// Forward references
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

/// @brief This method triggers a error clear on the device.
/// @param[in]     handle             Handle of the interface
/// @return        Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_clearDeviceError(void *handle);

/// @brief This method enable positive stop on the device.
/// @param[in]     handle             Handle of the interface
/// @param[in]     mode               positive stop mode
/// @return        Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setAndCheckPositiveStop(void *handle, const comm::axisprofile::AxisProfilePositiveStop mode);

/// @brief This method set clamping torque on the device.
/// @param[in]     handle             Handle of the interface
/// @param[in]     clampingTorque     clamping Torque value
/// @param[in]     metrication        metric system
/// @return        Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setAndCheckClampingTorque(void *handle, const double clampingTorque, const bool metrication);

/// @brief Deprecated. This method get clamping torque on the device.
/// @param[in]     handle             Handle of the interface
/// @param[in]     clampingTorque     clamping Torque value
/// @param[in]     metrication        metric system
/// @return        Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getClampingTorqueDeprecated(void *handle, double* const clampingTorque, const bool metrication);

/// @brief This method check if axis is in stand still status.
/// @param[in]      handle            Handle of the interface
/// @param[out]     standStill        Standstill result
/// @return         Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_checkStandStill(void *handle, bool* const standStill);

/// @brief This method This method set standstill window
/// @param[in]      handle           Handle of the interface
/// @param[in]      standStillWindow StandStillWindow value to set
/// @return         Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult  AXISPROFILE_setAndCheckstandStillWindow(void *handle, const double standStillWindow);

/// @brief This method This method set standstill window
/// @param[in]      handle           Handle of the interface
/// @param[out]     standStillWindow Address to save standstill window value
/// @return         Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult  AXISPROFILE_getStandStillWindow(void *handle, double* const standStillWindow);

/// @brief This method activates cyclic standstill check
/// @param[in]       handle           Handle of the interface
/// @return          Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_activateCyclicStandStillCheck(void *handle);

/// @brief This method deactivates cyclic standstill check
/// @param[in]       handle           Handle of the interface
/// @return          Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_deactivateCyclicStandStillCheck(void *handle);

/// @brief This method triggers set absolute position
/// @param[in]       handle           Handle of the interface
/// @param[in]       position         the position value to set (deg, mm or inch)
/// @param[in]       encoder          the encoder value to set  (encoder 1 or encoder 2)
/// @param[in]       ifDriveEnabled   if the user want trigger the command while drive in enabled state (for sercos drive, i.e. AF state)
/// @return          Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setAbsolutePosition(void *handle, const double position, const comm::axisprofile::AxisProfileEncoder encoder, const bool ifDriveEnabled);

/// @brief This method resets absolute position command
/// @param[in]       handle           Handle of the interface
/// @return          Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_resetAbsolutePosition(void* handle);

/// @brief This method references the drive
/// @param[in]       handle           Handle of the interface
/// @param[in]       newRefPos        New referencing position after homing
/// @param[in]       encoder          the encoder value to set  (encoder 1 or encoder 2)
/// @return          Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_referencingAxis(void *handle, const double newRefPos, const comm::axisprofile::AxisProfileEncoder encoder);

/// @brief This method resets referencingAxis
/// @param[in]       handle           Handle of the interface
/// @return          Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_resetReferencingAxis(void *handle);

/// @brief This method get whether the axis is homed
/// @param[in]       handle           Handle of the interface.
/// @param[out]      isHomed          Indicate if axis homed  
/// @return          Returns whether the axis is homed.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_checkAxisHomedDeprecated(void* handle, bool& isHomed);

/// @brief This method reset the axis profile command
/// @param[in]      handle            Handle of the interface
/// @param[in]      type              Type of the command
/// @return         Return value indicating success or error. 
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_resetAxisProfileCommandState(void* handle, const comm::axisprofile::AxisProfileCommandType type);

/// @brief This method abort the axis profile command
/// @param[in]      handle            Handle of the interface
/// @param[in]      type              Type of the command
/// @return         Return value indicating success or error. 
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_abortAxisProfileCommand(void* handle, const comm::axisprofile::AxisProfileCommandType type);

/// @brief This method get state of the command
/// @param[in]      handle            Handle of the interface
/// @param[in]      type              Type of the command
/// @return         Return state of the command
extern "C" comm::axisprofile::AxisprofileCommandState AXISPROFILE_getAxisProfileCommandState(void* handle, const comm::axisprofile::AxisProfileCommandType type);

/// @brief This method set setReferencing position into the drive
/// @param[in]       handle           Handle of the interface
/// @param[in]       refPos           New referencing position homing
/// @param[in]       encoder          the encoder value to set  (encoder 1 or encoder 2)
/// @param[in]       mode             Use which method to set new position
/// @return          Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setReferencingPosition(void* handle, const double refPos, const comm::axisprofile::AxisProfileEncoder encoder, comm::axisprofile::RefMode mode);

/// @brief This method get determined commands state
/// @param[in]      handle            Handle of the interface
/// @param[in]      cmdState          Command state
/// @param[out]     vecCmdType        Commands in determined state
/// @return         Return type of the command
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getCommandsAtDeterminedState(void* handle, const comm::axisprofile::AxisprofileCommandState cmdState, std::vector<comm::axisprofile::AxisProfileCommandType>& vecCmdType);
