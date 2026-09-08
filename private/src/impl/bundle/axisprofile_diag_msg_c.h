#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements the
/// C/C++ method mapping for the base class for diagnostic message handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_if_helper.h"
#include "i_axisprofile_diag_msg.h"

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

/// @brief This method gets the error type of driver.
/// @param[in]   handle   Handle of the interface
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileDeviceErrorType AXISPROFILE_getErrorType(void* handle);

/// @brief This method gets the error code of driver.
/// @param[in]   handle   Handle of the interface
/// @return      Error code of the driver
extern "C" std::uint32_t AXISPROFILE_getErrorCode(void* handle);
