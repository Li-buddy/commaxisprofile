/// @file
///
/// @brief This source file is part of the axisprofile library and implement the
/// sercos scaling class. 
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "diagnosis/dia.h"
#include "profiles/category/drive/axisprofile_device_param_drive.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "profiles/scaling/unit_helper.h"
#include "profiles/sercos/sercos_boot_param_snapshot.h"
#include "profiles/sercos/sercos_defs.h"
#include "sercos_scaling_strategy_drive.h"
#include <cmath>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

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

ScalingStrategySoEDrive::ScalingStrategySoEDrive(ProfileBasicInfo& profileBasicInfo)
  :ScalingStrategy(profileBasicInfo)
{
  m_scalingStrategyCfg.m_scalingStrategy = AxisProfileScalingStrategy::SOE_STANDARD;
}

AxisProfileResult ScalingStrategySoEDrive::calcScalingLSBWeights()
{
  auto res = Scaling::executeCalcScalingLSBWeights();
  res |= checkScalingUniform();
  return res;
}

AxisProfileResult ScalingStrategySoEDrive::getParameterValueFromSnapshot(std::uint16_t idn, std::uint8_t* bufPtr, std::size_t bufLen)
{
  return SercosBootParamSnapshot::getParameterValueFromSnapshot(m_sharingHandler, m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                                                                idn, bufPtr, bufLen);
}

///////////////////////////////////////////////////////////////////////////////////
// ****************************** PRIVATE METHODS ****************************** //

AxisProfileResult ScalingStrategySoEDrive::readPosScalingParamsSync()
{
  std::vector<SercosParamStruct> readStructSet;
  readStructSet.reserve(5);
  readStructSet.emplace_back(S_0_0076_SCALING_POS_TYPE,       &m_scalingData.m_posScalingType,          sizeof(m_scalingData.m_posScalingType));  // S-0-0076 (Position data scaling type)
  readStructSet.emplace_back(S_0_0077_SCALING_POS_FACTOR,     &m_scalingData.m_posScalingFactor,        sizeof(m_scalingData.m_posScalingFactor)); // S-0-0077 (Linear position data scaling factor)
  readStructSet.emplace_back(S_0_0078_SCALING_POS_EXPONENT,   &m_scalingData.m_posScalingExponent,      sizeof(m_scalingData.m_posScalingExponent)); // S-0-0078 (Linear position data scaling exponent)
  readStructSet.emplace_back(S_0_0079_SCALING_POS_ROT_RES,    &m_scalingData.m_posRotationalResolution, sizeof(m_scalingData.m_posRotationalResolution)); // S-0-0079 (Rotational position resolution)
  readStructSet.emplace_back(S_0_0103_SCALING_POS_MOD_VALUE,  &m_scalingData.m_posModuloValue,          sizeof(m_scalingData.m_posModuloValue)); // S-0-0103 (Maximum travel range: Modulo format)

  AxisProfileResult result{};
  for (const auto& SercosParamStruct : readStructSet)
  {
    result = getParameterValueFromSnapshot(SercosParamStruct.m_requestIDN,  static_cast<std::uint8_t*>(SercosParamStruct.m_ptrData), SercosParamStruct.dataSize);
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_READ_PARAM_FAILED,
                         m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "Position scaling, SoE IDN: %s, error: 0x%04X", idnToString(SercosParamStruct.m_requestIDN).c_str(), result);
      return result;
    }
  }
  return result;
}

AxisProfileResult ScalingStrategySoEDrive::calcPosScalingLSBWeight()
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // In some open loop case, not all position scaling parameter exist
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
    // but it is okay to switch to running(considering open loop velocity mode)
    APR_TRACE_MESSAGE("%s; NOTE: Due to the position cyclic data not mapped in profile, position mode function are limited", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::STS_OK;
  }

  // Note:
  // If we lose accuracy due to floating point calculation it may be helpful to use the reciprocal value for the LSB weight calculation.
  // We already did this in G1. For further information ask RAK or PEG.
  switch (m_scalingData.m_posScalingType & SERCOS_SCA_POS_TYPE_METHOD_MASK)
  {
    case SERCOS_SCA_POS_TYPE_METHOD_NONE:
    {
      Scaling::m_positionType = PosScalingType::UNDEFINED;
      // No Scaling method set (none). Set LSB weight to 1.
      m_posScalingLSBWeight = 1.0;
      result = AxisProfileResult::ERR_INVALID_ARGUMENT;
      break;
    }
    case SERCOS_SCA_POS_TYPE_METHOD_LINEAR:
    {
      // Linear Scaling
      Scaling::m_positionType = PosScalingType::LINEAR;
      if (false == ((m_scalingData.m_posScalingType >> SERCOS_SCA_POS_TYPE_UNIT_OFFSET) & 1U))
      {
        // Unit for linear Scaling (Bit 4) = Metres
        // The drive is working with meters but showing millimeters as unit ( exponent of -7 leads to 4 places after "." e.g. 0.0001)
        m_posMetrication = true;
        UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::POSITION, (m_scalingData.m_posScalingFactor * std::pow(10U, m_scalingData.m_posScalingExponent)), Unit::METER, m_posScalingLSBWeight);
      }
      else
      {
        // Unit for linear Scaling (Bit 4) = Inch
        UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::POSITION, (m_scalingData.m_posScalingFactor * std::pow(10U, m_scalingData.m_posScalingExponent)), Unit::INCH, m_posScalingLSBWeight);
        m_posMetrication = false;
      }
      break;
    }
    case SERCOS_SCA_POS_TYPE_METHOD_ROT:
    {
      // Rotational scaling
      Scaling::m_positionType = PosScalingType::ROTARY;
      // LSBweight = 1 revolution / RotationalPositionResolution
      m_posScalingLSBWeight = (m_scalingData.m_posRotationalResolution != 0.0) ? (360.0 / m_scalingData.m_posRotationalResolution) : 0.0;
      break;
    }    
    default:
    {
      APR_TRACE_ERROR("%s; Unknown scaling method detected in S-0-0076 (Bits 2-0)!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
      Scaling::m_positionType = PosScalingType::UNDEFINED;
      m_posScalingLSBWeight = 1.0;
      result = AxisProfileResult::ERR_INVALID_ARGUMENT;
      break;
    }
  }

  // update processing format 
  if (m_scalingData.m_posScalingType & SERCOS_SCA_POS_TYPE_DATA_FORMAT)
  {
    m_posDataFormat = AxisProfilePosDataFormat::FORMAT_MODULO;
  }
  else
  {
    m_posDataFormat = AxisProfilePosDataFormat::FORMAT_ABSOLUTE;
  }

  if (fabs(m_posScalingLSBWeight) < SCALING_EPS)
  {
    APR_TRACE_ERROR("%s; LSB weight calculation has resulted in 0 (Set to 1 and continue)!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    m_posScalingLSBWeight = 1.0;
    m_posScalingLSBWeight_inv = 1.0;
    return AxisProfileResult::ERR_MATH;
  }
  m_posScalingLSBWeight_inv = double(1.0) / m_posScalingLSBWeight;
  m_moduloValue = m_posScalingLSBWeight * static_cast<double>(m_scalingData.m_posModuloValue);

  return result;
}

AxisProfileResult ScalingStrategySoEDrive::readVelScalingParamsSync()
{
  std::vector<SercosParamStruct> readStructSet;
  readStructSet.reserve(3);
  readStructSet.emplace_back(S_0_0044_SCALING_VEL_TYPE, &m_scalingData.m_velScalingType, sizeof(m_scalingData.m_velScalingType)); // S-0-0044 (Velocity data scaling type)
  readStructSet.emplace_back(S_0_0045_SCALING_VEL_FACTOR, &m_scalingData.m_velScalingFactor, sizeof(m_scalingData.m_velScalingFactor)); // S-0-0045 (Linear Velocity data scaling factor)
  readStructSet.emplace_back(S_0_0046_SCALING_VEL_EXPONENT, &m_scalingData.m_velScalingExponent, sizeof(m_scalingData.m_velScalingExponent)); // S-0-0046 (Linear Velocity data scaling exponent)

  AxisProfileResult result{};
  for (const auto& SercosParamStruct : readStructSet)
  {
    result = getParameterValueFromSnapshot(SercosParamStruct.m_requestIDN, static_cast<std::uint8_t*>(SercosParamStruct.m_ptrData), SercosParamStruct.dataSize);
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_READ_PARAM_FAILED,
                         m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "Velocity scaling, SoE IDN: %s, error: 0x%04X", idnToString(SercosParamStruct.m_requestIDN).c_str(), result);
      return result;
    }
  }
  return result;
}

AxisProfileResult ScalingStrategySoEDrive::calcVelScalingLSBWeight()
{
  if (!m_sharingHandler||!m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // always read velocity parameters
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
    // but is okay to switch to running(considering open loop velocity mode)    
    APR_TRACE_MESSAGE("%s; NOTE: Due to the velocity cyclic data not mapped in profile, velocity mode function are limited", m_profileBasicInfo.m_profileTrace.createTraceMsg());
  }    

  if (true == ((m_scalingData.m_velScalingType >> SERCOS_SCA_VEL_TYPE_TIME_UNIT_OFFSET) & 1U))
  {
    m_timeUnit = TimeUnit::SECOND;
  }
  else
  {
    m_timeUnit = TimeUnit::MINUTE;
  }
  UnitObjType unitObjType{ UnitObjType::ANY };
  Unit unit{};
  switch (m_scalingData.m_velScalingType & SERCOS_SCA_VEL_TYPE_METHOD_MASK)
  {
    case SERCOS_SCA_VEL_TYPE_METHOD_NONE:
    {
      // No Scaling method set (none). Set LSB weight to 1.
      Scaling::m_velocityType = VelScalingType::UNDEFINED;
      m_velScalingLSBWeight = 1.0;
      break;
    }
    case SERCOS_SCA_VEL_TYPE_METHOD_LINEAR:
    {
      // Linear Scaling
      Scaling::m_velocityType = VelScalingType::LINEAR;
      unitObjType = UnitObjType::LINEAR;
      if (false == ((m_scalingData.m_velScalingType >> SERCOS_SCA_VEL_TYPE_UNIT_OFFSET) & 1U))
      {
        if (TimeUnit::SECOND == m_timeUnit)
        {
          unit = Unit::METER_PER_SECOND;
        }
        else
        {
          unit = Unit::METER_PER_MINUTE;
        }
        m_velMetrication = true;
      }
      else
      {
        if (TimeUnit::SECOND == m_timeUnit)
        {
          unit = Unit::INCH_PER_SECOND;
        }
        else
        {
          unit = Unit::INCH_PER_MINUTE;
        }
        m_velMetrication = false;
      }
      break;
    }
    case SERCOS_SCA_VEL_TYPE_METHOD_ROT:
    {
      Scaling::m_velocityType = VelScalingType::ROTARY;
      unitObjType = UnitObjType::ROTATIONAL;
      // Rotational scaling with different unit
      // due to drive firmware extend the bit 4 of parameter s-0-0044 with deg/s unit
      if (false == ((m_scalingData.m_velScalingType >> SERCOS_SCA_VEL_TYPE_UNIT_OFFSET) & 1U))
      {
        if (TimeUnit::SECOND == m_timeUnit)
        {
          unit = Unit::REVOLUTIONS_PER_SECOND;
        }
        else
        {
          unit = Unit::REVOLUTIONS_PER_MINUTE;
        }
        m_velMetrication = false;
      }
      else
      {
        if (TimeUnit::SECOND == m_timeUnit)
        {
          unit = Unit::DEGREE_PER_SECOND;
        }
        else
        {
          unit = Unit::DEGREE_PER_MINUTE;
        }
        m_velMetrication = true;
      }
      break;
    }
    default:
    {
      Scaling::m_velocityType = VelScalingType::UNDEFINED;
      APR_TRACE_ERROR("%s; Unknown scaling method detected in S-0-0044 (Bits 2-0)!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
      m_velScalingLSBWeight = 1.0;
      m_velScalingLSBWeight_inv = 1.0;
      return AxisProfileResult::ERR_NOTIMPLEMENTED;
    }
  }
  // velocity in m/s or degree/s
  UnitHelper::convertToSI(unitObjType, UnitValueType::VELOCITY, (m_scalingData.m_velScalingFactor * std::pow(10U, m_scalingData.m_velScalingExponent)), unit, m_velScalingLSBWeight);
  if (fabs(m_velScalingLSBWeight) < 1E-10)
  {
    APR_TRACE_ERROR("%s; LSB %s weight calculation has resulted in 0 (Set to 1 and continue)!", m_profileBasicInfo.m_profileTrace.createTraceMsg(), unitValueTypeToString(UnitValueType::VELOCITY));
    m_velScalingLSBWeight = 1.0;
    m_velScalingLSBWeight_inv = 1.0;
    result = AxisProfileResult::ERR_MATH;
  }
  m_velScalingLSBWeight_inv = double(1.0) / m_velScalingLSBWeight;
  return result;
}

AxisProfileResult ScalingStrategySoEDrive::readTorqueScalingParamsSync()
{
  std::vector<SercosParamStruct> readStructSet;
  readStructSet.reserve(7);
  readStructSet.emplace_back(S_0_0086_SCALING_TORQUE_TYPE,      &m_scalingData.m_torqueScalingType,     sizeof(m_scalingData.m_torqueScalingType));  // S-0-0086 (Torque data scaling type)
  readStructSet.emplace_back(S_0_0093_SCALING_TORQUE_FACTOR,    &m_scalingData.m_torqueScalingFactor,   sizeof(m_scalingData.m_torqueScalingFactor)); // S-0-0093 (Torque data scaling factor)
  readStructSet.emplace_back(S_0_0094_SCALING_TORQUE_EXPONENT,  &m_scalingData.m_torqueScalingExponent, sizeof(m_scalingData.m_torqueScalingExponent)); // S-0-0094 (Torque data scaling exponent)
  readStructSet.emplace_back(S_0_0533_NOMINAL_TORQUE,           &m_scalingData.m_motorRatedTorqueRaw,   sizeof(m_scalingData.m_motorRatedTorqueRaw)); // S-0-0533 (Nominal torque/force of motor)
  readStructSet.emplace_back(S_0_0121_GEAR_INPUT_REVOLUTION,    &m_scalingData.m_inputRevolution,       sizeof(m_scalingData.m_inputRevolution)); // S-0-0121 (Gear input revolution)
  readStructSet.emplace_back(S_0_0122_GEAR_OUTPUT_REVOLUTION,   &m_scalingData.m_outputRevolution,      sizeof(m_scalingData.m_outputRevolution)); // S-0-0122 (gear output revolution) 
  readStructSet.emplace_back(S_0_0123_FEED_CONSTANT,            &m_scalingData.m_feedConstant,          sizeof(m_scalingData.m_feedConstant));  // S-0-0123 (feed constant)

  AxisProfileResult result{};
  for (const auto& SercosParamStruct : readStructSet)
  {
    result = getParameterValueFromSnapshot(SercosParamStruct.m_requestIDN, static_cast<std::uint8_t*>(SercosParamStruct.m_ptrData), SercosParamStruct.dataSize);
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED,
                         m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "Torque scaling, SoE IDN: %s, error: 0x%04X", idnToString(SercosParamStruct.m_requestIDN).c_str(), result);
      return AxisProfileResult::STS_BOOTING_CONTINUE;
    }
  }

  if (m_scalingData.m_outputRevolution == 0 || m_scalingData.m_motorRatedTorqueRaw == 0 || m_scalingData.m_feedConstant == 0)
  {
    Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_TORQUE_FORCE_SCALING_TYPE,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Value should not zero, S-0-0122 is: %d, S-0-0123 is: %d, S-0-0533 is: %d",
                       m_scalingData.m_outputRevolution, m_scalingData.m_feedConstant, m_scalingData.m_motorRatedTorqueRaw);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  return result;
}

AxisProfileResult ScalingStrategySoEDrive::calcTorqueScalingLSBWeight()
{
  if (!m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // read torque scaling from drive, torque scaling should always read because fixedstop may use the scaling
  AxisProfileResult result = readTorqueScalingParamsSync();
  if (aprStatusFailed(result))
  {
    m_torqueScalingLSBWeight = 1.0;
    m_torqueScalingLSBWeight_inv = 1.0 / m_torqueScalingLSBWeight;
    return result;
  }
  else if (result == AxisProfileResult::STS_BOOTING_CONTINUE)
  {
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_WRN, DETAILED_DIAG_FUNCTION_NOT_SUPPORTED,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Torque related function not support");
  }

  // analysis the metrication
  m_torqueMetrication = !((m_scalingData.m_torqueScalingType >> SERCOS_SCA_TORQUE_TYPE_UNIT_OFFSET) & 0x1U);

  Unit unit{};

  // analysis the scaling type: percentage(000), linear(001), rotary(010)
  // store the SI conversion value
  switch (m_scalingData.m_torqueScalingType & SERCOS_SCA_TORQUE_TYPE_METHOD_MASK)
  {
    case SERCOS_SCA_TORQUE_TYPE_METHOD_PCTG:
    {
      // according to drive engineering, percentage always has exponent -1
      m_torqueScalingLSBWeight = 1.0 * std::pow(10.0, -1);
      Scaling::m_torqueType = TorqueScalingType::PERCENTAGE;
      break;
    }
    case SERCOS_SCA_TORQUE_TYPE_METHOD_LINEAR:
    {
      unit = m_torqueMetrication ? Unit::NEWTON : Unit::POUND_FORCE;
      UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::FORCE, (m_scalingData.m_torqueScalingFactor * std::pow(10.0, m_scalingData.m_torqueScalingExponent)), unit, m_torqueScalingLSBWeight);
      Scaling::m_torqueType = TorqueScalingType::LINEAR;
      break;
    }
    case SERCOS_SCA_TORQUE_TYPE_METHOD_ROT:
    {
      unit = m_torqueMetrication ? Unit::NEWTON_METERS : Unit::POUND_INCH;
      UnitHelper::convertToSI(UnitObjType::ROTATIONAL, UnitValueType::TORQUE, (m_scalingData.m_torqueScalingFactor * std::pow(10.0, m_scalingData.m_torqueScalingExponent)), unit, m_torqueScalingLSBWeight);      
      Scaling::m_torqueType = TorqueScalingType::ROTARY;
      break;
    }    
    default:
    {
      Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_TORQUE_FORCE_SCALING_TYPE,
                         m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "Torque scaling type error, IDN %s, Bits2-0", idnToString(S_0_0086_SCALING_TORQUE_TYPE).c_str());
      m_torqueScalingLSBWeight = 1.0;
      m_torqueScalingLSBWeight_inv = 1.0;
      Scaling::m_torqueType = TorqueScalingType::UNDEFINED;
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
  }

  if (fabs(m_torqueScalingLSBWeight) < 1E-7)
  {
    Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_TORQUE_FORCE_SCALING_TYPE,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Torque scaling error, LSB %s weight 0", unitValueTypeToString(UnitValueType::TORQUE));
    m_torqueScalingLSBWeight = 1;
    m_torqueScalingLSBWeight_inv = 1;
    return AxisProfileResult::ERR_MATH;
  }
  m_torqueScalingLSBWeight_inv = 1.0 / m_torqueScalingLSBWeight;

  AxisProfileDeviceScalingType torqueScalingType = getDeviceScalingType(axisprofile::AxisProfileDeviceValueType::TORQUE_FORCE);
  AxisProfileDeviceScalingType posScalingType = getDeviceScalingType(axisprofile::AxisProfileDeviceValueType::POSITION);

  // the transmitted value S-0-0533 (m_scalingData.rateTorque) is with always 3 place right move in decimal, E.g. 900->0.9 in SI
  double motorRateTorque = m_scalingData.m_motorRatedTorqueRaw * DECIM_PL3;
  // consider ratio
  double axisRateTorque = motorRateTorque * m_scalingData.m_inputRevolution / m_scalingData.m_outputRevolution;

  if (posScalingType == AxisProfileDeviceScalingType::LINEAR)
  {
    // convert feedconstant to SI: pitch decimal place decided by S-0-0077/S-0-0078
    double pitch {1.0};
    UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::POSITION, (m_scalingData.m_posScalingFactor * std::pow(10.0, m_scalingData.m_posScalingExponent)), Unit::METER, pitch);
    pitch *= m_scalingData.m_feedConstant;

    // Linear axis has consider feedconstant: 2*pi*Torque = Force*Pitch --> Force = 2*pi*Torque/Pitch 
    m_rateTorqueForce = axisRateTorque * 2 * PI / pitch;
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
  if (torqueScalingType != posScalingType && posScalingType != AxisProfileDeviceScalingType::UNDEFINED)
  {
    result = convertTorqueToSI(posScalingType, torqueScalingType, m_rateTorqueForce);
  }
  return result;
}

AxisProfileResult ScalingStrategySoEDrive::readAccScalingParamsSync()
{
  std::vector<SercosParamStruct> readStructSet{};
  readStructSet.reserve(3);
  readStructSet.emplace_back(S_0_0160_SCALING_ACC_TYPE,     &m_scalingData.m_accScalingType,      sizeof(m_scalingData.m_accScalingType)); // S-0-0160 (Acceleration data scaling type)
  readStructSet.emplace_back(S_0_0161_SCALING_ACC_FACTOR,   &m_scalingData.m_accScalingFactor,    sizeof(m_scalingData.m_accScalingFactor)); // S-0-0161 (Linear Acceleration data scaling factor)
  readStructSet.emplace_back(S_0_0162_SCALING_ACC_EXPONENT, &m_scalingData.m_accScalingExponent,  sizeof(m_scalingData.m_accScalingExponent)); // S-0-0162 (Linear Acceleration data scaling exponent)

  AxisProfileResult result{};
  for (const auto& SercosParamStruct : readStructSet)
  {
    result = getParameterValueFromSnapshot(SercosParamStruct.m_requestIDN,
                                          static_cast<std::uint8_t*>(SercosParamStruct.m_ptrData),
                                          SercosParamStruct.dataSize);
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_READ_PARAM_FAILED,
                         m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "Acceleration scaling, SoE IDN: %s, error: 0x%04X", idnToString(SercosParamStruct.m_requestIDN).c_str(), result);
      return result;
    }
  }
  return result;
}

AxisProfileResult ScalingStrategySoEDrive::calcAccScalingLSBWeight()  
{
  AxisProfileResult result = readAccScalingParamsSync();
  if (aprStatusFailed(result))
  {
    m_accScalingLSBWeight = 1.0;
    m_accScalingLSBWeight_inv = 1.0 / m_accScalingLSBWeight;
    return result;
  }
  UnitObjType unitObjType{ UnitObjType::ANY };
  Unit unit{};
  switch (m_scalingData.m_accScalingType & SERCOS_SCA_ACC_TYPE_METHOD_MASK)
  {
    case SERCOS_SCA_ACC_TYPE_METHOD_NONE:
    {
      Scaling::m_accelerationType = AccScalingType::UNDEFINED;
      // No Scaling method set (none). Set LSB weight to 1.
      m_accScalingLSBWeight = 1.0;
      break;
    }
    case SERCOS_SCA_ACC_TYPE_METHOD_LINEAR:
    {
      // Linear Scaling
      unitObjType = UnitObjType::LINEAR;
      Scaling::m_accelerationType = AccScalingType::LINEAR;
      if (false == ((m_scalingData.m_accScalingType >> SERCOS_SCA_ACC_TYPE_UNIT_OFFSET) & 1U))
      {
        // Unit for linear Scaling (Bit 4) = Metres
        // The drive is working with meters but showing millimeters as unit ( exponent of -7 leads to 4 places after "." eg. 0.0001)
        unit = Unit::METER_PER_SECOND_SQARED;
        m_accMetrication = true;
      }
      else
      {
        // Unit for linear Scaling (Bit 4) = Inch
        unit = Unit::INCH_PER_SECOND_SQARED;
        m_accMetrication = false;
      }
      break;
    }
    case SERCOS_SCA_ACC_TYPE_METHOD_ROT:
    {
      unitObjType = UnitObjType::ROTATIONAL;
      Scaling::m_accelerationType = AccScalingType::ROTARY;
      unit = Unit::RADIANS_PER_SECOND_SQARED;
      break;
    }
    default:
    {
      APR_TRACE_ERROR("%s; Unknown scaling method detected in S-0-0160 (Bits 2-0)!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
      Scaling::m_accelerationType = AccScalingType::UNDEFINED;
      m_accScalingLSBWeight = 1.0;
      m_accScalingLSBWeight_inv = 1.0;
      return AxisProfileResult::ERR_NOTIMPLEMENTED;
    }
  }
  UnitHelper::convertToSI(unitObjType, UnitValueType::ACCELERATION, (m_scalingData.m_accScalingFactor * std::pow(10U, m_scalingData.m_accScalingExponent)), unit, m_accScalingLSBWeight); 
  if (fabs(m_accScalingLSBWeight) < 1E-10)
  {
    APR_TRACE_ERROR("%s; LSB weight calculation has resulted in 0 (Set to 1 and continue)!", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    m_accScalingLSBWeight = 1.0;
    m_accScalingLSBWeight_inv = 1.0;
    return AxisProfileResult::ERR_MATH;
  }
  m_accScalingLSBWeight_inv = double(1.0) / m_accScalingLSBWeight;

  return result;
}

AxisProfileResult ScalingStrategySoEDrive::checkAndSetModulo(double modValue, std::string& dynErrTxt)
{
  if (fabs(modValue - m_moduloValue) > STD_EPS)
  {
    dynErrTxt = "Modulo value inconsistency, motion: " + std::to_string(modValue) + ", device: " + std::to_string(m_moduloValue);
    return AxisProfileResult::ERR_INCONSISTENCY;
  }
  dynErrTxt = "Modulo value consistency";
  return AxisProfileResult::STS_OK;
}
