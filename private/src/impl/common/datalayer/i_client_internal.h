#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define interface of wrapped datalayer client.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 -2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/datalayer.h"
#include <functional>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::datalayer
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
  // Declaration of classes
  // ---------------------------------------------------------------------------

  /// @brief Internal interface wrapper on IClinet.
  class IClientInternal : public IClient
  {
  public: // methods

    /// @brief Destructor
    virtual ~IClientInternal() noexcept override = default;

    /// @brief This method get base datalayer client.
    /// @return Pointer to base client.
    virtual IClient* getBase() = 0;

    /// @brief This method get function be called in background.
    /// @return callback function.
    virtual std::function<void(void)> getBackgroundFun() = 0;

    /// @brief This method enable background cycle, 
    /// @param[in] enable   True to move async behavior to back
    virtual void enableBackgroundFun(bool enable) = 0;
  };
}
