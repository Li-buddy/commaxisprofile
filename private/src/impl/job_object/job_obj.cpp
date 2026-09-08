/// @file
///
/// @brief This file is part of the axisprofile library and implements the job object.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "job_obj.h"
#include <string>

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

JobObj::JobObj() : m_stateChangged(false), m_state(JobState::STATE_IDLE),
m_oldState(JobState::STATE_IDLE), m_oldStateGetErr(JobState::STATE_IDLE)
{}

AxisProfileResult JobObj::setNewState(JobReq newReq, std::string& infoTxt, IContainer* arg)
{
  if (static_cast<std::int16_t>(newReq) >= static_cast<std::uint16_t>(JobReq::REQ_MAX) ||
      static_cast<std::int16_t>(newReq) < static_cast<std::uint16_t>(JobReq::REQ_MIN))
  {
    infoTxt = "invalid new request";
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  const auto& stateSet = obj::REQ_ALLOWED_MAP.find(newReq)->second;
  if (stateSet.find(m_state.load()) == stateSet.end())
  {
    infoTxt = std::string("new request ") + jobReqToStr(newReq) +
              std::string(" is not allowed in state ") + jobStateToStr(m_state.load());
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  switch (newReq)
  {
    case JobReq::REQ_RESET:
      onReset();
      break;

    case JobReq::REQ_START:
      if (aprStatusFailed(onSetArgument(arg)))
      {
        infoTxt = "invalid arguments";
        return AxisProfileResult::ERR_INVALID_ARGUMENT;
      }
      m_state.store(JobState::STATE_ACTIVATING);
      break;

    case JobReq::REQ_ABORT:
      m_state.store(JobState::STATE_ABORTING);
      break;

    default:
      break;
  }
  return AxisProfileResult::STS_OK;
}

JobState JobObj::getState()
{
  return m_state.load();
}

AxisProfileResult JobObj::mainCycle()
{
  m_stateChangged = false;
  if (m_oldState != m_state.load())
  {
    onJobStateChanged(m_state.load(), m_oldState);
    m_stateChangged = true;
    m_oldState = m_state.load();
  }
  
  AxisProfileResult result {};
  switch (m_state.load())
  {
    case JobState::STATE_IDLE:
    case JobState::STATE_ERROR:
      // nothing to do
      break;

    case JobState::STATE_ACTIVE:
      result = onJobInActived();
      break;

    case JobState::STATE_ACTIVATING:
      if (m_stateChangged) // first enter cycle
      {
        result = onJobEnterActivating();
        if (aprStatusFailed(result))
        {
          break;
        }
      }
      result = internalStateCycle([&]{return onJobActivating();}, JobState::STATE_ACTIVE);
      break;

    case JobState::STATE_ABORTING:
      if (m_stateChangged) // first enter cycle
      {
        result = onJobEnterAborting();
        if (aprStatusFailed(result))
        {
          break;
        }
      }
      result = internalStateCycle([&]{return onJobAborting();}, JobState::STATE_IDLE);
      break;

    default:
      return AxisProfileResult::ERR_INVALID_STATE;
  }

  if (aprStatusFailed(result))
  {
    m_oldStateGetErr = m_state.load();
    m_state.store(JobState::STATE_ERROR);
  }

  return AxisProfileResult::STS_OK;
}

const IContainer* JobObj::getJobData()
{
  return nullptr; // no data available
}

IContainer* JobObj::getJobDataRef()
{
  return nullptr; // no data available
}

// ---------------------------------------------------------------------------
// protected methods to be overridden by derived classes
// ---------------------------------------------------------------------------

AxisProfileResult JobObj::onSetArgument(IContainer* arg)
{
  (void)arg;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult JobObj::onJobEnterActivating()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult JobObj::onJobActivating()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult JobObj::onJobInActived()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult JobObj::onJobEnterAborting()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult JobObj::onJobAborting()
{
  return AxisProfileResult::STS_OK;
}

void JobObj::onReset()
{
  // if the job get error when active or activating
  // we should abort it first, it go to idle automatically after aborting
  if (m_oldStateGetErr == JobState::STATE_ACTIVE ||
      m_oldStateGetErr == JobState::STATE_ACTIVATING)
  {
    m_state.store(JobState::STATE_ABORTING);
  }
  else
  {
    m_state.store(JobState::STATE_IDLE);
  }
}

void JobObj::onJobStateChanged(JobState state, JobState oldState)
{
  (void)state;
  (void)oldState;
}

// ---------------------------------------------------------------------------
// private methods
// ---------------------------------------------------------------------------
AxisProfileResult JobObj::internalStateCycle(JobCb cb, JobState finalState)
{
  auto result = cb();

  // check if the job object can exit the current state
  if (result == AxisProfileResult::STS_OK)
  {
    if (finalState == JobState::STATE_IDLE)
    {
      onReset();
    }
    m_state.store(finalState);
  }

  return result;
}
