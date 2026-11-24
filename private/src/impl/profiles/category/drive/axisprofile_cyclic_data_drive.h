#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements
/// base class for cyclic data handling.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_cyclic_data.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestSercosCyclicData;
class TestCanopenCyclicData;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class ProfileTrace;
  class ICategoryAdapter;
  class IUserPDOMgr;

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief type of function pointer(the torque velocity limit status check function)
  using TorqueStatusFn = TorqueVelocityLimitStatus(*)(const WordBits&);

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the base class for cyclic data handling.
  class AxisProfileCyclicDataDrive : public AxisProfileCyclicData
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Pointer to the Profile Basic Information
    explicit AxisProfileCyclicDataDrive(const ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    ~AxisProfileCyclicDataDrive() noexcept override = default;
    
    bool checkActualValueAvailabilityNRT(AxisProfileValueTypeID valueTypeID, DataAccessErrorInfo& errorInfo) override;
    AxisProfileResult getSingleUserPDODataFromDevice(UserPDOID id, UserPDODataInfo& actualPDOValues) override;
    AxisProfileResult setSingleUserPDODataToDevice(UserPDOID id, UserPDODataInfo commandValues) override;
    bool checkSingleUserPDOActiveFromDevice(UserPDOID id) override;
    bool checkSingleUserPDOActiveToDevice(UserPDOID id) override;

    AxisProfileResult getDigitalInputValueBit(DigitalInputValueBitType valueType, bool& inputValue) override;

    /// @brief Check the current safety operation mode based on cyclic data.
    /// @param[out]           mode  indicates the current safety operation mode.
    /// @return                     result value indicating success or error.
    /// @retval STS_OK                   the checking of safety operation mode is success.
    /// @retval ERR_ERR_DATA_NOT_MAPPED  indicates safety-selection or safety-status was not mapped;
    AxisProfileResult checkSafetyOperationMode(SafetyOperationMode& mode);

    AxisProfileResult getSafetyOperationMode(SafetyOperationMode& mode) override;

  protected:  // methods

    /// @brief Initialize the torque velocity limitation status check function.
    AxisProfileResult initTorqueLimitFunction();

    /// @brief Check current cylic data is match the limitation requirement.
    /// @param[in]  valueID         The physic value type enum.
    /// @param[out] msg             Data access error information.
    /// @return Boolean result indicate success or not.
    bool checkIfMatchLimitation(AxisProfileValueTypeID valueID, DataAccessErrorInfo& msg);

  private:

    /// @brief Torque velocity no violation(for consistency)
    /// @param[in]  The bit field of torque velocity limitation status data.
    /// @return     The torque velocity limit status enum.
    static TorqueVelocityLimitStatus torqueVelLimitNotViolate(const WordBits& bits);

    /// @brief Check the torque velocity violation with version before 0606
    /// For version lower than 06V06, using bit0 and bit1 of SoE: P-0-2251/CoE:0x60CC.
    /// @param[in]  The bit field of torque velocity limitation status data.
    /// @return     The torque velocity limit status enum.
    static TorqueVelocityLimitStatus torqueVelLimitOldVersion(const WordBits& bits);

    /// @brief Check the torque velocity violation with version at least 0606
    /// For version 06V06 and higher using bit1 and bit2 of SoE:P-0-2251/CoE:0x60CC.
    /// @param[in]  The bit field of torque velocity limitation status data.
    /// @return     The torque velocity limit status enum.
    static TorqueVelocityLimitStatus torqueVelLimitNewVersion(const WordBits& bits);
    
  protected: // data

    friend class  ::TestSercosCyclicData;                ///< Unit test of the Sercos Cyclic Data
    friend class  ::TestCanopenCyclicData;               ///< Unit test of the Canopen Cyclic Data
    const AcknowledgeTelegram*    m_telegramAT {};       ///< Pointer to the Acknowledge Telegram (AT)
    MasterDataTelegram*           m_telegramMDT {};      ///< Pointer to the Master Data Telegram (MDT)
    TorqueStatusFn                m_torqueStatusFn {};   ///< function pointer to the torque velocity limit status check function
    IAxisProfilePropertyInternal* m_property {};         ///< Pointer to the property interface
  };
}
