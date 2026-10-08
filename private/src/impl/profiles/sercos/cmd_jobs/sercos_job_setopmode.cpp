/// @file
///
/// @brief This file is part of the axisprofile library and implements the Sercos operation-mode command job.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "sercos_job_setopmode.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

SercosJobSetOpMode::SercosJobSetOpMode(ProfileBasicInfo& profileInfo,
                                       IAxisProfileSharingHandler* shareHandle)
  : AxisprofileJobBase(profileInfo, shareHandle),
    m_statusControlIf(shareHandle ? shareHandle->getStatusControlIF() : nullptr)
{
}

AxisProfileResult SercosJobSetOpMode::onSetArgument(IContainer* arg)
{
  auto setOpModeArg = ArgSetOperationMode::safeCast(arg);
  if (!setOpModeArg)
  {
    APR_TRACE_ERROR("%s %s gets wrong type of argument",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_JOB_DATA_MISMATCH;
  }

  if (!m_statusControlIf)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  const AxisProfileOperationMode requestedMode = setOpModeArg->get().mode;
  OpModeErrorInfo errorInfo {};
  if (!m_statusControlIf->checkOperationModeSupported(requestedMode, errorInfo))
  {
    return AxisProfileResult::ERR_NOK;
  }

  m_requestedOperationMode = requestedMode;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobSetOpMode::onJobEnterActivating()
{
  if (!m_statusControlIf)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // Operation mode is encoded into the Sercos MDT controlword by the
  // fieldbus-specific StatusControl implementation. This keeps the existing
  // bit layout and IPO synchronization-bit handling in one place.
  const AxisProfileResult result = m_statusControlIf->setOperationMode(m_requestedOperationMode);
  if (aprStatusFailed(result))
  {
    return result;
  }

  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  return JobObj::onJobEnterActivating();
}

AxisProfileResult SercosJobSetOpMode::onJobActivating()
{
  if (!m_statusControlIf)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  const AxisProfileOperationMode actualOperationMode = m_statusControlIf->getOperationMode();
  if (actualOperationMode != m_requestedOperationMode)
  {
    return AxisProfileResult::STS_PENDING;
  }

  // Sercos AT feedback has confirmed the requested mode. Keep the confirmed
  // cache in StatusControl before publishing command completion.
  m_statusControlIf->confirmOperationModeFromDevice(actualOperationMode);
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobSetOpMode::onJobEnterAborting()
{
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  return JobObj::onJobEnterAborting();
}
