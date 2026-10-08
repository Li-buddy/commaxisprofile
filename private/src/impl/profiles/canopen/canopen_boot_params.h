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
#include "profiles/canopen/canopen_defs.h"
#include "profiles/i_device_boot_param_snapshot.h"

#include <array>
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  /// @brief Descriptor of a Canopen boot parameter.
  struct BootParamDescriptor
  {
    ObjectDirEntry objEntry;
    BootParamValueType type;
  };

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  inline const std::array<BootParamDescriptor, 17> COE_BOOT_PARAM_DESCRIPTOR_TABLE {{
    {ENCODER_1_PROPERTY,              BootParamValueType::UNSIGNED16},
    {ENCODER_2_PROPERTY,              BootParamValueType::UNSIGNED16},
    {SIGNAL_STATUS_WORD_CONFIG_LIST,  BootParamValueType::BYTEARRAY},
    {SIGNAL_CONTROL_WORD_CONFIG_LIST, BootParamValueType::BYTEARRAY},
    {SCALING_SI_UNIT_VELOCITY,        BootParamValueType::UNSIGNED32},
    {SCALING_SI_UNIT_POSITION,        BootParamValueType::UNSIGNED32},
    {SCALING_TORQUE_SCALING_TYPE,     BootParamValueType::UNSIGNED16},
    {SCALING_TORQUE_FACTOR,           BootParamValueType::UNSIGNED16},
    {SCALING_TORQUE_EXPONENT,         BootParamValueType::SIGNED16},
    {SCALING_GEAR_REVOLUTION_IN,      BootParamValueType::UNSIGNED32},
    {SCALING_GEAR_REVOLUTION_OUT,     BootParamValueType::UNSIGNED32},
    {SCALING_FEED_CONSTANT,           BootParamValueType::UNSIGNED32},
    {SCALING_SI_UNIT_ACCELERATION,    BootParamValueType::UNSIGNED32},
    {SIGNAL_STATUS_WORD_ASSIGN_LIST,  BootParamValueType::BYTEARRAY},
    {SIGNAL_CONTROL_WORD_ASSIGN_LIST, BootParamValueType::BYTEARRAY},
    {SCALING_NOMINAL_TORQUE,          BootParamValueType::UNSIGNED32},
    {QUICK_STOP_OPTION,               BootParamValueType::SIGNED16},
  }};

  inline const BootParamDescriptor* getCanopenBootParamDescriptor(const ObjectDirEntry& objEntry)
  {
    for (const auto& descriptor : COE_BOOT_PARAM_DESCRIPTOR_TABLE)
    {
      if (descriptor.objEntry.index == objEntry.index && descriptor.objEntry.subIndex == objEntry.subIndex)
      {
        return &descriptor;
      }
    }

    return nullptr;
  }

  /// @brief Build the default CoE boot parameter list from descriptor table.
  inline std::vector<ObjectDirEntry> getCanopenBootParamList()
  {
    std::vector<ObjectDirEntry> result {};
    result.reserve(COE_BOOT_PARAM_DESCRIPTOR_TABLE.size());
    for (const auto& descriptor : COE_BOOT_PARAM_DESCRIPTOR_TABLE)
    {
      result.push_back(descriptor.objEntry);
    }
    return result;
  }

}
