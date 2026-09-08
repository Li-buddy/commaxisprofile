/// @file
///
/// @brief This file is part of the axisprofile library and implements
///        C/C++ method mapping for the base class for command handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_if_helper.h"
#include "i_axisprofile_command.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Function implementation (Wrapper for C-Interface)
// ---------------------------------------------------------------------------

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_clearDeviceError(void* handle)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "clear device error");
  return commandInterface->clearDeviceError();
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setAndCheckPositiveStop(void* handle, const comm::axisprofile::AxisProfilePositiveStop mode)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "set positive stop");
  return commandInterface->setAndCheckPositiveStop(mode);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setAndCheckClampingTorque(void* handle, const double clampingTorque, const bool /* metrication */)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "set clamping torque");
  return commandInterface->setAndCheckClampingTorque(clampingTorque);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getClampingTorqueDeprecated(void* handle, double* const clampingTorque, const bool /* metrication */)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "get clamping torque");
  return commandInterface->getClampingTorque(clampingTorque);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_checkStandStill(void* handle, bool* const standStill)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "check standstill");
  return commandInterface->checkStandStill(standStill);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setAndCheckstandStillWindow(void* handle, const double standStillWindow)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "set standstill window");
  return commandInterface->setAndCheckStandStillWindow(standStillWindow);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getStandStillWindow(void* handle, double* const standStillWindow)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "get standstill window");
  return commandInterface->getStandStillWindow(standStillWindow);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_activateCyclicStandStillCheck(void* handle)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "activate CyclicStandStillCheck");
  return commandInterface->activateCyclicStandStillCheck();
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_deactivateCyclicStandStillCheck(void* handle)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "deactivate CyclicStandStillCheck");
  return commandInterface->deactivateCyclicStandStillCheck();
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setAbsolutePosition(void *handle, const double position, const comm::axisprofile::AxisProfileEncoder encoder, const bool ifDriveEnabled)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "set absolute position");
  return commandInterface->setAbsolutePosition(position, encoder, ifDriveEnabled);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_resetAbsolutePosition(void* handle)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "reset absolute position");
  return commandInterface->resetAbsolutePosition();
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_referencingAxis(void *handle, const double newRefPos, const AxisProfileEncoder encoder)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "reference axis");
  return commandInterface->referencingAxis(newRefPos, encoder);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_resetReferencingAxis(void *handle)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "reset reference axis");
  return commandInterface->resetReferencingAxis();
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_checkAxisHomedDeprecated(void* handle, bool& isHomed)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "check homed status");
  return commandInterface->checkAxisHomed(isHomed);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_resetAxisProfileCommandState(void* handle, const comm::axisprofile::AxisProfileCommandType type)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "reset command state");
  return commandInterface->resetAxisProfileCommandState(type);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_abortAxisProfileCommand(void* handle, const comm::axisprofile::AxisProfileCommandType type)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "abort command");
  return commandInterface->abortAxisProfileCommand(type);
}

extern "C" comm::axisprofile::AxisprofileCommandState AXISPROFILE_getAxisProfileCommandState(void* handle, const comm::axisprofile::AxisProfileCommandType type)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisprofileCommandState::CMD_STATE_UNDEFINED, "get command state");
  return commandInterface->getAxisProfileCommandState(type);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setReferencingPosition(void* handle, const double refPos, const comm::axisprofile::AxisProfileEncoder encoder,  RefMode mode)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "set reference position");
  return commandInterface->setReferencingPosition(refPos, encoder, mode);
}
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getCommandsAtDeterminedState(void* handle, const comm::axisprofile::AxisprofileCommandState cmdState, std::vector<comm::axisprofile::AxisProfileCommandType>& vecCmdType)
{
  IAxisProfileCommand* commandInterface = static_cast<IAxisProfileCommand*>(handle);
  RETURN_HANDLE_INVALIDE(commandInterface, AxisProfileResult::ERR_EXEC_NULL, "get commands at determined state");
  return commandInterface->getCommandsAtDeterminedState(cmdState, vecCmdType);
}
