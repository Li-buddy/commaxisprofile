/// @file
///
/// @brief This header file is part of the axisprofile library and implement SercosDeviceParamDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "diagnosis/dia.h"
#include "fieldbuses/fieldbus_helper.h"
#include "helper/checksum_idn_list_helper.h"
#include "helper/device_id_helper.h"
#include "profiles/sercos/cmd_jobs/sercos_job_device_param.h"
#include "profiles/sercos/sercos_boot_params.h"
#include "sercos_device_param_drive.h"
#include "sercos_signal_func_handler.h"

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
constexpr std::uint16_t SOFTWARE_LIMIT_ACTIVE = 0x0010;           ///< binary 0000 0000 0001 0000
constexpr std::size_t LIST_BUFF_CHECKSUM_PARAM_LIST_SIZE = 4858;  ///< Max buffer size for IDN list of checksum parameters, indra drive: 0x12FA

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

SercosDeviceParamDrive::SercosDeviceParamDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  :AxisProfileDeviceParamDrive(profileBasicInfo, at, mdt)
{
  m_signalFuncHandler = new SercosSignalFuncHandler(profileBasicInfo);
  m_bootingDataOpMode.clear();
  m_bootingDataOpMode.emplace(OperationMode::PRIMARY,    GenParamU16{S_0_0032_OP_MODE_PRIMARY,    0 });
  m_bootingDataOpMode.emplace(OperationMode::SECONDARY1, GenParamU16{S_0_0033_OP_MODE_SECONDARY1, 0 });
  m_bootingDataOpMode.emplace(OperationMode::SECONDARY2, GenParamU16{S_0_0034_OP_MODE_SECONDARY2, 0 });
  m_bootingDataOpMode.emplace(OperationMode::SECONDARY3, GenParamU16{S_0_0035_OP_MODE_SECONDARY3, 0 });
  m_bootingDataOpMode.emplace(OperationMode::SECONDARY4, GenParamU16{S_0_0284_OP_MODE_SECONDARY4, 0 });
  m_bootingDataOpMode.emplace(OperationMode::SECONDARY5, GenParamU16{S_0_0285_OP_MODE_SECONDARY5, 0 });
  m_bootingDataOpMode.emplace(OperationMode::SECONDARY6, GenParamU16{S_0_0286_OP_MODE_SECONDARY6, 0 });
  m_bootingDataOpMode.emplace(OperationMode::SECONDARY7, GenParamU16{S_0_0287_OP_MODE_SECONDARY7, 0 });    
}

void SercosDeviceParamDrive::loadBootingTasks()
{
  m_initDeviceParam.clear();
  m_initDeviceParam.reserve(10);
  m_initDeviceParam.emplace_back([this]{return getDeviceParamJobObj();});

  // prepareSignalFuncMgr -> it may managed by check sum
  m_initDeviceParam.emplace_back([this]{return prepareSignalFuncHandler();});

  if (!m_needReadBootParams)
  {
    APR_TRACE_MESSAGE("%s, Not need to read SoE drive parameters again.", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return;
  }
  m_initDeviceParam.emplace_back([this]{return readDelayBrakeParam();});
  m_initDeviceParam.emplace_back([this]{return getDelayBrakeInfo();});
  m_initDeviceParam.emplace_back([this]{return getBrakeDynLimit();});
  m_initDeviceParam.emplace_back([this]{return readOpModeParam();});
  m_initDeviceParam.emplace_back([this]{return readDriveLimits();});
  m_initDeviceParam.emplace_back([this]{return readEncodersTypes();});
  m_initDeviceParam.emplace_back([this]{return readDcConfigInfo();});
  m_initDeviceParam.emplace_back([this]{return readPosProcessMode();});  
}

AxisProfileResult SercosDeviceParamDrive::getDeviceInformation()
{
  std::size_t readLen {};
  std::array<std::uint8_t, LIST_BUFF_DEFAULT_SIZE> rawBuf {}; // buffer for raw data reading
  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, S_0_0030_SW_VERSION, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};
  requestData.maxLen = static_cast<std::uint32_t>(rawBuf.size());

  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, rawBuf.data(), rawBuf.size(), readLen);
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__,
                       "Manufacturer software version, SoE IDN: %s, error: 0x%X",
                       idnToString(S_0_0030_SW_VERSION).c_str(), result);
  }

  std::string versionStr;
  std::vector<char> charBuf {};
  FieldbusHelper::rawToParamListSoE(rawBuf.data(), readLen, charBuf);
  versionStr.assign(charBuf.begin(), charBuf.end());
  APR_TRACE_MESSAGE("device address: %u, firmware version: %s", m_profileBasicInfo.m_deviceAddress, versionStr.c_str());

  result = m_sharingHandler->getPropertyIF()->setDeviceManufacturerVersion(versionStr);//set manufacturer version,then can get software version
  if (aprStatusFailed(result))
  {
    APR_TRACE_WARNING("Software version parse failed. IDN %s value: %s, error: 0x%X", idnToString(S_0_0030_SW_VERSION).c_str(), versionStr.c_str(), result);
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Software version parse failed. IDN %s value: %s, error: 0x%X",
                       idnToString(S_0_0030_SW_VERSION).c_str(), versionStr.c_str(), result);

  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosDeviceParamDrive::readDelayBrakeParam()
{
  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, S_0_0000_DUMMY, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};

  const bool brakeDelaySupported = DeviceIdHelper::isRexrothCtrlXDrive(m_sharingHandler->getPropertyIF()->getDeviceSeries());

  std::vector<SercosParamStruct> readStructSet;
  readStructSet.reserve(3);
  readStructSet.emplace_back(P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR,
                             &m_bootingDataMultiBrake.m_multiBrakeDelaySupport,
                             sizeof(m_bootingDataMultiBrake.m_multiBrakeDelaySupport));

  if (brakeDelaySupported)
  {
    readStructSet.emplace_back(P_0_0117_0_1_REACTION_DELAY_F2_F3,
                               &m_bootingDataMultiBrake.m_controlUnitReactionTimeLightError,
                               sizeof(m_bootingDataMultiBrake.m_controlUnitReactionTimeLightError));
    readStructSet.emplace_back(P_0_0117_0_2_REACTION_DELAY_F4_F6_F7,
                               &m_bootingDataMultiBrake.m_controlUnitReactionTimeCriticalError,
                               sizeof(m_bootingDataMultiBrake.m_controlUnitReactionTimeCriticalError));
  }

  AxisProfileResult result {};
  for (const auto& readParam : readStructSet)
  {
    requestData.idn = readParam.m_requestIDN;
    result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(readParam.m_ptrData), readParam.dataSize);
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                         __FILE__, __FUNCTION__, __LINE__, "Delay brake, SoE DIN: %s, error: 0x%X", idnToString(readParam.m_requestIDN).c_str(), result);
      m_brakeDelySupport = false;
      return AxisProfileResult::STS_OK;
    }
  }

  m_brakeDelySupport = brakeDelaySupported;
  return AxisProfileResult::STS_OK;
}  

AxisProfileResult SercosDeviceParamDrive::getDelayBrakeInfo()
{
  if (!m_brakeDelySupport)
  {
    APR_TRACE_MESSAGE("%s; The brake delay is not supported", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::STS_OK;
  }
  else
  {
    AxisProfileResult result{};
    std::vector<MultiBrakeInfo> multiBrakeInfoStep;
    MultiBrakeInfo stepValue{};

    stepValue = (m_bootingDataMultiBrake.m_multiBrakeDelaySupport & SERCOS_MULTI_BRAKE_LIGHT_ERROR) == SERCOS_MULTI_BRAKE_LIGHT_ERROR ? MultiBrakeInfo::LIGHT_ERROR_ACTIVATED : MultiBrakeInfo::LIGHT_ERROR_INACTIVATED;
    multiBrakeInfoStep.push_back(stepValue);

    stepValue = (m_bootingDataMultiBrake.m_multiBrakeDelaySupport & SERCOS_MULTI_BRAKE_FIELDBUS_ERROR) == SERCOS_MULTI_BRAKE_FIELDBUS_ERROR ? MultiBrakeInfo::FIELDBUS_ERROR_ACTIVATED : MultiBrakeInfo::FIELDBUS_ERROR_INACTIVATED;
    multiBrakeInfoStep.push_back(stepValue);

    stepValue = (m_bootingDataMultiBrake.m_multiBrakeDelaySupport & SERCOS_MULTI_BRAKE_CRITICAL_ERROR) == SERCOS_MULTI_BRAKE_CRITICAL_ERROR ? MultiBrakeInfo::CRITICAL_ERROR_ACTIVATED : MultiBrakeInfo::CRITICAL_ERROR_INACTIVATED;
    multiBrakeInfoStep.push_back(stepValue);  

    m_delayInfoMap.clear();
    for (size_t switchStep = 0; switchStep < multiBrakeInfoStep.size(); ++ switchStep)
    {
      switch (multiBrakeInfoStep[switchStep])
      {
        case MultiBrakeInfo::LIGHT_ERROR_ACTIVATED:
        {
          
          m_brakeDelayInfo.m_isActivated = true;
          m_brakeDelayInfo.m_delayTime = m_bootingDataMultiBrake.m_controlUnitReactionTimeLightError;
          //convert the time unit from millisecond to second
          m_brakeDelayInfo.m_delayTime *=  std::pow(10, -3);   
          m_delayInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_LIGHT, m_brakeDelayInfo));
          break;
        }
        case MultiBrakeInfo::LIGHT_ERROR_INACTIVATED:
        {
          m_brakeDelayInfo.m_isActivated = false;
          m_brakeDelayInfo.m_delayTime = m_bootingDataMultiBrake.m_controlUnitReactionTimeLightError;
          //convert the time unit from millisecond to second
          m_brakeDelayInfo.m_delayTime *=  std::pow(10, -3);
          m_delayInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_LIGHT, m_brakeDelayInfo));
          break;
        }
        case MultiBrakeInfo::FIELDBUS_ERROR_ACTIVATED:
        {
          
          m_brakeDelayInfo.m_isActivated = true;
          m_brakeDelayInfo.m_delayTime = m_bootingDataMultiBrake.m_controlUnitReactionTimeCriticalError;
          //convert the time unit from millisecond to second
          m_brakeDelayInfo.m_delayTime *=  std::pow(10, -3);
          m_delayInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_FIELDBUS, m_brakeDelayInfo));
          break;
        }
        case MultiBrakeInfo::FIELDBUS_ERROR_INACTIVATED:
        {
          m_brakeDelayInfo.m_isActivated = false;
          m_brakeDelayInfo.m_delayTime = m_bootingDataMultiBrake.m_controlUnitReactionTimeCriticalError;
          //convert the time unit from millisecond to second
          m_brakeDelayInfo.m_delayTime *=  std::pow(10, -3);
          m_delayInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_FIELDBUS, m_brakeDelayInfo));
          break;
        }
        case MultiBrakeInfo::CRITICAL_ERROR_ACTIVATED:
        {
          
          m_brakeDelayInfo.m_isActivated = true;
          m_brakeDelayInfo.m_delayTime = m_bootingDataMultiBrake.m_controlUnitReactionTimeCriticalError;
          //convert the time unit from millisecond to second
          m_brakeDelayInfo.m_delayTime *=  std::pow(10, -3);
          m_delayInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_CRITICAL, m_brakeDelayInfo));
          break;
        }
        case MultiBrakeInfo::CRITICAL_ERROR_INACTIVATED:
        {
          m_brakeDelayInfo.m_isActivated = false;
          m_brakeDelayInfo.m_delayTime = m_bootingDataMultiBrake.m_controlUnitReactionTimeCriticalError;
          //convert the time unit from millisecond to second
          m_brakeDelayInfo.m_delayTime *=  std::pow(10, -3);
          m_delayInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_CRITICAL, m_brakeDelayInfo)); 
          break;
        }
        default:
        {
          result = AxisProfileResult::STS_OK;
          break;
        }
      }
    }
    result = AxisProfileResult::STS_OK;
    return result;
  }
}

AxisProfileResult SercosDeviceParamDrive::getBrakeDynLimit()
{
  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, S_0_0000_DUMMY, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};

  bool checkDecelerationLimitSupport{false};
  std::vector<SercosParamStruct> readStructSet;
  readStructSet.reserve(5);
  readStructSet.emplace_back(P_0_0119_CONTROL_UNIT_BEST_POSSIBLE_DEC,  &m_bootingDataMultiBrake.m_controlUnitReactionBestDeceleration, sizeof(m_bootingDataMultiBrake.m_controlUnitReactionBestDeceleration)); // P-0-3893 (CONTROL_UNIT_REACTION_TIME_F4_F6_F7)
  readStructSet.emplace_back(S_0_0429_EMERGENCY_HALT_DECELERATION,     &m_bootingDataMultiBrake.m_emergencyBrakeDeceleration,          sizeof(m_bootingDataMultiBrake.m_emergencyBrakeDeceleration));
  readStructSet.emplace_back(S_0_0138_BIPOLAR_ACC_LIMIT_VALUE,         &m_bootingDataMultiBrake.m_bipolarAccelerationLimitValue,       sizeof(m_bootingDataMultiBrake.m_bipolarAccelerationLimitValue));
  readStructSet.emplace_back(S_0_0372_DRIVE_HALT_ACC_BIPOLAR,          &m_bootingDataMultiBrake.m_driveHaltAccelerationBipolar,        sizeof(m_bootingDataMultiBrake.m_driveHaltAccelerationBipolar));
  readStructSet.emplace_back(S_0_0349_BIPOLAR_JERK_LIMIT,              &m_bootingDataMultiBrake.m_bipolarJerkLimit,                    sizeof(m_bootingDataMultiBrake.m_bipolarJerkLimit));
  AxisProfileResult result{};
  for (const auto& readParam : readStructSet)
  {
    requestData.idn = readParam.m_requestIDN;
    result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(readParam.m_ptrData), readParam.dataSize);
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                         __FILE__, __FUNCTION__, __LINE__, "Brake Dynamic limit, SoE DIN: %s, error: 0x%X", idnToString(readParam.m_requestIDN).c_str(), result);
      continue;
    } 
  }
  result = mappingBrakeInfo(checkDecelerationLimitSupport);
  m_decelerationLimitSupport = checkDecelerationLimitSupport;

  return result;
}

AxisProfileResult SercosDeviceParamDrive::mappingBrakeInfo(bool& checkDecelerationLimitSupport)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
    
  m_reactionInfoMap.clear();
  std::uint16_t brakeReactionType = m_bootingDataMultiBrake.m_controlUnitReactionBestDeceleration & VALUE_BIT_0_TO_BIT3;
  ErrorBrakeReaction errorValue{};

  if (brakeReactionType == BRAKE_REACTION_VALUE_2)
  {
    errorValue = ErrorBrakeReaction::QUICK_STOP;
  }
  else if (brakeReactionType == BRAKE_REACTION_VALUE_3)
  {
    errorValue = ErrorBrakeReaction::RETURN_MOTION;
  }
  else if (brakeReactionType == BRAKE_REACTION_VALUE_4)
  {
    errorValue = ErrorBrakeReaction::EMERGENCY_STOP_WITH_DECELERATION;
  }

  auto scaling = m_sharingHandler->getScalingIF();
  switch (errorValue)
  {
    case ErrorBrakeReaction::QUICK_STOP:
    {
      m_brakeReaction.m_deceleration = m_bootingDataMultiBrake.m_driveHaltAccelerationBipolar != 0 ? m_bootingDataMultiBrake.m_driveHaltAccelerationBipolar : m_bootingDataMultiBrake.m_bipolarAccelerationLimitValue;

      //Convert the unit of deceleration from micron/s2 and jerk from micron/s3 to SI
      m_brakeReaction.m_deceleration *= scaling->getAccScalingLSBWeight();
      m_brakeReaction.m_jerk = m_bootingDataMultiBrake.m_bipolarJerkLimit * scaling->getAccScalingLSBWeight();
      checkDecelerationLimitSupport = true;
      m_reactionInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_LIGHT, m_brakeReaction));
      m_reactionInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_FIELDBUS, m_brakeReaction));
      break;
    }
    case ErrorBrakeReaction::RETURN_MOTION:
    {
      m_brakeReaction.m_deceleration = m_bootingDataMultiBrake.m_returnAcceleration * scaling->getAccScalingLSBWeight();

      //Convert the unit of deceleration from micron/s2 to SI
      m_brakeReaction.m_jerk = m_bootingDataMultiBrake.m_returnJerk * scaling->getAccScalingLSBWeight();
      checkDecelerationLimitSupport = true;
      m_reactionInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_LIGHT, m_brakeReaction));
      m_reactionInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_FIELDBUS, m_brakeReaction));
      break;
    }
    case ErrorBrakeReaction::EMERGENCY_STOP_WITH_DECELERATION:
    {
      m_brakeReaction.m_deceleration = m_bootingDataMultiBrake.m_emergencyBrakeDeceleration != 0 ? m_bootingDataMultiBrake.m_emergencyBrakeDeceleration : m_bootingDataMultiBrake.m_bipolarAccelerationLimitValue;

      //Convert the unit of deceleration from micron/s2 and jerk from micron/s3 to SI
      m_brakeReaction.m_deceleration *= scaling->getAccScalingLSBWeight();
      m_brakeReaction.m_jerk = m_bootingDataMultiBrake.m_bipolarJerkLimit * scaling->getAccScalingLSBWeight();
      checkDecelerationLimitSupport = true;
      m_reactionInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_LIGHT, m_brakeReaction));
      m_reactionInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_FIELDBUS, m_brakeReaction));
      break;
    }
    default:
    {
      checkDecelerationLimitSupport = false;
      break;
    }
  }

  if ((m_bootingDataMultiBrake.m_controlUnitReactionBestDeceleration & VALUE_BIT_4_TO_BIT7) == BRAKE_REACTION_VALUE_40)
  {
    m_brakeReaction.m_deceleration = m_bootingDataMultiBrake.m_emergencyBrakeDeceleration != 0 ? m_bootingDataMultiBrake.m_emergencyBrakeDeceleration : m_bootingDataMultiBrake.m_bipolarAccelerationLimitValue;

    //Convert the unit of deceleration from micron/s2 and jerk from micron/s3 to SI
    m_brakeReaction.m_deceleration *= scaling->getAccScalingLSBWeight();
    m_brakeReaction.m_jerk = m_bootingDataMultiBrake.m_bipolarJerkLimit * scaling->getAccScalingLSBWeight();
    checkDecelerationLimitSupport = true;
    m_reactionInfoMap.insert(std::make_pair(AxisProfileDeviceErrorType::ERROR_CRITICAL, m_brakeReaction));
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosDeviceParamDrive::readOpModeParam()
{
  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, S_0_0000_DUMMY, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};

  AxisProfileResult result{};
  for (auto& readParam : m_bootingDataOpMode)
  {
    requestData.idn = readParam.second.m_idn;
    result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, reinterpret_cast<std::uint8_t*>(&readParam.second.m_value), sizeof(readParam.second.m_value));
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                         __FILE__, __FUNCTION__, __LINE__, "OP mode setting, SoE DIN: %s, error: 0x%X", idnToString(readParam.second.m_idn).c_str(), result);
      return AxisProfileResult::ERR_FIELDBUS_READ_VALUE;
    }
  }
  determineOpModeAvailable();

  return  AxisProfileResult::STS_OK;
}

AxisProfileResult SercosDeviceParamDrive::determineOpModeAvailable()
{
  m_opModeAvailInfoSercos.clearOpModeAvailInfoSercos();

  for (const auto& opMode : m_bootingDataOpMode)
  {
    switch (static_cast<OperationModeList>(opMode.second.m_value))
    {
      case OperationModeList::POSITION_CONTROL_ENCODER_1:
      case OperationModeList::POSITION_CONTROL_ENCODER_2:
      case OperationModeList::POSITION_CONTROL_LAGLESS_ENCODER_1:
      case OperationModeList::POSITION_CONTROL_LAGLESS_ENCODER_2:
      case OperationModeList::POSITION_CONTROL_DRIVE_CONTROLLED_ENCODER_1:
      case OperationModeList::POSITION_CONTROL_DRIVE_CONTROLLED_ENCODER_2:
      case OperationModeList::POSITION_CONTROL_DRIVE_CONTROLLED_LAGLESS_ENCODER_1:
      case OperationModeList::POSITION_CONTROL_DRIVE_CONTROLLED_LAGLESS_ENCODER_2:
      case OperationModeList::POSITION_CONTROL:
      case OperationModeList::PISITION_CONTROL_DRIVE_CONTROLLED:
        m_opModeAvailInfoSercos.positionAvail.available = true;
        m_opModeAvailInfoSercos.setOpModeAvailInfoSercos(opMode.first, AxisProfileOperationMode::POSITIONING );
        break;
      case OperationModeList::VELOCITY_CONTROL:
        m_opModeAvailInfoSercos.velocityAvail.available = true;
        m_opModeAvailInfoSercos.setOpModeAvailInfoSercos(opMode.first, AxisProfileOperationMode::VELOCITY);
        break;
      case OperationModeList::TORQUE_FORCE_CONTROL_VEL_LIMIT:
        m_opModeAvailInfoSercos.torqueForceAvail.available = true;
        m_opModeAvailInfoSercos.setOpModeAvailInfoSercos(opMode.first, AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT);
        break;
      default:
        m_opModeAvailInfoSercos.setOpModeAvailInfoSercos(opMode.first, AxisProfileOperationMode::UNDEFINED);
        break;
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosDeviceParamDrive::readEncodersTypes()
{
  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, S_0_0000_DUMMY, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};

  std::vector<SercosParamStruct> readStructSet {};
  readStructSet.reserve(2);
  readStructSet.emplace_back(S_0_0277_ENCODER_1_PROPERTY, &m_deviceCfgData.m_encoder1Property, sizeof(m_deviceCfgData.m_encoder1Property));
  readStructSet.emplace_back(S_0_0115_ENCODER_2_PROPERTY, &m_deviceCfgData.m_encoder2Property, sizeof(m_deviceCfgData.m_encoder2Property));
  AxisProfileResult result {AxisProfileResult::STS_OK};
  for (const auto& SercosParamStruct : readStructSet)
  {
    requestData.idn = SercosParamStruct.m_requestIDN;
    result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(SercosParamStruct.m_ptrData), SercosParamStruct.dataSize);
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, m_profileBasicInfo.m_profileName.c_str(),
                         __FILE__, __FUNCTION__, __LINE__, "Encoder type, SoE DIN: %s, error: 0x%X", idnToString(requestData.idn).c_str(), result);
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosDeviceParamDrive::readDriveLimits()
{
  if (!m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  std::uint16_t softwareLimit {};
  std::int32_t positivePosLimit {};
  std::int32_t negativePosLimit {};
  std::int32_t positiveVelLimit {};
  std::int32_t negativeVelLimit {};
  std::uint32_t bipolarVelLimit {};
  std::uint32_t bipolarAccLimit {};
  std::uint16_t trqLimitMax {};

  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, S_0_0000_DUMMY, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};

  std::vector<SercosParamStruct> readStructSet {};
  readStructSet.reserve(7);
  const auto& mappitems = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  bool flagPosCheck = mappitems.m_input.m_position || mappitems.m_output.m_position;
  if (flagPosCheck)
  {
    readStructSet.emplace_back(S_0_0055_POSITION_POLARITIES, &softwareLimit, sizeof(softwareLimit));
    readStructSet.emplace_back(S_0_0049_POSITIVE_POS_LIMIT, &positivePosLimit, sizeof(positivePosLimit));
    readStructSet.emplace_back(S_0_0050_NEGATIVE_POS_LIMIT, &negativePosLimit, sizeof(negativePosLimit));
  }
  
  readStructSet.emplace_back(S_0_0038_POSITIVE_VEL_LIMIT, &positiveVelLimit, sizeof(positiveVelLimit));
  readStructSet.emplace_back(S_0_0039_NEGATIVE_VEL_LIMIT, &negativeVelLimit, sizeof(negativeVelLimit));
  readStructSet.emplace_back(S_0_0091_BIPOLARE_VEL_LIMIT, &bipolarVelLimit, sizeof(bipolarVelLimit));
  readStructSet.emplace_back(S_0_0138_BIPOLAR_ACC_LIMIT_VALUE, &bipolarAccLimit, sizeof(bipolarAccLimit));

  AxisProfileResult result;
  for (const auto& SercosParamStruct : readStructSet)
  {
    requestData.idn = SercosParamStruct.m_requestIDN;
    result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(SercosParamStruct.m_ptrData), SercosParamStruct.dataSize);
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                         __FILE__, __FUNCTION__, __LINE__, "Drive limit, SoE DIN: %s, error: 0x%X", idnToString(SercosParamStruct.m_requestIDN).c_str());
      return result;
    }
  }
  if (flagPosCheck)
  {
    m_deviceCfgData.m_softwareLimitActive = (softwareLimit & SOFTWARE_LIMIT_ACTIVE) == SOFTWARE_LIMIT_ACTIVE ? true : false;
    m_deviceCfgData.m_positivePosLimit = positivePosLimit;
    m_deviceCfgData.m_negativePosLimit = negativePosLimit;
  }

  // read the max value, which is the physics torque limit of the hardware
  requestData.elementFlag = comm::ethercat::master::fbs::ElementFlags::ElementFlags_maxValue;
  requestData.idn = S_0_0092_BIPOLARE_TORQUE_LIMIT;
  result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, reinterpret_cast<std::uint8_t*>(&trqLimitMax), sizeof(trqLimitMax));
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_READ_PARAM_FAILED,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Max value of SoE IDN: %s, error: 0x%04X", idnToString(requestData.idn).c_str(), result);
  }

  // here actual judge which limitation take effect, and then store
  if (0 == positiveVelLimit || bipolarVelLimit < (std::uint32_t)abs(positiveVelLimit))
  {
    m_deviceCfgData.m_positiveVelLimit = bipolarVelLimit;
  }
  else
  {
    m_deviceCfgData.m_positiveVelLimit = positiveVelLimit;
  }

  if (0 == negativeVelLimit || bipolarVelLimit < (std::uint32_t)abs(negativeVelLimit))
  {
    // Because bipolarVelLimit can only be positive, it should be negated here
    m_deviceCfgData.m_negativeVelLimit = 0 - bipolarVelLimit;
  }
  else
  {
    m_deviceCfgData.m_negativeVelLimit = negativeVelLimit;
  }

  m_deviceCfgData.m_accLimit = bipolarAccLimit == 0 ? UINT32_MAX : bipolarAccLimit;
  m_deviceCfgData.m_trqLimitMax = trqLimitMax == 0 ? UINT16_MAX : trqLimitMax;

  return AxisProfileResult::STS_OK;
}

OpModeAvailInfo* SercosDeviceParamDrive::getOpModeAvailInfo()
{
  return &m_opModeAvailInfoSercos;
}

bool SercosDeviceParamDrive::checkFunctionSupported(const FunctionSupport function, FunctionSupportErrorInfo& errorInfo)
{
  bool result = true;
  switch (function)
  {
    case FunctionSupport::OPMODE_CYCLIC:
    {
      const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
      if (!profileAssignedInfo.m_output.m_controlWord || !profileAssignedInfo.m_input.m_statusWord)
      {
        result = false;
        errorInfo = {FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, "ControlWord or StatusWord not assigned"};
      }
    }
    break;
    case FunctionSupport::HOMING_STATUS_CYCLIC:
    {
      SignalFunction func;
      func.function = function;
      result = m_signalFuncHandler->isFuncInSignalStatusWord(func, errorInfo);
    }
    break;
    case FunctionSupport::ERROR_CODE_CYCLIC:
    {
      const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
      if (!profileAssignedInfo.m_input.m_diagcode)
      {
        result = false;
        errorInfo = {FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, "S-0-0390 or P-0-1901.0.12"};
      }
    }
    break;
    default:
    {
      result = false;
      errorInfo = {FuncSupportErrorType::ERROR_NOT_DEFINED, "invalid input parameter"};
    }
    break;
  }
  return result;
}

AxisProfileResult SercosDeviceParamDrive::readPosProcessMode()
{
  std::uint16_t posPrcMode {};
  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, P_0_0187_POS_PROCESS_MODE, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};

  auto result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, reinterpret_cast<std::uint8_t*>(&posPrcMode), sizeof(posPrcMode));
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_READ_PARAM_FAILED,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Position mode setting, SoE DIN: %s, error: 0x%X", idnToString(requestData.idn).c_str(), result);
    return AxisProfileResult::STS_BOOTING_CONTINUE;
  }

  PosProcessMode mode = toPosProcessMode(posPrcMode & 0x000F);
  if (mode != PosProcessMode::UNKNOWN_MODE)
  {
    m_hasOutputEffectiveDelay = true;

    // the end value of the buffer is also used in the calculation, so the latency is buffer (depth - 1)*Ts  
    m_outputEffectiveDelay = m_dcSync0CycleTime * (getPosProcessBufLen(mode) - 1);
  }
  else
  {
    APR_TRACE_MESSAGE("%s; Unknown position process mode of drive, mode = %d",
                      m_profileBasicInfo.m_profileTrace.createTraceMsg(), posPrcMode & 0x000F);
    return AxisProfileResult::STS_BOOTING_CONTINUE;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosDeviceParamDrive::getDeviceParamJobObj()
{
  if (!m_sharingHandler || !m_sharingHandler->getJobMgrIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_JobObj = m_sharingHandler->getJobMgrIF()->getJobObj(SercosJobDeviceParam::jobName());
  if (m_JobObj == nullptr)
  {
    APR_TRACE_MESSAGE("%s; %s not exit", m_profileBasicInfo.m_profileTrace.createTraceMsg(), SercosJobDeviceParam::jobName());
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosDeviceParamDrive::checkStandstill(bool& standStill)
{
  std::uint16_t value {};
  auto res = deviceParamSoE(m_profileBasicInfo, m_JobObj, ReadWriteType::READ,
                            S_0_0331_STANDSTILL_STATUS, value, sizeof(value));
  if (res == AxisProfileResult::STS_OK)
  {
    // bit0 of S-0-0331 indicates the standstill status
    standStill = (value & SERCOS_WORD_TOKEN_BIT0) != 0;
  }
  return res;
}

AxisProfileResult SercosDeviceParamDrive::readChecksumValueFromDevice(std::uint32_t& checksum)
{
  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, S_0_0326_CHECKSUM, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};

  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, reinterpret_cast<std::uint8_t*>(&checksum), sizeof(checksum));
  if (aprStatusFailed(result))
  {
    APR_TRACE_WARNING("device address: %u, get checksum error: 0x%X", m_profileBasicInfo.m_deviceAddress, result);
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                        __FILE__, __FUNCTION__, __LINE__, "Checksum, SoE DIN: %s, error: 0x%X", idnToString(requestData.idn).c_str(), result);
  }
  return result;
}

AxisProfileResult SercosDeviceParamDrive::readChecksumIdnListFromDevice(std::vector<std::uint16_t>& idnList)
{
  ParamReqDataSoE requestData {m_profileBasicInfo.m_deviceAddress, S_0_0327_CHECKSUM_IDN_LIST, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};
  requestData.maxLen = LIST_BUFF_CHECKSUM_PARAM_LIST_SIZE;

  std::array<std::uint8_t, LIST_BUFF_CHECKSUM_PARAM_LIST_SIZE> rawBuf {}; // buffer for raw data reading
  std::size_t readLen {};
  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, rawBuf.data(), rawBuf.size(), readLen);
  if (aprStatusFailed(result))
  {
    APR_TRACE_WARNING("device address: %u, checksum parameters list read error: 0x%X", m_profileBasicInfo.m_deviceAddress, result);
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Checksum parameters list read, SoE IDN: %s, error: 0x%X",
                       idnToString(requestData.idn).c_str(), result);
    return result;
  }

  result = FieldbusHelper::rawToParamListSoE(rawBuf.data(), readLen, idnList);
  if (aprStatusFailed(result))
  {
    APR_TRACE_WARNING("device address: %u, checksum parameters list parse error: 0x%X", m_profileBasicInfo.m_deviceAddress, result);
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_PARSE_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Checksum parameters list parse, SoE IDN: %s, error: 0x%X",
                       idnToString(requestData.idn).c_str(), result);
  }
  return result;
}

AxisProfileResult SercosDeviceParamDrive::writeChecksumIdnListToDevice(const std::vector<std::uint16_t>& idnList)
{
  std::vector<std::uint8_t> tmpBuff {};
  AxisProfileResult res = FieldbusHelper::paramListToRawSoE(idnList, tmpBuff);
  std::uint16_t idn = S_0_0327_CHECKSUM_IDN_LIST;
  if (aprStatusFailed(res))
  {
    APR_TRACE_WARNING("device address: %u, checksum parameters list parse error: 0x%X", m_profileBasicInfo.m_deviceAddress, res);
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_PARSE_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Checksum parameters list parse, SoE IDN: %s, error: 0x%X",
                       idnToString(idn).c_str(), res);
    return res;
  }

  ParamReqDataSoE writeData {m_profileBasicInfo.m_deviceAddress, idn, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value,
                             m_profileBasicInfo.m_driveNumber, tmpBuff.data(), static_cast<std::uint32_t>(tmpBuff.size()), LIST_BUFF_CHECKSUM_PARAM_LIST_SIZE};
  res = m_profileBasicInfo.m_fieldbusAdapter->writeSingleParamSync(writeData);
  if (aprStatusFailed(res))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_WRITE_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Checksum parameters list set, SoE IDN: %s, error: 0x%X",
                       idnToString(idn).c_str(), res);
  }
  return res;
}

bool SercosDeviceParamDrive::checkBootIdnListModified()
{
  DeviceSeries series = m_sharingHandler->getPropertyIF()->getDeviceSeries();
  std::vector<std::uint16_t> bootList = (series == DeviceSeries::CTRLXDRIVE) ? SOE_CTRLX_DRIVE_BOOT_PARAM_LIST : SOE_INDRA_DRIVE_BOOT_PARAM_LIST; //can't be other type in this function
  return ChecksumIdnListHelper::checkBootIdnListModifiedImpl<std::vector<std::uint16_t> >(m_checksumIdnList, bootList,
    [this] (std::vector<std::uint16_t>& list) {
      return this->readChecksumIdnListFromDevice(list);
    },
    [this] (const std::vector<std::uint16_t>& list) {
      return this->writeChecksumIdnListToDevice(list);
    },
    m_profileBasicInfo.m_deviceAddress
  );  
}

AxisProfileResult SercosDeviceParamDrive::getChecksumIdnList(std::vector<std::string>& idnStrListRet) const
{
  return ChecksumIdnListHelper::getChecksumIdnListStrImpl<std::uint16_t>(m_checksumIdnList, idnStrListRet, [] (std::uint16_t idn) {
                                                                          return idnToString(idn);
                                                                        });
}

AxisProfileResult SercosDeviceParamDrive::checkAxisHomed(bool& homed)
{
  AxisProfileResult res {AxisProfileResult::STS_OK};

  // get homed state via cyclic data
  if (isHomedInCyclicData())
  {
    homed = m_homed.load();
    return res;
  }

  // get homed state via async param access
  std::uint16_t value {};
  res = deviceParamSoE(m_profileBasicInfo, m_JobObj, ReadWriteType::READ,
                       S_0_0403_POSITION_FEEDBACK_VALUE_STATUS, value, sizeof(value));
  if (res == AxisProfileResult::STS_OK)
  {
    // S-0-0403.0 / 0x2508:01 Homed status of one of the encoder (encoder 1 or 2)
    if ((value & SERCOS_WORD_TOKEN_BIT0) == SERCOS_WORD_TOKEN_BIT0)
    {
      homed = true;
      m_homed.store(homed);
    }
  }
  return res;
}
