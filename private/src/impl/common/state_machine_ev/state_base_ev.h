#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and the state template for
/// the state machine pattern with events.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "../state_machine/state_base.h"
#include <functional>
#include <vector>

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

  /// @brief Base class for all states of a state machine
  template<typename StateType, typename StateIDType>
  class StateBaseEv : public StateBase<StateType>
  {
  public: // types

    /// @brief Define the callback function pointer
    typedef std::function<bool(StateIDType, StateIDType)> CallbackExit;
    /// @brief Define the callback function pointer
    typedef std::function<void(StateIDType, StateIDType)> CallbackEnter;

  public:
    /// Constructor
    /// @param[in]  automation        State machine to work with
    /// @param[in]  name              Name of the state
    /// @param[in]  id                ID of the state
    /// @param[in]  isEmergencyState  indicates, that this state is an emergency state
    StateBaseEv(AutomationBase<StateType>* automation, const char* name, StateIDType id, bool isEmergencyState = false)
      : StateBase<StateType>(automation, automation, name), m_myID(id), m_isEmergencyState(isEmergencyState)
    {}

    /// @brief Destructor
    virtual ~StateBaseEv() noexcept override = default;

    /// @brief add an exit state callback
    /// @param[in]  callback   the callback pointer
    void addCallback(std::function<bool(StateIDType, StateIDType)> callback)
    {
      m_exitCallbacks.push_back(callback);
    }

    /// @brief add an enter state callback
    /// @param[in]  callback   the callback pointer
    void addCallback(std::function<void(StateIDType, StateIDType)> callback)
    {
      m_enterCallbacks.push_back(callback);
    }

    /// @brief  get the ID of the state
    /// @return the ID of the state
    const StateIDType& getID() const
    {
      return m_myID;
    }

    /// @brief  check, if this state is an emergency state
    /// @return if this state is an emergency state
    bool isEmergencyState() const
    {
      return m_isEmergencyState;
    }

    /// @brief  run all state exit callbacks
    /// @param[in]  trgID              ID of the target state
    /// @param[in]  switchToEmergency  set to true, when we switch to an emergency state and all callback return values are ignored
    /// @return true, when all state exit callbacks return true
    bool runAllExitCallbacks(const StateIDType& trgID, bool switchToEmergency) const
    {
      // run all 'exit state' callbacks
      // when one of them returns false, the state change is canceled
      for (auto& cb : m_exitCallbacks)
      {
        if (!cb(m_myID, trgID) && !switchToEmergency)
        {
          return false;
        }
      }
      return true;
    }

    /// @brief  run all state enter callbacks
    /// @param[in]  srcID      ID of the source state
    void runAllEnterCallbacks(const StateIDType& srcID) const
    {
      // finally, run all 'enter state' callbacks of the new state
      for (auto& cb : m_enterCallbacks)
      {
        cb(srcID, m_myID);
      }
    }

  private: // data

    StateIDType m_myID;                               ///< ID of the state
    std::vector< CallbackExit > m_exitCallbacks;      ///< these callbacks are called, when the state should be leaved (and may cancel this)
    std::vector< CallbackEnter > m_enterCallbacks;    ///< these callbacks are called, when the state is entered
    bool m_isEmergencyState;                          ///< indicates, if this state is an emergency state
  };
}
