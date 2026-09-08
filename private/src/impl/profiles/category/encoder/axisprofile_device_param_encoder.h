#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and declare class AxisProfileDeviceParamEncoder.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2025-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_device_param.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestAxisProfileDeviceParamEncoder;

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
  class AxisProfileDeviceParamEncoder : public AxisProfileDeviceParam
  {
  public: // methods

    /// @brief Constructor      
    /// @param[in]   profileBasicInfo     Pointer to the Profile Basic Information
    /// @param[in] at               Pointer to the Acknowledge Telegram (AT)
    explicit AxisProfileDeviceParamEncoder(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at);

    /// @brief Destructor
    ~AxisProfileDeviceParamEncoder() noexcept override = default;
    AxisProfileResult getChecksumIdnList(std::vector<std::string>& idnStrListRet) const final;

  protected: // methods

    OpModeAvailInfo* getOpModeAvailInfo() final;
    bool checkFunctionSupported(const FunctionSupport function, FunctionSupportErrorInfo& errorInfo) final;
    void loadBootingTasks() final;
    bool checkBrakeDelaySupport() const final;
    AxisProfileResult getErrorBrakeDelayInfo(const AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTime) final;
    AxisProfileResult getErrorBrakeDynLimit(const AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk) final;
    AxisProfileResult getPositionDataFormat(AxisProfilePosDataFormat& posDataFormat) final;
    DeviceCfgData& getDeviceCfgData()final;
    bool isLatchParamSupport() const final;
    AxisProfileResult checkAndSetModulo(double modValue, std::string& dynErrTxt) final;
    
    AxisProfileResult checkStandstill(bool& standStill) final;
    bool checkNeedReadBootParams() final;
    AxisProfileResult getDeviceInformation() final;
    AxisProfileResult checkAxisHomed(bool& homed) final;
    SignalFuncHandler* getSignalFuncHandler() final;

  private: // methods

    /// @brief Method to send diagnostic for rejected commands
    /// @param[in]  cmdNameInfo   The additive information of the diagnostic
    void rejectCmdDiagnosis(const char* cmdNameInfo) const;

  protected: // data

    friend class ::TestAxisProfileDeviceParamEncoder;     ///< Unit test of the booting config
    const AcknowledgeTelegram* m_telegramAT {nullptr};    ///< Pointer to the Acknowledge Telegram (AT)
    DeviceCfgData m_cfgData {};                           ///< the device configured data
  };
}
