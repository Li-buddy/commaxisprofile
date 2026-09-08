/// @file
///
/// @brief This file is part of the axisprofile library and implements a class to
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
#include "debug_trace.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
#ifdef WIN32
  const bool g_startTraceEnabled = false;
#else
  const bool g_startTraceEnabled = true;
#endif // WIN32
}
common::log::trace::LogBuffered3 DebugTrace::s_bufferedTrace("comm.axisprofile", comm::axisprofile::g_startTraceEnabled);
common::log::trace::ILogRealTime3* DebugTrace::s_trace = &s_bufferedTrace;
common::log::trace::EnablingState DebugTrace::s_enablingState = {false, false, false};

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

void DebugTrace::addTrace(common::log::trace::IRegistrationRealTime3* traceRegistration)
{
  auto enablingStateCallback = [&s_enablingState = this->s_enablingState](const common::log::trace::EnablingState& enablingState)
  {
    s_enablingState = enablingState;
  };

  common::log::trace::TraceResult traceResult = traceRegistration->registerUnit(&s_trace, "comm.axisprofile", enablingStateCallback);
  if ( common::log::trace::statusFailed( traceResult ) )
  {
    s_trace = &s_bufferedTrace;
    APR_TRACE_ERROR("Calling 'registerUnit' failed with error code 0x%08X, so continue using buffered trace.", traceResult);
  }
  else
  {
    APR_TRACE_MESSAGE("Registered trace from bundle with pointer 0x%08X.", s_trace);
  }
}

void DebugTrace::removeTrace(common::log::trace::IRegistrationRealTime3* traceRegistration)
{
  // switch to buffered trace again
  if ( s_trace != &s_bufferedTrace )
  {
    common::log::trace::ILogRealTime3* traceObject = s_trace;

    s_trace = &s_bufferedTrace;

    common::log::trace::TraceResult traceResult = traceRegistration->unregisterUnit(&traceObject);
    if ( common::log::trace::statusFailed( traceResult ) )
    {
      APR_TRACE_ERROR("Calling 'unregisterUnit' failed with error code 0x%08X.", traceResult);
    }

    APR_TRACE_MESSAGE("Removed trace from bundle.");
  }
}

common::log::trace::ILogRealTime3* DebugTrace::getTrace()
{
  return s_trace;
}

void DebugTrace::setTrace(common::log::trace::ILogRealTime3* newTrace)
{
  s_trace = newTrace;
}

common::log::trace::ILogRealTime3* DebugTrace::getBufferedTrace()
{
  return &s_bufferedTrace;
}

const common::log::trace::EnablingState* DebugTrace::getEnableStates()
{
  return &s_enablingState;
}
