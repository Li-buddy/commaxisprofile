#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements axisprofile fpu helper functions.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "diagnosis/dia.h"
#include "fpu_helper.h"

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
  /// @brief This macro checks for FPU errors and sends a diagnosis if an error is detected.
  /// @param[in]  mainCode    The main diagnosis code to be sent if an FPU error is detected.
  /// @param[in]  detailCode  The detailed diagnosis code to be sent if an FPU error is detected.
  /// @param[in]  txt         The error text to be included in the diagnosis (optional).
  #define APR_CHECK_FPU_ERR_GEN(mainCode, detailCode, txt)  checkFPUErrGeneral(mainCode, detailCode, __FILE__, __FUNCTION__, __LINE__, txt)

  /// @brief This macro checks for FPU errors and sends a diagnosis if an error is detected.
  /// @param[in]  mainCode    The main diagnosis code to be sent if an FPU error is detected.
  /// @param[in]  detailCode  The detailed diagnosis code to be sent if an FPU error is detected.
  /// @param[in]  profile     The name of the profile where the check is performed
  /// @param[in]  txt         The error text to be included in the diagnosis (optional).
  #define APR_CHECK_FPU_ERR_PROFILE(mainCode, detailCode, profile, txt) checkFPUErrProfile(mainCode, detailCode, __FILE__, __FUNCTION__, __LINE__, profile, txt)

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This function checks for FPU errors and sends a diagnosis if an error is detected.
  /// @param[in]  mainCode    The main diagnosis code to be sent if an FPU error is detected.
  /// @param[in]  detailCode  The detailed diagnosis code to be sent if an
  /// @param[in]  file        The name of the file where the check is performed.
  /// @param[in]  function    The name of the function where the check is performed.
  /// @param[in]  line        The line number where the check is performed.
  /// @param[in]  errTxt      The error text to be included in the diagnosis (optional).
  static inline void checkFPUErrGeneral(std::uint32_t mainCode, std::uint32_t detailCode,
                                        const char* file, const char* function, std::uint32_t line,
                                        const char* errTxt = nullptr)
  {
    if (!fpu::g_FPUError)
    {
      AxisProfileResult result = AxisProfileResult::STS_OK;
      result = fpu::checkForFPUError(AxisProfileResult::STS_OK, AxisProfileResult::ERR_FPU_DIV_BY_ZERO);
      if (aprStatusFailed(result))
      {
        Dia::sendDiagnosis(mainCode, detailCode, file, function, line,
                           "gets error of '%s', Details: %s", axisprofileResultToString(result), errTxt == nullptr ? "none" : errTxt);
        fpu::g_FPUError = true;
      }
    }
  }

  /// @brief This function checks for FPU errors and sends a diagnosis if an error is detected.
  /// @param[in]  mainCode    The main diagnosis code to be sent if an FPU error is detected.
  /// @param[in]  detailCode  The detailed diagnosis code to be sent if an
  /// @param[in]  file        The name of the file where the check is performed.
  /// @param[in]  function    The name of the function where the check is performed.
  /// @param[in]  line        The line number where the check is performed.
  /// @param[in]  profile     The name of the profile where the check is performed.
  /// @param[in]  errTxt      The error text to be included in the diagnosis (optional).
  static inline void checkFPUErrProfile(std::uint32_t mainCode, std::uint32_t detailCode,
                                        const char* file, const char* function, std::uint32_t line,
                                        const char* profile, const char* errTxt = nullptr)
  {
    if (!fpu::g_FPUError)
    {
      AxisProfileResult result = AxisProfileResult::STS_OK;
      result = fpu::checkForFPUError(AxisProfileResult::STS_OK, AxisProfileResult::ERR_FPU_DIV_BY_ZERO);
      if (aprStatusFailed(result))
      {
        Dia::sendDiagnosis(mainCode, detailCode, profile, file, function, line,
                           "gets error of '%s', Details: %s", axisprofileResultToString(result), errTxt == nullptr ? "none" : errTxt);
        fpu::g_FPUError = true;
      }
    }
  }
}
