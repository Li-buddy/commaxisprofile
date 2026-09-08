#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the internal interface of command.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "bundle/i_axisprofile_cmd.h"
#include "job_object/i_job_obj.h"
#include "profiles/i_axisprofile_sharing.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct IAxisProfileCmdC;

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

  /// @brief This is internal interface for axisprofile command
  class IAxisProfileCmdInternal : public IAxisProfileCmd, public IAxisProfileSharing
  {
  public: // methods

    /// @brief Destructor
    virtual ~IAxisProfileCmdInternal() noexcept override = default;

    /// @brief This method gets the command C-Interface.
    /// @return             Pointer to the command C-Interface.
    virtual const IAxisProfileCmdC* getCInterface() const = 0;
  };
}
