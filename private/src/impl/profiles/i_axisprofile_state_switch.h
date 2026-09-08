#pragma once
/// @file
///
/// This header file is part of the axisprofile library and define the state switch operation. 
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

#include "comm/axisprofile/defines/axisprofile_statusdefs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
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

  /// @brief This class interface used to extend the behavior of state change.
  class IAxisProfileStateSwitch
  {
  public:
    virtual ~IAxisProfileStateSwitch() noexcept = default;

    /// @brief This method init axisprofile commands
    /// @return  Return value indicating success or error.
    virtual AxisProfileResult booting() = 0;

    /// @brief This method load booting tasks to container
    virtual void loadBootingTasks() = 0;
  };
}
