#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axis profile command interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_command_c.h"

// ---------------------------------------------------------------------------
// Forward references
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
  #define ASYNC_CYCLES_SIMULATION std::uint32_t(3) ///< cycles of async command for simulation

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This class is a simple helper that can be used as handle in this mockup.
  class AxisProfileCommandMockHandle
  {
  public: // methods

    /// @brief Set the return value of class methods.
    /// Note: Do not set 'm_returnValue' directly in your test or CppCheck may generate errors because of the extern C implementation in some cases.
    void setReturnValue(AxisProfileResult value) { m_returnValue = value; }

  public: // data

    AxisprofileCommandState m_cmdState = AxisprofileCommandState::CMD_STATE_UNDEFINED;  ///< State of the command
    AxisProfileResult m_returnValue = AxisProfileResult::STS_OK;   ///< Return value of C-Methods
    bool m_isHomed = false;                                        ///< Referenced value
    double m_refPos {0.0};                                         ///< Reference position
    std::uint32_t m_asyncSimCnter {ASYNC_CYCLES_SIMULATION};       ///< Async access counter for simulation
    std::vector<comm::axisprofile::AxisProfileCommandType> m_commands{}; ///< Post processing Commands in determined state
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // -------------------------------------- C-Interface Method-Mockups ---------------------------------------- //

  /// @brief Mockup of method AXISPROFILE_clearDeviceError().
  /// @param[in]   handle           Interface handle (use class AxisProfileCommandMockHandle)
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_clearDeviceErrorMock(void* handle);
    
  /// @brief Mockup of method AXISPROFILE_setAndCheckPositiveStop().
  /// @param[in]   handle           Interface handle (use class AxisProfileCommandMockHandle)
  /// @param[in]   mode             enable, disable or interrupt
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setAndCheckPositiveStopMock(void* handle, const AxisProfilePositiveStop mode);  

  /// @brief Deprecated. Mockup of method AXISPROFILE_setAndCheckClampingTorque(const double clampingTorque, const bool metrication).
  /// @param[in]  handle            Interface handle (use class AxisProfileCommandMockHandle)
  /// @param[in]  clampingTorque    clamping torque value to set on the device
  /// @param[in]  metrication       metric system
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setAndCheckClampingTorqueDeprecatedMock(void* handle,const double clampingTorque, const bool metrication);

  /// @brief Deprecated. Mockup of method AXISPROFILE_getClampingTorque(double* const clampingTorque, const bool metrication).
  /// @param[in]  handle            Interface handle (use class AxisProfileCommandMockHandle)
  /// @param[out] clampingTorque    clamping torque value to set on the device
  /// @param[in]  metrication       metric system
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getClampingTorqueDeprecatedMock(void* handle, double* const clampingTorque, const bool metrication);

  /// @brief Mockup of method AXISPROFILE_checkStandStill(bool* const standStill).
  /// @param[in]   handle           Interface handle (use class AxisProfileCommandMockHandle)
  /// @param[out]   standStill      address to store stand still information
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_checkStandStillMock(void* handle, bool* const standStill);

  /// @brief Mockup of method AXISPROFILE_setAndCheckstandStillWindow(const double standStillWindow).
  /// @param[in]   handle           Interface handle (use class AxisProfileCommandMockHandle)
  /// @param[in]   standStillWindow value to set
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setAndCheckstandStillWindowMock(void* handle, const double standStillWindow);

  /// @brief Mockup of method AXISPROFILE_getStandStillWindow(double* const standStillWindow).
  /// @param[in]   handle           Interface handle (use class AxisProfileCommandMockHandle)
  /// @param[out]  standStillWindow address to store standstill window value
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getStandStillWindowMock(void* handle, double* const standStillWindow);

  /// @brief Mockup of method AXISPROFILE_activateCyclicStandStillCheck().
  /// @param[in]   handle           Interface handle (use class AxisProfileCommandMockHandle)
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_activateCyclicStandStillCheckMock(void* handle);

  /// @brief Mockup of method AXISPROFILE_deactivateCyclicStandStillCheck().
  /// @param[in]   handle           Interface handle (use class AxisProfileCommandMockHandle)
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_deactivateCyclicStandStillCheckMock(void* handle);

  /// @brief This method triggers a error clear on the device.
  /// @param[in]   handle           Handle of the interface ((use class AxisProfileCommandMockHandle))
  /// @param[in]   position         the position value to set (deg, mm or inch)
  /// @param[in]   encoder          the encoder value to set  (encoder 1 or encoder 2)
  /// @param[in]   ifDriveEnabled   if the user want trigger the command while drive in enabled state (for sercos drive, i.e. AF state)
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setAbsolutePositionMock(void *handle, const double position, const AxisProfileEncoder encoder, const bool ifDriveEnabled);

  /// @brief Mockup of method AXISPROFILE_resetAbsolutePositionMock
  /// @param[in]   handle           Handle of interface (use class AxisProfileCommandMockHandle)
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_resetAbsolutePositionMock(void* handle);
  
  /// @brief Mockup of method AXISPROFILE_referencingAxis
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle)  
  /// @param[in]   newRefPos        New referencing position after homing
  /// @param[in]   encoder          the encoder value to set  (encoder 1 or encoder 2)
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_referencingAxisMock(void *handle, const double newRefPos, const AxisProfileEncoder encoder);

  /// @brief Mockup of method AXISPROFILE_resetReferencingAxis
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle) 
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_resetReferencingAxisMock(void *handle);

  /// @brief Mockup of method AXISPROFILE_setReferencingPositionMock
  /// @param[in]   handle           Handle of the interface ((use class AxisProfileCommandMockHandle))
  /// @param[in]   refPos           The position value to set (SI uint)
  /// @param[in]   encoder          The encoder value to set  (encoder 1 or encoder 2)
  /// @param[in]   mode             Use which method to set new position
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setReferencingPositionMock(void *handle, const double refPos, const AxisProfileEncoder encoder, RefMode mode);
    
  /// @brief Mockup of method AXISPROFILE_checkAxisHomed
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle). 
  /// @param[out]  isHomed          Indicate if axis homed  
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_checkAxisHomedMockDeprecated(void* handle, bool& isHomed);
  /// @brief Mockup of method AXISPROFILE_resetAxisprofileCommandStateMock
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle)
  /// @param[in]   type             type of the axisprofile command type
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_resetAxisprofileCommandStateMock(void* handle, const comm::axisprofile::AxisProfileCommandType type);

  /// @brief Mockup of method AXISPROFILE_abortAxisprofileCommandMock
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle)
  /// @param[in]   type             type of the axisprofile command type
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_abortAxisprofileCommandMock(void* handle, const comm::axisprofile::AxisProfileCommandType type);

  /// @brief Mockup of method AXISPROFILE_abortAxisprofileCommandMock
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle)
  /// @param[in]   type             type of the axisprofile command type
  /// @return                       state of the axisprofile state.
  extern "C" AxisprofileCommandState AXISPROFILE_getCommandStateMock(void* handle, const comm::axisprofile::AxisProfileCommandType type);
    
  /// @brief Mockup of method AXISPROFILE_getCommandsAtDeterminedStateMock
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle)
  /// @param[in]   cmdState         Command state
  /// @param[out]  vecCmdType       Commands in determined state
  /// @return                       Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getCommandsAtDeterminedStateMock(void* handle, const comm::axisprofile::AxisprofileCommandState cmdState, std::vector<comm::axisprofile::AxisProfileCommandType>& vecCmdType);
}
