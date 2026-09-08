#pragma once
/// @file  scaling_strategy_factory.h
///
/// This header file is part of the axisprofile library and define the
/// scaling strategy factory. 
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_scaling.h"
#include "profiles/profile_trace.h"
#include "profiles/profile_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestScalingFactory;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------
    class IFieldbusAdapter;
    class AxisProfile;

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
    /// @brief This class is used to create different scaling strategy
    class ScalingStrategyFactory
    {
    public://method

      ScalingStrategyFactory() = delete;
      /// @brief Destructor
      virtual ~ScalingStrategyFactory() noexcept = default;
      ScalingStrategyFactory& operator=(ScalingStrategyFactory&) = delete;

      /// @brief This method returns the class name.
      /// @return   Return class name.
      static const char* getClassName(void) { return (CLASS_NAME(ScalingStrategyFactory)); }

    public://method

      /// @brief This method create the scaling strategy.
      /// @param[in]  profileBasicInfo  Pointer to the Profile Basic Information       
      /// @param[in]  scalingStrategy   Scaling Strategy.
      /// @return Return the IScaling pointer.
      static IScaling* createScalingStrategy(ProfileBasicInfo& profileBasicInfo, AxisProfileScalingStrategy scalingStrategy);

      /// @brief This method check if the scaling strategy meets the requirement.
      /// @param[in]  trace  scaling Strategy.
      /// @param[in]  strategy  scaling Strategy.
      /// @param[in]  profileType  axisprofile type.
      /// @return Return value indicating success or error.
      static AxisProfileResult checkCompatiblityForStrategy(ProfileTrace* const trace, const AxisProfileScalingStrategy& strategy, const AxisProfileType& profileType);

    private:

      friend class ::TestScalingFactory;
    };
  }
}
