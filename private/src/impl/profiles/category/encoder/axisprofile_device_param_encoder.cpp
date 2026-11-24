/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for device parameter handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_device_param_encoder.h"
#include "bundle/axisprofile_device_param_c.h"
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

AxisProfileDeviceParamEncoder::AxisProfileDeviceParamEncoder(ProfileBasicInfo& profileBasicInfo,
                                                             const AcknowledgeTelegram* at)
  :AxisProfileDeviceParam(profileBasicInfo), m_telegramAT(at)
{
}

bool AxisProfileDeviceParamEncoder::checkBrakeDelaySupport() const
{
  rejectCmdDiagnosis("checkBrakeDelaySupport");
  return 0;
}

AxisProfileResult AxisProfileDeviceParamEncoder::checkStandstill(bool& /*standStill*/)
{
  rejectCmdDiagnosis("checkStandstill");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileDeviceParamEncoder::getErrorBrakeDelayInfo(const AxisProfileDeviceErrorType /*errorType*/, bool& /*isActivated*/, double& /*delayTime*/)
{
  rejectCmdDiagnosis("getErrorBrakeDelayInfo");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileDeviceParamEncoder::getErrorBrakeDynLimit(const AxisProfileDeviceErrorType /*errorType*/, double& /*deceleration*/, double& /*jerk*/)
{
  rejectCmdDiagnosis("getErrorBrakeDynLimit");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileDeviceParamEncoder::getPositionDataFormat(AxisProfilePosDataFormat& /*posDataFormat*/)
{
  rejectCmdDiagnosis("getPositionDataFormat");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileDeviceParamEncoder::checkAndSetModulo(double /*modValue*/, std::string& /*dynErrTxt*/)
{
  rejectCmdDiagnosis("checkAndSetModulo");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

DeviceCfgData& AxisProfileDeviceParamEncoder::getDeviceCfgData()
{
  rejectCmdDiagnosis("getDeviceCfgData");
  return m_cfgData;
}

bool AxisProfileDeviceParamEncoder::isLatchParamSupport() const
{
  rejectCmdDiagnosis("isLatchParamSupport");
  return 0;
}

AxisProfileResult AxisProfileDeviceParamEncoder::readSignalStatusWordCfg(std::vector<std::uint32_t>& /*params*/, std::vector<std::uint16_t>& /*assignBits*/)
{
  rejectCmdDiagnosis("readSignalStatusWordCfg");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

OpModeAvailInfo* AxisProfileDeviceParamEncoder::getOpModeAvailInfo()
{
  rejectCmdDiagnosis("getOpModeAvailInfo");
  return nullptr;
}

bool AxisProfileDeviceParamEncoder::checkFunctionSupported(const FunctionSupport function, FunctionSupportErrorInfo& errorInfo)
{
  bool result = false;
  switch (function)
  {
    case FunctionSupport::HOMING_STATUS_CYCLIC:
    {
      if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
      {
        return false;
      }
      const auto& assigned = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
      if (assigned.m_input.m_isInReference)
      {
        result = true;
      }
    }
    break;
    case FunctionSupport::HOMING_CMD_CYCLIC:
    {
      if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
      {
        return false;
      }
      const auto& assigned = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
      if (assigned.m_output.m_setReference && assigned.m_output.m_referenceOffset)
      {
        result = true;
      }
    }
    break;
    default:
    {
      result = false;
      errorInfo = {FuncSupportErrorType::ERROR_NOT_DEFINED, "invalid input parameter"};
    }
    break;
  }
  return result;
}

void AxisProfileDeviceParamEncoder::loadBootingTasks()
{
  m_initDeviceParam.clear();
  m_initDeviceParam.reserve(1);
  m_initDeviceParam.emplace_back([this]{return initHomeState();});
}

void AxisProfileDeviceParamEncoder::rejectCmdDiagnosis(const char* cmdNameInfo) const
{
  auto category = m_sharingHandler->getCategoryIF()->getProfileCategory();
  Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_FUNCTION_NOT_SUPPORTED, __FILE__, __FUNCTION__, __LINE__,
                     "Command %s not allowed for %s on profile %s.", cmdNameInfo, axisProfileCategoryTypeToString(category),
                     m_profileBasicInfo.m_profileName.c_str());
}

bool AxisProfileDeviceParamEncoder::checkNeedReadBootParams()
{
  return true;
}

AxisProfileResult AxisProfileDeviceParamEncoder::getDeviceInformation()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileDeviceParamEncoder::getChecksumIdnList(std::vector<std::string>& /*idnStrListRet*/) const
{
  rejectCmdDiagnosis("getChecksumIdnList");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileDeviceParamEncoder::checkAxisHomed(bool& homed)
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  const auto& assigned = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (assigned.m_input.m_isInReference)
  {
    m_homed.store(m_telegramAT->m_ioInReference.m_bits.bit0);
  }
  homed = m_homed.load();
  return AxisProfileResult::STS_OK;
}
