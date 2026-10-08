/// @file
///
/// @brief This file is part of the axisprofile library and implements the axisprofile command.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_cmd.h"
#include "bundle/axisprofile_cmd_c.h"
#include "profiles/job_mgr/i_job_mgr.h"
#include "diagnosis/dia.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
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

AxisProfileCmd::AxisProfileCmd(ProfileBasicInfo& profileInfo)
  :m_profileInfo(profileInfo), m_shareHandle(nullptr)
{
  (void)memset(&m_cItf, 0, sizeof(m_cItf));
  m_cItf.handle = this;
  m_cItf.getCmdState = AXISPROFILE_getCmdState;
  m_cItf.resetCmd = AXISPROFILE_resetCmd;
  m_cItf.abortCmd = AXISPROFILE_abortCmd;
  m_cItf.getCmdStatusInDevice = AXISPROFILE_getCmdStatusInDevice;
  m_cItf.clearDeviceError = AXISPROFILE_clearDeviceErrorCmd;
  m_cItf.clearDeviceError2 = AXISPROFILE_clearDeviceError2Cmd;
  m_cItf.setAbsolutePosition = AXISPROFILE_setAbsolutePositionCmd;
  m_cItf.referencingAxis = AXISPROFILE_referencingAxisCmd;
  m_cItf.fixedStop = AXISPROFILE_fixedStopCmd;
  m_cItf.setNewReferenceWithoutMovement = AXISPROFILE_setNewReferenceWithoutMovement;
  m_cItf.switchOperationModeWithCheck = AXISPROFILE_switchOperationModeWithCheckCmd;
}

void AxisProfileCmd::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_shareHandle = handler;
}

const IAxisProfileCmdC* AxisProfileCmd::getCInterface() const
{
  return &m_cItf;
}

JobIDType AxisProfileCmd::getJob(CmdType cmdType)
{
  auto id = static_cast<std::uint16_t>(cmdType);
  if (id > CMD_MAX || id < CMD_MIN)
  {
    return JOB_UNKNOWN;
  }
  return m_jobLookupTable[id];
}

CmdState AxisProfileCmd::getCmdState(CmdType cmdType)
{
  if (!m_shareHandle || !m_shareHandle->getJobMgrIF())
  {
    return CmdState::STATE_UNKNOWN;
  }

  return m_shareHandle->getJobMgrIF()->getJobState(getJob(cmdType));
}

AxisProfileResult AxisProfileCmd::resetCmd(CmdType cmdType)
{
  if (!m_shareHandle || !m_shareHandle->getJobMgrIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  return m_shareHandle->getJobMgrIF()->sendJobCmd(getJob(cmdType), CmdReq::REQ_RESET);
}

AxisProfileResult AxisProfileCmd::abortCmd(CmdType cmdType)
{
  if (!m_shareHandle || !m_shareHandle->getJobMgrIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  return m_shareHandle->getJobMgrIF()->sendJobCmd(getJob(cmdType), CmdReq::REQ_ABORT);
}

AxisProfileResult AxisProfileCmd::startCmd(CmdType cmdType, IContainer* agr)
{
  if (!m_shareHandle || !m_shareHandle->getJobMgrIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  AxisProfileResult result = m_shareHandle->getJobMgrIF()->sendJobCmd(getJob(cmdType), CmdReq::REQ_START, agr);
  if (aprStatusFailed(result))
  {
    APR_TRACE_ERROR("%s; start %s failed",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    cmdTypeToString(cmdType));
    return result;
  }

  APR_TRACE_MESSAGE("%s; command %s started", m_profileInfo.m_profileTrace.createTraceMsg(), cmdTypeToString(cmdType));
  return AxisProfileResult::STS_OK;
}

CmdStatusInDevice AxisProfileCmd::getCmdStatusInDevice(CmdType cmdType)
{
  if (!m_shareHandle || !m_shareHandle->getJobMgrIF())
  {
    return CmdStatusInDevice::DEVICE_UNKNOWN;
  }

  auto jobObj = m_shareHandle->getJobMgrIF()->getJobObj(getJob(cmdType));
  if (!jobObj)
  {
    APR_TRACE_ERROR("%s; %s don't have a job",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    cmdTypeToString(cmdType));

    return CmdStatusInDevice::DEVICE_UNKNOWN;
  }

  // the command status in device is only updated when command is working in axisprofile
  auto cmdState = jobObj->getState();
  if (cmdState != CmdState::STATE_IDLE &&
      cmdState != CmdState::STATE_ACTIVATING &&
      cmdState != CmdState::STATE_ACTIVE &&
      cmdState != CmdState::STATE_ABORTING)
  {
    APR_TRACE_ERROR("%s; %s is not executing",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    cmdTypeToString(cmdType));

    return CmdStatusInDevice::DEVICE_UNKNOWN;
  }

  auto deviceStatus = JobDataDeviceStatus::safeCast(jobObj->getJobData());
  if (!deviceStatus)
  {
    APR_TRACE_ERROR("%s; %s don't have a device status",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    cmdTypeToString(cmdType));

    return CmdStatusInDevice::DEVICE_UNKNOWN;
  }

  return deviceStatus->get().load();
}

AxisProfileResult AxisProfileCmd::clearDeviceError(CmdType& cmdType)
{
  cmdType = CmdType::CLEAR_DEV_ERR;
  return startCmd(cmdType);
}

AxisProfileResult AxisProfileCmd::setAbsolutePosition(double newPos, AxisProfileEncoder encoder, CmdType& cmdType)
{
  ArgSetAbsolutePosition arg({newPos, encoder, true});
  cmdType = CmdType::SET_ABS_POS;
  return startCmd(cmdType, &arg);
}

AxisProfileResult AxisProfileCmd::referencingAxis(double newPos, AxisProfileEncoder encoder, CmdType& cmdType)
{
  ArgHoming arg({newPos, encoder, RefMode::SET_POS_DEVICE, false});
  cmdType = CmdType::REFERENCE_POS;
  return startCmd(cmdType, &arg);
}

AxisProfileResult AxisProfileCmd::fixedStop(double clampingTorque, CmdType& cmdType)
{
  ArgFixedStop arg(clampingTorque);
  cmdType = CmdType::FIXED_STOP;
  return startCmd(cmdType, &arg);
}

AxisProfileResult AxisProfileCmd::clearDeviceError2()
{
  // reset all other commands before clear device error
  auto retValue = AxisProfileResult::STS_OK;
  if (AxisProfileCategory::CATEGORY_ENCODER == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
    return retValue;
  }

  for (const auto& cmd : m_resetCmds)
  {
    if (getCmdState(cmd) == CmdState::STATE_ERROR)
    {
      retValue = AxisProfileResult::STS_PENDING;
      resetCmd(cmd); // always success
    }
  }
  if (retValue != AxisProfileResult::STS_OK)
  {
    return retValue;
  }

  CmdType cmdType = CmdType::CLEAR_DEV_ERR;
  CmdStatusInDevice cmdStatusInDev {};
  auto cmdState = getCmdState(cmdType);
  if (cmdState == CmdState::STATE_IDLE)
  {
    // profile command is idle and can be triggered
    auto res = clearDeviceError(cmdType);
    if (aprStatusFailed(res))
    {
      return AxisProfileResult::ERR_COMMAND_EXECUTION;
    }
  }
  else if (cmdState == CmdState::STATE_ACTIVE)
  {
    // wait until reset has completed in device
    cmdStatusInDev = getCmdStatusInDevice(cmdType);
    if (cmdStatusInDev == CmdStatusInDevice::DEVICE_COMPLETED)
    {
      // trigger abort reset, axisprofile handle following precess itself
      // can't be failed
      abortCmd(cmdType);
      return AxisProfileResult::STS_OK;
    }
  }

  // error handling
  if (cmdState == CmdState::STATE_ERROR || cmdStatusInDev == CmdStatusInDevice::DEVICE_ERROR)
  {
    resetCmd(cmdType);
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_STATUS_ERROR,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Trigger Reset again please");
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  if (ENUM_TO_I16(cmdState) > STATE_MAX || ENUM_TO_I16(cmdState) < STATE_MIN ||
      cmdStatusInDev > CmdStatusInDevice::DEVICE_INTERRUPTED || cmdStatusInDev < CmdStatusInDevice::DEVICE_NOT_SET)
  {
    resetCmd(cmdType);
    APR_TRACE_ERROR("%s; get unknown state, cmdState %d, statusInDev %d",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    static_cast<std::int16_t>(cmdType),
                    static_cast<std::int16_t>(cmdStatusInDev));
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  return AxisProfileResult::STS_PENDING;
}

AxisProfileResult AxisProfileCmd::setNewReferenceWithoutMovement(double newPos, AxisProfileEncoder encoder, RefMode refMode, CmdType& cmdType)
{
  ArgHoming arg({newPos, encoder, refMode, true});
  cmdType = CmdType::SET_NEW_REF_WITHOUT_MOVE;
  return startCmd(cmdType, &arg);
}

AxisProfileResult AxisProfileCmd::switchOperationModeWithCheck(AxisProfileOperationMode opMode)
{
  CmdType cmdType = CmdType::SET_OPERATION_MODE_WITH_CHECK;
  CmdStatusInDevice cmdStatusInDev {};
  auto cmdState = getCmdState(cmdType);
  if (cmdState == CmdState::STATE_IDLE)
  {
    // profile command is idle and can be triggered
    ArgSetOperationMode arg({opMode});
    auto res = startCmd(cmdType, &arg);
    if (aprStatusFailed(res))
    {
      return AxisProfileResult::ERR_COMMAND_EXECUTION;
    }
  }
  else if (cmdState == CmdState::STATE_ACTIVE)
  {
    // wait until the requested operation mode has been confirmed by the device
    cmdStatusInDev = getCmdStatusInDevice(cmdType);
    if (cmdStatusInDev == CmdStatusInDevice::DEVICE_COMPLETED)
    {
      // trigger abort; the axisprofile job manager completes the cleanup
      // can't be failed
      abortCmd(cmdType);
      return AxisProfileResult::STS_OK;
    }    
  }
  // error handling
  if (cmdState == CmdState::STATE_ERROR || cmdStatusInDev == CmdStatusInDevice::DEVICE_ERROR)
  {
    resetCmd(cmdType);
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_STATUS_ERROR,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Trigger the operation mode switching again please");
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  if (ENUM_TO_I16(cmdState) > STATE_MAX || ENUM_TO_I16(cmdState) < STATE_MIN ||
      cmdStatusInDev > CmdStatusInDevice::DEVICE_INTERRUPTED || cmdStatusInDev < CmdStatusInDevice::DEVICE_NOT_SET)
  {
    resetCmd(cmdType);
    APR_TRACE_ERROR("%s; get unknown state, cmdState %d, statusInDev %d",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    static_cast<std::int16_t>(cmdType),
                    static_cast<std::int16_t>(cmdStatusInDev));
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  return AxisProfileResult::STS_PENDING;
}
