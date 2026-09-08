#pragma once

// ---------------------------------------------------------------------------
//
//! @file       state_machine.h
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
//! This file is part of the motion library and represents the global state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/statemachine_pattern_ev.h"
#include "axisprofile_internal_defs.h"
#include "state_base_global.h"

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
    //! This macro prints the given parameter as string.
    #define _CLASS_NAME_(x)   (           # x )
    //! This macro maps the given parameter to a macro that prints it as string.
    #define CLASS_NAME(x)     (_CLASS_NAME_(x))

    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class StateMachine
    //! This is the interface for the global state machine.
    class StateMachine : public AutomationBaseEv<StateBaseGlobal, StateIDType>, public IStateMachine
    {
    public: // methods
      //! @brief Constructor
      StateMachine();

      //! @brief Destructor
      virtual ~StateMachine() noexcept override;

      //! @brief This method returns the class name.
      //!
      //! @return                      Return class name.
      static const char* getClassName(void) { return (CLASS_NAME(StateMachine)); }

      //! @brief get the ID of the current state
      //! @return  the ID of the current state
      const StateIDType &getState() const final;

      //! @brief Request a state change.
      //! @param[in]  newStateID      the ID of the new state
      //! @return     true, is the state change was successful
      GlobalStateResult requestSwitchToState(const StateIDType &newStateID) final;

      //! @brief add an exit state callback
      //! @param[in]  stateID    the ID of the state the callback should be added
      //! @param[in]  callback   the callback pointer
      bool addCallback(const StateIDType &stateID, IStateMachine::CallbackExit callback) final;

      //! @brief add an enter state callback
      //! @param[in]  stateID    the ID of the state the callback should be added
      //! @param[in]  callback   the callback pointer
      bool addCallback(const StateIDType &stateID, IStateMachine::CallbackEnter callback) final;

      //! @brief get names of all states
      //! @return names of all states
      const std::vector<std::string> &getStateNames() const final;

      //! @brief get the name of the a state by it's ID
      //! @param[in]  stateID    the ID of the state
      //! @return  the name of the state or nullptr (when unknown ID)
      const char *getStateName(StateIDType stateID) const final;

      //! @brief get the ID of the a state by it's name
      //! @param[in]  stateName    the name of the state
      //! @param[out] stateID      the ID of the state
      //! @return  true, when successful
      bool getStateID(const char *stateName, StateIDType &stateID) const final;


      const StateMachineState* getStateMachineStatus() const final;

    private:  // variables
      StateBaseGlobal m_stateError;       //!< Error state
      StateBaseGlobal m_stateBooting;     //!< Booting state
      StateBaseGlobal m_stateRunning;     //!< Running state
      StateBaseGlobal m_stateConfig;      //!< Configuration state

      typedef StateEvent<StateBaseGlobal> Event;   //!< shortcut to Event type
      std::vector<Event *> m_events;               //!< vector of all created events
      std::vector<std::string> m_stateNames;       //!< names of all states

    private:  // functions

      //! @brief Adds an edge to the state machine
      //! @param[in] source   source state of the edge
      //! @param[in] target   destination state of the edge
      //! @return nothing
      void addEdge(StateBaseGlobal &srcState, StateBaseGlobal &trgState);
      StateMachineState m_StateMachineState{StateMachineState::BOOTING};
      const StateMachineState* const m_ptrStateMachineState = &m_StateMachineState;
    };
  }
}
