#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the extension macros for checking results of function calls in tests.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "motion/test/result_helper.h"
#include "stdafx.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  /// @brief Logging unexpected state.
  ///
  /// If not the expected error occurs the given comment, file position and result will be logged before
  /// the test aborts by an erroneous HRESULT.
  #define RETURN_TEST_STATE(expState,comment,actState,stateToStringFun) \
  {\
    std::int32_t state = static_cast<std::int32_t>(expState);\
    if (state != static_cast<std::int32_t>(actState))\
    {\
      const std::wstring wComment(comment);\
      error(L"\n%ls:%d %ls (result %ls != %ls).",LITERALWIDECHAR(__FILE__),__LINE__, wComment.c_str(), UTF8TOWIDE(stateToStringFun(actState)), UTF8TOWIDE(stateToStringFun(expState)));\
      return E_FAIL;\
    }\
    else\
    {\
      log(L"<DARK_GREEN><b>success.</b></DARK_GREEN>");\
    }\
  }

  /// @brief Logging unexpected state.
  ///
  /// If not the expected state the given comment, file position and result will be logged and
  /// also an error will be registered for the test instance before execution continues.
  /// If everything works fine file position and result 'ok' will be logged.
  #define LOG_TEST_STATE(resState,comment,expState,stateToStringFun) \
  {\
    std::int32_t state = static_cast<std::int32_t>(resState);\
    if (state != static_cast<std::int32_t>(expState))\
    {\
      const std::wstring wComment(comment);\
      error(L"\n%ls:%d %ls (result %ls != %ls).", LITERALWIDECHAR(__FILE__),__LINE__, wComment.c_str(), UTF8TOWIDE(stateToStringFun(resState)), UTF8TOWIDE(stateToStringFun(expState)));\
      errorSilent();\
    }\
    else\
    {\
      log(L"<DARK_GREEN><b>success.</b></DARK_GREEN>");\
    }\
  }

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
}
