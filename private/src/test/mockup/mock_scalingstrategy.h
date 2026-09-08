#pragma once
/// @file  mock_scalingstrategy.h
///
/// This header file is part of the axisprofile library and define the
/// mock implement of scaling strategy. 
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/scaling/scaling_strategy.h"
#include "profiles/profile_trace.h"
#include "fieldbuses/i_fieldbus_adapter.h"
#include "profiles/axisprofile.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "profiles/profile_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  class MockScalingStrategy :public comm::axisprofile::ScalingStrategy
  {
  public:
    MockScalingStrategy(comm::axisprofile::ProfileBasicInfo& profileBasicInfo)
      :ScalingStrategy(profileBasicInfo)
    {
      m_scalingStrategyCfg.m_scalingStrategy = comm::axisprofile::AxisProfileScalingStrategy::COE_CALC_DRIVE;
      m_scalingStrategyCfg.m_deviceScalingType = comm::axisprofile::AxisProfileDeviceScalingType::LINEAR;
      m_scalingStrategyCfg.m_numerator = 8388608;
      m_scalingStrategyCfg.m_denominator = 100000;
      m_scalingStrategyCfg.m_resolution = 8388608;
    }
    ~MockScalingStrategy() noexcept override = default;

    comm::axisprofile::AxisProfileResult calcPosScalingLSBWeight() final 
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult calcVelScalingLSBWeight() final
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult calcAccScalingLSBWeight() final
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult calcTorqueScalingLSBWeight() final
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult calcScalingLSBWeights()
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }
  };
}


