#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the booting parameters of Soe
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
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  /// @brief This const list indicate the default booting parameters of Indra drive Soe
  inline const std::vector<std::uint16_t> SOE_INDRA_DRIVE_BOOT_PARAM_LIST = {
    S_0_0026_CONFIG_LIST_SIGNAL_STATUS_WORD,
    S_0_0032_OP_MODE_PRIMARY,
    S_0_0033_OP_MODE_SECONDARY1,
    S_0_0034_OP_MODE_SECONDARY2,
    S_0_0035_OP_MODE_SECONDARY3,
    S_0_0038_POSITIVE_VEL_LIMIT,
    S_0_0039_NEGATIVE_VEL_LIMIT, 
    S_0_0044_SCALING_VEL_TYPE,
    S_0_0045_SCALING_VEL_FACTOR,
    S_0_0046_SCALING_VEL_EXPONENT,
    S_0_0049_POSITIVE_POS_LIMIT,
    S_0_0050_NEGATIVE_POS_LIMIT,
    S_0_0055_POSITION_POLARITIES,
    S_0_0076_SCALING_POS_TYPE,
    S_0_0077_SCALING_POS_FACTOR,
    S_0_0078_SCALING_POS_EXPONENT,
    S_0_0079_SCALING_POS_ROT_RES,
    S_0_0086_SCALING_TORQUE_TYPE,
    S_0_0091_BIPOLARE_VEL_LIMIT,
    S_0_0092_BIPOLARE_TORQUE_LIMIT,
    S_0_0093_SCALING_TORQUE_FACTOR,
    S_0_0094_SCALING_TORQUE_EXPONENT,
    S_0_0103_SCALING_POS_MOD_VALUE,
    S_0_0115_ENCODER_2_PROPERTY,
    S_0_0121_GEAR_INPUT_REVOLUTION,
    S_0_0122_GEAR_OUTPUT_REVOLUTION,
    S_0_0123_FEED_CONSTANT,   
    S_0_0138_BIPOLAR_ACC_LIMIT_VALUE,    
    S_0_0160_SCALING_ACC_TYPE,
    S_0_0161_SCALING_ACC_FACTOR,
    S_0_0162_SCALING_ACC_EXPONENT,
    S_0_0277_ENCODER_1_PROPERTY,
    S_0_0284_OP_MODE_SECONDARY4,
    S_0_0285_OP_MODE_SECONDARY5,
    S_0_0286_OP_MODE_SECONDARY6,
    S_0_0287_OP_MODE_SECONDARY7,
    S_0_0328_ASSIGN_LIST_SIGNAL_STATUS_WORD,
    //S_0_0331_STANDSTILL_STATUS,
    S_0_0349_BIPOLAR_JERK_LIMIT,
    S_0_0372_DRIVE_HALT_ACC_BIPOLAR,
    //S_0_0403_POSITION_FEEDBACK_VALUE_STATUS,
    S_0_0429_EMERGENCY_HALT_DECELERATION,
    S_0_0533_NOMINAL_TORQUE,
    P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR,
    P_0_0119_CONTROL_UNIT_BEST_POSSIBLE_DEC,
    P_0_0187_POS_PROCESS_MODE
  };

  /// @brief This const list indicate the default booting parameters of CtrlX drive Soe
  const std::vector<std::uint16_t> SOE_CTRLX_DRIVE_BOOT_PARAM_LIST = [] {
      auto result = SOE_INDRA_DRIVE_BOOT_PARAM_LIST;
      result.push_back(P_0_0117_0_1_REACTION_DELAY_F2_F3);
      result.push_back(P_0_0117_0_2_REACTION_DELAY_F4_F6_F7);
      return result;
  }();
}
