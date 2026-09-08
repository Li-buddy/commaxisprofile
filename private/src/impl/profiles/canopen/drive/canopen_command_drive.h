#pragma once

/// @file
///
/// @brief This source file is part of the axisprofile library and declare the CanopenCommandDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/category/drive/axisprofile_command_drive.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/canopen/canopen_param_access.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenCommand;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr std::uint16_t STEP_COUNTER_THRESHOLD = 3;      ///< max position value, 32bit max value
      
  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// This class implements all CiA402 commands.
  class CanopenCommandDrive : public AxisProfileCommandDrive
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Reference to the Profile Basic Information  
    /// @param[in]   at                 Pointer to the Acknowledge Telegram (AT)
    /// @param[in]   mdt                Pointer to the Master Data Telegram (MDT)   
    CanopenCommandDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    ~CanopenCommandDrive() noexcept override = default;

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

  private: // Methods

    void loadBootingTasks() final;

    /// @brief This method initialize references the drive
    /// @param[in]   newRefPos          Referencing position after homing
    /// @return                         Return value indicating success or error.
    AxisProfileResult initReferencingAxis(const double newRefPos);

    /// @brief This method executes referencingAxis step by step
    /// @return      Return value indicating success or error.
    AxisProfileResult executeReferencingAxis();

    /// @brief This method is  clear device error step
    /// @param[in,out]  asyncState      Async state machine
    /// @return      Return value indicating AxisProfileResult.
    AxisProfileResult executeClearDeviceError(AsyncCommandStatemachine& asyncState);        

    /// @brief This method executes resetReferencingAxis step by step
    /// @return      Return value indicating success or error.
    AxisProfileResult executeResetReferencingAxis();

    /// @brief This method initialize this instance during booting time
    /// @return      Return indicating success or error.
    AxisProfileResult initCanopenCommandDrive();

    /// @brief Init the reference set
    /// @param[in]      newRefPos     referencing position after homing
    /// @param[in]      refMode       reference mode
    /// @param[in]      cmdType       command type
    /// @return         Return value indicating success or error
    AxisProfileResult initReferenceSet(const double newRefPos, RefMode refMode, AxisProfileCommandType cmdType);

    /// @brief Establish the reference position
    /// @param[in,out]  asyncState       asyncState  Async state machine 
    /// @return         Return value indicating success or error
    AxisProfileResult establishReference(AsyncCommandStatemachine* asyncCommandStatemachine);

  private: // data

    friend class ::TestCanopenCommand;            // Unit test of the SercosCommandDrive

    const AcknowledgeTelegram* m_telegramAT {};   ///< Pointer to the Acknowledge Telegram (AT)
    MasterDataTelegram* m_telegramMDT {};         ///< Pointer to the Master Data Telegram (MDT)
    CanopenParamAccess* m_paramAccess {};         ///< Pointer to the Canopen async access handler

    std::list<CanopenParamStruct> m_paramListReferencing {};             ///< parameter list to be access asynchronously for homing
    std::list<CanopenParamStruct> m_paramListDriveHalt {};               ///< parameter list to be access asynchronously for check drive halt
    bool m_axisIsHomed{false};                                           ///< indicates if this device is homed.
  };
}
