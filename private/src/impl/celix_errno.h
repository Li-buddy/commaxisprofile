#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and redefine the celix_errno.h for Windows platform
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------
/*!
* Status type returned by all functions in Celix
*/
typedef int celix_status_t;

/*!
* The facility of celix default error code
*
*/
#define CELIX_FACILITY_FRAMEWORK 1

/*!
* Make the error code according to the specification
* \param fac Facility
* \param code Code
*/
#define CELIX_ERROR_MAKE(fac,code) (((fac)<<16) | ((code)&0xFFFF))

/*!
* Error code indicating successful execution of the function.
*/
#define CELIX_SUCCESS 0

/*!
* Starting point for Celix errors.
*/
#define CELIX_START_ERROR 70000

/*!
* The range for Celix errors.
* \deprecated It is recommended to use facility indicate the range of error codes
*/
#define CELIX_ERRSPACE_SIZE 1000

/*!
* The start error number user application can use.
* \deprecated It is recommended to use CELIX_CUSTOMER_ERR_MASK to define user error number
*/
#define CELIX_START_USERERR (CELIX_START_ERROR + CELIX_ERRSPACE_SIZE)

/*!
* Exception indicating a problem with a bundle
*/
#define CELIX_BUNDLE_EXCEPTION          CELIX_ERROR_MAKE(CELIX_FACILITY_FRAMEWORK, 4465)

/*!
* Argument is not correct
*/
#define CELIX_ILLEGAL_ARGUMENT          CELIX_ERROR_MAKE(CELIX_FACILITY_FRAMEWORK, 4467)
