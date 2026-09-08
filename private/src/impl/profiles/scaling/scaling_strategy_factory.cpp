/// @file  scaling_strategy_factory.cpp
///
/// This source file is part of the axisprofile library and implements the
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
#include "profiles/canopen/drive/canopen_scaling_strategy_control.h"
#include "profiles/canopen/drive/canopen_scaling_strategy_drive.h"
#include "profiles/sercos/drive/sercos_scaling_strategy_drive.h"
#include "scaling_strategy_factory.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

IScaling* ScalingStrategyFactory::createScalingStrategy(ProfileBasicInfo& profileBasicInfo, AxisProfileScalingStrategy scalingStrategy)
{
  IScaling* pScaling = nullptr;
  switch (scalingStrategy)
  {
  case AxisProfileScalingStrategy::SOE_STANDARD:
    pScaling = new sercos::ScalingStrategySoEDrive(profileBasicInfo);
    break;
  case AxisProfileScalingStrategy::COE_CALC_DRIVE:
    pScaling = new canopen::ScalingStrategyCoEDrive(profileBasicInfo);
    break;
  case AxisProfileScalingStrategy::COE_CALC_CONTROL:
    pScaling = new canopen::ScalingStrategyCoEControl(profileBasicInfo);
    break;
  default:
    break;
  }
  return pScaling;
}

AxisProfileResult ScalingStrategyFactory::checkCompatiblityForStrategy(ProfileTrace* const trace, const AxisProfileScalingStrategy& strategy, const AxisProfileType& profileType)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  //if the strategy obey the profile type, then the setting will not occur
  switch (profileType)
  {
  case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
    if (strategy != AxisProfileScalingStrategy::SOE_STANDARD)
    {
      APR_TRACE_ERROR("%s; current strategy mode: %s, which is not compatible to current profile type", trace->createTraceMsg(), axisProfileScalingStrategyToString(strategy));
      result = AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    break;
  case AxisProfileType::CANOPEN_OVER_ETHERCAT:
    if (strategy < AxisProfileScalingStrategy::COE_CALC_DRIVE)
    {
      APR_TRACE_ERROR("%s; current strategy mode: %s, which is not compatible to current profile type", trace->createTraceMsg(), axisProfileScalingStrategyToString(strategy));
      result = AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    break;
  default:
    APR_TRACE_ERROR("%s; current profile type is %s, it's invalid type", trace->createTraceMsg(), axisProfileTypeToString(profileType));
    result = AxisProfileResult::ERR_INVALID_ARGUMENT;
    break;
  }
  return result;
}
