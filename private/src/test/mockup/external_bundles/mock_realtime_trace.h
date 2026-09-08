#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_realtime_trace.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and declares a mockup
//! of the trace registration interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "common/log/trace/i_registration_real_time3.h"
#include "common/log/trace/i_log_real_time3.h"


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

#define UNUSED_ARGUMENT(x) (void)x
// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockTraceLogRT
  //! Helper class to simulate the trace log in test
  class MockTraceLogRT : public common::log::trace::ILogRealTime3
  {
  public:
    MockTraceLogRT() {};
    virtual ~MockTraceLogRT() {};
     common::log::trace::TraceResult logRealTime(std::uint32_t /*mainDiagnosisCode*/, const common::log::trace::LogParameters* /*logParameters*/, ...) override { return m_returnResult; }
     common::log::trace::TraceResult logRealTimeVaList(std::uint32_t /*mainDiagnosisCode*/, const common::log::trace::LogParameters* /*logParameters*/, va_list /*arguments*/) override { return m_returnResult; }

    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions by changing the returned value.
    //! @param[in]    result   New return code
    void setReturnValue(common::log::trace::TraceResult result) { m_returnResult = result; }

    //! @brief This method sets the callback function which is called when at least one enabling state for message, warning or error has changed.
    //! @param[in]   callbackEnablingState   Callback function that is called.
    void setCallbackEnablingState(const std::function<void(const common::log::trace::EnablingState&)>& callbackEnablingState) { m_callbackEnablingState = callbackEnablingState; }

    //! @brief This method enables or disables the trace outputs of the unit. Enabling state for messages, warnings and errors can be set individually.
    //! @param[in]   enablingState   Enabling state for messages, warnings and errors
    void setEnablingState(const common::log::trace::EnablingState& enablingState) { if (m_callbackEnablingState) { m_callbackEnablingState(enablingState); } }

  private: // data

    common::log::trace::TraceResult m_returnResult = common::log::trace::TraceResult::OKAY;    //!< Result of member functions
    std::function<void(const common::log::trace::EnablingState&)> m_callbackEnablingState{};   //!< Callback, used when any enabling state changes
  };


  //! @class MockTraceRegistrationRT
  //! Helper class to simulate the trace registration in test
  class MockTraceRegistrationRT : public common::log::trace::IRegistrationRealTime3
  {
  public:
    MockTraceRegistrationRT() = default;
    virtual ~MockTraceRegistrationRT() noexcept override = default;
    common::log::trace::TraceResult registerUnit(common::log::trace::ILogRealTime3** unit, const char* const /*unitName*/, const common::log::trace::EnablingState& /*enablingState*/, const std::function<void(const common::log::trace::EnablingState&)>& enablingStateCallback = nullptr) override
    {
      *unit = m_unit;
      if (m_unit)
      {
        static_cast<test::MockTraceLogRT*>(m_unit)->setCallbackEnablingState(enablingStateCallback);
      }
      return m_returnResult;
    }
    common::log::trace::TraceResult registerUnit(common::log::trace::ILogRealTime3** unit, const char* const /*unitName*/,
                                                 const std::function<void(const common::log::trace::EnablingState&)>& enablingStateCallback = nullptr) override
    {
      *unit = m_unit;
      if (m_unit)
      {
        static_cast<test::MockTraceLogRT*>(m_unit)->setCallbackEnablingState(enablingStateCallback);
      }
      return m_returnResult;
    }
    common::log::trace::TraceResult unregisterUnit(common::log::trace::ILogRealTime3** /*unit*/) override
    {
      return m_returnResult;
    }

    common::log::trace::TraceResult setEnablingState(common::log::trace::ILogRealTime3* const /*unit*/, const common::log::trace::EnablingState& /*enablingState*/) const
    {
      return m_returnResult;
    }
    common::log::trace::TraceResult getEnablingState(common::log::trace::ILogRealTime3* const/* unit*/, common::log::trace::EnablingState& /*enablingState*/) const
    {
      return m_returnResult;
    }

    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions by changing the returned value.
    //! @param[in]    result   New return code
    void setReturnValue(common::log::trace::TraceResult result) { m_returnResult = result; }

    //! @brief This method can be used by tests to set a log object for testing
    //! @param[in]    unit   New log object
    void setTestLog(common::log::trace::ILogRealTime3* unit) { m_unit = unit; }

  private: // data

    common::log::trace::TraceResult m_returnResult = common::log::trace::TraceResult::OKAY;   //!< Result of member functions
    common::log::trace::ILogRealTime3* m_unit = nullptr;                                       //!< used log object
  };
}


