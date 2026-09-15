/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup of the axis profile property.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile_property.h"

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
extern "C" AxisProfileResult AXISPROFILE_setDeviceAddressPropertyMock(void* handle, std::uint32_t deviceAddress)
{
  AxisProfilePropertyMockHandle* mock = static_cast<AxisProfilePropertyMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  mock->m_deviceAddress = deviceAddress;
  return mock->m_retValue;
}

extern "C" std::uint32_t AXISPROFILE_getDeviceAddressPropertyMock(void* handle)
{
  const AxisProfilePropertyMockHandle* mock = static_cast<AxisProfilePropertyMockHandle*>(handle);
  if (!mock)
  {
    return 0;
  }
  return mock->m_deviceAddress;
}

extern "C" AxisProfileType AXISPROFILE_getProfileTypePropertyMock(void* handle)
{
  const AxisProfilePropertyMockHandle* mock = static_cast<AxisProfilePropertyMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileType::UNDEFINED;
  }
  return mock->m_profileType;
}

extern "C" AxisProfileResult AXISPROFILE_getDeviceStatePropertyMock(void* handle)
{
  const AxisProfilePropertyMockHandle* mock = static_cast<AxisProfilePropertyMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return mock->m_retValue;
}

extern "C" AxisProfileDeviceScalingType AXISPROFILE_getDeviceScalingTypePropertyMock(void* handle, comm::axisprofile::AxisProfileDeviceValueType /*valueType*/)
{
  const AxisProfilePropertyMockHandle* mock = static_cast<AxisProfilePropertyMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileDeviceScalingType::UNDEFINED;
  }
  return mock->m_deviceType;
}

extern "C" AxisProfileCategory AXISPROFILE_getProfileCategoryPropertyMock(void* handle)
{
  const AxisProfilePropertyMockHandle* mock = static_cast<AxisProfilePropertyMockHandle*>(handle);
  if (!mock)
  {
    return AxisProfileCategory::UNDEFINED;
  }
  return mock->m_profileCateogry;
}
