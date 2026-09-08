/// @file
///
/// This source file is part of the axisprofile library and implements canopen probe parameters.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_probe_drive.h"
#include "diagnosis/dia.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::canopen;
using namespace comm::axisprofile;

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

CanopenProbeDrive::CanopenProbeDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at)
  :AxisProfileProbeDrive(profileBasicInfo), m_telegramAT(at)
{
  m_probeCmds.clear();
  setProbeCommands();
}

void CanopenProbeDrive::loadBootingTasks()
{
  m_initCommands.clear();
  m_initCommands.reserve(1);
  m_initCommands.emplace_back([this]{return initAsyncAccessHandler();});
}

AxisProfileResult CanopenProbeDrive::initAsyncAccessHandler()
{
  if (!m_sharingHandler)
  {
    // LCOV_EXCL_START
    return AxisProfileResult::ERR_EXEC_NULL;
    // LCOV_EXCL_STOP
  }

  m_paramAccess = static_cast<CanopenParamAccess*>(m_sharingHandler->getParamAccessIF());
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenProbeDrive::probeBaseConfig(ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool /*isAutoActivated*/)
{
  AsyncCommandStatemachine* asyncState {nullptr};
  AxisProfileResult result = raiseAndGetStatemachine(probeIdx, asyncState);
  if (aprStatusNotOkay(result))
  {
    return result;
  }

  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    asyncState->switchToCmdStartStep(); // nothing to be init, so switch to command start step
  }
  result = executeCtrlWordConfig(probeIdx, probeConfig, *asyncState);

  return resultHandlingCycle(*asyncState, result);
}

AxisProfileResult CanopenProbeDrive::probeExtConfig(ProbeIndex /*probeIdx*/, ProbeExtConfig /*probeExtendConfig*/, bool /*isStateMonitActivated*/)
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenProbeDrive::probeStatusCtrl(ProbeIndex probeIdx, bool enable)
{
  AsyncCommandStatemachine* asyncState {nullptr};
  AxisProfileResult result = raiseAndGetStatemachine(probeIdx, asyncState);
  if (aprStatusNotOkay(result))
  {
    return result;
  }

  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    m_probeEable.clear();

    bool preconditionFullFilled = itemListChecker(probeIdx, probeIdx == ProbeIndex::PROBE_1 ? m_probe1CheckList : m_probe2CheckList);
    if (!preconditionFullFilled)
    {
      // diagnosis has reported in checker
      return AxisProfileResult::ERR_PROFILE_CONFIG;
    }

    // check probe status
    m_probeEable.emplace_back(CanopenParamStruct(ReadWriteType::READ, PROBE_CTRL_WORD.index, PROBE_CTRL_WORD.subIndex,
                                                 &m_probeBitProcess.m_ctrlWord.m_word, sizeof(m_probeBitProcess.m_ctrlWord.m_word)));

    asyncState->switchToNextStep();
  }

  result = executeProbeStatusCtrl(probeIdx, enable, *asyncState);
  return resultHandlingCycle(*asyncState, result);
}

AxisProfileResult CanopenProbeDrive::getProbeActualValuesRT(ProbeIndex probeIdx, ProbeActualValues* actualValueProbe)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  auto scaling = m_sharingHandler->getScalingIF();
  if (probeIdx == ProbeIndex::PROBE_1)
  {
    actualValueProbe->m_probeLatchedPosEdge = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe1.posEdg);
    actualValueProbe->m_probeLatchedNegEdge = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe1.negEdg);
    actualValueProbe->m_posEdgeLatchedTimes = m_telegramAT->m_probe.m_probe1.posLatched;
    actualValueProbe->m_negEdgeLatchedTimes = m_telegramAT->m_probe.m_probe1.negLatched;
  }
  else if (probeIdx == ProbeIndex::PROBE_2)
  {
    actualValueProbe->m_probeLatchedPosEdge = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe2.posEdg);
    actualValueProbe->m_probeLatchedNegEdge = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe2.negEdg);
    actualValueProbe->m_posEdgeLatchedTimes = m_telegramAT->m_probe.m_probe2.posLatched;
    actualValueProbe->m_negEdgeLatchedTimes = m_telegramAT->m_probe.m_probe2.negLatched;
  }
  else
  {
    Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST, __FILE__, __FUNCTION__, __LINE__, "Invalid probe number.");
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenProbeDrive::onlineChangeParams(ProbeIndex /*probeIdx*/, ProbeUpdateMeasurement /*updateValue*/)
{
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenProbeDrive::getProbeValues(ProbeIndex probeIdx, ProbeBaseConfig* probeConfig, ProbeExtConfig* /*probeExtendConfig*/)
{
  AsyncCommandStatemachine* asyncState {nullptr};
  AxisProfileResult result = raiseAndGetStatemachine(probeIdx, asyncState);
  if (aprStatusNotOkay(result))
  {
    return result;
  }
  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    m_probeActualValues.clear();
    m_probeActualValues.emplace_back(CanopenParamStruct(ReadWriteType::READ,
                                                        PROBE_CTRL_WORD.index, PROBE_CTRL_WORD.subIndex,
                                                        &m_probeBitProcess.m_ctrlWord.m_word, sizeof(m_probeBitProcess.m_ctrlWord.m_word)));

    auto currentProbe = m_probeCmds.find(probeIdx);
    if (currentProbe == m_probeCmds.end())
    {
      Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST, __FILE__, __FUNCTION__, __LINE__, "Invalid probe number.");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    asyncState->switchToNextStep();
  }
  result = probeValueProcess(probeIdx, *asyncState);
  if (aprStatusOkay(result))
  {
    *probeConfig = m_probeParam.m_probeConfig;
  }

  return resultHandlingCycle(*asyncState, result);
}

AxisProfileResult CanopenProbeDrive::abortAndResetProbe()
{
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenProbeDrive::isProbeSupported()
{
  return AxisProfileResult::STS_OK;
}
//////////////////////////////////////////////////  PRIVATE  METHODS  //////////////////////////////////////////////////
AxisProfileResult CanopenProbeDrive::executeCtrlWordConfig(ProbeIndex probeIdx, ProbeBaseConfig  probeConfig, AsyncCommandStatemachine& asyncState)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  switch (asyncState.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS:
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      result = m_paramAccess->readAndCheckParamAsync(PROBE_CTRL_WORD.index, PROBE_CTRL_WORD.subIndex,
                                                     static_cast<void*>(&m_probeBitProcess.m_ctrlWord.m_word),
                                                     sizeof(m_probeBitProcess.m_ctrlWord.m_word), asyncState.getRequestState());
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET: // set control word configuration to probes
    {
      if (probeIdx == ProbeIndex::PROBE_1)
      {
        bool prb1LatTimSpt = m_sharingHandler->getDeviceParamIF()->isLatchParamSupport();
        m_probeBitProcess.m_ctrlWord.m_bits.bit1 = static_cast<std::uint16_t>(probeConfig.m_measureType);

        m_probeBitProcess.m_ctrlWord.m_bits.bit2 = 0;
        m_probeBitProcess.m_ctrlWord.m_bits.bit3 = 0;

        m_probeBitProcess.m_ctrlWord.m_bits.bit4 = static_cast<std::uint16_t>(probeConfig.m_evaluatePosEdge);
        if (prb1LatTimSpt) // Device support probe latched time or not?
        {
          m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_POS_EDGE, probeConfig.m_evaluatePosEdge,
                                         PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe1PosLat,
                                         objDirToString(PROBE1_POS_LATCHED_TIMES));
        }
        m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_POS_EDGE, probeConfig.m_evaluatePosEdge,
                                       PRECONDITION_EDGE_VALUE_MAPPED, profileAssignedInfo.m_input.m_probe1PosEdg,
                                       objDirToString(PROBE1_POS_DEGE_VALUE));

        m_probeBitProcess.m_ctrlWord.m_bits.bit5 = static_cast<std::uint16_t>(probeConfig.m_evaluateNegEdge);
        if (prb1LatTimSpt)
        {
          m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_NEG_EDGE, probeConfig.m_evaluateNegEdge,
                                         PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe1NegLat,
                                         objDirToString(PROBE1_NEG_LATCHED_TIMES));
        }
        m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_NEG_EDGE, probeConfig.m_evaluateNegEdge,
                                       PRECONDITION_EDGE_VALUE_MAPPED, profileAssignedInfo.m_input.m_probe1NegEdg,
                                       objDirToString(PROBE1_NEG_DEGE_VALUE));
      }
      else if (probeIdx == ProbeIndex::PROBE_2)
      {
        bool prb2LatTimSpt = m_sharingHandler->getDeviceParamIF()->isLatchParamSupport();
        m_probeBitProcess.m_ctrlWord.m_bits.bit9 = static_cast<std::uint16_t>(probeConfig.m_measureType);

        m_probeBitProcess.m_ctrlWord.m_bits.bit10 = 0;
        m_probeBitProcess.m_ctrlWord.m_bits.bit11 = 0;

        m_probeBitProcess.m_ctrlWord.m_bits.bit12 = static_cast<std::uint16_t>(probeConfig.m_evaluatePosEdge);
        if (prb2LatTimSpt) // Device support probe latched time or not?
        {
          m_probe2CheckList.emplace_back(CHEKC_ITEM_EVL_POS_EDGE, probeConfig.m_evaluatePosEdge,
                                         PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe2PosLat,
                                         objDirToString(PROBE2_POS_LATCHED_TIMES));
        }
        m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_POS_EDGE, probeConfig.m_evaluatePosEdge,
                                       PRECONDITION_EDGE_VALUE_MAPPED, profileAssignedInfo.m_input.m_probe2PosEdg,
                                       objDirToString(PROBE2_POS_DEGE_VALUE));

        m_probeBitProcess.m_ctrlWord.m_bits.bit13 = static_cast<std::uint16_t>(probeConfig.m_evaluateNegEdge);
        if (prb2LatTimSpt)
        {
          m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_NEG_EDGE, probeConfig.m_evaluateNegEdge,
                                         PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe2NegLat,
                                         objDirToString(PROBE2_NEG_LATCHED_TIMES));
        }
        m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_NEG_EDGE, probeConfig.m_evaluateNegEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe2NegEdg,
                                       objDirToString(PROBE2_NEG_DEGE_VALUE));
      }
      else
      {
        APR_TRACE_ERROR("invalid probe of set probe control word configuration, please check if selected probe is available.");
        Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST, __FILE__, __FUNCTION__, __LINE__, "Invalid probe number.");
        return AxisProfileResult::ERR_INVALID_ARGUMENT;
      }

      asyncState.switchToCertainStep(AsyncCommandStatemachine::CommandStep::CMD_DONE);
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      result = m_paramAccess->writeAndCheckParamAsync(PROBE_CTRL_WORD.index, PROBE_CTRL_WORD.subIndex,
                                                      static_cast<void*>(&m_probeBitProcess.m_ctrlWord.m_word),
                                                      sizeof(m_probeBitProcess.m_ctrlWord.m_word), asyncState.getRequestState());
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToCertainStep(AsyncCommandStatemachine::CommandStep::PARAM_POST);
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }

    case AsyncCommandStatemachine::CommandStep::CMD_CLEAR:
    case AsyncCommandStatemachine::CommandStep::PARAM_POST:
    {
      result = AxisProfileResult::STS_OK;
      break;
    }

    default: // error reaction
    {
      // LCOV_EXCL_START
      APR_TRACE_ERROR("invalid state to execute executeCtrlWordConfig.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
      // LCOV_EXCL_STOP
    }
  }
  return result;
}

AxisProfileResult CanopenProbeDrive::executeProbeStatusCtrl(ProbeIndex probeIdx, bool enable, AsyncCommandStatemachine& asyncState)
{
  AxisProfileResult result {AxisProfileResult::STS_OK};
  switch (asyncState.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS:
    {
      result = m_paramAccess->accessAndCheckParamListAsync(m_probeEable, asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        m_probeParam.m_probe1Enable = m_probeBitProcess.m_ctrlWord.m_bits.bit0;
        m_probeParam.m_probe2Enable = m_probeBitProcess.m_ctrlWord.m_bits.bit8;
        asyncState.switchToCmdStartStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      if (probeIdx == ProbeIndex::PROBE_1)
      {
        m_probeBitProcess.m_ctrlWord.m_bits.bit0 = enable ? 1 : 0;
      }
      if (probeIdx == ProbeIndex::PROBE_2)
      {
        m_probeBitProcess.m_ctrlWord.m_bits.bit8 = enable ? 1 : 0;
      }
      m_probeParam.m_cmdEnableValue = m_probeBitProcess.m_ctrlWord.m_word;

        result = m_paramAccess->writeAndCheckParamAsync(PROBE_CTRL_WORD.index, PROBE_CTRL_WORD.subIndex,
                                                        static_cast<void*>(&m_probeParam.m_cmdEnableValue),
                                                        sizeof(m_probeParam.m_cmdEnableValue), asyncState.getRequestState());
      if (aprStatusOkay(result))
      {
        asyncState.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      auto currentProbe = m_probeCmds.find(probeIdx);
      if (currentProbe != m_probeCmds.end())
      {
        m_probeParam.m_probeStatusCtrl = m_probeParam.m_cmdEnableValue;
        result = m_paramAccess->writeAndCheckParamAsync(static_cast<std::uint16_t>(currentProbe->second.m_probeStatusCtrl), std::uint8_t(0),
                                                        static_cast<void*>(&m_probeParam.m_probeStatusCtrl),
                                                        sizeof(m_probeParam.m_probeStatusCtrl), asyncState.getRequestState());
      }
      else
      {
        Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST, __FILE__, __FUNCTION__, __LINE__, "Invalid probe number.");
        result = AxisProfileResult::ERR_INVALID_ARGUMENT;
      }
      if (aprStatusOkay(result))
      {
        asyncState.switchToCertainStep(AsyncCommandStatemachine::CommandStep::PARAM_POST);
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    case AsyncCommandStatemachine::CommandStep::CMD_CLEAR:
    case AsyncCommandStatemachine::CommandStep::PARAM_POST:
    {
      result = AxisProfileResult::STS_OK;
      break;
    }

    // LCOV_EXCL_START 
    default: // error reaction
    {
      APR_TRACE_ERROR("invalid state to execute executeExtendedCtrlWordConfig.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
    // LCOV_EXCL_STOP
  }
  return result;
}

AxisProfileResult CanopenProbeDrive::probeValueProcess(ProbeIndex probeIdx, AsyncCommandStatemachine& asyncState)
{
  AxisProfileResult result = m_paramAccess->accessAndCheckParamListAsync(m_probeActualValues, asyncState);
  if (aprStatusOkay(result))
  {
    if (probeIdx == ProbeIndex::PROBE_1)
    {
      m_probeParam.m_probeConfig.m_measureType = m_probeBitProcess.m_ctrlWord.m_bits.bit1;
      m_probeParam.m_probeConfig.m_evaluatePosEdge = m_probeBitProcess.m_ctrlWord.m_bits.bit4;
      m_probeParam.m_probeConfig.m_evaluateNegEdge = m_probeBitProcess.m_ctrlWord.m_bits.bit5;
    }
    else
    {
      m_probeParam.m_probeConfig.m_measureType = m_probeBitProcess.m_ctrlWord.m_bits.bit9;
      m_probeParam.m_probeConfig.m_evaluatePosEdge = m_probeBitProcess.m_ctrlWord.m_bits.bit12;
      m_probeParam.m_probeConfig.m_evaluateNegEdge = m_probeBitProcess.m_ctrlWord.m_bits.bit13;
    }
  }
  return result;
}

void CanopenProbeDrive::setProbeCommands()
{
  ExecuteProbe probe_1;
  probe_1.m_probeStatusCtrl = PROBE_CTRL_WORD.index;
  m_probeCmds.emplace(std::make_pair(ProbeIndex::PROBE_1, probe_1));

  ExecuteProbe probe_2;
  probe_2.m_probeStatusCtrl = PROBE_CTRL_WORD.index;
  m_probeCmds.emplace(std::make_pair(ProbeIndex::PROBE_2, probe_2));
}
