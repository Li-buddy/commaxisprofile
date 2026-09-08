/// @file
///
/// @brief This source file is part of the axisprofile library and implement the SercosCommandDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "sercos_command_drive.h"
#include "axisprofile_internal_defs.h"
#include "profiles/sercos/sercos_param_access.h"
#include "diagnosis/dia.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------
const std::string SercosCommandDrive::TORQUE_TYPE_MISMATCH = "Torque scaling type in the drive does not match with the read request torque scaling type";

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------
#define SERCOS_MAX_STANDSTILL_WINDOW     (double)50000.0                                // 50000 units/s okay?

#define SERCOS_ABS_POS_CONTROL_HELPER    (std::uint16_t)0xFFFC                          // binary 1111 1111 1111 1100
#define SERCOS_ABS_POS_CONTROL_AF_TOKEN  (std::uint16_t)0x0004                          // binary 0000 0000 0000 0100
#define SERCOS_ABS_POS_CONTROL_ENCODER1_TOKEN (std::uint16_t)0x0001                     // binary 0000 0000 0000 0001
#define SERCOS_ABS_POS_CONTROL_ENCODER2_TOKEN (std::uint16_t)0x0002                     // binary 0000 0000 0000 0010
#define SERCOS_ENCODER_ABSOLUTE_CONFIGURATION_EVALUATION_TOKEN (std::uint16_t)0x100     // binary 0000 0001 0000 0000  for relative measuring system, bit 8 must be 1
#define SERCOS_ENCODER_ABSOLUTE          (std::uint16_t)0x40                            // binary 0000 0000 0100 0000  for absolute measuring system, bit 7 must 0
#define SERCOS_ENCODER_ABSOLUTE_TOKEN    (std::uint16_t)0xC0                            // binary 0000 0000 1100 0000
#define SERCOS_ENCODER_ONE_HOMED_TOKEN   (std::uint16_t)0x0002                          // binary 0000 0000 0000 0010
#define SERCOS_ENCODER_TWO_HOMED_TOKEN   (std::uint16_t)0x0004                          // binary 0000 0000 0000 0100


// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

SercosCommandDrive::SercosCommandDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at)
  :AxisProfileCommandDrive(profileBasicInfo), m_telegramAT(at)
{
  
}

void SercosCommandDrive::loadBootingTasks()
{
  m_initCommands.clear();
  m_initCommands.reserve(3);
  m_initCommands.emplace_back([this]{return resetAllAxisprofileCommands();});
  m_initCommands.emplace_back([this]{return initSercosCommandDrive();});
  m_initCommands.emplace_back([this]{return addAuxCyclicJobs();});
}

AxisProfileResult SercosCommandDrive::clearDeviceError()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::setAndCheckPositiveStop(const AxisProfilePositiveStop /*mode*/)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_CHECK_POSITIVE_STOP));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::setAndCheckClampingTorque(const double /*clampingTorque*/)  // Nm or N or %
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_CHECK_CLAMPING_TORQUE));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::getClampingTorque(double* const /*clampingTorque*/)  // Nm or N or %
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_GET_CLAMPING_TORQUE));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::checkStandStill(bool* const /*standStill*/)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_CHECK_STANDSTILL));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::setAndCheckStandStillWindow(const double /*standStillWindow*/) // m/s or degree/s
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_CHECK_STANDSTILL_WINDOW));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::getStandStillWindow(double* const /*standStillWindow*/) // m/s or degree/s
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_GET_STANDSTILL_WINDOW));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::activateCyclicStandStillCheck()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_ACTIVATE_STANDSTILL_CHECK));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::deactivateCyclicStandStillCheck()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_DEACTIVATE_STANDSTILL_CHECK));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::setAbsolutePosition(const double /*position*/, const AxisProfileEncoder /*encoder*/, const bool /*ifDriveEnabled*/)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::resetAbsolutePosition()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_RESET_ABSOLUTE_POSITION));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::referencingAxis(const double /*newRefPos*/, const AxisProfileEncoder /*encoder*/)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_REFERENCING_AXIS));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::resetReferencingAxis()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::setReferencingPosition(const double /*refPos*/, const AxisProfileEncoder /*encoder*/, RefMode /*mode*/)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_REFERENCING_POS));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult SercosCommandDrive::addAuxCyclicJobs()
{
  m_sharingHandler->getAuxJobHandlerIF()->clearAllJobs();
  auxJobs job;
  job.m_jobCyclic = [this] ()->AxisProfileResult
    {
      return this->executeSetAbsolutePosition();// for example: should switch to a member function of cmd object
    };
  m_sharingHandler->getAuxJobHandlerIF()->addAuxJob(job);

  job.m_jobCyclic = [this] ()->AxisProfileResult
    {
      return this->executeReferencingAxis();
    };
  m_sharingHandler->getAuxJobHandlerIF()->addAuxJob(job);

  job.m_jobCyclic = [this] ()->AxisProfileResult
    {
      return this->executeResetReferencingAxis();
    };
  m_sharingHandler->getAuxJobHandlerIF()->addAuxJob(job);

  job.m_jobCyclic = [this] ()->AxisProfileResult
    {
      return this->executeResetAbsolutePosition();
    };
  m_sharingHandler->getAuxJobHandlerIF()->addAuxJob(job);

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosCommandDrive::checkIfDeviceHalt(bool& isHalted)
{
  AsyncCommandStatemachine* asyncState {nullptr};
  AxisProfileResult result = raiseAndGetStatemachine(AxisProfileCommandType::CMD_CHECK_DEVICE_HALT, asyncState);
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_PROFILE_CMD_EXE_FAILED, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "could not get drive halt status");
    return result;
  }

  isHalted = false;

  std::uint16_t dataReceiver {0};
  result = m_paramAccess->readAndCheckParamDataAsync(P_0_0115_AXIS_STATUS_WORD, &dataReceiver, sizeof(dataReceiver), *asyncState);
  if ((AxisProfileResult::STS_OK == result) && ((dataReceiver >> 4) & 1))
  {
    isHalted = true;
  }

  return resultHandlingCycle(AxisProfileCommandType::CMD_CHECK_DEVICE_HALT, *asyncState, result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ******************************************* PRIVATE METHODS ******************************************* //

// LCOV_EXCL_START These functions have been deprecated but have not yet been removed
AxisProfileResult SercosCommandDrive::executeSetAbsolutePosition()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  AsyncCommandStatemachine* asyncCommandStatemachine = getAsyncStatemachine(AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION);
  // getCommandStateInQueue
  // lock existed in statemachine , can't handle in real-time thread
  if (asyncCommandStatemachine->getAxisProfileCommandState() != AxisprofileCommandState::CMD_STATE_ACTIVE)
  {
    return AxisProfileResult::STS_OK;
  }
  switch (asyncCommandStatemachine->getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS:
    {
      result = m_paramAccess->accessAndCheckParamListAsync(m_paramListSetAbsPos, *asyncCommandStatemachine);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncCommandStatemachine->switchToCmdStartStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    {
      // check only once when power on 
      result = m_paramAccess->executeSercosProcedureCommandAsync(ProcedureCommands::S_0_0447_SET_ABSOLUTE_POSITION, *asyncCommandStatemachine, m_cmdData.m_ifDriveEnabled ? 1 : 0);
      if (AxisProfileResult::STS_OK == result)
      {
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      // parameters backup
      m_paramListSetAbsPos.clear();
      m_paramListSetAbsPos.emplace_back(ReadWriteType::WRITE, S_0_0448_SET_ABSOLUTE_POSITION_CONTROL, static_cast<void*>(&m_cmdData.m_absPosControlBackup), sizeof(m_cmdData.m_absPosControlBackup));
      m_paramListSetAbsPos.emplace_back(ReadWriteType::WRITE, m_operatingEncoder.m_referenceDistance, static_cast<void*>(&m_cmdData.m_encoderRefBackup), sizeof(m_cmdData.m_encoderRefBackup));
      asyncCommandStatemachine->switchToCertainStep(AsyncCommandStatemachine::CommandStep::PARAM_POST);
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CLEAR:
    case AsyncCommandStatemachine::CommandStep::PARAM_POST:
    {
      result = m_paramAccess->accessAndCheckParamListAsync(m_paramListSetAbsPos, *asyncCommandStatemachine);
      break;
    }
    default:
    {
      APR_TRACE_ERROR("invalid state to execute executeSetAbsolutePosition.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
  }
  return resultHandlingCycle(AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION, *asyncCommandStatemachine, result);
}

AxisProfileResult SercosCommandDrive::executeReferencingAxis()
{
  AsyncCommandStatemachine* asyncCommandStatemachine = getAsyncStatemachine(AxisProfileCommandType::CMD_REFERENCING_AXIS);
  auto currentState = asyncCommandStatemachine->getAxisProfileCommandState();

  if ((currentState != AxisprofileCommandState::CMD_STATE_ACTIVE) && (currentState != AxisprofileCommandState::CMD_STATE_ABORTING))
  {
    return AxisProfileResult::STS_OK;
  }
  auto result = establishReference(asyncCommandStatemachine, [](){return AxisProfileResult::STS_OK;}, m_paramListReferencing);
  return resultHandlingCycle(AxisProfileCommandType::CMD_REFERENCING_AXIS, *asyncCommandStatemachine, result);
}

AxisProfileResult SercosCommandDrive::initSetAbsolutePosition(const double position, const AxisProfileEncoder encoder, const bool ifDriveEnabled)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  std::int32_t settingPosition = m_sharingHandler->getScalingIF()->convertPositionScaledToRaw(position);
  // get cfgdata from booting itf
  DeviceCfgData& deviceCfgData = m_sharingHandler->getDeviceParamIF()->getDeviceCfgData();

  if (deviceCfgData.m_softwareLimitActive)
  {
    if ((settingPosition > deviceCfgData.m_positivePosLimit) || (settingPosition < deviceCfgData.m_negativePosLimit))
    {
      Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_DL_INVALID_VALUE, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "set position out of drive limits");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
  }
  m_cmdData.m_ifDriveEnabled = ifDriveEnabled;
  m_cmdData.m_settingPosition = settingPosition;
  m_cmdData.m_absPosControl = 1;
  if (ifDriveEnabled)
  {
    m_cmdData.m_absPosControl |= SERCOS_ABS_POS_CONTROL_AF_TOKEN;
  }
  else
  {
    m_cmdData.m_absPosControl &= ~SERCOS_ABS_POS_CONTROL_AF_TOKEN;
  }
  m_cmdData.m_absPosControl &= SERCOS_ABS_POS_CONTROL_HELPER; // erase bit 0 and bit 1
  switch (encoder)
  {
    case AxisProfileEncoder::ENCODER_1:
    {
      if (((deviceCfgData.m_encoder1Property & SERCOS_ENCODER_ABSOLUTE_CONFIGURATION_EVALUATION_TOKEN)
           != SERCOS_ENCODER_ABSOLUTE_CONFIGURATION_EVALUATION_TOKEN) &&
          ((deviceCfgData.m_encoder1Property & SERCOS_ENCODER_ABSOLUTE_TOKEN) != SERCOS_ENCODER_ABSOLUTE))
      {
        m_operatingEncoder.m_referenceDistance = S_0_0000_DUMMY;
        Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_CONFIG_ERROR, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "drive not in absolute measuring system");
        return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
      }
      m_cmdData.m_absPosControl |= SERCOS_ABS_POS_CONTROL_ENCODER1_TOKEN;
      m_operatingEncoder.m_referenceDistance = S_0_0052_REFERENCE_DISTANCE_ENCODER_1;
      break;
    }
    case AxisProfileEncoder::ENCODER_2:
    {
      if (((deviceCfgData.m_encoder2Property & SERCOS_ENCODER_ABSOLUTE_CONFIGURATION_EVALUATION_TOKEN)
           != SERCOS_ENCODER_ABSOLUTE_CONFIGURATION_EVALUATION_TOKEN) &&
          ((deviceCfgData.m_encoder2Property & SERCOS_ENCODER_ABSOLUTE_TOKEN) != SERCOS_ENCODER_ABSOLUTE))
      {
        m_operatingEncoder.m_referenceDistance = S_0_0000_DUMMY;
        Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_CONFIG_ERROR, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "drive not in absolute measuring system");
        return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
      }
      m_cmdData.m_absPosControl |= SERCOS_ABS_POS_CONTROL_ENCODER2_TOKEN;
      m_operatingEncoder.m_referenceDistance = S_0_0054_REFERENCE_DISTANCE_ENCODER_2;
      break;
    }
    default:
    {
      Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_CONFIG_ERROR, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "undefined referencing encoder");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosCommandDrive::initReferencingScaling(const double newRefPos, const AxisProfileEncoder encoder, RefMode mode)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  std::int32_t homingPos = m_sharingHandler->getScalingIF()->convertPositionScaledToRaw(newRefPos);

  // get cfg data from booting itf
  DeviceCfgData const& deviceCfgData = m_sharingHandler->getDeviceParamIF()->getDeviceCfgData();

  // check and set necessary parameters for set position into device mode.
  if (mode == RefMode::SET_POS_DEVICE)
  {
    if (deviceCfgData.m_softwareLimitActive)
    {
      //TODO: get a position limit from device
      if ((homingPos > deviceCfgData.m_positivePosLimit) || (homingPos < deviceCfgData.m_negativePosLimit))
      {
        Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_DL_INVALID_VALUE, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "homing position is out of range");
        return AxisProfileResult::ERR_INVALID_ARGUMENT;
      }
    }

    if (AxisProfileEncoder::ENCODER_1 == encoder)
    {
      m_operatingEncoder.m_referenceDistance = S_0_0052_REFERENCE_DISTANCE_ENCODER_1;
      m_operatingEncoder.m_referenceOffset = S_0_0150_REFERENCE_OFFSET_ENCODER_1;
    }
    else if (AxisProfileEncoder::ENCODER_2 == encoder)
    {
      m_operatingEncoder.m_referenceDistance = S_0_0054_REFERENCE_DISTANCE_ENCODER_2;
      m_operatingEncoder.m_referenceOffset = S_0_0151_REFERENCE_OFFSET_ENCODER_2;
    }
    else
    {
      Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_DL_INVALID_VALUE, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "undefined referencing encoder");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
  }
  m_cmdData.m_homingPos = homingPos;
  APR_TRACE_MESSAGE("New reference position %d", m_cmdData.m_homingPos);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosCommandDrive::initSercosCommandDrive()
{
  if (!m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  
  m_paramAccess = static_cast<SercosParamAccess*>(m_sharingHandler->getParamAccessIF());
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosCommandDrive::executeResetReferencingAxis()
{
  AsyncCommandStatemachine* asyncCommandStatemachine = getAsyncStatemachine(AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS);
  if (asyncCommandStatemachine->getAxisProfileCommandState() != AxisprofileCommandState::CMD_STATE_ACTIVE)
  {
    return AxisProfileResult::STS_OK;
  }

  AxisProfileResult result = m_paramAccess->clearSercosProcedureCommandAsync(ProcedureCommands::S_0_0148_DRIVE_CONTROLLED_HOMING, *asyncCommandStatemachine);

  return resultHandlingCycle(AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS, *asyncCommandStatemachine, result);
}

AxisProfileResult SercosCommandDrive::executeSetReferencePosition(AsyncCommandStatemachine& asyncState)
{
  auto isHomeMethodFit = [this]()
  {
    if (((m_cmdData.m_homingConfig & SERCOS_EVALUATION_OF_HOME_SWITCH) != SERCOS_EVALUATION_OF_HOME_SWITCH)
        || ((m_cmdData.m_homingConfig & SERCOS_EVALUATION_OF_REF_MARK) != SERCOS_EVALUATION_OF_REF_MARK)
        || ((m_cmdData.m_homingConfig & SERCOS_EVALUATION_OF_RANGE) == SERCOS_EVALUATION_OF_RANGE)
        || ((m_cmdData.m_homingConfig & SERCOS_EVALUATION_OF_AXS_BLOCKING) == SERCOS_EVALUATION_OF_AXS_BLOCKING))
    {
      Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_PROFILE_CMD_EXE_FAILED, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "The reference method is incorrect on profile %s, please open drive engineering to check if reference method correct", m_profileBasicInfo.m_profileName.c_str());

      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    return AxisProfileResult::STS_OK;
  };
 
  return establishReference(&asyncState, isHomeMethodFit, m_paramListSetRefPos);
}

AxisProfileResult SercosCommandDrive::executeResetAbsolutePosition()
{
  AsyncCommandStatemachine* asyncCommandStatemachine = getAsyncStatemachine(AxisProfileCommandType::CMD_RESET_ABSOLUTE_POSITION);
  if (asyncCommandStatemachine->getAxisProfileCommandState() != AxisprofileCommandState::CMD_STATE_ACTIVE)
  {
    return AxisProfileResult::STS_OK;
  }

  AxisProfileResult result = m_paramAccess->clearSercosProcedureCommandAsync(ProcedureCommands::S_0_0447_SET_ABSOLUTE_POSITION, *asyncCommandStatemachine);

  return resultHandlingCycle(AxisProfileCommandType::CMD_RESET_ABSOLUTE_POSITION, *asyncCommandStatemachine, result);
}

AxisProfileResult SercosCommandDrive::establishReference(AsyncCommandStatemachine* asyncCommandStatemachine, 
                                                    std::function<AxisProfileResult()> conditionsCheck, 
                                                    std::list<SercosParamStruct>& taskList)
{
  auto cmdStep = asyncCommandStatemachine->getCommandStep();
  if (cmdStep == AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS)
  {
    resetHomeOffset();
  }

  auto result = AxisProfileResult::STS_OK;
  switch (cmdStep)
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS:
    {
      result = m_paramAccess->accessAndCheckParamListAsync(taskList, *asyncCommandStatemachine);
      if (AxisProfileResult::STS_OK == result)
      {
        result = conditionsCheck();
        if (aprStatusFailed(result))
        {
          return result;
        }

        asyncCommandStatemachine->switchToCmdStartStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      result = m_paramAccess->executeSercosProcedureCommandAsync(ProcedureCommands::S_0_0148_DRIVE_CONTROLLED_HOMING, *asyncCommandStatemachine);
     if (AxisProfileResult::STS_OK == result)
      {
        asyncCommandStatemachine->switchToCertainStep(AsyncCommandStatemachine::CommandStep::PARAM_POST);
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CLEAR:
    {
      asyncCommandStatemachine->reinitCommandTick();
      std::uint16_t cmd = SERCOS_COMMAND_NOT_SET;
      result = m_paramAccess->writeAndCheckParamAsync(static_cast<std::uint16_t>(ProcedureCommands::S_0_0148_DRIVE_CONTROLLED_HOMING),
                                                      &cmd, sizeof(cmd), *asyncCommandStatemachine);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncCommandStatemachine->switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::PARAM_POST:   //store the new reference position
    {
      result = AxisProfileResult::STS_OK;
      break;
    }

    default: // error reaction
    {
      APR_TRACE_ERROR("invalid state to execute establishReference.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
  }
  return result;
}
// LCOV_EXCL_STOP - exclude code coverage section end
