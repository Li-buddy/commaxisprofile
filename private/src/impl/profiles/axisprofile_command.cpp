/// @file
///
/// This source file is part of the axisprofile library and implements
/// base class for command handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_command.h"
#include "bundle/axisprofile_command_c.h"
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

AxisProfileCommand::AxisProfileCommand(ProfileBasicInfo& profileBasicInfo)
  :m_profileBasicInfo(profileBasicInfo), m_sharingHandler(nullptr)
{
  // *** Initialize C-Interface *** //
  (void)memset(&m_cInterface, 0, sizeof(m_cInterface));
  m_cInterface.handle = this;
  m_cInterface.clearDeviceError = AXISPROFILE_clearDeviceError;
  m_cInterface.setAndCheckPositiveStop = AXISPROFILE_setAndCheckPositiveStop;
  m_cInterface.setAndCheckClampingTorque = AXISPROFILE_setAndCheckClampingTorque;
  m_cInterface.getClampingTorque = AXISPROFILE_getClampingTorqueDeprecated;
  m_cInterface.checkStandStill = AXISPROFILE_checkStandStill;
  m_cInterface.setAndCheckStandStillWindow = AXISPROFILE_setAndCheckstandStillWindow;
  m_cInterface.getStandStillWindow = AXISPROFILE_getStandStillWindow;
  m_cInterface.activateCyclicStandStillCheck = AXISPROFILE_activateCyclicStandStillCheck;
  m_cInterface.deactivateCyclicStandStillCheck = AXISPROFILE_deactivateCyclicStandStillCheck;
  m_cInterface.setAbsolutePosition = AXISPROFILE_setAbsolutePosition;
  m_cInterface.resetAbsolutePosition = AXISPROFILE_resetAbsolutePosition;
  m_cInterface.referencingAxis = AXISPROFILE_referencingAxis;
  m_cInterface.resetReferencingAxis = AXISPROFILE_resetReferencingAxis;
  m_cInterface.checkAxisHomed = AXISPROFILE_checkAxisHomedDeprecated;
  m_cInterface.resetAxisProfileCommandState = AXISPROFILE_resetAxisProfileCommandState;
  m_cInterface.abortAxisProfileCommand = AXISPROFILE_abortAxisProfileCommand;
  m_cInterface.getAxisProfileCommandState = AXISPROFILE_getAxisProfileCommandState;
  m_cInterface.setReferencingPosition = AXISPROFILE_setReferencingPosition;
  m_cInterface.getCommandsAtDeterminedState = AXISPROFILE_getCommandsAtDeterminedState;

  m_asyncStates.clear();
  for (std::uint32_t i = static_cast<std::uint32_t>(AxisProfileCommandType::CMD_MIN); i < static_cast<std::uint32_t>(AxisProfileCommandType::CMD_MAX); i++)
  {
    m_asyncStates.emplace(static_cast<AxisProfileCommandType>(i), new AsyncCommandStatemachine);
  }
}

AxisProfileCommand::~AxisProfileCommand() noexcept
{
  for (auto& it : m_asyncStates)
  {
    if (it.second)
    {
      delete it.second;
      it.second = nullptr;
    }
  }
  m_asyncStates.clear();
}

const IAxisProfileCommandC* AxisProfileCommand::getCInterface() const
{
  return &m_cInterface;
}

AxisProfileResult AxisProfileCommand::booting()
{
  loadBootingTasks();
  AxisProfileResult result {AxisProfileResult::STS_OK};
  for (const auto& method : m_initCommands)
  {
    result = method();
    if (aprStatusNotOkay(result))
    {
      return result;
    }
  }
  return result;
}

void AxisProfileCommand::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

AsyncCommandStatemachine* AxisProfileCommand::getAsyncStatemachine(const AxisProfileCommandType type)
{
  auto it = m_asyncStates.find(type);
  if (it == m_asyncStates.end())
  {
    APR_TRACE_ERROR(m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND, axisProfileCommandTypeToString(type)));
    return nullptr;
  }

  return it->second;
}

AxisProfileResult AxisProfileCommand::raiseAndGetStatemachine(const AxisProfileCommandType type, AsyncCommandStatemachine*& asyncState)
{
  if (!m_profileBasicInfo.m_deviceAddress)
  {
    APR_TRACE_ERROR(m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_EXEC_NULL, "Command device address not set."));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto it = m_asyncStates.find(type);
  if (it == m_asyncStates.end())
  {
    APR_TRACE_ERROR(m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND, axisProfileCommandTypeToString(type)));
    asyncState = nullptr;
    return AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND;
  }

  AsyncCommandStatemachine::CommandStep oldStep = it->second->getCommandStep();
  if (!it->second->raiseAndCheckThreadLocker())
  {
    APR_TRACE_WARNING("%s, %s", m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_COMMAND_LOCKED), axisProfileCommandTypeToString(type));
    asyncState = nullptr;
    return AxisProfileResult::ERR_COMMAND_LOCKED;
  }

  asyncState = it->second;

  // trace only in first raised
  if (AsyncCommandStatemachine::CommandStep::PARAM_INIT == asyncState->getCommandStep() && oldStep == AsyncCommandStatemachine::CommandStep::STEP_IDLE)
  {
    APR_TRACE_MESSAGE("%s; command %s raised.", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileCommandTypeToString(type));
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCommand::resultHandlingCycle(const AxisProfileCommandType type, AsyncCommandStatemachine& asyncState, AxisProfileResult& result)
{
  switch (result)
  {
    case AxisProfileResult::STS_OK:
      asyncState.resetCommandState();
      APR_TRACE_MESSAGE("%s; command %s done and released.", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileCommandTypeToString(type));
      break;

    case AxisProfileResult::STS_PENDING:
      if (asyncState.tickCommandAndCheckExceed()) // timeout check
      {
        Dia::sendDiagnosis(MAIN_DIAG_PROFILE_ERR, DETAILED_DIAG_PROFILE_CMD_TIMEOUT, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "Timeout of command %s", axisProfileCommandTypeToString(type));
        result = AxisProfileResult::ERR_TIMEOUT;
      }
      break;

    default:
      if (aprStatusFailed(result))
      {
        asyncState.resetCommandState();
        APR_TRACE_MESSAGE("%s; command %s failed and released.", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileCommandTypeToString(type));
      }
  }
  return result;
}

AxisprofileCommandState AxisProfileCommand::getAxisProfileCommandState(const AxisProfileCommandType type)
{
  auto it = m_asyncStates.find(type);
  if (it == m_asyncStates.end())
  {
    APR_TRACE_ERROR(m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND, axisProfileCommandTypeToString(type)));
    return AxisprofileCommandState::CMD_STATE_UNDEFINED;
  }

  return it->second->getAxisProfileCommandState();
}

AxisProfileResult AxisProfileCommand::checkAxisHomed(bool& /*isHomed*/)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_CHECK_AXIS_HOMED));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}
