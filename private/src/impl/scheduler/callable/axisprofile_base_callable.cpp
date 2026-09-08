/// @file
///
/// @brief This file is part of the axisprofile library and implements the axisprofile
/// base callable.
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
#include "comm/datalayer/diagnosis_generated.h"
#include "common/debug_trace.h"
#include "common/scheduler/state_generated.h"
#include "common/scheduler/task_generated.h"
#include "component/i_axisprofile_component.h"
#include "component/statemachine/i_state_machine.h"

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
AxisprofileCallable::AxisprofileCallable(IAxisProfileComponent* axisProfileComponent): m_axisProfileComponent(axisProfileComponent)
{}

AxisprofileCallable::~AxisprofileCallable()
{}

::common::scheduler::SchedEventResponse AxisprofileCallable::execute(const ::common::scheduler::SchedEventType& eventType, const ::common::scheduler::SchedEventPhase& eventPhase, comm::datalayer::Variant& param)
{
  switch (eventType)
  {
    case ::common::scheduler::SchedEventType::SCHED_EVENT_TICK:
    {
      return eventTick();
    }

    case ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SERVICE:
    case ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP:
    {
      return switchToSetup(eventPhase, eventType, param);
    }

    case ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING:
    {
      return switchToOperation(eventPhase, param);
    }

    case ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT:
    {
      return switchToExit(eventPhase, param);
    }

    case ::common::scheduler::SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE:
    {
      return changeTaskProperties(eventPhase, param);
    }

    case ::common::scheduler::SchedEventType::SCHED_EVENT_GET_CURRENT_STATE:
    {
      return getCurrentState(param);
    }

    default:
      // generate error
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
  }
}

::common::scheduler::SchedState AxisprofileCallable::getState() const
{
  return m_state;
}

::common::scheduler::SchedEventResponse AxisprofileCallable::eventTick()
{
  return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
}

::common::scheduler::SchedEventResponse AxisprofileCallable::switchToOperation(const ::common::scheduler::SchedEventPhase& eventPhase, ::comm::datalayer::Variant& param)
{
  if (eventPhase == ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_BEGIN)
  {
    param.setValue(0); // set default timeout
  }
  else if (eventPhase == ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE)
  {
    m_state = common::scheduler::SchedState::SCHED_STATE_OPERATING;
  }
  else if (eventPhase == ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_END)
  {
    APR_TRACE_MESSAGE("Base callable switch to %s succeed.", getSystemStateAsString(m_state).c_str());
  }

  return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
}

::common::scheduler::SchedEventResponse AxisprofileCallable::switchToSetup(const ::common::scheduler::SchedEventPhase& eventPhase, const ::common::scheduler::SchedEventType& eventType, ::comm::datalayer::Variant& /*param*/)
{
  if (eventPhase == ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE)
  {
    switch (eventType)
    {
      case ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SERVICE:
        m_state = ::common::scheduler::SchedState::SCHED_STATE_SERVICE;
        break;
      case ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP:
        m_state = ::common::scheduler::SchedState::SCHED_STATE_SETUP;
        break;
      default:
        break;
    }
  }
  if (eventPhase == ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_END)
  {
    APR_TRACE_MESSAGE("Base callable switch to %s succeed.", getSystemStateAsString(m_state).c_str());
  }

  return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
}

::common::scheduler::SchedEventResponse AxisprofileCallable::switchToExit(const ::common::scheduler::SchedEventPhase& eventPhase, ::comm::datalayer::Variant& /*param*/)
{
  if (eventPhase == ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE)
  {
    m_state = common::scheduler::SchedState::SCHED_STATE_EXIT;
  }
  else if (eventPhase == ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_END)
  {
    APR_TRACE_MESSAGE("Base callable switch to %s succeed.", getSystemStateAsString(m_state).c_str());
  }
  return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
}

::common::scheduler::SchedEventResponse AxisprofileCallable::changeTaskProperties(const ::common::scheduler::SchedEventPhase& /*eventPhase*/, const ::comm::datalayer::Variant&/*param*/)
{
  return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
}

::common::scheduler::SchedEventResponse AxisprofileCallable::getCurrentState(::comm::datalayer::Variant& param)
{
  flatbuffers::FlatBufferBuilder builder;
  auto finished = ::common::scheduler::fbs::CreateState(builder, static_cast<::common::scheduler::fbs::CurrentState>(m_state));
  builder.Finish(finished);
  param.shareFlatbuffers(builder);
  return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
}

void AxisprofileCallable::packDiagnosisToVariant(comm::datalayer::Variant& param, const std::uint32_t mainDiagnosisCode, const std::uint32_t detailedDiagnosisCode, std::string const & dynamicDescription)
{
  flatbuffers::FlatBufferBuilder builder;
  comm::datalayer::DiagnosisT diagnosis {};

  diagnosis.mainDiagnosisCode = mainDiagnosisCode;
  diagnosis.detailedDiagnosisCode = detailedDiagnosisCode;
  diagnosis.dynamicDescription = dynamicDescription;

  builder.Finish(comm::datalayer::Diagnosis::Pack(builder, &diagnosis));
  param.copyFlatbuffers(builder);
}
