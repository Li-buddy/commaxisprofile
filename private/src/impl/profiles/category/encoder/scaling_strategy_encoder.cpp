/// @file 
///
/// @brief This source file is part of the axisprofile library and implements class ScalingStrategyEncoder
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "scaling_strategy_encoder.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "profiles/scaling/unit_defs.h"
#include "profiles/scaling/unit_helper.h"

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

ScalingStrategyEncoder::ScalingStrategyEncoder(ProfileBasicInfo& profileBasicInfo)
  :ScalingStrategy(profileBasicInfo)
{
  m_scalingStrategyCfg.m_scalingStrategy = AxisProfileScalingStrategy::DEVICE_IO;
}

void ScalingStrategyEncoder::loadBootingTasks()
{
  m_scalingLSBWeightsMethods.clear();
  m_scalingLSBWeightsMethods.reserve(1);
  m_scalingLSBWeightsMethods.emplace_back(&m_velScalingLSBWeight, &VELOCITY, [this]{return calcVelScalingLSBWeight();});
}

AxisProfileResult ScalingStrategyEncoder::calcScalingLSBWeights()
{
  return Scaling::executeCalcScalingLSBWeights();
}

AxisProfileResult ScalingStrategyEncoder::calcPosScalingLSBWeight()
{  
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyEncoder::calcVelScalingLSBWeight()
{
  const auto& mappitems = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (!mappitems.m_input.m_velocity)
  {
    m_velScalingLSBWeight = 1.0;
    m_velScalingLSBWeight_inv = 1.0;
  }
  else
  {
    //at data unit->10000*rpm
    //fieldbus should be deg/s
    // velocity in m/s or degree/s
    UnitHelper::convertToSI(UnitObjType::ROTATIONAL, UnitValueType::VELOCITY, (std::pow(10U, -4)), Unit::REVOLUTIONS_PER_MINUTE, m_velScalingLSBWeight);
    if (fabs(m_velScalingLSBWeight) < 1E-10)
    {
      APR_TRACE_ERROR("%s; LSB %s weight calculation has resulted in 0 (Set to 1 and continue)!", m_profileBasicInfo.m_profileTrace.createTraceMsg(), unitValueTypeToString(UnitValueType::VELOCITY));
      m_velScalingLSBWeight = 1.0;
      m_velScalingLSBWeight_inv = 1.0;
      return AxisProfileResult::ERR_MATH;
    }
    m_velScalingLSBWeight_inv = double(1.0) / m_velScalingLSBWeight;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyEncoder::calcAccScalingLSBWeight()
{ 
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyEncoder::calcTorqueScalingLSBWeight()
{
  return AxisProfileResult::STS_OK;
}
