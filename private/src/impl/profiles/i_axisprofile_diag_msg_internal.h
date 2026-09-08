#pragma once

/// @file
///
/// This header file is part of the axisprofile library and implements the
/// internal diagnostic message interface for all axisprofiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "bundle/i_axisprofile_diag_msg.h"
#include "axisprofile_internal_defs.h"
#include "profiles/i_axisprofile_sharing.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct IAxisProfileDiagMsgC;

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

  /// @brief This is the internal diagnostic message interface for all axisprofiles.
  class IAxisProfileDiagMsgInternal : public IAxisProfileDiagMsg, public IAxisProfileSharing
  {
  public: // methods

    /// @brief destructor
    virtual ~IAxisProfileDiagMsgInternal() noexcept override = default;

    /// @brief This method gets the diagnostic message C-Interface.
    /// @return Pointer to the diagnostic message C-Interface.
    virtual const IAxisProfileDiagMsgC* getCInterface() const = 0;

    /// @brief This method reset error code.
    virtual void resetErrCode() = 0;
  };
}
