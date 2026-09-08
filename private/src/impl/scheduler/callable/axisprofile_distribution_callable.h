#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements the
/// axisprofile distribution callable for the scheduler.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_base_callable.h"

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

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class IAxisProfileComponent;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the class for the axisprofile distribution callable.
  class AxisprofileDistributionCallable : public AxisprofileCallable
  {
  public: // methods

    /// @brief Constructor
    explicit AxisprofileDistributionCallable(IAxisProfileComponent* axisProfileComponent);
    /// @brief Destructor
    virtual ~AxisprofileDistributionCallable() noexcept override;

  protected: //methods

    ::common::scheduler::SchedEventResponse eventTick() final;
  };
}
