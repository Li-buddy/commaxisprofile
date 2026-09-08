/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for command handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_command_encoder.h"
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

AxisProfileCommandEncoder::AxisProfileCommandEncoder(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  :AxisProfileCommand(profileBasicInfo), m_telegramAT(at), m_telegramMDT(mdt)
{

}

void AxisProfileCommandEncoder::loadBootingTasks()
{
  m_initCommands.clear();
  m_initCommands.reserve(1);
  m_initCommands.emplace_back([this]{return addAuxCyclicJobs();});
}

AxisProfileResult AxisProfileCommandEncoder::resetAllAxisprofileCommands()
{
  for (auto& it : m_asyncStates)
  {
    it.second->resetCommandState();
  }

  return AxisProfileResult::STS_OK; // should always success
}

AxisProfileResult AxisProfileCommandEncoder::resetAxisProfileCommandState(const AxisProfileCommandType /*type*/)
{
  rejectCmdDiagnosis("resetAxisProfileCommandState");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::getCommandsAtDeterminedState(const AxisprofileCommandState /*cmdState*/, std::vector<AxisProfileCommandType>& /*activeCmds*/)
{
  rejectCmdDiagnosis("getCommandsAtDeterminedState");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::abortAxisProfileCommand(const AxisProfileCommandType /*type*/)
{
  rejectCmdDiagnosis("abortAxisProfileCommand");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

// ---------------------------------- Commands ------------------------------------
AxisProfileResult AxisProfileCommandEncoder::clearDeviceError()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCommandEncoder::setAndCheckPositiveStop(const AxisProfilePositiveStop /*mode*/)
{
  rejectCmdDiagnosis("setAndCheckPositiveStop");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::setAndCheckClampingTorque(const double /*clampingTorque*/)
{
  rejectCmdDiagnosis("setAndCheckClampingTorque");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::getClampingTorque(double* const /*clampingTorque*/)
{
  rejectCmdDiagnosis("getClampingTorque");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::checkStandStill(bool* const /*standStill*/)
{
  rejectCmdDiagnosis("checkStandStill");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::setAndCheckStandStillWindow(const double /*standStillWindow*/)
{
  rejectCmdDiagnosis("setAndCheckStandStillWindow");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::getStandStillWindow(double* const /*standStillWindow*/)
{
  rejectCmdDiagnosis("getStandStillWindow");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::activateCyclicStandStillCheck()
{
  rejectCmdDiagnosis("activateCyclicStandStillCheck");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::deactivateCyclicStandStillCheck()
{
  rejectCmdDiagnosis("deactivateCyclicStandStillCheck");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::setAbsolutePosition(const double position, const AxisProfileEncoder encoder, const bool /*ifDriveEnabled*/)
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto mappingHandler = m_sharingHandler->getMappingHandlerIF();
  const auto& assignedInfo = mappingHandler->getProfileAssignedInfo();
  if (!assignedInfo.m_input.m_isInReference ||
      !assignedInfo.m_output.m_setReference ||
      !assignedInfo.m_output.m_referenceOffset)
  {
    APR_TRACE_MESSAGE("set absolute position : handle as control based.");
    // handle by control side
    auto drvParamIf = m_sharingHandler->getDeviceParamIF();
    drvParamIf->setAxisHomed(true);
    return AxisProfileResult::STS_OK;
  }

  AsyncCommandStatemachine* asyncState {nullptr};
  AxisProfileResult result = raiseAndGetStatemachine(AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION, asyncState);
  if (aprStatusFailed(result))
  {
    return result;
  }

  if (asyncState->getCommandStep() == AsyncCommandStatemachine::CommandStep::PARAM_INIT)
  {
    APR_TRACE_MESSAGE("set absolute position : handle as device based.");
    m_encoder = encoder;
    m_position = position;
    /// handle by IO device
    m_telegramMDT->m_ioEncoderData.m_referenceOffset = false;
    asyncState->switchToCertainStep(AsyncCommandStatemachine::CommandStep::CMD_START);
    result = AxisProfileResult::STS_PENDING;
  }
  return result;
}

AxisProfileResult AxisProfileCommandEncoder::executeSetAbsolutePosition()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  AsyncCommandStatemachine* asyncCommandStatemachine = getAsyncStatemachine(AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION);
  // getCommandStateInQueue
  // lock existed in statemachine , can't handle in real-time thread
  if (asyncCommandStatemachine->getAxisProfileCommandState() != AxisprofileCommandState::CMD_STATE_ACTIVE)
  {
    return AxisProfileResult::STS_OK;
  }

  auto drvParamIf = m_sharingHandler->getDeviceParamIF();
  switch (asyncCommandStatemachine->getCommandStep())
  {
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      m_telegramMDT->m_ioEncoderData.m_referenceOffset = static_cast<std::uint32_t>(m_position);
      m_telegramMDT->m_ioEncoderData.m_setReference.m_bits.bit0 = true;
      asyncCommandStatemachine->switchToCertainStep(AsyncCommandStatemachine::CommandStep::CMD_DONE);
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:    
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      if (m_telegramAT->m_ioInReference.m_bits.bit0)
      {
        m_telegramMDT->m_ioEncoderData.m_setReference.m_bits.bit0 = false;
        drvParamIf->setAxisHomed(true);
        result = AxisProfileResult::STS_OK;
      }
      break;
    }
    default:
    {
      APR_TRACE_ERROR("invalid state to execute executeSetAbsolutePosition.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      drvParamIf->setAxisHomed(false);
      break;
    }
  }
  return resultHandlingCycle(AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION, *asyncCommandStatemachine, result);
}

AxisProfileResult AxisProfileCommandEncoder::resetAbsolutePosition()
{
  rejectCmdDiagnosis("resetAbsolutePosition");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::referencingAxis(const double /*newRefPos*/, const AxisProfileEncoder /*encoder*/)
{
  rejectCmdDiagnosis("referencingAxis");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::resetReferencingAxis()
{
  rejectCmdDiagnosis("resetReferencingAxis");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::setReferencingPosition(const double /*refPos*/, const AxisProfileEncoder /*encoder*/, RefMode /*mode*/)
{
  rejectCmdDiagnosis("setReferencingPosition");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileCommandEncoder::addAuxCyclicJobs()
{
  if (!m_sharingHandler || !m_sharingHandler->getAuxJobHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  m_sharingHandler->getAuxJobHandlerIF()->clearAllJobs();

  auxJobs job;
  job.m_jobCyclic = [this] ()->AxisProfileResult
    {
      return this->executeSetAbsolutePosition();
    };
  m_sharingHandler->getAuxJobHandlerIF()->addAuxJob(job);

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCommandEncoder::checkIfDeviceHalt(bool& /*isHalted*/)
{
  rejectCmdDiagnosis("checkIfDeviceHalt");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

void AxisProfileCommandEncoder::rejectCmdDiagnosis(const char* cmdNameInfo) const
{
  auto category = m_sharingHandler->getCategoryIF()->getProfileCategory();
  Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_FUNCTION_NOT_SUPPORTED, __FILE__, __FUNCTION__, __LINE__,
                     "Command %s not allowed for %s on profile %s.", cmdNameInfo, axisProfileCategoryTypeToString(category),
                     m_profileBasicInfo.m_profileName.c_str());
}
