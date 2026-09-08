#pragma once

/// @file
///
/// This header file is part of the axisprofile library and define the
/// internal command interface for all axisprofiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "bundle/i_axisprofile_command.h"
#include "profiles/i_axisprofile_sharing.h"
#include "profiles/i_axisprofile_state_switch.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct IAxisProfileCommandC;

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

  /// @brief This is the internal command interface for all axisprofiles.
  class IAxisProfileCommandInternal : public IAxisProfileCommand, public IAxisProfileSharing, public IAxisProfileStateSwitch
  {
  public: // methods

    /// @brief destructor
    virtual ~IAxisProfileCommandInternal() noexcept override = default;

    /// @brief This method gets the command C-Interface.
    /// @return             Pointer to the command C-Interface.
    virtual const IAxisProfileCommandC* getCInterface() const = 0;

    /// @brief This method reset all axisprofile commands
    /// @return             Return value indicating success or error.
    virtual AxisProfileResult resetAllAxisprofileCommands() = 0;    

    /// @brief This method gets Asynchronous communication of command pending registration result.
    /// @return             Return value indicating success or error.
    virtual AxisProfileResult addAuxCyclicJobs() = 0;

    /// @brief Internal asynchronous command to check if the device is in halt state.
    /// @param[out]         Is device halted
    /// @return             Return value indicating success or error.
    virtual AxisProfileResult checkIfDeviceHalt(bool& isHalted) = 0;
  };
}
