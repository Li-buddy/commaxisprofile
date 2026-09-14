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
    std::size_t valueLen;
  };

  constexpr std::size_t SIGNAL_STATUS_WORD_LIST_SIZE = 16;

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  inline const std::array<BootParamDescriptor, 15> COE_BOOT_PARAM_DESCRIPTOR_TABLE {{
    {ENCODER_1_PROPERTY,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t)},
    {ENCODER_2_PROPERTY,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t)},
    {SIGNAL_STATUS_WORD_CONFIG_LIST, BootParamValueType::BYTEARRAY,  SIGNAL_STATUS_WORD_LIST_SIZE * sizeof(std::uint32_t)},
    {SCALING_SI_UNIT_VELOCITY,       BootParamValueType::UNSIGNED32, sizeof(std::uint32_t)},
    {SCALING_SI_UNIT_POSITION,       BootParamValueType::UNSIGNED32, sizeof(std::uint32_t)},
    {SCALING_TORQUE_SCALING_TYPE,    BootParamValueType::UNSIGNED16, sizeof(std::uint16_t)},
    {SCALING_TORQUE_FACTOR,          BootParamValueType::UNSIGNED16, sizeof(std::uint16_t)},
    {SCALING_TORQUE_EXPONENT,        BootParamValueType::SIGNED16,   sizeof(std::int16_t)},
    {SCALING_GEAR_REVOLUTION_IN,     BootParamValueType::UNSIGNED32, sizeof(std::uint32_t)},
    {SCALING_GEAR_REVOLUTION_OUT,    BootParamValueType::UNSIGNED32, sizeof(std::uint32_t)},
    {SCALING_FEED_CONSTANT,          BootParamValueType::UNSIGNED32, sizeof(std::uint32_t)},
    {SCALING_SI_UNIT_ACCELERATION,   BootParamValueType::UNSIGNED32, sizeof(std::uint32_t)},
    {SIGNAL_STATUS_WORD_ASSIGN_LIST, BootParamValueType::BYTEARRAY,  SIGNAL_STATUS_WORD_LIST_SIZE * sizeof(std::uint16_t)},
    {SCALING_NOMINAL_TORQUE,         BootParamValueType::UNSIGNED32, sizeof(std::uint32_t)},
    {QUICK_STOP_OPTION,              BootParamValueType::SIGNED16,   sizeof(std::int16_t)},
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
