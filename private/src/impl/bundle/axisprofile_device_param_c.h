#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements the
/// C/C++ method mapping for the base class for device parameter access.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_axisprofile_device_param.h"

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

/// @brief This method get whether the device support brake delay function.
/// @param[in]   handle        Handle of the interface.
/// @return      Returns whether the device support this function.
extern "C" bool AXISPROFILE_checkBrakeDelaySupport(void* handle);

/// @brief This method get the delay information .
/// @param[in]   handle        Handle of the interface.
/// @param[in]   errorType     The error type.
/// @param[out]  isActivated   whether the delay activated.
/// @param[out]  delayTimeMs   The delay time.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getErrorBrakeDelayInfo(void* handle, const comm::axisprofile::AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTimeMs);

/// @brief This method get the deceleration information of the multi brake.
/// @param[in]   handle        Handle of the interface.
/// @param[in]   errorType     The error type.
/// @param[out]  deceleration  Deceleration of the multi brake.
/// @param[out]  jerk          Jerk of the multi brake.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getErrorBrakeDynLimit(void* handle, const comm::axisprofile::AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk);

/// @brief This method get the position data format in drive.
/// @param[in]  handle          Handle of the interface.
/// @param[out] posDataFormat   The position data format(absolute or module)
/// @return     Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getPositionDataFormat(void* handle, comm::axisprofile::AxisProfilePosDataFormat& posDataFormat);

/// @brief This function check and set modulo value of the device.
/// @param[in]  handle          Handle of the interface.
/// @param[in]  modValue        Modulo value that to be checked (when driver has build modulo conversion) or set (when axisprofile handles modulo conversion).
/// @param[out] dynErrTxt       Dynamic error text when error returns, used for diagnosis of the caller.
/// @return     Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_checkAndSetModulo(void* handle, double modValue, std::string & dynErrTxt);

/// @brief This function checks if the functionality is supported or not.
/// @param[in]  handle     Handle of the interface.
/// @param[in]  function   The function to be checked.
/// @param[out] errorInfo  Dynamic error information when function is not supported, used for diagnosis of the caller.
/// @return                TRUE indicates that the function is supported, FALSE indicates not support.
extern "C" bool AXISPROFILE_checkFunctionSupported(void* handle, const ::comm::axisprofile::FunctionSupport function, ::comm::axisprofile::FunctionSupportErrorInfo& errorInfo);

/// @brief This function checks if the axis is standstill
/// @param[in]  handle                Handle of the interface.
/// @param[out] standStill            TRUE indicates that the axis is standstill, FALSE indicates not standstill.
/// @return                 Status of the function call: STS_OK or an error code on
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_checkStandstill(void* handle, bool& standStill);

/// @brief This function gets rate torque/force of the load in IS unit
/// @param[in]  handle                Handle of the interface
/// @param[out] rateTorqueForce       Rate torque for rotary axis or rate force for linear axis in IS unit
/// @param[out] dynErrTxt             Dynamic error text when error returns, used for diagnosis of the caller
/// @return                 Status of the function call: STS_OK or an error code on
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_getRateTorqueForce(void* handle, double& rateTorqueForce, std::string& dynErrTxt);

/// @brief This function checks axis has homed or not.
/// @param[in]  handle                Handle of the interface.
/// @param[out] homed                 True indicates that the axis has homed, false indicates that the axis has not homed.
/// @return                           Status of the function call: STS_OK or an error code
extern "C" ::comm::axisprofile::AxisProfileResult AXISPROFILE_checkAxisHomed(void* handle, bool& homed);
