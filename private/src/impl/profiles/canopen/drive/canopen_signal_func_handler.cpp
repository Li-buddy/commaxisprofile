/// @file
///
/// @brief This file is part of the axisprofile library and implements the canopen signal function handler class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_signal_func_handler.h"
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "fieldbuses/fieldbus_helper.h"
#include "profiles/canopen/canopen_defs.h"
#include "diagnosis/dia.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
CanopenSignalFuncHandler::CanopenSignalFuncHandler(ProfileBasicInfo& profileBasicInfo)
  :SignalFuncHandler(profileBasicInfo)
{
  // Register function of homing status checking with cyclic data.
  registerSignalStatusWordFuncs(FunctionSupport::HOMING_STATUS_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // Homing status is supported when this object is configured in the signal status word.
                                  return static_cast<std::uint16_t>(param >> 16) == HOMING_FEEDBACK_VALUE_STATUS.index
                                    && static_cast<std::uint8_t>(param >> 8) == HOMING_FEEDBACK_VALUE_STATUS.subIndex && bit == 0;
                                });

  // Register function of error reset checking with cyclic data.
  registerSignalStatusWordFuncs(FunctionSupport::CLEAR_DEVERR_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // Reset command is supported when 0x2B87:02 is configured in the signal status word.
                                  return static_cast<std::uint16_t>(param >> 16) == RESET_CLASS_1_DIAGNOSTIC.index
                                    && static_cast<std::uint8_t>(param >> 8) == RESET_CLASS_1_DIAGNOSTIC.subIndex && bit == 0;
                                });
  // Register function of set absolute position checking with cyclic data.
  registerSignalControlWordFuncs(FunctionSupport::SET_ABS_POS_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // Reset command is supported when 0x2510:01 is configured in the signal control word.
                                  return static_cast<std::uint16_t>(param >> 16) == SET_ABSOLUTE_POSITION_CMD.index
                                    && static_cast<std::uint8_t>(param >> 8) == SET_ABSOLUTE_POSITION_CMD.subIndex && bit == 0;
                                });
  // Register function of set absolute position checking with cyclic data.
  registerSignalStatusWordFuncs(FunctionSupport::SET_ABS_POS_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // Reset command is supported when 0x2510:01 is configured in the signal status word.
                                  return static_cast<std::uint16_t>(param >> 16) == SET_ABSOLUTE_POSITION_CMD.index
                                    && static_cast<std::uint8_t>(param >> 8) == SET_ABSOLUTE_POSITION_CMD.subIndex && bit == 0;
                                });

  registerSignalControlWordFuncs(FunctionSupport::HOMING_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                 {
                                   // Homing command is supported when 0x2510:01 is configured in the signal control word.
                                   return static_cast<std::uint16_t>(param >> 16) == HOMING_DRIVE_CONTROLLED_CMD.index
                                     && static_cast<std::uint8_t>(param >> 8) == HOMING_DRIVE_CONTROLLED_CMD.subIndex && bit == 0;
                                 });

  registerSignalStatusWordFuncs(FunctionSupport::HOMING_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // Homing command is supported when 0x2510:01 is configured in the signal status word.
                                  return static_cast<std::uint16_t>(param >> 16) == HOMING_DRIVE_CONTROLLED_CMD.index
                                    && static_cast<std::uint8_t>(param >> 8) == HOMING_DRIVE_CONTROLLED_CMD.subIndex && bit == 0;
                                });

}

AxisProfileResult CanopenSignalFuncHandler::readDriveSignalStatusWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits)
{
  // Signal status word: Configuration list (0x2C82:02 / S-0-0026), Data length 4 bytes var
  // Maximum of 16 parameters can be configured.
  // The lists in 0x2C82:02 and 0x2C82:03 have the same length
  params.clear();
  assignBits.clear();

  constexpr std::size_t maxCfgParams = 16;
  auto signalControlObj = getObjDir(SIGNAL_STATUS_WORD_CONFIG_LIST, m_profileBasicInfo.m_driveNumber);
  auto res = readParamListCoE(signalControlObj, maxCfgParams, params);
  if (aprStatusFailed(res))
  {
    return res;
  }
  auto signalControlBitObj = getObjDir(SIGNAL_STATUS_WORD_ASSIGN_LIST, m_profileBasicInfo.m_driveNumber);
  return readParamListCoE(signalControlBitObj, maxCfgParams, assignBits);
}

AxisProfileResult CanopenSignalFuncHandler::readDriveSignalControlWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits)
{
  // Signal control word: Configuration list (0x2D24:05 / S-0-0024), Data length 4 bytes var
  // Maximum of 16 parameters can be configured.
  // The lists in 0x2C82:02 and 0x2C80:03 have the same length
  params.clear();
  assignBits.clear();

  constexpr std::size_t maxCfgParams = 16;
  auto signalControlObj = getObjDir(SIGNAL_CONTROL_WORD_CONFIG_LIST, m_profileBasicInfo.m_driveNumber);
  auto res = readParamListCoE(signalControlObj, maxCfgParams, params);
  if (aprStatusFailed(res))
  {
    return res;
  }
  auto signalControlBitObj = getObjDir(SIGNAL_CONTROL_WORD_ASSIGN_LIST, m_profileBasicInfo.m_driveNumber);
  return readParamListCoE(signalControlBitObj, maxCfgParams, assignBits);
}

AxisProfileResult CanopenSignalFuncHandler::readParamListCoE(const ObjectDirEntry& obj,
                                                             std::size_t maxElements,
                                                             std::vector<std::uint32_t>& outList)
{
  return FieldbusHelper::readParamListCoEImpl(m_profileBasicInfo, obj, maxElements, outList);
}

AxisProfileResult CanopenSignalFuncHandler::readParamListCoE(const ObjectDirEntry& obj,
                                                             std::size_t maxElements,
                                                             std::vector<std::uint16_t>& outList)
{
  return FieldbusHelper::readParamListCoEImpl(m_profileBasicInfo, obj, maxElements, outList);
}
