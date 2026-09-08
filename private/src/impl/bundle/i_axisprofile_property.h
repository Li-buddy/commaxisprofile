#pragma once
/// @file
///
/// @brief This file is part of the AxisProfile library and implements the interface for all AxisProfile properties.
///        It is a subinterface of “IAxisprofile”.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "axisprofile_internal_defs.h"
// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
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
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for axisprofile property.
  class IAxisProfileProperty
  {
  public: // methods

    virtual ~IAxisProfileProperty() noexcept = default;

    /// @brief This method writes the given device address to the profile. Negative values and zero are not allowed!
    /// @param[in]  deviceAddress  Address of the device.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult setDeviceAddress(std::uint32_t deviceAddress) = 0;

    /// @brief This method returns the device address of the corresponding device.
    /// @return     Returns the device address.
    virtual std::uint32_t getDeviceAddress() const = 0;

    /// @brief This method returns the type of the profile.
    /// @return     Type of the profile.
    virtual AxisProfileType getProfileType() const = 0;

    /// @brief  This method gets the state of the device.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult getDeviceState() = 0;

    /// @brief This method gets the actual scaling type of the connected device for the given value type
    /// @param[in]  valueType     Defines the value type for the scaling type request.
    /// @return     Returns the scaling type of the device
    virtual AxisProfileDeviceScalingType getDeviceScalingType(AxisProfileDeviceValueType valueType) const = 0;

    /// @brief This method write the device name to the profile.
    /// @param[in]    deviceName  name of device
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult setDeviceName(const char* deviceName) = 0;

    /// @brief This method return the device name. 
    /// @return       Return device name.
    virtual const char* getDeviceName() const = 0;

    /// @brief This method return the profile category.
    /// @return       Return category of current profile. 
    virtual AxisProfileCategory getProfileCategory() = 0;

    /// @brief This method writes the given device vendor ID and product code to the property
    /// @param[in]  vendorId  device vendor ID.
    /// @param[in]  productCode  devcie product code.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult setDeviceIdentification(std::uint32_t venderId, std::uint32_t productCode) = 0;

    /// @brief This method returns the device series.
    /// @return     Returns the device series.
    virtual DeviceSeries getDeviceSeries() const = 0;

    /// @brief This method sets the software version and parses the string to version number, should be called in booting.
    /// @param[in]  version  software version string.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult setDeviceManufacturerVersion(const std::string& version) = 0;

    /// @brief This method get the software version
    /// @param[out]  majorVersion  software major version.
    /// @param[out]  minorVersion  software minor string.
    /// @return     Return value indicating success or error, error indicate the return value not be excepted.
    virtual AxisProfileResult getDeviceSoftwareVersion(std::uint8_t& majorVersion, std::uint8_t& minorVersion) const = 0;

    /// @brief This method get the software version string
    /// @return     Return software version string.
    virtual const std::string& getDeviceSoftwareVersion() const = 0;

    /// @brief This method return the EtherCAT instance name of current axisprofile.
    /// @return The EtherCAT instance name.
    virtual const char* getEtherCATName() const = 0;
  };
}
