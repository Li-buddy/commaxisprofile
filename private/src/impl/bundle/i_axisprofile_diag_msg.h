#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the main
///        interface for getting diagnostic message (e.g. error, warning, ...).
///        This is a sub-interface of 'IAxisProfile'.
///
/// SPDX-FileCopyrightText: Copyright (c) 20121 - 2025 Bosch Rexroth AG
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
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for getting diagnostic message.
  class IAxisProfileDiagMsg
  {
  public: // methods

    virtual ~IAxisProfileDiagMsg() noexcept = default;

    /// @brief This method gets error type of driver.
    ///
    /// @return Return error type.
    virtual AxisProfileDeviceErrorType getErrorType() = 0;

    /// @brief This method gets error code of driver.
    /// @return Return error code of the driver.
    virtual std::uint32_t getErrorCode() = 0;
  };
}
