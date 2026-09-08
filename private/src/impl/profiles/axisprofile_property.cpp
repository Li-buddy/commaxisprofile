/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for property handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_property.h"
#include "bundle/axisprofile_property_c.h"
#include "profiles/category/category_adapter_factory.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "helper/device_id_helper.h"

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

AxisProfileProperty::AxisProfileProperty(ProfileBasicInfo& profileBasicInfo)
  :m_profileBasicInfo(profileBasicInfo), m_sharingHandler(nullptr)
{
  std::memset(&m_propertyInterface, 0, sizeof(m_propertyInterface));
  m_propertyInterface.handle = this;
  m_propertyInterface.setDeviceAddress = AXISPROFILE_setDeviceAddressProperty;
  m_propertyInterface.getDeviceAddress = AXISPROFILE_getDeviceAddressProperty;
  m_propertyInterface.getProfileType = AXISPROFILE_getProfileTypeProperty;
  m_propertyInterface.getDeviceScalingType = AXISPROFILE_getDeviceScalingTypeProperty;
  m_propertyInterface.getDeviceState = AXISPROFILE_getDeviceStateProperty;
  m_propertyInterface.getProfileCategory = AXISPROFILE_getProfileCategoryProperty;
  m_propertyInterface.getCfgedFieldbusMasterName = AXISPROFILE_getCfgedFieldbusMasterName;
}

const IAxisProfilePropertyC* AxisProfileProperty::getCInterface() const
{
  return &m_propertyInterface;
}

AxisProfileResult AxisProfileProperty::setDeviceAddress(std::uint32_t deviceAddress)
{
  m_profileBasicInfo.m_deviceAddress = deviceAddress;
  m_profileBasicInfo.m_profileTrace.updateDeviceAddress(m_profileBasicInfo.m_deviceAddress);
  return AxisProfileResult::STS_OK;
}

std::uint32_t AxisProfileProperty::getDeviceAddress() const
{
  return m_profileBasicInfo.m_deviceAddress;;
}

AxisProfileType AxisProfileProperty::getProfileType() const
{
  return m_profileBasicInfo.m_axisProfileType;
}

AxisProfileDeviceScalingType AxisProfileProperty::getDeviceScalingType(AxisProfileDeviceValueType valueType) const
{
  if (!m_sharingHandler->getCategoryIF() || !m_sharingHandler->getCategoryIF()->getScalingInternal())
  {
    return AxisProfileDeviceScalingType::UNDEFINED;
  }
  return m_sharingHandler->getCategoryIF()->getScalingInternal()->getDeviceScalingType(valueType);
}

AxisProfileResult AxisProfileProperty::setDeviceName(const char* cDeviceName)
{
  m_deviceName = cDeviceName;
  return AxisProfileResult::STS_OK;
}

const char* AxisProfileProperty::getDeviceName() const
{
  return m_deviceName.c_str();
}

AxisProfileCategory AxisProfileProperty::getProfileCategory()
{
  if (!m_sharingHandler->getCategoryIF())
  {
    return AxisProfileCategory::UNDEFINED;
  }

  return m_sharingHandler->getCategoryIF()->getProfileCategory();
}

AxisProfileResult AxisProfileProperty::getDeviceState()
{
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

void AxisProfileProperty::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

AxisProfileResult AxisProfileProperty::setDeviceIdentification(std::uint32_t venderId, std::uint32_t productCode)
{
  APR_TRACE_WARNING("%s, verderId: 0x%8X, produceCode 0x%8X", m_profileBasicInfo.m_profileName.c_str(), venderId, productCode);
  m_deviceSeries = DeviceIdHelper::calculateDeviceSeries(venderId, productCode);
  return AxisProfileResult::STS_OK;
}

DeviceSeries AxisProfileProperty::getDeviceSeries() const
{
  return m_deviceSeries;
}

AxisProfileResult AxisProfileProperty::setDeviceManufacturerVersion(const std::string& version)
{
  m_softwareVersion.valid = false;
  m_softwareVersion.versionStr = DeviceIdHelper::extractFirmwareByStr(m_deviceSeries, version);
  if (m_softwareVersion.versionStr.empty())
  {
    APR_TRACE_ERROR("set manufacturer version error. version: %s.", version.c_str());
    return AxisProfileResult::ERR_NOK;
  }

  if (!DeviceIdHelper::convertSoftwareVersion(m_softwareVersion.versionStr, m_softwareVersion.majorVersion, m_softwareVersion.minorVersion))
  {
    APR_TRACE_ERROR("set extract software version error. software version:%s.", m_softwareVersion.versionStr.c_str());
    return AxisProfileResult::ERR_NOK;
  }
  m_softwareVersion.valid = true;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileProperty::getDeviceSoftwareVersion(std::uint8_t& majorVersion, std::uint8_t& minorVersion) const
{
  if (!m_softwareVersion.valid)
  {
    APR_TRACE_WARNING("get device software version error, maybe setDeviceManufacturerVersion function occur error");
    return AxisProfileResult::ERR_NOK;
  }
  majorVersion = m_softwareVersion.majorVersion;
  minorVersion = m_softwareVersion.minorVersion;
  return AxisProfileResult::STS_OK;
}

const std::string& AxisProfileProperty::getDeviceSoftwareVersion() const
{
  return m_softwareVersion.versionStr;
}

void AxisProfileProperty::enableChecksum(bool enable)
{
  m_enableChecksum = enable;
}

bool AxisProfileProperty::isChecksumEnabled() const
{
  return m_enableChecksum;
}

const char* AxisProfileProperty::getEtherCATName() const
{
  return m_ethercatName.c_str();
}

AxisProfileResult AxisProfileProperty::setEtherCATName(const char* name)
{
  m_ethercatName = name;
  return AxisProfileResult::STS_OK;
}
