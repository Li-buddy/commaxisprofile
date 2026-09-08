#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the helpers of bundle interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/debug_trace.h"

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

  /// @brief Handle validation
  /// @param[in]  bundleIf    bundle interface
  /// @param[in]  retValue    return value if validation failed
  /// @param[in]  errStr      error text if validation failed
  #define RETURN_HANDLE_INVALIDE(bundleIf, retValue, errStr) \
    { \
      if (!bundleIf) \
      { \
        APR_TRACE_ERROR("Could not %s. Interface-Handle is missing or cast failed!", errStr); \
        return retValue; \
      } \
    }
}
