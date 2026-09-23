/// @file
///
/// @brief This file is part of the axisprofile library and implements the sercos homing job.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "diagnosis/dia.h"
#include "profiles/scaling/i_scaling.h"
#include "sercos_job_homing.h"

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

SercosJobHoming::SercosJobHoming(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle)
  : SercosJobBase(ProcedureCommands::S_0_0148_DRIVE_CONTROLLED_HOMING, profileInfo, shareHandle)
{}

AxisProfileResult SercosJobHoming::onSetArgument(IContainer* arg)
{
  auto homingArg = ArgHoming::safeCast(arg);
  if (homingArg == nullptr)
  {
    APR_TRACE_ERROR("%s %s gets wrong type of argument",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_JOB_DATA_MISMATCH;
  }

  // load inputs
  m_cmdData = homingArg->getRef();
  APR_TRACE_MESSAGE("%s %s with: cmdPos=%g, encoderId=%d, refMode=%s, withoutMove=%s",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    m_cmdData.newPos, static_cast<std::int16_t>(m_cmdData.encoder),
                    axisprofileRefModeToString(m_cmdData.mode),
                    m_cmdData.withoutMove ? "true" : "false");

  if (m_cmdData.mode != RefMode::SET_POS_DEVICE)
  {
    // check only when homing by device
    return AxisProfileResult::STS_OK;
  }

  // validate encoder and get related parameter idn and property
  auto scaling = m_shareHandle->getScalingIF();
  auto deviceParamIf = m_shareHandle->getDeviceParamIF();
  if (m_cmdData.encoder == AxisProfileEncoder::ENCODER_1)
  {
    m_devData.idnRefDist = S_0_0052_REFERENCE_DISTANCE_ENCODER_1;
    m_devData.idnRefOffset = S_0_0150_REFERENCE_OFFSET_ENCODER_1;
  }
  else if (m_cmdData.encoder == AxisProfileEncoder::ENCODER_2)
  {
    m_devData.idnRefDist = S_0_0054_REFERENCE_DISTANCE_ENCODER_2;
    m_devData.idnRefOffset = S_0_0151_REFERENCE_OFFSET_ENCODER_2;
  }
  else
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_ENCODER_NOT_EXIST,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Encoder id = %d", static_cast<std::int16_t>(m_cmdData.encoder));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  // new reference position should in range of travel limit of device
  m_devData.cmdPosRaw = scaling->convertPositionScaledToRaw(m_cmdData.newPos);
  const auto& deviceCfgData = deviceParamIf->getDeviceCfgData();
  if (deviceCfgData.m_softwareLimitActive && // check only when limitation is activated in device
      (m_devData.cmdPosRaw > deviceCfgData.m_positivePosLimit || m_devData.cmdPosRaw < deviceCfgData.m_negativePosLimit))
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_OUT_OF_TRAVEL_LIMIT,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "%g out range of [S-0-0049, S-0-0050] of device", m_cmdData.newPos);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobHoming::onJobEnterActivating()
{
  // setup activating job list
  m_jobFuns.clear();
  m_jobStep = 0;
  switch (m_cmdData.mode)
  {
    case RefMode::SET_POS_DEVICE:
      m_jobFuns.emplace_back([&]() {return setupReadListFromDevice();});
      m_jobFuns.emplace_back([&]() {return exeParamListAccess();}); // read from device
      m_jobFuns.emplace_back([&]() {return validateHomingSupport();});
      m_jobFuns.emplace_back([&]() {return clearReferencePoint();});
      m_jobFuns.emplace_back([&]() {return setupWriteListToDevice();});
      m_jobFuns.emplace_back([&]() {return exeParamListAccess();}); // write to device
      m_jobFuns.emplace_back([&]() {return activeHomingByDevice();});
      break;
    case RefMode::SET_POS_PROFILE:
      m_jobFuns.emplace_back([&]() {return activeHomingByProfile();});
      break;
    default:
      break;
  }

  m_asyncSm.setCommandTimeout(0); // disable timeout check since homing movement duration is unknown
  return SercosJobBase::onJobEnterActivating();
}

AxisProfileResult SercosJobHoming::onJobActivating()
{
  return runJobsInternal();
}

AxisProfileResult SercosJobHoming::onJobEnterAborting()
{
  m_jobFuns.clear();
  m_jobStep = 0;
  switch (m_cmdData.mode)
  {
    case RefMode::SET_POS_DEVICE:
      m_jobFuns.emplace_back([&]() {return abortHomingByDevice();});
      break;
    default:
      break;
  }
  return SercosJobBase::onJobEnterAborting();
}

AxisProfileResult SercosJobHoming::onJobAborting()
{
  return runJobsInternal();
}

////////////////////////////////////////// private methods //////////////////////////////////////////// 

AxisProfileResult SercosJobHoming::setupReadListFromDevice()
{
  m_paramList.clear();
  m_paramList.emplace_back(ReadWriteType::READ, S_0_0448_SET_ABSOLUTE_POSITION_CONTROL, &m_devData.ctrlWordS448.m_word, sizeof(std::uint16_t));
  m_paramList.emplace_back(ReadWriteType::READ, S_0_0147_HOMING_PARAMETER, &m_devData.ctrlWordS147.m_word, sizeof(std::uint16_t));
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobHoming::setupWriteListToDevice()
{
  m_paramList.clear();

  // set S-0-0448.b7 to make sure homing can be executed on absolute encoder
  m_devData.ctrlWordS448.m_bits.bit7 = true;
  m_paramList.emplace_back(ReadWriteType::WRITE, S_0_0448_SET_ABSOLUTE_POSITION_CONTROL, &m_devData.ctrlWordS448, sizeof(std::uint16_t));
  m_paramList.emplace_back(ReadWriteType::WRITE, m_devData.idnRefDist, &m_devData.cmdPosRaw, sizeof(std::int32_t));
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobHoming::validateHomingSupport()
{
  if (!m_cmdData.withoutMove)
  {
    return AxisProfileResult::STS_OK; // nothing to do
  }

  // very special case when reset new reference without any movement or switch searching
  if (m_devData.ctrlWordS147.m_bits.bit5 == false ||  // homing switch evaluation: disable
      m_devData.ctrlWordS147.m_bits.bit6 == false ||  // reference mark evaluation: disable
      m_devData.ctrlWordS147.m_bits.bit9 == true  ||  // range evaluation: enable
      m_devData.ctrlWordS147.m_bits.bit10 == true)    // axis blocking evaluation: enable
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_HOMING_PARAMETER,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Please set: S-0-0147 bit5=0, bit6=0, bit9=1, bit10=1, and switch PM->OM and reboot motion");
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobHoming::clearReferencePoint()
{
  // some FW version of ctrlX drive needs clear reference point before set new reference
  auto res = m_paramAccess->executeSercosProcedureCommandAsync(ProcedureCommands::S_0_0191_CANCEL_REF_POINT, m_asyncSm);
  if (res == AxisProfileResult::STS_OK)
  {
    m_asyncSm.resetCommandState();
    m_asyncSm.switchToCmdStartStep();
  }
  return res;
}

AxisProfileResult SercosJobHoming::exeParamListAccess()
{
  return m_paramAccess->accessAndCheckParamListAsync(m_paramList, m_asyncSm.getRequestState());
}

AxisProfileResult SercosJobHoming::activeHomingByDevice()
{
  auto res = m_paramAccess->setSercosProcedureCommandAsync(m_command, m_asyncSm);
  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  }
  return res;
}

AxisProfileResult SercosJobHoming::abortHomingByDevice()
{
  auto res = m_paramAccess->clearSercosProcedureCommandAsync(m_command, m_asyncSm);
  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  }
  return res;
}

AxisProfileResult SercosJobHoming::activeHomingByProfile()
{
  auto scaling = m_shareHandle->getScalingIF();
  auto cycleData = m_shareHandle->getCyclicDataIF();

  ActualValuesDevice actValue {};
  auto res = cycleData->getActualValuesDevice(&actValue);
  if (aprStatusFailed(res))
  {
    APR_TRACE_ERROR("%s %s get actual device value failed, ErrCode=0x%X.", m_profileInfo.m_profileName.c_str(),
                    getID().c_str(), static_cast<std::uint16_t>(res));
    return res;
  }

  if (!actValue.m_dataValid)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_PROFILE_FEEDBACK_VALUE_INVALID,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  double actualPosWithoutOffset = actValue.m_actualPosition - scaling->getHomeOffset();
  double newOffset = m_cmdData.newPos - actualPosWithoutOffset;
  scaling->setHomeOffset(newOffset);

  auto deviceParamIf = m_shareHandle->getDeviceParamIF();
  deviceParamIf->setAxisHomed(true);
  return AxisProfileResult::STS_OK;
}
