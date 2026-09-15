/// @file
///
/// @brief This source file is part of the axisprofile library and implement the AxisProfileCyclicDataEncoder.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_cyclic_data_encoder.h"
#include "axisprofile_internal_defs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "common/fpu_helper.h"
#include "diagnosis/dia.h"

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

AxisProfileCyclicDataEncoder::AxisProfileCyclicDataEncoder(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  : AxisProfileCyclicData(profileBasicInfo), m_telegramAT(at), m_telegramMDT(mdt)
{
}

void AxisProfileCyclicDataEncoder::loadBootingTasks()
{
  m_initCommands.clear();
  m_initCommands.reserve(1);
  m_initCommands.emplace_back([this]{return initAllSharedData();});
}

AxisProfileResult AxisProfileCyclicDataEncoder::initAllSharedData()
{
  if (!m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  m_scaling = m_sharingHandler->getScalingIF();
  return m_scaling ? AxisProfileResult::STS_OK : AxisProfileResult::ERR_EXEC_NULL;
}

AxisProfileResult AxisProfileCyclicDataEncoder::getActualValuesDevice(ActualValuesDevice* const actualValues)
{
  checkForFPUError();

  if (!m_stateMachineState)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  actualValues->m_actualPosition = m_telegramAT->m_atData.m_actualPosition;
  actualValues->m_actualVelocity = m_scaling->convertVelocityRawToScaled(m_telegramAT->m_atData.m_actualVelocity);
  actualValues->m_dataValid = (StateMachineState::RUNNING == (*m_stateMachineState)) ? true : false;
  
  // only check FPU when there is no FPU error
  checkForFPUError();

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCyclicDataEncoder::setCommandValuesDevice(CommandValuesDevice /*commandValues*/)
{
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCyclicDataEncoder::getSingleUserPDODataFromDevice(UserPDOID /*id*/, UserPDODataInfo& /*actualPDOValues*/)
{
  rejectCmdDiagnosis("getSingleUserPDODataFromDevice");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCyclicDataEncoder::setSingleUserPDODataToDevice(UserPDOID /*id*/, UserPDODataInfo /*commandValues*/)
{
  rejectCmdDiagnosis("setSingleUserPDODataToDevice");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

bool AxisProfileCyclicDataEncoder::checkSingleUserPDOActiveFromDevice(UserPDOID /*id*/)
{
  rejectCmdDiagnosis("checkSingleUserPDOActiveFromDevice");
  return 0;
}

bool AxisProfileCyclicDataEncoder::checkSingleUserPDOActiveToDevice(UserPDOID /*id*/)
{
  rejectCmdDiagnosis("checkSingleUserPDOActiveToDevice");
  return 0;
}

AxisProfileResult AxisProfileCyclicDataEncoder::getDigitalInputValueBit(DigitalInputValueBitType /*valueType*/, bool& /*inputValue*/)
{
  rejectCmdDiagnosis("getDigitalInputValueBit");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCyclicDataEncoder::getSafetyOperationMode(SafetyOperationMode& /*mode*/)
{
  rejectCmdDiagnosis("getSafetyOperationMode");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

void AxisProfileCyclicDataEncoder::rejectCmdDiagnosis(const char* cmdNameInfo) const
{
  auto category = m_sharingHandler->getCategoryIF()->getProfileCategory();
  Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_FUNCTION_NOT_SUPPORTED, __FILE__, __FUNCTION__, __LINE__,
                     "Command %s not allowed for %s on profile %s.", cmdNameInfo, axisProfileCategoryTypeToString(category),
                     m_profileBasicInfo.m_profileName.c_str());
}

AxisProfileResult AxisProfileCyclicDataEncoder::setNewRefPosition(std::int32_t newRefPos)
{
  m_telegramMDT->m_ioEncoderData.m_referenceOffset = newRefPos;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCyclicDataEncoder::activeNewRefOperation(bool active)
{
  m_telegramMDT->m_ioEncoderData.m_setReference.m_bits.bit0 = active;
  return AxisProfileResult::STS_OK;
}

