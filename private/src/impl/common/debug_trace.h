#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and declares a class to
/// use the developer trace.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "../src/diagnosis/axisprofile_main_diag.h"
#include "common/log/trace/i_log_real_time3.h"
#include "common/log/trace/i_registration_real_time3.h"
#include "common/log/trace/log_buffered3.h"

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

  /// This macro prints the given parameter as string.
  #define _CLASS_NAME_(x)   (           # x )
  /// This macro maps the given parameter to a macro that prints it as string.
  #define CLASS_NAME(x)     (_CLASS_NAME_(x))

  /// @brief Send a new trace message
  #define APR_TRACE_MESSAGE(text, ...) \
  { \
    if(comm::axisprofile::DebugTrace::getEnableStates()->messages) \
    { \
      COMMON_LOG_TRACE_REAL_TIME_LOG3(comm::axisprofile::DebugTrace::getTrace(), comm::axisprofile::MAIN_DIAG_AXISPROFILE_TRACE_MESSAGE, {}, "comm.axisprofile", common::log::trace::getFileName(__FILE__), __FUNCTION__, __LINE__, text, ##__VA_ARGS__) \
    } \
  }

  /// @brief Send a new trace warning
  #define APR_TRACE_WARNING(text, ...) \
  { \
    if(comm::axisprofile::DebugTrace::getEnableStates()->warnings) \
    { \
      COMMON_LOG_TRACE_REAL_TIME_LOG3(comm::axisprofile::DebugTrace::getTrace(), comm::axisprofile:: MAIN_DIAG_AXISPROFILE_TRACE_WARNING, {}, "comm.axisprofile", common::log::trace::getFileName(__FILE__), __FUNCTION__, __LINE__, text, ##__VA_ARGS__) \
    } \
  }

  /// @brief Send a new trace error
  #define APR_TRACE_ERROR(text, ...) \
  { \
    if(comm::axisprofile::DebugTrace::getEnableStates()->errors) \
    { \
      COMMON_LOG_TRACE_REAL_TIME_LOG3(comm::axisprofile::DebugTrace::getTrace(), comm::axisprofile::MAIN_DIAG_AXISPROFILE_TRACE_ERROR,   {}, "comm.axisprofile", common::log::trace::getFileName(__FILE__), __FUNCTION__, __LINE__, text, ##__VA_ARGS__) \
    } \
  }

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @class DebugTrace
  /// This class provides access to the developer trace from the trace bundle.
  class DebugTrace
  {
  public: // methods
    DebugTrace() = default;
    virtual ~DebugTrace() noexcept = default;

    /// @brief This method returns the class name.
    /// @return                      Return class name.
    static const char* getClassName(void) { return (CLASS_NAME(comm::axisprofile::DebugTrace)); }

    /// @brief Set the trace pointer from the bundle and replace the buffered trace
    /// @param[in]  traceRegistration  pointer to the trace registration
    void addTrace(common::log::trace::IRegistrationRealTime3* traceRegistration);

    /// @brief Remove the trace pointer from the bundle and use buffered trace again
    /// @param[in]  traceRegistration  pointer to the trace registration
    static void removeTrace(common::log::trace::IRegistrationRealTime3* traceRegistration);

    /// @brief Get the current trace object
    /// @return the current trace object
    static common::log::trace::ILogRealTime3* getTrace();

    /// @brief Set the current trace object
    /// @param[in] newTrace  the new trace object
    static void setTrace(common::log::trace::ILogRealTime3* newTrace);

    /// @brief Get the buffered Trace
    /// @return the buffered Trace
    static common::log::trace::ILogRealTime3* getBufferedTrace();

    /// @brief Get the current trace enable states
    /// @return The current trace enable states
    static const common::log::trace::EnablingState* getEnableStates();

  private: // data

    static common::log::trace::LogBuffered3 s_bufferedTrace;    ///< global buffered trace that gets used when no trace bundle is loaded
    static common::log::trace::ILogRealTime3* s_trace;          ///< global anchor for the debug trace
    static common::log::trace::EnablingState s_enablingState;   ///< enabling state of trace logs
  };
}
