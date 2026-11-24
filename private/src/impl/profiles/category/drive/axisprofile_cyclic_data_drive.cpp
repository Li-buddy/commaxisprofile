/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for cyclic data handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_cyclic_data_drive.h"
#include "diagnosis/dia.h"
#include "fieldbuses/fieldbus_defs.h"
#include "helper/device_id_helper.h"

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

AxisProfileCyclicDataDrive::AxisProfileCyclicDataDrive(const ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  : AxisProfileCyclicData(profileBasicInfo), m_telegramAT(at), m_telegramMDT(mdt)
{
  
}

bool AxisProfileCyclicDataDrive::checkActualValueAvailabilityNRT(AxisProfileValueTypeID valueID, DataAccessErrorInfo& errInfo)
{  
  return AxisProfileCyclicData::checkActualValueAvailabilityNRT(valueID, errInfo);
}

AxisProfileResult AxisProfileCyclicDataDrive::getSingleUserPDODataFromDevice(UserPDOID id, UserPDODataInfo& actualPDOValues)
{
  //check for nullptr
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  //check for argument correctness
  if (id >= USER_PDO_COUNT_LIMIT)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  //check if the item are active(mapped?)
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (profileAssignedInfo.m_input.m_userPDOs.m_userPDO[id])
  {
    actualPDOValues.m_userPDOData = m_telegramAT->m_userPDOInputs[id];
    actualPDOValues.m_active = true;
  }
  else
  {
    actualPDOValues.m_active = false;
    return AxisProfileResult::ERR_ERR_DATA_NOT_MAPPED;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCyclicDataDrive::setSingleUserPDODataToDevice(UserPDOID id, UserPDODataInfo newUserPDOInfo)
{
  //check for nullptr
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  //check for argument correctness
  if (id >= USER_PDO_COUNT_LIMIT)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  //check if the item are active(mapped?)
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (profileAssignedInfo.m_output.m_userPDOs.m_userPDO[id])
  {
    m_telegramMDT->m_userPDOOutputs[id] = newUserPDOInfo.m_userPDOData;
  }
  else
  {
    return AxisProfileResult::ERR_ERR_DATA_NOT_MAPPED;
  }

  return AxisProfileResult::STS_OK;
}

bool AxisProfileCyclicDataDrive::checkSingleUserPDOActiveFromDevice(UserPDOID id)
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return false;
  }

  if (id >= USER_PDO_COUNT_LIMIT)
  {
    return false;
  }
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  return profileAssignedInfo.m_input.m_userPDOs.m_userPDO[id];
}

bool AxisProfileCyclicDataDrive::checkSingleUserPDOActiveToDevice(UserPDOID id)
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return false;
  }

  if (id >= USER_PDO_COUNT_LIMIT)
  {
    return false;
  }
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  return profileAssignedInfo.m_output.m_userPDOs.m_userPDO[id];
}

AxisProfileResult AxisProfileCyclicDataDrive::getDigitalInputValueBit(DigitalInputValueBitType valueType, bool& inputValue)
{
  if (!m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  AxisProfileResult result = AxisProfileResult::STS_OK;

  switch (valueType)
  {
    case DigitalInputValueBitType::REFERENCE_SWITCH_BIT:
      result = !profileAssignedInfo.m_input.m_switchRef ? AxisProfileResult::ERR_PROFILE_CONFIG : AxisProfileResult::STS_OK;
      inputValue = m_telegramAT->m_digitalInput.m_bitRefSwitch.m_bit;
      break;
    case DigitalInputValueBitType::POSITIVE_LIMIT_SWITCH_BIT:
      result = !profileAssignedInfo.m_input.m_switchPosLim ? AxisProfileResult::ERR_PROFILE_CONFIG : AxisProfileResult::STS_OK;
      inputValue = m_telegramAT->m_digitalInput.m_bitPosLimSwitch.m_bit;
      break;
    case DigitalInputValueBitType::NEGATIVE_LIMIT_SWITCH_BIT:
      result = !profileAssignedInfo.m_input.m_switchNegLim ? AxisProfileResult::ERR_PROFILE_CONFIG : AxisProfileResult::STS_OK;
      inputValue = m_telegramAT->m_digitalInput.m_bitNegLimSwitch.m_bit;
      break;
    default:
      result = AxisProfileResult::ERR_NOK;
      inputValue = false;
      break;
  }
  return result;
}

bool AxisProfileCyclicDataDrive::checkIfMatchLimitation(AxisProfileValueTypeID valueID, DataAccessErrorInfo& msg)
{
  if (!m_sharingHandler || !m_sharingHandler->getDeviceParamIF())
  {
    return false;
  }
  //get device configuration data from booting cmp
  DeviceCfgData deviceCfgData = m_sharingHandler->getDeviceParamIF()->getDeviceCfgData();
  switch (valueID)
  {
    case AxisProfileValueTypeID::ACTUAL_POSITION:
    {
      // carefully check the value here should be raw value or the scaled value, should keep same
      const std::int32_t actualPosition = m_telegramAT->m_atData.m_actualPosition;
      if (!deviceCfgData.m_softwareLimitActive)
      {
        return true;
      }
      if (deviceCfgData.m_negativePosLimit > actualPosition ||
          deviceCfgData.m_positivePosLimit < actualPosition)
      {
        //beyond position limitation on drive side
        msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_BEYOND_THE_LIMIT;
        msg.m_additionInfo = "POSITION : " + std::string(dataAccessCommonErrorTypeToString(msg.m_accessErrorType));
        Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_DL_INVALID_VALUE, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "check the actual position is out of position limitation on drive side.");
        return false;
      }
      break;
    }
    case AxisProfileValueTypeID::ACTUAL_VELOCITY:
    {
      const std::int32_t actualVelocity = m_telegramAT->m_atData.m_actualVelocity;
      if (deviceCfgData.m_negativeVelLimit > actualVelocity ||
          deviceCfgData.m_positiveVelLimit < actualVelocity)
      {
        msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_BEYOND_THE_LIMIT;
        msg.m_additionInfo = "VELOCITY : " + std::string(dataAccessCommonErrorTypeToString(msg.m_accessErrorType));
        Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_DL_INVALID_VALUE, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "check the actual velocity is out of velocity limitation on drive side.");
        return false;
      }
      break;
    }
    case AxisProfileValueTypeID::ACTUAL_ACCELERATION:  // now skip for check
    case AxisProfileValueTypeID::ACTUAL_TORQUE:        // now skip for check
      // unnecessary to check torque/acceleration during booting
      break;
    case AxisProfileValueTypeID::LAST_ID:
    default:
      msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_NOT_PROVIDED;
      msg.m_additionInfo = dataAccessCommonErrorTypeToString(msg.m_accessErrorType);
      Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_DL_INVALID_VALUE, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "undefined axis profile value type ID.");
      return false;
  }
  return true;
}

AxisProfileResult AxisProfileCyclicDataDrive::initTorqueLimitFunction()
{
  // the property interface initialized in booting procedure
  DeviceSeries series = m_property->getDeviceSeries();
  if (!DeviceIdHelper::isRexrothCtrlXDrive(series))
  {
    m_torqueStatusFn = &torqueVelLimitNotViolate;
    return AxisProfileResult::STS_OK;
  }

  uint8_t majorV {}, minorV {};
  if (aprStatusFailed(m_property->getDeviceSoftwareVersion(majorV, minorV)))
  {
    m_torqueStatusFn = &torqueVelLimitNotViolate;
    return AxisProfileResult::STS_OK;
  }

  bool isNewVersion = (majorV > firmware_related::TORQUE_LIMIT_STATUS_MAJOR_VERSION) ||
    (majorV == firmware_related::TORQUE_LIMIT_STATUS_MAJOR_VERSION && minorV >= firmware_related::TORQUE_LIMIT_STATUS_MINOR_VERSION);

  m_torqueStatusFn = isNewVersion ? &torqueVelLimitNewVersion : &torqueVelLimitOldVersion;
  return AxisProfileResult::STS_OK;
}

TorqueVelocityLimitStatus AxisProfileCyclicDataDrive::torqueVelLimitNotViolate(const WordBits&)
{
  return TorqueVelocityLimitStatus::NO_VIOLATE;
}

TorqueVelocityLimitStatus AxisProfileCyclicDataDrive::torqueVelLimitOldVersion(const WordBits& statusBits)
{
  // For version lower than 06V06, using bit0 and bit1 of SoE: P-0-2251/CoE:0x60CC.
  if (statusBits.bit0)
  {
    return TorqueVelocityLimitStatus::VIOLATE_HIGH_VEL;
  }

  if (statusBits.bit1)
  {
    return TorqueVelocityLimitStatus::VIOLATE_LOW_VEL;
  }

  return TorqueVelocityLimitStatus::NO_VIOLATE;
}

TorqueVelocityLimitStatus AxisProfileCyclicDataDrive::torqueVelLimitNewVersion(const WordBits& statusBits)
{
  // For version 06V06 and higher using bit1 and bit2 of SoE : P-0-2251 / CoE:0x60CC.
  if (statusBits.bit1)
  {
    return TorqueVelocityLimitStatus::VIOLATE_HIGH_VEL;
  }

  if (statusBits.bit2)
  {
    return TorqueVelocityLimitStatus::VIOLATE_LOW_VEL;
  }

  return TorqueVelocityLimitStatus::NO_VIOLATE;
}

AxisProfileResult AxisProfileCyclicDataDrive::checkSafetyOperationMode(SafetyOperationMode& mode)
{
  // This calculation is based on the following parameters:
  // * P-0-3264, SMO: Safety function selection
  // * P-0-3264.0.1, SMO: Safety function status
  AxisProfileResult res = AxisProfileResult::STS_OK;
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (!profileAssignedInfo.m_input.m_safetySelection || !profileAssignedInfo.m_input.m_safetyStatus)
  {
    res = AxisProfileResult::ERR_ERR_DATA_NOT_MAPPED;
  }
  else
  {
    SafetyOperationMode modeInSafetySelection = SafetyOperationMode::SOM_NORMAL_MODE;
    // Check the cyclic data of the SMO: Safety function selection.
    auto& safetySelection = m_telegramAT->m_smoData.m_safetySelection;
    // bit4        (0:Selected/1:Not selected) indicates Safely-limited speed (SLS)
    // bit26~bit30 (0b00000:Special mode safe motion not selected, else SMM1~SMM16 selected.)
    if ((safetySelection & MASK1(32, 1, 4)) == 0 || (safetySelection & MASK1(32, 5, 26)) != 0)
    {
      modeInSafetySelection = SafetyOperationMode::SOM_SAFE_LIMITED_SPEED;
    }
    // bit2~3   indicates STO/SOS (0:Selected/1:Not selected)
    // bit14~17 indicates SS1/SS2 (0:Selected/1:Not selected)
    if (((~safetySelection) & (MASK1(32, 2, 2) | MASK1(32, 4, 14))) != 0)
    {
      modeInSafetySelection = SafetyOperationMode::SOM_SAFE_HOLD;
    }

    SafetyOperationMode modeInSafetyStatus = SafetyOperationMode::SOM_NORMAL_MODE;
    // Check the cyclic data of the SMO: Safety function status.
    auto& safetyStatus = m_telegramAT->m_smoData.m_safetyStatus;
    // bit4 indicates Safely-limited speed (SLS) (0:Not active/1:Active).
    if ((safetyStatus & MASK1(32, 1, 4)) != 0)
    {
      modeInSafetyStatus = SafetyOperationMode::SOM_SAFE_LIMITED_SPEED;
    }
    // bit2~3 indicates STO/SOS (0:Not active/1:Active)
    // bit14~17 indicates SS1/SS2 (0:Not active/1:Active)
    if ((safetyStatus & (MASK1(32, 2, 2) | MASK1(32, 4, 14))) != 0)
    {
      modeInSafetyStatus = SafetyOperationMode::SOM_SAFE_HOLD;
    }

    // Safe operation mode is higher means lower velocity, velocity = 0 is the highest.
    mode = (modeInSafetyStatus >= modeInSafetySelection) ? modeInSafetyStatus : modeInSafetySelection;
  }

  return res;
}

AxisProfileResult AxisProfileCyclicDataDrive::getSafetyOperationMode(SafetyOperationMode& mode)
{
  AxisProfileResult res = AxisProfileResult::ERR_EXEC_NULL;
  if (m_stateMachineState && m_telegramAT && m_sharingHandler)
  {
    if (StateMachineState::RUNNING == *m_stateMachineState)
    {
      res = checkSafetyOperationMode(mode);
    }
    else
    {
      res = AxisProfileResult::ERR_WRONG_DEVICE_STATE;
    }
  }
  return res;
}
