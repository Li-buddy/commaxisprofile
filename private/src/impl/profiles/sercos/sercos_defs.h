#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and contains defines relevant
/// for the sercos specification.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/profile_defs.h"
#include "profiles/value_mapping/mapping_handler_defs.h"
#include <map>
#include <string>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ******************** CONTROLWORD ******************** //
  constexpr std::uint16_t SERCOS_CTRLWORD_SYNC_BIT               = 0x0400;    ///< Sercos controlword (S-0-0134), toggle bit mask
  constexpr std::uint16_t SERCOS_CTRLWORD_SYNC_BIT_INVERTED      = 0xFBFF;    ///< Sercos controlword (S-0-0134), toggle bit mask inverted
  constexpr std::uint16_t SERCOS_CTRLWORD_SYNC_BIT_OFFSET        = 0x000A;    ///< Sercos controlword (S-0-0134), toggle bit offset
  constexpr std::uint16_t SERCOS_CTRLWORD_POWERMASK              = 0xE000;    ///< Sercos controlword (S-0-0134), power bitmask (Bits 15-13)
  constexpr std::uint16_t SERCOS_CTRLWORD_POWER_AF               = 0xE000;    ///< Sercos controlword (S-0-0134), power on (AF)
  constexpr std::uint16_t SERCOS_CTRLWORD_POWER_AB               = 0x4000;    ///< Sercos controlword (S-0-0134), power off (AB)
  constexpr std::uint16_t SERCOS_CTRLWORD_POWER_AH               = 0xC000;    ///< Sercos controlword (S-0-0134), drive halt (AH)
  constexpr std::uint16_t SERCOS_CTRLWORD_POWER_TORQUEFREE       = 0xA000;    ///< Sercos controlword (S-0-0134), torque free(AB)
  constexpr std::uint16_t SERCOS_CTRLWORD_POWERMASK_INVERTED     = 0x1FFF;    ///< Sercos controlword (S-0-0134), power bitmask inverted
  constexpr std::uint16_t SERCOS_CTRLWORD_POWERMODE_OFFSET       = 0x000E;    ///< Sercos controlword (S-0-0134), power mode offset
  constexpr std::uint16_t SERCOS_CTRLWORD_OPMODE                 = 0x0B00;    ///< Sercos controlword (S-0-0134), operation mode bitmask (Bits 11, 9, 8)
  constexpr std::uint16_t SERCOS_CTRLWORD_OPMODE_INVERTED        = 0xF4FF;    ///< Sercos controlword (S-0-0134), operation mode bitmask inverted
  constexpr std::uint16_t SERCOS_CTRLWORD_OPMODE_OFFSET          = 0x0008;    ///< Sercos controlword (S-0-0134), operation mode offset

  // ******************** STATUSWORD ******************** //
  constexpr std::uint16_t SERCOS_STSWORD_RDY_TO_OPERATE          = 0xC000;    ///< Sercos statusword (S-0-0135), ready to operate bitmask (Bits 15-14)
  constexpr std::uint16_t SERCOS_STSWORD_RDY_TO_OPERATE_OFFSET   = 0x000E;    ///< Sercos statusword (S-0-0135), ready to operate offset
  constexpr std::uint16_t SERCOS_STSWORD_DRVSHUTDOWN_OFFSET      = 0x000D;    ///< Sercos statusword (S-0-0135), Drive shutdown offset
  constexpr std::uint16_t SERCOS_STSWORD_WRNINGC2D_OFFSET        = 0x000C;    ///< Sercos statusword (S-0-0135), Warning in C2D offset
  constexpr std::uint16_t SERCOS_STSWORD_CHANGEBIT_C3D_OFFSET    = 0x000B;    ///< Sercos statusword (S-0-0135), Change bit in C3D offset
  constexpr std::uint16_t SERCOS_STSWORD_OPMODE                  = 0x0700;    ///< Sercos statusword (S-0-0135), operation mode bitmask (Bits 8-10)
  constexpr std::uint16_t SERCOS_STSWORD_OPMODE_OFFSET           = 0x0008;    ///< Sercos statusword (S-0-0135), operation mode offset
  constexpr std::uint16_t SERCOS_STSWORD_REALTIME_STS2_OFFSET    = 0x0007;    ///< Sercos statusword (S-0-0135), Realtime statusbit 2 offset
  constexpr std::uint16_t SERCOS_STSWORD_REALTIME_STS1_OFFSET    = 0x0006;    ///< Sercos statusword (S-0-0135), Realtime statusbit 1 offset
  constexpr std::uint16_t SERCOS_STSWORD_PROCEDURE_CMDBIT_OFFSET = 0x0005;    ///< Sercos statusword (S-0-0135), Procedure command change bit offset
  constexpr std::uint16_t SERCOS_STSWORD_STS_PARAM_LVL_OFFSET    = 0x0004;    ///< Sercos statusword (S-0-0135), Status parametrization level (PL) offset
  constexpr std::uint16_t SERCOS_STSWORD_CMAPROCESSING_OFFSET    = 0x0003;    ///< Sercos statusword (S-0-0135), Command value processing offset
  constexpr std::uint16_t SERCOS_STSWORD_ERROR_SVC_CHAN_OFFSET   = 0x0002;    ///< Sercos statusword (S-0-0135), Error in service channel offset
  constexpr std::uint16_t SERCOS_STSWORD_BUSY_OFFSET             = 0x0001;    ///< Sercos statusword (S-0-0135), Busy offset
  constexpr std::uint16_t SERCOS_STSWORD_SVC_HANDSHAKE_OFFSET    = 0x0000;    ///< Sercos statusword (S-0-0135), Service handshake offset

  // ******************** SCALING ******************** //
  constexpr std::uint16_t SERCOS_SCA_POS_TYPE_METHOD_MASK        = 0x0007;    ///< Sercos position data scaling type (S-0-0076), Scaling method mask (Bits 2-0)
  constexpr std::uint16_t SERCOS_SCA_POS_TYPE_METHOD_NONE        = 0x0000;    ///< Sercos position data scaling type (S-0-0076), Scaling method none
  constexpr std::uint16_t SERCOS_SCA_POS_TYPE_METHOD_LINEAR      = 0x0001;    ///< Sercos position data scaling type (S-0-0076), Scaling method linear
  constexpr std::uint16_t SERCOS_SCA_POS_TYPE_METHOD_ROT         = 0x0002;    ///< Sercos position data scaling type (S-0-0076), Scaling method rotational
  constexpr std::uint16_t SERCOS_SCA_POS_TYPE_UNIT_OFFSET        = 0x0004;    ///< Sercos position data scaling type (S-0-0076), Unit for linear scaling (Bit 4) bit offset
  constexpr std::uint16_t SERCOS_SCA_POS_TYPE_DATA_FORMAT        = 0x0080;    ///< Sercos position data scaling type (S-0-0076), data format (Bit 7) 0: Absolute format 1 : Modulo format

  constexpr std::uint16_t SERCOS_SCA_VEL_TYPE_METHOD_MASK        = 0x0007;    ///< Sercos velocity data scaling type (S-0-0044), Scaling method mask (Bits 2-0)
  constexpr std::uint16_t SERCOS_SCA_VEL_TYPE_METHOD_NONE        = 0x0000;    ///< Sercos velocity data scaling type (S-0-0044), Scaling method none
  constexpr std::uint16_t SERCOS_SCA_VEL_TYPE_METHOD_ROT         = 0x0002;    ///< Sercos velocity data scaling type (S-0-0044), Scaling method rotational 
  constexpr std::uint16_t SERCOS_SCA_VEL_TYPE_METHOD_LINEAR      = 0x0001;    ///< Sercos velocity data scaling type (S-0-0044), Scaling method linear
  constexpr std::uint16_t SERCOS_SCA_VEL_TYPE_UNIT_OFFSET        = 0x0004;    ///< Sercos velocity data scaling type (S-0-0044), Unit for linear scaling (Bit 4) bit offset
  constexpr std::uint16_t SERCOS_SCA_VEL_TYPE_TIME_UNIT_OFFSET   = 0x0005;    ///< Sercos velocity data scaling type (S-0-0044), Unit for time (Bit 5) bit offset

  constexpr std::uint16_t SERCOS_SCA_ACC_TYPE_METHOD_MASK        = 0x0007;    ///< Sercos acceleration data scaling type (S-0-0160), Scaling method mask (Bits 2-0)
  constexpr std::uint16_t SERCOS_SCA_ACC_TYPE_METHOD_NONE        = 0x0000;    ///< Sercos acceleration data scaling type (S-0-0160), Scaling method none
  constexpr std::uint16_t SERCOS_SCA_ACC_TYPE_METHOD_LINEAR      = 0x0001;    ///< Sercos acceleration data scaling type (S-0-0160), Scaling method linear
  constexpr std::uint16_t SERCOS_SCA_ACC_TYPE_METHOD_ROT         = 0x0002;    ///< Sercos acceleration data scaling type (S-0-0160), Scaling method rotational
  constexpr std::uint16_t SERCOS_SCA_ACC_TYPE_UNIT_OFFSET        = 0x0004;    ///< Sercos acceleration data scaling type (S-0-0160), Unit for linear scaling (Bit 4) bit offset

  constexpr std::uint16_t SERCOS_SCA_TORQUE_TYPE_METHOD_MASK     = 0x0007;    ///< Sercos torque data scaling type (S-0-0086), Scaling method mask (Bits 2-0)
  constexpr std::uint16_t SERCOS_SCA_TORQUE_TYPE_METHOD_PCTG     = 0x0000;    ///< Sercos torque data scaling type (S-0-0086), Scaling method percentage
  constexpr std::uint16_t SERCOS_SCA_TORQUE_TYPE_METHOD_LINEAR   = 0x0001;    ///< Sercos torque data scaling type (S-0-0086), Scaling method linear
  constexpr std::uint16_t SERCOS_SCA_TORQUE_TYPE_METHOD_ROT      = 0x0002;    ///< Sercos torque data scaling type (S-0-0086), Scaling method rotary
  constexpr std::uint16_t SERCOS_SCA_TORQUE_TYPE_UNIT_OFFSET     = 0x0004;    ///< Sercos torque data scaling type (S-0-0086), Unit for linear scaling (Bit 4) bit offset

  // ******************** COMMAND ******************** //
  constexpr std::uint16_t SERCOS_COMMAND_NOT_SET                 = 0x0000;    ///< Sercos command input, not set command / clear command
  constexpr std::uint16_t SERCOS_COMMAND_INTERRUPT               = 0x0001;    ///< Sercos command input, interrupt command
  constexpr std::uint16_t SERCOS_COMMAND_ENABLE                  = 0x0003;    ///< Sercos command input, set and enable command

  constexpr std::uint16_t SERCOS_COMMAND_STATUS_NOT_SET          = 0x0000;    ///< Sercos command status, not set command / clear command
  constexpr std::uint16_t SERCOS_COMMAND_STATUS_FINISH           = 0x0003;    ///< Sercos command status, command executed correctly
  constexpr std::uint16_t SERCOS_COMMAND_STATUS_INTERRUPT        = 0x0005;    ///< Sercos command status, command is interrupted
  constexpr std::uint16_t SERCOS_COMMAND_STATUS_IN_PROCESS       = 0x0007;    ///< Sercos command status, command is in process
  constexpr std::uint16_t SERCOS_COMMAND_STATUS_ERROR            = 0x000F;    ///< Sercos command status, error, command can't execute

  // ******************** Miscellaneous ******************** //
  constexpr std::uint16_t SERCOS_WORD_TOKEN_BIT0                 = 0x0001;    ///< Sercos token of bit 0 of word value

  /// @brief Container of input all mapping
  static const std::map<InputMappingValueID, MappingCfgInfo> INPUT_ALL_MAPPING
  {
    {InputMappingValueID::STATUSWORD, {MappingGroup::BASE, true}}, {InputMappingValueID::POSITION, {MappingGroup::BASE, false}}, {InputMappingValueID::DIAGMSG,
    {MappingGroup::BASE, false}}, {InputMappingValueID::VELOCITY, {MappingGroup::VELOCITY, false}}, {InputMappingValueID::SWITCH_REF, {MappingGroup::HOMING, false}},
    {InputMappingValueID::SWITCH_POS_LIM, {MappingGroup::HOMING, false}}, {InputMappingValueID::SWITCH_NEG_LIM, {MappingGroup::HOMING, false}},
    {InputMappingValueID::PROBE1_POSEDG, {MappingGroup::PROBE, false}}, {InputMappingValueID::PROBE1_NEGEDG, {MappingGroup::PROBE, false}},
    {InputMappingValueID::PROBE1_ACTIVE, {MappingGroup::PROBE, false}}, {InputMappingValueID::PROBE1_POSLAT, {MappingGroup::PROBE, false}},
    {InputMappingValueID::PROBE1_NEGLAT, {MappingGroup::PROBE, false}}, {InputMappingValueID::PROBE1_POSDIF, {MappingGroup::PROBE, false}},
    {InputMappingValueID::PROBE1_MAKFAL, {MappingGroup::PROBE, false}}, {InputMappingValueID::PROBE2_POSEDG, {MappingGroup::PROBE, false}},
    {InputMappingValueID::PROBE2_NEGEDG, {MappingGroup::PROBE, false}}, {InputMappingValueID::PROBE2_ACTIVE, {MappingGroup::PROBE, false}},
    {InputMappingValueID::PROBE2_POSLAT, {MappingGroup::PROBE, false}}, {InputMappingValueID::PROBE2_NEGLAT, {MappingGroup::PROBE, false}},
    {InputMappingValueID::PROBE2_POSDIF, {MappingGroup::PROBE, false}}, {InputMappingValueID::PROBE2_MAKFAL, {MappingGroup::PROBE, false}},
    {InputMappingValueID::TORQUE_FORCE,  {MappingGroup::TORQUE_FORCE, false}}, {InputMappingValueID::TORQUE_LIMIT_STATUS, {MappingGroup::TORQUE_FORCE, false}},
    {InputMappingValueID::SMO_SAFETY_SELECTION, {MappingGroup::SAFE_MOTION, false}}, {InputMappingValueID::SMO_SAFETY_STATUS, {MappingGroup::SAFE_MOTION, false}},
    {InputMappingValueID::USER_PDO_INPUT_1, {MappingGroup::USER_PDO, false}}, {InputMappingValueID::USER_PDO_INPUT_2, {MappingGroup::USER_PDO, false}},
    {InputMappingValueID::USER_PDO_INPUT_3, {MappingGroup::USER_PDO, false}}, {InputMappingValueID::USER_PDO_INPUT_4, {MappingGroup::USER_PDO, false}},
    {InputMappingValueID::USER_PDO_INPUT_5, {MappingGroup::USER_PDO, false}}, {InputMappingValueID::USER_PDO_INPUT_6, {MappingGroup::USER_PDO, false}},
    {InputMappingValueID::SIGNAL_STATUS_WORD, {MappingGroup::SIGNAL_WORD, false}}
  };

  /// @brief Container of output all mapping
  static const std::map<OutputMappingValueID, MappingCfgInfo> OUTPUT_ALL_MAPPING
  {
    {OutputMappingValueID::CONTROLWORD, {MappingGroup::BASE, true}}, {OutputMappingValueID::POSITION, {MappingGroup::BASE, false}},
    {OutputMappingValueID::VELOCITY, {MappingGroup::VELOCITY, false}}, {OutputMappingValueID::TORQUE_FORCE, {MappingGroup::TORQUE_FORCE, false}},
    {OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH, {MappingGroup::TORQUE_FORCE, false}}, {OutputMappingValueID::TORQUE_VEL_LIMIT_LOW, {MappingGroup::TORQUE_FORCE, false}},
    {OutputMappingValueID::USER_PDO_OUTPUT_1, {MappingGroup::USER_PDO, false}}, {OutputMappingValueID::USER_PDO_OUTPUT_2, {MappingGroup::USER_PDO, false}},
    {OutputMappingValueID::USER_PDO_OUTPUT_3, {MappingGroup::USER_PDO, false}}, {OutputMappingValueID::USER_PDO_OUTPUT_4, {MappingGroup::USER_PDO, false}},
    {OutputMappingValueID::USER_PDO_OUTPUT_5, {MappingGroup::USER_PDO, false}}, {OutputMappingValueID::USER_PDO_OUTPUT_6, {MappingGroup::USER_PDO, false}}
  };

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------
  /// @brief This method will check the target contained in the array
  /// @tparam[in] N     The size of array.
  /// @param[in] arr    The reference of array.
  /// @param[in] target The target value.
  /// @return    whether the target value contained in this array.
  template <std::size_t N>
  inline bool contains(const std::array<uint16_t, N>& arr, uint16_t target)
  {
    for (std::size_t i = 0; i < N; ++i)
    {
      if (arr[i] == target)
      {
        return true;
      }
    }
    return false;
  }
  /// @brief The follow IDN are general S-parameters
  constexpr std::uint16_t S_0_0000_DUMMY                          = IDN_S(0, 0);      ///< S-0-0000: Dummy parameter, used for initialization
  constexpr std::uint16_t S_0_0026_CONFIG_LIST_SIGNAL_STATUS_WORD = IDN_S(0, 26);     ///< S-0-0026: Configuration list for signal status word
  constexpr std::uint16_t S_0_0030_SW_VERSION                     = IDN_S(0, 30);     ///< S-0-0030: Manufacturer software version
  constexpr std::uint16_t S_0_0032_OP_MODE_PRIMARY                = IDN_S(0, 32);     ///< S-0-0032: Primary operation mode
  constexpr std::uint16_t S_0_0033_OP_MODE_SECONDARY1             = IDN_S(0, 33);     ///< S-0-0033: Secondary operation mode 1
  constexpr std::uint16_t S_0_0034_OP_MODE_SECONDARY2             = IDN_S(0, 34);     ///< S-0-0034: Secondary operation mode 2
  constexpr std::uint16_t S_0_0035_OP_MODE_SECONDARY3             = IDN_S(0, 35);     ///< S-0-0035: Secondary operation mode 3
  constexpr std::uint16_t S_0_0038_POSITIVE_VEL_LIMIT             = IDN_S(0, 38);     ///< S-0-0038: Positive velocity limit value
  constexpr std::uint16_t S_0_0039_NEGATIVE_VEL_LIMIT             = IDN_S(0, 39);     ///< S-0-0039: Negative velocity limit value
  constexpr std::uint16_t S_0_0041_HOMING_VELOCITY                = IDN_S(0, 41);     ///< S-0-0041: Homing velocity
  constexpr std::uint16_t S_0_0042_HOMING_ACCELERATION            = IDN_S(0, 42);     ///< S-0-0042: Homing acceleration
  constexpr std::uint16_t S_0_0044_SCALING_VEL_TYPE               = IDN_S(0, 44);     ///< S-0-0044: Scaling type for velocity
  constexpr std::uint16_t S_0_0045_SCALING_VEL_FACTOR             = IDN_S(0, 45);     ///< S-0-0045: Velocity data scaling factor
  constexpr std::uint16_t S_0_0046_SCALING_VEL_EXPONENT           = IDN_S(0, 46);     ///< S-0-0046: Velocity data scaling exponent
  constexpr std::uint16_t S_0_0049_POSITIVE_POS_LIMIT             = IDN_S(0, 49);     ///< S-0-0049: Positive position limit value
  constexpr std::uint16_t S_0_0050_NEGATIVE_POS_LIMIT             = IDN_S(0, 50);     ///< S-0-0050: Negative position limit value
  constexpr std::uint16_t S_0_0051_POSITION_FEEDBACK_ENCODER_1    = IDN_S(0, 51);     ///< S-0-0051: Position feedback value of encoder 1
  constexpr std::uint16_t S_0_0052_REFERENCE_DISTANCE_ENCODER_1   = IDN_S(0, 52);     ///< S-0-0052: Reference distance of encoder 1
  constexpr std::uint16_t S_0_0053_POSITION_FEEDBACK_ENCODER_2    = IDN_S(0, 53);     ///< S-0-0053: Position feedback value of encoder 2
  constexpr std::uint16_t S_0_0054_REFERENCE_DISTANCE_ENCODER_2   = IDN_S(0, 54);     ///< S-0-0054: Reference distance of encoder 2
  constexpr std::uint16_t S_0_0055_POSITION_POLARITIES            = IDN_S(0, 55);     ///< S-0-0055: Switch on/off the software limit
  constexpr std::uint16_t S_0_0076_SCALING_POS_TYPE               = IDN_S(0, 76);     ///< S-0-0076: Position data scaling type
  constexpr std::uint16_t S_0_0077_SCALING_POS_FACTOR             = IDN_S(0, 77);     ///< S-0-0077: Position data scaling factor
  constexpr std::uint16_t S_0_0078_SCALING_POS_EXPONENT           = IDN_S(0, 78);     ///< S-0-0078: Position data scaling exponent
  constexpr std::uint16_t S_0_0079_SCALING_POS_ROT_RES            = IDN_S(0, 79);     ///< S-0-0079: Rotational position resolution
  constexpr std::uint16_t S_0_0086_SCALING_TORQUE_TYPE            = IDN_S(0, 86);     ///< S-0-0086: Torque/force data scaling type
  constexpr std::uint16_t S_0_0091_BIPOLARE_VEL_LIMIT             = IDN_S(0, 91);     ///< S-0-0091: Bipolar velocity limit value
  constexpr std::uint16_t S_0_0092_BIPOLARE_TORQUE_LIMIT          = IDN_S(0, 92);     ///< S-0-0092: Torque/force bipolar limit value
  constexpr std::uint16_t S_0_0093_SCALING_TORQUE_FACTOR          = IDN_S(0, 93);     ///< S-0-0093: Torque/force data scaling factor
  constexpr std::uint16_t S_0_0094_SCALING_TORQUE_EXPONENT        = IDN_S(0, 94);     ///< S-0-0094: Torque/force data scaling exponent
  constexpr std::uint16_t S_0_0099_RESET_CLASS_1_DIAGNOSTIC       = IDN_S(0, 99);     ///< S-0-0099: C0500 Reset class 1 diagnostic
  constexpr std::uint16_t S_0_0103_SCALING_POS_MOD_VALUE          = IDN_S(0, 103);    ///< S-0-0103: Maximum travel range: Modulo format
  constexpr std::uint16_t S_0_0108_FEEDRATE_OVERRIDE              = IDN_S(0, 108);    ///< S-0-0108: Feedrate override
  constexpr std::uint16_t S_0_0115_ENCODER_2_PROPERTY             = IDN_S(0, 115);    ///< S-0-0115: Property of Encoder 2
  constexpr std::uint16_t S_0_0121_GEAR_INPUT_REVOLUTION          = IDN_S(0, 121);    ///< S-0-0121: Gear input revolution
  constexpr std::uint16_t S_0_0122_GEAR_OUTPUT_REVOLUTION         = IDN_S(0, 122);    ///< S-0-0122: Gear output revolution
  constexpr std::uint16_t S_0_0123_FEED_CONSTANT                  = IDN_S(0, 123);    ///< S-0-0123: Feed constant
  constexpr std::uint16_t S_0_0124_STANDSTILL_WINDOW              = IDN_S(0, 124);    ///< S-0-0124: Standstill window
  constexpr std::uint16_t S_0_0130_PROBE1_POS_EDGE                = IDN_S(0, 130);    ///< S-0-0130: Probe1 positive edge value
  constexpr std::uint16_t S_0_0131_PROBE1_NEG_EDGE                = IDN_S(0, 131);    ///< S-0-0131: Probe1 negative edge value
  constexpr std::uint16_t S_0_0132_PROBE2_POS_EDGE                = IDN_S(0, 132);    ///< S-0-0132: Probe2 positive edge value
  constexpr std::uint16_t S_0_0133_PROBE2_NEG_EDGE                = IDN_S(0, 133);    ///< S-0-0133: Probe2 negative edge value
  constexpr std::uint16_t S_0_0134_MASTER_CONTROL_WORD            = IDN_S(0, 134);    ///< S-0-0134: Master control word
  constexpr std::uint16_t S_0_0138_BIPOLAR_ACC_LIMIT_VALUE        = IDN_S(0, 138);    ///< S-0-0138: Bipolar acceleration limit value
  constexpr std::uint16_t S_0_0147_HOMING_PARAMETER               = IDN_S(0, 147);    ///< S-0-0147: Homing configuration parameter
  constexpr std::uint16_t S_0_0148_DRIVE_CONTROLLED_HOMING        = IDN_S(0, 148);    ///< S-0-0148: C0600 Drive-controlled homing procedure command
  constexpr std::uint16_t S_0_0149_FIXED_STOP_COMMAND             = IDN_S(0, 149);    ///< S-0-0149: C1300 Positive stop drive procedure command
  constexpr std::uint16_t S_0_0150_REFERENCE_OFFSET_ENCODER_1     = IDN_S(0, 150);    ///< S-0-0150: Reference offset of encoder 1
  constexpr std::uint16_t S_0_0151_REFERENCE_OFFSET_ENCODER_2     = IDN_S(0, 151);    ///< S-0-0151: Reference offset of encoder 2
  constexpr std::uint16_t S_0_0160_SCALING_ACC_TYPE               = IDN_S(0, 160);    ///< S-0-0160: Acceleration data scaling type
  constexpr std::uint16_t S_0_0161_SCALING_ACC_FACTOR             = IDN_S(0, 161);    ///< S-0-0161: Acceleration data scaling factor
  constexpr std::uint16_t S_0_0162_SCALING_ACC_EXPONENT           = IDN_S(0, 162);    ///< S-0-0162: Acceleration data scaling exponent
  constexpr std::uint16_t S_0_0169_PROBE_CTRL_WORD                = IDN_S(0, 169);    ///< S-0-0169: Probe control word
  constexpr std::uint16_t S_0_0170_PROBING_CYCLE_PROCEDURE        = IDN_S(0, 170);    ///< S-0-0170: Probe command procedure
  constexpr std::uint16_t S_0_0191_CANCEL_REF_POINT               = IDN_S(0, 191);    ///< S-0-0191: C1500 command of cancel reference point
  constexpr std::uint16_t S_0_0277_ENCODER_1_PROPERTY             = IDN_S(0, 277);    ///< S-0-0277: Property of Encoder 1
  constexpr std::uint16_t S_0_0284_OP_MODE_SECONDARY4             = IDN_S(0, 284);    ///< S-0-0284: Secondary operation mode 4
  constexpr std::uint16_t S_0_0285_OP_MODE_SECONDARY5             = IDN_S(0, 285);    ///< S-0-0285: Secondary operation mode 5
  constexpr std::uint16_t S_0_0286_OP_MODE_SECONDARY6             = IDN_S(0, 286);    ///< S-0-0286: Secondary operation mode 6
  constexpr std::uint16_t S_0_0287_OP_MODE_SECONDARY7             = IDN_S(0, 287);    ///< S-0-0287: Secondary operation mode 7
  constexpr std::uint16_t S_0_0305_ALLOCATION_OF_REAL_TIME_BIT_1  = IDN_S(0, 305);    ///< S-0-0305: Allocation of real-time status bit 1
  constexpr std::uint16_t S_0_0326_CHECKSUM                       = IDN_S(0, 326);    ///< S-0-0326: Parameter checksum
  constexpr std::uint16_t S_0_0327_CHECKSUM_IDN_LIST              = IDN_S(0, 327);    ///< S-0-0327: IDN list of checksum parameters    
  constexpr std::uint16_t S_0_0328_ASSIGN_LIST_SIGNAL_STATUS_WORD = IDN_S(0, 328);    ///< S-0-0328: Assign list signal status word
  constexpr std::uint16_t S_0_0331_STANDSTILL_STATUS              = IDN_S(0, 331);    ///< S-0-0331: Status "n_feedback = 0"
  constexpr std::uint16_t S_0_0349_BIPOLAR_JERK_LIMIT             = IDN_S(0, 349);    ///< S-0-0349: Bipolar jerk limit
  constexpr std::uint16_t S_0_0372_DRIVE_HALT_ACC_BIPOLAR         = IDN_S(0, 372);    ///< S-0-0372: Drive halt acceleration bipolar
  constexpr std::uint16_t S_0_0390_DIAGNOSTIC_MESSAGE_NUMBER      = IDN_S(0, 390);    ///< S-0-0390: Get Diagnostic message.
  constexpr std::uint16_t S_0_0403_POSITION_FEEDBACK_VALUE_STATUS = IDN_S(0, 403);    ///< S-0-0403: Position feedback value status
  constexpr std::uint16_t S_0_0405_ENABLE_PROBE_1                 = IDN_S(0, 405);    ///< S-0-0405: Enable probe 1
  constexpr std::uint16_t S_0_0406_ENABLE_PROBE_2                 = IDN_S(0, 406);    ///< S-0-0406: Enable probe 2
  constexpr std::uint16_t S_0_0409_PROBE1_POS_LATCH_COUNTER       = IDN_S(0, 409);    ///< S-0-0409: Probe1 positive latched counter
  constexpr std::uint16_t S_0_0410_PROBE1_NEG_LATCH_COUNTER       = IDN_S(0, 410);    ///< S-0-0410: Probe1 negative latched counter
  constexpr std::uint16_t S_0_0411_PROBE2_POS_LATCH_COUNTER       = IDN_S(0, 411);    ///< S-0-0411: Probe2 positive latched counter
  constexpr std::uint16_t S_0_0412_PROBE2_NEG_LATCH_COUNTER       = IDN_S(0, 412);    ///< S-0-0412: Probe2 negative latched counter
  constexpr std::uint16_t S_0_0415_BIT_NUMBER_OF_RT_STATUS_BIT_1  = IDN_S(0, 415);    ///< S-0-0415: Bit number allocation of real-time status bit 1
  constexpr std::uint16_t S_0_0426_SIGNAL_SELECTION_PROBE_1       = IDN_S(0, 426);    ///< S-0-0426: Signal selection probe 1
  constexpr std::uint16_t S_0_0427_SIGNAL_SELECTION_PROBE_2       = IDN_S(0, 427);    ///< S-0-0427: Signal selection probe 2
  constexpr std::uint16_t S_0_0429_EMERGENCY_HALT_DECELERATION    = IDN_S(0, 429);    ///< S-0-0429: Emergency brake deceleration
  constexpr std::uint16_t S_0_0447_SET_ABSOLUTE_POSITION          = IDN_S(0, 447);    ///< S-0-0447: C0300 Set absolute position procedure command
  constexpr std::uint16_t S_0_0448_SET_ABSOLUTE_POSITION_CONTROL  = IDN_S(0, 448);    ///< S-0-0448: Set absolute position control
  constexpr std::uint16_t S_0_0524_POS_EDGE_COMPENSATION_PROBE_1  = IDN_S(0, 524);    ///< S-0-0524: Probe 1 positive edge compensation
  constexpr std::uint16_t S_0_0525_NEG_EDGE_COMPENSATION_PROBE_1  = IDN_S(0, 525);    ///< S-0-0525: Probe 1 negative edge compensation
  constexpr std::uint16_t S_0_0526_POS_EDGE_COMPENSATION_PROBE_2  = IDN_S(0, 526);    ///< S-0-0526: Probe 2 positive edge compensation
  constexpr std::uint16_t S_0_0527_NEG_EDGE_COMPENSATION_PROBE_2  = IDN_S(0, 527);    ///< S-0-0527: Probe 2 negative edge compensation
  constexpr std::uint16_t S_0_0530_CLAMPING_TORQUE                = IDN_S(0, 530);    ///< S-0-0530: Clamping torque torque/force data
  constexpr std::uint16_t S_0_0533_NOMINAL_TORQUE                 = IDN_S(0, 533);    ///< S-0-0533: Nominal torque/force of motor

  /// @brief The follow IDN are general P-parameters
  constexpr std::uint16_t P_0_0115_AXIS_STATUS_WORD               = IDN_P(0, 115);    ///< P-0-0115: Axis status word
  constexpr std::uint16_t P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR = IDN_P(0, 117);    ///< P-0-0117: Multi-brake support
  constexpr std::uint16_t P_0_0119_CONTROL_UNIT_BEST_POSSIBLE_DEC = IDN_P(0, 119);    ///< P-0-0119: Best possible deceleration
  constexpr std::uint16_t P_0_0187_POS_PROCESS_MODE               = IDN_P(0, 187);    ///< P-0-0187: Parameter of position process mode
  constexpr std::uint16_t P_0_0200_EXP_WINDOW_START_PROBE_2       = IDN_P(0, 200);    ///< P-0-0200: Probe 2 extended window start
  constexpr std::uint16_t P_0_0201_EXP_WINDOW_END_PROBE_2         = IDN_P(0, 201);    ///< P-0-0201: Probe 2 extended window end
  constexpr std::uint16_t P_0_0204_EXP_WINDOW_START_PROBE_1       = IDN_P(0, 204);    ///< P-0-0204: Probe 1 extended window start
  constexpr std::uint16_t P_0_0205_EXP_WINDOW_END_PROBE_1         = IDN_P(0, 205);    ///< P-0-0205: Probe 2 extended window end
  constexpr std::uint16_t P_0_0224_PROBE1_MARKER_FAILURE          = IDN_P(0, 224);    ///< P-0-0224: Probe1 marker failure counter
  constexpr std::uint16_t P_0_0225_PROBE2_MARKER_FAILURE          = IDN_P(0, 225);    ///< P-0-0225: Probe2 marker failure counter
  constexpr std::uint16_t P_0_0226_PROBE_EXTENDED_CTRL_WORD       = IDN_P(0, 226);    ///< P-0-0226: Probe extended control word
  constexpr std::uint16_t P_0_2755_SYSTEM_FINE_TIME               = IDN_P(0, 2755);   ///< P-0-2755: System fine time, alias of S-0-1305.0.2
  constexpr std::uint16_t P_0_0117_0_1_REACTION_DELAY_F2_F3       = IDN_P(0, 3892);   ///< P-0-0117.0.1: Control unit reaction time F2/3, alias P-0-3892
  constexpr std::uint16_t P_0_0117_0_2_REACTION_DELAY_F4_F6_F7    = IDN_P(0, 3893);   ///< P-0-0117.0.2: Control unit reaction time F4/6/7, alias P-0-3893

  /// @brief list type parameter container
  constexpr std::array<uint16_t,3> LIST_PARAM {
                                                S_0_0026_CONFIG_LIST_SIGNAL_STATUS_WORD,
                                                S_0_0030_SW_VERSION,
                                                S_0_0328_ASSIGN_LIST_SIGNAL_STATUS_WORD
  };

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief This enum represents all Sercos operation modes.
  enum class OperationMode : std::int16_t
  {
    UNDEFINED = -1, ///< Operation mode not define
    PRIMARY = 0,    ///< Primary operation mode
    SECONDARY1,     ///< Secondary operation mode 1
    SECONDARY2,     ///< Secondary operation mode 2
    SECONDARY3,     ///< Secondary operation mode 3
    SECONDARY4,     ///< Secondary operation mode 4
    SECONDARY5,     ///< Secondary operation mode 5
    SECONDARY6,     ///< Secondary operation mode 6
    SECONDARY7,     ///< Secondary operation mode 7
  };      

  /// @brief This enum represents the list of supported operation modes (S-0-0292) of Sercos.
  enum class OperationModeList : std::uint16_t
  {
    TORQUE_FORCE_CONTROL = 0x0001,                                ///< Torque/force control
    VELOCITY_CONTROL = 0x0002,                                    ///< Velocity control
    POSITION_CONTROL_ENCODER_1 = 0x0003,                          ///< Position control, encoder 1
    POSITION_CONTROL_ENCODER_2 = 0x0004,                          ///< Position control, encoder 2
    POSITION_CONTROL_LAGLESS_ENCODER_1 = 0x000B,                  ///< Position control, lagless, encoder 1
    POSITION_CONTROL_LAGLESS_ENCODER_2 = 0x000C,                  ///< Position control, lagless, encoder 2
    POSITION_CONTROL_DRIVE_CONTROLLED_ENCODER_1 = 0x0103,         ///< Position control drive-controlled, encoder 1
    POSITION_CONTROL_DRIVE_CONTROLLED_ENCODER_2 = 0x0104,         ///< Position control drive-controlled, encoder 2
    POSITION_CONTROL_DRIVE_CONTROLLED_LAGLESS_ENCODER_1 = 0x010B, ///< Position control drive-controlled, lagless, encoder 1
    POSITION_CONTROL_DRIVE_CONTROLLED_LAGLESS_ENCODER_2 = 0x010C, ///< Position control drive-controlled, lagless, encoder 2
    POSITION_CONTROL = 0x0205,                                    ///< Position control
    PISITION_CONTROL_DRIVE_CONTROLLED = 0x0305,                   ///< Position control drive-controlled
    TORQUE_FORCE_CONTROL_VEL_LIMIT = 0x8101,                      ///< Torque/force control with velocity limit
  };

  /// @brief This enum represents all Sercos procedure commands.
  enum class ProcedureCommands : std::uint16_t
  {
    NO_COMMAND_ACTIVE = 0,                                                        ///< This entry indicates that currently no command is execute
    S_0_0099_RESET_CLASS_1_DIAGNOSTIC = S_0_0099_RESET_CLASS_1_DIAGNOSTIC,        ///< C0500 Reset class 1 diagnostic
    S_0_0148_DRIVE_CONTROLLED_HOMING = S_0_0148_DRIVE_CONTROLLED_HOMING,          ///< C0600 Drive-controlled homing procedure command
    S_0_0149_FIXED_STOP_COMMAND = S_0_0149_FIXED_STOP_COMMAND,                    ///< C1300 Positive stop drive procedure command S-0-0149
    S_0_0170_PROBING_CYCLE_PROCEDURE = S_0_0170_PROBING_CYCLE_PROCEDURE,          ///< Probing cycle procedure command S-0-0170
    S_0_0191_CANCEL_REF_POINT = S_0_0191_CANCEL_REF_POINT,                        ///< C1500 command of cancel reference point    
    S_0_0447_SET_ABSOLUTE_POSITION = S_0_0447_SET_ABSOLUTE_POSITION,              ///< C0300 Set absolute position procedure command S-0-0447      
  };

  /// @brief Position process mode of the drive, only at Rexroth drive
  enum class PosProcessMode
  {
    UNKNOWN_MODE = -1,        ///< Unknown mode
    LINEAR_INTERPOLATION = 0, ///< Linear interpolation
    CUBIC_APPORXIMATION = 1,  ///< Cubic approximation
    CUBIC_INTERPOLATION = 2,  ///< Cubic interpolation
  };

  /// @brief Number to enum of PosProcessMode
  /// @param[in]  value  The value to be converted
  /// @return Enum of PosProcessMode
  inline PosProcessMode toPosProcessMode(std::uint16_t value)
  {
    switch (value)
    {
      case 0: return PosProcessMode::LINEAR_INTERPOLATION;
      case 1: return PosProcessMode::CUBIC_APPORXIMATION;
      case 2: return PosProcessMode::CUBIC_INTERPOLATION;
      default: return PosProcessMode::UNKNOWN_MODE;
    }
  }

  /// @brief Get position process buffer length of the drive
  /// @param[in]  mode    Position process mode
  /// @return Buffer length
  inline std::uint16_t getPosProcessBufLen(PosProcessMode mode)
  {
    switch (mode)
    {
      case PosProcessMode::LINEAR_INTERPOLATION:
        return 2;
      case PosProcessMode::CUBIC_APPORXIMATION:
      case PosProcessMode::CUBIC_INTERPOLATION:
        return 3;
      default: return 0;
    }
  }

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This structure represents general unsigned 16bits parameter
  struct GenParamU16
  {
    std::uint16_t m_idn;     ///< Identification number of SoE parameter
    std::uint16_t m_value;   ///< Value read from the device
  };
      
  /// @brief Adapt the corresponding parameters for the current encoder.
  struct OperaterEncoder
  {
    std::uint16_t m_referenceDistance;    ///< parameter of reference distance
    std::uint16_t m_referenceFeedback;    ///< parameter of reference feedback
    std::uint16_t m_referenceOffset;      ///< parameter of reference offset
  };

  /// @brief structure stores information for accessing S parameter
  typedef struct SercosParamStruct
  {
    ReadWriteType m_readWriteType = ReadWriteType::READ;          ///< read or write 
    std::uint16_t m_requestIDN;             ///< IDN of the parameter
    void* m_ptrData;                        ///< pointer to the data        
    size_t dataSize;                        ///< data size
    /// @brief constructor
    SercosParamStruct(std::uint16_t requestIDN, void* ptrData, size_t dataSize)
    : m_requestIDN(requestIDN), m_ptrData(ptrData), dataSize(dataSize)
    {};

    /// @brief constructor with read write type
    SercosParamStruct(ReadWriteType type, std::uint16_t requestIDN, void* ptrData, size_t dataSize)
      : m_readWriteType(type), m_requestIDN(requestIDN), m_ptrData(ptrData), dataSize(dataSize)
    {};
  }SercosParamStruct;

  /// @brief This structure represents the complete sercos controlword S-0-0134 for communication profiles (CP): CP16/1 (SERCOS I), CP16/2 (SERCOS II) and CP12 (EtherCAT).
  struct ControlWord
  {
    bool m_driveOnOff = false;                                ///< Bit 15        | Drive On/Off
    bool m_driveEnable = false;                               ///< Bit 14        | Drive enable (condition: independent of bits 15 and 13)
    bool m_driveHalt = false;                                 ///< Bit 13        | Drive Halt, used to stop or restart the drive
                                                              //   Bit 12        | (Reserved)
    bool m_ipoSync = false;                                   ///< Bit 10        | Control unit synchronization bit
    OperationMode m_operationMode = OperationMode::PRIMARY;   ///< Bits 11, 9, 8 | Selection of operation mode
                                                              //   Bits 7-0      | (Reserved)
  };

  /// @brief This structure represents the complete sercos statusword S-0-0135 for communication profiles (CP): CP16/1 (SERCOS I), CP16/2 (SERCOS II) and CP12 (EtherCAT).
  struct StatusWord
  {
    ReadyToOperate m_readyToOperate = ReadyToOperate::DRIVE_NOT_READY;  ///< Bits 15-14 | Ready to operate
    bool m_driveShutdownErrorInC1D = false;                             ///< Bit 13     | Drive error situation of C1D
    bool m_warningInC2D = false;                                        ///< Bit 12     | When a warning occurs this bit is set to 1
    bool m_changeBitForC3D = false;                                     ///< Bit 11     | When a status message toggles in C3D this bit is set to 1
    OperationMode m_operationMode = OperationMode::PRIMARY;             ///< Bits 10-8  | Current operation mode
    bool m_realTimeStatusBit2 = false;                                  ///< Bit 7      | Real-time status bit 2 (See S-0-0306)
    bool m_realTimeStatusBit1 = false;                                  ///< Bit 6      | Real-time status bit 1 (See S-0-0304)
    bool m_procedureCmdChangeBit = false;                               ///< Bit 5      | Procedure command change bit
    bool m_statusParameterizationLvl = false;                           ///< Bit 4      | Status Parametrization level (PL)
    bool m_statusCommandValueProcessing = false;                        ///< Bit 3      | Status of command value processing
    bool m_errorInServiceChannel = false;                               ///< Bit 2      | Error in service channel
    bool m_busy = false;                                                ///< Bit 1      | Step finished or in progress
    bool m_serviceHandshake = false;                                    ///< Bit 0      | Service transport handshake of the slave
  };

  /// @brief This structure contain operation mode available information for Sercos
  typedef struct OpModeAvailInfoSercos : public OpModeAvailInfo
  {
    /// @brief Constructor 
    OpModeAvailInfoSercos(bool positionAvail, bool velocityAvail, bool torqueForceAvail): OpModeAvailInfo(positionAvail, velocityAvail, torqueForceAvail)
    {};

    /// @brief Constructor using a single value
    explicit OpModeAvailInfoSercos(bool avail): OpModeAvailInfo(avail)
    {};

    /// @brief Clear Sercos operation mode available information
    void clearOpModeAvailInfoSercos()
    {
      this->clearOpModeAvailInfo();
      m_sercosOpModeTable.clear();
    }

    /// @brief Set Sercos operation mode available information
    /// @param[in]   sercosOpMode             Sercos operation mode
    /// @param[in]   profileOpMode            Profile operation mode
    void setOpModeAvailInfoSercos(sercos::OperationMode sercosOpMode, AxisProfileOperationMode profileOpMode)
    {
      auto it = m_sercosOpModeTable.find(sercosOpMode);
      if (it != m_sercosOpModeTable.end())
      {
        it->second = profileOpMode;
      }
      else
      {
        m_sercosOpModeTable.emplace(sercosOpMode, profileOpMode);
      }
    }

    /// @brief Get Sercos operation mode
    /// @param[in]    profileOpMode            Profile operation mode
    /// @return Sercos operation mode
    OperationMode getSercosOpMode(AxisProfileOperationMode profileOpMode)
    {
      for (const auto& opMode : m_sercosOpModeTable)
      {
        if (profileOpMode == opMode.second)
        {
          return opMode.first;
        }
      }
      return OperationMode::UNDEFINED;
    }

    /// @brief Get Profile operation mode
    /// @param[in]    sercosOpMode             Sercos operation mode
    /// @return Profile operation mode
    AxisProfileOperationMode getProfileOpMode(OperationMode sercosOpMode)
    {
      auto it = m_sercosOpModeTable.find(sercosOpMode);
      return (it != m_sercosOpModeTable.end()) ? it->second : AxisProfileOperationMode::UNDEFINED;
    }

  private:

    ///< A lookup table between sercos operation mode(primary, secondary1, secondary2 ...) and profile operation modes(position, velocity, torque ...)
    std::map<OperationMode, AxisProfileOperationMode> m_sercosOpModeTable{};
  }OpModeAvailInfoSercos;
}
