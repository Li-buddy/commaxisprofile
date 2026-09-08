#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and declare class AxisProfileDeviceParamDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_device_param.h"
#include "profiles/signal_func/signal_func_handler.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestProfileDeviceParam;

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

  /// @brief This struct contain if the delay type activated and the value of delay time
  struct BrakeDelayInfo
  {
    bool m_isActivated {false};                 ///< Check if the delay type activated
    double m_delayTime {0};                     ///< The delay time of the multi brake
  };

  /// @brief This struct contain the brake deceleration and jerk
  struct BrakeReactionInfo
  {
    double m_deceleration {0};                  ///< Deceleration
    double m_jerk {0};                          ///< Jerk
  };

  /// @brief This is the base class for booting config handling.
  class AxisProfileDeviceParamDrive : public AxisProfileDeviceParam
  {
  public: //methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo     Pointer to the Profile Basic Information
    /// @param[in]   at                   Pointer to the Acknowledge Telegram (AT)
    /// @param[in]   mdt                  Pointer to the Master Data Telegram (MDT)
    explicit AxisProfileDeviceParamDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    virtual ~AxisProfileDeviceParamDrive() noexcept override;

  protected: //methods

    bool checkNeedReadBootParams() override final;

    /// @brief This method indicate whether the checksum idn list parameters in drive has been modified,
    ///        namely whether the idn list in drive is different to the default booting idn list.
    ///        When the value not same, update the default booting idn list to drive
    /// @return  Return value indicating whether the checksum idn list parameters in drive is different to the default booting idn list.
    bool virtual checkBootIdnListModified() = 0;

    /// @brief This method check if homed state can be access via cyclic way.
    /// @return if can be accessed, return the homed state via reference,
    ///         otherwise, just return false
    bool isHomedInCyclicData();

    /// @brief This method prepares the signal function handler for the drive.
    /// @return return the result of preparing the signal function handler for the drive.
    AxisProfileResult prepareSignalFuncHandler();

    SignalFuncHandler* getSignalFuncHandler() final;

  private: //methods

    /// @brief This method reads the checksum of the parameters.
    /// @param[out]  the checksum value return
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult readChecksumValueFromDevice(std::uint32_t& checkSum) = 0;

    bool checkBrakeDelaySupport() const final;
    AxisProfileResult getErrorBrakeDelayInfo(const AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTime) final;
    AxisProfileResult getErrorBrakeDynLimit(const AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk) final;
    AxisProfileResult getPositionDataFormat(AxisProfilePosDataFormat& posDataFormat) final;
    DeviceCfgData& getDeviceCfgData()final;
    bool isLatchParamSupport() const final;
    AxisProfileResult checkAndSetModulo(double modValue, std::string& dynErrTxt) final;

    /// @brief This method indicate whether drive support checksum parameter
    /// @return      Return value indicating whether support or not.
    bool isDriveSupportChecksum();

  protected: // data

    friend class ::TestProfileDeviceParam;     // Unit test of the booting config
    const comm::axisprofile::AcknowledgeTelegram* m_telegramAT {nullptr};            ///< Pointer to the Acknowledge Telegram (AT)
    comm::axisprofile::MasterDataTelegram*        m_telegramMDT {nullptr};           ///< Pointer to the Master Data Telegram (MDT)
    SignalFuncHandler*                            m_signalFuncHandler{nullptr};      ///< Signal function handler

    RequestStatemachine requestState {};       ///< Request state machine for async command
    DeviceCfgData m_deviceCfgData;             ///< Data of device configuration

    bool m_brakeDelySupport {false};                                             ///< Check if the device support this function
    BrakeDelayInfo m_brakeDelayInfo {};                                          ///< The structure contain the parameter of delay activated and delay time
    std::map<AxisProfileDeviceErrorType, BrakeDelayInfo> m_delayInfoMap;         ///< mapping the error type and delay information

    bool m_decelerationLimitSupport {false};                                     ///< Check if the deceleration limited
    BrakeReactionInfo m_brakeReaction {};                                        ///< Best possible deceleration of the error reaction.
    std::map<AxisProfileDeviceErrorType, BrakeReactionInfo> m_reactionInfoMap;   ///< mapping the error type and deceleration information

    bool m_prbLatTimesSupport {false};                                           ///< Check if the device support this
  };
}
