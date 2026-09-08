#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements the
/// axis profile callable factory for the scheduler.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "common/scheduler/i_scheduler2.h"
#include "scheduler/callable/axisprofile_adaption_callable.h"
#include "scheduler/callable/axisprofile_distribution_callable.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  /// @brief Description of the callable (+ factory)
  const std::vector<std::string> AXISPROFILE_CALLABLE_DESCRIPTION {
    "@author Bosch Rexroth AG",
    "@origin Motion",
    "@component Axisprofile",
    "@description Implements the real-time conversion from device data to motion values and back for various motion profiles for ctrlX MOTION.",
    "@note This is an automatically generated callable. Please do not modify. Required exactly twice in a control system.",
  };

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

  /// @brief This is the base class for the axisprofile callable factory.
  class AxisprofileCallableFactory : public common::scheduler::ICallableFactory2
  {
  public: // methods

    /// @brief Constructor
    explicit AxisprofileCallableFactory(IAxisProfileComponent* axisProfileComponent);
    /// @brief Destructor
    virtual ~AxisprofileCallableFactory() noexcept override;

    // ------------------------------------- Interface Methods -------------------------------------
    std::shared_ptr<common::scheduler::ICallable> createCallable(const comm::datalayer::Variant& param) final;
    common::scheduler::SchedStatus destroyCallable(const std::shared_ptr<common::scheduler::ICallable>& callable) final;
    common::scheduler::SchedStatus getCallableArguments(std::vector<std::string>& arguments) final;
    void getFactoryDescription(std::vector<std::string>& description) const final;
    void getCallableConfigurations(comm::datalayer::Variant& configurations) const final;

    /// @brief This method returns the state of the scheduler's callable.
    /// @param[in]  typeSel          Selector for callable type
    /// @returns    State of the scheduler's callable
    common::scheduler::SchedState  getCallableState(CallableType typeSel);

    /// @brief This method returns the callable
    /// @param[in]  typeSel          Selector for callable type
    /// @returns    shared pointer to the callable
    std::shared_ptr<AxisprofileCallable>  getCallable(CallableType typeSel);

  private: // data

    IAxisProfileComponent* m_axisProfileComponent{};      ///< Pointer to the axis profile component
    std::shared_ptr<AxisprofileAdaptionCallable> m_axisprofileAdaptionCallable;         ///< The axis profile adaption callable
    std::shared_ptr<AxisprofileDistributionCallable> m_axisprofileDistributionCallable; ///< The axis profile distribute callable 
  };
}
