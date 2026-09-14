/// @file
///
/// @brief This source file is part of the axisprofile library and implements the
/// scaling calculation of unit object.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_scaling_strategy_drive.h"
#include "common/debug_trace.h"
#include "diagnosis/dia.h"
#include "profiles/canopen/canopen_boot_param_snapshot.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/category/drive/axisprofile_device_param_drive.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "profiles/i_device_boot_param_snapshot.h"
#include "profiles/profile_trace.h"
#include "profiles/scaling/unit_defs.h"
#include "profiles/scaling/unit_helper.h"
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

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

ScalingStrategyCoEDrive::ScalingStrategyCoEDrive(ProfileBasicInfo& profileBasicInfo)
  : ScalingStrategy(profileBasicInfo)
{
  m_scalingStrategyCfg.m_scalingStrategy = AxisProfileScalingStrategy::COE_CALC_DRIVE;
  
  m_unitPosition.m_data = 0;
  m_unitVelocity.m_data = 0;
  m_unitAcceleration.m_data = 0;
}

AxisProfileResult ScalingStrategyCoEDrive::calcScalingLSBWeights()
{
  // Begin to calculate LSB values; parameter reads are done in calc* methods via snapshot helper paths.
  auto res = Scaling::executeCalcScalingLSBWeights();//template method related to calpos calvel calacc, and so on
  res |= checkScalingUniform();
  return res;
}

AxisProfileResult ScalingStrategyCoEDrive::readPosScalingParamsSync()
{
  const ObjectDirEntry objEntry = getObjDir(SCALING_SI_UNIT_POSITION, m_profileBasicInfo.m_driveNumber);
  auto result = getParameterValueFromSnapshot(objEntry, reinterpret_cast<std::uint8_t*>(&m_unitPosition.m_data), sizeof(m_unitPosition.m_data));
  if (aprStatusFailed(result))
  {
    return result;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEDrive::readVelScalingParamsSync()
{
  const ObjectDirEntry objEntry = getObjDir(SCALING_SI_UNIT_VELOCITY, m_profileBasicInfo.m_driveNumber);
  auto result = getParameterValueFromSnapshot(objEntry, reinterpret_cast<std::uint8_t*>(&m_unitVelocity.m_data), sizeof(m_unitVelocity.m_data));
  if (aprStatusFailed(result))
  {
    return result;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEDrive::readAccScalingParamsSync()
{
  const ObjectDirEntry objEntry = getObjDir(SCALING_SI_UNIT_ACCELERATION, m_profileBasicInfo.m_driveNumber);
  auto result = getParameterValueFromSnapshot(objEntry, reinterpret_cast<std::uint8_t*>(&m_unitAcceleration.m_data), sizeof(m_unitAcceleration.m_data));
  if (aprStatusFailed(result))
  {
    return result;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEDrive::readTorqueScalingParamsSync()
{
  std::vector<CanopenParamStruct> readStructSet;
  readStructSet.emplace_back(SCALING_TORQUE_SCALING_TYPE, &m_unitTorque.m_torqueScalingType,      sizeof(m_unitTorque.m_torqueScalingType));
  readStructSet.emplace_back(SCALING_TORQUE_FACTOR,       &m_unitTorque.m_torqueScalingFactor,    sizeof(m_unitTorque.m_torqueScalingFactor));
  readStructSet.emplace_back(SCALING_TORQUE_EXPONENT,     &m_unitTorque.m_torqueScalingExponent,  sizeof(m_unitTorque.m_torqueScalingExponent));
  readStructSet.emplace_back(SCALING_GEAR_REVOLUTION_IN,  &m_unitTorque.m_inputRevolution,        sizeof(m_unitTorque.m_inputRevolution));
  readStructSet.emplace_back(SCALING_GEAR_REVOLUTION_OUT, &m_unitTorque.m_outputRevolution,       sizeof(m_unitTorque.m_outputRevolution));
  readStructSet.emplace_back(SCALING_FEED_CONSTANT,       &m_unitTorque.m_feedConstant,           sizeof(m_unitTorque.m_feedConstant));
  readStructSet.emplace_back(SCALING_NOMINAL_TORQUE,      &m_unitTorque.m_motorRatedTorqueRaw,    sizeof(m_unitTorque.m_motorRatedTorqueRaw));

  for (const auto& item : readStructSet)
  {
    const ObjectDirEntry objEntry = getObjDir(item.m_objDirEntry, m_profileBasicInfo.m_driveNumber);
    AxisProfileResult result = getParameterValueFromSnapshot(objEntry, static_cast<std::uint8_t*>(item.m_ptrData), item.dataSize);
    if (aprStatusFailed(result))
    {
      APR_TRACE_MESSAGE("%s; read object %s failed at slaveId %d, check if the device supports it",
                        m_profileBasicInfo.m_profileTrace.createTraceMsg(result),
                        objDirToString(objEntry).c_str(), m_profileBasicInfo.m_deviceAddress);
      return result;
    }
  }
  if (m_unitTorque.m_outputRevolution == 0 || m_unitTorque.m_motorRatedTorqueRaw == 0 || m_unitTorque.m_feedConstant == 0)
  {
    Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_SCALING_VALUE_ERROR, __FILE__, __FUNCTION__, __LINE__,
                       "Must not 0, %s: %d, %s: %d, %s: %d, device %s, address %d",
                       objDirToString(SCALING_GEAR_REVOLUTION_OUT).c_str(), m_unitTorque.m_outputRevolution,
                       objDirToString(SCALING_FEED_CONSTANT).c_str(), m_unitTorque.m_feedConstant,
                       objDirToString(SCALING_NOMINAL_TORQUE).c_str(), m_unitTorque.m_motorRatedTorqueRaw,
                       m_profileBasicInfo.m_profileName.c_str(), m_profileBasicInfo.m_deviceAddress);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEDrive::getParameterValueFromSnapshot(const ObjectDirEntry& objEntry, std::uint8_t* bufPtr, std::size_t bufLen)
{
  return CanopenBootParamSnapshot::getParameterValueFromSnapshot(m_sharingHandler,
                                                                 m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                                                                 objEntry,
                                                                 bufPtr,
                                                                 bufLen);
}

AxisProfileResult ScalingStrategyCoEDrive::calcPosScalingLSBWeight()
{
  // check
  if (!m_sharingHandler||!m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  AxisProfileResult result = AxisProfileResult::STS_OK;
  const auto& mappitems = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (mappitems.m_input.m_position || mappitems.m_output.m_position)
  {
    result = readPosScalingParamsSync();
    if (aprStatusFailed(result))
    {
      m_posScalingLSBWeight = 1.0;
      m_posScalingLSBWeight_inv = 1.0 / m_posScalingLSBWeight;
      return result;
    }
  }
  else
  {
    // this case means, no position related PDO was configured
    // the position mode related function are limited
    // but ok to switch to running(considering open loop velocity mode)
    APR_TRACE_MESSAGE("%s; NOTE: Due to the position cyclic data not mapped in profile, position mode function are limited", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::STS_OK;
  }

  const std::int8_t exponent = static_cast<std::int8_t>(m_unitPosition.m_bytes.byte4);
  if (CANOPEN_POSITION_UNIT_LINEAR == m_unitPosition.m_bytes.byte3
    && exponent <= 0 && exponent >= -9)
  {
    Scaling::m_positionType = PosScalingType::LINEAR;
  }
  else if (CANOPEN_POSITION_UNIT_ROTARY == m_unitPosition.m_bytes.byte3
    && exponent <= 0 && exponent >= -6)
  {
    Scaling::m_positionType = PosScalingType::ROTARY;
  }
  else
  {
    m_positionType = PosScalingType::UNDEFINED;
    APR_TRACE_ERROR("%s, position data type is neither linear nor rotary!!!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_INIT_TELEGRAM_OFFSETS;    
  }
  m_posScalingLSBWeight = std::pow(10U, exponent);
  m_posScalingLSBWeight_inv = 1 / m_posScalingLSBWeight;
  m_posMetrication = true;

  /// set position data format as ignore
  m_posDataFormat = AxisProfilePosDataFormat::FORMAT_IGNORE;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEDrive::calcVelScalingLSBWeight()
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  // check
  AxisProfileResult result = readVelScalingParamsSync();
  if (aprStatusFailed(result))
  {
    m_velScalingLSBWeight = 1.0;
    m_velScalingLSBWeight_inv = 1.0 / m_velScalingLSBWeight;
    return result;
  }

  // only send function limitation note when not config velocity cyclic data
  const auto& mappitems = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (!mappitems.m_input.m_velocity || !mappitems.m_output.m_velocity)
  {
    // this case means, no velocity related PDO was configured
    // the velocity mode related function are limited
    // but ok to switch to running(considering open loop velocity mode)
    APR_TRACE_MESSAGE("%s; NOTE: Due to the velocity cyclic data not mapped in profile, velocity mode function are limited", m_profileBasicInfo.m_profileTrace.createTraceMsg());
  }

  const std::int8_t exponent = static_cast<std::int8_t>(m_unitVelocity.m_bytes.byte4);
  double unit{ 1.0 };
  if (CANOPEN_VELOCITY_UNIT_LINEAR == m_unitVelocity.m_bytes.byte3
    && exponent <= 0 && exponent >= -9)
  {
    Scaling::m_velocityType = VelScalingType::LINEAR;
  }
  else if (CANOPEN_VELOCITY_UNIT_ROTARY == m_unitVelocity.m_bytes.byte3
    && exponent <= 0 && exponent >= -6)
  {
    unit = 360.0;  //converse revolution to degree
    Scaling::m_velocityType = VelScalingType::ROTARY;
  }
  else
  {
    m_velocityType = VelScalingType::UNDEFINED;
    APR_TRACE_ERROR("%s, velocity data type is neither linear nor rotary!!!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_INIT_TELEGRAM_OFFSETS;
  }
  m_timeUnit = TimeUnit::SECOND;
  m_velScalingLSBWeight = std::pow(10U, exponent) * unit;
  if (CANOPEN_VELOCITY_TIME_UNIT_MINUTE == m_unitVelocity.m_bytes.byte2)
  {
    m_timeUnit = TimeUnit::MINUTE;
    m_velScalingLSBWeight = m_velScalingLSBWeight / 60;   //converse unit/min to unit/s
  }
  m_velScalingLSBWeight_inv = double(1.0) / m_velScalingLSBWeight;
  m_velMetrication = true;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEDrive::calcAccScalingLSBWeight()
{
  AxisProfileResult result = readAccScalingParamsSync();
  if (aprStatusFailed(result))
  {
    APR_TRACE_ERROR("%s; read canopen scaling parameters failed ", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return result;
  }

  const std::int8_t exponent = static_cast<std::int8_t>(m_unitAcceleration.m_bytes.byte4);
  double unit{ 1.0 };
  if (CANOPEN_ACCELERATION_UNIT_LINEAR == m_unitAcceleration.m_bytes.byte3
    && exponent <= 0 && exponent >= -9)
  {
    Scaling::m_accelerationType = AccScalingType::LINEAR;
  }
  else if (CANOPEN_ACCELERATION_UNIT_ROTARY == m_unitAcceleration.m_bytes.byte3
    && exponent <= 0 && exponent >= -6)
  {
    unit = 57.29577951471995;  //converse rad to degree
    Scaling::m_accelerationType = AccScalingType::ROTARY;
  }
  else
  {
    m_accelerationType = AccScalingType::UNDEFINED;
    APR_TRACE_ERROR("%s, acceleration data type is neither linear nor rotary!!!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_INIT_TELEGRAM_OFFSETS;
  }
  m_accScalingLSBWeight = std::pow(10U, exponent) * unit;
  m_accScalingLSBWeight_inv = double(1.0) / m_accScalingLSBWeight;
  m_accMetrication = true;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEDrive::calcTorqueScalingLSBWeight()
{
  if (!(m_sharingHandler && m_sharingHandler->getMappingHandlerIF()))
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  AxisProfileResult result;
  const auto& mappitems = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (mappitems.m_input.m_torqueForce || mappitems.m_output.m_torqueForce)
  {
    result = readTorqueScalingParamsSync();
    if (aprStatusFailed(result))
    {
      m_torqueScalingLSBWeight = 1.0;
      m_torqueScalingLSBWeight_inv = 1.0 / m_torqueScalingLSBWeight;
      return result;
    }
  }
  else
  {
    // this case means, no torque related PDO was configured
    // the torque mode related function are limited
    // but ok to switch to running(considering open loop velocity mode)
    APR_TRACE_MESSAGE("%s; NOTE: Due to the torque cyclic data not mapped in profile, torque mode function are limited", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::STS_OK;
  }
  bool torqueMetric = ((m_unitTorque.m_torqueScalingType >> CANOPEN_TORQUE_TYPE_UNIT_OFFSET) & 1U) == true ? false : true;

  switch (m_unitTorque.m_torqueScalingType & CANOPEN_TORQUE_TYPE_METHOD_MASK)
  {
    case CANOPEN_TORQUE_TYPE_METHOD_PCTG:
    {
      // percentage always has exponent = -1->according to drive engineering
      m_torqueScalingLSBWeight = 1.0 * std::pow(10U, -1);
      Scaling::m_torqueType = TorqueScalingType::PERCENTAGE;
      break;
    }
    case CANOPEN_TORQUE_TYPE_METHOD_LINEAR:
    {
      Unit unit = torqueMetric ? Unit::NEWTON : Unit::POUND_FORCE;
      UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::FORCE, (m_unitTorque.m_torqueScalingFactor * std::pow(10U, m_unitTorque.m_torqueScalingExponent)), unit, m_torqueScalingLSBWeight);
      Scaling::m_torqueType = TorqueScalingType::LINEAR;
      break;
    }
    case CANOPEN_TORQUE_TYPE_METHOD_ROT:
    {
      Unit unit = torqueMetric ? Unit::NEWTON_METERS : Unit::POUND_INCH;
      UnitHelper::convertToSI(UnitObjType::ROTATIONAL, UnitValueType::TORQUE, (m_unitTorque.m_torqueScalingFactor * std::pow(10U, m_unitTorque.m_torqueScalingExponent)), unit, m_torqueScalingLSBWeight);
      Scaling::m_torqueType = TorqueScalingType::ROTARY;
      break;
    }
    default:
    {
      APR_TRACE_ERROR("%s; Unknown torque scaling method detected in 0x2906:01 (Bits 2-0), neither linear nor rotary nor percentage!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
      m_torqueScalingLSBWeight = 1.0;
      m_torqueScalingLSBWeight_inv = 1.0;
      Scaling::m_torqueType = TorqueScalingType::UNDEFINED;
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
  }
  if (fabs(m_torqueScalingLSBWeight) < 1E-7)
  {
    APR_TRACE_ERROR("%s; LSB %s weight calculation has resulted in 0 (Set to 1 and continue)!", m_profileBasicInfo.m_profileTrace.createTraceMsg(), unitValueTypeToString(UnitValueType::TORQUE));
    m_torqueScalingLSBWeight = 1;
    m_torqueScalingLSBWeight_inv = 1;
    return AxisProfileResult::ERR_MATH;
  }
  m_torqueScalingLSBWeight_inv = double(1.0) / m_torqueScalingLSBWeight;

  AxisProfileDeviceScalingType torqueScalingType = getDeviceScalingType(axisprofile::AxisProfileDeviceValueType::TORQUE_FORCE);
  AxisProfileDeviceScalingType posScalingType = getDeviceScalingType(axisprofile::AxisProfileDeviceValueType::POSITION);

  // the transmitted value 0x6076:00(m_unitTorque.m_rateTorque) is with always 3 place right move in decimal, E.g. 900->0.9 in SI
  double motorRateTorque = m_unitTorque.m_motorRatedTorqueRaw * DECIM_PL3;
  // consider ratio
  double axisRateTorque = motorRateTorque * m_unitTorque.m_inputRevolution / m_unitTorque.m_outputRevolution;
  if (posScalingType == AxisProfileDeviceScalingType::LINEAR)
  {
    double pitch {1.0};
    const std::int8_t posExponent = static_cast<std::int8_t>(m_unitPosition.m_bytes.byte4);
    UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::POSITION, (std::pow(10U, posExponent)), Unit::METER, pitch);
    pitch *= m_unitTorque.m_feedConstant;

    // Linear axis has consider feedconstant: 2*pi*Torque = Force*Pitch --> Force = 2*pi*Torque/Pitch 
    m_rateTorqueForce = posScalingType == AxisProfileDeviceScalingType::LINEAR ? axisRateTorque * 2 * PI / pitch : axisRateTorque;
    APR_TRACE_MESSAGE("%s; RateForce %g", m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                      m_rateTorqueForce);
  }
  else
  {
    m_rateTorqueForce = axisRateTorque;
    APR_TRACE_MESSAGE("%s; RateTorque %g", m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                      m_rateTorqueForce);
  }

  // if position and torque scaling type not same, conversion is needed.
  if (torqueScalingType != posScalingType && torqueScalingType != AxisProfileDeviceScalingType::UNDEFINED)
  {
    result = convertTorqueToSI(posScalingType, torqueScalingType, m_rateTorqueForce);
  }
  return result;
}
