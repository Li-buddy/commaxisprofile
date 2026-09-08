/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup of the general axis profile interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile.h"

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

extern "C" const IAxisProfileCommandC* AXISPROFILE_getCommandInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock) { return nullptr; }
  return &mock->m_profileCommandMock;
}

extern "C" const IAxisProfileStatusControlC* AXISPROFILE_getStatusControlInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock) { return nullptr; }
  return &mock->m_profileStatusControlMock;
}

extern "C" const IAxisProfileDiagMsgC* AXISPROFILE_getDiagMsgInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock) { return nullptr; }
  return &mock->m_profileDiagMsgMock;
}

extern "C" const IAxisProfileCyclicDataC* AXISPROFILE_getCyclicDataInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock) { return nullptr; }
  return &mock->m_profileCyclicDataMock;
}

extern "C" const IAxisProfileBootingConfigC* AXISPROFILE_getBootingConfigInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock) { return nullptr; }
  return &mock->m_profileBootingConfigMock;
}

extern "C" const IAxisProfileDeviceParamC* AXISPROFILE_getDeviceParamInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock) { return nullptr; }
  return &mock->m_profileDeviceParamMock;
}

extern "C" const IAxisProfileProbeC* AXISPROFILE_getProbeInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock) { return nullptr; }
  return &mock->m_profileProbeMock;
}

extern "C" const IAxisProfileCmdC* AXISPROFILE_getCmdInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock) { return nullptr; }
  return &mock->m_profileCmdMock;
}

extern "C" const IAxisProfilePropertyC* AXISPROFILE_getPropertyInterfaceCMock(void* handle)
{
  const AxisProfileMockHandle* mock = static_cast<AxisProfileMockHandle*>(handle);
  if (!mock)
  {
    return nullptr;
  }
  return &mock->m_profilePropertyMock;
}
