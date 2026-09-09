#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines
///        C/C++ method mapping for the base class for status- and controlword handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_axisprofile_status_control.h"

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------- Statusword ----------------------------------------------- //

/// @brief This method gets the status word of the device.
/// @param[in]   handle   Handle of the interface
/// @return      Return value indicating success or error.
extern "C" std::uint16_t AXISPROFILE_getStatusWord(void *handle);

/// @brief This method checks if the given operation mode equals the operation mode of the device.
/// @param[in]   handle   Handle of the interface
/// @param[in]   opMode   Operation mode which will be checked
/// @return      Return value indicating if given operation mode is set in the device (true) or not (false).
extern "C" bool AXISPROFILE_checkOperationMode(void *handle, const comm::axisprofile::AxisProfileOperationMode opMode);

/// @brief This method checks if the given operation mode is supported in the operation mode of the device.
/// @param[in]   handle   Handle of the interface
/// @param[in]   opMode   Operation mode which will be checked
/// @param[out]  errorInfo   Error information of operation mode
/// @return      Return value indicating if operation mode is supported in the device (true) or not (false).
extern "C" bool AXISPROFILE_checkOperationModeSupported(void* handle, const comm::axisprofile::AxisProfileOperationMode opMode, comm::axisprofile::OpModeErrorInfo& errorInfo);

/// @brief This method gets the actual operation mode of the device.
/// @param[in]   handle   Handle of the interface
/// @return      Return value is the actual operation mode.   
extern "C" comm::axisprofile::AxisProfileOperationMode AXISPROFILE_getOperationMode(void *handle);

/// @brief This method reads the actual operation mode of the device.
/// @param[in]   handle          Handle of the interface
/// @param[out]  operationMode   Read operation mode
/// @return      Return value indicating success, pending state or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_readOperationMode(void *handle, comm::axisprofile::AxisProfileOperationMode& operationMode);

/// @brief This method checks if the given power mode equals the power mode of the device.
/// @param[in]   handle      Handle of the interface
/// @param[in]   powerMode   Power mode which will be checked
/// @return      Return value indicating if given power mode is set in the device (true) or not (false).
extern "C" bool AXISPROFILE_checkPowerMode(void *handle, const comm::axisprofile::AxisProfilePowerMode powerMode);

/// @brief This method gets the actual power mode of the device.
/// @param[in]   handle   Handle of the interface
/// @return      Return value is the actual power mode. 
extern "C" comm::axisprofile::AxisProfilePowerMode AXISPROFILE_getPowerMode(void *handle);

/// @brief This methods checks if the device is in error state.
/// @param[in]   handle   Handle of the interface
/// @return      Return value indicating whether the device is in error (true) or not (false).
extern "C" bool AXISPROFILE_checkError(void *handle);

/// @brief This methods checks if the device is in standstill state.
/// @param[in]   handle   Handle of the interface
/// This method is correctly functional, ONLY IF activateCyclicStandStillCheck() declared in i_axisprofile_command.h is called earlier. 
/// @return      Return value indicating whether the device is in standstill.
extern "C" bool AXISPROFILE_cyclicStandStillCheck(void *handle);

/// @brief This method is device follows the control to set point.
/// @param[in]   handle   Handle of the interface
/// @return      Return whether the device follows the control to set point.
extern "C" bool AXISPROFILE_isDeviceFollowsControl(void* handle);

/// @brief This method gets if the device has warning currently.
/// @param[in]   handle     Handle of the interface
/// @return      Return value indicating success or error.
extern "C" bool AXISPROFILE_deviceHasWarning(void *handle);

/// @brief This method gets if the device has warning currently.
/// @param[in]   handle     Handle of the interface
/// @return      Return value indicating the device operation status.
extern "C" comm::axisprofile::ReadyToOperate AXISPROFILE_checkReadyToOperate(void *handle);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------- Controlword ----------------------------------------------- //

/// @brief This method sets the control word of the device.
/// @param[in]   handle   Handle of the interface
/// @param[in]   controlWord   controlWord
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setControlWord(void *handle, std::uint16_t controlWord);

/// @brief This method sets the given operation mode to the device.
/// @param[in]   handle   Handle of the interface
/// @param[in]   opMode   Operation mode which will be set
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setOperationMode(void *handle, const comm::axisprofile::AxisProfileOperationMode opMode);

/// @brief  This method sets the given power mode to the device.
/// @param[in]   handle      Handle of the interface
/// @param[in]   powerMode   Power mode which will be set
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setPowerMode(void *handle, const comm::axisprofile::AxisProfilePowerMode powerMode);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------- Drive Halt ----------------------------------------------- //

/// @brief This method sets the given drive halt mode to device true for enable false for disable.
/// @param[in]   handle      Handle of the interface
/// @param[in]   haltEnable  Enable or disable drive halt
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setDriveHaltMode(void *handle, const bool haltEnable);

/// @brief This method checks if the drive halt in device is enabled. 
/// @param[in]   handle      Handle of the interface
/// @param[out]  isHaltEnabled Is drive halt enabled or not
/// @return      Return value indicating if the drive halt in device is enabled.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getDriveHaltMode(void *handle, bool& isHaltEnabled);
