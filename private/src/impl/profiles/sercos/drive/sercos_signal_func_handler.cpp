/// @file
///
/// @brief This file is part of the axisprofile library and implements the sercos signal function handler class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "diagnosis/dia.h"
#include "fieldbuses/fieldbus_helper.h"
#include "profiles/category/drive/axisprofile_device_param_drive.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "profiles/i_device_boot_param_snapshot.h"
#include "profiles/sercos/sercos_boot_param_snapshot.h"
#include "profiles/sercos/sercos_defs.h"
#include "sercos_signal_func_handler.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

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
SercosSignalFuncHandler::SercosSignalFuncHandler(ProfileBasicInfo& profileBasicInfo)
  :SignalFuncHandler(profileBasicInfo)
{
  // Register function of homing status checking with cyclic data.
  registerSignalStatusWordFuncs(FunctionSupport::HOMING_STATUS_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // Homing status is supported when IDN S-0-0403.0 is configured in the signal status word.
                                  return param == S_0_0403_POSITION_FEEDBACK_VALUE_STATUS && bit == 0;
                                });

  registerSignalControlWordFuncs(FunctionSupport::CLEAR_DEVERR_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                 {
                                   // Homing command is supported when IDN S-0-0099.0 is configured in the signal control word.
                                   return param == S_0_0099_RESET_CLASS_1_DIAGNOSTIC && bit == 0;
                                 });
  registerSignalStatusWordFuncs(FunctionSupport::CLEAR_DEVERR_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // Homing command is supported when IDN S-0-0099.0 is configured in the signal status word.
                                  return param == S_0_0099_RESET_CLASS_1_DIAGNOSTIC && bit == 0;
                                });

  registerSignalControlWordFuncs(FunctionSupport::SET_ABS_POS_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                 {
                                   // Set absolute position command is supported when IDN S-0-0447.0 is configured in the signal control word.
                                   return param == S_0_0447_SET_ABSOLUTE_POSITION && bit == 0;
                                 });
  registerSignalStatusWordFuncs(FunctionSupport::SET_ABS_POS_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // Set absolute position command is supported when IDN S-0-0447.0 is configured in the signal status word.
                                  return param == S_0_0447_SET_ABSOLUTE_POSITION && bit == 0;
                                });

  registerSignalControlWordFuncs(FunctionSupport::HOMING_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                 {
                                   // Homing command is supported when IDN S-0-0148.0 is configured in the signal control word.
                                   return param == S_0_0148_DRIVE_CONTROLLED_HOMING && bit == 0;
                                 });

  registerSignalStatusWordFuncs(FunctionSupport::HOMING_CMD_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                 {
                                   // Homing command is supported when IDN S-0-0148.0 is configured in the signal status word.
                                   return param == S_0_0148_DRIVE_CONTROLLED_HOMING && bit == 0;
                                 });
    
  registerSignalControlWordFuncs(FunctionSupport::PROBE1_ENABLE_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                 {
                                   // probe enable is supported when IDN S-0-0405.0 is configured in the signal control word.
                                   return param == S_0_0405_ENABLE_PROBE_1 && bit == 0;
                                 });

  registerSignalStatusWordFuncs(FunctionSupport::PROBE1_ENABLE_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // probe enable is supported when IDN S-0-0405.0 is configured in the signal status word.
                                  return param == S_0_0405_ENABLE_PROBE_1 && bit == 0;
                                });

  registerSignalControlWordFuncs(FunctionSupport::PROBE2_ENABLE_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                 {
                                   // probe enable is supported when IDN S-0-0406.0 is configured in the signal control word.
                                   return param == S_0_0406_ENABLE_PROBE_2 && bit == 0;
                                 });

  registerSignalStatusWordFuncs(FunctionSupport::PROBE2_ENABLE_CYCLIC, [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
                                {
                                  // probe enable is supported when IDN S-0-0406.0 is configured in the signal status word.
                                  return param == S_0_0406_ENABLE_PROBE_2 && bit == 0;
                                });
}

AxisProfileResult SercosSignalFuncHandler::readDriveSignalStatusWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits)
{
  params.clear();
  assignBits.clear();

  // S-0-0016: Configuration list of AT
  std::vector<std::uint16_t> tmpParams {};
  auto res = readParamListSoE(S_0_0016_CONFIG_LIST_AT, tmpParams);
  if (aprStatusFailed(res))
  {
    return res;
  }

  // if S-0-0144 not in AT list, then no need check more
  if (std::ranges::find(tmpParams, S_0_0144_SIGNAL_STATUS_WORD) == tmpParams.end())
  {
    return AxisProfileResult::STS_OK;
  }

  // S-0-0026: Configuration list for signal status word
  res = readParamListSoE(S_0_0026_CONFIG_LIST_SIGNAL_STATUS_WORD, tmpParams);
  if (aprStatusFailed(res))
  {
    return res;
  }
  params.assign(tmpParams.begin(), tmpParams.end());

  // S-0-0328: assign list signal status word
  res = readParamListSoE(S_0_0328_ASSIGN_LIST_SIGNAL_STATUS_WORD, tmpParams);
  if (aprStatusFailed(res))
  {
    return res;
  }
  assignBits.assign(tmpParams.begin(), tmpParams.end());
  return res;
}

AxisProfileResult SercosSignalFuncHandler::readDriveSignalControlWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits)
{
  params.clear();
  assignBits.clear();

  // S-0-0024: Configuration list of MDT
  std::vector<std::uint16_t> tmpParams {};
  auto res = readParamListSoE(S_0_0024_CONFIG_LIST_MDT, tmpParams);
  if (aprStatusFailed(res))
  {
    return res;
  }

  // if S-0-0145 not in MDT list, then no need check more
  if (std::ranges::find(tmpParams, S_0_0145_SIGNAL_CONTROL_WORD) == tmpParams.end())
  {
    return AxisProfileResult::STS_OK;
  }

  // S-0-0027: Configuration list for signal control word
  res = readParamListSoE(S_0_0027_CONFIG_LIST_SIGNAL_CONTROL_WORD, tmpParams);
  if (aprStatusFailed(res))
  {
    return res;
  }
  params.assign(tmpParams.begin(), tmpParams.end());

  // S-0-0329: assign list signal control word
  res = readParamListSoE(S_0_0329_ASSIGN_LIST_SIGNAL_CONTROL_WORD, tmpParams);
  if (aprStatusFailed(res))
  {
    return res;
  }
  assignBits.assign(tmpParams.begin(), tmpParams.end());
  return res;
}

AxisProfileResult SercosSignalFuncHandler::readParamListSoE(std::uint16_t idn, std::vector<std::uint16_t>& outList)
{
  outList.clear();
  std::array<std::uint8_t, LIST_BUFF_DEFAULT_SIZE> rawBuf {};

  APR_TRACE_MESSAGE("%s; sync reading IDN=%s, max length=%d.",
                    m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                    idnToString(idn).c_str(),
                    rawBuf.size());

  AxisProfileResult res = getParameterValueFromSnapshot(idn, rawBuf.data(), rawBuf.size());
  if (aprStatusFailed(res))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_READ_PARAM_FAILED,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Signal status, SoE IDN: %s, error: 0x%08X",
                       idnToString(idn).c_str(), res);
    return res;
  }

  res = FieldbusHelper::rawToParamListSoE(rawBuf.data(), rawBuf.size(), outList);
  if (aprStatusFailed(res))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_PARSE_PARAM_FAILED,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Signal status parse, SoE IDN: %s, error: 0x%08X",
                       idnToString(idn).c_str(), res);
  }

  return res;
}

AxisProfileResult SercosSignalFuncHandler::getParameterValueFromSnapshot(std::uint16_t idn, std::uint8_t* bufPtr, std::size_t bufLen)
{  
  return SercosBootParamSnapshot::getParameterValueFromSnapshot(m_sharingHandler, m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                                                                idn, bufPtr, bufLen);
}