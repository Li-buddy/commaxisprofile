#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and define the drive category adapter for SoE.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "category_adapter_drive.h"

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

  class DriveCategoryAdapterSoE : public CategoryAdapterDrive
  {
  public:
    /// @brief Constructor
    /// @param[in]  profileBasicInfo   Struct containing all basic information for the profile.
    /// @param[in]  at                 The acknowledge telegram struct.
    /// @param[in]  mdt                The master data telegram struct.
    DriveCategoryAdapterSoE(ProfileBasicInfo& profileBasicInfo,
                            const AcknowledgeTelegram* at,
                            MasterDataTelegram* mdt);

    /// @brief Destructor
    ~DriveCategoryAdapterSoE() noexcept override = default;

    AxisProfileResult updateScalingStrategy(const AxisProfileScalingStrategy& strategy) final;
  };
}
