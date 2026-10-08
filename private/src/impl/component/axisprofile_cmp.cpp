/// @file
///
/// @brief This file is part of the axis profile library and implements the device profile component.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#ifndef _MSC_VER
#include "celix/utils/celix_errno.h"
#else
#include "celix_errno.h"
#endif
#include "axisprofile_cmp.h"
#include "manager/axisprofile_manager.h"
#include "statemachine/state_machine.h"
#include "configuration/boot_mgr.h"
#include "configuration/backup_restore_mgr.h"
#include "configuration/boot_param_backup_restore_mgr.h"
#include "background_thread/background_thread.h"

// External Includes
#include "common/scheduler/i_scheduler.h"
#include "common/systemhandler/i_systemstate.h"
#include "common/systemhandler/component_generated.h"

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

IAxisProfileManager* AxisProfileCmp::getAxisProfileManagerInterface()
{
  return this;
}


AxisProfileCmp::~AxisProfileCmp()
{
  if (m_axisProfileManager)
  {
    delete m_axisProfileManager;
    m_axisProfileManager = nullptr;
  }

  if (m_statemachine)
  {
    delete m_statemachine;
    m_statemachine = nullptr;
  }
  
  if (m_bootManager)
  {
    delete m_bootManager;
    m_bootManager = nullptr;
  }
}


// -------------------------------------------------------------------------------
// ------------------------------- Celix Methods ---------------------------------
// -------------------------------------------------------------------------------

celix_status_t AxisProfileCmp::init()
{
  // Check service dependencies
  if (!m_datalayer || !m_persistenceHelper)
  {
    APR_TRACE_ERROR("Missing service dependency (null pointer)!");
    return CELIX_BUNDLE_EXCEPTION;
  }

  // create background thread
  m_bkThread.reset(new BackgroundThread(this));
  m_bkThread->init();

  // AxisProfile Manager
  delete m_axisProfileManager;
  m_axisProfileManager = new AxisProfileManager();
  m_axisProfileManager->setDataLayer(m_datalayer, m_bkThread.get());
  m_bkThread->appCallbackInRunningState([&]{m_axisProfileManager->runAllJobs();}); 

  // Global Statemachine
  delete m_statemachine;
  m_statemachine = new StateMachine();

  if(m_statemachine->getStateMachineStatus())
  {
    m_axisProfileManager->getStateMachineStatusIF() = m_statemachine->getStateMachineStatus();
  }

  // Backup/Restore Manager
  m_backupRestoreMgr = std::make_unique<BackupRestoreMgr>(m_axisProfileManager, m_datalayer->createClient(), m_persistenceHelper);

  m_bootParamBackupRestoreMgr = std::make_unique<BootParamBackupRestoreMgr>(m_axisProfileManager, m_persistenceHelper, m_bundlePath);
  m_axisProfileManager->setBootParamBackupRestoreMgr(m_bootParamBackupRestoreMgr.get());

  m_dprCallableFactory.reset();
  if(m_scheduler)
  {
    m_dprCallableFactory = std::make_shared<AxisprofileCallableFactory>(this);
  }
  // Boot Manager (must be created after the backup/restore manager!)
  delete m_bootManager;
  m_bootManager = new BootMgr();
  AxisProfileResult result = m_bootManager->init(this);
  if (aprStatusFailed(result))
  {
    APR_TRACE_ERROR("Initialization of boot manager failed! ErrCode: 0x%08X", result);
    return CELIX_BUNDLE_EXCEPTION;
  }

  // Provider Manager (must be initiated after the m_bootManager because of node_system_boot_state.cpp!)
  m_providerMgr.init(this);

  APR_TRACE_MESSAGE("Initializing %s complete.", m_bundleName.c_str());
  return CELIX_SUCCESS;
}

celix_status_t AxisProfileCmp::start()
{
  if ( !m_providerMgr.run() )
  {
    return CELIX_BUNDLE_EXCEPTION;
  }

  // load backup restore, call this before callable is registered!
  APR_TRACE_MESSAGE("Start initial booting to load configuration.");
  CmpMutexLockHelper mutexLockHelper(&m_mutexHelper);
  if (aprStatusFailed(m_bootManager->bootSystem()))
  {
    return CELIX_BUNDLE_EXCEPTION;
  }

  if (m_scheduler)
  {
    // Register callable at Scheduler
    if (!m_dprCallableFactory)
    {
      m_dprCallableFactory = std::make_shared<AxisprofileCallableFactory>(this);
    }
    common::scheduler::SchedStatus schedulerStatus = m_scheduler->registerCallableFactory(static_cast<const std::shared_ptr<common::scheduler::ICallableFactory2>&>(m_dprCallableFactory), "axisprofile");
    if (schedulerStatus != common::scheduler::SchedStatus::SCHED_S_OK)
    {
      APR_TRACE_ERROR("Error during register of axisprofile callable factory at scheduler.");
      return CELIX_BUNDLE_EXCEPTION;
    }
  }

  APR_TRACE_MESSAGE("Startup %s complete.", m_bundleName.c_str());
  return CELIX_SUCCESS;
}

celix_status_t AxisProfileCmp::stop()
{
  if ( m_bootManager )
  {
    m_bootManager->stop();
  }

  m_providerMgr.stop();

  if (m_scheduler)
  {
    // Unregister callable at Scheduler
    common::scheduler::SchedStatus schedulerStatus = m_scheduler->unregisterCallableFactory(static_cast<const std::shared_ptr<common::scheduler::ICallableFactory2>&>(m_dprCallableFactory));
    if (schedulerStatus != common::scheduler::SchedStatus::SCHED_S_OK)
    {
      APR_TRACE_ERROR("Error during unregister of axisprofile callable factory at scheduler.");
      return CELIX_BUNDLE_EXCEPTION;
    }
    m_dprCallableFactory.reset();
  }

  // delete outside resource in stop phase
  if ( m_bootManager )
  {
    delete m_bootManager;
    m_bootManager = nullptr;
  }

  m_backupRestoreMgr.reset();
  m_bootParamBackupRestoreMgr.reset();

  if ( m_axisProfileManager )
  {
    delete m_axisProfileManager;
    m_axisProfileManager = nullptr;
  }

  return CELIX_SUCCESS;
}

celix_status_t AxisProfileCmp::deinit()
{
  if ( m_statemachine )
  {
    delete m_statemachine;
    m_statemachine = nullptr;
  }

  m_bkThread.reset();
  return CELIX_SUCCESS;
}

// -------------------------------------------------------------------------------
// ---------------------- External Bundle Interface (Manager) --------------------
// -------------------------------------------------------------------------------

AxisProfileResult AxisProfileCmp::createAxisProfile(const AxisProfileType type, const std::string& profileName)
{
  CmpMutexLockHelper mutexLockHelper(&m_mutexHelper);

  if ( StateIDType::CONFIG != m_statemachine->getState() )
  {
    APR_TRACE_ERROR("Can not create profile. Bundle is not in state 'Configuration'!");
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  return m_axisProfileManager->createAxisProfile(type, profileName);
}

AxisProfileResult AxisProfileCmp::deleteAxisProfile(const std::string& profileName)
{
  CmpMutexLockHelper mutexLockHelper(&m_mutexHelper);

  if ( StateIDType::CONFIG != m_statemachine->getState() )
  {
    APR_TRACE_ERROR("Can not delete profile. Bundle is not in state 'Configuration'!");
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  return m_axisProfileManager->deleteAxisProfile(profileName);
}

AxisProfileResult AxisProfileCmp::connectAxisProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& profile)
{
  CmpMutexLockHelper mutexLockHelper(&m_mutexHelper);

  if (StateIDType::BOOTING >= m_statemachine->getState())
  {
    APR_TRACE_ERROR("Can not connect profile. Bundle is not in state 'Configuration' or 'Running'!");
    return AxisProfileResult::ERR_INVALID_STATE;
  }

  return m_axisProfileManager->connectAxisProfile(profileName, profile);
}

bool AxisProfileCmp::profileExists(const std::string & profileName) const
{
  return m_axisProfileManager->profileExists(profileName);
}

std::uint32_t AxisProfileCmp::getConfiguredProfileNumber() const
{
  return m_axisProfileManager->getConfiguredProfileNumber();
}

AxisProfileResult AxisProfileCmp::registerCheckCallback(const CheckCallBack& checkFct, const std::string& bundleName)
{
  return m_axisProfileManager->registerCheckCallback(checkFct, bundleName);
}

AxisProfileResult AxisProfileCmp::unregisterAllCheckCallbacks(const std::string& bundleName)
{
  return m_axisProfileManager->unregisterAllCheckCallbacks(bundleName);
}

// -------------------------------------------------------------------------------
// ---------------------- External Bundle Interface (System) ---------------------
// -------------------------------------------------------------------------------

const char* AxisProfileCmp::getAxisProfileState() const
{
  if ( !m_statemachine )
  {
    return nullptr;
  }
  return m_statemachine->getStateName(m_statemachine->getState());
}

AxisProfileResult AxisProfileCmp::requestAxisProfileState(const char *stateName)
{
  CmpMutexLockHelper mutexLockHelper(&m_mutexHelper);

  if ( !m_statemachine )
  {
    return AxisProfileResult::ERR_NOK;
  }
  StateIDType stateId;
  if ( !m_statemachine->getStateID(stateName, stateId) )
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  // Sometimes we hang in booting because we are waiting for the fieldbus datalayer RT memory. If so, commanding state 'CONFIG' can be used to stop it and continue.
  // The boot manager will then set the state to 'CONFIG', therefore we can return here.
  APR_TRACE_MESSAGE("New AxisProfile state requested! Actual: %s Target: %s", m_statemachine->getStateName(m_statemachine->getState()), stateName);
  if ((m_statemachine->getState() == StateIDType::BOOTING) && (stateId == StateIDType::CONFIG))
  {
    m_bootManager->abortBooting();
    return AxisProfileResult::STS_OK;
  }

  if ( GlobalStateResult::STS_OK != m_statemachine->requestSwitchToState(stateId) )
  {
    return AxisProfileResult::ERR_NOK;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCmp::getAxisProfileStateNames(std::vector<std::string> &stateNames) const
{
  if ( !m_statemachine )
  {
    return AxisProfileResult::ERR_NOK;
  }

  stateNames = m_statemachine->getStateNames();

  return AxisProfileResult::STS_OK;
}

// -------------------------------------------------------------------------------
// --------------------------- Internal Interface --------------------------------
// -------------------------------------------------------------------------------

IAxisProfileManagerInternal* AxisProfileCmp::getAxisProfileManager()
{
  return  static_cast<IAxisProfileManagerInternal*>(m_axisProfileManager);
}

IStateMachine* AxisProfileCmp::getStateMachine()
{
  return m_statemachine;
}

IBackupRestoreMgr* AxisProfileCmp::getBackupRestoreManager()
{
  return m_backupRestoreMgr.get();
}

const IBootParamBackupRestoreMgr* AxisProfileCmp::getBootParamBackupRestoreMgr() const
{
  return m_bootParamBackupRestoreMgr.get();
}

IBootMgr* AxisProfileCmp::getBootManager()
{
   return m_bootManager;
};

comm::datalayer::IDataLayerFactory* AxisProfileCmp::getDataLayer() const
{
  return m_datalayer;
}

const PersistenceHelperItf* AxisProfileCmp::getPersistenceHelper() const
{
  return m_persistenceHelper;
}

common::scheduler::IScheduler2* AxisProfileCmp::getScheduler() const
{
  return m_scheduler;
}

common::systemhandler::IComponent* AxisProfileCmp::getSystemStateComponent() const
{
  return m_systemStateComponent.get();
}

void AxisProfileCmp::setCycleTime(double cycleTime)
{
  m_cycleTime = cycleTime;
}

double AxisProfileCmp::getCycleTime() const
{
  return m_cycleTime;
}

double AxisProfileCmp::getMaxBootingTimeout() const
{
  return m_bootManager->getMaxBootingTimeout();
}

AxisProfileResult AxisProfileCmp::wakeupBkThread() const
{
  if (m_bkThread)
  {
    // polling cycle time same with rt thread
    m_bkThread->setCycleTime(m_cycleTime);
    m_bkThread->wakeup();
    return AxisProfileResult::STS_OK;
  }
  return AxisProfileResult::ERR_EXEC_NULL;
}

AxisProfileResult AxisProfileCmp::setBkThreadCycleTime(double cycleTime) const
{
  if (m_bkThread)
  {
    return m_bkThread->setCycleTime(cycleTime);
  }
  return AxisProfileResult::ERR_EXEC_NULL;
}

// -------------------------------------------------------------------------------
// --------------------------- Service Dependencies ------------------------------
// -------------------------------------------------------------------------------

void AxisProfileCmp::addDataLayer(comm::datalayer::IDataLayerFactory* datalayer)
{
  APR_TRACE_MESSAGE("Add datalayer service dependency.");
  if ( m_axisProfileManager )
  {
    m_axisProfileManager->setDataLayer(datalayer, m_bkThread.get());
  }
  m_datalayer = datalayer;
}

void AxisProfileCmp::removeDatalayer(comm::datalayer::IDataLayerFactory* /*datalayer*/)
{
  APR_TRACE_MESSAGE("Remove datalayer service dependency.");
  if ( m_axisProfileManager )
  {
    m_axisProfileManager->setDataLayer(nullptr, nullptr);
  }
  m_datalayer = nullptr;
}

void AxisProfileCmp::datalayerServiceChanged(comm::datalayer::IDataLayerFactory* /*datalayer*/)
{
  // Anything to do before adding/removing the datalayer?
}

void AxisProfileCmp::addTrace(common::log::trace::IRegistrationRealTime3* traceRegistration)
{
  APR_TRACE_MESSAGE("Set trace service dependency.");
  m_debugTrace.addTrace(traceRegistration);
}

void AxisProfileCmp::removeTrace(common::log::trace::IRegistrationRealTime3* traceRegistration)
{
  APR_TRACE_MESSAGE("Remove trace service dependency.");
  m_debugTrace.removeTrace(traceRegistration);
}

void AxisProfileCmp::traceServiceChanged(common::log::trace::IRegistrationRealTime3* /*traceRegistration*/)
{
  // Anything to do before adding/removing the trace?
}

void AxisProfileCmp::addDiagnosis(common::log::diagnosis::IRegistration4* diagnosisRegistration)
{
  APR_TRACE_MESSAGE("Set diagnosis service dependency.");
  m_diagnosis.setDiagnosisHandler(diagnosisRegistration, getBundlePath());
}

void AxisProfileCmp::removeDiagnosis(common::log::diagnosis::IRegistration4* diagnosisRegistration)
{
  APR_TRACE_MESSAGE("Remove diagnosis service dependency.");
  m_diagnosis.removeDiagnosisHandler(diagnosisRegistration);
}

void AxisProfileCmp::diagnosisServiceChanged(common::log::diagnosis::IRegistration4* /*diagnosisRegistration*/)
{
  // Anything to do before adding/removing the diagnosis?
}

void AxisProfileCmp::addScheduler(common::scheduler::IScheduler2* scheduler)
{
  APR_TRACE_MESSAGE("Add scheduler service dependency.");
  m_scheduler = scheduler;
}

void AxisProfileCmp::removeScheduler(common::scheduler::IScheduler2* /*scheduler*/)
{
  APR_TRACE_MESSAGE("Remove scheduler service dependency.");
  m_scheduler = nullptr;
}

void AxisProfileCmp::schedulerServiceChanged(common::scheduler::IScheduler2* /*scheduler*/)
{
  // Anything to do before adding/removing the scheduler?
}

void AxisProfileCmp::addPersistenceHelper(const PersistenceHelperItf* persistenceHelper)
{
  APR_TRACE_MESSAGE("Add persistence helper service dependency.");
  m_persistenceHelper = persistenceHelper;
}

void AxisProfileCmp::removePersistenceHelper(const PersistenceHelperItf* /*persistenceHelper*/)
{
  APR_TRACE_MESSAGE("Remove persistence helper service dependency.");
  m_persistenceHelper = nullptr;
}

void AxisProfileCmp::persistenceHelperServiceChanged(const PersistenceHelperItf* /*persistenceHelper*/)
{
  // Anything to do before adding/removing the persistence helper?
}

void AxisProfileCmp::addSystemState(::common::systemhandler::ISystemState2* systemState)
{
  APR_TRACE_MESSAGE("%s", "Add machine state service dependency.");

  if (nullptr == systemState)
  {
    APR_TRACE_MESSAGE("Interface 'ISystemState2' not available.");
    return;
  }

  comm::datalayer::Variant data;
  flatbuffers::FlatBufferBuilder builder;
  std::string componentName = "axis.profile";
  std::string displayName = "Motion-Axisprofile";

  // Current state of the component
  auto componentState = ::common::systemhandler::fbs::CreateState(builder, ::common::systemhandler::fbs::CurrentState_SERVICE);

  // Data Layer metadata of the component 
  flatbuffers::Offset<comm::datalayer::Reference> vecReferences[] = {comm::datalayer::CreateReferenceDirect(builder, "readType", "types/systemhandler/state")};
  auto metadata = comm::datalayer::CreateMetadata(builder,
                                                  comm::datalayer::NodeClass_Variable,
                                                  comm::datalayer::CreateAllowedOperations(builder, true, false, false, false, false),
                                                  builder.CreateString("provides axis profile configuration for ctrlX CORE systems"),
                                                  builder.CreateString(""),
                                                  builder.CreateString(displayName.c_str()),
                                                  comm::datalayer::DisplayFormat_Auto, 0, 0,
                                                  builder.CreateVectorOfSortedTables(vecReferences, 1), 0, 0);

  builder.Finish(::common::systemhandler::fbs::CreateComponentDirect(builder, componentName.c_str(), componentState, metadata));
  data.copyFlatbuffers(builder);

  m_systemStateComponent = systemState->registerComponent2(data);
}

void AxisProfileCmp::removeSystemState(::common::systemhandler::ISystemState2* /*systemState*/)
{
  APR_TRACE_MESSAGE("%s", "Remove machine state service dependency.");
  m_systemStateComponent.get()->unregister();
  m_systemStateComponent.reset();
}
