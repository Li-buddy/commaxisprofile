/// @file
///
/// @brief This source file is part of the axisprofile library and declare the CanopenCommandDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_command_drive.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/canopen/canopen_param_access.h"
#include "diagnosis/dia.h"
#include "helper/device_id_helper.h"

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
constexpr std::int32_t STANDSTILL_WINDOW = 500;          ///< set standstill window as 0.5mm/s       

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
CanopenCommandDrive::CanopenCommandDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  : AxisProfileCommandDrive(profileBasicInfo), m_telegramAT(at), m_telegramMDT(mdt)
{
}

void CanopenCommandDrive::loadBootingTasks()
{
  m_initCommands.clear();
  m_initCommands.reserve(3);
  m_initCommands.emplace_back([this]{return resetAllAxisprofileCommands();});
  m_initCommands.emplace_back([this]{return initCanopenCommandDrive();});
  m_initCommands.emplace_back([this]{return addAuxCyclicJobs();});
}

AxisProfileResult CanopenCommandDrive::clearDeviceError()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::setAndCheckPositiveStop(const AxisProfilePositiveStop /* mode */)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_CHECK_POSITIVE_STOP));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::setAndCheckClampingTorque(const double /* clampingTorque */)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_CHECK_CLAMPING_TORQUE));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::getClampingTorque(double* const /* clampingTorque */)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_GET_CLAMPING_TORQUE));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::checkStandStill(bool* const /* standStill */)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_CHECK_STANDSTILL));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::setAndCheckStandStillWindow(const double /* standStillWindow */) // m/s or degree/s
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_CHECK_STANDSTILL_WINDOW));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::getStandStillWindow(double* const /* standStillWindow */) // m/s or degree/s
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_GET_STANDSTILL_WINDOW));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::activateCyclicStandStillCheck()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_ACTIVATE_STANDSTILL_CHECK));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::setAbsolutePosition(const double /* position */, const AxisProfileEncoder /* encoder */, const bool /* ifDriveEnabled */)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::resetAbsolutePosition()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_RESET_ABSOLUTE_POSITION));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::deactivateCyclicStandStillCheck()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_DEACTIVATE_STANDSTILL_CHECK));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::referencingAxis(const double /*newRefPos*/, const AxisProfileEncoder /* encoder */)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_REFERENCING_AXIS));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::resetReferencingAxis()
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::setReferencingPosition(const double /*refPos*/, const AxisProfileEncoder /*encoder*/, RefMode /*mode*/)
{
  APR_TRACE_ERROR("%s not implemented.", axisProfileCommandTypeToString(AxisProfileCommandType::CMD_SET_REFERENCING_POS));
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult CanopenCommandDrive::addAuxCyclicJobs()
{
  m_sharingHandler->getAuxJobHandlerIF()->clearAllJobs();
  auxJobs job;
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
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenCommandDrive::initCanopenCommandDrive()
{
  if (!m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  m_paramAccess = static_cast<CanopenParamAccess*>(m_sharingHandler->getParamAccessIF());
  m_axisIsHomed = false;
  auto* propertyIf = m_sharingHandler->getPropertyIF();

  auto deviceSeries = propertyIf->getDeviceSeries();
  if(!DeviceIdHelper::isRexrothDrive(deviceSeries))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_NO_HOMING_STATUS_FROM_DEVICE,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "DeviceSeries:%s", deviceSeriesToString(deviceSeries));
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenCommandDrive::checkIfDeviceHalt(bool& /*isHalted*/)
{
  return AxisProfileResult::STS_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ******************************************* PRIVATE METHODS ******************************************* //

// LCOV_EXCL_START These functions have been deprecated but have not yet been removed
AxisProfileResult CanopenCommandDrive::initReferencingAxis(const double newRefPos)
{
  if (!m_sharingHandler || !m_sharingHandler->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  std::int32_t homingPos = m_sharingHandler->getScalingIF()->convertPositionScaledToRaw(newRefPos);

  auto deviceParamIf = m_sharingHandler->getDeviceParamIF();
  const auto& deviceCfgData = deviceParamIf->getDeviceCfgData();
  if ((homingPos > deviceCfgData.m_positivePosLimit) ||
      (homingPos < deviceCfgData.m_negativePosLimit))
  {
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_DL_INVALID_VALUE,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "homing newRefPos %d is out of range [%d, %d].",
                       homingPos, deviceCfgData.m_negativePosLimit, deviceCfgData.m_positivePosLimit);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  m_cmdData.m_homingPos = homingPos;

  // homing should not execute when power off, PLCopen standard
  if (m_telegramAT->m_statusWord.m_bits.bit0 == 1 && m_telegramAT->m_statusWord.m_bits.bit2 == 0 && m_telegramAT->m_statusWord.m_bits.bit3 == 0)
  {
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_DL_INVALID_VALUE, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "homing is only allow when power on");
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  // adapt the homing behavior of Indra Drive
  auto bootingCfgItf = m_sharingHandler->getDeviceParamIF();
  auto* propertyIf = m_sharingHandler->getPropertyIF();

  if (!bootingCfgItf)
  {
    APR_TRACE_ERROR("The booting configuration interface of %s is invalid", m_profileBasicInfo.m_profileName);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  // set referencing procedure command read/write list which is only used for set position into device
  m_paramListReferencing.clear();

  if (getRefMode() == RefMode::SET_POS_DEVICE)
  {
    if (propertyIf->getDeviceSeries() == DeviceSeries::INDRADRIVE)
    { 
      m_paramListReferencing.emplace_back(ReadWriteType::WRITE, getObjIndex(REF_DISTANCE_INDRA_DRIVE.index, m_profileBasicInfo.m_driveNumber),
                                          REF_DISTANCE_INDRA_DRIVE.subIndex, &m_cmdData.m_homingPos, sizeof(m_cmdData.m_homingPos));
      
      // It is a little bit confusion here, because for Indra drive, the REF_DISTANCE is used for set offset to 0, 
      // but for other drive the REF_DISTANCE is used for set the homing position.
      // The reason is, the REF_DISTANCE for indra drive is used for set the homing offset, the homing position must use 
      // REF_DISTANCE_INDRADRIVE to set
      m_paramListReferencing.emplace_back(ReadWriteType::WRITE, getObjIndex(REF_DISTANCE.index, m_profileBasicInfo.m_driveNumber),
                                          REF_DISTANCE.subIndex, &m_cmdData.m_homingOffset, sizeof(m_cmdData.m_homingOffset));
    }
    else
    {
      m_paramListReferencing.emplace_back(ReadWriteType::WRITE, getObjIndex(REF_DISTANCE.index, m_profileBasicInfo.m_driveNumber),
                                          REF_DISTANCE.subIndex, &m_cmdData.m_homingPos, sizeof(m_cmdData.m_homingPos));
      if(DeviceIdHelper::isRexrothCtrlXDrive(propertyIf->getDeviceSeries()))
      {
        // May cause misunderstand see comments above.
        m_paramListReferencing.emplace_back(ReadWriteType::WRITE, getObjIndex(HOMING_OFFSET_CTRLX_DRIVE.index, m_profileBasicInfo.m_driveNumber),
                                            HOMING_OFFSET_CTRLX_DRIVE.subIndex, &m_cmdData.m_homingOffset, sizeof(m_cmdData.m_homingOffset));
      }
    }
  }
  
  // set dummy cycle for compensate scheduler system delay.
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenCommandDrive::executeReferencingAxis()
{
  AsyncCommandStatemachine* asyncCommandStatemachine = getAsyncStatemachine(AxisProfileCommandType::CMD_REFERENCING_AXIS);

  if ((asyncCommandStatemachine->getAxisProfileCommandState() != AxisprofileCommandState::CMD_STATE_ACTIVE) && 
      (asyncCommandStatemachine->getAxisProfileCommandState() != AxisprofileCommandState::CMD_STATE_ABORTING))
  {
    return AxisProfileResult::STS_OK;
  }

  auto result = establishReference(asyncCommandStatemachine);
  return resultHandlingCycle(AxisProfileCommandType::CMD_REFERENCING_AXIS, *asyncCommandStatemachine, result);
}

AxisProfileResult CanopenCommandDrive::executeClearDeviceError(AsyncCommandStatemachine& asyncState)
{
  AxisProfileResult result {AxisProfileResult::STS_PENDING};
  switch (asyncState.getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      asyncState.reinitCommandTick();
      m_telegramMDT->m_controlWord.m_bits.bit7 = 0;
      asyncState.switchToNextStep();
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      m_telegramMDT->m_controlWord.m_bits.bit7 = 1;
      asyncState.switchToNextStep();
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      if (!m_telegramAT->m_statusWord.m_bits.bit3)
      {
        m_telegramMDT->m_controlWord.m_bits.bit7 = 0;
        asyncState.switchToCertainStep(AsyncCommandStatemachine::CommandStep::PARAM_POST);
        result = AxisProfileResult::STS_OK;
      }
      break;
    }
    default:
    {
      APR_TRACE_ERROR("invalid state to execute Command.");
      result = AxisProfileResult::ERR_INVALID_STATE;
    }
  }
  return result;
}

AxisProfileResult CanopenCommandDrive::executeResetReferencingAxis()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenCommandDrive::initReferenceSet(const double newRefPos, RefMode refMode, AxisProfileCommandType cmdType)
{
  AsyncCommandStatemachine* asyncState {nullptr};
  AxisProfileResult result = raiseAndGetStatemachine(cmdType, asyncState);
  if (aprStatusFailed(result))
  {
    return result;
  }

  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    setMethod(refMode);  // reference mode for drive based homing is always SET_POS_DEVICE
    result = initReferencingAxis(newRefPos);
    if (AxisProfileResult::STS_OK == result)
    {
      if ((refMode == RefMode::SET_POS_DEVICE) && (cmdType == AxisProfileCommandType::CMD_SET_REFERENCING_POS))
      {
        m_paramListReferencing.emplace_back(ReadWriteType::WRITE, getObjIndex(HOMING_METHOD.index, m_profileBasicInfo.m_driveNumber),
                                            HOMING_METHOD.subIndex, &m_cmdData.m_homingMethod, sizeof(m_cmdData.m_homingMethod));
      }
      m_cmdData.m_opMode = CANOPEN_HOMING_MODE;
      m_paramListReferencing.emplace_back(ReadWriteType::WRITE, getObjIndex(MODES_OF_OPERATION.index, m_profileBasicInfo.m_driveNumber),
                                          MODES_OF_OPERATION.subIndex, &m_cmdData.m_opMode, sizeof(m_cmdData.m_opMode));
      asyncState->setCommandTimeout(0); // disable timeout check for homing
      asyncState->switchToNextStep();
      result = AxisProfileResult::STS_PENDING;
    }
  }

  return result;
}

AxisProfileResult CanopenCommandDrive::establishReference(AsyncCommandStatemachine* asyncCommandStatemachine)
{
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  bool isOpModeAssigned = profileAssignedInfo.m_output.m_opMode ? true : false;
  auto currentStep = asyncCommandStatemachine->getCommandStep();
  if (((currentStep > AsyncCommandStatemachine::CommandStep::PARAM_PROCESS) 
      && (currentStep < AsyncCommandStatemachine::CommandStep::PARAM_POST))
      && isOpModeAssigned)
  {
    m_telegramMDT->m_mdtData.m_opMode = CANOPEN_HOMING_MODE;
  }

  AxisProfileResult result = AxisProfileResult::STS_OK;

  switch (currentStep)
  {
    case AsyncCommandStatemachine::CommandStep::PARAM_PROCESS:      // quick stop axis and write asynchronous commands into device
    {
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit0 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit3 = 1;

      result = m_paramAccess->accessAndCheckParamListAsync(m_paramListReferencing, *asyncCommandStatemachine);
      if (AxisProfileResult::STS_PENDING != result)
      {
        if (aprStatusFailed(result))
        {
          APR_TRACE_WARNING("Homing offset parameter %s may not support, please check in drive.", objDirToString(REF_DISTANCE).c_str())
        }
        asyncCommandStatemachine->switchToCmdStartStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_START:          // switch back to operation enable
    {
      m_telegramMDT->m_controlWord.m_bits.bit8 = 0;
      if (m_sharingHandler->getStatusControlIF()->getOperationMode() == AxisProfileOperationMode::HOMING)
      {
        asyncCommandStatemachine->switchToNextStep();
      }
      else
      {
        m_sharingHandler->getStatusControlIF()->setOperationMode(AxisProfileOperationMode::HOMING);
      }
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:            // start the homing procedure
    {
      m_telegramMDT->m_controlWord.m_bits.bit4 = 1;
      if (m_telegramAT->m_statusWord.m_bits.bit12 == 0 || m_telegramAT->m_statusWord.m_bits.bit13 != 0)
      {
        asyncCommandStatemachine->switchToNextStep();
        resetHomeOffset();
      }
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:   // check if the command finish 
    {
      m_axisIsHomed = false;
      if ((m_telegramAT->m_statusWord.m_bits.bit10 == 1)
          && (m_telegramAT->m_statusWord.m_bits.bit12 == 1)
          && (m_telegramAT->m_statusWord.m_bits.bit13 == 0))
      {
        m_axisIsHomed = true;
        asyncCommandStatemachine->switchToNextStep();
      }
      else if (m_telegramAT->m_statusWord.m_bits.bit13 == 1)
      {
        APR_TRACE_ERROR("Homing error occurred during drive processing of %s.", m_profileBasicInfo.m_profileName.c_str());
        return AxisProfileResult::ERR_COMMAND_EXECUTION;
      }

      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:           // quick stop the axis
    {
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit0 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit3 = 1;
      asyncCommandStatemachine->switchToNextStep();
      result = AxisProfileResult::STS_PENDING;

      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_CLEAR:          // abort the command and then quick stop the axis
    {
      m_sharingHandler->getStatusControlIF()->setOperationMode(AxisProfileOperationMode::POSITIONING);
      if (m_sharingHandler->getStatusControlIF()->getOperationMode() == AxisProfileOperationMode::POSITIONING)
      {
        asyncCommandStatemachine->switchToNextStep();
      }
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::PARAM_POST:          // compensate the communication delay and finish the command
    {
      m_telegramMDT->m_controlWord.m_word = 0;
      m_telegramMDT->m_controlWord.m_bits.bit0 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit1 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit2 = 1;
      m_telegramMDT->m_controlWord.m_bits.bit3 = 1;
      result = AxisProfileResult::STS_OK;
      break;
    }
    default: // error reaction
    {
      APR_TRACE_ERROR("invalid state to execute executeReferencingAxis.");
      result = AxisProfileResult::ERR_INVALID_STATE;
    }
  }

  return result;
}
// LCOV_EXCL_STOP - exclude code coverage section end
