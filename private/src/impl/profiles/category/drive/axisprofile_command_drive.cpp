/// @file
///
/// @brief This source file is part of the axisprofile library and implements
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
#include "axisprofile_command_drive.h"
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

/// @brief This method converts command state to old type, will be removed after all commands migrated
/// @param[in]  state     Command state
/// @return Old type of command state

// LCOV_EXCL_START
inline AxisprofileCommandState cmdStateToOldState(CmdState state)
{
  switch (state)
  {
    case comm::axisprofile::CmdState::STATE_IDLE:
      return AxisprofileCommandState::CMD_STATE_IDLE;
    case comm::axisprofile::CmdState::STATE_ACTIVATING:
    case comm::axisprofile::CmdState::STATE_ACTIVE:
      return AxisprofileCommandState::CMD_STATE_ACTIVE;
    case comm::axisprofile::CmdState::STATE_ABORTING:
      return AxisprofileCommandState::CMD_STATE_ABORTING;
    case comm::axisprofile::CmdState::STATE_ERROR:
      return AxisprofileCommandState::CMD_STATE_ERROR;
    default:
      return AxisprofileCommandState::CMD_STATE_UNDEFINED;
  }
  return AxisprofileCommandState::CMD_STATE_UNDEFINED;
}
// LCOV_EXCL_STOP This is temporary solution, so will be note tested 

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

AxisProfileCommandDrive::AxisProfileCommandDrive(ProfileBasicInfo& profileBasicInfo)
  :AxisProfileCommand(profileBasicInfo), m_cmdData {}
{
  
}

AxisProfileResult AxisProfileCommandDrive::resetAllAxisprofileCommands()
{
  for (auto& it : m_asyncStates)
  {
    it.second->resetCommandState();
  }

  return AxisProfileResult::STS_OK; // should always success
}

AxisProfileResult AxisProfileCommandDrive::resetAxisProfileCommandState(const AxisProfileCommandType type)
{
  auto it = m_asyncStates.find(type);
  if (it == m_asyncStates.end())
  {
    APR_TRACE_ERROR(m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND, axisProfileCommandTypeToString(type)));
    return AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND;
  }

  it->second->resetCommandState();

  APR_TRACE_MESSAGE("%s; command of %s reset on %s.", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileCommandTypeToString(type), m_profileBasicInfo.m_profileName.c_str());
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCommandDrive::getCommandsAtDeterminedState(const AxisprofileCommandState cmdState, std::vector<AxisProfileCommandType>& activeCmds)
{
  activeCmds.clear();
  for (auto& type : m_asyncStates)
  {
    if (cmdState == type.second->getAxisProfileCommandState())
    {
      activeCmds.emplace_back(type.first);
    }
  }

  // adaption for new command interface, will be removed after all commands migrated
  // LCOV_EXCL_START
  auto newCmdIf = m_sharingHandler->getCmdIF();
  for (size_t cmd = 0; cmd < CMD_MAX; cmd++)
  {
    auto state = newCmdIf->getCmdState(static_cast<CmdType>(cmd));
    if (cmdStateToOldState(state) == cmdState)
    {
      std::uint16_t cmdIdxBase = static_cast<std::uint16_t>(AxisProfileCommandType::CMD_MAX);
      activeCmds.emplace_back(static_cast<AxisProfileCommandType>(cmdIdxBase + cmd));
    }
  }
  // LCOV_EXCL_STOP This is temporary solution will be removed after all command migrated

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCommandDrive::abortAxisProfileCommand(const AxisProfileCommandType type)
{
  auto it = m_asyncStates.find(type);
  if (it == m_asyncStates.end())
  {
    APR_TRACE_ERROR(m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND, axisProfileCommandTypeToString(type)));
    return AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND;
  }

  // only active command can be aborted
  if (AxisprofileCommandState::CMD_STATE_ACTIVE == it->second->getAxisProfileCommandState())
  {
    it->second->switchToAbortStep();
  }

  APR_TRACE_MESSAGE("%s; command of %s abort on %s.", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileCommandTypeToString(type), m_profileBasicInfo.m_profileName.c_str());
  return AxisProfileResult::STS_OK;
}

// LCOV_EXCL_START These functions have been deprecated but have not yet been removed
void AxisProfileCommandDrive::resetHomeOffset()
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return;
  }

  if (m_sharingHandler->getScalingIF()->getHomeOffset())
  {
    m_sharingHandler->getScalingIF()->setHomeOffset(0);
  }
  APR_TRACE_MESSAGE("Reset the offset of %s to %g", m_profileBasicInfo.m_profileName.c_str(), m_sharingHandler->getScalingIF()->getHomeOffset());
}

//////////////////////////////////////////////////setter and getter/////////////////////////////////////////////////////

void AxisProfileCommandDrive::setMethod(RefMode mode)
{
  m_refMode = mode;
}

RefMode AxisProfileCommandDrive::getRefMode() const
{
  return m_refMode;
}

void AxisProfileCommandDrive::setHomeOffset(double newRefPos, double deviceActualPos)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return;
  }
  double offset = newRefPos - deviceActualPos;
  m_sharingHandler->getScalingIF()->setHomeOffset(offset);
}
// LCOV_EXCL_STOP - exclude code coverage section end
