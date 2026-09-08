/// @file
///
/// @brief This file is part of the axisprofile library and implements the helper class to determine device identification.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "helper/device_id_helper.h"
#include "common/string_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

//const std::map<uint32_t, DeviceSeries> VENDOR_WHITE_LIST = {
//  {0x00100000, DeviceSeries::INNOVANCE},
//  {0x00000168, DeviceSeries::MOON},
//};

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

bool DeviceIdHelper::isRexrothDrive(DeviceSeries devSeries)
{
  return devSeries == DeviceSeries::INDRADRIVE || isRexrothCtrlXDrive(devSeries);
}

bool DeviceIdHelper::isRexrothCtrlXDrive(DeviceSeries devSeries)
{
  return devSeries == DeviceSeries::CTRLXDRIVE || devSeries == DeviceSeries::CTRLXDRIVE_XC20;
}

DeviceSeries DeviceIdHelper::calculateDeviceSeries(std::uint32_t vendorId, std::uint32_t productCode)
{
  if (!isRexrothDevice(vendorId))
  {
    //if (isDeviceInWhiteList(vendorId))
    //  return DeviceSeries::WHITE_LIST;
    return DeviceSeries::NOTINDICATES;
  }

  std::uint16_t highWord = static_cast<std::uint16_t>(productCode >> 16);

  switch (highWord)
  {
    case INDRA_DRIVE:
      return DeviceSeries::INDRADRIVE;

    case CTRLX_IP20_MODULAR:
    case CTRLX_COMPACT:
    case CTRLX_IP65_CABINETFREE:
      return DeviceSeries::CTRLXDRIVE;

    case CTRLX_XC20:
      return DeviceSeries::CTRLXDRIVE_XC20;
    default:
      return DeviceSeries::NOTINDICATES;
  }

}

std::string DeviceIdHelper::extractFirmwareByStr(DeviceSeries devSeries, const std::string& manufacturerVersion)
{
  // get version number+release for ctrl X
  // the information defined in Manufacturer software version from 14th to 17th letter
  std::string ret;
  // eg.FWA-XD1-AXS-V-0504N-NN-00
  if(isRexrothCtrlXDrive(devSeries))
  {
    if (manufacturerVersion.size() > 14)
    {
      ret = manufacturerVersion.substr(14, 4);
    }
  }

  // eg.FWA-INDRV*-MPB-20V28-D5-1-SNC-NN
  if (DeviceSeries::INDRADRIVE == devSeries)
  {
    if (manufacturerVersion.size() > 18)
    {
      ret = manufacturerVersion.substr(15, 2);
      ret += manufacturerVersion.substr(18, 2);
    }
  }

  return ret;
}

bool DeviceIdHelper::convertSoftwareVersion(const std::string& versionStr, std::uint8_t& majorVersion, std::uint8_t& minorVersion)
{
  if (versionStr.length() != 4)
  {
    return false;
  }
  std::int32_t major = safeConvertToInt(versionStr.substr(0, 2), 10);
  std::int32_t minor = safeConvertToInt(versionStr.substr(2, 2), 10);
  bool ret = (major != INT_MIN && minor != INT_MIN);
  if (ret)
  {
    majorVersion = major;
    minorVersion = minor;
  }
  return ret;
}

bool DeviceIdHelper::isRexrothDevice(std::uint32_t vendorId)
{
  return vendorId == REXROTH_VENDOR_ID;
}

//bool DeviceJudgeHelper::isDeviceInWhiteList(std::uint32_t vendorId)
//{
//  return VENDOR_WHITE_LIST.find(vendorId) != VENDOR_WHITE_LIST.cend();
//}

bool DeviceIdHelper::deviceSupportCheckSum(uint8_t majorV, uint8_t minorV, DeviceSeries series, AxisProfileType profileType)
{
  bool isNewVersion = (majorV > firmware_related::CHECKSUM_MAJOR_VERSION) ||
    (majorV == firmware_related::CHECKSUM_MAJOR_VERSION && minorV >= firmware_related::CHECKSUM_MINOR_VERSION);

  if (profileType == AxisProfileType::SERVODRIVE_OVER_ETHERCAT)
  {
    return (series == DeviceSeries::INDRADRIVE || (series == DeviceSeries::CTRLXDRIVE && isNewVersion));
  }
  else if (profileType == AxisProfileType::CANOPEN_OVER_ETHERCAT)
  {
    return (series == DeviceSeries::INDRADRIVE) ? false : (series == DeviceSeries::CTRLXDRIVE && isNewVersion);
  }  
  return false;
}
