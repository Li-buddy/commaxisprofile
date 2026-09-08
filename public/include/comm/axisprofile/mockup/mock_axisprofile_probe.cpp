/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup of the axis profile probe.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile_probe.h"

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
extern "C" AxisProfileResult AXISPROFILE_probeBaseConfigMock(void* handle, ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool /*isAutoActivated*/)
{
  AxisProfileProbeMockHandle* mock = static_cast<AxisProfileProbeMockHandle*>(handle);
  mock->m_ctrlWordConfig[probeIdx] = probeConfig;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_probeExtConfigMock(void* handle, ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig, bool /*isStateMonitActivated*/)
{
  AxisProfileProbeMockHandle* mock = static_cast<AxisProfileProbeMockHandle*>(handle);
  mock->m_extendedCtrlWordConfig[probeIdx] = probeExtendConfig;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_probeStatusCtrlMock(void* handle, ProbeIndex probeIdx, bool enable)
{
  AxisProfileProbeMockHandle* mock = static_cast<AxisProfileProbeMockHandle*>(handle);
  mock->m_probeStatusCtrl[probeIdx] = enable;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_getProbeActualValuesRTMock(void* handle, ProbeIndex probeIdx, ProbeActualValues* actualValueProbe)
{
  AxisProfileProbeMockHandle* mock = static_cast<AxisProfileProbeMockHandle*>(handle);

  auto probe = mock->m_probeActualValues.find(probeIdx);
  if (probe == mock->m_probeActualValues.end())
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  // adapt the probe configuration
  mock->checkControlWordConfig(probeIdx);
  mock->checkExtendedCtrlWordConfig(probeIdx);
  mock->checkIfProbeEnabled(probeIdx);
  mock->adapteProbeConfig(probeIdx);
  
  // output the actual values of current probe
  *actualValueProbe = probe->second;
  
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_onlineChangeParamsMock(void* handle, ProbeIndex /*probeIdx*/, ProbeUpdateMeasurement /*updateValue*/)
{
  const AxisProfileProbeMockHandle* mock = static_cast<AxisProfileProbeMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT; 
}

extern "C" AxisProfileResult AXISPROFILE_getProbeValuesMock(void* handle, ProbeIndex /*probeIdx*/, ProbeBaseConfig*  /*probeConfig*/, ProbeExtConfig* /*probeExtendConfig*/)
{
  const AxisProfileProbeMockHandle* mock = static_cast<AxisProfileProbeMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT; 
}

extern "C" AxisProfileResult AXISPROFILE_abortAndResetProbeMock(void* handle) 
{
  const AxisProfileProbeMockHandle* mock = static_cast<AxisProfileProbeMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT; 
}

extern "C" AxisProfileResult AXISPROFILE_isProbeSupportedMock(void* handle) 
{
  const AxisProfileProbeMockHandle* mock = static_cast<AxisProfileProbeMockHandle*>(handle);
  return (mock) ? mock->m_returnValue : AxisProfileResult::ERR_INVALID_ARGUMENT; 
}
