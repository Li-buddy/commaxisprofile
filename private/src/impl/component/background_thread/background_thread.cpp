/// @file
///
/// @brief This file is part of the axisprofile library and implements background thread.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <vector>

#include "background_thread.h"
#include "common/debug_trace.h"
#include "component/i_axisprofile_component.h"
#include "component/statemachine/i_state_machine.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "profiles/i_axisprofile_internal.h"
#include "common/systemhandler/i_systemstate.h"

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

BackgroundThread::BackgroundThread(IAxisProfileComponent* profileCmp):
  m_profileCmp(profileCmp),
  m_thread(nullptr),
  m_exeAbort(false),
  m_cycleTimeUs {bkthread::DEFALUT_CYCLE_TIME_US}
{
}

BackgroundThread::~BackgroundThread()
{
  deInit();
}

void BackgroundThread::init()
{
  APR_TRACE_MESSAGE("Create background thread");
  m_thread = new std::thread(&BackgroundThread::execution, this);

#ifndef _MSC_VER
  pthread_setname_np(m_thread->native_handle(), "axsprof.bkthrd"); // naming has 16 chars limit
#endif
}

void BackgroundThread::deInit()
{
  m_exeAbort.store(true);
  m_sema.notify();

  if (m_thread)
  {
    m_thread->join();
    delete m_thread;
    m_thread = nullptr;
  }
}

void BackgroundThread::wakeup()
{
  // for safety reason, background thread can only be waked up in BOOTING and RUNNING state
  // because the callback may be taken at init and start phase of the bundle.
  auto aprState = m_profileCmp->getStateMachine();
  if (!aprState || (aprState->getState() != StateIDType::RUNNING &&
                    aprState->getState() != StateIDType::BOOTING))
  {
    APR_TRACE_ERROR("Background thread can't wakeup when state is not in %s or %s...",
                    stateTypeToString(StateIDType::RUNNING),
                    stateTypeToString(StateIDType::BOOTING));
    return;
  }

  m_sema.notify();
}

AxisProfileResult BackgroundThread::appCallbackInNormalState(BkCallbackFun cb)
{
  // when axisprofile in booting or running state, callback can't be append
  auto aprState = m_profileCmp->getStateMachine();
  if (aprState && (aprState->getState() == StateIDType::RUNNING ||
                   aprState->getState() == StateIDType::BOOTING))
  {
    APR_TRACE_MESSAGE("RT-Thread synchronization is active, callback can't be added...");
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  m_normalCbFuns.emplace_back(cb);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult BackgroundThread::appCallbackInRunningState(BkCallbackFun cb)
{
  // when axisprofile in running state, callback can't be append
  auto aprState = m_profileCmp->getStateMachine();
  if (aprState && aprState->getState() == StateIDType::RUNNING)
  {
    APR_TRACE_MESSAGE("callback can't be added when axisprofile in RUNNING state...");
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  m_runningCbFuns.emplace_back(cb);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult BackgroundThread::execution()
{
  APR_TRACE_MESSAGE("Background thread is startup...");

  while (!m_exeAbort.load())
  {
    // sleep the thread until another trigger.
    APR_TRACE_MESSAGE("Background thread is sleep down...");
    m_sema.wait();
    APR_TRACE_MESSAGE("Background thread is waked up...");

    while (!m_exeAbort.load())
    {
      auto state = m_profileCmp->getStateMachine()->getState();
      if (state == StateIDType::RUNNING)
      {
        exeInRuningState();
      }

      if (state == StateIDType::RUNNING || state == StateIDType::BOOTING)
      {
        exeInNormalState();
      }

      if (state == StateIDType::CONFIG || m_exeAbort.load())
      {
        break;
      }
      std::this_thread::sleep_for(std::chrono::microseconds(m_cycleTimeUs.load()));
    }
  }

  APR_TRACE_MESSAGE("Background thread is shutdown...");
  return AxisProfileResult::STS_OK;
}

void BackgroundThread::exeInRuningState()
{
  for (const auto& it : m_runningCbFuns)
  {
    // abort execution immediately
    bool abort = m_exeAbort.load() || m_profileCmp->getStateMachine()->getState() != StateIDType::RUNNING;
    if (abort) { break;}
    if (it) { it(); }
  }
}

void BackgroundThread::exeInNormalState()
{
  for (const auto& it : m_normalCbFuns)
  {
    // abort execution immediately when aborted or switch out from (RUNNING and BOOTING)
    bool abort = m_exeAbort.load() || (m_profileCmp->getStateMachine()->getState() != StateIDType::RUNNING &&
                                       m_profileCmp->getStateMachine()->getState() != StateIDType::BOOTING);
    if (abort) { break;}
    if (it) { it(); }
  }
}

AxisProfileResult BackgroundThread::setCycleTime(double cycleTime)
{
  // can be changed dynamically
  m_cycleTimeUs = static_cast<std::uint32_t>(SECOND_2_MICROSECOND(cycleTime));
  APR_TRACE_MESSAGE("Background thread polling cycle set to %u us", m_cycleTimeUs.load());
  return AxisProfileResult::STS_OK;
}
