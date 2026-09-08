#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines an interface for an event sink.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------

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

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief Interface for an event sink
  template<typename StateType>
  class IEventSink
  {
  public: // methods

    /// @brief Destructor
    virtual ~IEventSink() noexcept = default;

    /// @brief Cast an event
    /// @param[in]  ev     Event to cast
    /// @return nothing
    virtual void castEvent(StateEvent<StateType>* ev) = 0;
  };
}
