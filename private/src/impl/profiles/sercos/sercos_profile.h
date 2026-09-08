#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements
/// main class for the SERCOS profile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile.h"

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------
class TestScalingFactory;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class SercosCommandDrive;
  class SercosStatusControlDrive;
  class SercosCyclicData;
  class SercosDeviceParamDrive;

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

  /// @brief This is the main class for the SERCOS profile.
  class SercosProfile : public AxisProfile
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]     profileName       Name of the profile
    /// @param[in]     technology        The network technology that is used for this profile
    explicit SercosProfile(const std::string& profileName, const NetworkTechnology technology);

    /// @brief Destructor
    virtual ~SercosProfile() noexcept = default;

    AxisProfileResult init() final;
    AxisProfileResult processDataAt(std::uint8_t* dataByteArray) final;
    AxisProfileResult processDataMdt(std::uint8_t* dataByteArray) final;
    AxisProfileResult getDeviceState() final;
    AxisProfileScalingStrategyCfg getProfileScalingStrategyCfg() const final;
    std::vector<std::unique_ptr<IJobObj>> onGetJobList() final;

  private: // data

    friend class ::TestScalingFactory; // used for unit test
  };
}
