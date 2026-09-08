/// @file
///
/// This source file is part of the axisprofile library and implements sercos probe parameters.
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
#include "diagnosis/dia.h"
#include "profiles/sercos/sercos_param_access.h"
#include "sercos_probe_drive.h"

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

SercosProbeDrive::SercosProbeDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at)
  :AxisProfileProbeDrive(profileBasicInfo), m_telegramAT(at)
{
  m_probeCmds.clear();
  setProbeCommands();
}

void SercosProbeDrive::loadBootingTasks()
{
  m_initCommands.clear();
  m_initCommands.reserve(1);
  m_initCommands.emplace_back([this]{return initAsyncAccessHandler();});
}

AxisProfileResult SercosProbeDrive::initAsyncAccessHandler()
{
  if (!m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_paramAccess = static_cast<SercosParamAccess*>(m_sharingHandler->getParamAccessIF());
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosProbeDrive::isProbeSupported()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosProbeDrive::probeBaseConfig(ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool isAutoActivated)
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
  result = executeCtrlWordConfig(probeIdx, probeConfig, isAutoActivated, *asyncState);

  return resultHandlingCycle(*asyncState, result);
}

AxisProfileResult SercosProbeDrive::probeExtConfig(ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig, bool isStateMonitActivated)
{
  if (probeIdx == ProbeIndex::PROBE_UNDEFINED)
  {
    APR_TRACE_ERROR("Invalid probe index: %d", probeIdx);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  AsyncCommandStatemachine* asyncState{ nullptr };
  AxisProfileResult result = raiseAndGetStatemachine(probeIdx, asyncState);
  if (aprStatusNotOkay(result))
  {
    return result;
  }

  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
    {
      return AxisProfileResult::ERR_EXEC_NULL;
    }
    // expected window unit convert
    m_probeParam.m_expPosStart = m_sharingHandler->getScalingIF()->convertPositionScaledToRaw(probeExtendConfig.m_expectWindowPosStart);
    m_probeParam.m_expPosEnd = m_sharingHandler->getScalingIF()->convertPositionScaledToRaw(probeExtendConfig.m_expectWindowPosEnd);

    // dead time compensation unit convert
    m_probeParam.m_posEdgeCompensation = timeUnitAdapteDevice(probeExtendConfig.m_timeCompensationPosEdge);
    m_probeParam.m_negEdgeCompensation = timeUnitAdapteDevice(probeExtendConfig.m_timeCompensationNegEdge);
    
    if (m_probeParam.m_posEdgeCompensation > PROBE_COMPENSATION_LIM || m_probeParam.m_negEdgeCompensation > PROBE_COMPENSATION_LIM)
    {
      Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_PROBE_TIME_MEASURE_EXCEED, __FILE__, __FUNCTION__, __LINE__, 
                        "Dead Time Compensation exceed limit %g", PROBE_COMPENSATION_LIM);
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    // convert measured signal
    switch (probeExtendConfig.m_measuredSignal)
    {
      case MeasuringSignalSel::POS_FEEDBACK_VAL_ENCODER_1:
      {
        m_probeParam.m_selectedSignal = S_0_0051_POSITION_FEEDBACK_ENCODER_1;
        break;
      }
      case MeasuringSignalSel::POS_FEEDBACK_VAL_ENCODER_2:
      {
        m_probeParam.m_selectedSignal = S_0_0053_POSITION_FEEDBACK_ENCODER_2;
        break;
      }
      case MeasuringSignalSel::SYSTEM_FINE_TIME:
      {
        m_probeParam.m_selectedSignal = P_0_2755_SYSTEM_FINE_TIME;
        break;
      }
      case MeasuringSignalSel::NO_COMMAND_ACTIVE:
      {
        m_probeParam.m_selectedSignal = S_0_0000_DUMMY;
        break;
      }
      default:
      {
        APR_TRACE_ERROR("The input measured signal is undefined");
        return AxisProfileResult::ERR_INVALID_ARGUMENT;
      }
    }

    if (probeIdx == ProbeIndex::PROBE_1)
    {
      m_prb1MeasureSignal = m_probeParam.m_selectedSignal;
    }
    else 
    {
      m_prb2MeasureSignal = m_probeParam.m_selectedSignal;
    }

    m_paramListExtendedCtrlWord.clear();

    // read probe extended control word.  
    m_paramListExtendedCtrlWord.emplace_back(SercosParamStruct(ReadWriteType::READ, P_0_0226_PROBE_EXTENDED_CTRL_WORD, 
                                             &m_probeBitProcess.m_extendedCtrlWord.m_word, sizeof(m_probeBitProcess.m_extendedCtrlWord.m_word)));
    auto currentProbe = m_probeCmds.find(probeIdx);
    if (currentProbe != m_probeCmds.end())
    {
      // only if expected window activated and time measurement not activated it is meaningful to write expected window into device.
      if (probeExtendConfig.m_isExpWindowActivated && !probeExtendConfig.m_isTimeMeasureActivated) 
      {
        m_paramListExtendedCtrlWord.emplace_back(SercosParamStruct(ReadWriteType::WRITE, currentProbe->second.m_expWindowStart, 
                                                 &m_probeParam.m_expPosStart, sizeof(m_probeParam.m_expPosStart)));
        m_paramListExtendedCtrlWord.emplace_back(SercosParamStruct(ReadWriteType::WRITE, currentProbe->second.m_expWindowEnd, 
                                                 &m_probeParam.m_expPosEnd, sizeof(m_probeParam.m_expPosEnd)));
      }
      // write measured signal
      m_paramListExtendedCtrlWord.emplace_back(SercosParamStruct(ReadWriteType::WRITE, currentProbe->second.m_signalSelection,
                                               &m_probeParam.m_selectedSignal, sizeof(m_probeParam.m_selectedSignal)));
      // write dead time compensation for positive edge
      m_paramListExtendedCtrlWord.emplace_back(SercosParamStruct(ReadWriteType::WRITE, currentProbe->second.m_posEdgeCompensation,
                                               &m_probeParam.m_posEdgeCompensation, sizeof( m_probeParam.m_posEdgeCompensation)));
      // write dead time compensation for negative edge
      m_paramListExtendedCtrlWord.emplace_back(SercosParamStruct(ReadWriteType::WRITE, currentProbe->second.m_negEdgeCompensation,
                                               &m_probeParam.m_negEdgeCompensation, sizeof(m_probeParam.m_negEdgeCompensation)));
    }
    else
    {
      Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST,
                         __FILE__, __FUNCTION__, __LINE__, "Invalid probe index: %d", probeIdx);
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    asyncState->switchToNextStep();
  }
  
  result = executeExtendedCtrlWordConfig(probeIdx, probeExtendConfig, isStateMonitActivated, *asyncState);
  return resultHandlingCycle(*asyncState, result);
}

AxisProfileResult SercosProbeDrive::probeStatusCtrl(ProbeIndex probeIdx, bool enable)
{
  AsyncCommandStatemachine* asyncState {nullptr};
  AxisProfileResult result = raiseAndGetStatemachine(probeIdx, asyncState);
  if (aprStatusNotOkay(result))
  {
    return result;
  }

  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    checkIsSignalSupported(probeIdx, m_isSignalFuncCmd);

    APR_TRACE_MESSAGE(m_isSignalFuncCmd ? "probe-ex command : handled as signal function." : "probe-ex : handled as normal command.");
    m_probeEable.clear();

    bool preconditionFullFilled = itemListChecker(probeIdx, probeIdx == ProbeIndex::PROBE_1 ? m_probe1CheckList : m_probe2CheckList);
    if (!preconditionFullFilled)
    {
      // diagnosis has reported in checker
      return AxisProfileResult::ERR_PROFILE_CONFIG;
    }

    if (!m_isSignalFuncCmd)
    {
      // check probe status
      m_probeEable.emplace_back(SercosParamStruct(ReadWriteType::READ, S_0_0405_ENABLE_PROBE_1,
                                                  &m_probeParam.m_probe1Enable, sizeof(m_probeParam.m_cmdEnableValue)));
      m_probeEable.emplace_back(SercosParamStruct(ReadWriteType::READ, S_0_0406_ENABLE_PROBE_2,
                                                  &m_probeParam.m_probe2Enable, sizeof(m_probeParam.m_cmdEnableValue)));
    }
    asyncState->switchToNextStep();
  }
  if (!m_isSignalFuncCmd)
  {
    result = executeProbeStatusCtrl(probeIdx, enable, *asyncState);
  }
  else
  {
    result = executeProbeStatusCtrlSignalFunc(probeIdx, enable, *asyncState);
  }
  return resultHandlingCycle(*asyncState, result);
}

AxisProfileResult SercosProbeDrive::checkIsSignalSupported(ProbeIndex probeIdx, bool& supported)
{
  supported = false;
  // decide the probe working method, check if the command is invoked via signal function or via probe async interface
  if (!m_sharingHandler->getDeviceParamIF())
  {
    APR_TRACE_MESSAGE("failed to checkIsSignalSupported, command will handle in async way");
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto deviceParam = m_sharingHandler->getDeviceParamIF();
  auto handler = deviceParam->getSignalFuncHandler();
  m_signalFuncProbe.initSignalFuncHandler(handler);

  // can decide the actual probe via probe index, since the probe async interface can not be used by its 2 probes same time!
  switch (probeIdx)
  {
    case ProbeIndex::PROBE_1:
      m_signalFuncProbe.setSignalFunc(SignalFunction(FunctionSupport::PROBE1_ENABLE_CYCLIC));
      break;
    case ProbeIndex::PROBE_2:
      m_signalFuncProbe.setSignalFunc(SignalFunction(FunctionSupport::PROBE2_ENABLE_CYCLIC));
      break;
    default:
      Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST, __FILE__, __FUNCTION__, __LINE__, "Invalid probe number: %s", probeIdx);
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  FunctionSupportErrorInfo errorInfo {};
  supported = m_signalFuncProbe.isSignalFuncSupported(errorInfo);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosProbeDrive::getProbeActualValuesRT(ProbeIndex probeIdx, ProbeActualValues* actualValueProbe)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  auto scaling = m_sharingHandler->getScalingIF();
  if (probeIdx == ProbeIndex::PROBE_1)
  {
    if (m_prb1MeasureSignal != P_0_2755_SYSTEM_FINE_TIME)
    {
      
      actualValueProbe->m_probeLatchedPosEdge = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe1.posEdg);
      actualValueProbe->m_probeLatchedNegEdge = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe1.negEdg);
      actualValueProbe->m_probeEdgeDiff = m_sharingHandler->getScalingIF()->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe1.positionDif);
    }
    else
    {
      std::uint32_t probeFienTime = 0;
      const std::size_t dataSize = 4;   // convert std::int32_t to std::uint32_t
      actualValueProbe->m_probeLatchedPosEdge = timeUintAdapteInv(probeFienTime, m_telegramAT->m_probe.m_probe1.posEdg, dataSize);
      actualValueProbe->m_probeLatchedNegEdge = timeUintAdapteInv(probeFienTime, m_telegramAT->m_probe.m_probe1.negEdg, dataSize);
      actualValueProbe->m_probeEdgeDiff = timeUintAdapteInv(probeFienTime, m_telegramAT->m_probe.m_probe1.positionDif, dataSize);

    }
    actualValueProbe->m_isProbeActivated = m_telegramAT->m_probe.m_probe1.probeActive;
    actualValueProbe->m_posEdgeLatchedTimes = m_telegramAT->m_probe.m_probe1.posLatched;
    actualValueProbe->m_negEdgeLatchedTimes = m_telegramAT->m_probe.m_probe1.negLatched;
    actualValueProbe->m_probeMarkeFailures = m_telegramAT->m_probe.m_probe1.markFailure;
  }
  else if (probeIdx == ProbeIndex::PROBE_2)
  {
    if (m_prb2MeasureSignal != P_0_2755_SYSTEM_FINE_TIME)
    {
      actualValueProbe->m_probeLatchedPosEdge = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe2.posEdg);
      actualValueProbe->m_probeLatchedNegEdge = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe2.negEdg);
      actualValueProbe->m_probeEdgeDiff = scaling->convertPositionRawToScaled(m_telegramAT->m_probe.m_probe2.positionDif);
    }
    else
    {
      std::uint32_t probeFienTime = 0;
      const std::size_t dataSize = 4;   // convert std::int32_t to std::uint32_t
      actualValueProbe->m_probeLatchedPosEdge = timeUintAdapteInv(probeFienTime, m_telegramAT->m_probe.m_probe2.posEdg, dataSize);
      actualValueProbe->m_probeLatchedNegEdge = timeUintAdapteInv(probeFienTime, m_telegramAT->m_probe.m_probe2.negEdg, dataSize);
      actualValueProbe->m_probeEdgeDiff = timeUintAdapteInv(probeFienTime, m_telegramAT->m_probe.m_probe2.positionDif, dataSize);

    }

    actualValueProbe->m_isProbeActivated = m_telegramAT->m_probe.m_probe2.probeActive;
    actualValueProbe->m_posEdgeLatchedTimes = m_telegramAT->m_probe.m_probe2.posLatched;
    actualValueProbe->m_negEdgeLatchedTimes = m_telegramAT->m_probe.m_probe2.negLatched;
    actualValueProbe->m_probeMarkeFailures = m_telegramAT->m_probe.m_probe2.markFailure;
  }
  else
  {
    Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST,
                       __FILE__, __FUNCTION__, __LINE__, "Invalid probe number: %s", probeIdx);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosProbeDrive::onlineChangeParams(ProbeIndex probeIdx, ProbeUpdateMeasurement updateValue)
{
  AsyncCommandStatemachine* asyncState{ nullptr };
  AxisProfileResult result = raiseAndGetStatemachine(probeIdx, asyncState);
  if (aprStatusNotOkay(result))
  {
    return result;
  }

  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  auto scaling = m_sharingHandler->getScalingIF();

  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    m_probeParam.m_updateExpPosStart = scaling->convertPositionScaledToRaw(updateValue.m_expectWindowStart);
    m_probeParam.m_updateExpPosEnd = scaling->convertPositionScaledToRaw(updateValue.m_expectWindowEnd);

    m_onlineUpdate.clear();
    m_onlineUpdate.emplace_back(SercosParamStruct(ReadWriteType::READ, S_0_0169_PROBE_CTRL_WORD, 
                                &m_probeBitProcess.m_ctrlWord.m_word, sizeof(m_probeBitProcess.m_ctrlWord.m_word)));
    m_onlineUpdate.emplace_back(SercosParamStruct(ReadWriteType::READ, P_0_0226_PROBE_EXTENDED_CTRL_WORD, 
                                &m_probeBitProcess.m_extendedCtrlWord.m_word, sizeof(m_probeBitProcess.m_extendedCtrlWord.m_word)));

    asyncState->switchToNextStep();
  }
  
  result = updateMeasureData(probeIdx, *asyncState, updateValue);
  return resultHandlingCycle(*asyncState, result);;
}

AxisProfileResult SercosProbeDrive::getProbeValues(ProbeIndex probeIdx, ProbeBaseConfig* probeConfig, ProbeExtConfig* probeExtendConfig)
{
  AsyncCommandStatemachine* asyncState{ nullptr };
  AxisProfileResult result = raiseAndGetStatemachine(probeIdx, asyncState);
  if (aprStatusNotOkay(result))
  {
    return result;
  }
  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    m_probeActualValues.clear();
    m_probeActualValues.emplace_back(SercosParamStruct(ReadWriteType::READ, S_0_0169_PROBE_CTRL_WORD, 
                                    &m_probeBitProcess.m_ctrlWord.m_word, sizeof(m_probeBitProcess.m_ctrlWord.m_word)));
    m_probeActualValues.emplace_back(SercosParamStruct(ReadWriteType::READ, P_0_0226_PROBE_EXTENDED_CTRL_WORD, 
                                    &m_probeBitProcess.m_extendedCtrlWord.m_word, sizeof(m_probeBitProcess.m_extendedCtrlWord.m_word)));

    
    auto currentProbe = m_probeCmds.find(probeIdx);
    if (currentProbe != m_probeCmds.end())
    {
      m_probeActualValues.emplace_back(SercosParamStruct(ReadWriteType::READ, currentProbe->second.m_expWindowStart, 
                                      &m_probeParam.m_expPosStart, sizeof(m_probeParam.m_expPosStart)));

      m_probeActualValues.emplace_back(SercosParamStruct(ReadWriteType::READ, currentProbe->second.m_expWindowEnd, 
                                      &m_probeParam.m_expPosEnd, sizeof(m_probeParam.m_expPosEnd))); 

      m_probeActualValues.emplace_back(SercosParamStruct(ReadWriteType::READ, currentProbe->second.m_signalSelection,
                                      &m_probeParam.m_selectedSignal, sizeof(m_probeParam.m_selectedSignal)));

      m_probeActualValues.emplace_back(SercosParamStruct(ReadWriteType::READ, currentProbe->second.m_posEdgeCompensation,
                                      &m_probeParam.m_posEdgeCompensation, sizeof(m_probeParam.m_posEdgeCompensation)));

      m_probeActualValues.emplace_back(SercosParamStruct(ReadWriteType::READ, currentProbe->second.m_negEdgeCompensation,
                                      &m_probeParam.m_negEdgeCompensation, sizeof(m_probeParam.m_negEdgeCompensation))); 
    }
    else
    {
      Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST,
                         __FILE__, __FUNCTION__, __LINE__, "Invalid probe number: %d", probeIdx);
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    asyncState->switchToNextStep();
   }
    result = probeValueProcess(probeIdx, *asyncState);

    if (aprStatusOkay(result))
    {
      *probeConfig = m_probeParam.m_probeConfig;
      *probeExtendConfig = m_probeParam.m_probeExtendConfig;
    }

  return resultHandlingCycle(*asyncState, result);
}

AxisProfileResult SercosProbeDrive::abortAndResetProbe()
{
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}
//////////////////////////////////////////////////  PRIVATE  METHODS  //////////////////////////////////////////////////
AxisProfileResult SercosProbeDrive::executeCtrlWordConfig(ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool isAutoActivated, AsyncCommandStatemachine& asyncState)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  switch (asyncState.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS: 
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      result = m_paramAccess->readAndCheckParamDataAsync(S_0_0169_PROBE_CTRL_WORD,
                                                         static_cast<void*>(&m_probeBitProcess.m_ctrlWord.m_word), 
                                                         sizeof(m_probeBitProcess.m_ctrlWord.m_word), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:   // set control word configuration to probes
    {
      if (probeIdx == ProbeIndex::PROBE_1)
      {
        m_probeBitProcess.m_ctrlWord.m_bits.bit0 = static_cast<std::uint16_t>(probeConfig.m_evaluatePosEdge);
        m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_POS_EDGE, probeConfig.m_evaluatePosEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe1PosLat,
                                       idnToString(S_0_0409_PROBE1_POS_LATCH_COUNTER));
        m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_POS_EDGE, probeConfig.m_evaluatePosEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe1PosEdg,
                                       idnToString(S_0_0130_PROBE1_POS_EDGE));

        m_probeBitProcess.m_ctrlWord.m_bits.bit1 = static_cast<std::uint16_t>(probeConfig.m_evaluateNegEdge);
        m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_NEG_EDGE, probeConfig.m_evaluateNegEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe1NegLat,
                                       idnToString(S_0_0410_PROBE1_NEG_LATCH_COUNTER));
        m_probe1CheckList.emplace_back(CHEKC_ITEM_EVL_NEG_EDGE, probeConfig.m_evaluateNegEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe1NegEdg,
                                       idnToString(S_0_0131_PROBE1_NEG_EDGE));

        m_probeBitProcess.m_ctrlWord.m_bits.bit5 = static_cast<std::uint16_t>(probeConfig.m_measureType);
      }
      else if (probeIdx == ProbeIndex::PROBE_2)
      {
        m_probeBitProcess.m_ctrlWord.m_bits.bit2 = static_cast<std::uint16_t>(probeConfig.m_evaluatePosEdge);
        m_probe2CheckList.emplace_back(CHEKC_ITEM_EVL_POS_EDGE, probeConfig.m_evaluatePosEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe2PosLat,
                                       idnToString(S_0_0411_PROBE2_POS_LATCH_COUNTER));
        m_probe2CheckList.emplace_back(CHEKC_ITEM_EVL_POS_EDGE, probeConfig.m_evaluatePosEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe2PosEdg,
                                       idnToString(S_0_0132_PROBE2_POS_EDGE));

        m_probeBitProcess.m_ctrlWord.m_bits.bit3 = static_cast<std::uint16_t>(probeConfig.m_evaluateNegEdge);
        m_probe2CheckList.emplace_back(CHEKC_ITEM_EVL_NEG_EDGE, probeConfig.m_evaluateNegEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe2NegLat,
                                       idnToString(S_0_0412_PROBE2_NEG_LATCH_COUNTER));
        m_probe2CheckList.emplace_back(CHEKC_ITEM_EVL_NEG_EDGE, probeConfig.m_evaluateNegEdge,
                                       PRECONDITION_LATCHED_CNT_MAPPED, profileAssignedInfo.m_input.m_probe2NegEdg,
                                       idnToString(S_0_0133_PROBE2_NEG_EDGE));
        m_probeBitProcess.m_ctrlWord.m_bits.bit6 = static_cast<std::uint16_t>(probeConfig.m_measureType);
      }
      else 
      {
        Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST,
                           __FILE__, __FUNCTION__, __LINE__, "Invalid probe number: %d", probeIdx);
        return AxisProfileResult::ERR_INVALID_ARGUMENT;
      }
      m_probeBitProcess.m_ctrlWord.m_bits.bit8 = static_cast<std::uint16_t>(isAutoActivated);
      
      asyncState.switchToCertainStep(AsyncCommandStatemachine::CommandStep::CMD_DONE);
      result = AxisProfileResult::STS_PENDING;
      break;

    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS: 
    case AsyncCommandStatemachine::CommandStep::CMD_DONE: 
    {
      result = m_paramAccess->writeAndCheckParamAsync(S_0_0169_PROBE_CTRL_WORD,
                                                      static_cast<void*>(&m_probeBitProcess.m_ctrlWord.m_word), 
                                                      sizeof(m_probeBitProcess.m_ctrlWord.m_word), asyncState);
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
      APR_TRACE_ERROR("invalid state to execute executeCtrlWordConfig.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
  }
  return result;
}

AxisProfileResult SercosProbeDrive::executeExtendedCtrlWordConfig(ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig, 
                                                             bool isStateMonitActivated, AsyncCommandStatemachine& asyncState)
{
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo(); 
  AxisProfileResult result = AxisProfileResult::STS_OK;
  switch (asyncState.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS: 
    {
      result = m_paramAccess->accessAndCheckParamListAsync(m_paramListExtendedCtrlWord, asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToCmdStartStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_START:   // write extended control word into probes 
    {
      if (probeIdx == ProbeIndex::PROBE_1)
      {
        m_probeBitProcess.m_extendedCtrlWord.m_bits.bit0 = probeExtendConfig.m_isExpWindowActivated;

        m_probeBitProcess.m_extendedCtrlWord.m_bits.bit1 = probeExtendConfig.m_isMarkerFailureActivated;
        m_probe1CheckList.emplace_back(CHEKC_ITEM_MARKER_FAILURE, probeExtendConfig.m_isMarkerFailureActivated,
                                       PRECONDITION_MARKER_FAILURE, profileAssignedInfo.m_input.m_probe1MakFal,
                                       idnToString(P_0_0224_PROBE1_MARKER_FAILURE));
        m_probeBitProcess.m_extendedCtrlWord.m_bits.bit2 = probeExtendConfig.m_isTimeMeasureActivated;
        m_probeBitProcess.m_extendedCtrlWord.m_bits.bit11 = isStateMonitActivated;
      }
      else   //the probe number has already been judged so if not probe 1, is probe 2.
      {
        m_probeBitProcess.m_extendedCtrlWord.m_bits.bit4 = probeExtendConfig.m_isExpWindowActivated;

        m_probeBitProcess.m_extendedCtrlWord.m_bits.bit5 = probeExtendConfig.m_isMarkerFailureActivated;
        m_probe2CheckList.emplace_back(CHEKC_ITEM_MARKER_FAILURE, probeExtendConfig.m_isMarkerFailureActivated,
                                       PRECONDITION_MARKER_FAILURE, profileAssignedInfo.m_input.m_probe2MakFal,
                                       idnToString(P_0_0225_PROBE2_MARKER_FAILURE));
        m_probeBitProcess.m_extendedCtrlWord.m_bits.bit6 = probeExtendConfig.m_isTimeMeasureActivated;
      }

      asyncState.switchToNextStep();
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET: 
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS: 
    case AsyncCommandStatemachine::CommandStep::CMD_DONE: 
    {
      result = m_paramAccess->writeAndCheckParamAsync(P_0_0226_PROBE_EXTENDED_CTRL_WORD,
                                                      static_cast<void*>(&m_probeBitProcess.m_extendedCtrlWord.m_word), 
                                                      sizeof(m_probeBitProcess.m_extendedCtrlWord.m_word), asyncState);
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
      APR_TRACE_ERROR("invalid state to execute executeExtendedCtrlWordConfig.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
  }
  return result;
}

AxisProfileResult SercosProbeDrive::executeProbeStatusCtrl(ProbeIndex probeIdx, bool enable, AsyncCommandStatemachine& asyncState)
{
  AxisProfileResult result {AxisProfileResult::STS_OK};
  switch (asyncState.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS: 
    {
      result = m_paramAccess->accessAndCheckParamListAsync(m_probeEable, asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToCmdStartStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      if (checkProbeEnabledStatus(probeIdx, enable))
      {
        m_probeParam.m_cmdEnableValue = enable ? SERCOS_COMMAND_ENABLE : SERCOS_COMMAND_NOT_SET;
        result = m_paramAccess->writeAndCheckParamAsync(S_0_0170_PROBING_CYCLE_PROCEDURE,
                                                        static_cast<void*>(&m_probeParam.m_cmdEnableValue), 
                                                        sizeof(m_probeParam.m_cmdEnableValue), asyncState);
      }
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
        m_probeParam.m_probeStatusCtrl = static_cast<std::uint16_t>(enable);
        result = m_paramAccess->writeAndCheckParamAsync(static_cast<std::uint16_t>(currentProbe->second.m_probeStatusCtrl),
                                                        static_cast<void*>(&m_probeParam.m_probeStatusCtrl), 
                                                        sizeof(m_probeParam.m_probeStatusCtrl), asyncState);                          
      }
      else
      {
        Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST, __FILE__, __FUNCTION__, __LINE__, "Invalid probe number: %s", probeIdx);
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

    default: // error reaction
    {
      APR_TRACE_ERROR("invalid state to execute executeProbeStatusCtrl.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
  }
  return result;
}

AxisProfileResult SercosProbeDrive::executeProbeStatusCtrlSignalFunc(ProbeIndex probeIdx, bool enable, AsyncCommandStatemachine& asyncState)
{
  AxisProfileResult result {AxisProfileResult::STS_PENDING};
  switch (asyncState.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS:
    {
      bool state {};
      result = m_signalFuncProbe.getSignalFuncState(state);
      // init the probe parameter for signal function
      switch (probeIdx)
      {
        case ProbeIndex::PROBE_1:
        {
          m_probeParam.m_probe1Enable = state;
          break;
        }
        case ProbeIndex::PROBE_2:
        {
          m_probeParam.m_probe2Enable = state;
          break;
        }
        default:
          Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST, __FILE__, __FUNCTION__, __LINE__, "Invalid probe number: %s", probeIdx);
          result = AxisProfileResult::ERR_INVALID_ARGUMENT;
          break;
      }
      if (aprStatusOkay(result))
      {
        asyncState.switchToCmdStartStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      uint16_t probeCmd = enable ? SERCOS_COMMAND_ENABLE : SERCOS_COMMAND_NOT_SET;
      result = m_paramAccess->writeAndCheckParamAsync(S_0_0170_PROBING_CYCLE_PROCEDURE,
                                                        static_cast<void*>(&probeCmd),
                                                        sizeof(probeCmd), asyncState);      
      if (aprStatusOkay(result))
      {
        asyncState.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      result = m_signalFuncProbe.onJobAborting();
      if (aprStatusOkay(result))
      {
        asyncState.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    {
      if (enable)
      {
        result = m_signalFuncProbe.singleActiveSignalFunc();
      }
      else
      {
        result = AxisProfileResult::STS_OK;
      }
      if (aprStatusOkay(result))
      {
        asyncState.switchToCertainStep(AsyncCommandStatemachine::CommandStep::PARAM_POST);
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    case AsyncCommandStatemachine::CommandStep::CMD_CLEAR:
    case AsyncCommandStatemachine::CommandStep::PARAM_POST:
    {
      result = AxisProfileResult::STS_OK;
      break;
    }

    default: // error reaction
    {
      APR_TRACE_ERROR("invalid state to execute executeProbeStatusCtrl.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
  }
  return result;
}

AxisProfileResult SercosProbeDrive::probeValueProcess(ProbeIndex probeIdx, AsyncCommandStatemachine& asyncState)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  auto scaling = m_sharingHandler->getScalingIF();

  AxisProfileResult result = m_paramAccess->accessAndCheckParamListAsync(m_probeActualValues, asyncState);
  if (aprStatusOkay(result))
  {
    if (probeIdx == ProbeIndex::PROBE_1)
    {
      m_probeParam.m_probeConfig.m_measureType = m_probeBitProcess.m_ctrlWord.m_bits.bit5;
      m_probeParam.m_probeConfig.m_evaluatePosEdge = m_probeBitProcess.m_ctrlWord.m_bits.bit0;
      m_probeParam.m_probeConfig.m_evaluateNegEdge = m_probeBitProcess.m_ctrlWord.m_bits.bit1;

      m_probeParam.m_probeExtendConfig.m_isExpWindowActivated = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit0;
      m_probeParam.m_probeExtendConfig.m_isMarkerFailureActivated = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit1;
      m_probeParam.m_probeExtendConfig.m_isTimeMeasureActivated = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit2;
      m_probeParam.m_isStateMonitActivated = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit11;
    }
    else
    {
      m_probeParam.m_probeConfig.m_measureType = m_probeBitProcess.m_ctrlWord.m_bits.bit6;
      m_probeParam.m_probeConfig.m_evaluatePosEdge = m_probeBitProcess.m_ctrlWord.m_bits.bit2;
      m_probeParam.m_probeConfig.m_evaluateNegEdge = m_probeBitProcess.m_ctrlWord.m_bits.bit3;

      m_probeParam.m_probeExtendConfig.m_isExpWindowActivated = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit4;
      m_probeParam.m_probeExtendConfig.m_isMarkerFailureActivated = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit5;
      m_probeParam.m_probeExtendConfig.m_isTimeMeasureActivated = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit6;
    }
    
    m_probeParam.m_isAutoActivated = m_probeBitProcess.m_ctrlWord.m_bits.bit8;

    if (m_probeParam.m_probeExtendConfig.m_isExpWindowActivated)  //only if expected window activated the value is reasonable
    {
      m_probeParam.m_probeExtendConfig.m_expectWindowPosStart =  scaling->convertPositionRawToScaled(m_probeParam.m_expPosStart);
      m_probeParam.m_probeExtendConfig.m_expectWindowPosEnd = scaling->convertPositionRawToScaled(m_probeParam.m_expPosEnd);
    }

    m_probeParam.m_probeExtendConfig.m_timeCompensationPosEdge =  scaling->convertPositionRawToScaled(m_probeParam.m_posEdgeCompensation);
    m_probeParam.m_probeExtendConfig.m_timeCompensationNegEdge = scaling->convertPositionRawToScaled(m_probeParam.m_negEdgeCompensation);
    m_probeParam.m_probeExtendConfig.m_measuredSignal = measuredSignalConvert();
  }

  return result;
}

AxisProfileResult SercosProbeDrive::updateMeasureData(ProbeIndex probeIdx, AsyncCommandStatemachine& asyncState, ProbeUpdateMeasurement updateValue)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  switch (asyncState.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS: 
    {
      result = m_paramAccess->accessAndCheckParamListAsync(m_onlineUpdate, asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToCmdStartStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      if (probeIdx == ProbeIndex::PROBE_1)
      {
        // update control word
        m_probeBitProcess.m_ctrlWord.m_bits.bit0 = updateValue.m_evaluatePosEdge;
        m_probeBitProcess.m_ctrlWord.m_bits.bit1 = updateValue.m_evaluateNegEdge;
        m_probeBitProcess.m_ctrlWord.m_bits.bit5 = updateValue.m_measureType;

        // update extended control word 
        // probe 1 time measurement activated if bit 2 of extended control word is true could not write expect window
        m_timeMeasure = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit2 ? true : false;
        if (m_timeMeasure)
        {
          // if time measurement activated, expect window and marker failure is deactivated.
          m_probeBitProcess.m_extendedCtrlWord.m_bits.bit0 = false;
          m_probeBitProcess.m_extendedCtrlWord.m_bits.bit1 = false;
          APR_TRACE_MESSAGE("time measurement activated, expect window and marker failure is deactivated.");
        }
        else
        {
          m_probeBitProcess.m_extendedCtrlWord.m_bits.bit0 = updateValue.m_isExpWindowActivated;
          m_probeBitProcess.m_extendedCtrlWord.m_bits.bit1 = updateValue.m_isMarkerFailureActivated;
        }
      }
      else if (probeIdx == ProbeIndex::PROBE_2)
      {
        // update control word
        m_probeBitProcess.m_ctrlWord.m_bits.bit2 = updateValue.m_evaluatePosEdge;
        m_probeBitProcess.m_ctrlWord.m_bits.bit3 = updateValue.m_evaluateNegEdge;
        m_probeBitProcess.m_ctrlWord.m_bits.bit6 = updateValue.m_measureType;

        // update extended control word 
        // probe 1 time measurement activated if bit 2 of extended control word is true could not write expect window
        m_timeMeasure = m_probeBitProcess.m_extendedCtrlWord.m_bits.bit6 ? true : false;
        if (m_timeMeasure)
        {
          // if time measurement activated, expect window and marker failure is deactivated.
          m_probeBitProcess.m_extendedCtrlWord.m_bits.bit4 = false;
          m_probeBitProcess.m_extendedCtrlWord.m_bits.bit5 = false;
          APR_TRACE_MESSAGE("time measurement activated, expect window and marker failure is deactivated.");
        }
        else
        {
          m_probeBitProcess.m_extendedCtrlWord.m_bits.bit4 = updateValue.m_isExpWindowActivated;
          m_probeBitProcess.m_extendedCtrlWord.m_bits.bit5 = updateValue.m_isMarkerFailureActivated;
        }
      }
      else
      {
        Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST,
                           __FILE__, __FUNCTION__, __LINE__, "Invalid probe number: %d", probeIdx);
        APR_TRACE_ERROR("invalid probe of online update probe measurement data, please check if selected probe is available.");
        return AxisProfileResult::ERR_INVALID_ARGUMENT;
      }
      asyncState.switchToNextStep();
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET: 
    {
      m_onlineUpdate.emplace_back(SercosParamStruct(ReadWriteType::WRITE, S_0_0169_PROBE_CTRL_WORD,
                                  &m_probeBitProcess.m_ctrlWord.m_word, sizeof(m_probeBitProcess.m_ctrlWord.m_word)));
    
      m_onlineUpdate.emplace_back(SercosParamStruct(ReadWriteType::WRITE, P_0_0226_PROBE_EXTENDED_CTRL_WORD,
                                  &m_probeBitProcess.m_extendedCtrlWord.m_word, sizeof(m_probeBitProcess.m_extendedCtrlWord.m_word)));
      if (!m_timeMeasure && updateValue.m_isExpWindowActivated)
      {
        auto currentProbe = m_probeCmds.find(probeIdx);
        if (currentProbe != m_probeCmds.end())
        {
          m_onlineUpdate.emplace_back(SercosParamStruct(ReadWriteType::WRITE, currentProbe->second.m_expWindowStart, 
                                      &m_probeParam.m_updateExpPosStart , sizeof(m_probeParam.m_updateExpPosStart )));

          m_onlineUpdate.emplace_back(SercosParamStruct(ReadWriteType::WRITE, currentProbe->second.m_expWindowEnd, 
                                      &m_probeParam.m_updateExpPosEnd , sizeof(m_probeParam.m_updateExpPosEnd )));
        }
      }
      asyncState.switchToNextStep();
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS: 
    case AsyncCommandStatemachine::CommandStep::CMD_DONE: 
    {
      result = m_paramAccess->accessAndCheckParamListAsync(m_onlineUpdate, asyncState);
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
      m_timeMeasure = false;
      break;
    }

    default: // error reaction
    {
      APR_TRACE_ERROR("invalid state to execute executeExtendedCtrlWordConfig.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
  }
  return result;
}

void SercosProbeDrive::setProbeCommands()
{
  ExecuteProbe probe_1;
  probe_1.m_expWindowStart = P_0_0204_EXP_WINDOW_START_PROBE_1;
  probe_1.m_expWindowEnd = P_0_0205_EXP_WINDOW_END_PROBE_1;
  probe_1.m_signalSelection = S_0_0426_SIGNAL_SELECTION_PROBE_1;
  probe_1.m_posEdgeCompensation = S_0_0524_POS_EDGE_COMPENSATION_PROBE_1;
  probe_1.m_negEdgeCompensation = S_0_0525_NEG_EDGE_COMPENSATION_PROBE_1;
  probe_1.m_probeStatusCtrl = S_0_0405_ENABLE_PROBE_1;
  
  m_probeCmds.emplace(std::make_pair(ProbeIndex::PROBE_1, probe_1));

  ExecuteProbe probe_2;
  probe_2.m_expWindowStart = P_0_0200_EXP_WINDOW_START_PROBE_2;
  probe_2.m_expWindowEnd = P_0_0201_EXP_WINDOW_END_PROBE_2;
  probe_2.m_signalSelection = S_0_0427_SIGNAL_SELECTION_PROBE_2;
  probe_2.m_posEdgeCompensation = S_0_0526_POS_EDGE_COMPENSATION_PROBE_2;
  probe_2.m_negEdgeCompensation = S_0_0527_NEG_EDGE_COMPENSATION_PROBE_2;
  probe_2.m_probeStatusCtrl = S_0_0406_ENABLE_PROBE_2;

  m_probeCmds.emplace(std::make_pair(ProbeIndex::PROBE_2, probe_2));
}

MeasuringSignalSel SercosProbeDrive::measuredSignalConvert()
{
  switch (m_probeParam.m_selectedSignal)
  {
    case S_0_0051_POSITION_FEEDBACK_ENCODER_1:
    {
      m_probeParam.m_probeExtendConfig.m_measuredSignal = MeasuringSignalSel::POS_FEEDBACK_VAL_ENCODER_1;
      break;
    }
    case S_0_0053_POSITION_FEEDBACK_ENCODER_2:
    {
      m_probeParam.m_probeExtendConfig.m_measuredSignal = MeasuringSignalSel::POS_FEEDBACK_VAL_ENCODER_2;
      break;
    }
    case P_0_2755_SYSTEM_FINE_TIME:
    {
      m_probeParam.m_probeExtendConfig.m_measuredSignal = MeasuringSignalSel::SYSTEM_FINE_TIME;
      break;
    }
    case S_0_0000_DUMMY:
    {
      m_probeParam.m_probeExtendConfig.m_measuredSignal = MeasuringSignalSel::NO_COMMAND_ACTIVE;
      break;
    }
    default:
    { 
      APR_TRACE_ERROR("Selected command has not been defined.");
      return MeasuringSignalSel::SIGNAL_UNDEFINED;
    }
  }
  return m_probeParam.m_probeExtendConfig.m_measuredSignal;
}

std::int32_t SercosProbeDrive::timeUnitAdapteDevice(double rawTime)
{
  // The time of motion side is SI, the input unit is second, 
  // the time unit in ctrlX/indra drive of probe is nanosecond.
  // So the time unit needs to be convert from second to nanosecond.
  return static_cast<std::int32_t>(SECOND_2_NANOSECOND(rawTime)); 
}                                                

double SercosProbeDrive::timeUintAdapteInv(std::uint32_t convertTime, std::int32_t rawTime, const size_t dataSize)
{
  std::memcpy(&convertTime, &rawTime, dataSize);
  double returnValue = NANOSECOND_2_SECOND(static_cast<double>(convertTime));
  
  return returnValue;
}

bool SercosProbeDrive::checkProbeEnabledStatus(ProbeIndex probeIdx,  bool enable) const
{
  if (!m_probeParam.m_probe1Enable && !m_probeParam.m_probe2Enable) 
  {
    return enable;
  }

  if (m_probeParam.m_probe1Enable && m_probeParam.m_probe2Enable) 
  {
    return false;
  }

  if ((m_probeParam.m_probe1Enable && probeIdx == ProbeIndex::PROBE_1) 
     || (m_probeParam.m_probe2Enable && probeIdx == ProbeIndex::PROBE_2)) 
  {
    return !enable;
  }
  
  return false;
}
