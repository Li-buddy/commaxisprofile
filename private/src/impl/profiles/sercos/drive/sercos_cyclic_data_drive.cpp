/// @file
///
/// @brief This source file is part of the axisprofile library and implement the SercosCyclicDataDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "common/axisprofile_fpu_helper.h"
#include "common/fpu_helper.h"
#include "diagnosis/dia.h"
#include "sercos_cyclic_data_drive.h"

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

SercosCyclicDataDrive::SercosCyclicDataDrive(const ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  : AxisProfileCyclicDataDrive(profileBasicInfo,at,mdt)
{
}

void SercosCyclicDataDrive::loadBootingTasks()
{
  m_initCommands.clear();
  m_initCommands.reserve(2);
  m_initCommands.emplace_back([this]{return initAllSharedData();});
  m_initCommands.emplace_back([this]{return initTorqueLimitFunction();});
}

AxisProfileResult SercosCyclicDataDrive::initAllSharedData()
{
  if (!m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_scaling = m_sharingHandler->getScalingIF();
  m_command = m_sharingHandler->getCommandIF();
  m_property = m_sharingHandler->getPropertyIF();

  if (!m_scaling || !m_command || !m_property || !m_stateMachineState)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosCyclicDataDrive::getActualValuesDevice(ActualValuesDevice* const actualValues)
{
  APR_CHECK_FPU_ERR_PROFILE(MAIN_DIAG_SYSTEM_ERR, DETAILED_DIAG_FPU_ERR_BEFORE_AXISPROFILE,
                        m_profileBasicInfo.m_profileName.c_str(), "scaling of INPUT data");

  actualValues->m_actualPosition = m_scaling->convertPositionRawToScaled(m_telegramAT->m_atData.m_actualPosition) + m_scaling->getHomeOffset();
  actualValues->m_actualVelocity = m_scaling->convertVelocityRawToScaled(m_telegramAT->m_atData.m_actualVelocity);
  actualValues->m_actualTorqueForce = m_scaling->convertTorqueRawToScaled(m_telegramAT->m_atData.m_actualTorqueInfo.m_actualTorqueForce);
  actualValues->m_torqueLimitStatus = m_torqueStatusFn(m_telegramAT->m_atData.m_actualTorqueInfo.m_torqueLimitStatus.m_bits);
  actualValues->m_dataValid = (StateMachineState::RUNNING == (*m_stateMachineState)
                               && (0 != m_telegramAT->m_statusWord.m_word)) ? true : false;
  
  // only check FPU when there is no FPU error
  APR_CHECK_FPU_ERR_PROFILE(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_FPU_SCALING_VALUE_ERROR,
                        m_profileBasicInfo.m_profileName.c_str(), "scaling of INPUT data");

  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosCyclicDataDrive::setCommandValuesDevice(CommandValuesDevice commandValues)
{
  APR_CHECK_FPU_ERR_PROFILE(MAIN_DIAG_SYSTEM_ERR, DETAILED_DIAG_FPU_ERR_BEFORE_AXISPROFILE,
                        m_profileBasicInfo.m_profileName.c_str(), "scaling of OUTPUT data");

  // work around for control based homing
  if (AxisprofileCommandState::CMD_STATE_ACTIVE == m_command->getAxisProfileCommandState(AxisProfileCommandType::CMD_SET_REFERENCING_POS))
  {
    m_telegramMDT->m_mdtData.m_cmdPosition = m_telegramAT->m_atData.m_actualPosition;
    m_telegramMDT->m_mdtData.m_cmdVelocity = m_telegramAT->m_atData.m_actualVelocity;
    m_telegramMDT->m_mdtData.m_cmdTorqueForceInfo.m_cmdTorqueForce = m_telegramAT->m_atData.m_actualTorqueInfo.m_actualTorqueForce;
    return AxisProfileResult::STS_OK;
  }

  double convertedValue = commandValues.m_commandPosition;
  m_telegramMDT->m_mdtData.m_cmdPosition = m_scaling->convertPositionScaledToRaw(convertedValue - m_scaling->getHomeOffset());
  convertedValue = commandValues.m_commandVelocity;
  m_telegramMDT->m_mdtData.m_cmdVelocity = m_scaling->convertVelocityScaledToRaw(convertedValue);
  convertedValue = commandValues.m_commandTorqueForce;
  m_telegramMDT->m_mdtData.m_cmdTorqueForceInfo.m_cmdTorqueForce = m_scaling->convertTorqueScaledToRaw(convertedValue);
  convertedValue = commandValues.m_torqueVelLimitHigh;
  m_telegramMDT->m_mdtData.m_cmdTorqueForceInfo.m_velLimitHigh = m_scaling->convertVelocityScaledToRaw(convertedValue);
  convertedValue = commandValues.m_torqueVelLimitLow;
  m_telegramMDT->m_mdtData.m_cmdTorqueForceInfo.m_velLimitLow = m_scaling->convertVelocityScaledToRaw(convertedValue);

  // only check FPU when there is no FPU error
  APR_CHECK_FPU_ERR_PROFILE(MAIN_DIAG_SCALING_ERR, DETAILED_DIAG_FPU_SCALING_VALUE_ERROR,
                        m_profileBasicInfo.m_profileName.c_str(), "scaling of OUTPUT data");

  return AxisProfileResult::STS_OK;
}


