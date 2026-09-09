/// @file
///
/// This header file is part of the axisprofile library and implement SercosStatusControlDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "diagnosis/dia.h"
#include "sercos_status_control_drive.h"

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

SercosStatusControlDrive::SercosStatusControlDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  : AxisProfileStatusControlDrive(profileBasicInfo), m_telegramAT(at), m_telegramMDT(mdt)
{ 
  
}

void SercosStatusControlDrive::loadBootingTasks()
{
  m_initStatusControl.clear();
  m_initStatusControl.reserve(1);
  m_initStatusControl.emplace_back([this]{return updateOpModeSupportedInfo();});
}

/////////////////////////////////////////////////////////////////////////////////////
// ********************************* STATUSWORD ********************************** //

std::uint16_t SercosStatusControlDrive::getStatusWord() const
{
  return m_telegramAT->m_statusWord.m_word;
}

bool SercosStatusControlDrive::checkError() const
{
  return m_sercosStatusWord.m_driveShutdownErrorInC1D;
}

bool SercosStatusControlDrive::cyclicStandStillCheck() const
{
  return m_sercosStatusWord.m_realTimeStatusBit1; // true: standstill, false: not standstill
}

bool SercosStatusControlDrive::checkOperationModeSupported(const AxisProfileOperationMode opMode, OpModeErrorInfo& errorInfo)
{
  bool opModeSupported = false;

  switch (opMode)
  {
  case AxisProfileOperationMode::POSITIONING:
    opModeSupported = determineOpModeSupported(m_opModeAvailInfo->positionAvail.available, m_profileAssignedInfo.m_input.m_position, m_profileAssignedInfo.m_output.m_position, errorInfo);
    break;
  case AxisProfileOperationMode::VELOCITY:
    opModeSupported = determineOpModeSupported(m_opModeAvailInfo->velocityAvail.available, m_profileAssignedInfo.m_input.m_velocity, m_profileAssignedInfo.m_output.m_velocity, errorInfo);
    break;
  case AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT:
    opModeSupported = determineOpModeSupported(m_opModeAvailInfo->torqueForceAvail.available,
                                               m_profileAssignedInfo.m_input.m_torqueForce && m_profileAssignedInfo.m_input.m_torqueForceLimSts,
                                               m_profileAssignedInfo.m_output.m_torqueForce && m_profileAssignedInfo.m_output.m_torqueVelLimHigh && m_profileAssignedInfo.m_output.m_torqueVelLimLow,
                                               errorInfo);
    break;
  case AxisProfileOperationMode::UNDEFINED:
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_NOT_DEFINED;
    errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
    break;
  default:
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_NOT_SUPPORT;
    errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
    break;
  }
  return opModeSupported;
}

AxisProfileResult SercosStatusControlDrive::analyzeStatusWord()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  if (m_telegramAT)
  {
    if ( m_telegramAT->m_statusWord.m_word == 0 )
    {
      m_sercosStatusWord = {};
      //m_sercosStatusWord.m_driveShutdownErrorInC1D = true;
      return result;
    }

    result = determineReadyToOperateFromAT();
    if (aprStatusFailed(result))
    {
      return result;
    }

    m_sercosStatusWord.m_driveShutdownErrorInC1D = (m_telegramAT->m_statusWord.m_bits.bit13) & 1U;
    m_sercosStatusWord.m_warningInC2D = (m_telegramAT->m_statusWord.m_bits.bit12) & 1U;
    m_sercosStatusWord.m_changeBitForC3D = (m_telegramAT->m_statusWord.m_bits.bit11) & 1U;

    result = determineOpModeFromAT();
    if (aprStatusFailed(result))
    {
      return result;
    }

    m_sercosStatusWord.m_realTimeStatusBit2 = (m_telegramAT->m_statusWord.m_bits.bit7) & 1U;
    m_sercosStatusWord.m_realTimeStatusBit1 = (m_telegramAT->m_statusWord.m_bits.bit6) & 1U;
    m_sercosStatusWord.m_procedureCmdChangeBit = (m_telegramAT->m_statusWord.m_bits.bit5) & 1U;
    m_sercosStatusWord.m_statusParameterizationLvl = (m_telegramAT->m_statusWord.m_bits.bit4) & 1U;
    m_sercosStatusWord.m_statusCommandValueProcessing = (m_telegramAT->m_statusWord.m_bits.bit3) & 1U;
    m_sercosStatusWord.m_errorInServiceChannel = (m_telegramAT->m_statusWord.m_bits.bit2) & 1U;
    m_sercosStatusWord.m_busy = (m_telegramAT->m_statusWord.m_bits.bit1) & 1U;
    m_sercosStatusWord.m_serviceHandshake = (m_telegramAT->m_statusWord.m_bits.bit0) & 1U;
  }
  else
  {
    result = AxisProfileResult::ERR_EXEC_NULL;
  }

  return result;
}

bool SercosStatusControlDrive::isDeviceFollowsControl() const
{
  return m_sercosStatusWord.m_statusCommandValueProcessing;
}

ReadyToOperate SercosStatusControlDrive::checkReadyToOperate()
{
  std::uint16_t actualReadyToOperate = m_telegramAT->m_statusWord.m_bits.bit14 | (m_telegramAT->m_statusWord.m_bits.bit15 << 1);
  return static_cast<ReadyToOperate>(actualReadyToOperate);
}

/////////////////////////////////////////////////////////////////////////////////////
// ********************************* CONTROLWORD ********************************* //

std::uint16_t SercosStatusControlDrive::getControlWord() const
{
  return m_telegramMDT->m_controlWord.m_word;
}

AxisProfileResult SercosStatusControlDrive::setControlWord(std::uint16_t controlWord)
{
  // Set controlword but keep the state of the toggle bit
  m_telegramMDT->m_controlWord.m_word  = (m_telegramMDT->m_controlWord.m_word  & SERCOS_CTRLWORD_SYNC_BIT) | (controlWord & SERCOS_CTRLWORD_SYNC_BIT_INVERTED);

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosStatusControlDrive::toggleIpoSyncBit()
{
  m_telegramMDT->m_controlWord.m_word ^= 1UL << SERCOS_CTRLWORD_SYNC_BIT_OFFSET;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosStatusControlDrive::setOperationMode(const comm::axisprofile::AxisProfileOperationMode opMode)
{
  OperationMode sercosOpMode{};
  AxisProfileResult result = determineSercosOpMode(opMode, &sercosOpMode);

  if (aprStatusSucceeded(result))
  {
    std::uint16_t preparedOpMode = static_cast<std::uint16_t>(sercosOpMode) << SERCOS_CTRLWORD_OPMODE_OFFSET;

    // Some special conversion is necessary, because the operation mode bits are not placed
    // successively in the control word (Bits 11, 9, 8  --> Bit 10 is IpoSyncBit) which is
    // not considered in the OperationMode enum.
    if ((preparedOpMode >> SERCOS_CTRLWORD_SYNC_BIT_OFFSET) & 1U)   // Is Bit 10 true?
    {
      preparedOpMode &= ~(1UL << SERCOS_CTRLWORD_SYNC_BIT_OFFSET);      // Clear Bit 10
      preparedOpMode |= 1UL << (SERCOS_CTRLWORD_SYNC_BIT_OFFSET + 1);   // Set Bit 11
    }

    APR_TRACE_MESSAGE("%s; Set OperationMode '%s' in Controlword", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileOpModeToString(opMode))
    result = setControlWord((m_telegramMDT->m_controlWord.m_word  & SERCOS_CTRLWORD_OPMODE_INVERTED) | preparedOpMode);
  }
  return result;
}

AxisProfileOperationMode SercosStatusControlDrive::getOperationMode() const
{
  return m_checkedOperationMode;
}

AxisProfileResult SercosStatusControlDrive::readOperationMode(AxisProfileOperationMode& operationMode)
{
  if (!m_telegramAT)
  {
    operationMode = AxisProfileOperationMode::UNDEFINED;
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  const AxisProfileResult result = determineOpModeFromAT();
  operationMode = m_checkedOperationMode;
  return result;
}

AxisProfileResult SercosStatusControlDrive::setPowerMode(const AxisProfilePowerMode powerMode)
{
  AxisProfileResult result{};

  switch (powerMode)
  {
  case AxisProfilePowerMode::POWER_ON:
    if (m_sercosStatusWord.m_readyToOperate < ReadyToOperate::DRIVE_READY_AND_MAIN_POWER_APPLIED)
    {
      // Device not ready to turn power on
      APR_TRACE_ERROR("%s; StatusWord: 0x%08X", m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_WRONG_DEVICE_STATE, "PowerOn"), m_telegramAT->m_statusWord.m_word)
      result = AxisProfileResult::ERR_WRONG_DEVICE_STATE;
    }
    else
    {
      APR_TRACE_MESSAGE(m_profileBasicInfo.m_profileTrace.createTraceMsg("Set 'PowerOn' in Controlword"))
      result = setControlWord((m_telegramMDT->m_controlWord.m_word & SERCOS_CTRLWORD_POWERMASK_INVERTED) | SERCOS_CTRLWORD_POWER_AF);
    }
    break;
  case AxisProfilePowerMode::POWER_OFF:
    if ((m_sercosStatusWord.m_readyToOperate == ReadyToOperate::DRIVE_READY_AND_MAIN_POWER_APPLIED) && m_sercosStatusWord.m_statusCommandValueProcessing)
    {
      // ToDo: Additional checks necessary. Drive is following command values. Is it okay to turn power off?
    }
    APR_TRACE_MESSAGE(m_profileBasicInfo.m_profileTrace.createTraceMsg("Set 'PowerOff' in Controlword"))
    result = setControlWord((m_telegramMDT->m_controlWord.m_word  & SERCOS_CTRLWORD_POWERMASK_INVERTED) | SERCOS_CTRLWORD_POWER_AB);
    break;
  case AxisProfilePowerMode::POWER_OFF_TORQUE_FREE:
    if ((m_sercosStatusWord.m_readyToOperate == ReadyToOperate::DRIVE_READY_AND_MAIN_POWER_APPLIED) && m_sercosStatusWord.m_statusCommandValueProcessing)
    {
      // ToDo: Additional checks necessary. Drive is following command values. Is it okay to turn torque free?
    }
    APR_TRACE_MESSAGE(m_profileBasicInfo.m_profileTrace.createTraceMsg("Set 'PowerOff TorqueFree' in Controlword"))
    result = setControlWord((m_telegramMDT->m_controlWord.m_word & SERCOS_CTRLWORD_POWERMASK_INVERTED) |SERCOS_CTRLWORD_POWER_TORQUEFREE);
    break;
  default:
    result = AxisProfileResult::ERR_NOK;
    break;
  }
  return result;
}

/////////////////////////////////////////////////////////////////////////////////////
// ********************************** DRIVE HALT ********************************* //

AxisProfileResult SercosStatusControlDrive::setDriveHaltMode(const bool haltEnable)
{
  WordStructure tmpCtrlWord {m_telegramMDT->m_controlWord.m_word};
  tmpCtrlWord.m_bits.bit13 = haltEnable ? false : true; // 1->0 to trigger halt
  return setControlWord(tmpCtrlWord.m_word);
}

/////////////////////////////////////////////////////////////////////////////////////
// ******************************* PRIVATE METHODS ******************************* //

AxisProfileResult SercosStatusControlDrive::determineOpModeFromAT()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  std::uint16_t actualOpMode = m_telegramAT->m_statusWord.m_bits.bit8 | (m_telegramAT->m_statusWord.m_bits.bit9 << 1) | (m_telegramAT->m_statusWord.m_bits.bit10 << 2);
  m_sercosStatusWord.m_operationMode = static_cast<OperationMode>(actualOpMode);

  switch (m_sercosStatusWord.m_operationMode)
  {
    case OperationMode::PRIMARY:
    case OperationMode::SECONDARY1:
    case OperationMode::SECONDARY2:
    case OperationMode::SECONDARY3:
    case OperationMode::SECONDARY4:
    case OperationMode::SECONDARY5:
    case OperationMode::SECONDARY6:
    case OperationMode::SECONDARY7:
      m_checkedOperationMode = m_opModeAvailInfo->getProfileOpMode(m_sercosStatusWord.m_operationMode);
      break;
      // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen (3 Bit = 8 states)
    default:
      m_checkedOperationMode = AxisProfileOperationMode::UNDEFINED;
      result = AxisProfileResult::ERR_NOK;
      break;
      // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}

AxisProfileResult SercosStatusControlDrive::determineReadyToOperateFromAT()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  m_sercosStatusWord.m_readyToOperate = checkReadyToOperate();

  switch (m_sercosStatusWord.m_readyToOperate)
  {
  case ReadyToOperate::DRIVE_NOT_READY:
  case ReadyToOperate::DRIVE_READY_FOR_MAIN_POWER_ON:
  case ReadyToOperate::DRIVE_READY_AND_MAIN_POWER_APPLIED:
    m_actualPowerMode = AxisProfilePowerMode::POWER_OFF;
    break; 
  case ReadyToOperate::DRIVE_ENABLED:
    m_actualPowerMode = AxisProfilePowerMode::POWER_ON;
    break;
    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen (3 Bit = 8 states)
  default:
    m_actualPowerMode = AxisProfilePowerMode::UNDEFINED;
    result = AxisProfileResult::ERR_NOK;
    break;
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}


AxisProfileResult SercosStatusControlDrive::determineSercosOpMode(const AxisProfileOperationMode generalOpMode, OperationMode* sercosOpMode)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  OpModeErrorInfo opModeInfo{};
  if (!checkOperationModeSupported(generalOpMode, opModeInfo))
  {
    switch (opModeInfo.m_error)
    {
    case OpModeSupportedErrorType::ERROR_DRIVE_CONFIG:
      Dia::sendDiagnosis(MAIN_DIAG_DEVICE_ERR, DETAILED_DIAG_DEVICE_OP_MODE_NOT_CONFIG, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "%s mode %s", axisProfileOpModeToString(generalOpMode), opModeInfo.m_addInfo.c_str());
      break;
    case OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING:
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_PROFILE_NOT_ASSIGN, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "%s mode %s", axisProfileOpModeToString(generalOpMode), opModeInfo.m_addInfo.c_str());
      break;
    case OpModeSupportedErrorType::ERROR_NOT_SUPPORT:
      Dia::sendDiagnosis(MAIN_DIAG_DEVICE_ERR, DETAILED_DIAG_DEVICE_NOT_SUPPORT, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "%s mode %s", axisProfileOpModeToString(generalOpMode), opModeInfo.m_addInfo.c_str());
      break;
    default:
      break;
    }
    result = AxisProfileResult::ERR_NOK;
  }
  else
  {
    *sercosOpMode = m_opModeAvailInfo->getSercosOpMode(generalOpMode);
    result = ( OperationMode::UNDEFINED == *sercosOpMode ) ? AxisProfileResult::ERR_NOK : AxisProfileResult::STS_OK;
  }
  return result;
}

bool SercosStatusControlDrive::determineOpModeSupported(const bool opModeAvailable, const bool opModeInputAssigned, const bool opModeOutputAssigned, OpModeErrorInfo& errorInfo)
{
  bool opModeSupported{};
  if (opModeAvailable && opModeInputAssigned && opModeOutputAssigned)
  {
    opModeSupported = true;
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_NONE;
    errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
  }
  else
  {
    opModeSupported = false;
    if (!opModeAvailable)
    {
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_DRIVE_CONFIG;
      errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
    }
    else if (!opModeInputAssigned)
    {
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING;
      errorInfo.m_addInfo = "input " + std::string(opModeSupportedErrorTypeToString(errorInfo.m_error));
    }
    else if (!opModeOutputAssigned)
    {
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING;
      errorInfo.m_addInfo = "output " + std::string(opModeSupportedErrorTypeToString(errorInfo.m_error));
    }
  }
  return opModeSupported;
}

AxisProfileResult SercosStatusControlDrive::updateOpModeSupportedInfo()
{
  if (!m_sharingHandler)
  {
    APR_TRACE_ERROR("SharingHandler is nullptr, can not check if operation mode is supported.");
    return AxisProfileResult::ERR_NOK;
  }

  m_opModeAvailInfo = static_cast<OpModeAvailInfoSercos*>( m_sharingHandler->getDeviceParamIF()->getOpModeAvailInfo() );
  m_profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosStatusControlDrive::getDriveHaltMode(bool& isHaltEnabled) const
{
  if (!m_sharingHandler)
  {
    APR_TRACE_ERROR("The sharing handler is nullptr.");
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto result = m_sharingHandler->getCommandIF()->checkIfDeviceHalt(isHaltEnabled);
  if (aprStatusFailed(result))
  {
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  return result;
}
