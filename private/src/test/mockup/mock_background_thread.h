#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define mockup class for interface IBackgroundThread.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "component/background_thread/i_background_thread.h"

#include <thread>
#include <atomic>
#include <vector>
#include "common/semaphore.h"
#include "mockup/mock_statemachine.h"

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

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is mockup class of background thread.
  class MockBackgroundThread : public comm::axisprofile::IBackgroundThread
  {
  public: // methods

    /// @brief Constructor
    MockBackgroundThread() = default;
    /// @brief Destructor
    virtual ~MockBackgroundThread() noexcept override = default;

    void init() override { return ; };
    void deInit() override { return ; };
    void wakeup() override  { return ; };
    ::comm::axisprofile::AxisProfileResult appCallbackInNormalState(comm::axisprofile::BkCallbackFun /*cb*/) override { return m_retValue; };
    ::comm::axisprofile::AxisProfileResult appCallbackInRunningState(comm::axisprofile::BkCallbackFun /*cb*/) override { return m_retValue; };
    ::comm::axisprofile::AxisProfileResult setCycleTime(double cycleTime)
    {
      m_cycleTimeUs = static_cast<std::uint32_t>(SECOND_2_MICROSECOND(cycleTime));
      return m_retValue;
    };

  public: // data

    std::uint32_t m_cycleTimeUs {2000}; ///< default tick cycle time
    ::comm::axisprofile::AxisProfileResult m_retValue {::comm::axisprofile::AxisProfileResult::STS_OK}; ///< return value
  };

  /// @brief This is mockup class of background thread that include thread function.
  class MockBackgroundThread2 : public comm::axisprofile::IBackgroundThread
  {
  public: // methods

    /// @brief Constructor
    MockBackgroundThread2() = default;
    /// @brief Destructor
    virtual ~MockBackgroundThread2() { deInit(); };

    void init() override
    {
      m_thread.reset(new std::thread(&MockBackgroundThread2::execution, this));
    }
    void deInit() override
    {
      m_exeAbort.store(true);
      m_sema.notify();
      m_thread->join();
    }
    void wakeup() override  { m_sema.notify(); }
    ::comm::axisprofile::AxisProfileResult appCallbackInNormalState(::comm::axisprofile::BkCallbackFun cb) override
    {
      m_normalCbFuns.emplace_back(cb);
      return m_retValue;
    }
    ::comm::axisprofile::AxisProfileResult appCallbackInRunningState(::comm::axisprofile::BkCallbackFun cb) override
    {
      m_runningCbFuns.emplace_back(cb);
      return m_retValue;
    }
    void execution()
    {
      while (!m_exeAbort.load())
      {
        // sleep the thread until another sync trigger from rt-thread.
        m_sema.wait();

        while (!m_exeAbort.load())
        {
          auto state = m_state.getState();
          if (state == ::comm::axisprofile::StateIDType::RUNNING)
          {
            for (auto& it : m_runningCbFuns)
            {
              if (it) { it(); }
            }
          }

          if (state == ::comm::axisprofile::StateIDType::RUNNING || state == ::comm::axisprofile::StateIDType::BOOTING)
          {
            for (auto& it : m_normalCbFuns)
            {
              if (it) { it(); }
            }
          }

          if (state == ::comm::axisprofile::StateIDType::CONFIG || m_exeAbort.load())
          {
            break;
          }

          std::this_thread::sleep_for(std::chrono::microseconds(m_cycleTimeUs));
        }

      }
    }

    ::comm::axisprofile::AxisProfileResult setCycleTime(double cycleTime)
    {
      m_cycleTimeUs = static_cast<std::uint32_t>(SECOND_2_MICROSECOND(cycleTime));
      return m_retValue;
    };

  public: // helper methods

    void setAprState(::comm::axisprofile::StateIDType state)
    {
      m_state.m_stateID = state;
    }
    
  public: // data

    MockStateMachine m_state;               ///< Axisprofile mock component
    std::atomic_bool m_exeAbort {false};    ///< Abort execution of thread
    std::unique_ptr<std::thread> m_thread;  ///< Handle of the background thread
    std::vector<::comm::axisprofile::BkCallbackFun> m_normalCbFuns {};   ///< Callback functions in normal state
    std::vector<::comm::axisprofile::BkCallbackFun> m_runningCbFuns {};  ///< Callback functions in running state

    std::uint32_t m_cycleTimeUs {2000};          ///< Default cycle time
    ::comm::axisprofile::Semaphore m_sema;  ///< Sleep down the thread
    ::comm::axisprofile::AxisProfileResult m_retValue {::comm::axisprofile::AxisProfileResult::STS_OK}; ///< Return value
  };
}
