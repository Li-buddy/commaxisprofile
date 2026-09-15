/// @file
///
/// @brief This file is part of the axisprofile library and implements the
/// main class for the CiA402 profile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_cmd.h"
#include "canopen_profile.h"
#include "drive/canopen_status_control_drive.h"
#include "helper/datalayer_helper.h"
#include "profiles/axisprofile_diag_msg.h"
#include "profiles/category/drive/drive_category_adapter_coe.h"
#include "profiles/scaling/scaling_strategy.h"
#include "profiles/value_mapping/i_mapping_handler.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::canopen;
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
// Class implementation/
// ---------------------------------------------------------------------------

CanopenProfile::CanopenProfile(const std::string& profileName, const NetworkTechnology technology) :
  AxisProfile(profileName, AxisProfileType::CANOPEN_OVER_ETHERCAT, DriveProfileType::CANOPEN, technology)
{
  // create async handler
  m_paramAccess = new CanopenParamAccess(m_profileBasicInfo);

  // Create SercosCmd
  m_cmdIf = new CanopenCmd(m_profileBasicInfo);

  // create profile category object
  m_profileCategory = new DriveCategoryAdapterCoE(m_profileBasicInfo, &m_telegramAT, &m_telegramMDT);

  // Create sharing handler class
  m_profileCategory->registerSharingITFs(m_sharingHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY,      m_profileCategory);  
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS,   m_paramAccess);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CMD,           m_cmdIf);

  // add init methods
  loadInitMethod();
}

AxisProfileResult CanopenProfile::init()
{
  // Initialize the statusword with drive enabled
  m_telegramAT.m_statusWord.m_word = 1U;
  m_telegramAT.m_diagMsg.m_word = static_cast<std::uint32_t>(0U);

  return AxisProfile::init();
}

AxisProfileResult CanopenProfile::processDataAt(std::uint8_t* dataByteArray)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  IAxisProfileMappingHandler* const handler = getMappingHandler();
  std::uint16_t dummy{ 0 };
  const auto& realTimeInfos = handler->getInputRealtimeInfo();
  for (auto const& realTimeInfo : realTimeInfos)
  {
    std::uint8_t* data{ reinterpret_cast<std::uint8_t*>(&dummy) };
    switch (realTimeInfo.m_id)
    {
    case InputMappingValueID::STATUSWORD:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_statusWord.m_word));
      break;
    case InputMappingValueID::POSITION:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_actualPosition));
      break;
    case InputMappingValueID::VELOCITY:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_actualVelocity));
      break;
    case InputMappingValueID::DIAGMSG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_diagMsg.m_word));
      break;
    case InputMappingValueID::SWITCH_REF:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_digitalInput.m_bitRefSwitch.m_byte));
      break;
    case InputMappingValueID::SWITCH_POS_LIM:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_digitalInput.m_bitPosLimSwitch.m_byte));
      break;
    case InputMappingValueID::SWITCH_NEG_LIM:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_digitalInput.m_bitNegLimSwitch.m_byte));
      break;
    case InputMappingValueID::PROBE1_POSEDG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.posEdg));
      break;
    case InputMappingValueID::PROBE1_NEGEDG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.negEdg));
      break;
    case InputMappingValueID::PROBE1_POSLAT:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.posLatched));
      break;
    case InputMappingValueID::PROBE1_NEGLAT:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.negLatched));
      break;
    case InputMappingValueID::PROBE2_POSEDG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.posEdg));
      break;
    case InputMappingValueID::PROBE2_NEGEDG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.negEdg));
      break;
    case InputMappingValueID::PROBE2_POSLAT:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.posLatched));
      break;
    case InputMappingValueID::PROBE2_NEGLAT:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.negLatched));
      break;
    case InputMappingValueID::OP_MODE_DISPLAY:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_opModeDisplay));
      break;
    case InputMappingValueID::SMO_SAFETY_SELECTION:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_smoData.m_safetySelection));
      break;
    case InputMappingValueID::SMO_SAFETY_STATUS:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_smoData.m_safetyStatus));
      break;
    case InputMappingValueID::TORQUE_FORCE:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_actualTorqueInfo.m_actualTorqueForce));
      break;
    case InputMappingValueID::TORQUE_LIMIT_STATUS:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_actualTorqueInfo.m_torqueLimitStatus.m_word));
      break;
    case InputMappingValueID::USER_PDO_INPUT_1:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[0]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_2:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[1]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_3:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[2]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_4:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[3]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_5:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[4]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_6:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[5]));
      break;
    case InputMappingValueID::SIGNAL_STATUS_WORD:
      data = reinterpret_cast<uint8_t*>(&(m_telegramAT.m_signalStatusword));
      break;
    case InputMappingValueID::IO_IN_REFERENCE:
      data = reinterpret_cast<uint8_t*>(&(m_telegramAT.m_ioInReference.m_byte));
      break;
    case InputMappingValueID::HOMED_STATUS:
      data = reinterpret_cast<uint8_t*>(&(m_telegramAT.m_bitStatusHomed.m_byte));
      break;
    default:
      result = AxisProfileResult::ERR_WRONGDATATYPE;
      break;
    }
    DatalayerHelper::copyBits(data, 0, dataByteArray, realTimeInfo.m_bitOffset, realTimeInfo.m_bitSize);
  }

  checkForFPUError();
  // Interpret the new AT data
  m_profileCategory->getStatusControlInternal()->analyzeStatusWord();
  static_cast<AxisProfileDiagMsg*>(m_diagMsgInterface)->analyzeDiagMsg();
  checkForFPUError();

  return result;
}

AxisProfileResult CanopenProfile::processDataMdt(std::uint8_t* dataByteArray)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  IAxisProfileMappingHandler* const handler = getMappingHandler();
  std::uint16_t dummy{ 0 };
  const auto& realTimeInfos = handler->getOutputRealtimeInfo();
  for (auto const& realTimeInfo : realTimeInfos)
  {
    std::uint8_t* data{ reinterpret_cast<std::uint8_t*>(&dummy) };
    switch (realTimeInfo.m_id)
    {
    case OutputMappingValueID::CONTROLWORD:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_controlWord.m_word));
      break;
    case OutputMappingValueID::POSITION:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdPosition));
      break;
    case OutputMappingValueID::VELOCITY:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdVelocity));
      break;
    case OutputMappingValueID::OP_MODE:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_opMode));
      break;
    case OutputMappingValueID::TORQUE_FORCE:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdTorqueForceInfo.m_cmdTorqueForce));
      break;
    case OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdTorqueForceInfo.m_velLimitHigh));
      break;
    case OutputMappingValueID::TORQUE_VEL_LIMIT_LOW:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdTorqueForceInfo.m_velLimitLow));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_1:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[0]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_2:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[1]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_3:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[2]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_4:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[3]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_5:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[4]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_6:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[5]));
      break;
    case OutputMappingValueID::IO_SET_REFERENCE:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_ioEncoderData.m_setReference.m_byte));
      break;
    case OutputMappingValueID::IO_REFERENCE_OFFSET:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_ioEncoderData.m_referenceOffset));
      break;
    default:
      result = AxisProfileResult::ERR_WRONGDATATYPE;
      break;
    }
    DatalayerHelper::copyBits(dataByteArray, realTimeInfo.m_bitOffset, data, 0, realTimeInfo.m_bitSize);
  }
  return result;
}

AxisProfileResult CanopenProfile::getDeviceState()
{
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenProfile::setProfileScalingStrategyCfg(const AxisProfileScalingStrategyCfg& strategyCfg)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;    
  if (getProfileScalingStrategy() != strategyCfg.m_scalingStrategy)
  {
    result = setProfileScalingStrategy(strategyCfg.m_scalingStrategy);
    if (AxisProfileResult::STS_OK != result)
    {
      APR_TRACE_ERROR("%s; set scaling strategy to %s failed.", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileScalingStrategyToString(strategyCfg.m_scalingStrategy));
    }
  }
  setScalingNumerator(strategyCfg.m_numerator);
  setScalingDenominator(strategyCfg.m_denominator);
  setEncoderResolution(strategyCfg.m_resolution);
  setDeviceScalingType(strategyCfg.m_deviceScalingType);
  setVelocityFactorCfg(strategyCfg.m_velFactor);

  loadInitMethod();
  return result;
}

AxisProfileScalingStrategyCfg CanopenProfile::getProfileScalingStrategyCfg() const
{
  AxisProfileScalingStrategyCfg scalingcfg {getProfileScalingStrategy(),
    getDeviceScalingType(AxisProfileDeviceValueType::POSITION),getScalingNumerator(),
    getScalingDenominator(), getEncoderResolution(), getVelocityFactorCfg()};
  return scalingcfg;
}

AxisProfileResult CanopenProfile::setProfileScalingStrategy(const AxisProfileScalingStrategy& strategy)
{  
  return m_profileCategory->updateScalingStrategy(strategy);
}

AxisProfileScalingStrategy CanopenProfile::getProfileScalingStrategy() const
{
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return pStrategy->getScalingStrategy();
}

std::uint32_t CanopenProfile::getScalingNumerator() const
{
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return pStrategy->getScalingNumerator();
}

AxisProfileResult CanopenProfile::setScalingNumerator(const std::uint32_t numerator)
{
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  if (AxisProfileScalingStrategy::COE_CALC_CONTROL != getProfileScalingStrategy())
  {
    return AxisProfileResult::STS_OK;
  }
  return pStrategy->setScalingNumerator(numerator);
}

std::uint32_t CanopenProfile::getScalingDenominator() const
{
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return pStrategy->getScalingDenominator();
}

AxisProfileResult CanopenProfile::setScalingDenominator(const std::uint32_t denominator)
{
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  if (AxisProfileScalingStrategy::COE_CALC_CONTROL != getProfileScalingStrategy())
  {
    return AxisProfileResult::STS_OK;
  }
  return pStrategy->setScalingDenominator(denominator);
}

std::uint32_t CanopenProfile::getEncoderResolution()const
{
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return pStrategy->getEncoderResolution();
}

AxisProfileResult CanopenProfile::setEncoderResolution(const std::uint32_t m_resolution)
{
  if (AxisProfileScalingStrategy::COE_CALC_CONTROL != getProfileScalingStrategy())
  {
    return AxisProfileResult::STS_OK;
  }
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return pStrategy->setEncoderResolution(m_resolution);
}

AxisProfileResult CanopenProfile::setDeviceScalingType(const AxisProfileDeviceScalingType& deviceScalingType)
{
  if (AxisProfileScalingStrategy::COE_CALC_CONTROL != getProfileScalingStrategy())
  {
    return AxisProfileResult::STS_OK;
  }
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return pStrategy->setDeviceScalingType(deviceScalingType);
}

AxisProfileResult CanopenProfile::setVelocityFactorCfg(const ScalingFactor& scalingFactorSettings)
{
  if (AxisProfileScalingStrategy::COE_CALC_CONTROL != getProfileScalingStrategy())
  {
    return AxisProfileResult::STS_OK;
  }
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return pStrategy->setVelocityFactorCfg(scalingFactorSettings);
}

const ScalingFactor& CanopenProfile::getVelocityFactorCfg() const
{
  ScalingStrategy* pStrategy = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return pStrategy->getVelocityFactorCfg();
}

std::vector<std::unique_ptr<IJobObj>> CanopenProfile::onGetJobList()
{
  std::vector<std::unique_ptr<IJobObj>> jobs;
  jobs.reserve(5);
 
  jobs.emplace_back(std::make_unique<CanopenJobReset>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<CanopenJobHoming>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<CanopenJobDeviceParam>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<CanopenJobSetAbsPos>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<CanopenJobSetOpMode>(m_profileBasicInfo, m_sharingHandler, &m_telegramAT, &m_telegramMDT));

  return jobs;
}
