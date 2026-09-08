#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the base template for
///        the state machine pattern with events.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------

#include "../state_machine/automation_base.h"
#include "common/debug_trace.h"
#include "component/statemachine/statemachine_statusdefs.h"
#include "state_base_ev.h"
#include <atomic>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @breif Base class for a state machine
  template<typename StateType, typename StateIDType>
  class AutomationBaseEv : public AutomationBase< StateType >
  {
  public: // methods

    AutomationBaseEv() : m_stateChangeInWork(false), m_currentEmergencyState(nullptr) {}

    virtual ~AutomationBaseEv() noexcept override = default;

    /// @breif Request a state change.
    /// @param[in]  newStateID      the ID of the new state
    /// @return     true, is the state change was successful
    virtual GlobalStateResult reqSwitchToState(const StateIDType &newStateID)
    {
      StateType *newState = findState(newStateID);
      if (!newState)
      {
        APR_TRACE_MESSAGE("AutomationBaseEv::requestSwitchToState() -> can't find state");
        return GlobalStateResult::ERR_INVALID_PARAMETER;
      }
      return requestSwitchToState(newState);
    }

    /// @brief get the ID of the current state
    /// @return  the ID of the current state
    virtual const StateIDType &getState() const
    {
      // when we have an emergency state, then we return it's ID
      const StateType *emergencyState = m_currentEmergencyState.load();
      if (emergencyState)
      {
        return emergencyState->getID();
      }
      return AutomationBase< StateType >::m_currentState->getID();
    }

    /// @brief add an exit state callback
    /// @param[in]  stateID    the ID of the state the callback should be added
    /// @param[in]  callback   the callback pointer
    bool addCallback(const StateIDType &stateID, std::function<bool(StateIDType, StateIDType)> callback)
    {
      StateType *state = findState(stateID);
      if (!state)
      {
        return false;
      }
      state->addCallback(callback);
      return true;
    }

    /// @brief add an enter state callback
    /// @param[in]  stateID    the ID of the state the callback should be added
    /// @param[in]  callback   the callback pointer
    bool addCallback(const StateIDType &stateID, std::function<void(StateIDType, StateIDType)> callback)
    {
      StateType *state = findState(stateID);
      if (!state)
      {
        return false;
      }
      state->addCallback(callback);
      return true;
    }

  private:

    std::atomic_bool m_stateChangeInWork;        ///< indicates, that a state change is in work
    std::atomic<const StateType *> m_currentEmergencyState;   ///< pointer to the active emergency state (nullptr means 'none active')


    /// @breif Request a state change.
    /// @param[in]  newState      the new state that should be switched to
    /// @return     result of the transition
    virtual GlobalStateResult requestSwitchToState(const StateType *newState)
    {
      // check, if the newState is an emergency state
      bool switchToEmergencyState = newState->isEmergencyState();
      if (switchToEmergencyState)
      {
        // set the pointer and indicate, that we are at once in the emergency state although a state change is still in progress
        m_currentEmergencyState.store(newState);
      }

      // test, if we are in a state change currently
      bool expected = false;
      if (!m_stateChangeInWork.compare_exchange_strong(expected, true))
      {
        // we are in a state change currently -> this request is denied (if we don't switched to an emergency state)
        APR_TRACE_MESSAGE("AutomationBaseEv::requestSwitchToState() -> we are in a state change currently");
        return switchToEmergencyState ? GlobalStateResult::STS_OK : GlobalStateResult::ERR_STATE_CHANGE_ACTIVE;
      }

      // find an event, that switches from the current state to the new state
      auto event = findEvent(newState);
      if (!event)
      {
        // state change finished, we did not find an event to switch to the requested state
        m_stateChangeInWork.store(false);
        APR_TRACE_ERROR("AutomationBaseEv::requestSwitchToState() -> no transition found");
        return GlobalStateResult::ERR_NO_TRANSITION;   // no transition from current state to new state
      }

      bool result = castEventEv(event, newState->getID(), switchToEmergencyState);
      m_stateChangeInWork.store(false);   // state change finished

      // special logic for emergency states
      const StateType *emergencyState = m_currentEmergencyState.load();
      if (emergencyState)
      {
        // an emergency state should be set, check if we are at this state already
        if (emergencyState == AutomationBase< StateType >::m_currentState)
        {
          // then we can set the internal variable back and further state changes may happen
          m_currentEmergencyState.store(nullptr);
        }
        else
        {
          // we just finished a other state change, try again to switch to the emergency state
          requestSwitchToState(emergencyState);
        }
      }

      return result ? GlobalStateResult::STS_OK : GlobalStateResult::ERR_CB_CANCEL;     // return state change result
    }

    /// @brief find an event, that switches from the current state to the given state
    /// @param[in]  newState      the new state that should be switched to
    /// @return     pointer to the event or nullptr
    StateEvent<StateType>* findEvent(const StateType *newState)
    {
      auto &eventMap = AutomationBase< StateType >::getTransitionMap()[AutomationBase< StateType >::m_currentState];
      for (auto &event : eventMap)
      {
        if (event.second == newState)
        {
          return event.first;
        }
      }

      return nullptr;
    }

    /// @breif Cast an event of the statemachine and execute the corresponding state transition.
    /// @param[in]  ev         Event that should be casted.
    /// @param[in]  trgID      ID of the target state
    /// @param[in]  switchToEmergencyState    indicate, if we currently switch to an emergency state
    /// @return     true, is the state change was successful
    virtual bool castEventEv(StateEvent<StateType> *ev, const StateIDType &trgID, bool switchToEmergencyState)
    {
      if (!AutomationBase< StateType >::m_currentState->runAllExitCallbacks(trgID, switchToEmergencyState))
      {
        return false;
      }

      const StateIDType &srcID = AutomationBase< StateType >::m_currentState->getID();
      // state change happens
      AutomationBase< StateType >::castEvent(ev);

      AutomationBase< StateType >::m_currentState->runAllEnterCallbacks(srcID);
      return true;
    }

    /// @brief  try to find a known state by it's name
    /// @param[in]  name                 name of the state to find
    /// @param[in]  includeFinalStates   find final states (that have no transition to an other state) too
    /// @return     the found state or nullptr
    StateType *findState(const StateIDType &id, bool includeFinalStates = true)
    {
      for (auto state : AutomationBase< StateType >::getTransitionMap())
      {
        if (id == state.first->getID())
        {
          return state.first;
        }
        if (includeFinalStates)
        {
          for (auto event : state.second)
          {
            if (id == event.second->getID())
            {
              return event.second;
            }
          }
        }
      }
      return nullptr;
    }
  };
}
