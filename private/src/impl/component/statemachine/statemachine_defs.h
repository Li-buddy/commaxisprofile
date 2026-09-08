#pragma once

// ---------------------------------------------------------------------------
//
//! @file      statemachine_defs.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2020
//
// ---------------------------------------------------------------------------
//
//! This header file contains defines relevant for the system statemachine.
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------


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

    //! @brief enum for all states
    enum class StateIDType
    {
      ERROR_STATE = 0,  //!< Error state
      BOOTING,          //!< Booting state
      RUNNING,          //!< Running state
      CONFIG            //!< Configuration state
    };

    // ---------------------------------------------------------------------------
    // Declaration of macros
    // ---------------------------------------------------------------------------


    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @brief Convert the given state define to a string constant
    //! @param[in]   state   The state to be converted
    //! @return      The converted state
    inline const char *stateTypeToString(StateIDType state)
    {
      switch (state)
      {
        case StateIDType::ERROR_STATE: return "Error";
        case StateIDType::BOOTING:     return "Booting";
        case StateIDType::RUNNING:     return "Running";
        case StateIDType::CONFIG:      return "Configuration";
      }
      return "";
    }
  }
}
