#pragma once
// ---------------------------------------------------------------------------
//
//! @file       statemachine_statusdefs.h
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
//! This header file is part of the motion library and contains error defines
//! relevant for the plc-open state machines.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------


    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------

    //! Definition of the return type used in the statemachine namespace.
    enum class StateResult
    {
      STS_OK                   = APR_STATUS_MAKE_SUCCESS(0x1000),   //! Return status is ok.

      ERR_NOK                  = APR_STATUS_MAKE_ERROR(0x1001),     //! Return status is not ok.
      ERR_FAILED_TO_LOCK_MUTEX = APR_STATUS_MAKE_ERROR(0x1002),     //! Locking of statemachine mutex failed
    };

    //! Return whether status code is an error status code
    //! @param[in]  status     Status code
    //! @return true if status code is an error status code
    inline bool stateStatusFailed(StateResult status)
    {
      return (std::uint32_t(status) & (1u << 31));
    };


    //! @brief Return codes for state changes of the global state machine
    enum class GlobalStateResult
    {
      STS_OK                  = APR_STATUS_MAKE_SUCCESS(0x0000),   //!< indicating success

      ERR_NOK                 = APR_STATUS_MAKE_ERROR(0x0000),     //!< generic error result
      ERR_CB_CANCEL           = APR_STATUS_MAKE_ERROR(0x0001),     //!< state change is canceled by an exit-callback
      ERR_INVALID_PARAMETER   = APR_STATUS_MAKE_ERROR(0x0002),     //!< invalid parameter for method
      ERR_NO_TRANSITION       = APR_STATUS_MAKE_ERROR(0x0003),     //!< no transition from current state to target state defined
      ERR_STATE_CHANGE_ACTIVE = APR_STATUS_MAKE_ERROR(0x0004),     //!< a state change is in progress currently
    };

    //! Return whether status code is an error status code
    //! @param[in]  status     Status code
    //! @return true if status code is an error status code
    inline bool globalStateStatusFailed(GlobalStateResult status)
    {
      return (std::uint32_t(status) & (1u << 31));
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
  }
}
