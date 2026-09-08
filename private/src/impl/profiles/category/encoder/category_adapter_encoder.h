#pragma once

/// @file
///
/// @brief This source file is part of the axisprofile library and declare the CategoryAdapterEncoder class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/category/category_adapter.h"

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

  /// @brief This class is responsible for the encoder profile-related behavior.
  class CategoryAdapterEncoder : public CategoryAdapter
  {
  public:
    /// @brief Constructor
    /// @param[in]  profileBasicInfo   Struct containing all basic information for the profile.    
    /// @param[in]  at                 Pointer to the Acknowledge Telegram (AT)
    /// @param[in]  mdt                Pointer to the Master Data Telegram (MDT)     
    CategoryAdapterEncoder(const ProfileBasicInfo& profileBasicInfo,
                           const AcknowledgeTelegram* at,
                           MasterDataTelegram* mdt);

    /// @brief Destructor
    ~CategoryAdapterEncoder() noexcept = default;  

    AxisProfileResult updateScalingStrategy(const AxisProfileScalingStrategy& strategy) final;
    
  };
}
