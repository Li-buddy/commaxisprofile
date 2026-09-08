#pragma once
/// @file
///
/// @brief This header file is part of the axisprofile library and implements
/// base class for status and controlword handling.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_status_control.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class ProfileTrace;

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

  /// @brief This is the base class for status- and controlword handling.
  class AxisProfileStatusControlDrive : public AxisProfileStatusControl
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Reference to the Profile Basic Information
    explicit AxisProfileStatusControlDrive(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~AxisProfileStatusControlDrive() noexcept override = default;

    bool checkOperationMode(const AxisProfileOperationMode opMode) const final;
    AxisProfilePowerMode getPowerMode() const final;
    bool checkPowerMode(const AxisProfilePowerMode powerMode) const final;   

  protected: // data

    AxisProfilePowerMode m_actualPowerMode = AxisProfilePowerMode::UNDEFINED; ///< Actual power mode of the device (generalized)
    ProfileAssignedInfo m_profileAssignedInfo {false};                        ///< Check if profile variable is assigned
  };
}
