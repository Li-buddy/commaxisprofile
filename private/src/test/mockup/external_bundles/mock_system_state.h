#pragma once

/// @file          mock_system_state.h
///
/// This header file is part of the axisprofile library and implements the
/// mockup of system state
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/systemhandler/i_systemstate.h"

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
  // Forward references
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief mockup of machine component
  class MockMachineComponent : public ::common::systemhandler::IComponent
  {
  public: // method

    MockMachineComponent() = default;
    virtual ~MockMachineComponent() noexcept override = default;

    std::string getName() override { return "mockMachineComponent"; }
    void setState(::common::systemhandler::ComponentState newState) override { m_state = newState; }
    ::common::systemhandler::ComponentState getState() override { return m_state; }
    void unregister() override {};
    void getStatus() override {};

  public: // data

    ::common::systemhandler::ComponentState m_state {common::systemhandler::ComponentState::COMPONENT_STATE_UNDEFINED};  ///< component state
  };

  /// @brief mockup of system state
  class MockSystemState : public ::common::systemhandler::ISystemState2
  {
  public: // method

    MockSystemState() = default;
    virtual ~MockSystemState() = default;

    virtual std::shared_ptr<::common::systemhandler::IComponent> registerComponent2(const comm::datalayer::Variant& /*data*/) override
    { return std::unique_ptr<::common::systemhandler::IComponent>(new MockMachineComponent); };
  };
}
