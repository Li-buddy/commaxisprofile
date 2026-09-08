/// @file
///
/// @brief This source file is part of the axisprofile library and implements the
/// C/C++ method mapping for the base class for device parameter access.
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
#include "i_axisprofile_device_param.h"

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

extern "C" bool AXISPROFILE_checkBrakeDelaySupport(void* handle)
{
  const IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
  RETURN_HANDLE_INVALIDE(deviceParamInterface, false, "get brake delay support on current device");
  return deviceParamInterface->checkBrakeDelaySupport();
}

extern "C" AxisProfileResult AXISPROFILE_getErrorBrakeDelayInfo(void* handle, const AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTimeMs)
{
  IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
  RETURN_HANDLE_INVALIDE(deviceParamInterface, AxisProfileResult::ERR_EXEC_NULL, "get brake delay information");
  return deviceParamInterface->getErrorBrakeDelayInfo(errorType, isActivated, delayTimeMs);
}

extern "C" AxisProfileResult AXISPROFILE_getErrorBrakeDynLimit(void* handle, const AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk)
{
  IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
 RETURN_HANDLE_INVALIDE(deviceParamInterface, AxisProfileResult::ERR_EXEC_NULL, "get multi brake dynamic limit");
 return deviceParamInterface->getErrorBrakeDynLimit(errorType, deceleration, jerk);
}

extern "C" AxisProfileResult AXISPROFILE_getPositionDataFormat(void* handle, AxisProfilePosDataFormat& posDataFormat)
{
  IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
  RETURN_HANDLE_INVALIDE(deviceParamInterface, AxisProfileResult::ERR_EXEC_NULL, "get position data format from current device");
  return deviceParamInterface->getPositionDataFormat(posDataFormat);
}

extern "C" AxisProfileResult AXISPROFILE_checkAndSetModulo(void* handle, double modValue, std::string & dynErrTxt)
{
  IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
  RETURN_HANDLE_INVALIDE(deviceParamInterface, AxisProfileResult::ERR_EXEC_NULL, "check and set modulo value");
  return deviceParamInterface->checkAndSetModulo(modValue, dynErrTxt);
}

extern "C" bool AXISPROFILE_checkFunctionSupported(void* handle, const FunctionSupport function, FunctionSupportErrorInfo & errorInfo)
{
  IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
  if (!deviceParamInterface)
  {
    APR_TRACE_ERROR("Could not check function supported information, Interface-Handle is missing or cast failed!");
    errorInfo.errorType = FuncSupportErrorType::ERROR_NOT_DEFINED;
    errorInfo.addInfo   = "Invalid Interface-Handle";
    return false;
  }
  return deviceParamInterface->checkFunctionSupported(function, errorInfo);
}

extern "C" AxisProfileResult AXISPROFILE_checkStandstill(void* handle, bool& standStill)
{
  IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
  RETURN_HANDLE_INVALIDE(deviceParamInterface, AxisProfileResult::ERR_EXEC_NULL, "check standstill");
  return deviceParamInterface->checkStandstill(standStill);
}

extern "C" AxisProfileResult AXISPROFILE_getRateTorqueForce(void* handle, double& rateTorqueForce, std::string& dynErrTxt)
{
  IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
  RETURN_HANDLE_INVALIDE(deviceParamInterface, AxisProfileResult::ERR_EXEC_NULL, "get rate torque force");
  return deviceParamInterface->getRateTorqueForce(rateTorqueForce, dynErrTxt);
}

extern "C" AxisProfileResult AXISPROFILE_checkAxisHomed(void* handle, bool& homed)
{
  IAxisProfileDeviceParam* deviceParamInterface = static_cast<IAxisProfileDeviceParam*>(handle);
  RETURN_HANDLE_INVALIDE(deviceParamInterface, AxisProfileResult::ERR_EXEC_NULL, "check axis homed");
  return deviceParamInterface->checkAxisHomed(homed);
}
