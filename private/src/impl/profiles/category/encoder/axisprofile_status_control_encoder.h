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
  class AxisProfileStatusControlEncoder : public AxisProfileStatusControl
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Reference to the Profile Basic Information
    explicit AxisProfileStatusControlEncoder(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~AxisProfileStatusControlEncoder() noexcept override = default;

    bool checkOperationMode(const AxisProfileOperationMode opMode) const final;
    AxisProfilePowerMode getPowerMode() const final;
    bool checkPowerMode(const AxisProfilePowerMode powerMode) const final;
    AxisProfileResult analyzeStatusWord() final;
    void confirmOperationModeFromDevice(AxisProfileOperationMode operationMode) final;

    // -------------------------------------- Statusword ----------------------------------------
    std::uint16_t getStatusWord() const final;
    bool checkError() const final;
    bool cyclicStandStillCheck() const final;
    bool checkOperationModeSupported(const AxisProfileOperationMode opMode, OpModeErrorInfo& errorInfo) final;
    AxisProfileOperationMode getOperationMode() const final;
    bool isDeviceFollowsControl() const final;
    bool deviceHasWarning() const final;
    ReadyToOperate checkReadyToOperate() final;

    // -------------------------------------- Controlword ---------------------------------------
    std::uint16_t getControlWord() const final;
    AxisProfileResult setControlWord(std::uint16_t controlWord) final;
    AxisProfileResult setOperationMode(const AxisProfileOperationMode opMode) final;
    AxisProfileResult setPowerMode(const AxisProfilePowerMode powerMode) final;

    // -------------------------------------- Drive Halt ---------------------------------------
    AxisProfileResult setDriveHaltMode(const bool haltEnable) final;
    AxisProfileResult getDriveHaltMode(bool& isHaltEnabled) const final;

  private:

    /// @brief Method to send diagnostic for rejected commands
    /// @param[in]  cmdNameInfo   The additive information of the diagnostic
    void rejectCmdDiagnosis(const char* cmdNameInfo) const;

    void loadBootingTasks() final;

  protected: // data

    AxisProfilePowerMode m_actualPowerMode = AxisProfilePowerMode::UNDEFINED; ///< Actual power mode of the device (generalized)
    ProfileAssignedInfo m_profileAssignedInfo {false};                        ///< Check if profile variable is assigned
  };
}
