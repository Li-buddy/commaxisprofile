#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and declare class AxisProfileCommandEncoder.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_command.h"

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

  /// @class AxisProfileCommandEncoder
  /// This is the base class for command handling.
  class AxisProfileCommandEncoder : public AxisProfileCommand
  {
  public: // methods   

    /// @brief Constructor
    /// @param[in]      profileBasicInfo   reference to the Profile Basic Information
    /// @param[in]      at                 the acknolowdge telegram
    /// @param[in, out] mdt                the master data telegram
    AxisProfileCommandEncoder(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    ~AxisProfileCommandEncoder() noexcept override = default;

    AxisProfileResult resetAxisProfileCommandState(const AxisProfileCommandType type) final;
    AxisProfileResult resetAllAxisprofileCommands() final;
    AxisProfileResult abortAxisProfileCommand(const AxisProfileCommandType type) final;
    AxisProfileResult getCommandsAtDeterminedState(const AxisprofileCommandState cmdState, std::vector<AxisProfileCommandType>& vecCmdType) final;

    // ---------------------------------- Commands ------------------------------------
    AxisProfileResult clearDeviceError() final;
    AxisProfileResult setAndCheckPositiveStop(const AxisProfilePositiveStop mode) final;
    AxisProfileResult setAndCheckClampingTorque(const double clampingTorque) final;
    AxisProfileResult getClampingTorque(double* const clampingTorque) final;
    AxisProfileResult checkStandStill(bool* const standStill) final;
    AxisProfileResult setAndCheckStandStillWindow(const double standStillWindow) final;
    AxisProfileResult getStandStillWindow(double* const standStillWindow) final;
    AxisProfileResult activateCyclicStandStillCheck() final;
    AxisProfileResult deactivateCyclicStandStillCheck() final;
    AxisProfileResult setAbsolutePosition(const double position, const AxisProfileEncoder encoder, const bool ifDriveEnabled) final;
    AxisProfileResult resetAbsolutePosition() final;
    AxisProfileResult referencingAxis(const double newRefPos, const AxisProfileEncoder encoder) final;
    AxisProfileResult resetReferencingAxis() final;
    AxisProfileResult setReferencingPosition(const double refPos, const AxisProfileEncoder encoder, RefMode mode) final;
    AxisProfileResult addAuxCyclicJobs() final;
    AxisProfileResult checkIfDeviceHalt(bool& isHalted) final;

  private:

    void loadBootingTasks() final;

    /// @brief This method executes setAbsolutePosition step by step
    /// @return      Return value indicating success or error.
    AxisProfileResult executeSetAbsolutePosition();

    /// @brief Method to send diagnostic for rejected commands
    /// @param[in]  cmdNameInfo   The additive information of the diagnostic
    void rejectCmdDiagnosis(const char* cmdNameInfo) const;

  protected: // data

    const AcknowledgeTelegram* m_telegramAT {};   ///< Pointer to the Acknowledge Telegram (AT)
    MasterDataTelegram* m_telegramMDT {};         ///< Pointer to the Master Data Telegram (MDT)
    AxisProfileEncoder m_encoder {};              ///< the encoder id
    double m_position {};                         ///< the command position
  };
}
