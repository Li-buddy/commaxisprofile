#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and declare class AxisProfileCommandDrive.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
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
  constexpr char     CMD_LOCKED[] = "current command is currently locked by another thread";  ///< trace message
  constexpr std::int16_t  BIT_ZERO = 0;                                                        ///< zero bit

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief command helper structure
  struct CmdCommonData
  {
    // reference value data
    std::uint16_t m_clampingTorqueConversedValue {0};             ///< clampingTorque conversed value

    // real time status bit 1 of status word S-0-0135
    std::uint16_t m_statusWordBitOne {0};                         ///< set value to S-0-0305
    std::uint16_t m_statusWordBitOneBackup {0};                   ///< backup value to S-0-0305
    std::uint16_t m_allocatedBitNumber {0};                       ///< set value to S-0-0415 (0...15)
    std::uint16_t m_allocatedBitNumberBackup {0};                 ///< backup value to S-0-0415 (0...15)
    bool m_cycSSCheckActivated {false};                           ///< flag indicates if cyclic standstill check is activated

    // Set absolute position and referencing axis
    AxisProfileEncoder m_encoder {AxisProfileEncoder::UNDEFINED}; ///< indicates encoder one or two
    bool     m_ifDriveEnabled {false};                            ///< drive enabled
    std::int32_t  m_settingPosition {0};                          ///< setAbsPos the position to set 
    std::uint16_t m_absPosControl {1U};                           ///< default: set encoder 1
    std::uint16_t m_absPosControlBackup {1U};                     ///< backup of the current setting (for recovering once cmd completed)
    std::int32_t  m_encoderRefBackup {0U};                        ///< encoder reference backup (for recovering once cmd completed)      
    std::int32_t  m_homingPos {0};                                ///< homing position    
    std::int32_t  m_homingOffset {0};                             ///< homing offset
    std::uint16_t m_homingConfig {96};                            ///< homing config,96 represent current state is not one of SERCOS_EVALUATION_OF_HOME_SWITCH,SERCOS_EVALUATION_OF_REF_MARK,SERCOS_EVALUATION_OF_RANGE,SERCOS_EVALUATION_OF_AXS_BLOCKING
    std::int8_t   m_homingMethod {37};                            ///< homing method
    // Op mode
    std::int8_t   m_opMode {8};                                   ///< get operation mode
    bool     m_opAssignedStatus {false};                          ///< check if op mode related PDO assigned
    // drive halt
    std::int32_t m_actualVel {0};                                  ///< actual velocity
    std::int16_t m_deviceStatusWord {0};                           ///< device status word
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @class AxisProfileCommand
  /// This is the base class for command handling.
  class AxisProfileCommandDrive : public AxisProfileCommand
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo     reference to the Profile Basic Information
    explicit AxisProfileCommandDrive(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~AxisProfileCommandDrive() noexcept override = default;

    AxisProfileResult resetAxisProfileCommandState(const AxisProfileCommandType type) final;
    AxisProfileResult resetAllAxisprofileCommands() final;
    AxisProfileResult abortAxisProfileCommand(const AxisProfileCommandType type) final;
    AxisProfileResult getCommandsAtDeterminedState(const AxisprofileCommandState cmdState, std::vector<AxisProfileCommandType>& vecCmdType) final;

  protected: // method
        
    /// @brief This method set the home offset between device and control in SI
    /// @param[in]      newRefPos       Home position in SI unit
    /// @param[in]      deviceActualPos Actual position of SI unit
    void setHomeOffset(double newRefPos, double deviceActualPos);

    /// @brief This method reset the offset to 0 between device and motion
    void resetHomeOffset();

    ///////////////////////////////////////////////Methods For Ctrl Based Homing//////////////////////////////////////
    /// @brief This method set control based homing mode
    /// @param[in] mode homing mode
    void setMethod(RefMode mode);

    /// @brief This method get control based homing method
    /// @return           homing mode
    RefMode getRefMode() const;

  protected: // data

    UniversalFlag m_checkHomedStatusFlag {};                          ///< UniversalFlag for m_checkHomedStatusFlag
    CmdCommonData m_cmdData {};                                       ///< Command related data

    RefMode m_refMode {};                                             ///< Control based homing mode

  };
}
