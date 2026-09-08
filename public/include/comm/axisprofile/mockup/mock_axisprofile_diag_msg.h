#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axis profile diagnostic message interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_diag_msg_c.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
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

  /// @brief This class is a simple helper that can be used as handle in this mockup.
  class AxisProfileDiagMsgMockHandle
  {
  public: // data

    AxisProfileDeviceErrorType m_errorType = AxisProfileDeviceErrorType::ERROR_NO_INFO;    ///< Current error type
    std::uint32_t m_errorCode = 0;  ///< Current error code
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // -------------------------------------- C-Interface Method-Mockups ---------------------------------------- //


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // --------------------------------------- Diagnostic message ----------------------------------------------- //


  /// @brief Mockup of method AXISPROFILE_getErrorTypeMock().
  /// @param[in]   handle   Interface handle (use class AxisProfileDiagMsgMockHandle)
  /// @return  Member variable 'm_actualErrorType' of the handle is returned.
  extern "C" AxisProfileDeviceErrorType AXISPROFILE_getErrorTypeMock(void* handle);

  /// @brief Mockup of method AXISPROFILE_getErrorCodeMock().
  /// @param[in]   handle   Interface handle (use class AxisProfileDiagMsgMockHandle)
  /// @return  Member variable 'm_actualErrorCode' of the handle is returned.
  extern "C" std::uint32_t AXISPROFILE_getErrorCodeMock(void* handle);
}
