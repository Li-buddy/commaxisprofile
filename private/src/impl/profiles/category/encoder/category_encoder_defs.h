#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and declares the data definition of category related.
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
#include <map>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------   

namespace comm::axisprofile::category_encoder
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
  /// @brief Container of input all mapping
  static const std::map<InputMappingValueID, MappingCfgInfo> INPUT_ALL_MAPPING
  {
    {InputMappingValueID::POSITION, {MappingGroup::BASE, true}},
    {InputMappingValueID::VELOCITY, {MappingGroup::VELOCITY, false}},
    {InputMappingValueID::IO_IN_REFERENCE, {MappingGroup::IO_DEVICE, false}},
    {InputMappingValueID::SIGNAL_STATUS_WORD, {MappingGroup::IO_DEVICE, false}}// here the signal status word is used for reference check so it is reasonable to exist in IO_DEVICE
  };

  /// @brief Container of output all mapping
  static const std::map<OutputMappingValueID, MappingCfgInfo> OUTPUT_ALL_MAPPING
  {
    {OutputMappingValueID::IO_SET_REFERENCE, {MappingGroup::IO_DEVICE, false}},
    {OutputMappingValueID::IO_REFERENCE_OFFSET, {MappingGroup::IO_DEVICE, false}},
  };

  /// @brief Container of all available input mapping items
  static const std::vector<InputMappingValueID> AVAILABLE_INPUT_MAPPING
  {
    InputMappingValueID::POSITION,
    InputMappingValueID::VELOCITY,
    InputMappingValueID::IO_IN_REFERENCE,
    InputMappingValueID::SIGNAL_STATUS_WORD,
  };

  /// @brief Container of all available output mapping items
  static const std::vector<OutputMappingValueID> AVAILABLE_OUTPUT_MAPPING
  {
    OutputMappingValueID::IO_SET_REFERENCE,
    OutputMappingValueID::IO_REFERENCE_OFFSET,
  };

  /// @brief Container of all mandatory input mapping items
  static const std::vector<InputMappingValueID> MANDATORY_INPUT_MAPPING
  {
    InputMappingValueID::POSITION
  };

  /// @brief Container of all mandatory output mapping items
  static const std::vector<OutputMappingValueID> MANDATORY_OUTPUT_MAPPING
  {
  };
}
