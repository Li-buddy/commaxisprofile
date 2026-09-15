#pragma once
/// @file
///
/// @brief This header file is part of the axisprofile library and implements the
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
#include "category/i_category_adapter.h"
#include "i_axisprofile_property_internal.h"
#include "job_mgr/job_mgr.h"
#include "profiles/data_sharing/i_sharing_handler.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class IAxisProfileSharingHandler;

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  struct SoftwareVersion
  {
    std::string   versionStr {};                             ///< software version string
    std::uint8_t  majorVersion {};                           ///< major version
    std::uint8_t  minorVersion {};                           ///< minor version
    bool          valid {false};                             ///< whether convert success
  };

  /// @brief This is the base class for property handling.
  class AxisProfileProperty : public IAxisProfilePropertyInternal
  {
  public: // methods
    /// @brief Constructor
    /// @param[in]   profileInfo     Reference to the Profile Basic Information
    explicit AxisProfileProperty(ProfileBasicInfo& profileInfo);

    /// @brief Destructor
    virtual ~AxisProfileProperty() noexcept override = default;

    const IAxisProfilePropertyC* getCInterface() const final;
    AxisProfileResult setDeviceAddress(std::uint32_t deviceAddress) final;
    std::uint32_t getDeviceAddress() const final;
    AxisProfileType getProfileType() const final;
    AxisProfileDeviceScalingType getDeviceScalingType(AxisProfileDeviceValueType valueType) const final;
    AxisProfileResult setDeviceName(const char* cDeviceName) final;
    const char* getDeviceName() const final;
    AxisProfileCategory getProfileCategory() final;
    AxisProfileResult getDeviceState() final;
    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    AxisProfileResult setDeviceIdentification(std::uint32_t venderId, std::uint32_t productCode) final;
    DeviceSeries getDeviceSeries() const final;
    AxisProfileResult setDeviceManufacturerVersion(const std::string& version) final;
    AxisProfileResult getDeviceSoftwareVersion(std::uint8_t& majorVersion, std::uint8_t& minorVersion) const final;
    const std::string& getDeviceSoftwareVersion() const final;
    void enableChecksum(bool enable) final;
    bool isChecksumEnabled() const final;

  protected: // data
    ProfileBasicInfo& m_profileBasicInfo;                           ///< Profile basic information
    std::string m_deviceName {};                                    ///< Name of the configured device
    IAxisProfileSharingHandler* m_sharingHandler {};                ///< Pointer to the data sharing handler
    DeviceSeries m_deviceSeries {DeviceSeries::NOTINDICATES};       ///< device series
    SoftwareVersion m_softwareVersion;                              ///< software version info

  private: // data
    IAxisProfilePropertyC m_propertyInterface {};                   ///< C-Interface for bundle interface
    bool m_enableChecksum {true};                                   ///< whether enable checksum funciton
  };
}
