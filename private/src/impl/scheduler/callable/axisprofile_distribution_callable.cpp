/// @file
///
/// @brief This file is part of the axisprofile library and implements the axisprofile
/// distribution callable.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_distribution_callable.h"
#include "common/debug_trace.h"
#include "common/scheduler/state_generated.h"
#include "common/scheduler/task_generated.h"
#include "component/i_axisprofile_component.h"
#include "component/statemachine/i_state_machine.h"
#include "diagnosis/dia.h"
#include "manager/i_axisprofile_manager_internal.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
AxisprofileDistributionCallable::AxisprofileDistributionCallable(IAxisProfileComponent* axisProfileComponent): AxisprofileCallable(axisProfileComponent)
{}

AxisprofileDistributionCallable::~AxisprofileDistributionCallable()
{}

::common::scheduler::SchedEventResponse AxisprofileDistributionCallable::eventTick()
{
  if (!m_axisProfileComponent)
  {
    return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
  }

  // eventTick is generated on in SCHED_STATE_SETUP and SCHED_STATE_OPERATING, if this condition is not met, return OKAY and do nothing else
  if ((m_state != ::common::scheduler::SchedState::SCHED_STATE_OPERATING) && (m_state != ::common::scheduler::SchedState::SCHED_STATE_SETUP))
  {
    return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
  }

  auto state = m_axisProfileComponent->getStateMachine()->getState();
  if (StateIDType::RUNNING == state)
  {
    m_axisProfileComponent->getAxisProfileManager()->processDataMdt();
    m_axisProfileComponent->getAxisProfileManager()->processAuxJobs(); // TODO: remove aux job after whole background function is ready
  }

  return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
}
