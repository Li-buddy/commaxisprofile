// ---------------------------------------------------------------------------
//
//! @file       scaling.cpp
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2020
//
// ---------------------------------------------------------------------------
//
//! This source file is part of the axisprofile library and implements the
//! base class for scaling.
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "scaling.h"
#include "diagnosis/dia.h"
#include "profiles/data_sharing/sharing_handler.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------
const std::string Scaling::POSITION{"position"};          
const std::string Scaling::VELOCITY{"velocity"};         
const std::string Scaling::TORQUE{"torque"};            
const std::string Scaling::ACCELERATION{"acceleration"};

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------


Scaling::Scaling(ProfileBasicInfo& profileBasicInfo):
  m_profileBasicInfo(profileBasicInfo)
{
}

AxisProfilePosDataFormat Scaling::getPosDataFormat() const
{
  return m_posDataFormat;
}

double Scaling::getPosScalingLSBWeight() const
{
  return m_posScalingLSBWeight;
}

double Scaling::getPosScalingLSBWeight_inv() const
{
  return m_posScalingLSBWeight_inv;
}

double Scaling::getVelScalingLSBWeight() const
{
  return m_velScalingLSBWeight;
}

double Scaling::getVelScalingLSBWeight_inv() const
{
  return m_velScalingLSBWeight_inv;
}

double Scaling::getAccScalingLSBWeight() const
{
  return m_accScalingLSBWeight;
}

double Scaling::getAccScalingLSBWeight_inv() const
{
  return m_accScalingLSBWeight_inv;
}

double Scaling::getTorqueScalingLSBWeight() const
{
  return m_torqueScalingLSBWeight;
}

double Scaling::getTorqueScalingLSBWeight_inv() const
{
  return m_torqueScalingLSBWeight_inv;
}

void Scaling::setHomeOffset(double offset)
{
  m_offset = offset;
}

double Scaling::getHomeOffset() const
{
  return m_offset;
}

AxisProfileResult Scaling::booting()
{
  loadBootingTasks();
  return calcScalingLSBWeights();
}

AxisProfileResult Scaling::checkScalingUniform() const
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  const auto& mappitems = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if ((!mappitems.m_input.m_position && !mappitems.m_output.m_position)
      || (!mappitems.m_input.m_velocity && !mappitems.m_output.m_velocity))
  {
    // if position or velocity not mapped, then no need to check uniform
    return AxisProfileResult::STS_OK;
  }

  if((PosScalingType::ROTARY == m_positionType) 
  && (VelScalingType::ROTARY == m_velocityType) 
  && (AccScalingType::ROTARY == m_accelerationType) )
  {
    return AxisProfileResult::STS_OK;
  }

  if((PosScalingType::LINEAR == m_positionType) 
  && (VelScalingType::LINEAR == m_velocityType) 
  && (AccScalingType::LINEAR == m_accelerationType))
  {
    if(m_velMetrication == m_posMetrication 
    && m_velMetrication == m_accMetrication)
    {
      return AxisProfileResult::STS_OK;
    }
    Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_INCONSISTENCE_SCALING_TYPE, __FILE__, __FUNCTION__, __LINE__,
                       "the unit system of velocity, position and acceleration do not match with each other, position: %s, velocity: %s, acceleration: %s",
                       m_posMetrication ? "metric" : "imperial", m_velMetrication ? "metric" : "imperial", m_accMetrication ? "metric" : "imperial");
  }
  Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_INCONSISTENCE_SCALING_TYPE, __FILE__, __FUNCTION__, __LINE__,
                     "the scaling type of velocity, position and acceleration do not match with each other, position: %d, velocity: %d, acceleration: %d",
                     m_positionType, m_velocityType, m_accelerationType);
  return AxisProfileResult::ERR_NOK;
}

AxisProfileDeviceScalingType Scaling::getDeviceScalingType(const AxisProfileDeviceValueType valueType) const
{
  AxisProfileDeviceScalingType deviceScalingType{};
  switch (valueType)
  {
    case AxisProfileDeviceValueType::POSITION:
      if (PosScalingType::LINEAR == m_positionType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::LINEAR;
      }
      else if (PosScalingType::ROTARY == m_positionType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::ROTATIONAL;
      }
      else
      {
        deviceScalingType = AxisProfileDeviceScalingType::UNDEFINED;
      }
      break;
    case AxisProfileDeviceValueType::VELOCITY:
      if (VelScalingType::LINEAR == m_velocityType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::LINEAR;
      }
      else if (VelScalingType::ROTARY == m_velocityType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::ROTATIONAL;
      }
      else
      {
        deviceScalingType = AxisProfileDeviceScalingType::UNDEFINED;
      }
      break;
    case AxisProfileDeviceValueType::ACCELERATION:
      if (AccScalingType::LINEAR == m_accelerationType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::LINEAR;
      }
      else if (AccScalingType::ROTARY == m_accelerationType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::ROTATIONAL;
      }
      else
      {
        deviceScalingType = AxisProfileDeviceScalingType::UNDEFINED;
      }
      break;      
    case AxisProfileDeviceValueType::TORQUE_FORCE:
      if (TorqueScalingType::LINEAR == m_torqueType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::LINEAR;
      }
      else if (TorqueScalingType::ROTARY == m_torqueType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::ROTATIONAL;
      }
      else if (TorqueScalingType::PERCENTAGE == m_torqueType)
      {
        deviceScalingType = AxisProfileDeviceScalingType::PERCENTAGE;
      }
      else 
      {
        deviceScalingType = AxisProfileDeviceScalingType::UNDEFINED;
      }
      break;
    default:
      deviceScalingType = AxisProfileDeviceScalingType::UNDEFINED;
      break;
  }
  return deviceScalingType;
}

AxisProfileResult Scaling::executeCalcScalingLSBWeights()
{
  AxisProfileResult result{AxisProfileResult::STS_OK};  
  for (auto& method : m_scalingLSBWeightsMethods)
  {
    AxisProfileResult funcResult = method.m_fnct();
    if (aprStatusFailed(funcResult))
    {
      APR_TRACE_ERROR("%s; Could not calculate %s data scaling factor", method.m_methodName->c_str(), m_profileBasicInfo.m_profileTrace.createTraceMsg());
      result = funcResult;
    }
    APR_TRACE_MESSAGE("%s; New %s data scaling LSB weight = %g", m_profileBasicInfo.m_profileTrace.createTraceMsg(), method.m_methodName->c_str(), *(method.m_ptrScalingLSBWeight));
  }
  return result;
}

// Data converter
double Scaling::convertPositionRawToScaled(std::int32_t rawTelegramValue) const
{
  double convertedRawValue = static_cast<double>(rawTelegramValue);
  return (convertedRawValue) * getPosScalingLSBWeight();
}

std::int32_t Scaling::convertPositionScaledToRaw(double scaledInternalValue) const
{
  std::int32_t rawTelegramValue = static_cast<std::int32_t>(scaledInternalValue * getPosScalingLSBWeight_inv());
  return rawTelegramValue;
}

double Scaling::convertVelocityRawToScaled(std::int32_t rawTelegramValue) const
{
  double convertedRawValue = static_cast<double>(rawTelegramValue);
  return (convertedRawValue) * getVelScalingLSBWeight();
}
std::int32_t Scaling::convertVelocityScaledToRaw(double scaledInternalValue) const
{
  std::int32_t rawTelegramValue = static_cast<std::int32_t>(scaledInternalValue * getVelScalingLSBWeight_inv());
  return rawTelegramValue;
}

double Scaling::convertTorqueRawToScaled(std::int16_t rawTelegramValue) const
{
  double convertedRawValue = static_cast<double>(rawTelegramValue);
  return (convertedRawValue) * getTorqueScalingLSBWeight();
}

std::int16_t Scaling::convertTorqueScaledToRaw(double scaledInternalValue) const
{
  std::int16_t rawTelegramValue = static_cast<std::int16_t>(scaledInternalValue * getTorqueScalingLSBWeight_inv());
  return rawTelegramValue;
}

AxisProfileResult Scaling::checkAndSetModulo(double modValue, std::string& dynErrTxt)
{
  // Note: profile does not support build in modulo by default
  setPosDataFormat(AxisProfilePosDataFormat::FORMAT_MODULO);
  m_moduloValue = modValue;
  dynErrTxt = "Modulo value set to " + std::to_string(modValue);
  APR_TRACE_MESSAGE("%s; %s", m_profileBasicInfo.m_profileTrace.createTraceMsg(), dynErrTxt.c_str());
  return AxisProfileResult::STS_OK;
}

double Scaling::getModValue() const
{
  return m_moduloValue;
}

double Scaling::getCycleTime() const
{
  return m_cycleTime;
}

void Scaling::setCycleTime(double cycleTime)
{
  m_cycleTime = cycleTime;
}

AxisProfileResult Scaling::setPosDataFormat(AxisProfilePosDataFormat format)
{
  m_posDataFormat = format;
  return AxisProfileResult::STS_OK;
}

void Scaling::initModuloPosCmp(std::int32_t /*rawActPos*/)
{

}

double Scaling::getRateTorqueForce() const
{
  return m_rateTorqueForce;
}

void Scaling::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}
