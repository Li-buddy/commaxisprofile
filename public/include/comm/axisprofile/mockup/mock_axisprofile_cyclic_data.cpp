/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup of the axis profile cyclic data interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile_cyclic_data.h"

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

extern "C" AxisProfileResult AXISPROFILE_getActualValuesDeviceMock(void* handle, ActualValuesDevice* actualValues)
{
  const AxisProfileCyclicDataMockHandle* mock = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!mock || !actualValues) { return AxisProfileResult::ERR_INVALID_ARGUMENT; }
  actualValues->m_actualPosition = mock->m_actualValues.m_actualPosition;
  actualValues->m_actualVelocity = mock->m_actualValues.m_actualVelocity;
  actualValues->m_actualTorqueForce = mock->m_actualValues.m_actualTorqueForce;
  actualValues->m_dataValid = mock->m_actualValues.m_dataValid;
  actualValues->m_torqueLimitStatus = mock->m_actualValues.m_torqueLimitStatus;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_setCommandValuesDeviceMock(void* handle, CommandValuesDevice commandValues)
{
  AxisProfileCyclicDataMockHandle* mock = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!mock) { return AxisProfileResult::ERR_INVALID_ARGUMENT; }

  if (mock->m_copyCmdToActualValues)
  {
    mock->m_actualValues.m_actualPosition = commandValues.m_commandPosition;
    mock->m_actualValues.m_actualVelocity = commandValues.m_commandVelocity;
    mock->m_actualValues.m_actualTorqueForce = commandValues.m_commandTorqueForce;
    mock->m_actualValues.m_torqueLimitStatus = TorqueVelocityLimitStatus::NO_VIOLATE;
  }
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_getDigitalInputValueBitMock(void* handle, DigitalInputValueBitType valueType, bool& inputValue)
{
  const AxisProfileCyclicDataMockHandle* mock = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!mock) { return AxisProfileResult::ERR_INVALID_ARGUMENT; }
  auto it = mock->m_digitalInputValueBit.find(valueType);
  if (it == mock->m_digitalInputValueBit.end())
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  inputValue = it->second;
  return mock->m_returnValue;
}

extern "C" bool AXISPROFILE_checkActualValueAvailabilityNRTMock(void* handle, AxisProfileValueTypeID /*valueType*/, DataAccessErrorInfo & errorInfo)
{
  const AxisProfileCyclicDataMockHandle* mock = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!mock)
  {
    errorInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_NOT_PROVIDED;
    return false;
  }
  errorInfo = mock->m_mockDataAccessErrorInfo;
  return mock->m_bMockAvailableNRT;
}
    
extern "C" AxisProfileResult AXISPROFILE_getSafetyOperationModeMock(void* handle, SafetyOperationMode& mode)
{
  const AxisProfileCyclicDataMockHandle* mock = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!mock)
  { 
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  mode = mock->m_curSafetyOperationMode;
  return mock->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_getSingleUserPDODataFromDeviceMock(void* handle, UserPDOID /*id*/, UserPDODataInfo& actualPDOValues)
{
  const AxisProfileCyclicDataMockHandle* pCylicDataInterface = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!pCylicDataInterface)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  actualPDOValues = pCylicDataInterface->m_mockUserPDOData;
  return pCylicDataInterface->m_returnValue;
}

extern "C" AxisProfileResult AXISPROFILE_setSingleUserPDODataToDeviceMock(void* handle, UserPDOID /*id*/, UserPDODataInfo newUserPDOInfo)
{
  AxisProfileCyclicDataMockHandle* pCylicDataInterface = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!pCylicDataInterface)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  pCylicDataInterface->m_mockUserPDOData = newUserPDOInfo;
  return pCylicDataInterface->m_returnValue;
}

extern "C" bool AXISPROFILE_checkSingleUserPDOActiveFromDeviceMock(void* handle, UserPDOID /*id*/)
{
  const AxisProfileCyclicDataMockHandle* pCylicDataInterface = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!pCylicDataInterface)
  {
    return false;
  }
  return pCylicDataInterface->m_bMockSingleUserPDOActiveFromDevice;
}

extern "C" bool AXISPROFILE_checkSingleUserPDOActiveToDeviceMock(void* handle, comm::axisprofile::UserPDOID /*id*/)
{
  const AxisProfileCyclicDataMockHandle* pCylicDataInterface = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!pCylicDataInterface)
  {
    return false;
  }
  return pCylicDataInterface->m_bMockSingleUserPDOActiveToDevice;
}

extern "C" double AXISPROFILE_getEffectiveTimeOfActOutputMock(void* handle)
{
  const AxisProfileCyclicDataMockHandle* mock = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!mock)
  {
    return 0.0;
  }
  return mock->m_mockEffectiveTimeOfActOutput;
}

extern "C" double AXISPROFILE_getLatchTimeOfActInputMock(void* handle)
{
  const AxisProfileCyclicDataMockHandle* mock = static_cast<AxisProfileCyclicDataMockHandle*>(handle);
  if (!mock)
  {
    return 0.0;
  }
  return mock->m_mockLatchTimeOfActInput;
}
