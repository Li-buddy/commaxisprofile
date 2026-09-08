#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements the
/// axisprofile base callable for the scheduler.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/scheduler/i_scheduler2.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestAxisProfileScheduler;

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

  /// @brief This is the class for the axisprofile base callable.
  class AxisprofileCallable : public common::scheduler::ICallable
  {
  public: // methods

    /// @brief Constructor
    explicit AxisprofileCallable(IAxisProfileComponent* axisProfileComponent);
    /// @brief Destructor
    virtual ~AxisprofileCallable() noexcept override;

    /// @brief This method returns state of the callable.
    /// @return Return callable state.
    ::common::scheduler::SchedState getState() const;

    // ------------------------------------- Interface Methods -------------------------------------
    /// @brief execute
    /// @param[in]  eventType     eventType
    /// @param[in]  eventPhase    scheduler event phase
    /// @param[in]  param         variant
    /// @return     Response of the event request
    ::common::scheduler::SchedEventResponse execute(const common::scheduler::SchedEventType& eventType, const common::scheduler::SchedEventPhase& eventPhase, comm::datalayer::Variant& param) final;

  protected: //methods

    /// @brief This method executes the scheduler cyclic event tick
    /// @return     Response of the event request 
    virtual ::common::scheduler::SchedEventResponse eventTick();

    /// @brief This method executes the scheduler SCHED_EVENT_SWITCH_TO_OPERATING event request.
    /// @param[in]  eventPhase     scheduler event phase
    /// @param[in]  param          reference to variant containing optional information
    /// @return     Response of the event request
    virtual ::common::scheduler::SchedEventResponse switchToOperation(const ::common::scheduler::SchedEventPhase& eventPhase,
                                                                      ::comm::datalayer::Variant& param);

    /// @brief This method executes the scheduler SCHED_EVENT_SWITCH_TO_SETUP and SCHED_EVENT_SWITCH_TO_SERVICE event request.
    /// @param[in]  eventPhase     scheduler event phase
    /// @param[in]  eventType      scheduler event type
    /// @param[in]  param          reference to variant containing optional information
    /// @return     Response of the event request
    virtual ::common::scheduler::SchedEventResponse switchToSetup(const ::common::scheduler::SchedEventPhase& eventPhase,
                                                                  const ::common::scheduler::SchedEventType& eventType,
                                                                  ::comm::datalayer::Variant& param);

    /// @brief This method executes the scheduler SWITCH_TO_EXIT event request.
    /// @param[in]  eventPhase     scheduler event phase
    /// @param[in]  param          reference to variant containing optional information
    /// @return     Response of the event request
    virtual ::common::scheduler::SchedEventResponse switchToExit(const ::common::scheduler::SchedEventPhase& eventPhase,
                                                                  ::comm::datalayer::Variant& param);

    /// @brief This method executes the scheduler TASK_PROPERTIES_CHANGE event request.
    /// @param[in]  eventPhase     scheduler event phase
    /// @param[in]  param          scheduler event parameters
    /// @return     Response of the event request
    virtual ::common::scheduler::SchedEventResponse changeTaskProperties(const ::common::scheduler::SchedEventPhase& eventPhase,
                                                                          const comm::datalayer::Variant &param);

    /// @brief This method executes the scheduler SCHED_EVENT_GET_CURRENT_STATE event request.
    /// @param[in]  param          scheduler event parameters
    /// @return     Response of the event request
    ::common::scheduler::SchedEventResponse getCurrentState(comm::datalayer::Variant& param);

    /// @brief This method create diagnosis variable to scheduler
    /// @param[in,out]  param                  scheduler event parameters
    /// @param[in]      mainDiagnosisCode      32Bit main diagnosis code defined in the diagnosis database
    /// @param[in]      detailedDiagnosisCode  32Bit detailed diagnosis code defined in the diagnosis database
    /// @param[in]      dynamicDescription     string of explanation of the problem
    /// @return         Response of the event request
    void packDiagnosisToVariant(comm::datalayer::Variant& param,
                                const std::uint32_t mainDiagnosisCode,
                                const std::uint32_t detailedDiagnosisCode,
                                std::string const & dynamicDescription);

  protected: // data

    friend class ::TestAxisProfileScheduler;

    IAxisProfileComponent* m_axisProfileComponent{}; ///< Reference to the axis profile component
    ::common::scheduler::SchedState m_state = common::scheduler::SchedState::SCHED_STATE_SERVICE;   ///< State of the callable
    bool m_bootingOneShotFlag {false};               ///< One shot flag for booting request
    bool m_configOneShotFlag {false};                ///< One shot flag for configure request
  };
}
