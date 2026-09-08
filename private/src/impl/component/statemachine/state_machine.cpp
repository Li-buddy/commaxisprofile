// ---------------------------------------------------------------------------
//
//! @file       state_machine.cpp
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
//! This file is part of the motion library and implements the global state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "state_machine.h"
// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

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
// Implementation of classes and structures
// ---------------------------------------------------------------------------

StateMachine::StateMachine() : AutomationBaseEv<StateBaseGlobal, StateIDType>(),
  m_stateError( this, "Error", StateIDType::ERROR_STATE, true ),
  m_stateBooting( this, "Booting", StateIDType::BOOTING ),
  m_stateRunning( this, "Running", StateIDType::RUNNING ),
  m_stateConfig( this, "Configuration", StateIDType::CONFIG )
{
  m_currentState = &m_stateBooting;

  // add all names to the vector for easy access, ensure same sequence as in StateIDType!
  m_stateNames.reserve(4);
  m_stateNames.push_back( m_stateError.getName() );
  m_stateNames.push_back( m_stateBooting.getName() );
  m_stateNames.push_back( m_stateRunning.getName() );
  m_stateNames.push_back( m_stateConfig.getName() );

  addEdge( m_stateBooting, m_stateRunning );    // setup successful
  addEdge( m_stateBooting, m_stateConfig );     // switch to configuration
  addEdge( m_stateBooting, m_stateError );      // critical error

  addEdge( m_stateRunning, m_stateConfig );     // switch to configuration
  addEdge( m_stateRunning, m_stateError );      // critical error

  addEdge( m_stateConfig, m_stateBooting );     // configuration finished
  addEdge( m_stateConfig, m_stateError );       // critical error

  addEdge( m_stateError, m_stateConfig );       // switch to configuration
}

StateMachine::~StateMachine()
{ 
  m_StateMachineState = StateMachineState::BOOTING;
  for ( auto &ev : m_events )
  {
    delete ev;
    ev = nullptr;
  }
  m_events.clear();
}

void StateMachine::addEdge( StateBaseGlobal &srcState, StateBaseGlobal &trgState )
{
  std::string temp = std::string( srcState.getName() ) + " --> " + trgState.getName();
  Event *newEvent = new Event( &srcState, temp.c_str() );
  m_events.push_back( newEvent );
  AutomationBaseEv<StateBaseGlobal, StateIDType>::addEdge( &srcState, newEvent, &trgState );
}

const StateIDType &StateMachine::getState() const
{
  return AutomationBaseEv<StateBaseGlobal, StateIDType>::getState();
}

GlobalStateResult StateMachine::requestSwitchToState( const StateIDType &newState )
{
  GlobalStateResult result;
  result = AutomationBaseEv<StateBaseGlobal, StateIDType>::reqSwitchToState( newState );
  if(GlobalStateResult::STS_OK == result)
  {
    switch(newState)
    {
      case StateIDType::RUNNING:
      {
        m_StateMachineState = StateMachineState::RUNNING;
        break;
      }
      case StateIDType::CONFIG:
      {
        m_StateMachineState = StateMachineState::CONFIG;
        break;
      }
      case StateIDType::BOOTING:
      {
        m_StateMachineState = StateMachineState::BOOTING;
        break;
      }
      default:
      {
        m_StateMachineState = StateMachineState::ERROR_STATE;
        break;  
      }
    }
  }
  return result;
}

bool StateMachine::addCallback( const StateIDType &stateID, IStateMachine::CallbackExit callback )
{
  return AutomationBaseEv<StateBaseGlobal, StateIDType>::addCallback( stateID, callback );
}

bool StateMachine::addCallback( const StateIDType &stateID, IStateMachine::CallbackEnter callback )
{
  return AutomationBaseEv<StateBaseGlobal, StateIDType>::addCallback( stateID, callback );
}

const std::vector<std::string> &StateMachine::getStateNames() const
{
  return m_stateNames;
}

const char *StateMachine::getStateName( StateIDType stateID ) const
{
  std::uint32_t idx = static_cast<std::uint32_t>( stateID );
  if ( idx >= m_stateNames.size() )
  {
    return nullptr;
  }
  return m_stateNames[idx].c_str();
}

bool StateMachine::getStateID( const char *stateName, StateIDType &stateID ) const
{
  if ( !stateName )
  {
    return false;
  }

  std::uint32_t idx = 0;
  while ( idx < m_stateNames.size() )
  {
    if ( m_stateNames[idx] == stateName )
    {
      stateID = static_cast<StateIDType>(idx);
      return true;
    }
    ++idx;
  }
  return false;
}

const StateMachineState* StateMachine::getStateMachineStatus() const
{
  return m_ptrStateMachineState;
}
