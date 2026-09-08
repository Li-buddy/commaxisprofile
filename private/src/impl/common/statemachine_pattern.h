#pragma once
// ---------------------------------------------------------------------------
//
//! @file       statemachine_pattern.h
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
//! This file is part of the motion library and collects all header files for
//! the state machine pattern.
//
// ---------------------------------------------------------------------------

//! @page PLC_STM PLCOpen state machine
//! @section sec_Intro Introduction
//! Pattern for easy implementation of a state machine. While using this pattern you have following advantages:
//! - add a new state
//! - add a new edge of the state machine
//! - reduce code of state machine
//! - easy get a dot representation of your state machine ( AutomationBase::getDot() )
//! 
//! The most important thing you have to define functions you state machine reacts on. Typically you
//! define these functions in an separate interface (all of these functions are pure virtual). The state machine
//! and all states of the state machine have to implement these functions. 
//! \n \n
//! Implementation of an state.\n
//! Every state is an object implementing your interface function. Each state only knows its events to fire (later events with start and end
//! state will be edges). It did not know other states. For every state you have following to do:
//! - define your state as a class. Derive from your state machine interface (interface function). So you have to implement each of these functions
//! - define your events to fire
//! - implement your interface functions and cast events if you have to switch to an another state using iEventSink::castEvent(). You did not define the target state here!
//!
//! \n \n
//! Implementation of the state machine\n
//! - Implement the above interface functions of your state machine. You have to call simply the same function of the current state.
//! - Make private pointers to your states. New them in constructor of your state machine
//! - In constructor define your starting state (assignment of current state).
//! @cond DoNotRaiseWarning
//! - In constructor define your edges of your state (<start state> -> <start state>.event -> <dest. state>) using AutomationBase::addEdge()
//! @endcond
//!
//! \n \n
//! Using the state machine:\n
//! - make an object of your state machine
//! - use interface functions


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------

#include "state_machine/i_event.h"
#include "state_machine/automation_base.h"
#include "state_machine/state_base.h"
#include "state_machine/state_event.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

