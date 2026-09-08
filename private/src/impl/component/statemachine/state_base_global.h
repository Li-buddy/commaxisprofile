#pragma once

// ---------------------------------------------------------------------------
//
//! @file       state_base_global.h
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
//! This file is part of the motion library and represents a single state in
//! the global state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/statemachine_pattern_ev.h"
#include "i_state_machine.h"

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
    //! @class StateBaseGlobal
    //! This is the interface for the global state machine.
    class StateBaseGlobal : public StateBaseEv<StateBaseGlobal, StateIDType>
    {
    public: // methods

      //! Constructor
      //! @param[in]  automation        State machine to work with
      //! @param[in]  name              Name of the state
      //! @param[in]  id                ID of the state
      //! @param[in]  isEmergencyState  indicates, that this state is an emergency state
      StateBaseGlobal(AutomationBase<StateBaseGlobal>* automation, const char* name, StateIDType id, bool isEmergencyState = false)
        : StateBaseEv(automation, name, id, isEmergencyState) {}

      //! @brief Destructor
      virtual ~StateBaseGlobal() noexcept override = default;

      //! @brief This method returns the class name.
      //!
      //! @return                      Return class name.
      static const char* getClassName(void) { return (CLASS_NAME(StateBaseGlobal)); }

    };
  }
}
