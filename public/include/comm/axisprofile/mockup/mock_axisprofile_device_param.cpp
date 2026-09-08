/// @file
///
/// @brief This file is part of the axisprofile library and implements a mockup for the axis profile device parameter. 
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile_device_param.h"
#include <cmath>

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

extern "C" bool AXISPROFILE_checkBrakeDelaySupportMock(void* handle)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  if (!mock) { return false; }
  return mock->m_brakeDelaySupport;
}

extern "C" AxisProfileResult AXISPROFILE_getErrorBrakeDelayInfoMock(void* handle, const comm::axisprofile::AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTimeMs)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  if (mock)
  {
    auto it = mock->m_devRxnMockData.find(errorType);
    if (it != mock->m_devRxnMockData.end())
    {
      isActivated = it->second.m_brkDlyActivated;
      delayTimeMs = it->second.m_brkDlyTimeMs;
      return it->second.m_retToGetErrorBrakeDelayInfo;
    }
  }

  return AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_getErrorBrakeDynLimitMock(void* handle, const comm::axisprofile::AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  if (mock)
  {
    auto it = mock->m_devRxnMockData.find(errorType);
    if (it != mock->m_devRxnMockData.end())
    {
      deceleration = it->second.m_dec;
      jerk = it->second.m_jrk;
      return it->second.m_retTogetErrorBrakeDynLimit;
    }
  }

  return AxisProfileResult::ERR_INVALID_ARGUMENT;
}

extern "C" AxisProfileResult AXISPROFILE_getPositionDataFormatMock(void* handle, comm::axisprofile::AxisProfilePosDataFormat & posDataFormat)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  posDataFormat = mock->m_devicePosFormatMock;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_checkAndSetModuloMock(void* handle, double modValue, std::string& /*dynErrTxt*/)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  if (fabs(modValue - mock->m_modValue) > 1E-6)
  {
    return AxisProfileResult::ERR_INCONSISTENCY;
  }

  return mock->m_returnValue;
}

extern "C" bool AXISPROFILE_checkFunctionSupportedMock(void* handle, const FunctionSupport /*function*/, FunctionSupportErrorInfo & errorInfo)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  errorInfo = {FuncSupportErrorType::ERROR_NOT_DEFINED, "This is a mockup for testing"};
  if (!mock)
  {
    errorInfo = {FuncSupportErrorType::ERROR_NOT_DEFINED, "Invalid Interface-Handle"};
    return false;
  }
  return mock->m_funcSupported;
}

extern "C" AxisProfileResult AXISPROFILE_checkStandstillMock(void* handle, bool& standStill)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  standStill = mock->m_standStill;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_getRateTorqueForceMock(void* handle, double& rateTorqueForce, std::string& /*dynErrTxt*/)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  rateTorqueForce = mock->m_rateTorqueForce;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_checkAxisHomedMock(void* handle, bool& homed)
{
  const AxisProfileDeviceParamMockHandle* mock = static_cast<AxisProfileDeviceParamMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  homed = mock->m_isHomed;
  return mock->m_returnValue; 
}
