#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines state base class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <list>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr std::uint32_t m_nameSize = 40;

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  template<typename StateType> class StateEvent;
    
  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief Base class for all states of a state machine
  template<typename StateType>
  class StateBase
  {
  protected: // types

    /// @brief Pointer to the corresponding statemachine.
    AutomationBase<StateType>* m_automation;
    /// @brief Pointer to the usable event sink.
    IEventSink<StateType>*      m_eventSink;
    /// @brief Name of the state.
    char m_name[m_nameSize];

  public: // methods

    /// @brief Destructor
    virtual ~StateBase() noexcept = default;

    /// @brief Typedef of a list of state events.
    typedef typename std::list<StateEvent<StateType>*> EventList;

    /// @brief Constructor
    /// @param[in]  automation   State machine to work with
    /// @param[in]  eventSink    Event sink to work with
    /// @param[in]  name         Name of the state
    StateBase(AutomationBase<StateType>* automation, IEventSink<StateType>* eventSink, const char* name)
    {
      m_automation = automation;
      m_eventSink = eventSink;
      if (name)
      {
        stringCpyS(m_name, m_nameSize, name, STATE_STRING_COPY_COUNT);
      }
    }

    /// @brief Get name of the state
    /// @return name of the state
    const char* getName() const { return m_name; }

    /// @brief Add an event to the state.
    /// @param[in]  event   Event to add.
    /// @return  nothing
    void addEvent(StateEvent<StateType>* event)
    {
      m_eventList.push_back(event);
    }

    /// @brief Get read access to the event list
    /// @return  the event list
    const EventList &getEventList() const
    {
      return m_eventList;
    }

  protected:

    /// @brief List of all available events of the state.
    EventList m_eventList;

    /// @brief Cast an event of this state.
    /// @param[in]  ev         Event that should be casted.
    /// @return nothing
    void castEvent(StateEvent<StateType>* ev)
    {
      m_eventSink->castEvent(ev);
    }
  };
}
