#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the helper class to determine device identification.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/profile_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr uint32_t REXROTH_VENDOR_ID = 0x00000024; /// Rexroth vendor id, reference:https://www.ethercat.org/en/vendor_id_list.html

  // product code, reference:https://inside-docupedia.bosch.com/confluence/spaces/EDCA/pages/5935191927/OPL_MEF_CORE+2+-+Product+Code+for+ctrlX+CORE+as+EtherCAT+Subdevice
  constexpr uint16_t INDRA_DRIVE = 0x0024;              ///< IndraDrive
  constexpr uint16_t CTRLX_IP20_MODULAR = 0x2000;       ///< ctrlX DRIVE IP20 modular
  constexpr uint16_t CTRLX_COMPACT = 0x2100;            ///< ctrlX DRIVE compact
  constexpr uint16_t CTRLX_XC20 = 0x2200;               ///< ctrlX DRIVE XC20
  constexpr uint16_t CTRLX_IP65_CABINETFREE = 0x3100;   ///< ctrlX DRIVE IP65 Cabinet-free


  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  /// @brief This is helper class for device identification.
  class DeviceIdHelper
  {
  public: // method

    /// @brief check whether the device support the checksum parameter
    /// @param[in]  majorV        device's major version
    /// @param[in]  minorV        device's minor version
    /// @param[in]  series        device's type
    /// @param[in]  profileType   device's profile type
    /// @return     returns       whether the device support the checksum parameter
    static bool deviceSupportCheckSum(uint8_t majorV, uint8_t minorV, DeviceSeries series, AxisProfileType profileType);

    /// @brief check whether the device is a rexroth drive
    /// @param[in]  devSeries      device's series
    /// @return     returns       whether the device is a rexroth drive
    static bool isRexrothDrive(DeviceSeries devSeries);

    /// @brief check whether the device is a rexroth ctrlX drive
    /// @param[in]  devSeries      device's series
    /// @return     returns       whether the device is a rexroth CtrlX drive
    static bool isRexrothCtrlXDrive(DeviceSeries devSeries);

    /// @brief calculate the device's series
    /// @param[in]  vendorId      device's vendor id
    /// @param[in]  productCode   device's product code
    /// @return     returns       device's series
    static DeviceSeries calculateDeviceSeries(std::uint32_t vendorId, std::uint32_t productCode);

    /// @brief extract software version from manufacturer version string
    /// @param[in]  devSeries      device series.
    /// @param[in]  manufacturerVersion      manufacturer version string, eg.
    ///             FWA-XD1-AXS-V-0504N-NN-00
    ///             FWA-INDRV*-MPB-20V28-D5-1-SNC-NN
    /// @return     returns       software version string
    static std::string extractFirmwareByStr(DeviceSeries devSeries, const std::string& manufacturerVersion);

    /// @brief This method convert the software version string to int
    /// @param[in]   versionStr    software version string, the format is "XXXX"
    /// @param[out]  majorVersion  software major version.
    /// @param[out]  minorVersion  software minor string.
    /// @return     Return value indicating success or error
    static bool convertSoftwareVersion(const std::string& versionStr, std::uint8_t& majorVersion, std::uint8_t& minorVersion);

  private: // methods

    /// @brief check whether the device is a rexroth device
    /// @param[in]  vendorId      device's vendor id
    /// @return     returns       whether the device is a rexroth device
    static bool isRexrothDevice(std::uint32_t vendorId);

    /// @brief check whether the device is in white list
    /// @param[in]  vendorId      device's vendor id
    /// @return     returns       whether the device is in white list
    //static bool isDeviceInWhiteList(std::uint32_t vendorId);
  };
}

