#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements factory to create fieldbus adapters.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "i_fieldbus_adapter.h"
#include "profiles/profile_defs.h"

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

  /// @brief Fieldbus adapter factory
  class FieldbusAdapterFactory
  {
  public: // methods

    /// @brief destructor
    virtual ~FieldbusAdapterFactory() noexcept = default;

    /// @brief This is the central method to create new profiles.
    /// @param[in]    profileBasicInfo Pointer to the Profile Basic Information
    /// @param[out]   newFieldbusAdapter   Pointer to the new fieldbus adapter
    static AxisProfileResult createFieldbusAdapter(ProfileBasicInfo& profileBasicInfo, IFieldbusAdapter** newFieldbusAdapter);
  };
}
