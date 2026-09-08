#pragma once

// ---------------------------------------------------------------------------
//
//! @file       i_state_machine.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This file is part of the motion library and defines the interface for the
//! global state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <functional>
#include <string>
#include <vector>
#include "statemachine_defs.h"
#include "statemachine_statusdefs.h"
#include "axisprofile_internal_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
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

    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------
    //! @class IStateMachine
    //! This is the interface for the global state machine.
    class IStateMachine
    {
    public: // types

      //! @brief Define the callback function pointer
      typedef std::function<bool(StateIDType, StateIDType)> CallbackExit;
      //! @brief Define the callback function pointer
      typedef std::function<void(StateIDType, StateIDType)> CallbackEnter;

    public: // methods
      //! @brief Destructor
      virtual ~IStateMachine() noexcept = default;

      //! @brief get the ID of the current state
      //! @return  the ID of the current state
      virtual const StateIDType &getState() const = 0;

      //! @brief Request a state change.
      //! @param[in]  newStateID      the ID of the new state
      //! @return     result of the request
      virtual GlobalStateResult requestSwitchToState(const StateIDType &newStateID) = 0;

      //! @brief add an exit state callback
      //! @param[in]  stateID    the ID of the state the callback should be added
      //! @param[in]  callback   the callback pointer
      virtual bool addCallback(const StateIDType &stateID, CallbackExit callback) = 0;

      //! @brief add an enter state callback
      //! @param[in]  stateID    the ID of the state the callback should be added
      //! @param[in]  callback   the callback pointer
      virtual bool addCallback(const StateIDType &stateID, CallbackEnter callback) = 0;

      //! @brief get names of all states
      //! @return names of all states
      virtual const std::vector<std::string> &getStateNames() const = 0;

      //! @brief get the name of the a state by it's ID
      //! @param[in]  stateID    the ID of the state
      //! @return  the name of the state or nullptr (when unknown ID)
      virtual const char *getStateName(StateIDType stateID) const = 0;

      //! @brief get the ID of the a state by it's name
      //! @param[in]  stateName    the name of the state
      //! @param[out] stateID      the ID of the state
      //! @return  true, when successful
      virtual bool getStateID(const char *stateName, StateIDType &stateID) const = 0;

      //! @brief get the pointer to the state machine state
      //! @return  state machine state pointer
      virtual const StateMachineState* getStateMachineStatus() const = 0;

    };
  }
}
