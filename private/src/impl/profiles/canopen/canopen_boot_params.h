#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the booting parameters of Coe
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  /// @brief This const list indicate the default booting parameters of CoE
  inline const std::vector<ObjectDirEntry> COE_BOOT_PARAM_LIST = {
    ENCODER_1_PROPERTY,
    ENCODER_2_PROPERTY,
    SIGNAL_STATUS_WORD_CONFIG_LIST,
    //SUPPORTED_DRIVE_MODE,
    SCALING_SI_UNIT_VELOCITY,
    SCALING_SI_UNIT_POSITION,
    SCALING_TORQUE_SCALING_TYPE,
    SCALING_TORQUE_FACTOR,
    SCALING_TORQUE_EXPONENT,
    SCALING_GEAR_REVOLUTION_IN,
    SCALING_GEAR_REVOLUTION_OUT,
    SCALING_FEED_CONSTANT,
    SCALING_SI_UNIT_ACCELERATION,
    SIGNAL_STATUS_WORD_ASSIGN_LIST,
    //HOMING_FEEDBACK_VALUE_STATUS,
    SCALING_NOMINAL_TORQUE,
    //TORQUE_FORCE_OPTIONS_CODE,
    QUICK_STOP_OPTION,
    //PROBE1_POS_LATCHED_TIMES,
    //PROBE1_NEG_LATCHED_TIMES,
    //PROBE2_POS_LATCHED_TIMES,
    //PROBE2_NEG_LATCHED_TIMES,
  };

}
