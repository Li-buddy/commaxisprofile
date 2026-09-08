#pragma once

/// @file
///
/// This header file is part of the axisprofile library and implements
/// main class for the CiA402 profile.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenProfile;
class TestScalingFactory;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

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

  /// @brief This is the main class for the CiA402 profile.
  class CanopenProfile : public AxisProfile
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileName       Name of the profile
    /// @param[in]   technology        The network technology that is used for this profile
    explicit CanopenProfile(const std::string& profileName, const comm::axisprofile::NetworkTechnology technology);

    /// @brief Destructor
    ~CanopenProfile() noexcept override = default;

    AxisProfileResult init() final;
    AxisProfileResult processDataAt(std::uint8_t* dataByteArray) final;
    AxisProfileResult processDataMdt(std::uint8_t* dataByteArray) final;
    AxisProfileResult getDeviceState() final;
    std::vector<std::unique_ptr<IJobObj>> onGetJobList() final;

    /// @brief This method set scaling strategy configuration for profile.
    /// @param[in]  strategyCfg  scaling strategy configuration.
    /// @return Return value indicating success or error.
    AxisProfileResult setProfileScalingStrategyCfg(const AxisProfileScalingStrategyCfg& strategyCfg) final;

    /// @brief This method get scaling strategy configuration for profile.
    /// @return Return value of AxisProfileScalingStrategyCfg.
    AxisProfileScalingStrategyCfg getProfileScalingStrategyCfg() const final;

  private: // methods

    /// @brief This method set scaling strategy for profile.
    /// @param[in]  strategy  scaling strategy.
    /// @return Return value indicating success or error.
    AxisProfileResult setProfileScalingStrategy(const AxisProfileScalingStrategy& strategy) ;

    /// @brief This method get scaling strategy for profile.
    /// @return Return AxisProfileScalingStrategy data.
    AxisProfileScalingStrategy getProfileScalingStrategy() const ;

    /// @brief This method get scaling numerator.
    /// @return Return numerator data.
    std::uint32_t getScalingNumerator() const ;

    /// @brief This method set scaling numerator.
    /// @param[in]  numerator  scaling numerator.
    /// @return Return value indicating success or error.
    AxisProfileResult setScalingNumerator(const std::uint32_t numerator) ;

    /// @brief This method get scaling numerator.
    /// @return Return denominator data.
    std::uint32_t getScalingDenominator() const ;

    /// @brief This method set scaling denominator.
    /// @param[in]  denominator  scaling denominator.
    /// @return Return value indicating success or error.
    AxisProfileResult setScalingDenominator(const std::uint32_t denominator) ;

    /// @brief This method get encoder resolution.
    /// @return Return resolution data.
    std::uint32_t getEncoderResolution()const ;

    /// @brief This method set encoder resolution.
    /// @param[in]  resolution  encoder resolution.
    /// @return Return value indicating success or error.
    AxisProfileResult setEncoderResolution(const std::uint32_t resolution) ;

    /// @brief This method set device scaling type for profile.
    /// @param[in]  deviceScalingType  device scaling type.
    /// @return Return value indicating success or error.
    AxisProfileResult setDeviceScalingType(const comm::axisprofile::AxisProfileDeviceScalingType& deviceScalingType) ;

    /// @brief This method set the velocity scaling factor configuration.
    /// @param[in] factorCfg scaling factor
    /// @return Return value indicating success or error.
    AxisProfileResult setVelocityFactorCfg(const ScalingFactor& factorCfg);

    /// @brief This method return the velocity scaling factor data;
    /// @return return velocity scaling factor.
    const ScalingFactor& getVelocityFactorCfg() const;
      
  private: // data

    friend class ::TestScalingFactory;
    friend class ::TestCanopenProfile;      // Unittest of the CanopenProfile
  };
}
