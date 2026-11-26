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
  m_outOfRangeThreshold = 1;
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
  m_positionClamp.reset();
  m_velocityClamp.reset();
  m_torqueClamp.reset();

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
    return AxisProfileResult::ERR_NOK;
  }
  Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_INCONSISTENCE_SCALING_TYPE, __FILE__, __FUNCTION__, __LINE__,
                     "The device mapped to profile [%s] address : %d, exist Scaling type mismatch: position=%s, velocity=%s, acceleration=%s",
                     m_profileBasicInfo.m_profileName.c_str(),
                     m_profileBasicInfo.m_deviceAddress,
                     scalingTypeToString(m_positionType),
                     scalingTypeToString(m_velocityType),
                     scalingTypeToString(m_accelerationType));
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

template<typename T>
T Scaling::convertScaledToRaw(double scaledInternalValue, double inverseWeight, ClampState& state, const char* valueName) const
{
  const double rawValue = scaledInternalValue * inverseWeight;
  bool clamped = false;
  const T rawTelegramValue = clampCast<T>(rawValue, clamped);
  if (!clamped)
  {
    state.reset();
  }
  else if (updateClampCounter(state))
  {
    reportClampError(rawValue, rawTelegramValue, state, valueName);
  }
  return rawTelegramValue;
}

std::int32_t Scaling::convertPositionScaledToRaw(double scaledInternalValue) const
{
  return convertScaledToRaw<std::int32_t>(scaledInternalValue, getPosScalingLSBWeight_inv(), m_positionClamp, "Position");
}

double Scaling::convertVelocityRawToScaled(std::int32_t rawTelegramValue) const
{
  double convertedRawValue = static_cast<double>(rawTelegramValue);
  return (convertedRawValue) * getVelScalingLSBWeight();
}

std::int32_t Scaling::convertVelocityScaledToRaw(double scaledInternalValue) const
{
  return convertScaledToRaw<std::int32_t>(scaledInternalValue, getVelScalingLSBWeight_inv(), m_velocityClamp, "Velocity");
}

double Scaling::convertTorqueRawToScaled(std::int16_t rawTelegramValue) const
{
  double convertedRawValue = static_cast<double>(rawTelegramValue);
  return (convertedRawValue) * getTorqueScalingLSBWeight();
}

std::int16_t Scaling::convertTorqueScaledToRaw(double scaledInternalValue) const
{
  return convertScaledToRaw<std::int16_t>(scaledInternalValue, getTorqueScalingLSBWeight_inv(), m_torqueClamp, "Torque");
}

template<typename T>
void Scaling::reportClampError(double rawValue, T rawTelegramValue, ClampState& state, const char* valueName) const
{
  if (state.reported)
  {
    return;
  }

  Dia::sendDiagnosis(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_SCALING_VALUE_CLAMPED, m_profileBasicInfo.m_profileName.c_str(),
                      __FILE__, __FUNCTION__, __LINE__,
                      "%s data of profile [%s]. "
                      "Raw value [%.17g] exceeds range [%d, %d]. "
                      "Clamped output [%d].",
                      valueName,
                      m_profileBasicInfo.m_profileName.c_str(),
                      rawValue,
                      static_cast<int>(std::numeric_limits<T>::min()),
                      static_cast<int>(std::numeric_limits<T>::max()),
                      static_cast<int>(rawTelegramValue));
  state.reported = true;
}

std::uint32_t Scaling::getOutOfRangeErrCnt(const ScalingDataCategory& category) const
{
  switch (category)
  {
    case ScalingDataCategory::SCALINGDATA_POSITION:
      return m_positionClamp.count;
    case ScalingDataCategory::SCALINGDATA_VELOCITY:
      return m_velocityClamp.count;
    case ScalingDataCategory::SCALINGDATA_TORQUE:
      return m_torqueClamp.count;
    default:
      return 0;
  }
}

bool Scaling::setOutOfRangeErrLimit(std::uint32_t maxTimes)
{
  m_outOfRangeThreshold = maxTimes;
  m_positionClamp.reset();
  m_velocityClamp.reset();
  m_torqueClamp.reset();
  return true;
}

std::uint32_t Scaling::getOutOfRangeErrLimit() const
{
  return m_outOfRangeThreshold;
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

bool Scaling::updateClampCounter(ClampState& clamp) const
{
  if ((++clamp.count) >= m_outOfRangeThreshold)
  {
    clamp.count = 0;
    return true;
  }
  return false;
}
