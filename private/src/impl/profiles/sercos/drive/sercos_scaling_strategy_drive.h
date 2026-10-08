#pragma once
/// @file
///
/// @brief This header file is part of the axisprofile library and define the
/// sercos scaling class.
///
/// @note SERCOS only have drive scaling since it is highly restriction by the standard.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/scaling/scaling_strategy.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestScalingStrategySoEDrive;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
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
  /// @brief Struct that stores information for reading an S parameter
  struct ScalingData
  {
    std::uint16_t m_posScalingType{};            ///< S-0-0076 (Position data scaling type)
    std::uint16_t m_posScalingFactor{};          ///< S-0-0077 (Linear position data scaling factor)
    std::int16_t  m_posScalingExponent{};        ///< S-0-0078 (Linear position data scaling exponent)
    std::uint32_t m_posRotationalResolution{};   ///< S-0-0079 (Rotational position resolution)
    std::uint32_t m_posModuloValue{};            ///< S-0-0103 (Maximum travel range: Modulo format)

    std::uint16_t m_velScalingType{};            ///< S-0-0044 (Velocity data scaling type)
    std::uint16_t m_velScalingFactor{};          ///< S-0-0045 (Velocity data scaling factor)
    std::int16_t  m_velScalingExponent{};        ///< S-0-0046 (Velocity data scaling exponent)

    std::uint16_t m_torqueScalingType{};         ///< S-0-0086 (Torque data scaling type)
    std::uint16_t m_torqueScalingFactor{};       ///< S-0-0093 (Torque data scaling factor)
    std::int16_t  m_torqueScalingExponent{};     ///< S-0-0094 (Torque data scaling exponent)

    std::uint32_t m_inputRevolution {1};         ///< S-0-0121 (Input revolution of the gear)
    std::uint32_t m_outputRevolution {1};        ///< S-0-0122 (Output revolution of the gear)
    std::uint32_t m_feedConstant {1};            ///< S-0-0123 (Feed constant)
    std::uint32_t m_motorRatedTorqueRaw {1};     ///< S-0-0533 (Motor rate torque)

    std::uint16_t m_accScalingType{};            ///< S-0-0160 (Acceleration data scaling type)
    std::uint16_t m_accScalingFactor{};          ///< S-0-0161 (Acceleration data scaling factor)
    std::int16_t  m_accScalingExponent{};        ///< S-0-0162 (Acceleration data scaling exponent)        
  };

  /// @brief This is the sercos scaling class.
  class ScalingStrategySoEDrive : public ScalingStrategy
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Profile basic information
    explicit ScalingStrategySoEDrive(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    virtual ~ScalingStrategySoEDrive() noexcept override = default;

    AxisProfileResult calcScalingLSBWeights() final;
    AxisProfileResult checkAndSetModulo(double modValue, std::string& dynErrTxt) final;

  private: // methods

    /// @brief Read one SoE parameter from boot snapshot/cache or drive.
    /// @param[in] idn SoE IDN.
    /// @param[out] bufPtr Target buffer.
    /// @param[in] bufLen Target buffer length.
    /// @return Return value indicating success or error.
    AxisProfileResult getParameterValueFromSnapshot(std::uint16_t idn, std::uint8_t* bufPtr, std::size_t bufLen);

    AxisProfileResult calcPosScalingLSBWeight() final;
    AxisProfileResult calcVelScalingLSBWeight() final;
    AxisProfileResult calcTorqueScalingLSBWeight() final;        
    AxisProfileResult calcAccScalingLSBWeight() final;

    /// @brief This method reads all position data scaling parameters and stores the values in the class member variables.
    /// @return Return value indicating success or error.
    AxisProfileResult readPosScalingParamsSync();        

    /// @brief This method reads all position data scaling parameters and stores the values in the class member variables.
    /// @return Return value indicating success or error.
    AxisProfileResult readVelScalingParamsSync();        

    /// @brief This method reads all torque data scaling parameters and stores the values in the class member variables.
    /// @return Return value indicating success or error.
    AxisProfileResult readTorqueScalingParamsSync();        

    /// @brief This method reads all acceleration data scaling parameters and stores the values in the class member variables.
    /// @return Return value indicating success or error.
    AxisProfileResult readAccScalingParamsSync();      

  private: // data

    friend class ::TestScalingStrategySoEDrive;     // unittest of sercos scaling
    ScalingData m_scalingData{};                    ///< struct that stores all scaling related data
  };
}
