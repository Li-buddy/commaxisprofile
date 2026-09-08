#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the
///        C/C++ method mapping for the base class for cyclic data handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_axisprofile_cyclic_data.h"
#include "axisprofile_if_helper.h"

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

/// @brief This method gets the at data container of the axisprofile.
/// @param[in]   handle   Handle of the interface
/// @param[out]  actualValues   Pointer to actual values container the axisprofile data should be stored to.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getActualValuesDevice(void *handle, comm::axisprofile::ActualValuesDevice* actualValues);

/// @brief This method copies the given command values container to the data container in the axisprofile.
/// @param[in]   handle          Handle of the interface
/// @param[in]   commandValues   Commanded values container that should be copied.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setCommandValuesDevice(void *handle, comm::axisprofile::CommandValuesDevice commandValues);

/// @brief This method gets the digital input value for a single bit of the axisprofile.
/// @param[in]   handle   Handle of the interface
/// @param[in]   valueType   The type of digital input value bit
/// @param[out]  inputValue   Actual value of the specific bit
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getDigitalInputValueBit(void* handle, comm::axisprofile::DigitalInputValueBitType valueType, bool& inputValue);

/// @brief This method check data for NRT usage. 
/// @param[in]  handle      Handle of the interface
/// @param[in]  valueType   Enum to define the target value type of user.
/// @param[out] errorInfo   Once the data unavailable, return the error information.
/// @return     Indicate whether the value return correctly.
extern "C" bool AXISPROFILE_checkActualValueAvailabilityNRT(void* handle, comm::axisprofile::AxisProfileValueTypeID valueType, comm::axisprofile::DataAccessErrorInfo& errorInfo);

/// @brief Get the current safety operation mode of drive.
/// @param[in]  handle  handle of the interface.
/// @param[out]   mode  indicates the current safety operation mode.
/// @return             return value indicates success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getSafetyOperationMode(void* handle, comm::axisprofile::SafetyOperationMode& mode);

/// @brief This method gets the single user PDO data from device.
/// @param[in]  handle          Handle of the interface
/// @param[in]  id              The user PDO id to inquired.
/// @param[out] actualPDOInfo   Reference to actual user PDO information.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getSingleUserPDODataFromDevice(void* handle, comm::axisprofile::UserPDOID id, comm::axisprofile::UserPDODataInfo& actualPDOInfo);

/// @brief This method set the single user PDO data to device.
/// @param[in]  handle        Handle of the interface
/// @param[in]  id            The user PDO id to inquired.
/// @param[out] newPDOInfo    New user PDO information.
/// @return     Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setSingleUserPDODataToDevice(void* handle, comm::axisprofile::UserPDOID id, comm::axisprofile::UserPDODataInfo newPDOInfo);

/// @brief This method check whether single user PDO (AT) is active.
/// @param[in]  handle   Handle of the interface
/// @param[in]  id       The user PDO id to check.
/// @return   Indicate whether activated.
extern "C" bool AXISPROFILE_checkSingleUserPDOActiveFromDevice(void* handle, comm::axisprofile::UserPDOID id);

/// @brief This method check whether single user PDO (MDT) is active.
/// @param[in]   handle   Handle of the interface
/// @param[in]   id       The user PDO id to check.
/// @return   Indicate whether activated.
extern "C" bool AXISPROFILE_checkSingleUserPDOActiveToDevice(void* handle, comm::axisprofile::UserPDOID id);

/// @brief This method get effective time of actual output in unit of second
/// @return     Effective time of actual output
extern "C" double AXISPROFILE_getEffectiveTimeOfActOutput(void* handle);

/// @brief This method get latch time of actual input in unit of second
/// @return     Latch time of actual input
extern "C" double AXISPROFILE_getLatchTimeOfActInput(void* handle);
