/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup of axisprofile command interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile_cmd.h"

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

extern "C" CmdState AXISPROFILE_getCmdStateMock(void* handle, CmdType /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_cmdState : CmdState::STATE_UNKNOWN;
}

extern "C" AxisProfileResult AXISPROFILE_resetCmdMock(void* handle, CmdType /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_abortCmdMock(void* handle, CmdType /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" CmdStatusInDevice AXISPROFILE_getCmdStatusInDeviceMock(void* handle, CmdType /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_cmdStatusInDevice : CmdStatusInDevice::DEVICE_UNKNOWN;
}

extern "C" AxisProfileResult AXISPROFILE_clearDeviceErrorMockCmd(void* handle, CmdType& /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_clearDeviceError2MockCmd(void* handle)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_setAbsolutePositionMockCmd(void* handle, double /*newPos*/, AxisProfileEncoder /*encoder*/, CmdType& /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_referencingAxisMockCmd(void* handle, double /*newPos*/, AxisProfileEncoder /*encoder*/, CmdType& /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_fixedStopMockCmd(void* handle, double /*clampingTorque*/, CmdType& /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_setNewReferenceWithoutMovementMockCmd(void* handle, double /*newPos*/, AxisProfileEncoder /*encoder*/, RefMode /*refMode*/, CmdType& /*cmdType*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_setOperationModeMockCmd(void* handle, AxisProfileOperationMode /*opMode*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_setOperationModeCyclicMockCmd(void* handle, AxisProfileOperationMode /*opMode*/)
{
  const AxisProfileCmdMockHandle* mock = static_cast<AxisProfileCmdMockHandle*>(handle);
  return (mock) ? mock->m_retValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}
