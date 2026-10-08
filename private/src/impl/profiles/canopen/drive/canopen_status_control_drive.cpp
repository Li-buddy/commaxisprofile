/// @file
///
/// @brief This file is part of the axisprofile library and implements the status- and controlword handling for CiA402.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/canopen/canopen_param_access.h"
#include "canopen_status_control_drive.h"
#include "diagnosis/dia.h"

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
// Class implementation
// ---------------------------------------------------------------------------

CanopenStatusControlDrive::CanopenStatusControlDrive(ProfileBasicInfo& profileBasicInfo,
                                                     const AcknowledgeTelegram* at,
                                                     MasterDataTelegram* mdt)
  :AxisProfileStatusControlDrive(profileBasicInfo),
  m_fieldbusAdapter(profileBasicInfo.m_fieldbusAdapter),
  m_telegramAT(at), m_telegramMDT(mdt)
{}

/////////////////////////////////////////////////////////////////////////////////////
// ********************************* STATUSWORD ********************************** //

void CanopenStatusControlDrive::loadBootingTasks()
{
}

std::uint16_t CanopenStatusControlDrive::getStatusWord() const
{
  return m_telegramAT->m_statusWord.m_word;
}

bool CanopenStatusControlDrive::checkError() const
{
  return m_canStatusWord.m_fault;
}

bool CanopenStatusControlDrive::cyclicStandStillCheck() const
{
  return false; // true: standstill, false: not standstill
}

bool CanopenStatusControlDrive::checkOpModeAvailable(AvailabilityInfo supported, bool assignedInput, bool assignedOutput, OpModeErrorInfo& errorInfo)
{
  bool opModeSupported;
  if (supported.available && assignedInput && assignedOutput)
  {
    opModeSupported = true;
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_NONE;
    errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
  }
  else
  {
    opModeSupported = false;
    if (!supported.available)
    {
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_DRIVE_CONFIG;
      errorInfo.m_addInfo = supported.message? supported.message : opModeSupportedErrorTypeToString(errorInfo.m_error);
    }
    else if (!assignedInput)
    {
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING;
      errorInfo.m_addInfo = "input " + std::string(opModeSupportedErrorTypeToString(errorInfo.m_error));
    }
    else if (!assignedOutput)
    {
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING;
      errorInfo.m_addInfo = "output " + std::string(opModeSupportedErrorTypeToString(errorInfo.m_error));
    }
  }
  return opModeSupported;
}

bool CanopenStatusControlDrive::checkOperationModeSupported(const AxisProfileOperationMode opMode, OpModeErrorInfo& errorInfo)
{
  const OpModeAvailInfo* opModeAvailInfo = m_sharingHandler->getDeviceParamIF()->getOpModeAvailInfo();
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  bool opModeSupported {};
  switch (opMode)
  {
    case AxisProfileOperationMode::POSITIONING:
      opModeSupported = checkOpModeAvailable(opModeAvailInfo->positionAvail, profileAssignedInfo.m_input.m_position, profileAssignedInfo.m_output.m_position, errorInfo);
      break;

    case AxisProfileOperationMode::VELOCITY:
      opModeSupported = checkOpModeAvailable(opModeAvailInfo->velocityAvail, profileAssignedInfo.m_input.m_velocity, profileAssignedInfo.m_output.m_velocity, errorInfo);
      break;

    case AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT:
      opModeSupported = checkOpModeAvailable(opModeAvailInfo->torqueForceAvail, profileAssignedInfo.m_input.m_torqueForce && profileAssignedInfo.m_input.m_torqueForceLimSts,
                                             profileAssignedInfo.m_output.m_torqueForce && profileAssignedInfo.m_output.m_torqueVelLimHigh && profileAssignedInfo.m_output.m_torqueVelLimLow, errorInfo);
      break;

    case AxisProfileOperationMode::HOMING:
      opModeSupported = true; // Homing should always support since it is mandatory of CiA402
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

AxisProfileOperationMode CanopenStatusControlDrive::getOperationMode() const
{
  AxisProfileOperationMode result;
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (profileAssignedInfo.m_input.m_opModeDisplay) 
  {
    result = convertFromCanopenOpMode(m_telegramAT->m_atData.m_opModeDisplay);
  }
  else
  { 
    result = m_setAndCheckOpModeState.checkedOpMode;
  }
  return result;
}

void CanopenStatusControlDrive::confirmOperationModeFromDevice(AxisProfileOperationMode operationMode)
{
  if (operationMode != AxisProfileOperationMode::UNDEFINED)
  {
    m_setAndCheckOpModeState.checkedOpMode = operationMode;
  }
}

AxisProfileResult CanopenStatusControlDrive::checkOperationMode()
{
  AxisProfileResult result {AxisProfileResult::STS_OK};
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (profileAssignedInfo.m_input.m_opModeDisplay)
  {
    // direct from cyclic data, no need to asynchronous request the operation mode from drive.
    m_setAndCheckOpModeState.tmpOpMode = m_telegramAT->m_atData.m_opModeDisplay;
    m_setAndCheckOpModeState.checkedOpMode = convertFromCanopenOpMode(m_setAndCheckOpModeState.tmpOpMode);
  }
  else
  {
    m_setAndCheckOpModeState.checkedOpMode = AxisProfileOperationMode::OPMODE_PENDING;
    // no cyclic data, need to asynchronous request the operation mode from drive.
    std::uint16_t objIndex = getObjIndex(MODES_OF_OPERATION_DISPLAY.index, m_profileBasicInfo.m_driveNumber);
    std::uint8_t objSubIdx = 0x0;
    auto handler = static_cast<CanopenParamAccess*>(m_sharingHandler->getParamAccessIF());
    result = handler->readAndCheckParamAsync(objIndex, objSubIdx, &m_setAndCheckOpModeState.tmpOpMode,
                                             sizeof(m_setAndCheckOpModeState.tmpOpMode), m_setAndCheckOpModeState.stateReadingOpModeReq.getRequestState());
    if (aprStatusFailed(result))
    {
      // LCOV_EXCL_START can't happen
      m_setAndCheckOpModeState.checkedOpMode = AxisProfileOperationMode::UNDEFINED;
      APR_TRACE_WARNING("%s; Cannot read the current operation mode, result=0x%08X.", m_profileBasicInfo.m_profileTrace.createTraceMsg(), result);
      // LCOV_EXCL_STOP
    }
    if (result == AxisProfileResult::STS_OK)
    {
      m_setAndCheckOpModeState.checkedOpMode = convertFromCanopenOpMode(m_setAndCheckOpModeState.tmpOpMode);
    }
  }
  return result;
}

AxisProfileResult CanopenStatusControlDrive::analyzeStatusWord()
{
  if (!m_telegramAT)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  m_canStatusWord.m_followingError = (m_telegramAT->m_statusWord.m_bits.bit13) & 1U;
  m_canStatusWord.m_setPointAck = (m_telegramAT->m_statusWord.m_bits.bit12) & 1U;   
  m_canStatusWord.m_internalLimitActive = (m_telegramAT->m_statusWord.m_bits.bit11) & 1U;
  m_canStatusWord.m_targetReached = (m_telegramAT->m_statusWord.m_bits.bit10) & 1U;
  m_canStatusWord.m_remote = (m_telegramAT->m_statusWord.m_bits.bit9) & 1U;
  m_canStatusWord.m_warning = (m_telegramAT->m_statusWord.m_bits.bit7) & 1U;
  m_canStatusWord.m_switchOnDisabled = (m_telegramAT->m_statusWord.m_bits.bit6) & 1U;
  m_canStatusWord.m_notQuickStop = (m_telegramAT->m_statusWord.m_bits.bit5) & 1U;
  m_canStatusWord.m_voltageEnabled = (m_telegramAT->m_statusWord.m_bits.bit4) & 1U;
  m_canStatusWord.m_fault = (m_telegramAT->m_statusWord.m_bits.bit3) & 1U;
  m_canStatusWord.m_operationEnabled = (m_telegramAT->m_statusWord.m_bits.bit2) & 1U;
  m_canStatusWord.m_switchedOn = (m_telegramAT->m_statusWord.m_bits.bit1) & 1U;
  m_canStatusWord.m_readyToSwichOn = (m_telegramAT->m_statusWord.m_bits.bit0) & 1U;

  // analysis status word totally follow the CiA402 standard
  m_fsaStateOld = m_fsaState;
  m_fsaState = getFsaStateFromStatusWord(m_telegramAT->m_statusWord.m_word);
  if (m_fsaStateOld != m_fsaState)
  {
    APR_TRACE_MESSAGE("%s, PSD state switch to %s from %s",
                      m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                      FasStateToString(m_fsaState), FasStateToString(m_fsaStateOld));
  }

  if (m_fsaState == FsaState::OPERATION_ENABLED || m_fsaState == FsaState::QUICK_STOP_ACTIVE)
  {
    m_actualPowerMode = AxisProfilePowerMode::POWER_ON;
  }
  else
  {
    m_actualPowerMode = AxisProfilePowerMode::POWER_OFF;
  }

  return AxisProfileResult::STS_OK;
}

ReadyToOperate CanopenStatusControlDrive::checkReadyToOperate()
{
  switch (m_fsaState)
  {
    case FsaState::OPERATION_ENABLED:
    case FsaState::QUICK_STOP_ACTIVE:
      return ReadyToOperate::DRIVE_ENABLED;

    case FsaState::SWITCHED_ON:
      return ReadyToOperate::DRIVE_READY_AND_MAIN_POWER_APPLIED;

    case FsaState::READY_TO_SWITCH_ON:
    case FsaState::SWITCH_ON_DISABLED:
      return ReadyToOperate::DRIVE_READY_FOR_MAIN_POWER_ON;

    case FsaState::NOT_READY_TO_SWITCH_ON:
    case FsaState::FAULT:
    case FsaState::FAULT_REACTION_ACTIVE:
      return ReadyToOperate::DRIVE_NOT_READY;

    // LCOV_EXCL_START, can't happen
    default:
      return ReadyToOperate::UNDEF_OPERATION;
    // LCOV_EXCL_STOP
  }
}

bool CanopenStatusControlDrive::isDeviceFollowsControl() const
{
  bool followState = false;
  // Whether the drive follows the command value is only valid under the following four modes:
  // Cyclic Position Mode / Cyclic Velocity Mode / Cyclic Torque mode / Profile Torque Mode
  // For more information, refer to: // https://docs.automation.boschrexroth.com/unit/1324353626/cia402-status-word-0x6041-00-p-0-1901-0-2/latest/en/?inTopicJump=0x6041%3A00&sel=AwegjCBMXiAsJSgZAXMANKC1oQUvFKmAMxA%3D
  switch (getOperationMode())
  {
    case AxisProfileOperationMode::POSITIONING:
    case AxisProfileOperationMode::VELOCITY:
    case AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT:
      followState = m_canStatusWord.m_setPointAck;
      break;
    default:
      break;
  }
  return followState;
}

/////////////////////////////////////////////////////////////////////////////////////
// ********************************* CONTROLWORD ********************************* //

std::uint16_t CanopenStatusControlDrive::getControlWord() const
{
  return m_telegramMDT->m_controlWord.m_word;
}

AxisProfileResult CanopenStatusControlDrive::setControlWord(std::uint16_t controlWord)
{
  // Set controlword
  m_telegramMDT->m_controlWord.m_word = controlWord;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenStatusControlDrive::setOperationMode(const AxisProfileOperationMode opMode)
{
  std::uint8_t ciaMode {};
  AxisProfileResult res = determineCanopenOpMode(opMode, ciaMode);
  if (aprStatusFailed(res))
  {
    return res;
  }
  switch (m_setAndCheckOpModeState.stepSetOpMode)
  {
    case SetOpModeStep::SET_MODE:
    {
      const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
      if (profileAssignedInfo.m_output.m_opMode)
      {
        // The data length of opmode might be: 2Byte / 1Byte. At current stage, the defined outputs data type requires std::uint8_t.
        m_telegramMDT->m_mdtData.m_opMode = ciaMode;
        if (profileAssignedInfo.m_input.m_opModeDisplay)
        {
          return AxisProfileResult::STS_OK;
        }
      }
      else
      {
        // Asynchronous request the operation mode from drive.
        std::uint16_t objIndex = getObjIndex(MODES_OF_OPERATION.index, m_profileBasicInfo.m_driveNumber);
        std::uint8_t objSubIdx = 0x0;
        std::uint8_t tmpMode = ciaMode;
        auto handler = static_cast<CanopenParamAccess*>(m_sharingHandler->getParamAccessIF());
        res = handler->writeAndCheckParamAsync(objIndex, objSubIdx, &tmpMode, sizeof(tmpMode), m_setAndCheckOpModeState.stateWritingOpModeReq.getRequestState());
        if (aprStatusFailed(res))
        {
          // LCOV_EXCL_START can not happen
          APR_TRACE_WARNING("%s; Setting the operation mode (%d:%s) failed, result=0x%08X.",
                            m_profileBasicInfo.m_profileTrace.createTraceMsg(), ciaMode, axisProfileOpModeToString(opMode), res);
          // LCOV_EXCL_STOP
        }
      }
      if (res == AxisProfileResult::STS_OK)
      {
        res = AxisProfileResult::STS_PENDING;
        m_setAndCheckOpModeState.stepSetOpMode = SetOpModeStep::CHECK_MODE;
      }
      break;
    }
    case SetOpModeStep::CHECK_MODE:
      res = checkOperationMode();
      if (res == AxisProfileResult::STS_OK)
      {
        APR_TRACE_MESSAGE("%s; The checked operation mode is (%s).", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileOpModeToString(getOperationMode()));
        m_setAndCheckOpModeState.stepSetOpMode = SetOpModeStep::SET_MODE;
      }
      break;
    default:
      // LCOV_EXCL_START can not happen
      m_setAndCheckOpModeState.stepSetOpMode = SetOpModeStep::SET_MODE;
      res = AxisProfileResult::ERR_NOK;
      break;
      // LCOV_EXCL_STOP
  }
  if (aprStatusFailed(res))
  {
    m_setAndCheckOpModeState.stepSetOpMode = SetOpModeStep::SET_MODE;
  }
  return res;
}

AxisProfileResult CanopenStatusControlDrive::setPowerMode(const AxisProfilePowerMode powerMode)
{
  AxisProfileResult result {AxisProfileResult::STS_PENDING};

  // every state switching should be completed within the timeout threshold
  if (m_fsaStateOld != m_fsaState)
  {
    // we reset the step counter when state changes
    m_switchTimeOut = CANOPEN_SWITCH_TIMEOUT_THRESHOLD;
  }
  else
  {
    if (m_switchTimeOut-- == 0)
    {
      m_switchTimeOut = CANOPEN_SWITCH_TIMEOUT_THRESHOLD;
      Dia::sendDiagnosis(MAIN_DIAG_DEVICE_ERR, DETAILED_DIAG_DEVICE_POWER_TIMEOUT,
                         m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "%s, Device state block in %s", axisprofilePowerModeToString(powerMode),
                         FasStateToString(m_fsaState));
      return AxisProfileResult::ERR_TIMEOUT;
    }
  }

  switch(powerMode)
  {
    case AxisProfilePowerMode::POWER_ON:
      result = powerOnProcedure();
      break;

    case AxisProfilePowerMode::POWER_OFF:
      result = powerOffProcedure();
      break;

    default:
      result = AxisProfileResult::ERR_NOK;
      break;
  }
  return result;
}

/////////////////////////////////////////////////////////////////////////////////////
// ********************************** DRIVE HALT ********************************* //

AxisProfileResult CanopenStatusControlDrive::setDriveHaltMode(const bool haltEnable)
{
  if (m_actualPowerMode != AxisProfilePowerMode::POWER_ON)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_PROFILE_CMD_EXE_FAILED, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "device disabled");
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  m_telegramMDT->m_controlWord.m_bits.bit2 = haltEnable ? false : true;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenStatusControlDrive::getDriveHaltMode(bool& isHaltEnabled) const
{
  // TODO: m_canStatusWord.m_targetReach has to be check also, however, most of the drives not support this.
  // Only ctrlX drive with FW version higher than 6.04 support gives target reached.
  // It would be nice add a config node to enable target reached checking by user.
  isHaltEnabled = !m_canStatusWord.m_notQuickStop;
  return AxisProfileResult::STS_OK;
}

/////////////////////////////////////////////////////////////////////////////////////
// ******************************* PRIVATE METHODS ******************************* //
AxisProfileResult CanopenStatusControlDrive::powerOnProcedure()
{
  AxisProfileResult result {AxisProfileResult::STS_PENDING};
  switch (m_fsaState)
  {
    case FsaState::SWITCH_ON_DISABLED:
      // transition 2
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      break;

    case FsaState::READY_TO_SWITCH_ON:
      // transition 3
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit0 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      break;

    case FsaState::SWITCHED_ON:
      // transition 4
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit0 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit3 = 1;
      break;

    case FsaState::QUICK_STOP_ACTIVE:
      // transition 16
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit0 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit3 = 1;
      break;

    case FsaState::OPERATION_ENABLED:
      m_sharingHandler->getScalingIF()->initModuloPosCmp(m_telegramAT->m_atData.m_actualPosition);
      result = AxisProfileResult::STS_OK;
      break;

    default:
      APR_TRACE_MESSAGE("%s, PSD is in %s state, can't be powered",
                        m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                        FasStateToString(m_fsaState));
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
  }
  return result;
}

AxisProfileResult CanopenStatusControlDrive::powerOffProcedure()
{
  AxisProfileResult result {AxisProfileResult::STS_PENDING};
  switch (m_fsaState)
  {
    // Note transition 8 may be not supported by some drives, so we skip it here
    // In future, there will be a config node to enable/disable transition 8 for user selection.
    case FsaState::OPERATION_ENABLED:
      // transition 5
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit0 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      break;

    case comm::axisprofile::canopen::FsaState::SWITCHED_ON:
      // transition 6
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      break;

    default:
      // other states are all powered off includes device error stop
      result = AxisProfileResult::STS_OK;
      break;
  }
  return result;
}

AxisProfileResult CanopenStatusControlDrive::determineCanopenOpMode(const AxisProfileOperationMode generalOpMode, std::uint8_t& mode)
{
  if (!convertToCanopenOpMode(generalOpMode, mode))
  {
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_FUNCTION_NOT_SUPPORTED,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Operation mode id %d not supported", static_cast<int32_t>(generalOpMode));
    return AxisProfileResult::ERR_NOT_SUPPORTED;
  }

  OpModeErrorInfo opModeInfo {};
  if (!checkOperationModeSupported(generalOpMode, opModeInfo))
  {
    switch (opModeInfo.m_error)
    {
      case OpModeSupportedErrorType::ERROR_DRIVE_CONFIG:
        Dia::sendDiagnosis(MAIN_DIAG_DEVICE_ERR, DETAILED_DIAG_DEVICE_OP_MODE_NOT_CONFIG,
                           m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                           "%s mode %s", axisProfileOpModeToString(generalOpMode), opModeInfo.m_addInfo.c_str());
        break;
      case OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING:
        Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_PROFILE_NOT_ASSIGN,
                           m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                           "%s mode %s", axisProfileOpModeToString(generalOpMode), opModeInfo.m_addInfo.c_str());
        break;
      default:
        break;
    }
    return AxisProfileResult::ERR_NOK;
  }
  return AxisProfileResult::STS_OK;
}
