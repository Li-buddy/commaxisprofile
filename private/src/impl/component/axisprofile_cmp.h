#pragma once

/// @file
///
/// @brief This file is part of the axis profile library and declares the device profile component.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "background_thread/i_background_thread.h"
#include "common/debug_trace.h"
#include "common/mutex_helper.h"
#include "configuration/i_backup_restore_mgr.h"
#include "configuration/i_boot_param_backup_restore_mgr.h"
#include "diagnosis/dia.h"
#include "i_axisprofile_component.h"
#include "provider/provider_mgr.h"
#include "scheduler/axisprofile_callable_factory.h"
#include <memory>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

namespace comm
{
  namespace axisprofile
  {
    class IBootMgr;
    class IBackupRestoreMgr;
    class AxisProfileManager;
    class IAxisProfileManagerInternal;

    namespace state
    {
      class IStateMachine;
    }
  }
}

namespace common
{
  namespace systemhandler
  {
    class ISystemState2;
    class IComponent;
  }
}

class TestAxisProfileCmp;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------

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
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class AxisProfileCmp
    //! This class implements the axis profile component.
    class AxisProfileCmp : public IAxisProfileComponent
    {
    public:
      //! @brief constructor
      AxisProfileCmp() = default;
      //! @brief destructor
      virtual ~AxisProfileCmp() noexcept override;

      // ------------------------------------ External Interfaces --------------------------------------

      //! @brief This method returns a pointer to the device profile manager for external usage.
      //! @return Pointer to the axis profile manager
      IAxisProfileManager* getAxisProfileManagerInterface();

      // --------------------------------------- Celix Methods -----------------------------------------

      int init() override;
      int start() override;
      int stop() override;
      int deinit() override;

      // ---------------------------------- External celix Interface -----------------------------------

      AxisProfileResult createAxisProfile(const AxisProfileType type, const std::string& profileName) override;
      AxisProfileResult deleteAxisProfile(const std::string& profileName) override;
      AxisProfileResult connectAxisProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& profile) override;
      bool profileExists(const std::string& profileName) const override;
      std::shared_ptr<AxisprofileCallableFactory> getCallableFactory() const override {return m_dprCallableFactory;};
      std::uint32_t getConfiguredProfileNumber() const override;
      AxisProfileResult registerCheckCallback(const CheckCallBack& checkFct, const std::string& bundleName) override;
      AxisProfileResult unregisterAllCheckCallbacks(const std::string& bundleName) override;

      // ------------------------------------- Internal Interface --------------------------------------
      IAxisProfileManagerInternal* getAxisProfileManager() override;
      IStateMachine* getStateMachine() override;
      IBackupRestoreMgr* getBackupRestoreManager() override;
      const IBootParamBackupRestoreMgr* getBootParamBackupRestoreMgr() const override;
      comm::datalayer::IDataLayerFactory* getDataLayer() const final;
      const PersistenceHelperItf* getPersistenceHelper() const override;
      common::scheduler::IScheduler2* getScheduler() const final;
      common::systemhandler::IComponent* getSystemStateComponent() const final;
      IBootMgr* getBootManager() override;
      void setCycleTime(double cycleTime) final;
      double getCycleTime() const final;
      double getMaxBootingTimeout() const final;
      AxisProfileResult wakeupBkThread() const final;
      AxisProfileResult setBkThreadCycleTime(double cycleTime) const final;

      // ------------------------------------- Service Dependencies ------------------------------------

      void addDataLayer(comm::datalayer::IDataLayerFactory* datalayer) final;
      void removeDatalayer(comm::datalayer::IDataLayerFactory* datalayer) final;
      void datalayerServiceChanged(comm::datalayer::IDataLayerFactory* datalayer) final;
      void addTrace(common::log::trace::IRegistrationRealTime3* traceRegistration) final;
      void removeTrace(common::log::trace::IRegistrationRealTime3* traceRegistration) final;
      void traceServiceChanged(common::log::trace::IRegistrationRealTime3* traceRegistration) final;
      void addDiagnosis(common::log::diagnosis::IRegistration4* diagnosisRegistration) final;
      void removeDiagnosis(common::log::diagnosis::IRegistration4* diagnosisRegistration) final;
      void diagnosisServiceChanged(common::log::diagnosis::IRegistration4* diagnosisRegistration) final;
      void addScheduler(common::scheduler::IScheduler2* scheduler) final;
      void removeScheduler(common::scheduler::IScheduler2* scheduler) final;
      void schedulerServiceChanged(common::scheduler::IScheduler2* scheduler) final;
      void addPersistenceHelper(const PersistenceHelperItf* persistenceHelper) final;
      void removePersistenceHelper(const PersistenceHelperItf* persistenceHelper) final;
      void persistenceHelperServiceChanged(const PersistenceHelperItf* persistenceHelper) final;
      void addSystemState(::common::systemhandler::ISystemState2* systemState) final;
      void removeSystemState(::common::systemhandler::ISystemState2* systemState) final;

      // --------------------------------------- System Interface --------------------------------------
      const char *getAxisProfileState() const override;
      AxisProfileResult requestAxisProfileState(const char *stateName) override;
      AxisProfileResult getAxisProfileStateNames(std::vector<std::string> &stateNames) const override;

      // -------------------------------------- Bundle Information -------------------------------------

      void setBundlePath(const char* bundlePath) final { if (bundlePath) { m_bundlePath.assign(bundlePath); } else { m_bundlePath.clear(); } };
      void clrBundlePath(void) final { setBundlePath(nullptr); };
      const std::string &getBundlePath() const final { return m_bundlePath; };
      void setBundleName(const char* bundleName) final { if (bundleName) { m_bundleName.assign(bundleName); } else { m_bundleName.clear(); } };
      void clrBundleName(void) final { setBundleName(nullptr); };
      const std::string &getBundleName() const final { return m_bundleName; };

    protected: //data

      std::string m_bundlePath;      //!< bundle file path
      std::string m_bundleName;      //!< bundle name

    private: // data

      friend TestAxisProfileCmp;                                                              ///< for unit test
      DebugTrace m_debugTrace;                                                                ///< ServiceDependency - The debug trace for the whole component
      Dia m_diagnosis;                                                                        ///< ServiceDependency - The diagnosis for the whole component
      common::scheduler::IScheduler2* m_scheduler = nullptr;                                  ///< ServiceDependency - Pointer to the system scheduler
      comm::datalayer::IDataLayerFactory* m_datalayer = nullptr;                              ///< ServiceDependency - Pointer to datalayer
      const PersistenceHelperItf* m_persistenceHelper = nullptr;                              ///< ServiceDependency - Pointer to the persistence helper

      std::shared_ptr<AxisprofileCallableFactory> m_dprCallableFactory = nullptr;             ///< Shared pointer to the axis profile callable factory
      std::shared_ptr<::common::systemhandler::IComponent> m_systemStateComponent = nullptr;  ///< Shared pointer to the system state component
      IStateMachine* m_statemachine = nullptr;                                                ///< Pointer to global statemachine
      AxisProfileManager* m_axisProfileManager = nullptr;                                     ///< Pointer to axis profile manager
      IBootMgr* m_bootManager = nullptr;                                                      ///< Pointer to boot manager of the axis profile bundle
      std::unique_ptr<IBackupRestoreMgr> m_backupRestoreMgr;                                  ///< Pointer to the backup restore manager
      std::unique_ptr<IBootParamBackupRestoreMgr> m_bootParamBackupRestoreMgr;                ///< Pointer to boot parameter backup helper

      provider::ProviderMgr m_providerMgr;                                                    ///< provider manager for datalayer nodes
      MutexHelper m_mutexHelper{};                                                            ///< mutex helper for easy use of mutex

      double m_cycleTime { STD_CYCLE_TIME };                                                  ///< cycle time of system in seconds
      std::unique_ptr<IBackgroundThread> m_bkThread;                                          ///< Pointer against background thread
    };
  }
}
