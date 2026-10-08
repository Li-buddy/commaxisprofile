/// @file
///
/// @brief This file is part of the axisprofile library and implements the reset job of CANOpen.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_reset.h"

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

CanopenJobReset::CanopenJobReset(ProfileBasicInfo& profileInfo,
                                 IAxisProfileSharingHandler* shareHandle)
  : CanopenJobBase(profileInfo, shareHandle)
{
}

AxisProfileResult CanopenJobReset::onJobEnterActivating()
{
  // clear FPU error state
  if (!m_shareHandle || !m_shareHandle->getCyclicDataIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  m_shareHandle->getCyclicDataIF()->resetFPUFlag();

  m_controlWord.m_word = 0;
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);

  getSignalFuncHandler(m_signalFuncHandler);
  FunctionSupportErrorInfo errorInfo {};
  m_isSignalFuncCmd = m_signalFuncHandler->isFuncInSignalStatusWord(m_signalFunc, errorInfo);
  APR_TRACE_MESSAGE(m_isSignalFuncCmd ? "reset command : handled as signal function." : "reset command : handled as normal command.");

  return CanopenJobBase::onJobEnterActivating();
}

AxisProfileResult CanopenJobReset::executeCmdBySignalFunc(SignalFunction sFunc, AsyncCommandStatemachine& stm)
{
  AxisProfileResult result {AxisProfileResult::STS_PENDING};
  switch (stm.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      stm.reinitCommandTick();
      m_controlWord.m_bits.bit7 = 0;
      m_statusControlIf->setControlWord(m_controlWord.m_word);
      stm.switchToNextStep();
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      m_controlWord.m_bits.bit7 = 1;
      m_statusControlIf->setControlWord(m_controlWord.m_word);
      stm.switchToNextStep();
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    {
      bool val {};
      auto res = m_signalFuncHandler->getSignalFuncState(sFunc, val);
      m_statusWord.m_word = m_statusControlIf->getStatusWord();
      if (AxisProfileResult::STS_OK == res && (val || !m_statusWord.m_bits.bit3))
      {
        stm.switchToNextStep();
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      m_controlWord.m_bits.bit7 = 0;
      m_statusControlIf->setControlWord(m_controlWord.m_word);
      result = AxisProfileResult::STS_OK;
      break;
    }
    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
    default:
    {
      APR_TRACE_ERROR("invalid state to execute Command.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}

AxisProfileResult CanopenJobReset::executeCmdByParamOperation(AsyncCommandStatemachine& stm)
{
  AxisProfileResult result {AxisProfileResult::STS_PENDING};
  switch (stm.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      stm.reinitCommandTick();
      m_controlWord.m_bits.bit7 = 0;
      m_statusControlIf->setControlWord(m_controlWord.m_word);
      stm.switchToNextStep();
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      m_controlWord.m_bits.bit7 = 1;
      m_statusControlIf->setControlWord(m_controlWord.m_word);
      stm.switchToNextStep();
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    {
      m_statusWord.m_word = m_statusControlIf->getStatusWord();
      if (!m_statusWord.m_bits.bit3)
      {
        stm.switchToNextStep();
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      m_controlWord.m_bits.bit7 = 0;
      m_statusControlIf->setControlWord(m_controlWord.m_word);
      result = AxisProfileResult::STS_OK;
      break;
    }
    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
    default:
    {
      APR_TRACE_ERROR("invalid state to execute Command.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}

AxisProfileResult CanopenJobReset::onJobActivating()
{
  AxisProfileResult res {AxisProfileResult::STS_PENDING};
  if (m_isSignalFuncCmd)
  {
    res = executeCmdBySignalFunc(m_signalFunc, m_asyncSm);
  }
  else
  {
    res = executeCmdByParamOperation(m_asyncSm);
  }
  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  }
  return res;
}

AxisProfileResult CanopenJobReset::onJobAborting()
{
  m_controlWord.m_word = m_statusControlIf->getControlWord();
  m_controlWord.m_bits.bit7 = 0;
  m_statusControlIf->setControlWord(m_controlWord.m_word);
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);

  return AxisProfileResult::STS_OK; 
}

AxisProfileResult CanopenJobReset::onUpdateStatusInDevice()
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
