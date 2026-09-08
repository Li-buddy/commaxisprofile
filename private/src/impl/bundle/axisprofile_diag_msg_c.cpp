/// @file
///
/// @brief This source file is part of the axisprofile library and implements the
///        C/C++ method mapping for the base class for diagnostic message handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_diag_msg_c.h"
#include "axisprofile_if_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Function implementation (Wrapper for C-Interface)
// ---------------------------------------------------------------------------

extern "C" comm::axisprofile::AxisProfileDeviceErrorType AXISPROFILE_getErrorType(void* handle)
{
  IAxisProfileDiagMsg* diagMsgInterface = static_cast<IAxisProfileDiagMsg*>(handle);
  RETURN_HANDLE_INVALIDE(diagMsgInterface, AxisProfileDeviceErrorType::ERROR_NO_INFO, "get error type");
  return diagMsgInterface->getErrorType();
}

extern "C" std::uint32_t AXISPROFILE_getErrorCode(void* handle)
{
  IAxisProfileDiagMsg* diagMsgInterface = static_cast<IAxisProfileDiagMsg*>(handle);
  RETURN_HANDLE_INVALIDE(diagMsgInterface, UINT32_MAX, "get error code");
  return diagMsgInterface->getErrorCode();
}
