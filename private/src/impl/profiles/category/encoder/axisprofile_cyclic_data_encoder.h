#pragma once
/// @file
///
/// @brief This source file is part of the axisprofile library and declare the AxisProfileCyclicDataEncoder.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_cyclic_data.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

namespace comm::axisprofile
{
  class AxisProfileCyclicDataEncoder : public AxisProfileCyclicData
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Pointer to the Profile Basic Information 
    /// @param[in]   at                 Pointer to the Acknowledge Telegram (AT)   
    AxisProfileCyclicDataEncoder(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    ~AxisProfileCyclicDataEncoder() noexcept override = default;

    AxisProfileResult getActualValuesDevice(ActualValuesDevice* const actualValues) final;
    AxisProfileResult setCommandValuesDevice(CommandValuesDevice /*commandValues*/) final;

    AxisProfileResult getSingleUserPDODataFromDevice(UserPDOID id, UserPDODataInfo& actualPDOValues) override;
    AxisProfileResult setSingleUserPDODataToDevice(UserPDOID id, UserPDODataInfo commandValues) override;
    bool checkSingleUserPDOActiveFromDevice(UserPDOID id) override;
    bool checkSingleUserPDOActiveToDevice(UserPDOID id) override;

    AxisProfileResult getDigitalInputValueBit(DigitalInputValueBitType valueType, bool& inputValue) override;

    AxisProfileResult getSafetyOperationMode(SafetyOperationMode& mode) override;

    void loadBootingTasks() final;

    AxisProfileResult setNewRefPosition(std::int32_t newRefPos) override;

    AxisProfileResult activeNewRefOperation(bool active) override;
    AxisProfileResult activeSignalFunc(std::uint8_t bitOffset) final;
    AxisProfileResult resetSignalFuncBit(std::uint8_t bitOffset) final;
    AxisProfileResult getSignalFuncState(std::uint8_t bitOffset, bool& value) final;

  private:

    /// @brief Method to send diagnostic for rejected commands
    /// @param[in]  cmdNameInfo   The additive information of the diagnostic
    void rejectCmdDiagnosis(const char* cmdNameInfo) const;

    /// @brief Initialize the required shared data during booting.
    /// @return STS_OK the initialization is success.
    AxisProfileResult initAllSharedData();

  private:

    const AcknowledgeTelegram* m_telegramAT {};       ///< Pointer to the Acknowledge Telegram (AT)
    MasterDataTelegram* m_telegramMDT {};             ///< Pointer to the Master Data Telegram (MDT)
  };
}
