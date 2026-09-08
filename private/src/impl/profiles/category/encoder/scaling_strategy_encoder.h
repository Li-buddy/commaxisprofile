#pragma once
/// @file
///
/// @brief This header file is part of the axisprofile library and define the class ScalingStrategyEncoder
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
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
class TestScalingStrategyEncoder;

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

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is specific class to implement encoder scaling
  class ScalingStrategyEncoder : public ScalingStrategy
  {
  public: // methods

    /// @brief constructor
    /// @param[in] profileBasicInfo     Information about the profile
    explicit ScalingStrategyEncoder(ProfileBasicInfo& profileBasicInfo);
    ScalingStrategyEncoder() = delete;
    /// @brief destructor
    ~ScalingStrategyEncoder() noexcept override = default;

    AxisProfileResult calcScalingLSBWeights() final;

  private: // method

    AxisProfileResult calcPosScalingLSBWeight() final;
    AxisProfileResult calcVelScalingLSBWeight() final;
    AxisProfileResult calcAccScalingLSBWeight() final;
    AxisProfileResult calcTorqueScalingLSBWeight() final;
    void loadBootingTasks() final;

  private: // data

    friend class  ::TestScalingStrategyEncoder;
  };
}
