/// @file
///
/// @brief This file is part of the axisprofile library and implements the boot manager.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "boot_mgr.h"
#include "comm/datalayer/datalayer.h"
#include "common/debug_trace.h"
#include "common/fpu_helper.h"
#include "common/systemhandler/i_systemstate.h"
#include "diagnosis/dia.h"
#include "fieldbuses/i_fieldbus_adapter.h"
#include "fieldbuses/i_fieldbus_master.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "profiles/i_axisprofile_internal.h"
#include <cfenv>

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

BootMgr::BootMgr()
{
  // The methods will be executed in the order they are added to the vector
  m_bootMethods.reserve(9);
  m_bootMethods.emplace_back("Loading system configuration",          "loadSystemCfg",                 [this]()->AxisProfileResult { return loadSystemCfg(); });
  m_bootMethods.emplace_back("Executing external checking",           "execExternalChecking",          [this]()->AxisProfileResult { return m_component->getAxisProfileManager()->execExternalChecking(); });
  m_bootMethods.emplace_back("collect fieldbus master information",   "collectAllDeviceInfo",          [this]()->AxisProfileResult { return m_component->getAxisProfileManager()->collectAllDeviceInfo(m_abortBooting); });
  m_bootMethods.emplace_back("check Consistency ",                    "checkConsistency",              [this]()->AxisProfileResult { return m_component->getAxisProfileManager()->checkConsistency(); });
  m_bootMethods.emplace_back("Checking fieldbus and device status",   "checkFieldbusAndDeviceStatus",  [this]()->AxisProfileResult { return checkFieldbusAndDeviceStatus(); });
  m_bootMethods.emplace_back("Initiating axisprofile manager",        "initManager",                   [this]()->AxisProfileResult { return m_component->getAxisProfileManager()->initManager(m_abortBooting, m_component->getCycleTime()); });
  m_bootMethods.emplace_back("Checking scheduler state",              "isSchedulerCallableInRunState", [this]()->AxisProfileResult { return isSchedulerCallableInRunState(); });
  m_bootMethods.emplace_back("Reset all profiles",                    "resetAllProfiles",              [this]()->AxisProfileResult { return resetAllProfiles(); });
  m_bootMethods.emplace_back("Reset all FPU error flags",             "resetAllFPUFlag()",             [this]()->AxisProfileResult { return m_component->getAxisProfileManager()->resetAllFPUFlag(); });

  // Callback: Enter Booting
  m_enterStateBootingCallback = [this](StateIDType srcStateID, StateIDType trgStateID)
  {
    APR_TRACE_MESSAGE("BootMgr::enterBootingCB() -> enter %s (from %s)", this->m_statemachine->getStateName(trgStateID), this->m_statemachine->getStateName(srcStateID));
    if ( StateIDType::CONFIG == srcStateID )
    {
      APR_TRACE_MESSAGE("BootMgr::enterBootingCB() --> wakeup booting thread");
      this->m_exitedConfigState = true;
      this->wakeup();
      this->m_component->wakeupBkThread();
    }
  };

  // Callback: Exit Booting
  m_exitStateBootingCallback = [this](StateIDType /*srcStateID*/, StateIDType trgStateID)
  {
    if ( !this->m_allowBootingExit.load() )
    {
      APR_TRACE_MESSAGE("BootMgr::exitBootingCB() -> state change from Booting to %s requested, but not allowed --> deny state change request", this->m_statemachine->getStateName(trgStateID));
      // this request is called not from the BootManager --> deny it
      return false;
    }

    APR_TRACE_MESSAGE("BootMgr::exitBootingCB() -> state change from Booting to %s requested and allowed --> notify system boot thread to finish startup", this->m_statemachine->getStateName(trgStateID));
    return true;
  };

  // Callback: Entering Config
  m_enterStateConfigCallback = [this](StateIDType srcStateID, StateIDType trgStateID)
  {
    APR_TRACE_MESSAGE("BootMgr::enterConfigCB() -> enter %s (from %s)", this->m_statemachine->getStateName(trgStateID), this->m_statemachine->getStateName(srcStateID));

    // set the machine state to "Setup"
    this->m_component->getSystemStateComponent()->setState(::common::systemhandler::ComponentState::COMPONENT_STATE_SETUP);
  };

  // Callback: Entering Running
  m_enterStateRunningCallback = [this](StateIDType srcStateID, StateIDType trgStateID)
  {
    APR_TRACE_MESSAGE("BootMgr::enterRunningCB() -> enter %s (from %s)", this->m_statemachine->getStateName(trgStateID), this->m_statemachine->getStateName(srcStateID));

    // set the machine state to "Operating"
    this->m_component->getSystemStateComponent()->setState(::common::systemhandler::ComponentState::COMPONENT_STATE_OPERATING);
  };
}

BootMgr::~BootMgr()
{
  stop();
  delete m_datalayerClient;
  m_datalayerClient = nullptr;
}

AxisProfileResult BootMgr::init(IAxisProfileComponent* component)
{
  APR_TRACE_MESSAGE("BootMgr::init()");
  if ( !component )
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_component = component;
  m_statemachine = m_component->getStateMachine();
  if ( !m_statemachine )
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_backupRestoreMgr = m_component->getBackupRestoreManager();
  if ( !m_backupRestoreMgr )
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  delete m_datalayerClient; m_datalayerClient = nullptr;
  m_datalayerClient = m_component->getDataLayer() ? m_component->getDataLayer()->createClient() : nullptr;
  if ( !m_datalayerClient )
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // Add statemachine callbacks
  if ( !m_statemachine->addCallback(StateIDType::BOOTING, m_enterStateBootingCallback) )
  {
    return AxisProfileResult::ERR_STATEMACHINE_ADD_CALLBACK;
  }

  if ( !m_statemachine->addCallback(StateIDType::BOOTING, m_exitStateBootingCallback) )
  {
    return AxisProfileResult::ERR_STATEMACHINE_ADD_CALLBACK;
  }

  if ( !m_statemachine->addCallback(StateIDType::CONFIG, m_enterStateConfigCallback) )
  {
    return AxisProfileResult::ERR_STATEMACHINE_ADD_CALLBACK;
  }

  if ( !m_statemachine->addCallback(StateIDType::RUNNING, m_enterStateRunningCallback) )
  {
    return AxisProfileResult::ERR_STATEMACHINE_ADD_CALLBACK;
  }

  APR_TRACE_MESSAGE("BootMgr::init() --> create boot thread");
  m_triggerBootThread.reset();

  // ToDo: Remove standard threads
#ifdef _MSC_VER
  std::uint64_t affinityMask = 0;
  AOX_THREAD_RESULT result = AOX_eThreadGetUsableCores(&affinityMask);
  if ( AOX_STATUS_FAILED(result) )
  {
    return AxisProfileResult::ERR_AOX_THREAD;
  }
  // ToDo: Thread parameters (priority, stack size and affinity) have to be reviewed!
  AOX_THREAD_FUNCTION_POINTER internalBootupLambda = [](void* bootObj)->std::uint32_t { return static_cast<std::uint32_t>( static_cast<BootMgr*>( bootObj )->internalBootup() ); };
  result = AOX_eThreadCreate(&m_thread, "axsprof.boot", internalBootupLambda, this, BOOT_MGR_THREAD_PRIORITY, STANDARD_THREAD_STACK_SIZE, affinityMask, 0);
  if ( AOX_STATUS_FAILED(result) )
  {
    return AxisProfileResult::ERR_AOX_THREAD;
  }
#else
  m_thread = new std::thread(&BootMgr::internalBootup, this);
  pthread_setname_np(m_thread->native_handle(), "axsprof.boot");
#endif

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BootMgr::bootSystem()
{
  APR_TRACE_MESSAGE("BootMgr::bootSystem()");
  if ( !m_statemachine )
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // Trigger the bootup-thread
  APR_TRACE_MESSAGE("BootMgr::bootSystem() --> wake up boot thread (return immediately, do not wait...)");
  wakeup();

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BootMgr::stop()
{
  AxisProfileResult aprResult = AxisProfileResult::STS_OK;

  m_stopBootThread = true;
  wakeup();

  if ( m_thread )
  {
    // ToDo: Remove standard threads
#ifdef _MSC_VER
    // ToDo: Review timeout and AOX_THREAD_INVALID_HANDLE return treatment
    AOX_THREAD_RESULT aoxResult = AOX_eThreadJoin(m_thread, 5000, nullptr);
    if ( AOX_STATUS_FAILED(aoxResult) )
    {
      aprResult = AxisProfileResult::ERR_AOX_THREAD;
    }
    else
    {
      aoxResult = AOX_eThreadDelete(m_thread);
      if ( AOX_STATUS_FAILED(aoxResult) )
      {
        aprResult = AxisProfileResult::ERR_AOX_THREAD;
      }
    }
#else
    m_thread->join();
    delete m_thread;
#endif

    m_thread = nullptr;
  }

  return aprResult;
}

void BootMgr::abortBooting()
{
  APR_TRACE_WARNING("Abort of the booting procedure has been requested!");
  m_abortBooting = true;
}

double BootMgr::getMaxBootingTimeout() const
{
  // a rough timeout can be calculated this way:
  // each profile read 40 parameters during booting (contain reset command);
  // each parameter reading need 4 ~ 10 fieldbus cycles;
  // maxTotalTimeout = 40 * 10 * profileNumber * cycleTime + waitTimeForFieldbusOp + waitTimeForSchedulerRun

  std::int32_t timeoutMs = 0;  // 2147483647‬ms at maximum

  timeoutMs += m_component->getAxisProfileManager()->getConfiguredProfileNumber() * TIMEOUT_READ_DEVICE_PRAMETERS;
  timeoutMs += TIMEOUT_SCHED_IN_RUN_STATE_MS + TIMEOUT_FIELDBUS_DEVICE_OP_MS + TIMEOUT_WAIT_BEFORE_DEVICE_OP;
  double timeout = MILLISECOND_2_SECOND(static_cast<double>(timeoutMs)) + 1;
  return timeout;
}

bool BootMgr::isInitBooting()
{
  return m_initialBootup;
}

///////////////////////////////////////////////////////////////////////////////////
// ****************************** PRIVATE METHODS ****************************** //

AxisProfileResult BootMgr::internalBootup()
{
  while ( !m_stopBootThread )
  {
    APR_TRACE_MESSAGE("BootMgr::internalBootup() --> wait for new trigger");
    m_triggerBootThread.wait();

    // cppcheck-suppress knownConditionTrueFalse ; False positive: the m_stopBootThread is set in an other thread
    if ( m_stopBootThread )
    {
      break;
    }

    // Do the bootup logic
    m_actBootStep = 0;
    bool initialBootup = m_initialBootup;
    bool goToRunning = true;
    m_abortBooting = false;
    APR_TRACE_MESSAGE("BootMgr::internalBootup() --> do %s...", initialBootup? "initial booting" : "normal booting");

    for ( auto& method : m_bootMethods )
    {
      AxisProfileResult res{};
      res = method.m_fnct();
      goToRunning = aprStatusFailed(res) ? false : true;

      // Special case: error diagnose will not present, but axisprofile state will not switch to running, switch back to CONFIG
      if ( initialBootup && res == AxisProfileResult::STS_NO_CONFIG_FILES_FOUND ) // no config file found
      {
        // bug 231947, could not find any config file during bootup
        goToRunning = false;
        m_actBootStep = BOOT_FAILED;
        APR_TRACE_MESSAGE("BootMgr::internalBootup() --> %s, could not find any configuration files during booting!", method.m_methodName.c_str());
        break;
      }
      else if ( res == AxisProfileResult::STS_BOOTING_ABORT ) // booting method request stopping booting
      {
        // bug 516579, any booting function requires BOOTING_ABORT
        goToRunning = false;
        m_actBootStep = BOOT_ABORT;
        APR_TRACE_MESSAGE("BootMgr::internalBootup() --> %s, require booting be aborted!", method.m_methodName.c_str());
        break;
      }

      APR_TRACE_MESSAGE("BootMgr::internalBootup() --> %s %s", method.m_methodName.c_str(), ( goToRunning ? "successful" : "failed" )); // bug 260608
      if ( !goToRunning )
      {
        // diagnosis should send inside on each booting method
        m_actBootStep = BOOT_FAILED;
        break;
      }
      ++m_actBootStep;
    }

    // when ready to go to running --> clear all FPU errors that might be in the system (because they are checked in RUNNING state)
    if ( goToRunning )
    {
      fpu::clearFPUFlags();   //Clear all FPU exceptions
    }

    bool finishedBooting = false;
    while ( !finishedBooting )
    {
      // prevent switches of the global state machine from BOOTING to another state, this may only this thread do
      m_allowBootingExit.store(true);
      GlobalStateResult res = m_statemachine->requestSwitchToState(goToRunning ? comm::axisprofile::StateIDType::RUNNING : comm::axisprofile::StateIDType::CONFIG);
      m_allowBootingExit.store(false);

      switch ( res )
      {
        case GlobalStateResult::ERR_STATE_CHANGE_ACTIVE:
          APR_TRACE_MESSAGE("BootMgr::internalBootup() --> request switch to state %s failed! There is still a state change active.", goToRunning ? "RUNNING" : "CONFIG");
          std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_SLEEP_GENERAL_MS));
          break;
        case GlobalStateResult::STS_OK:
          APR_TRACE_MESSAGE("BootMgr::internalBootup() --> switched to state %s successful", goToRunning ? "RUNNING" : "CONFIG");
          finishedBooting = true;
          break;
        default:
          APR_TRACE_MESSAGE("BootMgr::internalBootup() --> request switch to state %s failed! Current state is %s. Error code 0x%x", goToRunning ? "RUNNING" : "CONFIG",
                            m_statemachine->getStateName(m_statemachine->getState()), static_cast<std::uint32_t>( res ));
          finishedBooting = true;
          break;
      }
    }
  }

  return AxisProfileResult::STS_OK;
}

void BootMgr::wakeup()
{
  m_triggerBootThread.notify();
}

AxisProfileResult BootMgr::loadSystemCfg()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  if ( m_initialBootup )
  {
    // bugfix 231947
    result = m_backupRestoreMgr->loadSystemCfg(""); // Call without path --> Use default storage location

    // bug 516579, component startup abort after load config, following booting will only be triggered by scheduler
    if (AxisProfileResult::STS_OK == result)
    {
      APR_TRACE_MESSAGE("Everything is fine when loadSystemCfg(), aborting booting.");
      result =  AxisProfileResult::STS_BOOTING_ABORT;
    }
  }

  m_initialBootup = false;
  return result;
}

AxisProfileResult BootMgr::checkFieldbusAndDeviceStatus()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  // if no available profile exist, no need to check master
  std::vector<IAxisProfile*> activatedProfiles;
  m_component->getAxisProfileManager()->getActivatedProfiles(activatedProfiles);
  if (activatedProfiles.empty())
  {
    return AxisProfileResult::STS_OK;
  }

  // if profile existed, master is necessary
  ContainerFieldbusInst fieldbusInstances {};
  auto pProfileMgr = m_component->getAxisProfileManager();
  pProfileMgr->getFieldbusInstances(fieldbusInstances);
  if (fieldbusInstances.empty())
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_MASTER_NOT_EXIT,
                       __FILE__, __FUNCTION__, __LINE__, "No available fieldbus master exist");
    return AxisProfileResult::ERR_FIELDBUS_NO_MASTER;
  }

  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_MASTER_STATES,
                     __FILE__, __FUNCTION__, __LINE__, "");

  std::int32_t timeout = m_timeoutFieldBusDeviceOP;
  std::int32_t timeourErrorStatus = TIMEOUT_FIELDBUS_DEVICE_ERROR / POLLING_SLEEP_DEVICE_OP_MS;
  do
  {
    std::vector<AxisProfileResult> resultList;

    // abort booting by UI or scheduler
    if (m_abortBooting)
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_ABORTED,
                         __FILE__, __FUNCTION__, __LINE__,
                         "Booting is aborted by UI or datalayer!");
      return AxisProfileResult::STS_BOOTING_ABORT;
    }

    // abort booting when timeout
    if (--timeout <= 0)
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_ABORTED,
                         __FILE__, __FUNCTION__, __LINE__,
                         "Timeout waiting for fieldbus devices OP!");
      return AxisProfileResult::ERR_TIMEOUT;
    }

    bool masterNotInitialized {false};
    ContainerFieldbusInst::iterator masterIt;
    for (masterIt = fieldbusInstances.begin(); masterIt != fieldbusInstances.end();)
    {
      IFieldbusAdapter* fieldbusAdapter = nullptr;
      const auto& linkedProfiles = masterIt->second->getLinkedProfiles();
      // skip if current master not linked by any profile
      if (linkedProfiles.empty())
      {
        ++masterIt;
        continue;
      }
      for (const auto profile : linkedProfiles)
      {
        if (profile->checkProfileActivated())
        {
          fieldbusAdapter = profile->getFieldbusAdapter();
          break;
        }
      }

      result = checkFieldbusStatus(masterIt->first, fieldbusAdapter);
      if (result == AxisProfileResult::STS_FIELDBUS_NOT_INIT)
      {
        masterNotInitialized = true;
        break;
      }
      if (result == AxisProfileResult::STS_OK)
      {
        // already in OP, not check any more.
        masterIt = fieldbusInstances.erase(masterIt);
      }
      else
      {
        ++masterIt; // check next instance
      }
      resultList.push_back(result);
    }

    // master not initialized, polling again
    if (masterNotInitialized)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_SLEEP_DEVICE_OP_MS));
      continue;
    }

    for (auto slaveIt = activatedProfiles.begin(); slaveIt != activatedProfiles.end();)
    {
      IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(*slaveIt);
      result = checkDeviceStatus(profile->getProfileName());
      if (result == AxisProfileResult::STS_OK)
      {
        // already in OP, not check any more.
        slaveIt = activatedProfiles.erase(slaveIt);
      }
      else
      {
        ++slaveIt; // check next instance
      }
      resultList.push_back(result);
    }

    // all result should be STS_OK
    for ( auto it : resultList)
    {
      result = it;
      if (aprStatusFailed(result))
      {
        // workaround of bugfix563709, fieldbus error can be accepted with in 10 seconds
        timeourErrorStatus--;
        if (timeourErrorStatus == 0)
        {
          Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_ABORTED,
                             __FILE__, __FUNCTION__, __LINE__,
                             "Fieldbus error status for %d ms, booting abort!", TIMEOUT_FIELDBUS_DEVICE_ERROR);
          return result;
        }
        break;
      }
      else if (aprStatusNotOkay(result))
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_SLEEP_DEVICE_OP_MS));
        break; // polling again until all STS_OK
      }
    }

  }
  while (aprStatusNotOkay(result));

  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_FIELDBUS_MASTER_STATES_READY,
                     __FILE__, __FUNCTION__, __LINE__, "All motion devices in OP state");
  return result;
}

AxisProfileResult BootMgr::checkFieldbusStatus(const std::string& masterName, IFieldbusAdapter* adapter)
{
  if (!adapter)
  {
    APR_TRACE_ERROR("Master of %s is nullptr.", masterName.c_str());
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  MasterStatus status;
  AxisProfileResult result = adapter->readFieldbusMasterStatusSync(status.m_status);
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_MASTER_STATUS_ERROR,
                       __FILE__, __FUNCTION__, __LINE__,
                       "Read master status failed on %s.", masterName.c_str());
    return AxisProfileResult::ERR_FIELDBUS_READ_VALUE;
  }

  // already in OP
  if (!status.m_statusBits.NotInStateOp)
  {
    APR_TRACE_MESSAGE("Master of %s has in OP state.", masterName.c_str());
    return AxisProfileResult::STS_OK;
  }

  // not in Init, PreOP or SafeOP, that means master not has not initialized yet
  if (!status.m_statusBits.StateInit &&
      !status.m_statusBits.StatePreOp &&
      !status.m_statusBits.StateSafeOp)
  {
    return AxisProfileResult::STS_FIELDBUS_NOT_INIT;
  }

  if (status.m_statusBits.NoEthernetLink ||
      // status.m_statusBits.TopologyNotOK || bugfix563709, acceptable case: the slave is not used by motion
      // status.m_statusBits.SlavesError || bugfix563709, acceptable case: the slave is not used by motion
      status.m_statusBits.SettingsInconsistent ||
      status.m_statusBits.PortError ||
      status.m_statusBits.LicenseError)
  {
    MasterStatus tmpStatus = status;
    tmpStatus.m_statusBits.TopologyNotOK = false; // bugfix563709
    tmpStatus.m_statusBits.SlavesError = false;
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_MASTER_STATUS_ERROR,
                       __FILE__, __FUNCTION__, __LINE__,
                       "master %s status is %s, please check setting of the master.", masterName.c_str(), getMasterHighestErrorInfoString(tmpStatus));
    return AxisProfileResult::ERR_FIELDBUS;
  }

  return AxisProfileResult::STS_PENDING;
}

AxisProfileResult BootMgr::checkDeviceStatus(const std::string& profileName)
{
  IAxisProfile* tempProfile = m_component->getAxisProfileManager()->getProfileByName(profileName.c_str());
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(tempProfile);
  if (!profile)
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_PROFILE_NOT_FOUND,
                       __FILE__, __FUNCTION__, __LINE__, "Profile %s is not found.", profileName.c_str());
    return AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  }

  SlaveStatus status;
  ReadSlaveStateData readSlaveStateData {};
  readSlaveStateData.m_deviceAddress = profile->getPropertyInterface()->getDeviceAddress();
  AxisProfileResult result = profile->getFieldbusAdapter()->readSlaveStatusSync(readSlaveStateData, status.m_status);
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_SLAVE_STATUS_ERROR,
                       __FILE__, __FUNCTION__, __LINE__, "Read slave status failed on profile %s.", profileName.c_str());
    return AxisProfileResult::ERR_FIELDBUS_READ_VALUE;
  }

  if (status.m_statusBits.Error || status.m_statusBits.NotOnline)
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_SLAVE_STATUS_ERROR,
                       __FILE__, __FUNCTION__, __LINE__,
                       "slave status of profile %s is %s, check setting and connection of the slave.", profileName.c_str(), getSlaveHighestErrorInfoString(status));
    return AxisProfileResult::ERR_FIELDBUS;
  }

  if (status.m_statusBits.NotInStateOp) // wait until timeout
  {
    return AxisProfileResult::STS_PENDING;
  }
  APR_TRACE_MESSAGE("Device of %s has in OP state.", profileName.c_str());
  return AxisProfileResult::STS_OK;
}

AxisProfileResult BootMgr::isSchedulerCallableInRunState()
{
  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_SCHEDULER_IS_RUNNING, __FILE__, __FUNCTION__, __LINE__, "");
  if ( !m_component || !m_component->getCallableFactory() )
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  std::int32_t timeout = m_timeoutSchedRunState;
  while ( m_component->getCallableFactory()->getCallableState(CallableType::ADAPTION_CALLABLE) != common::scheduler::SchedState::SCHED_STATE_OPERATING )
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_SLEEP_GENERAL_MS));
    --timeout;
    if ( timeout < 0 )
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_BOOT_CALLABLE_NOT_READY, __FILE__, __FUNCTION__, __LINE__, 
                         "the control (including the Scheduler) must be switched to OPERATION");
      return AxisProfileResult::ERR_INVALID_STATE;
    }
  }
  return AxisProfileResult::STS_OK;
}

void BootMgr::getBootingState(std::uint32_t& actStep, std::uint32_t& maxSteps, std::string& txt) const
{
  maxSteps = static_cast<std::uint32_t>( m_bootMethods.size() );
  actStep = m_actBootStep;   // always work on the local copy to avoid race conditions
  if ( BOOT_FAILED == actStep )
  {
    actStep = 0;
    txt = "Booting failed";
    return;
  }

  if ( BOOT_ABORT == actStep )
  {
    actStep = 0;
    txt = "Booting aborted";
    return;
  }

  actStep = (std::uint32_t)( ( ( actStep ) < ( maxSteps ) ) ? ( actStep ) : ( maxSteps ) );

  // cppcheck-suppress knownConditionTrueFalse ; False positive: the actStep is depend on a pre-condition
  if ( actStep == maxSteps )
  {
    txt = "Booting finished";
    return;
  }
  txt = m_bootMethods[ actStep ].m_name;
}

AxisProfileResult BootMgr::resetAllProfiles()
{
  std::vector<std::string> profileNames;
  m_component->getAxisProfileManager()->getProfileNames(profileNames);

  for (const auto& profileName : profileNames)
  {
    auto profile = static_cast<IAxisProfileInternal*>(m_component->getAxisProfileManager()->getProfileByName(profileName.c_str()));
    auto result = profile->bootingReset();
    if (aprStatusFailed(result))
    {
      APR_TRACE_MESSAGE("Reset profile %s failed, ErrCode: %#010x.", profileName.c_str(), result);
      return AxisProfileResult::ERR_PROFILE_RESET_FAILED;
    }
  }

  return AxisProfileResult::STS_OK;
}
