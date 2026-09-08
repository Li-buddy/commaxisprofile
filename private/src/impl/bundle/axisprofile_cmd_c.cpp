/// @file
///
/// @brief This file is part of the axisprofile library and implements the command handle.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_if_helper.h"
#include "i_axisprofile_cmd.h"

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
// Class implementation
// ---------------------------------------------------------------------------

extern "C" CmdState AXISPROFILE_getCmdState(void* handle, CmdType cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, CmdState::STATE_UNKNOWN, "getCmdState");
  return cmdIf->getCmdState(cmdType);
}

extern "C" AxisProfileResult AXISPROFILE_resetCmd(void* handle, CmdType cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "resetCmd");
  return cmdIf->resetCmd(cmdType);
}

extern "C" AxisProfileResult AXISPROFILE_abortCmd(void* handle, CmdType cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "abortCmd");
  return cmdIf->abortCmd(cmdType);
}

extern "C" CmdStatusInDevice AXISPROFILE_getCmdStatusInDevice(void* handle, CmdType cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, CmdStatusInDevice::DEVICE_UNKNOWN, "getCmdStatusInDevice");
  return cmdIf->getCmdStatusInDevice(cmdType);
}

extern "C" AxisProfileOperationMode AXISPROFILE_getOperationModeCmd(void* handle)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileOperationMode::UNDEFINED, "getOperationMode");
  return cmdIf->getOperationMode();
}

extern "C" AxisProfileResult AXISPROFILE_setOperationModeCmd(void* handle, const AxisProfileOperationMode opMode)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "setOperationMode");
  return cmdIf->setOperationMode(opMode);
}

extern "C" AxisProfileResult AXISPROFILE_clearDeviceErrorCmd(void* handle, CmdType& cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "clearDeviceError");
  return cmdIf->clearDeviceError(cmdType);
}

extern "C" AxisProfileResult AXISPROFILE_setAbsolutePositionCmd(void* handle, double newPos, AxisProfileEncoder encoder, CmdType& cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "setAbsolutePosition");
  return cmdIf->setAbsolutePosition(newPos, encoder, cmdType);
}

extern "C" AxisProfileResult AXISPROFILE_referencingAxisCmd(void* handle, double newPos, AxisProfileEncoder encoder, CmdType& cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "referencingAxis");
  return cmdIf->referencingAxis(newPos, encoder, cmdType);
}

extern "C" AxisProfileResult AXISPROFILE_fixedStopCmd(void* handle, double clampingTorque, CmdType& cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "fixedStop");
  return cmdIf->fixedStop(clampingTorque, cmdType);
}

extern "C" AxisProfileResult AXISPROFILE_clearDeviceError2Cmd(void* handle)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "clearDeviceError2");
  return cmdIf->clearDeviceError2();
}

extern "C" AxisProfileResult AXISPROFILE_setNewReferenceWithoutMovement(void* handle, double newPos, AxisProfileEncoder encoder, RefMode refMode, CmdType& cmdType)
{
  IAxisProfileCmd* cmdIf = static_cast<IAxisProfileCmd*>(handle);
  RETURN_HANDLE_INVALIDE(cmdIf, AxisProfileResult::ERR_EXEC_NULL, "setNewReferenceWithoutMovement");
  return cmdIf->setNewReferenceWithoutMovement(newPos, encoder, refMode, cmdType);
}
