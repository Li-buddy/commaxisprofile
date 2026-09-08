#pragma once

/// @file
///
/// This header file is part of the axisprofile library and declares the data definition of drive category related.
///
/// @copyright (C) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/value_mapping/mapping_handler_defs.h"
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------   

namespace comm::axisprofile::category_drive
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  
  /// @brief Container of all available input mapping items
  static const std::vector<InputMappingValueID> AVAILABLE_INPUT_MAPPING
  {
    InputMappingValueID::STATUSWORD, InputMappingValueID::POSITION,
    InputMappingValueID::DIAGMSG, InputMappingValueID::VELOCITY,
    InputMappingValueID::SWITCH_REF, InputMappingValueID::SWITCH_POS_LIM, InputMappingValueID::SWITCH_NEG_LIM,
    InputMappingValueID::PROBE1_POSEDG, InputMappingValueID::PROBE1_NEGEDG, InputMappingValueID::PROBE1_ACTIVE,
    InputMappingValueID::PROBE1_POSLAT, InputMappingValueID::PROBE1_NEGLAT, InputMappingValueID::PROBE1_POSDIF,
    InputMappingValueID::PROBE1_MAKFAL,
    InputMappingValueID::PROBE2_POSEDG, InputMappingValueID::PROBE2_NEGEDG, InputMappingValueID::PROBE2_ACTIVE,
    InputMappingValueID::PROBE2_POSLAT, InputMappingValueID::PROBE2_NEGLAT, InputMappingValueID::PROBE2_POSDIF,
    InputMappingValueID::PROBE2_MAKFAL,
    InputMappingValueID::TORQUE_FORCE, InputMappingValueID::TORQUE_LIMIT_STATUS,
    InputMappingValueID::OP_MODE_DISPLAY,
    InputMappingValueID::SMO_SAFETY_SELECTION, InputMappingValueID::SMO_SAFETY_STATUS,
    InputMappingValueID::SIGNAL_STATUS_WORD
  };

  /// @brief Container of all available output mapping items
  static const std::vector<OutputMappingValueID> AVAILABLE_OUTPUT_MAPPING
  {
    OutputMappingValueID::CONTROLWORD, OutputMappingValueID::POSITION, OutputMappingValueID::VELOCITY,
    OutputMappingValueID::TORQUE_FORCE, OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH, OutputMappingValueID::TORQUE_VEL_LIMIT_LOW,
    OutputMappingValueID::OP_MODE
  };

  /// @brief Container of all mandatory input mapping items
  static const std::vector<InputMappingValueID> MANDATORY_INPUT_MAPPING
  {
    InputMappingValueID::STATUSWORD
  };

  /// @brief Container of all mandatory output mapping items
  static const std::vector<OutputMappingValueID> MANDATORY_OUTPUT_MAPPING
  {
    OutputMappingValueID::CONTROLWORD
  };
}
