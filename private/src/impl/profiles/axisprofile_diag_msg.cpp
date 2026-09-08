/// @file
///
/// This source file is part of the axisprofile library and implements
/// the handler method for async access.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profile_trace.h"
#include "axisprofile_diag_msg.h"
#include "bundle/axisprofile_diag_msg_c.h"
#include "helper/device_id_helper.h"

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

AxisProfileDiagMsg::AxisProfileDiagMsg(ProfileTrace* const trace, AcknowledgeTelegram* const at)
  : m_profileTrace(trace),
  m_telegramAT(at)
{
  // *** Initialize C-Interface *** //
  m_cInterface.handle = this;
  m_cInterface.getErrorType = AXISPROFILE_getErrorType;
  m_cInterface.getErrorCode = AXISPROFILE_getErrorCode;
}

const IAxisProfileDiagMsgC* AxisProfileDiagMsg::getCInterface() const
{
  return &m_cInterface;
}

AxisProfileDeviceErrorType AxisProfileDiagMsg::getErrorType()
{
  if (!m_sharingHandler || !m_sharingHandler->getPropertyIF())
  {
    return AxisProfileDeviceErrorType::ERROR_NOT_DEFINED;
  }
  auto diagItf = m_sharingHandler->getPropertyIF();
  auto deviceSeries = diagItf->getDeviceSeries();
  // for device series which is 3rd party, return critical error type
  if (!DeviceIdHelper::isRexrothDrive(deviceSeries))
  {
    return AxisProfileDeviceErrorType::ERROR_CRITICAL;
  }
  return m_actualErrorType;
}

std::uint32_t AxisProfileDiagMsg::getErrorCode() 
{
  return m_actualErrorCode;
}

void AxisProfileDiagMsg::resetErrCode() 
{
  m_actualErrorCode = UINT32_MAX;
}

void AxisProfileDiagMsg::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

AxisProfileResult AxisProfileDiagMsg::analyzeDiagMsg()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  m_actualErrorCode = UINT32_MAX;
  if (m_telegramAT)
  {
    m_actualErrorCode = m_telegramAT->m_diagMsg.m_word;

    if (m_actualErrorCode == 0)
    {
      m_diagMsg = {};
      return result;
    }

    // The diagnostic message can be from parameter (0x1002:00 / S-0-0390 / --) or (0x603F / P-0-1901.0.12)
    // If actual error code is from 0x603F (P-0-1901.0.12, CiA402: Error Code), no need to do analyzation.
    if ((m_actualErrorCode & 0xFFFF0000) == 0)
    {
      m_actualErrorType = AxisProfileDeviceErrorType::ERROR_CRITICAL;
      return result;
    }

    m_diagMsg.m_msgStatus = (m_telegramAT->m_diagMsg.m_bits.bit20) & 1U;
    determineMsgClassFromAT();
    determineStatusCodeFromAT();
    result = detemineErrorType();
    if (aprStatusFailed(result))
    {
      return result;
    }
  }
  else
  {
    result = AxisProfileResult::ERR_EXEC_NULL;
  }

  return result;
}

AxisProfileResult AxisProfileDiagMsg::determineMsgClassFromAT()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  std::uint16_t actualMsgClass = (m_telegramAT->m_diagMsg.m_bits.bit19 << 3) |
                                 (m_telegramAT->m_diagMsg.m_bits.bit18 << 2) |
                                 (m_telegramAT->m_diagMsg.m_bits.bit17 << 1) |
                                 (m_telegramAT->m_diagMsg.m_bits.bit16);

  m_diagMsg.m_msgClass = static_cast<MsgClass>(actualMsgClass);

  return result;
}

AxisProfileResult AxisProfileDiagMsg::determineStatusCodeFromAT()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  m_diagMsg.m_statusCode = (m_telegramAT->m_diagMsg.m_word) & 0xFFFF;

  std::uint16_t actualErrorLevel = (m_telegramAT->m_diagMsg.m_bits.bit15 << 3) |
                                   (m_telegramAT->m_diagMsg.m_bits.bit14 << 2) |
                                   (m_telegramAT->m_diagMsg.m_bits.bit13 << 1) |
                                   (m_telegramAT->m_diagMsg.m_bits.bit12);

  m_secosErrorLevel = static_cast<ErrorLevel>(actualErrorLevel);

  return result;
}

AxisProfileResult AxisProfileDiagMsg::detemineErrorType()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  if (m_diagMsg.m_msgClass != MsgClass::ERR_MSG)
  {
    m_actualErrorType = AxisProfileDeviceErrorType::ERROR_NONE;
  }
  else
  {
    switch (m_secosErrorLevel)
    {
    case ErrorLevel::LEVEL_2:
    case ErrorLevel::LEVEL_3:
      m_actualErrorType = AxisProfileDeviceErrorType::ERROR_LIGHT;
      break;

    case ErrorLevel::LEVEL_4:
      m_actualErrorType = AxisProfileDeviceErrorType::ERROR_FIELDBUS;
      break;

    case ErrorLevel::LEVEL_6:
    case ErrorLevel::LEVEL_7:
      m_actualErrorType = AxisProfileDeviceErrorType::ERROR_CRITICAL;
      break;

    case ErrorLevel::LEVEL_8:
      m_actualErrorType = AxisProfileDeviceErrorType::ERROR_FATAL;
      break;

    case ErrorLevel::NO_LEVEL:
    default:
      m_actualErrorType = AxisProfileDeviceErrorType::ERROR_NOT_DEFINED;
      break;
    }
  }
  return result;
}
