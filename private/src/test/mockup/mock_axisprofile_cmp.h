#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_axisprofile_cmp.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and declares a mockup
//! of the axisprofile component.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "axisprofile_internal_defs.h"
#include "component/i_axisprofile_component.h"
#include "scheduler/axisprofile_callable_factory.h"
// Mockups
#include "mockup/mock_statemachine.h"
#include "mockup/mock_axisprofile_manager.h"
#include "mockup/mock_backup_restore_mgr.h"
#include "mockup/mock_boot_manager.h"
#include "mockup/mock_provider_mgr.h"
#include "mockup/mock_background_thread.h"
#include "comm/datalayer/persistence_helper.h"
// External Includes
#ifndef _MSC_VER
#include "celix/utils/celix_errno.h"
#else
#include "celix_errno.h"
#endif


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockAxisProfileCmp
  //! Mockup of the axisprofile component.
  class MockAxisProfileCmp : public comm::axisprofile::IAxisProfileComponent
  {
  protected: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   //!< Result of member functions

  private: // data

    std::string m_bundlePath = "TEST_BundlePath";   //!< Bundle Path
    std::string m_bundleName = "TEST_BundleName";   //!< Bundle Name
    comm::axisprofile::IStateMachine* m_mockStatemachine{};                           //!< Mockup of the global state machine
    comm::axisprofile::IAxisProfileManagerInternal* m_mockAxisProfileManager{};       //!< Mockup of the axis profile manager
    comm::axisprofile::provider::ProviderMgr* m_mockProviderManager{};                //!< Mockup of the provider manager
    comm::axisprofile::IBackupRestoreMgr* m_mockBackupRestoreMgr{};                   //!< Mockup of the backup/restore manager
    comm::axisprofile::IBootMgr* m_mockBootMgr{};                                     //!< Mockup of the boot manager
    comm::axisprofile::IBackgroundThread* m_mockBkThread {};                          //!< Mockup of the background thread
    comm::datalayer::IDataLayerFactory* m_mockDatalayer{};                            //!< Mockup of the datalayer
    common::scheduler::IScheduler2* m_mockScheduler{};                                //!< Mockup of the scheduler
    comm::datalayer::PersistenceHelper* m_persistenceHelper{};                        //!< Persistence helper used in test
    double m_cycleTime { comm::axisprofile::STD_CYCLE_TIME };                                            //!< Cycle time of system in seconds
    std::shared_ptr<comm::axisprofile::AxisprofileCallableFactory> m_mockDprCallableFactory = nullptr;   //!< Mockup of shared pointer to the axis profile callable factory
    std::shared_ptr<::common::systemhandler::IComponent> m_mockSystemStateComponent = nullptr;           //!< Shared pointer for the system state

  public: // methods

    //! @brief constructor
    MockAxisProfileCmp()
    {
      m_mockStatemachine = new test::MockStateMachine();
      m_mockAxisProfileManager = new test::MockAxisProfileManager();
      m_mockBackupRestoreMgr = new test::MockBackupRestoreManager();
      m_mockProviderManager = new test::MockProviderMgr();
      m_mockProviderManager->init(this);
      m_mockBootMgr = new test::MockBootManager();
      m_mockBkThread = new test::MockBackgroundThread();
    }
    //! @brief destructor
    virtual ~MockAxisProfileCmp()
    {
      if (m_mockStatemachine) delete m_mockStatemachine;
      if (m_mockAxisProfileManager) delete m_mockAxisProfileManager;
      if (m_mockBackupRestoreMgr) delete m_mockBackupRestoreMgr;
      if (m_mockProviderManager) delete m_mockProviderManager;
      if (m_mockBootMgr) delete m_mockBootMgr;
      if (m_mockBkThread) delete m_mockBkThread;
    }

    comm::axisprofile::IAxisProfileManagerInternal* getAxisProfileManager() override { return m_mockAxisProfileManager; }
    comm::axisprofile::IStateMachine* getStateMachine() override { return m_mockStatemachine; }
    comm::axisprofile::IBackupRestoreMgr* getBackupRestoreManager() override { return m_mockBackupRestoreMgr; }
    comm::datalayer::IDataLayerFactory* getDataLayer() const override { return m_mockDatalayer; }
    const PersistenceHelperItf* getPersistenceHelper() const override { return m_persistenceHelper->getCInterface(); }
    common::scheduler::IScheduler2* getScheduler() const override { return m_mockScheduler; }
    common::systemhandler::IComponent* getSystemStateComponent() const override { return m_mockSystemStateComponent.get(); }
    comm::axisprofile::IBootMgr* getBootManager() override { return m_mockBootMgr; }
    void setCycleTime(double cycleTime) override { m_cycleTime = cycleTime; }
    double getCycleTime() const override { return m_cycleTime; }
    double getMaxBootingTimeout() const override { return m_mockBootMgr->getMaxBootingTimeout(); }
    comm::axisprofile::AxisProfileResult wakeupBkThread() const override { return m_returnValue; };
    comm::axisprofile::AxisProfileResult setBkThreadCycleTime(double /*cycleTimeUs*/) const override { return m_returnValue; };

    // --------------------------------------- Celix Methods -----------------------------------------

    celix_status_t init() override { return CELIX_SUCCESS; }
    celix_status_t start() override { return CELIX_SUCCESS; }
    celix_status_t stop() override { return CELIX_SUCCESS; }
    celix_status_t deinit() override { return CELIX_SUCCESS; }

    // ---------------------------------- External celix Interface -----------------------------------

    comm::axisprofile::AxisProfileResult createAxisProfile(const comm::axisprofile::AxisProfileType type, const std::string& profileName) override {
      if (m_mockAxisProfileManager) { m_mockAxisProfileManager->createAxisProfile(type, profileName); } return m_returnValue;
    }
    comm::axisprofile::AxisProfileResult deleteAxisProfile(const std::string& profileName) override {
      if (m_mockAxisProfileManager) { m_mockAxisProfileManager->deleteAxisProfile(profileName); } return m_returnValue;
    }
    comm::axisprofile::AxisProfileResult connectAxisProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& profile) override {
      if (m_mockAxisProfileManager) { m_mockAxisProfileManager->connectAxisProfile(profileName, profile); } return m_returnValue;
    }
    bool profileExists(const std::string& profileName) const override {
      return (m_mockAxisProfileManager) ? m_mockAxisProfileManager->profileExists(profileName) : false;
    }
    std::uint32_t getConfiguredProfileNumber() const override {
      return (m_mockAxisProfileManager) ? m_mockAxisProfileManager->getConfiguredProfileNumber() : 0;
    }

    AxisProfileResult registerCheckCallback(const CheckCallBack& checkFct, const std::string& bundleName) override
    {
      return m_mockAxisProfileManager ? m_mockAxisProfileManager->registerCheckCallback(checkFct, bundleName) : AxisProfileResult::ERR_EXEC_NULL;
    }

    AxisProfileResult unregisterAllCheckCallbacks(const std::string& bundleName) override
    {
      return m_mockAxisProfileManager ? m_mockAxisProfileManager->unregisterAllCheckCallbacks(bundleName) : AxisProfileResult::ERR_EXEC_NULL;
    }

    // ------------------------------------- Service Dependencies ------------------------------------

    void addDataLayer(comm::datalayer::IDataLayerFactory* datalayer) override { m_mockDatalayer = datalayer; }
    void removeDatalayer(comm::datalayer::IDataLayerFactory* /*datalayer*/) override { m_mockDatalayer = nullptr; }
    void datalayerServiceChanged(comm::datalayer::IDataLayerFactory* /*datalayer*/) override { }
    void addTrace(common::log::trace::IRegistrationRealTime3* /*traceRegistration*/) override { }
    void removeTrace(common::log::trace::IRegistrationRealTime3* /*traceRegistration*/) override { }
    void traceServiceChanged(common::log::trace::IRegistrationRealTime3* /*traceRegistration*/) override { }
    void addDiagnosis(common::log::diagnosis::IRegistration4* /*diagnosisRegistration*/) override { }
    void removeDiagnosis(common::log::diagnosis::IRegistration4* /*diagnosisRegistration*/) override { }
    void diagnosisServiceChanged(common::log::diagnosis::IRegistration4* /*diagnosisRegistration*/) override { }
    void addScheduler(common::scheduler::IScheduler2* scheduler) override { m_mockScheduler = scheduler; }
    void removeScheduler(common::scheduler::IScheduler2* /*scheduler*/) override { m_mockScheduler = nullptr; }
    void schedulerServiceChanged(common::scheduler::IScheduler2* /*scheduler*/) override { }
    void addPersistenceHelper(const PersistenceHelperItf* /*persistenceHelper*/) override { }
    void removePersistenceHelper(const PersistenceHelperItf* /*persistenceHelper*/) override { }
    void persistenceHelperServiceChanged(const PersistenceHelperItf* /*persistenceHelper*/) override { }
    void addSystemState(::common::systemhandler::ISystemState2* /*systemState*/) override { }
    void removeSystemState(::common::systemhandler::ISystemState2* /*systemState*/) override { }

    // --------------------------------------- System Interface --------------------------------------

    const char *getAxisProfileState() const override { return m_mockStatemachine ? m_mockStatemachine->getStateName(m_mockStatemachine->getState()) : nullptr; }
    comm::axisprofile::AxisProfileResult requestAxisProfileState(const char * /*stateName*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult getAxisProfileStateNames(std::vector<std::string> &/*stateNames*/) const override { return m_returnValue; }
    std::shared_ptr<comm::axisprofile::AxisprofileCallableFactory> getCallableFactory() const override {return m_mockDprCallableFactory;};

    // -------------------------------------- Bundle Information -------------------------------------

    void setBundlePath(const char* /*bundlePath*/) override { }
    void clrBundlePath(void) override { };
    const std::string &getBundlePath() const override { return m_bundlePath; }
    void setBundleName(const char* /*bundleName*/) override { }
    void clrBundleName(void) override { }
    const std::string &getBundleName() const override { return m_bundleName; }

    // ----------------------------------------- Test methods ----------------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

    //! @brief This method returns a pointer to the provider manager mockup.
    //! @return Pointer to the provider manager.
    comm::axisprofile::provider::ProviderMgr* getProviderManager() { return m_mockProviderManager; }

    //! @brief This method returns a pointer to the persistence helper Cpp interface.
    //! @return Pointer to the persistence helper cpp interface.
    comm::datalayer::PersistenceHelper* getPersistenceHelperCppItf() { return m_persistenceHelper; }

    //! @brief This method returns a pointer to the persistence helper Cpp interface.
//! @return Pointer to the persistence helper cpp interface.
    void setPersistenceHelper(comm::datalayer::PersistenceHelper* persistenceHelper) { m_persistenceHelper = persistenceHelper; }

    //! @brief This method deletes the existing statemachine, creates a new one, or registers a statemachine that is passed to the function.
    //! @param[in]   deleteOrCreate   If false, the existing statemachine will be deleted. If true, a new statemachine mockup will be created OR
    //!                               if a statemachine was passed to the function this one will be set.
    //! @param[in]   statemachine     Optional. Pointer to the statemachine instance that should be used.
    void setStatemachine(bool deleteOrCreate, comm::axisprofile::IStateMachine* statemachine = nullptr)
    {
      if (m_mockStatemachine) { delete m_mockStatemachine; m_mockStatemachine = nullptr; }
      if (deleteOrCreate) { m_mockStatemachine = statemachine ? statemachine : new test::MockStateMachine(); }
    }

    //! @brief This method deletes the existing profile manager, creates a new one, or registers a profile manager that is passed to the function.
    //! @param[in]   deleteOrCreate   If false, the existing profile manager will be deleted. If true, a new profile manager mockup will be created OR
    //!                               if a profile manager was passed to the function this one will be set.
    //! @param[in]   profileMgr     Optional. Pointer to the profile manager instance that should be used.
    void setProfileManager(bool deleteOrCreate, comm::axisprofile::IAxisProfileManagerInternal* profileMgr = nullptr)
    {
      if (m_mockAxisProfileManager) { delete m_mockAxisProfileManager; m_mockAxisProfileManager = nullptr; }
      if (deleteOrCreate) { m_mockAxisProfileManager = profileMgr ? profileMgr : new test::MockAxisProfileManager(); }
    }

    //! @brief This method deletes the existing provider manager, creates a new one, or registers a provider manager that is passed to the function.
    //! @param[in]   deleteOrCreate   If false, the existing provider manager will be deleted. If true, a new provider manager mockup will be created OR
    //!                               if a provider manager was passed to the function this one will be set.
    //! @param[in]   providerMgr      Optional. Pointer to the provider manager instance that should be used.
    void setProviderManager(bool deleteOrCreate, comm::axisprofile::provider::ProviderMgr* providerMgr = nullptr)
    {
      if (m_mockProviderManager) { delete m_mockProviderManager; m_mockProviderManager = nullptr; }
      if (deleteOrCreate) { m_mockProviderManager = providerMgr ? providerMgr : new test::MockProviderMgr(); m_mockProviderManager->init(this); }
    }

    //! @brief This method deletes the existing provider manager, creates a new one, or registers a provider manager that is passed to the function.
    //! @param[in]   deleteOrCreate     If false, the existing provider manager will be deleted. If true, a new provider manager mockup will be created OR
    //!                                 if a provider manager was passed to the function this one will be set.
    //! @param[in]   backupRestoreMgr   Optional. Pointer to the backup-restore manager instance that should be used.
    void setBackupRestoreManager(bool deleteOrCreate, comm::axisprofile::IBackupRestoreMgr* backupRestoreMgr = nullptr)
    {
      if ( m_mockBackupRestoreMgr ) { delete m_mockBackupRestoreMgr; m_mockBackupRestoreMgr = nullptr; }
      if ( deleteOrCreate ) { m_mockBackupRestoreMgr = backupRestoreMgr ? backupRestoreMgr : new test::MockBackupRestoreManager(); }
    }

    //! @brief This method deletes the existing boot manager, creates a new one, or registers a boot manager that is passed to the function.
    //! @param[in]   deleteOrCreate     If false, the existing boot manager will be deleted. If true, a new boot manager mockup will be created OR
    //!                                 if a boot manager was passed to the function this one will be set.
    //! @param[in]   bootMgr            Optional. Pointer to the boot manager instance that should be used.
    void setBootManager(bool deleteOrCreate, comm::axisprofile::IBootMgr* bootMgr = nullptr)
    {
      if (m_mockBootMgr) { delete m_mockBootMgr; m_mockBootMgr = nullptr; }
      if (deleteOrCreate) { m_mockBootMgr = bootMgr ? bootMgr : new test::MockBootManager(); }
    }
    //! @brief This method set CallableFactory, creates a new one, or registers a boot manager that is passed to the function.
    //! @param[in]   mockDprCallableFactory     callable factory to be set
    void setCallableFactory(std::shared_ptr<comm::axisprofile::AxisprofileCallableFactory> mockDprCallableFactory)
    {
      m_mockDprCallableFactory = mockDprCallableFactory;
    }
    //! @brief This method set system state component.
    //! @param[in]   mockSystemStateCmp     system state component to be set
    void setSystemStateCmp(std::shared_ptr<::common::systemhandler::IComponent> mockSystemStateCmp)
    {
      m_mockSystemStateComponent = mockSystemStateCmp;
    }

    //! @brief This method deletes the existing background thread, creates a new one, or registers a background thread that is passed to the function.
    //! @param[in]   deleteOrCreate     If false, the existing one will be deleted. If true, a new mockup will be created OR
    //!                                 if a background thread was passed to the function this one will be set.
    //! @param[in]   bootMgr            Optional. Pointer to the background thread instance that should be used.
    void setBackgroundThread(bool deleteOrCreate, comm::axisprofile::IBackgroundThread* bkThread = nullptr)
    {
      if (m_mockBkThread) { delete m_mockBkThread; m_mockBkThread = nullptr; }
      if ( deleteOrCreate ) { m_mockBkThread = bkThread ? bkThread : new test::MockBackgroundThread(); }
    }
  };
}
