/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup of the axis profile status/control interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile_status_control.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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
// Declaration of classes and structures
// ---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------- Statusword ----------------------------------------------- //
extern "C" std::uint16_t AXISPROFILE_getStatusWordMock(void* /*handle*/)
{
  return 0;
}

extern "C" bool AXISPROFILE_checkOperationModeMock(void* /*handle*/, const AxisProfileOperationMode /*opMode*/)
{
  return true;
}

extern "C" bool AXISPROFILE_checkOperationModeSupportedMock(void* handle, const AxisProfileOperationMode /*opMode*/, comm::axisprofile::OpModeErrorInfo& errorInfo)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  if (!mock) { return false; }
  errorInfo.m_error = mock->m_opModeErrorInfo.m_error;
  errorInfo.m_addInfo = mock->m_opModeErrorInfo.m_addInfo;
  return  mock->m_opModeSupported;
}

extern "C" AxisProfileOperationMode AXISPROFILE_getOperationModeMock(void* handle)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  return (mock) ? mock->m_operationMode : AxisProfileOperationMode::UNDEFINED;
}

extern "C" bool AXISPROFILE_checkPowerModeMock(void* /*handle*/, const AxisProfilePowerMode /*powerMode*/)
{
  return true;
}

extern "C" AxisProfilePowerMode AXISPROFILE_getPowerModeMock(void* handle)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  return (mock) ? mock->m_powerMode : AxisProfilePowerMode::UNDEFINED;
}

extern "C" bool AXISPROFILE_checkErrorMock(void* handle)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  return (mock) ? mock->m_deviceError : false;
}

extern "C" bool AXISPROFILE_cyclicStandStillCheckMock(void* handle)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  return (mock) ? mock->m_standStill : false;
}

extern "C" bool AXISPROFILE_isDeviceFollowsControl(void* handle)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  return (mock) ? mock->m_isDeviceFollowsControl : false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------- Controlword ----------------------------------------------- //

extern "C" AxisProfileResult AXISPROFILE_setControlWordMock(void* handle, std::uint16_t /*controlWord*/)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_setOperationModeMock(void* handle, const AxisProfileOperationMode opMode)
{
  AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  if (!mock) { return AxisProfileResult::ERR_INVALID_ARGUMENT; }
  mock->m_operationMode = opMode;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_setPowerModeMock(void* handle, const AxisProfilePowerMode powerMode)
{
  AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  if (!mock) { return AxisProfileResult::ERR_INVALID_ARGUMENT; }
  mock->m_powerMode = powerMode;
  return mock->m_returnValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------- Drive Halt ----------------------------------------------- //

extern "C" AxisProfileResult AXISPROFILE_setDriveHaltModeMock(void *handle, const bool haltEnable)
{
  AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  if (!mock) { return AxisProfileResult::ERR_INVALID_ARGUMENT; }
  mock->m_haltEnable = haltEnable;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_getDriveHaltModeMock(void *handle, bool& isHaltEnabled)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  if (!mock) { return AxisProfileResult::ERR_INVALID_ARGUMENT; }
  isHaltEnabled = mock->m_haltEnable;
  return mock->m_returnValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------- Statusword ----------------------------------------------- //
extern "C" bool AXISPROFILE_deviceHasWarningMock(void* handle)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  return (mock ? mock->m_deviceHasWarning : false);
}

extern "C" ReadyToOperate AXISPROFILE_checkReadyToOperateMock(void* handle)
{
  const AxisProfileStatusControlMockHandle* mock = static_cast<AxisProfileStatusControlMockHandle*>(handle);
  return (mock ? mock->m_readyToOperate : ReadyToOperate::DRIVE_NOT_READY);
}
