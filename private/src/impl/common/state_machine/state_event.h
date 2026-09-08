#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines state event class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


namespace comm::axisprofile
{
  /// @brief class StateEvent
  /// Base class for all events
  template<typename StateType>
  class StateEvent
  {
  private: // type

    /// @brief Name of the event.
    char m_name[m_nameSize];

  public: // methods

    /// @brief Constructor
    /// @param[in] state    name of the state that holds the event.
    /// @param[in] name     name of the event.
    StateEvent(StateBase<StateType> *const state, const char* name)
    {
      memset(m_name, 0, m_nameSize);
      if (name)
      {
        stringCpyS(m_name, m_nameSize, name, STATE_STRING_COPY_COUNT);
      }
      state->addEvent(this);
    };

    /// @brief Destructor
    virtual ~StateEvent() noexcept = default;

    /// @brief Get name of the event
    /// @return name of the event
    char* getName() { return m_name; };
  };
}
