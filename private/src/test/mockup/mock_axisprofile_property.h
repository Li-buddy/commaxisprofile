#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements a mockup for
/// the axis profile property interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------

#include "axisprofile_internal_defs.h"
#include "profiles/i_axisprofile_property_internal.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

namespace test
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  struct SoftwareVersion
  {
    std::string   m_versionStr {};                             ///< software version string
    std::uint8_t  m_majorVersion {};                           ///< major version
    std::uint8_t  m_minorVersion {};                           ///< minor version
    bool          m_convertSuccee {false};                     ///< whether convert success
  };

  /// @brief This class implements a mockup for the axis profile property interface.
  class MockAxisProfileProperty : public comm::axisprofile::IAxisProfilePropertyInternal
  {
  public:
    /// @brief Constructor
    MockAxisProfileProperty() = default;
    /// @brief Destructor
    virtual ~MockAxisProfileProperty() noexcept override = default;
    const IAxisProfilePropertyC* getCInterface() const override { return nullptr; }

    comm::axisprofile::AxisProfileResult setDeviceAddress(std::uint32_t deviceAddress) override { m_deviceAddress = deviceAddress; return m_returnValue; }
    std::uint32_t getDeviceAddress() const override { return m_deviceAddress; }
    comm::axisprofile::AxisProfileType getProfileType() const { return m_profileType; }
    comm::axisprofile::AxisProfileResult setProfileType(comm::axisprofile::AxisProfileType profileType) {
      m_profileType = profileType;
      return AxisProfileResult::STS_OK;
    }
    comm::axisprofile::AxisProfileResult getDeviceState() override { return m_returnValue; }
    comm::axisprofile::AxisProfileDeviceScalingType getDeviceScalingType(AxisProfileDeviceValueType /*valueType*/) const override { return comm::axisprofile::AxisProfileDeviceScalingType::UNDEFINED; }
    comm::axisprofile::AxisProfileResult setDeviceName(const char* cDeviceName) override { m_deviceName = cDeviceName; return comm::axisprofile::AxisProfileResult::STS_OK; }
    const char* getDeviceName() const override { return m_deviceName.c_str(); }
    comm::axisprofile::AxisProfileCategory getProfileCategory() override { return m_profileCategory; }
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/)override {};

    AxisProfileResult setDeviceIdentification(std::uint32_t /*venderId*/, std::uint32_t/* productCode*/) override { return m_returnValue; };
    void setDeviceSeries(DeviceSeries series) { m_deviceSeries = series;}
    DeviceSeries getDeviceSeries() const override { return m_deviceSeries;}
    void setSoftwareVersion(std::uint8_t majorVersion, std::uint8_t minorVersion)
    {
      m_softwareVersion.m_majorVersion = majorVersion;
      m_softwareVersion.m_minorVersion = minorVersion;
    }
    AxisProfileResult setDeviceManufacturerVersion(const std::string& /*fwVersion*/) override { return m_returnValue; }
    AxisProfileResult getDeviceSoftwareVersion(std::uint8_t& majorVersion, std::uint8_t& minorVersion) const override
    {
      majorVersion = m_softwareVersion.m_majorVersion;
      minorVersion = m_softwareVersion.m_minorVersion;
      return m_returnValue;
    }
    const std::string& getDeviceSoftwareVersion() const override { return m_softwareVersion.m_versionStr; }
    void enableChecksum(bool enable) override { m_enableChecksum = enable; };
    bool isChecksumEnabled() const override { return m_enableChecksum; };
    const char* getEtherCATName() const override {return m_ethercatName.c_str(); }
    AxisProfileResult setEtherCATName(const char* name) override { m_ethercatName = name; return comm::axisprofile::AxisProfileResult::STS_OK;  }

  protected: // data
    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   ///< Result of member functions
    comm::axisprofile::AxisProfileType m_profileType;                                                    ///< Type of the profile
    comm::axisprofile::AxisProfileCategory m_profileCategory{ comm::axisprofile::AxisProfileCategory::CATEGORY_DRIVE };
    std::uint32_t m_deviceAddress{};                                                                     ///< Address of the connected device
    std::string m_deviceName;                                                                            ///< Name of the connected device

    DeviceSeries m_deviceSeries {DeviceSeries::NOTINDICATES};                                            ///< device series
    SoftwareVersion m_softwareVersion;                                                                   ///< software version info
    bool m_enableChecksum {false};                                                                       ///whether enable checksum funciton
    std::string m_ethercatName;                                                                          ///< name of the configured EtherCAT master
  };
}
