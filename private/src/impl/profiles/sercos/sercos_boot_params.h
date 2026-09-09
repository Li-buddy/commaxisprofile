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
#include "profiles/sercos/sercos_defs.h"
#include "profiles/i_device_boot_param_snapshot.h"

#include <algorithm>
#include <array>
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  /// @brief Descriptor of a Sercos boot parameter.
  struct BootParamDescriptor
  {
    std::uint16_t idn;
    BootParamValueType type;
    std::size_t valueLen;
    DeviceSeries series;
  };


  /// @brief Ordered descriptor table used as single source for both lookup and default boot IDN lists.
  /// @note The order of the table is important, keep it ascending order by idn.
  inline const std::array<BootParamDescriptor, 46> SERCOS_BOOT_PARAM_DESCRIPTOR_TABLE {{
    {S_0_0026_CONFIG_LIST_SIGNAL_STATUS_WORD, BootParamValueType::BYTEARRAY,  LIST_BUFF_DEFAULT_SIZE,      DeviceSeries::INDRADRIVE},
    {S_0_0032_OP_MODE_PRIMARY,                BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0033_OP_MODE_SECONDARY1,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0034_OP_MODE_SECONDARY2,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0035_OP_MODE_SECONDARY3,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0038_POSITIVE_VEL_LIMIT,             BootParamValueType::SIGNED32,   sizeof(std::int32_t),        DeviceSeries::INDRADRIVE},
    {S_0_0039_NEGATIVE_VEL_LIMIT,             BootParamValueType::SIGNED32,   sizeof(std::int32_t),        DeviceSeries::INDRADRIVE},
    {S_0_0044_SCALING_VEL_TYPE,               BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0045_SCALING_VEL_FACTOR,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0046_SCALING_VEL_EXPONENT,           BootParamValueType::SIGNED16,   sizeof(std::int16_t),        DeviceSeries::INDRADRIVE},
    {S_0_0049_POSITIVE_POS_LIMIT,             BootParamValueType::SIGNED32,   sizeof(std::int32_t),        DeviceSeries::INDRADRIVE},
    {S_0_0050_NEGATIVE_POS_LIMIT,             BootParamValueType::SIGNED32,   sizeof(std::int32_t),        DeviceSeries::INDRADRIVE},
    {S_0_0055_POSITION_POLARITIES,            BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0076_SCALING_POS_TYPE,               BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0077_SCALING_POS_FACTOR,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0078_SCALING_POS_EXPONENT,           BootParamValueType::SIGNED16,   sizeof(std::int16_t),        DeviceSeries::INDRADRIVE},
    {S_0_0079_SCALING_POS_ROT_RES,            BootParamValueType::UNSIGNED32, sizeof(std::uint32_t),       DeviceSeries::INDRADRIVE},
    {S_0_0086_SCALING_TORQUE_TYPE,            BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0091_BIPOLARE_VEL_LIMIT,             BootParamValueType::UNSIGNED32, sizeof(std::uint32_t),       DeviceSeries::INDRADRIVE},
    {S_0_0092_BIPOLARE_TORQUE_LIMIT,          BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0093_SCALING_TORQUE_FACTOR,          BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0094_SCALING_TORQUE_EXPONENT,        BootParamValueType::SIGNED16,   sizeof(std::int16_t),        DeviceSeries::INDRADRIVE},
    {S_0_0103_SCALING_POS_MOD_VALUE,          BootParamValueType::UNSIGNED32, sizeof(std::uint32_t),       DeviceSeries::INDRADRIVE},
    {S_0_0115_ENCODER_2_PROPERTY,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0121_GEAR_INPUT_REVOLUTION,          BootParamValueType::UNSIGNED32, sizeof(std::uint32_t),       DeviceSeries::INDRADRIVE},
    {S_0_0122_GEAR_OUTPUT_REVOLUTION,         BootParamValueType::UNSIGNED32, sizeof(std::uint32_t),       DeviceSeries::INDRADRIVE},
    {S_0_0123_FEED_CONSTANT,                  BootParamValueType::UNSIGNED32, sizeof(std::uint32_t),       DeviceSeries::INDRADRIVE},
    {S_0_0138_BIPOLAR_ACC_LIMIT_VALUE,        BootParamValueType::UNSIGNED32, sizeof(std::uint32_t),       DeviceSeries::INDRADRIVE},
    {S_0_0160_SCALING_ACC_TYPE,               BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0161_SCALING_ACC_FACTOR,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0162_SCALING_ACC_EXPONENT,           BootParamValueType::SIGNED16,   sizeof(std::int16_t),        DeviceSeries::INDRADRIVE},
    {S_0_0277_ENCODER_1_PROPERTY,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0284_OP_MODE_SECONDARY4,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0285_OP_MODE_SECONDARY5,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0286_OP_MODE_SECONDARY6,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0287_OP_MODE_SECONDARY7,             BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {S_0_0328_ASSIGN_LIST_SIGNAL_STATUS_WORD, BootParamValueType::BYTEARRAY,  LIST_BUFF_DEFAULT_SIZE,      DeviceSeries::INDRADRIVE},
    {S_0_0349_BIPOLAR_JERK_LIMIT,             BootParamValueType::SIGNED32,   sizeof(std::int32_t),        DeviceSeries::INDRADRIVE},
    {S_0_0372_DRIVE_HALT_ACC_BIPOLAR,         BootParamValueType::SIGNED32,   sizeof(std::int32_t),        DeviceSeries::INDRADRIVE},
    {S_0_0429_EMERGENCY_HALT_DECELERATION,    BootParamValueType::SIGNED32,   sizeof(std::int32_t),        DeviceSeries::INDRADRIVE},
    {S_0_0533_NOMINAL_TORQUE,                 BootParamValueType::UNSIGNED32, sizeof(std::uint32_t),       DeviceSeries::INDRADRIVE},
    {P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR, BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {P_0_0119_CONTROL_UNIT_BEST_POSSIBLE_DEC, BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {P_0_0187_POS_PROCESS_MODE,               BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::INDRADRIVE},
    {P_0_0117_0_1_REACTION_DELAY_F2_F3,       BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::CTRLXDRIVE},
    {P_0_0117_0_2_REACTION_DELAY_F4_F6_F7,    BootParamValueType::UNSIGNED16, sizeof(std::uint16_t),       DeviceSeries::CTRLXDRIVE}
  }};

  inline const BootParamDescriptor* getSercosBootParamDescriptor(std::uint16_t idn)
  {
    const auto it = std::ranges::lower_bound(SERCOS_BOOT_PARAM_DESCRIPTOR_TABLE, idn, std::less {},
                                             &BootParamDescriptor::idn);
    if (it != SERCOS_BOOT_PARAM_DESCRIPTOR_TABLE.end() && it->idn == idn)
    {
      return &(*it);
    }

    return nullptr;
  }

  /// @brief Collect boot IDNs from the descriptor table matching a predicate.
  inline std::vector<std::uint16_t> getSoeBootParamList(const std::function<bool(const BootParamDescriptor&)>& predicate)
  {
    std::vector<std::uint16_t> result {};
    result.reserve(SERCOS_BOOT_PARAM_DESCRIPTOR_TABLE.size());
    for (const auto& descriptor : SERCOS_BOOT_PARAM_DESCRIPTOR_TABLE)
    {
      if (predicate(descriptor))
      {
        result.push_back(descriptor.idn);
      }
    }
    return result;
  }

  /// @brief Get default boot IDN list for Indra drive.
  inline std::vector<std::uint16_t> getSoeIndraDriveBootParamList()
  {
    return getSoeBootParamList([] (const BootParamDescriptor& d) {
                                 return d.series == DeviceSeries::INDRADRIVE;
                               });
  }

  /// @brief Get default boot IDN list for CtrlX drive.
  inline std::vector<std::uint16_t> getSoeCtrlxDriveBootParamList()
  {
    return getSoeBootParamList([] (const BootParamDescriptor& d) {
                                 return d.series == DeviceSeries::INDRADRIVE || d.series == DeviceSeries::CTRLXDRIVE;
                               });
  }
}
