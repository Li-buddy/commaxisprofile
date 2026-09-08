#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines base class of statemachine of automation.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "../string_helper.h"
#include <algorithm>
#include <map>

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
  template<typename StateType> class StateEvent;
  template<typename StateType> class StateBase;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief Base class for a state machine
  template<typename StateType>
  class AutomationBase : public IEventSink<StateType>
  {
  public: // methods

    /// Typedef of a map of events and their target state.
    typedef typename std::map<StateEvent<StateType>*, StateType*> EventMap;
    /// Typedef of a map of states and their corresponding event map.
    typedef typename std::map<StateType*, EventMap >    StateMap;
    /// Typedef of the iterator for the event map.
    typedef typename EventMap::iterator EventMapIter;
    /// Typedef of the iterator of the state map.
    typedef typename StateMap::iterator StateMapIter;

    virtual ~AutomationBase() noexcept override = default;

    /// @brief Cast an event of the statemachine and execute the corresponding state transition.
    /// @param[in,out]  ev         Event that should be casted.
    /// @return nothing
    virtual void castEvent(StateEvent<StateType>* ev)
    {
      if (ev != NULL)
      {
        m_currentState = m_stateTransitions[m_currentState][ev];
      }
    }

    /// @brief Get dot format of the state machine (see http://www.graphviz.org/)
    /// @param[in,out]  dot         Buffer to save dot format in
    /// @param[in]      dotLength   Length of dot buffer
    /// @return nothing
    virtual bool getDot(char* dot, std::uint32_t dotLength)
    {
      if (!isComplete(dot, dotLength))
      {
        return false;
      }

      stringCpyS(dot, dotLength, "digraph G {", dotLength);

      for (StateMapIter it = m_stateTransitions.begin(); it != m_stateTransitions.end(); it++)
      {
        stringCatS(dot, dotLength, "\n", STATE_STRING_COPY_COUNT);
        stringCatS(dot, dotLength, it->first->getName(), STATE_STRING_COPY_COUNT);
        if (it->first == m_currentState)
        {
          stringCatS(dot, dotLength, " [style=filled color=grey]", STATE_STRING_COPY_COUNT);
        }
        stringCatS(dot, dotLength, ";", STATE_STRING_COPY_COUNT);
      }

      for (StateMapIter it = m_stateTransitions.begin(); it != m_stateTransitions.end(); it++)
      {
        for (EventMapIter it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
          stringCatS(dot, dotLength, "\n", STATE_STRING_COPY_COUNT);
          stringCatS(dot, dotLength, it->first->getName(), STATE_STRING_COPY_COUNT);
          stringCatS(dot, dotLength, " -> ", STATE_STRING_COPY_COUNT);
          stringCatS(dot, dotLength, it2->second->getName(), STATE_STRING_COPY_COUNT);
          stringCatS(dot, dotLength, " [label=\"", STATE_STRING_COPY_COUNT);
          stringCatS(dot, dotLength, it2->first->getName(), STATE_STRING_COPY_COUNT);
          stringCatS(dot, dotLength, "\"];", STATE_STRING_COPY_COUNT);
        }
      }
      stringCatS(dot, dotLength, "\n}", dotLength);
      return true;
    }

    /// @brief Check if the implemented statemachine is complete and all events have a corresponding transition.
    /// If the statemachine is not complete, the method returns the dot format of the incomplete states.
    /// @param[in,out]  dot         Buffer to save dot format in
    /// @param[in]      dotLength   Length of dot buffer
    /// @return         bool        Returns if the implemented statemachine is complete.
    virtual bool isComplete(char* dot, std::uint32_t dotLength)
    {
      bool boSuccess = true;
      stringCpyS(dot, dotLength, "digraph G {", STATE_STRING_COPY_COUNT);

      for (StateMapIter originStateEventMap = m_stateTransitions.begin(); originStateEventMap != m_stateTransitions.end(); originStateEventMap++)
      {
        StateBase<StateType>* originState = originStateEventMap->first;
        auto eventList = originState->getEventList();

        for (auto ev = eventList.begin(); ev != eventList.end(); ev++)
        {
          EventMap eventMap = originStateEventMap->second;
          if (eventMap.find(*ev) == eventMap.end())
          {
            // LCOV_EXCL_START - exclude code coverage section begin because this is only a helper in case of an error
            //state
            stringCatS(dot, dotLength, "\n", STATE_STRING_COPY_COUNT);
            stringCatS(dot, dotLength, originState->getName(), STATE_STRING_COPY_COUNT);
            stringCatS(dot, dotLength, ";", STATE_STRING_COPY_COUNT);

            //missing edge
            stringCatS(dot, dotLength, "\n", STATE_STRING_COPY_COUNT);
            stringCatS(dot, dotLength, originState->getName(), STATE_STRING_COPY_COUNT);
            stringCatS(dot, dotLength, " -> ", STATE_STRING_COPY_COUNT);
            stringCatS(dot, dotLength, "NotConnected", STATE_STRING_COPY_COUNT);
            stringCatS(dot, dotLength, " [label=\"", STATE_STRING_COPY_COUNT);
            stringCatS(dot, dotLength, (*ev)->getName(), STATE_STRING_COPY_COUNT);
            stringCatS(dot, dotLength, "\"];", STATE_STRING_COPY_COUNT);
            boSuccess = false;
            // LCOV_EXCL_STOP - exclude code coverage section end
          }
        }
      }
      stringCatS(dot, dotLength, "\n}", STATE_STRING_COPY_COUNT);
      return boSuccess;
    }

  protected: // methods

    /// @brief Current state
    StateType* m_currentState;

    /// @brief Adds an edge to the state machine
    /// @param[in] source   source state of the edge
    /// @param[in] ev       event for the edge
    /// @param[in] target   destination state of the edge
    void addEdge(StateType *source, StateEvent<StateType> *ev, StateType *target)
    {
      m_stateTransitions[source][ev] = target;
    }

    /// @brief get access to the transition map for derived classes
    /// @return the transition map
    StateMap &getTransitionMap()
    {
      return m_stateTransitions;
    }

  private: // data

    /// @breif Map of the statemachine with all available transitions between the states.
    StateMap m_stateTransitions;
  };
}
