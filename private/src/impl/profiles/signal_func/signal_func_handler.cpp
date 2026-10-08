
/// @file
///
/// @brief This file is part of the axisprofile library and implement the signal function handler class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "signal_func_handler.h"
#include "profiles/data_sharing/i_sharing_handler.h"
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
SignalFuncHandler::SignalFuncHandler(ProfileBasicInfo& profileBasicInfo)
  :m_profileBasicInfo(profileBasicInfo)
{

}

AxisProfileResult SignalFuncHandler::init(IAxisProfileSharingHandler* sharing)
{
  m_sharingHandler = sharing;
  m_cyclicData = m_sharingHandler->getCyclicDataIF();
  if (!m_sharingHandler->getCyclicDataIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  validateSignalControlWordCfg();
  validateSignalStatusWordCfg();
  return AxisProfileResult::STS_OK;
}

bool SignalFuncHandler::isSignalFuncSupported(SignalFunction& func, FunctionSupportErrorInfo& errorInfo)
{
  if (!isFuncInSignalStatusWord(func, errorInfo))
  {
    return false;
  }

  if (!isFuncInSignalControlWord(func, errorInfo))
  {
    return false;
  }
  return true;
}

bool SignalFuncHandler::isFuncInSignalStatusWord(SignalFunction& func, FunctionSupportErrorInfo& errorInfo)
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    errorInfo = {FuncSupportErrorType::ERROR_NOT_DEFINED, "invalid mapping handler"};
    return false;
  }

  // Checking signal status word mapping configuration
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (!profileAssignedInfo.m_input.m_signalStatusWord)
  {
    errorInfo = {FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, "signal status word not assigned"};
    return false;
  }

  // Checking Signal status word support information
  const auto& itFound = std::find_if(m_signalStatusWordFuncs.begin(),
                                     m_signalStatusWordFuncs.end(),
                                     [&func] (const auto& item)
                                     {
                                       return func.function == item.function;
                                     });
  if (itFound == m_signalStatusWordFuncs.end())
  {
    errorInfo.errorType = FuncSupportErrorType::ERROR_CHECK_POINT_NOT_IMPLEMENTED;
    errorInfo.addInfo = "not implemented";
    return false;
  }

  func.statusBitOffset = itFound->statusBitOffset;
  func.statusSupported = itFound->statusSupported;
  func.supported = itFound->statusSupported;
  if (itFound->supported == false)
  {
    errorInfo.errorType = FuncSupportErrorType::ERROR_DEVICE_NOT_SUPPORT;
    errorInfo.addInfo = "the required function parameter not configured in device signal status word";
    return false;
  }
  return true;
}

bool SignalFuncHandler::isFuncInSignalControlWord(SignalFunction& func, FunctionSupportErrorInfo& errorInfo)
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    errorInfo = {FuncSupportErrorType::ERROR_NOT_DEFINED, "invalid mapping handler"};
    return false;
  }

  // Checking signal status word mapping configuration
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (!profileAssignedInfo.m_output.m_signalControlWord)
  {
    errorInfo = {FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, "signal control word not assigned in axisprofile configuration"};
    return false;
  }

  // Checking Signal status word support information
  const auto& itFound = std::find_if(m_signalControlWordFuncs.begin(),
                                     m_signalControlWordFuncs.end(),
                                     [&func] (const auto& item)
                                     {
                                       return func.function == item.function;
                                     });
  if (itFound == m_signalControlWordFuncs.end())
  {
    errorInfo.errorType = FuncSupportErrorType::ERROR_CHECK_POINT_NOT_IMPLEMENTED;
    errorInfo.addInfo = "not implemented";
    return false;
  }

  func.controlBitOffset = itFound->controlBitOffset;
  func.controlSupported = itFound->controlSupported;
  func.supported = itFound->controlSupported;
  if (itFound->supported == false)
  {
    errorInfo.errorType = FuncSupportErrorType::ERROR_DEVICE_NOT_SUPPORT;
    errorInfo.addInfo = "the required function parameter not configured in device signal control word";
    return false;
  }
  return true;
}

void SignalFuncHandler::registerSignalStatusWordFuncs(const FunctionSupport& function, const SignalWordValidator& validator)
{
  m_signalStatusWordFuncs.emplace_back(function, validator);
}

void SignalFuncHandler::registerSignalControlWordFuncs(const FunctionSupport& function, const SignalWordValidator& validator)
{
  m_signalControlWordFuncs.emplace_back(function, validator);
}

AxisProfileResult SignalFuncHandler::validateSignalStatusWordCfg()
{
  std::vector<std::uint32_t> sourceParams {};
  std::vector<std::uint16_t> assignBits {};
  AxisProfileResult res = this->readDriveSignalStatusWordCfg(sourceParams, assignBits);
  if (aprStatusFailed(res) || sourceParams.size() != assignBits.size())
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "ErrorCode:0x%X, configured params: %d, bits: %d",
                       res, sourceParams.size(), assignBits.size());
  }
  else
  {
    auto sizeOfParams = sourceParams.size();
    for (auto& f : m_signalStatusWordFuncs)
    {
      for (std::size_t i = 0; i < sizeOfParams; ++i)
      {
        f.statusBitOffset = static_cast<std::uint8_t>(i);
        f.statusSupported = f.validator(sourceParams[i], assignBits[i]);
        f.supported = f.statusSupported;
        if (f.supported)
        {
          break;
        }
      }

      APR_TRACE_MESSAGE("%s; Function 0x%X %s at signal status word, offset %d checked",
                        m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                        f.function, (f.supported ? "supported" : "not supported"), f.statusBitOffset);
    }
  }

  return comm::axisprofile::AxisProfileResult::STS_OK;
}

AxisProfileResult SignalFuncHandler::validateSignalControlWordCfg()
{
  std::vector<std::uint32_t> sourceParams {};
  std::vector<std::uint16_t> assignBits {};
  AxisProfileResult res = this->readDriveSignalControlWordCfg(sourceParams, assignBits);
  if (aprStatusFailed(res) || sourceParams.size() != assignBits.size())
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "ErrorCode:0x%X, configured params: %d, bits: %d",
                       res, sourceParams.size(), assignBits.size());
  }
  else
  {
    auto sizeOfParams = sourceParams.size();
    for (auto& f : m_signalControlWordFuncs)
    {
      for (std::size_t i = 0; i < sizeOfParams; ++i)
      {
        f.controlBitOffset = static_cast<std::uint8_t>(i);
        f.controlSupported = f.validator(sourceParams[i], assignBits[i]);
        f.supported = f.controlSupported;
        if (f.supported)
        {
          break;
        }
      }

      APR_TRACE_MESSAGE("%s; Function 0x%X %s at signal control word, offset %d checked",
                        m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                        f.function, (f.supported ? "supported" : "not supported"), f.controlBitOffset);
    }
  }

  return comm::axisprofile::AxisProfileResult::STS_OK;
}

AxisProfileResult SignalFuncHandler::resetSignalFuncBit(SignalFunction& func)
{
  if (!func.controlSupported)
  {
    return AxisProfileResult::ERR_ERR_DATA_NOT_MAPPED;
  }
  return m_cyclicData->resetSignalFuncBit(func.controlBitOffset);
}

AxisProfileResult SignalFuncHandler::activeSignalFunc(SignalFunction& func)
{
  if (!func.controlSupported)
  {
    return AxisProfileResult::ERR_ERR_DATA_NOT_MAPPED;
  }
  return m_cyclicData->activeSignalFunc(func.controlBitOffset);
}

AxisProfileResult SignalFuncHandler::getSignalFuncState(SignalFunction& func, bool& val)
{
  if (!func.statusSupported)
  {
    return AxisProfileResult::ERR_ERR_DATA_NOT_MAPPED;
  }
  return m_cyclicData->getSignalFuncState(func.statusBitOffset, val);
}
