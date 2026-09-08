/// @file
///
/// @brief This file is part of the axisprofile library and implements the celix
/// bundle activator class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "activator.h"
#include "comm/axisprofile/interfaces/i_axisprofile_manager.h"
#include "comm/axisprofile/interfaces/i_axisprofile_system.h"
#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/persistence_helper_itf.h"
#include "comm_axisprofile_bundle.h"
#include "common/log/diagnosis/i_registration4.h"
#include "common/log/trace/i_registration_real_time3.h"
#include "common/scheduler/i_scheduler2.h"
#include "common/systemhandler/i_systemstate.h"
#include <iostream>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

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

/// This method should be used to allocated and initialize a dm activator structure.
/// If needed this structure can be used to store object during the life-cycle of the bundle.
/// This method is called during start of bundle.
///
/// @param[in]    mng            A reference to the dependency manager.
/// @return                      A pointer to the created activator class.
DmActivator* DmActivator::create(DependencyManager& mng)
{
  std::cout << "call of 'DmActivator" << "::" << __func__ << "'..." << std::endl;

  // instantiate activator
  return new Activator(mng);
}

void Activator::init()
{
  std::cout << "call of '" << this->getBundleName() << ": " << this->getClassName() << "::" << __func__ << "'..." << std::endl;
  
  // create component
  auto cmp = std::unique_ptr<comm::axisprofile::AxisProfileBundle>(new comm::axisprofile::AxisProfileBundle());
  if (cmp)
  {
    // remember object because after 'move' 'cmp' is not longer valid, usable for register interfaces
    // auto ptrObject = cmp.get();

    // save bundle context because needed to detect bundle path etc.
    cmp->setDependencyManager(mng.bundleContext());

    // save root path and name of bundle, maybe needed for finding files, loading additional shared objects etc.
    cmp->setBundlePath(this->getBundlePath());
    cmp->setBundleName(this->getBundleName());

    // create dependency manager component; adapt component name if needed
    Component<comm::axisprofile::AxisProfileBundle>& component = mng.createComponent(std::move(cmp), "comm.axisprofile");

    // set callbacks for life cycle management
    component.setCallbacks(&comm::axisprofile::AxisProfileBundle::init,
                           &comm::axisprofile::AxisProfileBundle::start,
                           &comm::axisprofile::AxisProfileBundle::stop,
                           &comm::axisprofile::AxisProfileBundle::deinit);
    {
      // create service dependency for trace (C++)
      component.createServiceDependency<common::log::trace::IRegistrationRealTime3>(common::log::trace::CPP_INTERFACE_TRACE_REGISTRATION_REAL_TIME_3_NAME)
        .setVersionRange(common::log::trace::CPP_INTERFACE_TRACE_REGISTRATION_REAL_TIME_3_CONSUMER_RANGE)
        .setRequired(false)
        .setStrategy(DependencyUpdateStrategy::locking)
        .setCallbacks(&comm::axisprofile::AxisProfileBundle::addTrace, &comm::axisprofile::AxisProfileBundle::removeTrace);
      APR_TRACE_MESSAGE("created service dependency for %s", common::log::trace::CPP_INTERFACE_TRACE_REGISTRATION_REAL_TIME_3_NAME);
    }

    {
    // create service dependency for datalayer factory (C++)
    component.createServiceDependency<comm::datalayer::IDataLayerFactory>(IDATALAYER_FACTORY_INTERFACE_NAME)
      .setRequired(true)
      .setVersionRange(IDATALAYER_CONSUMER_RANGE)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::datalayerServiceChanged)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::addDataLayer, &comm::axisprofile::AxisProfileBundle::removeDatalayer);
    APR_TRACE_MESSAGE("created service dependency for %s", IDATALAYER_FACTORY_INTERFACE_NAME);
    }

    {
    // create service dependency for datalayer persistence helper (C)
    component.createCServiceDependency<PersistenceHelperItf>(PERSISTENCE_C_INTERFACE_NAME)
      .setVersionRange(PERSISTENCE_C_INTERFACE_DEFAULT_CONSUMER_RANGE)
      .setRequired(true)
      .setStrategy(DependencyUpdateStrategy::suspend)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::persistenceHelperServiceChanged)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::addPersistenceHelper, &comm::axisprofile::AxisProfileBundle::removePersistenceHelper);
    APR_TRACE_MESSAGE("created service dependency for %s", PERSISTENCE_C_INTERFACE_NAME);
    }

    {
    // create service dependency for scheduler (C++)
    component.createServiceDependency<common::scheduler::IScheduler2>(COMMON_SCHED_CPP_INTERFACE_NAME)
      .setRequired(true)
      .setVersionRange(COMMON_SCHED_CPP_INTERFACE_DEFAULT_CONSUMER_RANGE)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::schedulerServiceChanged)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::addScheduler, &comm::axisprofile::AxisProfileBundle::removeScheduler);
    APR_TRACE_MESSAGE("created service dependency for %s", COMMON_SCHED_CPP_INTERFACE_NAME);
    }

    {
    // create service dependency for machine state
    component.createServiceDependency<common::systemhandler::ISystemState2>()
      .setVersionRange(ICOMMON_SYSTEMHANDLER_SYSTEM_STATE_CPP_CONSUMER_RANGE)
      .setRequired(true)
      .setStrategy(DependencyUpdateStrategy::suspend)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::addSystemState, &comm::axisprofile::AxisProfileBundle::removeSystemState);
    APR_TRACE_MESSAGE("created service dependency for %s", ICOMMON_SYSTEMHANDLER_SYSTEM_STATE2_CPP_NAME);
    }

    {
    // create service dependency for diagnosis (C++)
    component.createServiceDependency<common::log::diagnosis::IRegistration4>(common::log::diagnosis::CPP_INTERFACE_DIAGNOSIS_REGISTRATION_4_NAME)
      .setRequired(true)
      .setVersionRange(common::log::diagnosis::CPP_INTERFACE_DIAGNOSIS_REGISTRATION_4_CONSUMER_RANGE)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::diagnosisServiceChanged)
      .setCallbacks(&comm::axisprofile::AxisProfileBundle::addDiagnosis, &comm::axisprofile::AxisProfileBundle::removeDiagnosis);
    APR_TRACE_MESSAGE("created service dependency for %s", common::log::diagnosis::CPP_INTERFACE_DIAGNOSIS_REGISTRATION_4_NAME);
    }

    // now register axis profile manager external interface (C++)
    Properties prop_ext{};
    prop_ext["meta.info.key.name"] = IAPR_CM_CPP_INTERFACE_NAME;
    prop_ext["meta.info.key.version"] = IAPR_CM_CPP_INTERFACE_VERSION;
    component.addInterfaceWithName<comm::axisprofile::IAxisProfileManager>(IAPR_CM_CPP_INTERFACE_NAME, IAPR_CM_CPP_INTERFACE_VERSION, prop_ext);
    APR_TRACE_MESSAGE("Bundlename: %s; --> register axis profile manager interface", this->getBundleName());

    // now register axis profile system external interface (C++)
    Properties prop_extSystem{};
    prop_extSystem[ "meta.info.key.name" ] = IAPR_SYSTEM_CM_CPP_INTERFACE_NAME;
    prop_extSystem[ "meta.info.key.version" ] = IAPR_SYSTEM_CM_CPP_INTERFACE_VERSION;
    component.addInterfaceWithName<comm::axisprofile::IAxisProfileSystem>(IAPR_SYSTEM_CM_CPP_INTERFACE_NAME, IAPR_SYSTEM_CM_CPP_INTERFACE_VERSION, prop_extSystem);
    APR_TRACE_MESSAGE("Bundlename: %s; --> register axis profile system interface", this->getBundleName());
  }
}

void Activator::deinit()
{
  std::cout << "call of '" << this->getBundleName() << ": " << this->getClassName() << "::" << __func__ << "'..." << std::endl;
}
