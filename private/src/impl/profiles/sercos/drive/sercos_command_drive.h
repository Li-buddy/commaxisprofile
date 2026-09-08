#pragma once

/// @file
///
/// @brief This source file is part of the axisprofile library and declare the SercosCommandDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <list>
#include "profiles/sercos/sercos_defs.h"
#include "axisprofile_internal_defs.h"
#include "profiles/category/drive/axisprofile_command_drive.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestSercosCommand;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr std::uint16_t SERCOS_FEEDRATE_OVERRIDE = (std::uint16_t)10000U;                ///< 100%
  //Bit control of homing method
  constexpr std::uint16_t SERCOS_EVALUATION_OF_HOME_SWITCH = (std::uint16_t)0x0020;        ///< binary 0000 0000 0010 0000
  constexpr std::uint16_t SERCOS_EVALUATION_OF_REF_MARK = (std::uint16_t)0x0040;           ///< binary 0000 0000 0100 0000
  constexpr std::uint16_t SERCOS_EVALUATION_OF_RANGE = (std::uint16_t)0x0200;              ///< binary 0000 0010 0000 0000
  constexpr std::uint16_t SERCOS_EVALUATION_OF_AXS_BLOCKING = (std::uint16_t)0x0400;       ///< binary 0000 0100 0000 0000

  // Homed judge parameters
  constexpr std::uint16_t AXIS_HOMED_VALUE_ENCODER_1 = 0x0003;                        ///< Check if the axis is homed by encoder 1.
  constexpr std::uint16_t AXIS_HOMED_VALUE_ENCODER_2 = 0x0005;                        ///< Check if the axis is homed by encoder 2.

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
      
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class SercosParamAccess;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// This class implements all sercos commands.
  class SercosCommandDrive : public AxisProfileCommandDrive
  {
  public: // methods
    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Reference to the Profile Basic Information
    /// @param[in]   at                 Pointer to the acknowledge data
    explicit SercosCommandDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at);

    /// @brief Destructor
    virtual ~SercosCommandDrive() noexcept override = default;

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

  private: // methods

    void loadBootingTasks() final;

    /// @brief This method initialize set absolute position of the drive
    /// @param[in]   position           The position value to set (degree, or meter)
    /// @param[in]   encoder            The encoder value to set  (encoder 1 or encoder 2)
    /// @param[in]   ifDriveEnabled     If the user wants to trigger the command while drive in enabled state (for sercos drive, i.e. AF state)
    /// @return                         Return value indicating success or error.
    AxisProfileResult initSetAbsolutePosition(const double position, const AxisProfileEncoder encoder, const bool ifDriveEnabled);

    /// @brief This method executes setAbsolutePosition step by step
    /// @return      Return value indicating success or error.
    AxisProfileResult executeSetAbsolutePosition();

    /// @brief This method initialize referencing the drive
    /// @param[in]   newRefPos          Referencing position after homing
    /// @param[in]   encoder            The encoder value to set  (encoder 1 or encoder 2)
    /// @param[in]   method             use which method to set new position
    /// @return                         Return value indicating success or error.
    AxisProfileResult initReferencingScaling(const double newRefPos, const AxisProfileEncoder encoder, RefMode mode);

    /// @brief This method executes referencingAxis step by step
    /// @return      Return value indicating success or error.
    AxisProfileResult executeReferencingAxis();        

    /// @brief This method initialize this instance during booting time
    /// @return      Return indicating success or error.
    AxisProfileResult initSercosCommandDrive();

    /// @brief This method executes reset referencing Axis
    /// @return      Return value indicating success or error.
    AxisProfileResult executeResetReferencingAxis();

    /// @brief execute referencing position
    /// @param[in,out] asyncState        asyncState  Async state machine 
    /// @return        Return value indicating success or error
    AxisProfileResult executeSetReferencePosition(AsyncCommandStatemachine& asyncState);

    /// @brief This method executes reset absolute position command
    /// @return        Return value indicating success or error.
    AxisProfileResult executeResetAbsolutePosition();

    /// @brief Establish the reference position
    /// @param[in,out]  asyncState      async state machine
    /// @param[in]      conditionsCheck check some configurations to judge if the command should execute further
    /// @param[in]      taskList        task list to execute
    /// @return         Return value indicating success or error
    AxisProfileResult establishReference(AsyncCommandStatemachine* asyncCommandStatemachine, 
                                          std::function<AxisProfileResult()> conditionsCheck,
                                          std::list<SercosParamStruct>& taskList);

  private: // data

    friend class ::TestSercosCommand;               ///< unit test of the SercosCommandDrive

    const AcknowledgeTelegram* m_telegramAT{};      ///< Pointer to the Acknowledge Telegram (AT)
    OperaterEncoder m_operatingEncoder {};          ///< the current operating encoder 
    SercosParamAccess* m_paramAccess {};            ///< Pointer to the Sercos async access handler

    static const std::string TORQUE_TYPE_MISMATCH;  ///< for trace error

    std::list<SercosParamStruct> m_paramListSetAbsPos{};               ///< parameter list to be access asynchronously for set absolute position
    std::list<SercosParamStruct> m_paramListReferencing{};             ///< parameter list to be access asynchronously for referencing
    std::list<SercosParamStruct> m_paramListSetRefPos{};               ///< parameter list to be access asynchronously for set reference position
    std::list<SercosParamStruct> m_paramListActiveStandstillCheck{};   ///< parameter list to be access asynchronously for active standstill check
    std::list<SercosParamStruct> m_paramListDeactiveStandstillCheck{}; ///< parameter list to be access asynchronously for deactivate standstill check
  };
} // namespace comm
