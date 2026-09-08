/// @file
///
/// @brief This file is part of the axisprofile library and implements the axisprofile
/// adaption callable.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_adaption_callable.h"
#include "common/debug_trace.h"
#include "common/scheduler/state_generated.h"
#include "common/scheduler/task_generated.h"
#include "common/systemhandler/i_systemstate.h"
#include "component/i_axisprofile_component.h"
#include "component/statemachine/i_state_machine.h"
#include "configuration/i_boot_mgr.h"
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

AxisprofileAdaptionCallable::AxisprofileAdaptionCallable(IAxisProfileComponent* axisProfileComponent) : AxisprofileCallable(axisProfileComponent)
{}

AxisprofileAdaptionCallable::~AxisprofileAdaptionCallable()
{}

::common::scheduler::SchedEventResponse AxisprofileAdaptionCallable::switchToOperation(const ::common::scheduler::SchedEventPhase& eventPhase, comm::datalayer::Variant& param)
{
  switch (eventPhase)
  {
    // in begin phase, scheduler will call it several cycles until all callable return OKAY
    // axisprofile only take checking whether it can switching to operation, return OKAY if checking success
    case ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_BEGIN:
    {
      APR_TRACE_MESSAGE("Adaption callable try to switchToOperation");

      if (!m_axisProfileComponent || !m_axisProfileComponent->getAxisProfileState())
      {
        AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_NULL, "Null axisProfileComponent pointer detected.");
        APR_TRACE_ERROR("Null axisProfileComponent pointer detected!");
        return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
      }

      // when axisprofile in Config, Running or Booting state, go ahead next step
      if (0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Configuration") ||
          0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Running") ||
          0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Booting"))
      {
        m_bootingOneShotFlag = true;

        param.setValue(getCallableTimeout()); // timeout providers to scheduler
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
      }

      AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_STATEMACHINE_ERR, DETAILED_DIAG_STM_AT_ERROR_STATE, "Axisprofile is in error state.");
      APR_TRACE_ERROR("Invalid axisprofile state: %s!", m_axisProfileComponent->getAxisProfileState());
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
    }

    // in execute phase, scheduler will call it several cycles until all callable return OKAY, and than switch to end phase
    // scheduler keep in OP if any callable failed during Booting.
    // axisprofile will switch to Running during this, return IN_PROCESS if Booting not finished, return OKAY when finished
    case ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE:
    {
      // scheduler has callable generate callable successfully, no matter axisprofile is Booting finished.
      m_state = ::common::scheduler::SchedState::SCHED_STATE_OPERATING;

      // when axisprofile in CONFIG state, trigger Booting
      if (0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Configuration"))
      {
        if (m_bootingOneShotFlag)
        {
          m_bootingOneShotFlag = false;
          auto result = m_axisProfileComponent->requestAxisProfileState("Booting");
          if (aprStatusFailed(result))
          {
            APR_TRACE_ERROR("Switch axisprofile state to Booting failed! Result: 0x%x!", result);

            AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_STATEMACHINE_ERR, DETAILED_DIAG_STM_REQUEST_FAILED, "Request booting failed.");
            return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
          }
          return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS;
        }

        AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_STATEMACHINE_ERR, DETAILED_DIAG_STM_REQUEST_FAILED, "Axisprofile Booting failed.");
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
      }

      // when axisprofile is in Booting state, return IN_PROCIESS
      if (0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Booting"))
      {
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS;
      }

      // when axisprofile is in Running state, return OKAY
      if (0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Running"))
      {
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
      }

      // could never be triggered
      AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_STATEMACHINE_ERR, DETAILED_DIAG_STM_AT_ERROR_STATE, "Invalid state for switching to Running.");
      APR_TRACE_ERROR("Switch axisprofile to Running failed, current state: %s!", m_axisProfileComponent->getAxisProfileState());
      return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
    }

    case ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_END:
    {
      APR_TRACE_MESSAGE("Adaption callable switchToOperation finished");
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }

    default:
    {
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
    }
  }
}

::common::scheduler::SchedEventResponse AxisprofileAdaptionCallable::switchToSetup(const ::common::scheduler::SchedEventPhase& eventPhase, const ::common::scheduler::SchedEventType& eventType, comm::datalayer::Variant& param)
{
  if (eventType != ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP &&
      eventType != ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SERVICE)
  {
    APR_TRACE_ERROR("%s event is not accepted to switch to Setup or Service!", common::scheduler::getSchedEventTypeAsString(eventType).c_str());
    return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
  }

  switch (eventPhase)
  {
    // only take some check in begin phase
    case common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_BEGIN:
    {
      APR_TRACE_MESSAGE("Adaption callable try to %s",
                        eventType == ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP ?
                        "switchToSetup" : "switchToService");

      if (!m_axisProfileComponent || !m_axisProfileComponent->getAxisProfileState())
      {
        Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_NULL, __FILE__, __FUNCTION__, __LINE__, "Null pointer detected");
        AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_NULL, "Null pointer detected.");
        return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
      }

      // should always be able to switch back to config
      m_configOneShotFlag = true;
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }
    case common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE:
    {
      m_state = eventType == ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP ?
         ::common::scheduler::SchedState::SCHED_STATE_SETUP : ::common::scheduler::SchedState::SCHED_STATE_SERVICE;

      if (m_axisProfileComponent->getBootManager()->isInitBooting())
      {
        // it only happen when reinstall a new version MOT
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS;
      }

      // when axisprofile is in Running or Booting, trigger switching to Config
      if (0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Running") ||
          0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Booting"))
      {
        if (m_configOneShotFlag)
        {
          m_configOneShotFlag = false;
          APR_TRACE_MESSAGE("Axisprofile component is %s state, try to switch to Configuration.", m_axisProfileComponent->getAxisProfileState());
          auto result = m_axisProfileComponent->requestAxisProfileState("Configuration");
          if (aprStatusFailed(result))
          {
            APR_TRACE_ERROR("Switch axisprofile state to Configuration failed! Result: 0x%x!", result);
            AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_STATEMACHINE_ERR, DETAILED_DIAG_STM_REQUEST_FAILED, "Request config failed.");
            return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
          }
        }
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS;
      }

      // when axisprofile is in Config state, return OKAY
      if (0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Configuration"))
      {
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
      }

      // could never be triggered
      APR_TRACE_ERROR("Switching to Configuration failed, current state: %s!", m_axisProfileComponent->getAxisProfileState());
      AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_STATEMACHINE_ERR, DETAILED_DIAG_STM_AT_ERROR_STATE, "invalid state.");
      return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
    }
    case ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_END:
    {
      // bugfix 512644
      // when switch to service, axisprofile switch to config by state machine, but the state should shows SERVICE, so overwrite by COMPONENT_STATE_SERVICE
      if (eventType == ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SERVICE)
      {
        APR_TRACE_MESSAGE("Adaption callable switchToService finished.");
        m_axisProfileComponent->getSystemStateComponent()->setState(::common::systemhandler::ComponentState::COMPONENT_STATE_SERVICE);
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
      }

      APR_TRACE_MESSAGE("Adaption callable switchToSetup finished.");
      m_axisProfileComponent->getSystemStateComponent()->setState(::common::systemhandler::ComponentState::COMPONENT_STATE_SETUP);
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }
    default:
    {
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
    }
  }
}

::common::scheduler::SchedEventResponse AxisprofileAdaptionCallable::switchToExit(const ::common::scheduler::SchedEventPhase& eventPhase, comm::datalayer::Variant& param)
{
  switch (eventPhase)
  {
    case common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_BEGIN:
    {
      APR_TRACE_MESSAGE("Adaption try to callable switchToExit");

      if (!m_axisProfileComponent || !m_axisProfileComponent->getAxisProfileState())
      {
        Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_NULL, __FILE__, __FUNCTION__, __LINE__, "Null pointer detected");
        return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
      }

      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }
    case common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE:
    {
      m_state = common::scheduler::SchedState::SCHED_STATE_EXIT;
       
      if (0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Running") ||
          0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Booting") ||
          0 == strcmp(m_axisProfileComponent->getAxisProfileState(), "Error"))
      {
        auto result = m_axisProfileComponent->requestAxisProfileState("Configuration");
        if (aprStatusFailed(result))
        {
          APR_TRACE_ERROR("Switch axisprofile state to Configuration failed! Cannot respond to scheduler SWITCH_TO_CONFIG event request!");
          AxisprofileCallable::packDiagnosisToVariant(param, MAIN_DIAG_STATEMACHINE_ERR, DETAILED_DIAG_STM_REQUEST_FAILED, "Request config state failed.");
          return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
        }
        return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS;
      }
      
      return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }
    case ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_END:
    {
      APR_TRACE_MESSAGE("Adaption callable switchToExit finished.");
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }
    default:
    {
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
    }
  }
}

::common::scheduler::SchedEventResponse AxisprofileAdaptionCallable::changeTaskProperties(const ::common::scheduler::SchedEventPhase& eventPhase, const ::comm::datalayer::Variant& param)
{
  switch (eventPhase)
  {
    case ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_BEGIN:
    {
      APR_TRACE_MESSAGE("Adaption callable try to changeTaskProperties");

      if (!m_axisProfileComponent || !m_axisProfileComponent->getAxisProfileState())
      {
        Dia::sendDiagnosis(MAIN_DIAG_GENERAL_ERR, DETAILED_DIAG_NULL, __FILE__, __FUNCTION__, __LINE__, "Null pointer detected");
        return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
      }

      // Callable/Scheduler needs to be in STEUP OR SERVICE state for task changes
      if ((m_state != ::common::scheduler::SchedState::SCHED_STATE_SETUP) && (m_state != ::common::scheduler::SchedState::SCHED_STATE_SERVICE))
      {
        APR_TRACE_ERROR("%s", "Axisprofile callable/scheduler not in state SEPUP OR SERVICE! Cannot respond to scheduler TASK_PROPERTIES_CHANGE event request!");
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
      }

      // Axisprofile needs to be in "Booting" or "Configuration" state for task changes 
      const char* axisprofileState = m_axisProfileComponent->getAxisProfileState();
      if ((0 != strcmp(axisprofileState, "Booting")) && (0 != strcmp(axisprofileState, "Configuration")))
      {
        APR_TRACE_ERROR("%s", "Axisprofile not in state Booting or Configuration! Cannot respond to scheduler TASK_PROPERTIES_CHANGE event request!");
        return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
      }

      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }

    case ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE:
    {
      if (comm::datalayer::DlResult::DL_OK != param.verifyFlatbuffers(common::scheduler::fbs::VerifyTaskBuffer))
      {
        return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
      }

      std::uint32_t cycletime = common::scheduler::fbs::GetTask(param.getData())->cycletime();
      APR_TRACE_MESSAGE("Scheduler properties changed: new cycle time: %d micro second.", cycletime);

      m_axisProfileComponent->setCycleTime(MICROSECOND_2_SECOND(cycletime));
      return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }

    case ::common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_END:
    {
      APR_TRACE_MESSAGE("Adaption callable changeTaskProperties finished.");
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
    }

    default:
    {
      return ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR;
    }
  }
}

std::uint32_t AxisprofileAdaptionCallable::getCallableTimeout()
{
  double maxTotalTimeout = m_axisProfileComponent->getMaxBootingTimeout();
  APR_TRACE_MESSAGE("Adaption callable timeout set: %.3f seconds.", maxTotalTimeout);
  return static_cast<std::uint32_t>(maxTotalTimeout) + 1;
}

::common::scheduler::SchedEventResponse AxisprofileAdaptionCallable::eventTick()
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
    m_axisProfileComponent->getAxisProfileManager()->processDataAt();
  }
  return common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY;
}
