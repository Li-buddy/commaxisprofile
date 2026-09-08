#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines interface of background thread.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include <functional>

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
  /// @brief Type of callback function that be called in background thread.
  using BkCallbackFun = std::function<void (void)>;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is interface of background thread
  class IBackgroundThread
  {
  public:

    /// @brief Destructor
    virtual ~IBackgroundThread() noexcept = default;

    /// @brief Init of the background thread, called in init() of bundle
    virtual void init() = 0;

    /// @brief DeInit of the background thread, called in deInit() of bundle
    virtual void deInit() = 0;

    /// @brief Wake up the thread, called in booting phase (CONFIG --> RUNNING)
    virtual void wakeup() = 0;

    /// @brief This method append a callback function which will be called in
    ///        RUNNING and BOOTING state of component.
    /// @param[in] cb       Callback function in normal state
    /// @return   Return value indicating success or error
    virtual AxisProfileResult appCallbackInNormalState(BkCallbackFun cb) = 0;

    /// @brief This method append a callback function which will be called in
    ///        RUNNING state of component.
    /// @param[in] cb       Callback function in normal state
    /// @return   Return value indicating success or error
    virtual AxisProfileResult appCallbackInRunningState(BkCallbackFun cb) = 0;

    /// @brief This method set polling cycle time of the background thread, unit in second.
    /// @param[in] cycleTime   Cycle time in second
    /// @return   Return value indicating success or error
    virtual AxisProfileResult setCycleTime(double cycleTime) = 0;
  };
}
