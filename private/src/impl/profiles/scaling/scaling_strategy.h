#pragma once
/// @file
///
/// @brief This header file is part of the axisprofile library and define the
/// scaling calculation strategy. 
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "scaling.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
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

  /// @brief This union defines DataBits
  typedef struct DataBits
  {
    std::uint8_t byte1 : 8;        ///< least significant byte
    std::uint8_t byte2 : 8;        ///< second least significant byte
    std::uint8_t byte3 : 8;        ///< second most significant byte
    std::uint8_t byte4 : 8;        ///< most significant byte
  }DataBits;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This is the basic class of different scaling strategy
  class ScalingStrategy : public Scaling
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileBasicInfo    Basic information of the profile
    explicit ScalingStrategy(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    virtual ~ScalingStrategy() noexcept override = default;

    /// @brief This method set the scaling numerator.
    /// @param[in]  scalingNumerator  scaling numerator value.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult setScalingNumerator(const std::uint32_t scalingNumerator);

    /// @brief This method set the scaling denominator.
    /// @param[in]  scalingDenominator  scaling denominator value.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult setScalingDenominator(const std::uint32_t scalingDenominator);

    /// @brief This method set the scaling resolution.
    /// @param[in]  resolution  scaling resolution value.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult setEncoderResolution(const std::uint32_t resolution);

    /// @brief This method set the device scaling type.
    /// @param[in]  deviceScalingType  device scaling type.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult setDeviceScalingType(const AxisProfileDeviceScalingType deviceScalingType);

    /// @brief This method set the velocity scaling factor configuration.
    /// @param[in] factorCfg scaling factor
    /// @return Return value indicating success or error.
    virtual AxisProfileResult setVelocityFactorCfg(const ScalingFactor& factorCfg);

    /// @brief This method return the velocity scaling factor data.
    /// @return return velocity scaling factor.
    virtual const ScalingFactor& getVelocityFactorCfg() const;

    /// @brief This method return the scaling strategy.
    /// @return Return scaling strategy.
    AxisProfileScalingStrategy getScalingStrategy()const;

    /// @brief This method return the scaling numerator.
    /// @return Return scaling numerator.
    std::uint32_t getScalingNumerator() const;

    /// @brief This method return the scaling denominator.
    /// @return Return scaling denominator.
    std::uint32_t getScalingDenominator() const;

    /// @brief This method return the scaling resolution.
    /// @return Return scaling resolution.
    std::uint32_t getEncoderResolution() const;

    void loadBootingTasks() override;

  private: // methods

    /// @brief This method calculate the position scaling
    /// @return Return value indicating success or error.
    virtual AxisProfileResult calcPosScalingLSBWeight() = 0;

    /// @brief This method calculate the velocity scaling parameters.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult calcVelScalingLSBWeight() = 0;

    /// @brief This method calculate the acceleration scaling parameters.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult calcAccScalingLSBWeight() = 0;

    /// @brief This method calculate the torque scaling parameters.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult calcTorqueScalingLSBWeight() = 0;      

  protected: // methods

    /// @brief This method convert torque value between SI and scaling value, convert the inputs from N*M/N/percentage value to SI value.
    /// @param[in]  posScalingType      position scaling type
    /// @param[in]  torqueScalingType   torque scaling type
    /// @param[in]  rateTorqueForce     rate torque/force
    /// @return     return the conversion success or error
    AxisProfileResult convertTorqueToSI(AxisProfileDeviceScalingType posScalingType,
                                        AxisProfileDeviceScalingType torqueScalingType,
                                        double rateTorqueForce);

  protected: // data

    AxisProfileScalingStrategyCfg m_scalingStrategyCfg{};  ///< struct for scaling strategy configuration data
  };
}
