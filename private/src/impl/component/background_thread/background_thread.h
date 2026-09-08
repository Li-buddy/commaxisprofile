#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines header of background thread.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_background_thread.h"

#include <thread>
#include <atomic>
#include <vector>
#include "common/semaphore.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class IAxisProfileComponent;

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  namespace bkthread
  {
    constexpr std::uint32_t DEFALUT_CYCLE_TIME_US = 2000; ///< Default cycle time of background
  }

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is class of background thread
  class BackgroundThread : public IBackgroundThread
  {
  public: // methods

    /// @brief Constructor, the thread is created in init of the bundle and deleted in deInit of the bundle.
    /// @Note Execution RUNNING state of axisprofile and the thread is sleep down automatically if axisprofile
    ///       switched to CONFIG/SERIVCE state.
    /// @param[in] profileCmp   Pointer to axisprofile component
    explicit BackgroundThread(IAxisProfileComponent* profileCmp);

    /// @brief Destructor
    virtual ~BackgroundThread() noexcept override;

    void init() final;
    void deInit() final;
    void wakeup() final;
    AxisProfileResult appCallbackInNormalState(BkCallbackFun cb) final;
    AxisProfileResult appCallbackInRunningState(BkCallbackFun cb) final;
    AxisProfileResult setCycleTime(double cycleTime) final;

  private: // methods

    /// @brief Internal execution of the thread
    /// @return   Return value indicating success or error
    AxisProfileResult execution();

    /// @brief This method execute function that in RUNNING state
    void exeInRuningState();

    /// @brief This method execute function that in RUNNING and BOOTING state
    void exeInNormalState();

  private: // data

    IAxisProfileComponent* m_profileCmp;  ///< Pointer to profile component

    Semaphore m_sema;       ///< Sleep down the thread
    std::thread* m_thread;  ///< Handle of the background thread
    std::atomic_bool m_exeAbort;  ///< Abort execution of thread
    std::atomic<std::uint32_t> m_cycleTimeUs; ///< Cycle time of the thread in us

    std::vector<BkCallbackFun> m_normalCbFuns {};   ///< Callback functions in normal state
    std::vector<BkCallbackFun> m_runningCbFuns {};  ///< Callback functions in running state
  };
}
