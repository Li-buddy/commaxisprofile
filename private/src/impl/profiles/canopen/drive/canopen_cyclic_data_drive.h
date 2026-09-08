#pragma once

/// @file
///
/// @brief This source file is part of the axisprofile library and declare canopen cyclic behavior in drive category.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/category/drive/axisprofile_cyclic_data_drive.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

namespace comm::axisprofile::canopen
{
  class CanopenCyclicDataDrive : public AxisProfileCyclicDataDrive
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Pointer to the Profile Basic Information 
    /// @param[in]   at                 Pointer to the Acknowledge Telegram (AT)
    /// @param[in]   mdt                Pointer to the Master Data Telegram (MDT)     
    CanopenCyclicDataDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    ~CanopenCyclicDataDrive() noexcept override = default;

    AxisProfileResult getActualValuesDevice(ActualValuesDevice* const actualValues) final;
    AxisProfileResult setCommandValuesDevice(CommandValuesDevice commandValues) final;

  private:

    /// @brief Initialize the required shared data during booting.
    /// @return STS_OK the initialization is success.
    AxisProfileResult initAllSharedData();

    void loadBootingTasks() final;
  };
}
