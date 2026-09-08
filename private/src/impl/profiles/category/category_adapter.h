#pragma once

/// @file
///
/// @brief This source file is part of the axisprofile library and declare the CategoryAdapter.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "i_category_adapter.h"
#include "profiles/data_sharing/i_sharing_handler.h"

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
    /// @brief This class responsible for profile category common behavior.
    class CategoryAdapter : public ICategoryAdapter
    {
    public://method
      /// @brief Constructor
      /// @param[in]  profileBasicInfo   Struct containing all basic information for the profile.      
      /// @param[in]  at                 Pointer to the Acknowledge Telegram (AT)
      /// @param[in]  mdt                Pointer to the Master Data Telegram (MDT)
      /// @param[in]  category           The profile category.     
      CategoryAdapter(const ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at,
                      MasterDataTelegram* mdt, AxisProfileCategory category);

      ~CategoryAdapter() noexcept override;

      AxisProfileCategory getProfileCategory() const final;

      void setSharingHandler(IAxisProfileSharingHandler* handler) final;

      IAxisProfileCommandInternal* getCommandInternal() final;

      IAxisProfileStatusControlInternal* getStatusControlInternal()final;

      IAxisProfileCyclicDataInternal* getCyclicDataInternal() final;

      IAxisProfileDeviceParamInternal* getDeviceParamInternal() final;

      IAxisProfileProbeInternal* getProbeInternal() final;

      IScaling* getScalingInternal() final;

      void registerSharingITFs(IAxisProfileSharingHandler* handler) final;

      void removeSharingITFs() final;

      AxisProfileResult setStatemachine(const StateMachineState* const statemachine) final;

      const StateMachineState*& getStatemachine() final;

      void loadInitMethod(std::vector<std::function<AxisProfileResult()>>& methods) final;

    protected: // data

      ProfileBasicInfo m_profileBasicInfo;                                              ///< Profile basic information
      const AcknowledgeTelegram* m_telegramAT {};                                       ///< Pointer to the Acknowledge Telegram (AT)
      MasterDataTelegram* m_telegramMDT {};                                             ///< Pointer to the Master Data Telegram (MDT)
      AxisProfileCategory m_profileCategory {AxisProfileCategory::UNDEFINED};           ///< profile category
      
      IAxisProfileSharingHandler* m_sharingHandler {};                                  ///< Pointer to the sharing handler
      IScaling* m_scaling {};                                           ///< Scaling class used for this profile
      IAxisProfileCommandInternal* m_commandInterface {};               ///< Pointer to the internal command interface
      IAxisProfileStatusControlInternal* m_statusControlInterface {};   ///< Pointer to the internal status- and controlword interface
      IAxisProfileCyclicDataInternal* m_cyclicDataInterface {};         ///< Pointer to the internal cyclic data interface
      IAxisProfileDeviceParamInternal* m_deviceParamInterface {};       ///< Pointer to the internal device parameter interface
      IAxisProfileProbeInternal* m_probeInterface {};                   ///< Pointer to the internal probe interface
    };
}
