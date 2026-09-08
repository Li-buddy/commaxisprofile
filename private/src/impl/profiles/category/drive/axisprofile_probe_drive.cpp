/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for probe handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_probe_drive.h"
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

AxisProfileProbeDrive::AxisProfileProbeDrive(ProfileBasicInfo& profileBasicInfo)
  :AxisProfileProbe(profileBasicInfo)
{
  delete m_asyncStates;
  m_asyncStates = new AsyncCommandStatemachine;
}

AxisProfileProbeDrive::~AxisProfileProbeDrive() noexcept
{
  resetProbeCommand();
  delete m_asyncStates;
  m_asyncStates = nullptr;
}

AxisProfileResult AxisProfileProbeDrive::raiseAndGetStatemachine(ProbeIndex probeIdx, AsyncCommandStatemachine*& asyncState)
{
  if (!m_profileBasicInfo.m_deviceAddress)
  {
    APR_TRACE_ERROR(m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_EXEC_NULL, "Command device address not set."));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  AsyncCommandStatemachine::CommandStep oldStep = m_asyncStates->getCommandStep();
  if (!m_asyncStates->raiseAndCheckThreadLocker())
  {
    APR_TRACE_WARNING("%s, %s", m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_COMMAND_LOCKED));
    asyncState = nullptr;
    return AxisProfileResult::ERR_COMMAND_LOCKED;
  }

  // request probe is different working probe, pending
  if (m_workingProbe != ProbeIndex::PROBE_UNDEFINED && m_workingProbe != probeIdx)
  {
    return AxisProfileResult::STS_PENDING;
  }

  asyncState = m_asyncStates;

  // trace only in first raised
  if (AsyncCommandStatemachine::CommandStep::PARAM_INIT == asyncState->getCommandStep() &&
      oldStep == AsyncCommandStatemachine::CommandStep::STEP_IDLE &&
      m_workingProbe == ProbeIndex::PROBE_UNDEFINED) // no working probe
  {
    m_workingProbe = probeIdx;
    APR_TRACE_MESSAGE("%s; probe asynchronous command raised.", m_profileBasicInfo.m_profileTrace.createTraceMsg());
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileProbeDrive::resultHandlingCycle(AsyncCommandStatemachine& asyncState, AxisProfileResult& result)
{
  switch (result)
  {
    case AxisProfileResult::STS_OK:
      resetProbeCommand();
      APR_TRACE_MESSAGE("%s; asynchronous probe command done and released.", m_profileBasicInfo.m_profileTrace.createTraceMsg());
      break;

    case AxisProfileResult::STS_PENDING:
      if (asyncState.tickCommandAndCheckExceed()) // timeout check
      {
        Dia::sendDiagnosis(MAIN_DIAG_PROFILE_ERR, DETAILED_DIAG_PROFILE_CMD_TIMEOUT, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "Timeout of asynchronous probe command");
        result = AxisProfileResult::ERR_TIMEOUT;
      }
      break;

    default:
      if (aprStatusFailed(result))
      {
        resetProbeCommand();
        APR_TRACE_MESSAGE("%s; command asynchronous probe command failed and released.", m_profileBasicInfo.m_profileTrace.createTraceMsg());
      }
  }
  return result;
}

AxisProfileResult AxisProfileProbeDrive::resetProbeCommand()
{
  m_asyncStates->resetCommandState();
  m_workingProbe = ProbeIndex::PROBE_UNDEFINED;
  return AxisProfileResult::STS_OK; // should always success
}

bool AxisProfileProbeDrive::itemListChecker(ProbeIndex probeIdx, std::vector<EnableCheckItem>& checkList) const
{
  bool res = true;
  for (const auto& itr : checkList)
  {
    // the item is enabled by control word but precondition is not full filled
    if (itr.m_itemEnabled && !itr.m_preconditionFullfill)
    {
      Dia::sendDiagnosis(MAIN_DIAG_PROBE_ERR, DETAILED_DIAG_PROBE_CMD_EXECUTE_FAILED,
                         m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "%s can't %s, %s missing, Parameter %s", probeIndexToString(probeIdx),
                         itr.m_itemName.c_str(), itr.m_precondition.c_str(), itr.m_addTxt.c_str());
      res = false;
    }
  }
  checkList.clear();
  return res;
}
