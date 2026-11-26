/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for device parameter handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_device_param_drive.h"
#include "bundle/axisprofile_device_param_c.h"
#include "common/string_helper.h"
#include "diagnosis/dia.h"
#include "helper/device_id_helper.h"
#include "profiles/i_device_boot_param_snapshot.h"

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

AxisProfileDeviceParamDrive::AxisProfileDeviceParamDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  :AxisProfileDeviceParam(profileBasicInfo, at, mdt)
{
}

AxisProfileResult AxisProfileDeviceParamDrive::booting()
{
  getDeviceInformation(); // calculate software version before using checksum parameter

  bool isNodeChecksumEnable = m_sharingHandler->getPropertyIF()->isChecksumEnabled();
  m_needReadBootParams = isNodeChecksumEnable ? checkNeedReadBootParams() : true;
  APR_TRACE_MESSAGE("%s, m_needReadBootParams: %d, node checksumEnable:%d", m_profileBasicInfo.m_profileTrace.createTraceMsg(), m_needReadBootParams, isNodeChecksumEnable);

  return AxisProfileDeviceParam::booting();
}

bool AxisProfileDeviceParamDrive::isNeedReadBootParams() const
{
  return m_needReadBootParams;
}

bool AxisProfileDeviceParamDrive::checkBrakeDelaySupport() const
{
  return m_brakeDelySupport;
}

AxisProfileResult AxisProfileDeviceParamDrive::getErrorBrakeDelayInfo(const AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTime)
{
  if (errorType == AxisProfileDeviceErrorType::ERROR_NOT_DEFINED)
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, __FILE__, __FUNCTION__, __LINE__,
                       "Brake delay invalid for ErrorType of %s", axisProfileDeviceErrorTypeToString(errorType));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  auto error = m_delayInfoMap.find(errorType);
  if (error != m_delayInfoMap.end())
  {
    isActivated = error->second.m_isActivated;
    delayTime = error->second.m_delayTime;
  }
  else
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, __FILE__, __FUNCTION__, __LINE__,
                       "Brake delay not support for ErrorType of %d", axisProfileDeviceErrorTypeToString(errorType));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileDeviceParamDrive::getErrorBrakeDynLimit(const AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk)
{
  if (errorType == AxisProfileDeviceErrorType::ERROR_NOT_DEFINED)
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, __FILE__, __FUNCTION__, __LINE__,
                       "Brake dynamic limit invalid for ErrorType of %s", axisProfileDeviceErrorTypeToString(errorType));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  auto error = m_reactionInfoMap.find(errorType);
  if ((error != m_reactionInfoMap.end()) && m_decelerationLimitSupport)
  {
    deceleration = error->second.m_deceleration;
    jerk = error->second.m_jerk;
  }
  else
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, __FILE__, __FUNCTION__, __LINE__,
                       "Brake dynamic limit not support for ErrorType of %s", axisProfileDeviceErrorTypeToString(errorType));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileDeviceParamDrive::getPositionDataFormat(AxisProfilePosDataFormat& posDataFormat)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  posDataFormat = m_sharingHandler->getScalingIF()->getPosDataFormat();
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileDeviceParamDrive::checkAndSetModulo(double modValue, std::string& dynErrTxt)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    dynErrTxt = "Profile scaling is nullptr";
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  return m_sharingHandler->getScalingIF()->checkAndSetModulo(modValue, dynErrTxt);
}

DeviceCfgData& AxisProfileDeviceParamDrive::getDeviceCfgData()
{
  return m_deviceCfgData;
}

bool AxisProfileDeviceParamDrive::isLatchParamSupport() const
{
  return m_prbLatTimesSupport;
}

bool AxisProfileDeviceParamDrive::checkNeedReadBootParams()
{
  //1. check whether the drive support the checksum
  if (!isDriveSupportChecksum())
  {
    APR_TRACE_WARNING("%s, the drive not has checksum ability", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return true;
  }

  //2. check checksum value
  std::uint32_t checkSum {};
  AxisProfileResult res = readChecksumValueFromDevice(checkSum);
  if (aprStatusFailed(res))
  {
    APR_TRACE_WARNING("%s, get checksum error", m_profileBasicInfo.m_profileTrace.createTraceMsg(res));
    return true;
  }

  IDeviceBootParamSnapshot* snapshotIF = m_sharingHandler->getBootParamSnapshotIF();
  if (!snapshotIF)
  {
    APR_TRACE_WARNING("%s, boot parameter snapshot interface not found", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return true;
  }

  BootParamSnapshot snapshot = snapshotIF->getBootParamSnapshot();
  bool checksumValueDiff = (snapshot.checksumValue != checkSum);
  if (checksumValueDiff)
  {
    APR_TRACE_MESSAGE("%s, update the checksum value:0x%X -> 0x%X", m_profileBasicInfo.m_profileTrace.createTraceMsg(), snapshot.checksumValue, checkSum);
    snapshotIF->updateChecksumValue(checkSum);
    //return true;
  }
  APR_TRACE_MESSAGE("%s, whether the checksum value changed: %d", m_profileBasicInfo.m_profileTrace.createTraceMsg(), checksumValueDiff);

  //3. check checksum idn list
  return checkBootIdnListModified() || checksumValueDiff;
}

bool AxisProfileDeviceParamDrive::isDriveSupportChecksum()
{
  uint8_t majorV {}, minorV {};
  if (aprStatusFailed(m_sharingHandler->getPropertyIF()->getDeviceSoftwareVersion(majorV, minorV)))
  {
    APR_TRACE_WARNING("%s, read drive version failed.", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return false;
  }
  DeviceSeries series = m_sharingHandler->getPropertyIF()->getDeviceSeries();
  AxisProfileType profileType = m_profileBasicInfo.m_axisProfileType;
  APR_TRACE_MESSAGE("%s, isDriveSupportChecksum version: %d.%d, series: %d. profileType: %d", m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                    majorV, minorV, series, profileType);

  return DeviceIdHelper::deviceSupportCheckSum(majorV, minorV, series, profileType);
}
