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

  m_asyncSm.resetCommandState();
  m_asyncSm.switchToCmdStartStep();
  m_controlWord.m_word = 0;
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  return CanopenJobBase::onJobEnterActivating();
}

AxisProfileResult CanopenJobReset::onJobActivating()
{
  m_statusWord.m_word = m_statusControlIf->getStatusWord();
  if (!m_statusWord.m_bits.bit3)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
    return AxisProfileResult::STS_OK;
  }
  AxisProfileResult result {AxisProfileResult::STS_PENDING};
  switch (m_asyncSm.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      m_asyncSm.reinitCommandTick();
      m_controlWord.m_bits.bit7 = 0;
      m_statusControlIf->setControlWord(m_controlWord.m_word);
      m_asyncSm.switchToNextStep();
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      m_controlWord.m_bits.bit7 = 1;
      m_statusControlIf->setControlWord(m_controlWord.m_word);
      m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
      m_asyncSm.switchToNextStep();
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      m_statusWord.m_word = m_statusControlIf->getStatusWord();
      if (!m_statusWord.m_bits.bit3)
      {
        m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
        result = AxisProfileResult::STS_OK;
      }
      break;
    }
    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
    default:
    {
      APR_TRACE_ERROR("invalid state to execute Command.");
      result = AxisProfileResult::ERR_INVALID_STATE;
    }
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}

AxisProfileResult CanopenJobReset::onJobAborting()
{
  m_controlWord.m_word = m_statusControlIf->getControlWord();
  m_controlWord.m_bits.bit7 = 0;
  m_statusControlIf->setControlWord(m_controlWord.m_word);
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);

  return AxisProfileResult::STS_OK; 
}
