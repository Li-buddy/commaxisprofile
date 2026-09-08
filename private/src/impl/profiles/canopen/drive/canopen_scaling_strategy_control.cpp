/// @file canopen_scaling_strategy_control.cpp
///
/// This source file is part of the axisprofile library and implements the
/// scaling calculation.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "canopen_scaling_strategy_control.h"
#include "diagnosis/dia.h"
#include "provider/provider_helper.h"
#include <cstdint>
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
ScalingStrategyCoEControl::ScalingStrategyCoEControl(ProfileBasicInfo& profileBasicInfo)
  :ScalingStrategy(profileBasicInfo)
{
  m_scalingStrategyCfg.m_scalingStrategy = AxisProfileScalingStrategy::COE_CALC_CONTROL;
  m_scalingStrategyCfg.m_deviceScalingType = AxisProfileDeviceScalingType::UNDEFINED;
  m_scalingStrategyCfg.m_denominator = 100000;
  m_scalingStrategyCfg.m_numerator = DEFAULT_ENCODER_VALUE;
  m_scalingStrategyCfg.m_resolution = DEFAULT_ENCODER_VALUE;

  m_modulo.init(getModValue());
  m_absolute.init();
}

AxisProfileResult ScalingStrategyCoEControl::calcScalingLSBWeights()
{
  // begin to calculate LSB (trigger from scaling)
  AxisProfileResult result = Scaling::executeCalcScalingLSBWeights();//template method related to calpos calvel calacc, and so on
  if (AxisProfileResult::STS_OK != result)
  {
    APR_TRACE_ERROR("%s; calculate the LSB value failed", m_profileBasicInfo.m_profileTrace.createTraceMsg(result));
  }
  // for control side scaling type don't need to check the uniform
  return result;
}

AxisProfileResult ScalingStrategyCoEControl::calcPosScalingLSBWeight()
{
  AxisProfileResult result = scalingLBSGeneralCalc(m_posScalingLSBWeight);
  if (aprStatusFailed(result))
  {
    return result;
  }

  m_posScalingLSBWeight_inv = 1.0 / m_posScalingLSBWeight;
  m_posMetrication = true;

  /// set position data format as ignore
  m_posDataFormat = AxisProfilePosDataFormat::FORMAT_IGNORE;
  double scaledLimitHigh = INT32_MAX * getPosScalingLSBWeight();
  double scaledLimitLow = INT32_MIN * getPosScalingLSBWeight();
  m_absolute.init(scaledLimitHigh, scaledLimitLow);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::calcVelScalingLSBWeight()
{
  AxisProfileResult result {};
  if (m_scalingStrategyCfg.m_velFactor.m_enabled)
  {
    result = calcStandaloneScalingFactor(m_velScalingLSBWeight);
  }
  else
  {
    result = scalingLBSGeneralCalc(m_velScalingLSBWeight);
  }

  // check for scaling calculation
  if (aprStatusFailed(result))
  {
    return result;
  }

  m_velScalingLSBWeight_inv = 1.0 / m_velScalingLSBWeight;
  m_velMetrication = true;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::calcStandaloneScalingFactor(double& scalingLSBWeight)
{
  if (m_scalingStrategyCfg.m_velFactor.m_denominator == 0 ||
      m_scalingStrategyCfg.m_velFactor.m_numerator == 0)
  {
    std::string txt {};
    txt += m_scalingStrategyCfg.m_velFactor.m_numerator == 0 ? " numerator is 0 " : "";
    txt += m_scalingStrategyCfg.m_velFactor.m_denominator == 0 ? " denominator is 0 " : "";
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_SCALING_VALUE_ERROR, __FILE__, __FUNCTION__, __LINE__, txt.c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  double ratio = (double)m_scalingStrategyCfg.m_velFactor.m_denominator / (double)m_scalingStrategyCfg.m_velFactor.m_numerator;
  if (AxisProfileDeviceScalingType::LINEAR == m_scalingStrategyCfg.m_deviceScalingType)
  {
    double resolution = 1.0 / ratio;  //mm/inv
    scalingLSBWeight = resolution * DECIM_PL3; //1000->mm-m
  }
  else if (AxisProfileDeviceScalingType::ROTATIONAL == m_scalingStrategyCfg.m_deviceScalingType)
  {
    double resolution = CIRCLE_DEG / ratio;  // deg/inv
    scalingLSBWeight = resolution;      // deg->deg
  }
  else
  {
    std::string url;
    provider::ProviderHelper::replaceFirstWideCharInAddress(provider::NodeTree.APR_PROFILES_CFG_SCALINGINFO_TYPE,
                                                            m_profileBasicInfo.m_profileName, url);

    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_SCALLING_TYPE_ERROR,
                       __FILE__, __FUNCTION__, __LINE__, "Scaling type %s, url %s",
                       axisProfileDeviceScalingTypeToString(m_scalingStrategyCfg.m_deviceScalingType), url.c_str());

    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::calcAccScalingLSBWeight()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::calcTorqueScalingLSBWeight()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::setScalingNumerator(const std::uint32_t scalingNumerator)
{
  m_scalingStrategyCfg.m_numerator = scalingNumerator;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::setScalingDenominator(const std::uint32_t scalingDenominator)
{
  m_scalingStrategyCfg.m_denominator = scalingDenominator;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::setEncoderResolution(const std::uint32_t m_resolution)
{
  m_scalingStrategyCfg.m_resolution = m_resolution;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::setDeviceScalingType(const AxisProfileDeviceScalingType deviceType)
{
  m_scalingStrategyCfg.m_deviceScalingType = deviceType;
  switch (deviceType)
  {
  case AxisProfileDeviceScalingType::ROTATIONAL:
    m_positionType = PosScalingType::ROTARY;
    m_velocityType = VelScalingType::ROTARY;
    m_accelerationType = AccScalingType::ROTARY;
    break;
  case AxisProfileDeviceScalingType::LINEAR:
    m_positionType = PosScalingType::LINEAR;
    m_velocityType = VelScalingType::LINEAR;
    m_accelerationType = AccScalingType::LINEAR;
    break;
  default:
    m_positionType = PosScalingType::UNDEFINED;
    m_velocityType = VelScalingType::ROTARY;
    m_accelerationType = AccScalingType::ROTARY;
    break;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::setVelocityFactorCfg(const ScalingFactor& scalingFactor)
{
  m_scalingStrategyCfg.m_velFactor = {scalingFactor.m_enabled, scalingFactor.m_numerator, scalingFactor.m_denominator};

  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::scalingLBSGeneralCalc(double& scalingLSBWeight)
{
  if (m_scalingStrategyCfg.m_numerator == 0 ||
      m_scalingStrategyCfg.m_denominator == 0 ||
      m_scalingStrategyCfg.m_resolution == 0)
  {
    std::string txt {};
    txt += m_scalingStrategyCfg.m_numerator == 0 ? " numerator is 0 " : "";
    txt += m_scalingStrategyCfg.m_denominator == 0 ? " denominator is 0 " : "";
    txt += m_scalingStrategyCfg.m_resolution == 0 ? " resolution is 0 " : "";
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_SCALING_VALUE_ERROR, __FILE__, __FUNCTION__, __LINE__, txt.c_str());

    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  double ratio = double(m_scalingStrategyCfg.m_denominator) / double(m_scalingStrategyCfg.m_numerator);
  if (AxisProfileDeviceScalingType::LINEAR == m_scalingStrategyCfg.m_deviceScalingType)
  {
    double resolution = 1.0 / (m_scalingStrategyCfg.m_resolution * ratio);  //mm/inv
    scalingLSBWeight = resolution * DECIM_PL3;                        //1000->mm-m
  }
  else if (AxisProfileDeviceScalingType::ROTATIONAL == m_scalingStrategyCfg.m_deviceScalingType)
  {
    double resolution = CIRCLE_DEG / (m_scalingStrategyCfg.m_resolution * ratio);  // deg/inv
    scalingLSBWeight = resolution;           //deg->deg
  }
  else
  {
    std::string url;
    provider::ProviderHelper::replaceFirstWideCharInAddress(provider::NodeTree.APR_PROFILES_CFG_SCALINGINFO_TYPE,
                                                            m_profileBasicInfo.m_profileName, url);
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_SCALLING_TYPE_ERROR,
                       __FILE__, __FUNCTION__, __LINE__, "Scaling type %s, url %s",
                       axisProfileDeviceScalingTypeToString(m_scalingStrategyCfg.m_deviceScalingType), url.c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ScalingStrategyCoEControl::checkAndSetModulo(double modValue, std::string& dynErrTxt)
{
  double scaledLimitHigh = INT32_MAX * getPosScalingLSBWeight();
  double scaledLimitLow = INT32_MIN * getPosScalingLSBWeight();
  m_modulo.init(modValue, scaledLimitHigh, scaledLimitLow);
  APR_TRACE_MESSAGE("Re-Init MDT modulo linear tool with modulo = %f.", modValue);
  return Scaling::checkAndSetModulo(modValue, dynErrTxt);
}

double ScalingStrategyCoEControl::convertPositionRawToScaled(std::int32_t rawTelegramValue) const
{
  double scaledValue = Scaling::convertPositionRawToScaled(rawTelegramValue);
  if (Scaling::getPosDataFormat() == AxisProfilePosDataFormat::FORMAT_MODULO)
  {
    scaledValue = m_modulo.convertLinToMod(scaledValue);
  }
  else
  {
    scaledValue = m_absolute.restoreLinValue(scaledValue);
  }
  return scaledValue;
}

std::int32_t ScalingStrategyCoEControl::convertPositionScaledToRaw(double scaledInternalValue) const
{
  double scaledValue;
  if (Scaling::getPosDataFormat() == AxisProfilePosDataFormat::FORMAT_MODULO)
  {
    format::PosFormatHelperMod::ModValuePair valuePair;
    m_modulo.convertModToLin(scaledInternalValue, valuePair);

    scaledValue = valuePair.turns * getPosScalingLSBWeight_inv() * getModValue() + valuePair.value * getPosScalingLSBWeight_inv();
  }
  else
  {
    double adjustValue = m_absolute.adjustForOverflow(scaledInternalValue);
    scaledValue = adjustValue * getPosScalingLSBWeight_inv();
  }
  if (scaledValue > INT32_MAX)
  {
    scaledValue = INT32_MAX;
  }
  else if (scaledValue < INT32_MIN)
  {
    scaledValue = INT32_MIN;
  }
  return static_cast<std::int32_t>(scaledValue);
}

void ScalingStrategyCoEControl::initModuloPosCmp(std::int32_t rawActPos)
{
  if (Scaling::getPosDataFormat() == AxisProfilePosDataFormat::FORMAT_MODULO)
  {
    double scaledValue = Scaling::convertPositionRawToScaled(rawActPos);
    m_modulo.resetModToLin(scaledValue);
  }
}
