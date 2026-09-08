/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup of the axis profile command interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile_command.h"

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
// Declaration of C-Interface Method-Mockups
// ---------------------------------------------------------------------------

extern "C" AxisProfileResult AXISPROFILE_clearDeviceErrorMock(void* handle)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_setAndCheckPositiveStopMock(void* handle, const comm::axisprofile::AxisProfilePositiveStop /*mode*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_setAndCheckClampingTorqueDeprecatedMock(void* handle, const double /*clampingTorque*/, const bool /*metrication*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_getClampingTorqueDeprecatedMock(void* handle, double* const /*clampingTorque*/, const bool /*metrication*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_checkStandStillMock(void* handle, bool* const /*standStill*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_setAndCheckstandStillWindowMock(void* handle, const double /*standStillWindow*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_getStandStillWindowMock(void* handle, double* const /*standStillWindow*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_activateCyclicStandStillCheckMock(void* handle)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_deactivateCyclicStandStillCheckMock(void* handle)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setAbsolutePositionMock(void *handle, const double /*position*/, const comm::axisprofile::AxisProfileEncoder /*encoder*/, const bool /*ifDriveEnabled*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_resetAbsolutePositionMock(void *handle)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_referencingAxisMock(void *handle, const double /* newRefPos */, const comm::axisprofile::AxisProfileEncoder /* encoder */)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_resetReferencingAxisMock(void *handle)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_resetAxisprofileCommandStateMock(void* handle, const comm::axisprofile::AxisProfileCommandType /*type*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_abortAxisprofileCommandMock(void* handle, const comm::axisprofile::AxisProfileCommandType /*type*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisprofileCommandState AXISPROFILE_getCommandStateMock(void* handle, const comm::axisprofile::AxisProfileCommandType /*type*/)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  return (mock) ? mock->m_cmdState : AxisprofileCommandState::CMD_STATE_UNDEFINED;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setReferencingPositionMock(void *handle, const double refPos, const comm::axisprofile::AxisProfileEncoder /*encoder*/, RefMode /*method*/)
{
  AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  if (--mock->m_asyncSimCnter) // for simulation of motion function test
  {
    mock->m_refPos = refPos;
    mock->m_isHomed = true;
    return AxisProfileResult::STS_PENDING;
  }

  mock->m_asyncSimCnter = ASYNC_CYCLES_SIMULATION;
  return mock->m_returnValue;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_checkAxisHomedMockDeprecated(void* handle, bool& isHomed)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  isHomed = mock->m_isHomed;
  return mock->m_returnValue;
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getCommandsAtDeterminedStateMock(void* handle, comm::axisprofile::AxisprofileCommandState /* cmdState */, std::vector<comm::axisprofile::AxisProfileCommandType>& vecCmdType)
{
  const AxisProfileCommandMockHandle* mock = static_cast<AxisProfileCommandMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  vecCmdType = mock->m_commands;
  return mock->m_returnValue;
}
