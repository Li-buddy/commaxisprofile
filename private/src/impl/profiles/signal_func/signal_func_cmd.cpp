/// @file
///
/// @brief This file is part of the axisprofile library and implements the signal function command class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "signal_func_cmd.h"
#include "signal_func_handler.h"
#include "profiles/axisprofile_command_statemachine.h"

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
SignalFuncCmd::SignalFuncCmd(SignalFunction func)
  : m_signalFunc(func)
{

}

void SignalFuncCmd::initSignalFuncHandler(SignalFuncHandler* mgr)
{
  m_signalFuncHandler = mgr;
}

void SignalFuncCmd::setSignalFunc(SignalFunction func)
{
  m_signalFunc = func;
}

bool SignalFuncCmd::isSignalFuncSupported(FunctionSupportErrorInfo& errorInfo)
{
  if (!m_signalFuncHandler)
  {
    return false;
  }
  return m_signalFuncHandler->isSignalFuncSupported(m_signalFunc, errorInfo);
}

AxisProfileResult SignalFuncCmd::onJobActivating(AsyncCommandStatemachine& stm)
{
  // many result = AxisProfileResult::STS_PENDING; can be simplified
  AxisProfileResult result = AxisProfileResult::STS_PENDING;
  switch (stm.getCommandStep())
  {
    // STEP1: Clear CC bit to ensure a rising edge in the next step
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      stm.reinitCommandTick();
      auto res = m_signalFuncHandler->resetSignalFuncBit(m_signalFunc);
      if (AxisProfileResult::STS_OK == res)
      {
        stm.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    // STEP2: Set CC bit to start the command
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      auto res = m_signalFuncHandler->activeSignalFunc(m_signalFunc);
      if (AxisProfileResult::STS_OK == res)
      {
        stm.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    // STEP3: Read and check executing status, procedure finish if success
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      bool val {};
      auto res = m_signalFuncHandler->getSignalFuncState(m_signalFunc, val);
      if (AxisProfileResult::STS_OK == res && val)
      {
        result = AxisProfileResult::STS_OK;
      }
      break;
    }
    default:
      break;
  }
  return result;
}

AxisProfileResult SignalFuncCmd::onJobAborting()
{
  return m_signalFuncHandler->resetSignalFuncBit(m_signalFunc);
}

AxisProfileResult SignalFuncCmd::getSignalFuncState(bool& isActive)
{
  return m_signalFuncHandler->getSignalFuncState(m_signalFunc, isActive);
}

AxisProfileResult SignalFuncCmd::singleActiveSignalFunc()
{
  return m_signalFuncHandler->activeSignalFunc(m_signalFunc);
}
