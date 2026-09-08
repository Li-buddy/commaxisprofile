#pragma once

/// @file
///
/// This header file is part of the axisprofile library and implements the
/// internal probe interface for all axisprofiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "bundle/i_axisprofile_probe.h"
#include "profiles/i_axisprofile_sharing.h"
#include "profiles/i_axisprofile_state_switch.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct IAxisProfileProbeC;

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

  /// @brief his is the internal probe interface for all axisprofiles.
  class IAxisProfileProbeInternal : public IAxisProfileProbe, public IAxisProfileSharing, public IAxisProfileStateSwitch
  {
  public: // methods

    /// @brief destructor
    virtual ~IAxisProfileProbeInternal() noexcept override = default;

    /// @brief This method gets the status/control C-Interface.
    /// @return      Return Pointer to the status/control C-Interface.
    virtual const IAxisProfileProbeC* getCInterface() const = 0;
  };
}
