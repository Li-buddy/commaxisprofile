/// @file
///
/// @brief This file is part of the axisprofile library and implements the CanOpen set operation mode job.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_setopmode.h"
#include "profiles/i_axisprofile_device_param_internal.h"
#include "profiles/value_mapping/i_mapping_handler.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

CanopenJobSetOpMode::CanopenJobSetOpMode(ProfileBasicInfo& profileInfo,
                                         IAxisProfileSharingHandler* shareHandle,
                                         const AcknowledgeTelegram* telegramAt,
                                         MasterDataTelegram* telegramMdt)
  : CanopenJobBase(profileInfo, shareHandle),
    m_telegramAT(telegramAt), m_telegramMDT(telegramMdt)
{
  m_jobFuns.reserve(2);
}

AxisProfileResult CanopenJobSetOpMode::onSetArgument(IContainer* arg)
{
  auto setOpModeArg = ::comm::axisprofile::ArgSetOperationMode::safeCast(arg);
  if (!setOpModeArg)
  {
    APR_TRACE_ERROR("%s %s gets wrong type of argument",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_JOB_DATA_MISMATCH;
  }

  if (!m_shareHandle)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_requestedOpMode = setOpModeArg->get();
  return determineCanopenOpMode(m_requestedOpMode, m_requestedCiaMode);
}

AxisProfileResult CanopenJobSetOpMode::onJobEnterActivating()
{
  if (!m_telegramAT || !m_telegramMDT || !m_shareHandle ||
      !m_shareHandle->getMappingHandlerIF() || !m_shareHandle->getParamAccessIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_readOpModeReq.resetCommandState();
  m_readOpModeReq.switchToCmdStartStep();
  m_writeOpModeReq.resetCommandState();
  m_writeOpModeReq.switchToCmdStartStep();

  const auto& assignedInfo = m_shareHandle->getMappingHandlerIF()->getProfileAssignedInfo();
  m_outputModeMapped = assignedInfo.m_output.m_opMode;

  m_jobFuns.clear();
  m_jobStep = 0;
  if (m_outputModeMapped)
  {
    m_jobFuns.emplace_back(
      [this]() { return writeModeToPdo(); });
  }
  else
  {
    m_jobFuns.emplace_back(
      [this]() { return writeModeBySdo(); });
  }
  m_jobFuns.emplace_back(
    [this]() { return verifyOperationMode(); });

  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  return CanopenJobBase::onJobEnterActivating();
}

AxisProfileResult CanopenJobSetOpMode::onJobActivating()
{
  const auto result = runJobsInternal();
  if (result == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
  }
  else if (aprStatusFailed(result))
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_ERROR);
  }
  return result;
}

AxisProfileResult CanopenJobSetOpMode::onJobEnterAborting()
{
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_INTERRUPTED);
  m_jobFuns.clear();
  m_jobStep = 0;
  m_readOpModeReq.resetCommandState();
  m_writeOpModeReq.resetCommandState();
  return CanopenJobBase::onJobEnterAborting();
}

bool CanopenJobSetOpMode::checkOpModeAvailable(AvailabilityInfo supported,
                                               bool assignedInput,
                                               bool assignedOutput,
                                               OpModeErrorInfo& errorInfo) const
{
  if (supported.available && assignedInput && assignedOutput)
  {
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_NONE;
    errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
    return true;
  }

  if (!supported.available)
  {
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_DRIVE_CONFIG;
    errorInfo.m_addInfo = supported.message ? supported.message : opModeSupportedErrorTypeToString(errorInfo.m_error);
  }
  else if (!assignedInput)
  {
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING;
    errorInfo.m_addInfo = "input " + std::string(opModeSupportedErrorTypeToString(errorInfo.m_error));
  }
  else
  {
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING;
    errorInfo.m_addInfo = "output " + std::string(opModeSupportedErrorTypeToString(errorInfo.m_error));
  }
  return false;
}

bool CanopenJobSetOpMode::checkOperationModeSupported(AxisProfileOperationMode opMode,
                                                      OpModeErrorInfo& errorInfo) const
{
  auto* deviceParam = m_shareHandle->getDeviceParamIF();
  const auto* mapping = m_shareHandle->getMappingHandlerIF();
  if (!deviceParam || !mapping || !deviceParam->getOpModeAvailInfo())
  {
    errorInfo.m_error = OpModeSupportedErrorType::ERROR_NOT_SUPPORT;
    errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
    return false;
  }

  const auto* available = deviceParam->getOpModeAvailInfo();
  const auto& assigned = mapping->getProfileAssignedInfo();

  switch (opMode)
  {
    case AxisProfileOperationMode::POSITIONING:
      return checkOpModeAvailable(available->positionAvail, assigned.m_input.m_position,
                                  assigned.m_output.m_position, errorInfo);
    case AxisProfileOperationMode::VELOCITY:
      return checkOpModeAvailable(available->velocityAvail, assigned.m_input.m_velocity,
                                  assigned.m_output.m_velocity, errorInfo);
    case AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT:
      return checkOpModeAvailable(available->torqueForceAvail,
                                  assigned.m_input.m_torqueForce && assigned.m_input.m_torqueForceLimSts,
                                  assigned.m_output.m_torqueForce && assigned.m_output.m_torqueVelLimHigh && assigned.m_output.m_torqueVelLimLow,
                                  errorInfo);
    case AxisProfileOperationMode::HOMING:
      return true;
    case AxisProfileOperationMode::UNDEFINED:
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_NOT_DEFINED;
      errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
      return false;
    default:
      errorInfo.m_error = OpModeSupportedErrorType::ERROR_NOT_SUPPORT;
      errorInfo.m_addInfo = opModeSupportedErrorTypeToString(errorInfo.m_error);
      return false;
  }
}

AxisProfileResult CanopenJobSetOpMode::determineCanopenOpMode(AxisProfileOperationMode opMode,
                                                              std::uint8_t& mode) const
{
  if (!convertToCanopenOpMode(opMode, mode))
  {
    return AxisProfileResult::ERR_NOT_SUPPORTED;
  }

  OpModeErrorInfo errorInfo {};
  if (!checkOperationModeSupported(opMode, errorInfo))
  {
    return AxisProfileResult::ERR_NOK;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobSetOpMode::writeModeToPdo()
{
  if (!m_telegramMDT)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // The PDO value is consumed by the cyclic fieldbus path.
  m_telegramMDT->m_mdtData.m_opMode = m_requestedCiaMode;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobSetOpMode::writeModeBySdo()
{
  return m_paramAccessIf->writeAndCheckParamAsync(
    getObjIndex(MODES_OF_OPERATION.index, m_profileInfo.m_driveNumber),
    MODES_OF_OPERATION.subIndex,
    &m_requestedCiaMode,
    sizeof(m_requestedCiaMode),
    m_writeOpModeReq.getRequestState());
}

AxisProfileResult CanopenJobSetOpMode::verifyOperationMode()
{
  const auto& assignedInfo = m_shareHandle->getMappingHandlerIF()->getProfileAssignedInfo();
  if (assignedInfo.m_input.m_opModeDisplay)
  {
    m_feedbackCiaMode = m_telegramAT->m_atData.m_opModeDisplay;
    return (m_feedbackCiaMode == m_requestedCiaMode) ? AxisProfileResult::STS_OK
                                                      : AxisProfileResult::STS_PENDING;
  }

  const auto result = m_paramAccessIf->readAndCheckParamAsync(
    getObjIndex(MODES_OF_OPERATION_DISPLAY.index, m_profileInfo.m_driveNumber),
    MODES_OF_OPERATION_DISPLAY.subIndex,
    &m_feedbackCiaMode,
    sizeof(m_feedbackCiaMode),
    m_readOpModeReq.getRequestState());

  if (result == AxisProfileResult::STS_OK)
  {
    if (m_feedbackCiaMode == m_requestedCiaMode)
    {
      return AxisProfileResult::STS_OK;
    }

    // The drive has not applied the requested mode yet. Start a fresh read
    // request on the next cycle without blocking the real-time task.
    m_readOpModeReq.resetCommandState();
    m_readOpModeReq.switchToCmdStartStep();
    return AxisProfileResult::STS_PENDING;
  }

  return result;
}
