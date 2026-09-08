/// @file
///
/// @brief This file is part of the axisprofile library and implements the
///        C/C++ method mapping for the base class for cyclic data handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_cyclic_data_c.h"
#include "axisprofile_if_helper.h"

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

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getActualValuesDevice(void* handle, comm::axisprofile::ActualValuesDevice* actualValues)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, AxisProfileResult::ERR_EXEC_NULL, "get actual device values");
  return cyclicDataInterface->getActualValuesDevice(actualValues);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setCommandValuesDevice(void* handle, comm::axisprofile::CommandValuesDevice commandValues)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, AxisProfileResult::ERR_EXEC_NULL, "set command device values");
  return cyclicDataInterface->setCommandValuesDevice(commandValues);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getDigitalInputValueBit(void* handle, comm::axisprofile::DigitalInputValueBitType valueType, bool& inputValue)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, AxisProfileResult::ERR_EXEC_NULL, "get the specific bit value of digital input");
  return cyclicDataInterface->getDigitalInputValueBit(valueType, inputValue);
}

extern "C" bool AXISPROFILE_checkActualValueAvailabilityNRT(void* handle, comm::axisprofile::AxisProfileValueTypeID valueType, DataAccessErrorInfo& errorInfo)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, false, "check the actual value availability");
  return cyclicDataInterface->checkActualValueAvailabilityNRT(valueType, errorInfo);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getSafetyOperationMode(void* handle, comm::axisprofile::SafetyOperationMode& mode)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, AxisProfileResult::ERR_EXEC_NULL, "get the specific value of drive");
  return cyclicDataInterface->getSafetyOperationMode(mode);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getSingleUserPDODataFromDevice(void* handle, comm::axisprofile::UserPDOID id, comm::axisprofile::UserPDODataInfo & actualPDOValues)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, AxisProfileResult::ERR_EXEC_NULL, "get the specific value of drive");
  return cyclicDataInterface->getSingleUserPDODataFromDevice(id, actualPDOValues);
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setSingleUserPDODataToDevice(void* handle, comm::axisprofile::UserPDOID id, comm::axisprofile::UserPDODataInfo commandValues)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, AxisProfileResult::ERR_EXEC_NULL, "get the specific value of drive");
  return cyclicDataInterface->setSingleUserPDODataToDevice(id, commandValues);
}

extern "C" bool AXISPROFILE_checkSingleUserPDOActiveFromDevice(void* handle, comm::axisprofile::UserPDOID id)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, false, "get the specific value of drive");
  return cyclicDataInterface->checkSingleUserPDOActiveFromDevice(id);
}

extern "C" bool AXISPROFILE_checkSingleUserPDOActiveToDevice(void* handle, comm::axisprofile::UserPDOID id)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, false, "get the specific value of drive");
  return cyclicDataInterface->checkSingleUserPDOActiveToDevice(id);
}

extern "C" double AXISPROFILE_getEffectiveTimeOfActOutput(void* handle)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, 0.0, "get effective time of actual output");
  return cyclicDataInterface->getEffectiveTimeOfActOutput();
}

extern "C" double AXISPROFILE_getLatchTimeOfActInput(void* handle)
{
  IAxisProfileCyclicData* cyclicDataInterface = static_cast<IAxisProfileCyclicData*>(handle);
  RETURN_HANDLE_INVALIDE(cyclicDataInterface, 0.0, "get latch time of actual input");
  return cyclicDataInterface->getLatchTimeOfActInput();
}
