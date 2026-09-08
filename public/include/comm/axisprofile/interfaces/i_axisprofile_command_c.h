#pragma once

// ---------------------------------------------------------------------------
//
//! @file       i_axisprofile_command_c.h
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
//! This file is part of the axisprofile library and implements the main
//! interface for all axisprofile commands. This is a sub-interface of
//! 'IAxisProfile'.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include <vector>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of classes and structures
// ---------------------------------------------------------------------------


#ifdef __cplusplus
extern "C" {
#endif
  //! @brief This is the C-Interface for the axisprofile command interface 'IAxisProfileCommand'.
  //! @note The interface will be deprecated and replaced by 2 new interfaces
  typedef struct IAxisProfileCommandC
  {
    void *handle;  //!< pointer to the instantiated object
    
          comm::axisprofile::AxisProfileResult ( *clearDeviceError )                ( void *handle );  //!< function pointer to the clearDeviceError, deprecated by clearDeviceError in IAxisProfileCmdC
          comm::axisprofile::AxisProfileResult ( *setAndCheckPositiveStop)          ( void* handle, const comm::axisprofile::AxisProfilePositiveStop mode);  //!< function pointer to the setAndCheckPositiveStop, deprecated by fixedStop in IAxisProfileCmdC
          comm::axisprofile::AxisProfileResult ( *setAndCheckClampingTorque )       ( void *handle, const double clampingTorque, const bool metrication);  //!< function pointer to the setAndCheckClampingTorque, deprecated by fixedStop in IAxisProfileCmdC
          comm::axisprofile::AxisProfileResult ( *getClampingTorque)                ( void *handle, double* const clampingTorque, const bool metrication); //!< function pointer to the getClampingTorque, not used anymore
          comm::axisprofile::AxisProfileResult ( *checkStandStill )                 ( void *handle, bool* const standStill);  //!< function pointer to the checkStandStill, deprecated by checkStandstill in IAxisProfileDeviceParamC
          comm::axisprofile::AxisProfileResult ( *setAndCheckStandStillWindow )     ( void *handle, const double standStillWindow);  //!< function pointer to the setAndCheckStandStillWindow, not used anymore
          comm::axisprofile::AxisProfileResult ( *getStandStillWindow )             ( void *handle, double* const standStillWindow);  //!< function pointer to the getStandStillWindow, not used anymore
          comm::axisprofile::AxisProfileResult ( *activateCyclicStandStillCheck )   ( void *handle );  //!< function pointer to the activateCyclicStandStillCheck, not used anymore
          comm::axisprofile::AxisProfileResult ( *deactivateCyclicStandStillCheck ) ( void *handle );  //!< function pointer to the deactivateCyclicStandStillCheck, not used anymore
          comm::axisprofile::AxisProfileResult ( *setAbsolutePosition)              ( void* handle, const double position, const comm::axisprofile::AxisProfileEncoder encoder, const bool ifDriveEnabled);  //!< function pointer to the setAbsolutePosition, deprecated by setAbsolutePosition in IAxisProfileCmdC
          comm::axisprofile::AxisProfileResult ( *referencingAxis )                 ( void *handle, const double newRefPos, const comm::axisprofile::AxisProfileEncoder encoder);  //!< function pointer to the referencingAxis, deprecated by referencingAxis in IAxisProfileCmdC
          comm::axisprofile::AxisProfileResult ( *resetReferencingAxis )            ( void *handle );  //!< function pointer to the resetreferencingAxis, deprecated by resetReferencingAxis in IAxisProfileCmdC
          comm::axisprofile::AxisProfileResult ( *setReferencingPosition)           ( void *handle, const double refPos, const comm::axisprofile::AxisProfileEncoder encoder, comm::axisprofile::RefMode mode);  //!< function pointer to the setReferencingPosition
          comm::axisprofile::AxisProfileResult ( *checkAxisHomed)                   ( void* handle, bool& isHomed);  ///<  function pointer to the checkAxisHomed, deprecated by checkAxisHomed in IAxisProfileDeviceParamC
          comm::axisprofile::AxisProfileResult ( *resetAxisProfileCommandState )    ( void* handle, const comm::axisprofile::AxisProfileCommandType type);  ///< reset the command state, deprecated by resetCmd in IAxisProfileCmdC
          comm::axisprofile::AxisProfileResult ( *abortAxisProfileCommand )         ( void* handle, const comm::axisprofile::AxisProfileCommandType type);  ///< abort the command, deprecated by abortCmd in IAxisProfileCmdC
    comm::axisprofile::AxisprofileCommandState ( *getAxisProfileCommandState )      ( void* handle, const comm::axisprofile::AxisProfileCommandType type);  ///< get command state, deprecated by getCmdState in IAxisProfileCmdC
          comm::axisprofile::AxisProfileResult ( *getCommandsAtDeterminedState )    ( void* handle, const comm::axisprofile::AxisprofileCommandState cmdstate, std::vector<comm::axisprofile::AxisProfileCommandType>& vecCmdType);  //!< get determined command state, not used anymore
          comm::axisprofile::AxisProfileResult ( *resetAbsolutePosition )           ( void* handle );  ///< function pointer to the resetAbsolutePositon, deprecated by resetAbsolutePosition in IAxisProfileCmdC

    void *reserved[100];  //!< reservation
  } IAxisProfileCommandC;

#ifdef __cplusplus
}
#endif
