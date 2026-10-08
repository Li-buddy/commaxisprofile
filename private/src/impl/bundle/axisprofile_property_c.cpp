/// @file
///
/// @brief This file is part of the axisprofile library and define the property handle.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// --------------------------------------------------------------------------- 
#include "axisprofile_if_helper.h"
#include "axisprofile_property_c.h"
#include "i_axisprofile_property.h"

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

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setDeviceAddressProperty(void* handle, std::uint32_t deviceAddress)
{
  IAxisProfileProperty* propertyInterface = static_cast<IAxisProfileProperty*>(handle);
  RETURN_HANDLE_INVALIDE(propertyInterface, AxisProfileResult::ERR_EXEC_NULL, "set device address");
  return propertyInterface->setDeviceAddress(deviceAddress);
}

extern "C" std::uint32_t AXISPROFILE_getDeviceAddressProperty(void* handle)
{
  const IAxisProfileProperty* propertyInterface = static_cast<IAxisProfileProperty*>(handle);
  RETURN_HANDLE_INVALIDE(propertyInterface, 0u, "device address");
  return propertyInterface->getDeviceAddress();
}

extern "C" comm::axisprofile::AxisProfileType AXISPROFILE_getProfileTypeProperty(void* handle)
{
  const IAxisProfileProperty* propertyInterface = static_cast<IAxisProfileProperty*>(handle);
  RETURN_HANDLE_INVALIDE(propertyInterface, AxisProfileType::UNDEFINED, "get profile type");
  return propertyInterface->getProfileType();
}

extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getDeviceStateProperty(void* handle)
{
  IAxisProfileProperty* propertyInterface = static_cast<IAxisProfileProperty*>(handle);
  RETURN_HANDLE_INVALIDE(propertyInterface, AxisProfileResult::ERR_EXEC_NULL, "get device state");
  return propertyInterface->getDeviceState();
}

extern "C" comm::axisprofile::AxisProfileDeviceScalingType AXISPROFILE_getDeviceScalingTypeProperty(void* handle, comm::axisprofile::AxisProfileDeviceValueType valueType)
{
  const IAxisProfileProperty* propertyInterface = static_cast<IAxisProfileProperty*>(handle);
  RETURN_HANDLE_INVALIDE(propertyInterface, AxisProfileDeviceScalingType::UNDEFINED, "get device scaling type");
  return propertyInterface->getDeviceScalingType(valueType);
}

extern "C" comm::axisprofile::AxisProfileCategory AXISPROFILE_getProfileCategoryProperty(void* handle)
{
  IAxisProfileProperty* propertyInterface = static_cast<IAxisProfileProperty*>(handle);
  RETURN_HANDLE_INVALIDE(propertyInterface, AxisProfileCategory::UNDEFINED, "get profile category");
  return propertyInterface->getProfileCategory();
}

extern "C" const char* AXISPROFILE_getCfgedFieldbusMasterName(void* handle)
{
  const IAxisProfileProperty* propertyInterface = static_cast<IAxisProfileProperty*>(handle);
  RETURN_HANDLE_INVALIDE(propertyInterface, "", "get profile configured EtherCAT name");
  return propertyInterface->getEtherCATName();
}
