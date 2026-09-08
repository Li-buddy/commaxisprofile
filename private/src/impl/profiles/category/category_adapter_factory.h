#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define CategoryAdapterFactory class
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "encoder/encoder_category_adapter_coe.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "common/debug_trace.h"
#include "drive/drive_category_adapter_coe.h"
#include "drive/drive_category_adapter_soe.h"
#include "profiles/profile_trace.h"

using namespace comm::axisprofile;

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
  /// @brief This is CategoryAdapterFactory responsible for category creation
  class CategoryAdapterFactory
  {
  public:
    virtual ~CategoryAdapterFactory() noexcept = default;    

  public:

    /// @brief This method create detail category object.
    /// @param[in]  profileBasicInfo   Struct containing all basic information for the profile.
    /// @param[in]  at                 The acknowledge telegram struct.
    /// @param[in]  mdt                The master data telegram struct.
    /// @param[in]  category           The profile category.
    /// @return     The category object pointer 
    static ICategoryAdapter* createCategoryAdapter(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at,
                                                   MasterDataTelegram* mdt, AxisProfileCategory category)
    {
      ICategoryAdapter* pCategory {};
      switch (category)
      {
        case AxisProfileCategory::CATEGORY_DRIVE:        
          switch (profileBasicInfo.m_axisProfileType)
          {
            case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
              pCategory = new DriveCategoryAdapterSoE(profileBasicInfo, at, mdt);
              break;
            case AxisProfileType::CANOPEN_OVER_ETHERCAT:
              pCategory = new DriveCategoryAdapterCoE(profileBasicInfo, at, mdt);
              break;
            case AxisProfileType::UNDEFINED:
              break;
          }
          break;
        case AxisProfileCategory::CATEGORY_ENCODER:
          switch (profileBasicInfo.m_axisProfileType)
          {
            case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
            case AxisProfileType::CANOPEN_OVER_ETHERCAT:
              pCategory = new EncoderCategoryAdapterCoE(profileBasicInfo, at, mdt);
              break;
            case AxisProfileType::UNDEFINED:
              break;
          }
          break;
        default:
          APR_TRACE_ERROR("%s; current profile category is %s, it's invalid", profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileCategoryTypeToString(category));
          break;
      }

      return pCategory;
    }

  private:
    CategoryAdapterFactory (const CategoryAdapterFactory& category);
    CategoryAdapterFactory& operator= (const CategoryAdapterFactory& category) = delete;
  };
}
