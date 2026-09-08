/// @file
///
/// @brief This file is part of the axisprofile library and implements the job clear device error.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "sercos_job_reset.h"

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

SercosJobReset::SercosJobReset(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle)
  : SercosJobBase(ProcedureCommands::S_0_0099_RESET_CLASS_1_DIAGNOSTIC, profileInfo, shareHandle),
  m_statusControlIf(m_shareHandle->getStatusControlIF())
{
   
}

AxisProfileResult SercosJobReset::onJobEnterActivating()
{
  SignalFuncHandler* signalFuncHandler {};
  getSignalFuncHandler(signalFuncHandler);
  m_signalFuncCmd.initSignalFuncHandler(signalFuncHandler);

  FunctionSupportErrorInfo errorInfo {};
  // nullptr handled inside the support check
  m_isSignalFuncCmd = m_signalFuncCmd.isSignalFuncSupported(errorInfo);
  APR_TRACE_MESSAGE(m_isSignalFuncCmd ? "reset command : handled as signal function." : "reset command : handled as normal command.");
  return SercosJobBase::onJobEnterActivating();
}

AxisProfileResult SercosJobReset::onJobActivating()
{
  AxisProfileResult res {AxisProfileResult::STS_PENDING};
  if(m_isSignalFuncCmd)
  {
    res = m_signalFuncCmd.onJobActivating(m_asyncSm);
  }
  else
  {
    res = m_paramAccess->setSercosProcedureCommandAsync(m_command, m_asyncSm);
  }
  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  }
  return res;
}

AxisProfileResult SercosJobReset::onJobAborting()
{
  AxisProfileResult res {AxisProfileResult::STS_PENDING};
  if (m_isSignalFuncCmd)
  {
    res = m_signalFuncCmd.onJobAborting();
  }
  else
  {
    res = m_paramAccess->clearSercosProcedureCommandAsync(m_command, m_asyncSm);
  }

  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  }
  return res;
}

AxisProfileResult SercosJobReset::onUpdateStatusInDevice()
{
  // we check device error flag of status word to indicate command completed, which is faster and stable.
  if (m_deviceStatus.get() == CmdStatusInDevice::DEVICE_IN_PROCESS)
  {
    // device has no error, we can treat command completed
    if (!m_statusControlIf->checkError())
    {
      m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
      APR_TRACE_MESSAGE(m_isSignalFuncCmd ? "reset command : finished as signal function." : "reset command : finished as normal command.");
    }
  }
  return AxisProfileResult::STS_OK;
}
