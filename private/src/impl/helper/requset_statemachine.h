#pragma once

/// @file      requset_statemachine.h
///
/// This header file is part of the axisprofile library and implements the
/// statemachine async request statemachine
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "fieldbuses/fieldbus_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------
    namespace async
    {
      constexpr std::int32_t TIMEOUT_CALLBACK = 2001;    //!< Timeout used for response callbacks (number of cycles)
    }

    // ---------------------------------------------------------------------------
    // Declaration of macros
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    /// @brief This class implements a state-machine for an async request
    class RequestStatemachine
    {
    public: // types

      /// @brief This enum represents the request-states of the asynchronous parameter access.
      enum class RequestStates : std::uint16_t
      {
        SEND_REQ = 0,         ///< Send request to device.
        WAIT_FOR_REQ,         ///< Wait for the request to be finished.
        DONE_REQ,             ///< Finish the request.
      };

    public: // methods

      /// @brief Constructor
      RequestStatemachine() = default;

      /// @brief Destructor
      virtual ~RequestStatemachine() noexcept = default;

      /// @brief This method switches the request-state to the next one.
      /// @return   The new request-state.
      RequestStates switchToNextRequestState()
      {
        switch (m_requestState)
        {
          case RequestStates::SEND_REQ:     m_requestState = RequestStates::WAIT_FOR_REQ; break;
          case RequestStates::WAIT_FOR_REQ: m_requestState = RequestStates::DONE_REQ; break;
          case RequestStates::DONE_REQ:     break;
        }
        return m_requestState;
      }

      /// @brief This method set the required request-state, used for test.
      /// @param[in]  state  Request state to be set
      /// @return   The required request-state.
      RequestStates setRequestState(RequestStates state)
      {
        return m_requestState = state;
      }

      /// @brief This method gets the actual request-state.
      /// @return   The actual request-state.
      RequestStates getRequestState()
      {
        return m_requestState;
      }

      /// @brief This method tick callback counter and check whether exceed
      /// @return   return true if request tick exceed
      bool tickCallbackAndCheckExceed()
      {
        if (!m_timeout || m_counter)
        {
          m_counter--;
          return false;
        }
        return true;
      }

      /// @brief This method get pointer of parameter callback data
      /// @return pointer of callback data
      ParameterResponseCallbackData* getCallbackData()
      {
        return &m_callbackInfo;
      }

      /// @brief This method get pointer of datalayer variant in callback data
      /// @return pointer of datalayer variant in callback data
      const comm::datalayer::Variant* getCallbackDlVar()
      {
        return &m_callbackInfo.m_requestVariant;
      }

      /// @brief This method reset callback data, should be called before create a async request
      void resetCallbackData()
      {
        m_callbackInfo.m_requestFinishedFlag = false;
        m_callbackInfo.m_dlResult = comm::datalayer::DlResult::DL_OK;
      }

      /// @brief This method set time out value of callback,  set to 0 for never timeout
      /// @param[in]  callbackTimeout   time out limitation of request
      void setCallbackTimeout(std::uint32_t callbackTimeout)
      {
        m_timeout = callbackTimeout;
      }

      /// @brief This method resets the request-state to the initial state.
      void resetRequestState()
      {
        m_requestState = RequestStates::SEND_REQ;
        reinitCallbackTick();
      }

      /// @brief re-init callback tick counter
      void reinitCallbackTick()
      {
        m_counter = m_timeout;
      }

    private: // data

      std::int32_t  m_timeout  {async::TIMEOUT_CALLBACK};     ///< Timeout value for async callback
      std::int32_t  m_counter  {m_timeout};                   ///< Callback timeout variable which is decremented once per cycle, when an asynchronous datalayer request is in progress.
      RequestStates m_requestState {RequestStates::SEND_REQ}; ///< Request state of async parameter access
      ParameterResponseCallbackData  m_callbackInfo {};       ///< Async callback information
    };
  }
}
