#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define scaling calculation of unit object.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/scaling/scaling_strategy.h"
#include "profiles/profile_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenScalingStrategyDrive;

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
    /// @brief Struct that stores information for reading an CoE parameter
  struct TorqueScalingData
  {
    std::uint16_t m_torqueScalingType {};              ///< 0x2906:01 (Torque data scaling type)
    std::uint16_t m_torqueScalingFactor {1};           ///< 0x2906:02 (Torque data scaling factor)
    std::int16_t m_torqueScalingExponent {};           ///< 0x2906:03 (Torque data scaling exponent)

    std::uint32_t m_inputRevolution {1};               ///< 0x6091:01 (input revolution of the gear)
    std::uint32_t m_outputRevolution {1};              ///< 0x6091:02 (output revolution of the gear)
    std::uint32_t m_feedConstant {1};                  ///< 0x6092:01 (feed constant)
    std::uint32_t m_motorRatedTorqueRaw {1};           ///< 0x6076:00 (rate torque)
  };

  /// @brief This union defines CanScalingData
  typedef union CanScalingData
  {
    std::uint32_t m_data {};  ///< data per SE
    DataBits m_bytes;         ///< bytes information
  }CanScalingData;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
            
  /// @brief This is specific class to implement rexroth CoE scaling method
  class ScalingStrategyCoEDrive : public ScalingStrategy
  {
  public: // methods

    /// @brief constructor
    /// @param[in] profileBasicInfo     Information about the profile
    explicit ScalingStrategyCoEDrive(comm::axisprofile::ProfileBasicInfo& profileBasicInfo);
    ScalingStrategyCoEDrive() = delete;
    /// @brief destructor
    ~ScalingStrategyCoEDrive() noexcept override = default;

    comm::axisprofile::AxisProfileResult calcScalingLSBWeights() final;

  private: // method

    comm::axisprofile::AxisProfileResult calcPosScalingLSBWeight() final;
    comm::axisprofile::AxisProfileResult calcVelScalingLSBWeight() final;
    comm::axisprofile::AxisProfileResult calcAccScalingLSBWeight() final;
    comm::axisprofile::AxisProfileResult calcTorqueScalingLSBWeight() final;

    /// @brief This method reads all position data scaling parameters and stores the values in the class member variables.
    /// @return Return value indicating success or error.
    AxisProfileResult readPosScalingParamsSync();

    /// @brief This method reads all position data scaling parameters and stores the values in the class member variables.
    /// @return Return value indicating success or error.
    AxisProfileResult readVelScalingParamsSync();

    /// @brief This method reads all acceleration data scaling parameters and stores the values in the class member variables.
    /// @return Return value indicating success or error.
    AxisProfileResult readAccScalingParamsSync();

    /// @brief This method reads all torque data scaling parameters and stores the values in the class member variables.
    /// @return Return value indicating success or error.
    AxisProfileResult readTorqueScalingParamsSync();

  private: // data

    friend class  ::TestCanopenScalingStrategyDrive;
    CanScalingData m_unitPosition{};        ///< unit position
    CanScalingData m_unitVelocity{};        ///< unit velocity
    CanScalingData m_unitAcceleration{};    ///< unit Acceleration
    TorqueScalingData m_unitTorque{};       ///< unit torque
  };
}
