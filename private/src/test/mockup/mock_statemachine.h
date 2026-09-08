#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_statemachine.h
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
//! This header file is part of the axisprofile library and declares a mockup
//! of the axisprofile state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "component/statemachine/i_state_machine.h"
#include "axisprofile_internal_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

namespace test
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockStateMachine
  //! @brief This class implements a mockup for the axisprofile state machine.
  class MockStateMachine : public comm::axisprofile::IStateMachine
  {
  public: // methods

    //! @brief Destructor
    virtual ~MockStateMachine() noexcept override = default;

    const comm::axisprofile::StateIDType &getState() const override { return m_stateID; }
    comm::axisprofile::GlobalStateResult requestSwitchToState( const comm::axisprofile::StateIDType &newStateID ) override { m_stateID = newStateID; return m_retResult; }
    bool addCallback( const comm::axisprofile::StateIDType &/*stateID*/, CallbackExit /*callback*/ ) override { return comm::axisprofile::globalStateStatusFailed(m_retResult) ? false : true; }
    bool addCallback( const comm::axisprofile::StateIDType &/*stateID*/, CallbackEnter /*callback*/ ) override { return comm::axisprofile::globalStateStatusFailed(m_retResult) ? false : true; }
    const std::vector<std::string> &getStateNames() const override { return m_names; }
    const char *getStateName(comm::axisprofile::StateIDType stateID ) const override { return m_names[static_cast<int>(stateID)].c_str(); }
    bool getStateID(const char * /*stateName*/, comm::axisprofile::StateIDType &stateID) const override { stateID = m_stateID;  return true; }
    const comm::axisprofile::StateMachineState* getStateMachineStatus() const override {return m_ptrStateMachineState;};
  public: // Data

    comm::axisprofile::GlobalStateResult m_retResult = comm::axisprofile::GlobalStateResult::STS_OK;   //!< Result of member functions
    comm::axisprofile::StateIDType m_stateID = comm::axisprofile::StateIDType::BOOTING;                //!< current state

  private: // Data
    comm::axisprofile::StateMachineState m_StateMachineState = comm::axisprofile::StateMachineState::BOOTING;
    const comm::axisprofile::StateMachineState* const m_ptrStateMachineState = &m_StateMachineState;
    std::vector<std::string> m_names{"Error", "Booting", "Running", "Configuration"};   //!< Names of all existing states
  };
}
