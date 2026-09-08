#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and the
/// axisprofile adaption callable for the scheduler.
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

  /// @brief This is the class for the axisprofile adaption callable.
  class AxisprofileAdaptionCallable : public AxisprofileCallable
  {
  public: // methods

    /// @brief Constructor
    explicit AxisprofileAdaptionCallable(IAxisProfileComponent* axisProfileComponent);
    /// @brief Destructor
    virtual ~AxisprofileAdaptionCallable() noexcept override;

  protected: // methods

    ::common::scheduler::SchedEventResponse switchToOperation(const ::common::scheduler::SchedEventPhase& eventPhase, comm::datalayer::Variant& param) final;
    ::common::scheduler::SchedEventResponse switchToSetup(const ::common::scheduler::SchedEventPhase& eventPhase, const ::common::scheduler::SchedEventType& eventType, comm::datalayer::Variant& param) final;
    ::common::scheduler::SchedEventResponse switchToExit(const ::common::scheduler::SchedEventPhase& eventPhase, comm::datalayer::Variant& param) final;
    ::common::scheduler::SchedEventResponse changeTaskProperties(const ::common::scheduler::SchedEventPhase& eventPhase, const comm::datalayer::Variant& param) final;
    ::common::scheduler::SchedEventResponse eventTick() final;

  private: // methods

    /// @brief This method get callable timeout in seconds
    /// @return timeout value
    std::uint32_t getCallableTimeout();
  };
}
