/// @file
///
/// @brief This source file is part of the axisprofile library and implement the
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
#include "diagnosis/dia.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "profiles/scaling/unit_defs.h"
#include "scaling_strategy.h"
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
ScalingStrategy::ScalingStrategy(ProfileBasicInfo& profileBasicInfo)
  : Scaling(profileBasicInfo)
{
  m_scalingStrategyCfg.m_scalingStrategy = AxisProfileScalingStrategy::UNKNOWN_STRATEGY;
  m_scalingStrategyCfg.m_deviceScalingType = AxisProfileDeviceScalingType::UNDEFINED;
  m_scalingStrategyCfg.m_numerator = 1;
  m_scalingStrategyCfg.m_denominator = 1;
  m_scalingStrategyCfg.m_resolution = 1;
}

AxisProfileResult ScalingStrategy::setScalingNumerator(const std::uint32_t /*scalingNumerator*/)
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategy::setScalingDenominator(const std::uint32_t /*scalingDenominator*/)
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategy::setEncoderResolution(const std::uint32_t /*resolution*/)
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategy::setDeviceScalingType(const AxisProfileDeviceScalingType /*deviceScalingType*/)
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategy::setVelocityFactorCfg(const ScalingFactor& /*strategyCfg*/)
{
  return AxisProfileResult::STS_OK;
}

AxisProfileScalingStrategy ScalingStrategy::getScalingStrategy()const
{
  return m_scalingStrategyCfg.m_scalingStrategy;
}

std::uint32_t ScalingStrategy::getScalingNumerator()const
{
  return m_scalingStrategyCfg.m_numerator;
}

std::uint32_t ScalingStrategy::getScalingDenominator()const
{
  return m_scalingStrategyCfg.m_denominator;
}

std::uint32_t ScalingStrategy::getEncoderResolution()const
{
  return m_scalingStrategyCfg.m_resolution;
}

const ScalingFactor& ScalingStrategy::getVelocityFactorCfg() const
{
  return m_scalingStrategyCfg.m_velFactor;
}

AxisProfileResult ScalingStrategy::convertTorqueToSI(AxisProfileDeviceScalingType posScalingType,
                                                     AxisProfileDeviceScalingType torqueScalingType,
                                                     double rateTorqueForce)
{
  switch (posScalingType)
  {
    case AxisProfileDeviceScalingType::LINEAR:
    case AxisProfileDeviceScalingType::ROTATIONAL:
    {
      if (AxisProfileDeviceScalingType::PERCENTAGE == torqueScalingType)
      {
        // percentage scaling factor
        Scaling::m_torqueScalingLSBWeight = Scaling::m_torqueScalingLSBWeight * rateTorqueForce * PERCENT;

        // percentage scaling is set in the drive 
        // but valueSI is in linear scaling i.e.N
        Scaling::m_torqueScalingLSBWeight_inv = Scaling::m_torqueScalingLSBWeight_inv / rateTorqueForce / PERCENT;
      }
      else
      {
        Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_TORQUE_FORCE_SCALING_TYPE,
                           m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                           "torque scaling type: %s conflict with position scaling type: %s",
                           axisProfileDeviceScalingTypeToString(torqueScalingType), axisProfileDeviceScalingTypeToString(posScalingType));
        return AxisProfileResult::ERR_INVALID_ARGUMENT;
      }
      break;
    }
    default:
    {
      APR_TRACE_ERROR("%s; torque scaling type: %s can't be converted to match with position scaling type: %s",
                      m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_INVALID_ARGUMENT),
                      axisProfileDeviceScalingTypeToString(torqueScalingType), axisProfileDeviceScalingTypeToString(posScalingType));
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
  }
  APR_TRACE_MESSAGE("%s; torque scaling type %s, LSB %g", m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                    axisProfileDeviceScalingTypeToString(torqueScalingType), m_torqueScalingLSBWeight);
  return AxisProfileResult::STS_OK;
}

void ScalingStrategy::loadBootingTasks()
{
  m_scalingLSBWeightsMethods.clear();
  m_scalingLSBWeightsMethods.reserve(4);
  m_scalingLSBWeightsMethods.emplace_back(&m_posScalingLSBWeight,    &POSITION,     [this]{return calcPosScalingLSBWeight();});
  m_scalingLSBWeightsMethods.emplace_back(&m_velScalingLSBWeight,    &VELOCITY,     [this]{return calcVelScalingLSBWeight();});
  m_scalingLSBWeightsMethods.emplace_back(&m_torqueScalingLSBWeight, &TORQUE,       [this]{return calcTorqueScalingLSBWeight();});
  m_scalingLSBWeightsMethods.emplace_back(&m_accScalingLSBWeight,    &ACCELERATION, [this]{return calcAccScalingLSBWeight();});
}
