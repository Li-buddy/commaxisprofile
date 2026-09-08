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
#include "axisprofile_callable_factory.h"
#include "common/debug_trace.h"
#include "common/scheduler/callable_configurations_generated.h"
#include "component/i_axisprofile_component.h"

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

AxisprofileCallableFactory::AxisprofileCallableFactory(IAxisProfileComponent* axisProfileComponent)
  : m_axisProfileComponent(axisProfileComponent)
{
}

AxisprofileCallableFactory::~AxisprofileCallableFactory()
{
}

std::shared_ptr<common::scheduler::ICallable> AxisprofileCallableFactory::createCallable(const comm::datalayer::Variant& param)
{
  if (comm::datalayer::VariantType::ARRAY_OF_STRING != param.getType())
  {
    return nullptr;
  }

  std::vector<std::string> arguments;
  for (std::size_t i = 0, j = param.getCount(); i < j; ++i)
  {
    arguments.push_back(static_cast<const char**>(param)[i]);
  }

  if (arguments.size() == 0)
  {
    APR_TRACE_MESSAGE("%s", "create callable with empty string!");
    return nullptr;
  }

  if (0 == arguments[0].compare(callableTypeString(CallableType::ADAPTION_CALLABLE)))
  {
    if (!m_axisprofileAdaptionCallable)
    {
      APR_TRACE_MESSAGE("create axisprofile %s callable called", callableTypeString(CallableType::ADAPTION_CALLABLE));
      m_axisprofileAdaptionCallable = std::make_shared<AxisprofileAdaptionCallable>(m_axisProfileComponent);
      return m_axisprofileAdaptionCallable;
    }
    else
    {
      APR_TRACE_MESSAGE("axisprofile %s callable already created", callableTypeString(CallableType::ADAPTION_CALLABLE));
      return nullptr;
    }
  }
  else if (0 == arguments[0].compare(callableTypeString(CallableType::DISTRIBUTION_CALLABLE)))
  {
    if (!m_axisprofileDistributionCallable)
    {
      APR_TRACE_MESSAGE("create axisprofile %s callable called", callableTypeString(CallableType::DISTRIBUTION_CALLABLE));
      m_axisprofileDistributionCallable = std::make_shared<AxisprofileDistributionCallable>(m_axisProfileComponent);
      return m_axisprofileDistributionCallable;
    }
    else
    {
      APR_TRACE_MESSAGE("axisprofile %s callable already created", callableTypeString(CallableType::DISTRIBUTION_CALLABLE));
      return nullptr;
    }
  }

  APR_TRACE_MESSAGE("%s", "invalid argument.");
  return nullptr;
}

common::scheduler::SchedStatus AxisprofileCallableFactory::destroyCallable(const std::shared_ptr<common::scheduler::ICallable>& callable)
{
  if (callable != m_axisprofileAdaptionCallable && callable != m_axisprofileDistributionCallable)
  {
    APR_TRACE_MESSAGE("%s", "Tried to destroy a not specified axisprofile callable!");
    return common::scheduler::SchedStatus::SCHED_E_INVALID_ARGUMENTS;
  }

  if (callable == m_axisprofileAdaptionCallable)
  {
    APR_TRACE_MESSAGE("destroy axisprofile %s callable called", callableTypeString(CallableType::ADAPTION_CALLABLE));
    m_axisprofileAdaptionCallable.reset();
  }
  else
  {
    APR_TRACE_MESSAGE("destroy axisprofile %s callable called", callableTypeString(CallableType::DISTRIBUTION_CALLABLE));
    m_axisprofileDistributionCallable.reset();
  }

  return common::scheduler::SchedStatus::SCHED_S_OK;
}

common::scheduler::SchedStatus AxisprofileCallableFactory::getCallableArguments(std::vector<std::string>& arguments)
{
  arguments.push_back(callableTypeString(CallableType::ADAPTION_CALLABLE));
  arguments.push_back(callableTypeString(CallableType::DISTRIBUTION_CALLABLE));
  return common::scheduler::SchedStatus::SCHED_S_OK;
}

common::scheduler::SchedState AxisprofileCallableFactory::getCallableState(CallableType typeSel)
{
  auto schedState{ ::common::scheduler::SchedState::SCHED_STATE_SERVICE};
  if (typeSel == CallableType::ADAPTION_CALLABLE)
  {
    if (m_axisprofileAdaptionCallable)
    {
      schedState = m_axisprofileAdaptionCallable->getState();
    }
  }

  if (typeSel == CallableType::DISTRIBUTION_CALLABLE)
  {
    if (m_axisprofileDistributionCallable)
    {
      schedState = m_axisprofileDistributionCallable->getState();
    }
  }

  return schedState;
}

std::shared_ptr<AxisprofileCallable> AxisprofileCallableFactory::getCallable(CallableType typeSel)
{
  if (typeSel == CallableType::ADAPTION_CALLABLE)
  {
    return m_axisprofileAdaptionCallable;
  }

  if (typeSel == CallableType::DISTRIBUTION_CALLABLE)
  {
    return m_axisprofileDistributionCallable;
  }
  return nullptr;
};

void AxisprofileCallableFactory::getFactoryDescription(std::vector<std::string>& description) const
{
  description = AXISPROFILE_CALLABLE_DESCRIPTION;
}

void AxisprofileCallableFactory::getCallableConfigurations(comm::datalayer::Variant& configurations) const
{
  flatbuffers::FlatBufferBuilder builder;
  std::vector<flatbuffers::Offset<common::scheduler::fbs::CallableConfiguration>> fbsCallableConfig;
  auto fbsTaskSpec = common::scheduler::fbs::CreateTaskSpecsDirect(builder,
                                                                   "ctrlXAutomation",
                                                                   "high",
                                                                   "cyclic/ms/2");
  // config for adaption, which called before motion and after fieldbus
  {
    auto afterList = builder.CreateVectorOfStrings({"axisprofile-adaption-entry", "fieldbuses-exit"});
    auto beforeList = builder.CreateVectorOfStrings({"axisprofile-adaption-exit"});
    auto fbsSyncPoints = common::scheduler::fbs::CreateSyncPoints(builder, afterList, beforeList);
    auto args = builder.CreateVectorOfStrings({"adaption"});
    auto alias = builder.CreateString("adaption");
    fbsCallableConfig.push_back({common::scheduler::fbs::CreateCallableConfiguration(builder,
                                                                                     alias,
                                                                                     fbsSyncPoints,
                                                                                     args,
                                                                                     common::scheduler::fbs::CallableWdgConfig_WDG_HW_TASK,
                                                                                     fbsTaskSpec)});
  }

  // config for distribution, which called after motion
  {
    auto afterList = builder.CreateVectorOfStrings({"axisprofile-distribution-entry", "motion-exit"});
    auto beforeList = builder.CreateVectorOfStrings({"axisprofile-distribution-exit"});
    auto fbsSyncPoints = common::scheduler::fbs::CreateSyncPoints(builder, afterList, beforeList);
    auto args = builder.CreateVectorOfStrings({"distribution"});
    auto alias = builder.CreateString("distribution");
    fbsCallableConfig.push_back({common::scheduler::fbs::CreateCallableConfiguration(builder,
                                                                                     alias,
                                                                                     fbsSyncPoints,
                                                                                     args,
                                                                                     common::scheduler::fbs::CallableWdgConfig_WDG_HW_TASK,
                                                                                     fbsTaskSpec)});
  }

  auto finished = common::scheduler::fbs::CreateCallableConfigurationsDirect(builder, &fbsCallableConfig);

  builder.Finish(finished);
  configurations.copyFlatbuffers(builder);
  return;
}
