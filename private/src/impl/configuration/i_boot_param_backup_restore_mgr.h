#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface for boot parameter backup/restore.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include <string>

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
  /// @brief Interface for loading and saving profile boot parameter snapshots.
  class IBootParamBackupRestoreMgr
  {
  public:
    virtual ~IBootParamBackupRestoreMgr() noexcept = default;

    /// @brief Load boot parameters for one profile from persistent storage.
    /// @param[in] profileName Profile name.
    /// @param[in] filePath Source file path.
    /// @return Operation result.
    virtual AxisProfileResult loadBootParams(const std::string& profileName, const std::string& filePath) const = 0;

    /// @brief Save boot parameters for one profile to persistent storage.
    /// @param[in] profileName Profile name.
    /// @param[in] filePath Target file path.
    /// @return Operation result.
    virtual AxisProfileResult saveBootParam(const std::string& profileName, const std::string& filePath) const = 0;
  };
}
