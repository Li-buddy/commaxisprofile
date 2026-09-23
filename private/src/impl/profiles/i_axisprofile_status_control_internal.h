#pragma once
/// @file
///
/// This header file is part of the axisprofile library and define the
/// internal status control interface for all axisprofiles.
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
#include "bundle/i_axisprofile_status_control.h"
#include "profiles/i_axisprofile_sharing.h"
#include "profiles/i_axisprofile_state_switch.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct IAxisProfileStatusControlC;

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

  /// @brief This is the internal status control interface for all axisprofiles.
  class IAxisProfileStatusControlInternal : public IAxisProfileStatusControl, public IAxisProfileSharing, public IAxisProfileStateSwitch
  {
  public: // methods

    /// @brief destructor
    virtual ~IAxisProfileStatusControlInternal() noexcept override = default;

    /// @brief This method gets the status/control C-Interface.
    /// @return Pointer to the status/control C-Interface.
    virtual const IAxisProfileStatusControlC* getCInterface() const = 0;

    // ------------------------------ Internal methods (not in interface!) --------------------------------

    /// @brief This method extracts all information from the actual raw status word (current cycle).
    /// @return   Return value indicating success or error.
    virtual AxisProfileResult analyzeStatusWord() = 0;

    /// @brief Commits an operation mode which has been verified against the device.
    /// @param[in] operationMode Operation mode confirmed by the command job.
    ///
    /// This synchronization hook is internal and is not exposed through the C interface.
    virtual void confirmOperationModeFromDevice(AxisProfileOperationMode operationMode) = 0;
  };
}
