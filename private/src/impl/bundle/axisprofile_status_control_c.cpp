/// @file
///
/// @brief This file is part of the axisprofile library and implements
///        C/C++ method mapping for the base class for status- and controlword handling.
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
#include "axisprofile_status_control_c.h"

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------- Statusword ----------------------------------------------- //

extern "C" std::uint16_t AXISPROFILE_getStatusWord(void* handle)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, 0, "get Statusword");
  return statusControlInterface->getStatusWord();
}

extern "C" bool AXISPROFILE_checkOperationMode(void* handle, const comm::axisprofile::AxisProfileOperationMode opMode)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, false, "check operation mode");
  return statusControlInterface->checkOperationMode(opMode);
}

extern "C" bool AXISPROFILE_checkOperationModeSupported(void* handle, const comm::axisprofile::AxisProfileOperationMode opMode, comm::axisprofile::OpModeErrorInfo& errorInfo)
{
  IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, false, "check operation mode");
  return statusControlInterface->checkOperationModeSupported(opMode, errorInfo);
}

extern "C" comm::axisprofile::AxisProfileOperationMode AXISPROFILE_getOperationMode(void* handle)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, AxisProfileOperationMode::UNDEFINED, "get operation mode");
  return statusControlInterface->getOperationMode();
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_readOperationMode(void* handle, comm::axisprofile::AxisProfileOperationMode& operationMode)
{
  IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, AxisProfileResult::ERR_EXEC_NULL, "read operation mode");
  return statusControlInterface->readOperationMode(operationMode);
}

extern "C" bool AXISPROFILE_checkPowerMode(void* handle, const comm::axisprofile::AxisProfilePowerMode powerMode)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, false, "check power mode");
  return statusControlInterface->checkPowerMode(powerMode);
}

extern "C" comm::axisprofile::AxisProfilePowerMode AXISPROFILE_getPowerMode(void* handle)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, AxisProfilePowerMode::UNDEFINED, "get power mode");
  return statusControlInterface->getPowerMode();
}

extern "C" bool AXISPROFILE_checkError(void* handle)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, false, "get power mode");
  return statusControlInterface->checkError();
}

extern "C" bool AXISPROFILE_cyclicStandStillCheck(void* handle)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, false, "check standstill");
  return statusControlInterface->cyclicStandStillCheck();
}

extern "C" bool AXISPROFILE_isDeviceFollowsControl(void* handle)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, false, "check if device follows control");
  return statusControlInterface->isDeviceFollowsControl();
}

extern "C" bool AXISPROFILE_deviceHasWarning(void* handle)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, false, "check if device has waning");
  return statusControlInterface->deviceHasWarning();
}

extern "C" ReadyToOperate AXISPROFILE_checkReadyToOperate(void* handle)
{
  IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, ReadyToOperate::DRIVE_NOT_READY, "check if device is ready to operate");
  return statusControlInterface->checkReadyToOperate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------- Controlword ----------------------------------------------- //

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setControlWord(void* handle, std::uint16_t controlWord)
{
  IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, AxisProfileResult::ERR_EXEC_NULL, "set controlword");
  return statusControlInterface->setControlWord(controlWord);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setOperationMode(void* handle, const comm::axisprofile::AxisProfileOperationMode opMode)
{
  IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, AxisProfileResult::ERR_EXEC_NULL, "set operation mode");
  return statusControlInterface->setOperationMode(opMode);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setPowerMode(void* handle, const comm::axisprofile::AxisProfilePowerMode powerMode)
{
  IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, AxisProfileResult::ERR_EXEC_NULL, "set power mode");
  return statusControlInterface->setPowerMode(powerMode);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------- Drive Halt ----------------------------------------------- //

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setDriveHaltMode(void *handle, const bool haltEnable)
{
  IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, AxisProfileResult::ERR_EXEC_NULL, "set drive halt into device");
  return statusControlInterface->setDriveHaltMode(haltEnable);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getDriveHaltMode(void *handle, bool& isHaltEnabled)
{
  const IAxisProfileStatusControl* statusControlInterface = static_cast<IAxisProfileStatusControl*>(handle);
  RETURN_HANDLE_INVALIDE(statusControlInterface, AxisProfileResult::ERR_EXEC_NULL, "get the drive halt mode");
  return statusControlInterface->getDriveHaltMode(isHaltEnabled);
}
