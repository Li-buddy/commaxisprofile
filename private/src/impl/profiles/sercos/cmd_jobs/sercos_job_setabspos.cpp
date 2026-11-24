/// @file
///
/// @brief This file is part of the axisprofile library and implements the Sercos job of set absolute position.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "diagnosis/dia.h"
#include "sercos_job_setabspos.h"
#include "profiles/profile_defs.h"

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

SercosJobSetAbsPos::SercosJobSetAbsPos(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle)
  : SercosJobBase(ProcedureCommands::S_0_0447_SET_ABSOLUTE_POSITION, profileInfo, shareHandle)
{}

AxisProfileResult SercosJobSetAbsPos::onSetArgument(IContainer* arg)
{
  auto setAbsPosArg = ArgSetAbsolutePosition::safeCast(arg);
  if (setAbsPosArg == nullptr)
  {
    APR_TRACE_ERROR("%s %s gets wrong type of argument",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_JOB_DATA_MISMATCH;
  }

  // load inputs
  m_cmdPos = setAbsPosArg->getRef().newPos;
  m_encoder = setAbsPosArg->getRef().encoder;
  m_allowPowerEnabled = setAbsPosArg->getRef().allowPowerEnabled;
  APR_TRACE_MESSAGE("%s %s with: cmdPos=%g, encoderId=%d, allowPowerEnabled=%s",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    m_cmdPos, static_cast<std::int16_t>(m_encoder),
                    m_allowPowerEnabled ? "true" : "false");

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobSetAbsPos::onJobEnterActivating()
{
  m_jobFuns.clear();
  m_jobStep = 0;

  if (AxisProfileCategory::CATEGORY_ENCODER == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
     m_jobFuns.emplace_back([&]() {return judgmentHandleBased();});
  }
  else if (AxisProfileCategory::CATEGORY_DRIVE == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
    m_jobFuns.emplace_back([&]() {return validateConfig();});
    m_jobFuns.emplace_back([&]() {return writeConfigToDevice();});
    m_jobFuns.emplace_back([&]() {return activeSetAbsPosByDevice();});
  }
  else
  {
    APR_TRACE_ERROR("%s %s gets wrong type of profile category",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_NOK;
  }

  m_asyncSm.setCommandTimeout(0); // disable timeout check since homing movement duration is unknown
  return SercosJobBase::onJobEnterActivating();
}

AxisProfileResult SercosJobSetAbsPos::onJobActivating()
{
  return runJobsInternal();
}

AxisProfileResult SercosJobSetAbsPos::onJobEnterAborting()
{
  m_jobFuns.clear();
  m_jobStep = 0;

  if (AxisProfileCategory::CATEGORY_ENCODER == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
     m_jobFuns.emplace_back([&]() {return resetIoEncoderData();});
  }
  else if (AxisProfileCategory::CATEGORY_DRIVE == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
    m_jobFuns.emplace_back([&]() {return abortSetAbsPosByDevice();});
  }
  else
  {
    APR_TRACE_ERROR("%s %s gets wrong type of profile category",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_NOK;
  }

  return SercosJobBase::onJobEnterAborting();
}

AxisProfileResult SercosJobSetAbsPos::onJobAborting()
{
  return runJobsInternal();
}

////////////////////////////////////////// private methods //////////////////////////////////////////// 

AxisProfileResult SercosJobSetAbsPos::validateConfig()
{
  // scaling and validation
  auto scaling = m_shareHandle->getScalingIF();
  auto bootCfg = m_shareHandle->getDeviceParamIF();

  std::int32_t newRefRaw = scaling->convertPositionScaledToRaw(m_cmdPos);

  // new reference position should in range of travel limit of device
  const auto& deviceCfgData = bootCfg->getDeviceCfgData();
  if (deviceCfgData.m_softwareLimitActive && // check only when limitation is activated in device
      (newRefRaw > deviceCfgData.m_positivePosLimit || newRefRaw < deviceCfgData.m_negativePosLimit))
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_OUT_OF_TRAVEL_LIMIT,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "new position %g out range of [%s, %s] of device", m_cmdPos,
                       idnToString(S_0_0049_POSITIVE_POS_LIMIT).c_str(),
                       idnToString(S_0_0050_NEGATIVE_POS_LIMIT).c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  WordStructure ctrlWordS448;
  WordStructure encoderProperty;
  std::uint16_t encoderPropertyIdn;
  std::uint16_t newRefIdn;
  if (m_encoder == AxisProfileEncoder::ENCODER_1)
  {
    encoderProperty.m_word = deviceCfgData.m_encoder1Property;
    encoderPropertyIdn = S_0_0277_ENCODER_1_PROPERTY;
    newRefIdn = S_0_0052_REFERENCE_DISTANCE_ENCODER_1;
    ctrlWordS448.m_bits.bit0 = true; // S-0-0448.bit0： set absolute position on Encoder1
  }
  else if (m_encoder == AxisProfileEncoder::ENCODER_2)
  {
    encoderProperty.m_word = deviceCfgData.m_encoder2Property;
    newRefIdn = S_0_0054_REFERENCE_DISTANCE_ENCODER_2;
    encoderPropertyIdn = S_0_0115_ENCODER_2_PROPERTY;
    ctrlWordS448.m_bits.bit1 = true; // S-0-0448.bit1： set absolute position on Encoder2
  }
  else
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_NOT_SUPPORT,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "EncoderId %d not exist", static_cast<std::int16_t>(m_encoder));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  // error case: absolute encoder works in relative mode
  if (encoderProperty.m_bits.bit6 && encoderProperty.m_bits.bit7)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_ENCODER_TYPE_ERROR,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Encoder is relative mode. Please set %s.bit7 to 0 and reboot motion", idnToString(encoderPropertyIdn).c_str());
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  // error case: relative encoder works in relative mode
  if (encoderProperty.m_bits.bit6 == false && encoderProperty.m_bits.bit8 == false)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_ENCODER_TYPE_ERROR,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Encoder is relative mode. Please set %s.bit8 to 1 and reboot motion", idnToString(encoderPropertyIdn).c_str());
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  ctrlWordS448.m_bits.bit2 = m_allowPowerEnabled;

  // validation is okay, prepare parameters to be written
  m_newRefIdn = newRefIdn;
  m_newRefRaw = newRefRaw;
  m_ctrlWordS448 = ctrlWordS448.m_word;

  m_paramList.clear();
  m_paramList.emplace_back(ReadWriteType::WRITE, S_0_0448_SET_ABSOLUTE_POSITION_CONTROL, &m_ctrlWordS448, sizeof(m_ctrlWordS448));
  m_paramList.emplace_back(ReadWriteType::WRITE, m_newRefIdn, &m_newRefRaw, sizeof(m_newRefRaw));

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobSetAbsPos::writeConfigToDevice()
{
  auto res = m_paramAccess->accessAndCheckParamListAsync(m_paramList, m_asyncSm.getRequestState());
  if (AxisProfileResult::STS_OK == res)
  {
    m_shareHandle->getScalingIF()->setHomeOffset(0.0); // clear homing offset of control
  }
  return res;
}

AxisProfileResult SercosJobSetAbsPos::activeSetAbsPosByDevice()
{
  auto res = m_paramAccess->setSercosProcedureCommandAsync(m_command, m_asyncSm);
  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  }
  return res;
}

AxisProfileResult SercosJobSetAbsPos::abortSetAbsPosByDevice()
{
  auto res = m_paramAccess->clearSercosProcedureCommandAsync(m_command, m_asyncSm);
  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  }
  return res;
}

AxisProfileResult SercosJobSetAbsPos::judgmentHandleBased()
{
  if (!m_shareHandle || !m_shareHandle->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto mappingHandler = m_shareHandle->getMappingHandlerIF();
  const auto& assignedInfo = mappingHandler->getProfileAssignedInfo();
  if (!assignedInfo.m_input.m_isInReference ||
      !assignedInfo.m_output.m_setReference ||
      !assignedInfo.m_output.m_referenceOffset)
  {
    APR_TRACE_MESSAGE("set absolute position : handle as control based.");
    // handle as control based
    return AxisProfileResult::STS_OK;  //The return value needs to be changed.
  }

  // handle as device based
  // handle by IO device
  m_shareHandle->getCyclicDataIF()->setNewRefPosition(0);

  return setIoEncoderData();
}

AxisProfileResult SercosJobSetAbsPos::setIoEncoderData()
{
  std::int32_t newRefPos = static_cast<std::uint32_t>(m_cmdPos);
  m_shareHandle->getCyclicDataIF()->setNewRefPosition(newRefPos);
  m_shareHandle->getCyclicDataIF()->activeNewRefOperation(true);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobSetAbsPos::resetIoEncoderData()
{
  m_shareHandle->getCyclicDataIF()->activeNewRefOperation(false);
  return AxisProfileResult::STS_OK;
}
