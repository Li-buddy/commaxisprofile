/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup for the axis profile diagnostic message interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_axisprofile_diag_msg.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of classes and structures
// ---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------- Statusword ----------------------------------------------- //
extern "C" AxisProfileDeviceErrorType AXISPROFILE_getErrorTypeMock(void* handle)
{
  const AxisProfileDiagMsgMockHandle* mock = static_cast<AxisProfileDiagMsgMockHandle*>(handle);
  return (mock) ? mock->m_errorType : AxisProfileDeviceErrorType::ERROR_NO_INFO;
}

extern "C" std::uint32_t AXISPROFILE_getErrorCodeMock(void* handle)
{
  const AxisProfileDiagMsgMockHandle* mock = static_cast<AxisProfileDiagMsgMockHandle*>(handle);
  return (mock) ? mock->m_errorCode : 0;
}
