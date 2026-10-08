#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the boot manager.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/semaphore.h"
#include "component/statemachine/i_state_machine.h"
#include "i_backup_restore_mgr.h"
#include "i_boot_mgr.h"
#include <atomic>

// ToDo: Remove standard threads
#ifdef _MSC_VER
// External Includes
#include "common/aox/AOX_Thread.h"
#else
#include <thread>
#endif

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm { namespace datalayer { class IClient; } };
namespace comm { namespace axisprofile {class IFieldbusAdapter; } };
class TestConfigurationBootManager;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr std::uint32_t BOOT_MGR_THREAD_PRIORITY      = 46;              ///< priority of the boot manager thread
  constexpr std::uint32_t STANDARD_THREAD_STACK_SIZE    = 0x20000;         ///< standard thread stack size

  constexpr std::int32_t POLLING_SLEEP_GENERAL_MS       = 2;               ///< sleep time when polling, for general methods, in milliseconds
#ifdef WIN32 // saving test time on windows
  constexpr std::int32_t POLLING_SLEEP_DEVICE_OP_MS     = 10;              ///< sleep time when polling, for fieldbus status checking, in milliseconds
  constexpr std::int32_t TIMEOUT_FIELDBUS_DEVICE_OP_MS  = 200;             ///< timeout value for waiting masters and devices switch to OP state, in milliseconds
  constexpr std::int32_t TIMEOUT_FIELDBUS_DEVICE_ERROR  = 100;             ///< timeout value for acceptable error state when waiting masters and devices to OP
#else // taken more time on real core
  constexpr std::int32_t POLLING_SLEEP_DEVICE_OP_MS     = 1000;            ///< sleep time when polling, for fieldbus status checking, in milliseconds
  constexpr std::int32_t TIMEOUT_FIELDBUS_DEVICE_OP_MS  = 100000;          ///< timeout value for waiting masters and devices switch to OP state, in milliseconds
  constexpr std::int32_t TIMEOUT_FIELDBUS_DEVICE_ERROR  = 10000;           ///< timeout value for acceptable error state when waiting masters and devices to OP
#endif
    
  constexpr std::int32_t TIMEOUT_SCHED_IN_RUN_STATE_MS  = 2000;            ///< timeout value for expecting scheduler/callable in RUN state, in milliseconds
  constexpr std::int32_t TIMEOUT_READ_DEVICE_PRAMETERS  = 40 * 10 * 2;     ///< timeout value for reading parameters from device, in milliseconds
  constexpr std::int32_t TIMEOUT_WAIT_BEFORE_DEVICE_OP  = 1000 * 60 * 2;   ///< timeout value before checking fieldbus, in milliseconds, 2 minutes same as EtherCAT master

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the class for the boot manager.
  class BootMgr : public IBootMgr
  {
  public: // methods

    /// @brief Constructor
    BootMgr();
    BootMgr(const BootMgr&) = delete;
    BootMgr& operator=(const BootMgr&) = delete;
    /// @brief Destructor
    virtual ~BootMgr() noexcept override;

    AxisProfileResult init(IAxisProfileComponent* component) final;
    AxisProfileResult bootSystem() final;
    AxisProfileResult stop() final;
    void abortBooting() final;
    void getBootingState(std::uint32_t& actStep, std::uint32_t& maxSteps, std::string& txt) const final;
    double getMaxBootingTimeout() const final;
    bool isInitBooting() final;

  private: // methods

    /// @brief This method triggers the boot thread in a loop until it is finished.
    /// @return Return value indicating success or error.
    AxisProfileResult internalBootup();

    /// @brief    Triggers the bootup thread.
    void wakeup();

    /// @brief This method calls the loadSystemCfg() method of the backup/restore manager.
    /// @return   Return value indicating success or error.
    AxisProfileResult loadSystemCfgAndDriveParams();

    /// @brief This method checks fieldbus and device status,
    /// @return   Return value indicating success or error.
    AxisProfileResult checkFieldbusAndDeviceStatus();

    /// @brief This method checks fieldbus status
    /// @param[in]     adapter      Field bus instance adapter
    /// @return   Return value indicating STS_OK for status check success, STS_PENGDING for still checking, ERR_xxx for error
    static AxisProfileResult checkFieldbusStatus(const std::string& masterName, IFieldbusAdapter* adapter);

    /// @brief This method checks device status
    /// @param[in]     profileName   Profile name
    /// @return   Return value indicating STS_OK for status check success, STS_PENGDING for still checking, ERR_xxx for error
    AxisProfileResult checkDeviceStatus(const std::string& profileName);

    /// @brief This method checks if scheduler is in running state.
    /// @return   Return value indicating success or error.
    AxisProfileResult isSchedulerCallableInRunState();

    /// @brief This method clear and reset all profile
    /// @return   Return value indicating success or error.
    AxisProfileResult resetAllProfiles();

    /// @brief This method loads boot parameters for all profiles.
    /// @return   Return value indicating success or error.
    AxisProfileResult loadProfilesBootParams();
    
  private: // types

    /// @brief Struct that stores function pointers of methods which need to be executed on every booting process
    struct BootMethod
    {
      const char *m_name{};                          ///< for user  monitor
      const std::string m_methodName{};              ///< for trace output
      std::function<AxisProfileResult()> m_fnct{};   ///< method to call

      /// @brief constructor
      /// @param[in]    name         method name
      /// @param[in]    methodName   method info (for trace)
      /// @param[in]    fnct         function pointer
      BootMethod(const char* name, const std::string& methodName, const std::function<AxisProfileResult()>& fnct)
        : m_name(name), m_methodName(methodName), m_fnct(fnct)
      {
      };
    };

  private: // data

    friend class ::TestConfigurationBootManager;       // Unittest of the BootMgr

    IAxisProfileComponent* m_component = nullptr;      ///< Pointer to the axisprofile component
    IBackupRestoreMgr* m_backupRestoreMgr = nullptr;   ///< Pointer to the backup/restore manager
    IStateMachine* m_statemachine = nullptr;           ///< Pointer to the axisprofile statemachine
    comm::datalayer::IClient* m_datalayerClient{};     ///< Pointer to the datalayer client of the boot manager
    bool m_exitedConfigState = false;                  ///< Keep the information, that the user switched from config-->booting
    bool m_stopBootThread = false;                     ///< Variable to abort the bootup thread.
    bool m_abortBooting = false;                       ///< Variable that is used to abort the booting procedure when it stuck
    bool m_initialBootup = true;                       ///< Variable shows if it is the initial bootup.
    Semaphore m_triggerBootThread;                     ///< Sleep and wait for bootup trigger.
    std::atomic_bool m_allowBootingExit{ false };      ///< may a change from booting state happen?
    std::uint32_t m_actBootStep{ 0 };                  ///< current boot step
    std::vector<BootMethod> m_bootMethods;             ///< All boot tasks to be done
    std::int32_t m_timeoutSchedRunState = { TIMEOUT_SCHED_IN_RUN_STATE_MS / POLLING_SLEEP_GENERAL_MS };
                                                        ///< timeout value for expecting scheduler/callable in RUN state
    std::int32_t m_timeoutFieldBusDeviceOP = { TIMEOUT_FIELDBUS_DEVICE_OP_MS / POLLING_SLEEP_DEVICE_OP_MS};
                                                        ///< timeout value for waiting masters and devices switch to OP state
    const std::uint32_t BOOT_FAILED = static_cast<std::uint32_t>(-1);    ///< some constant, that is put into the m_actBootStep when the booting failed
    const std::uint32_t BOOT_ABORT  = static_cast<std::uint32_t>(-2);    ///< some constant, that is put into the m_actBootStep when the booting is aborted

    // ------------------------------------ Callbacks --------------------------------------

    IStateMachine::CallbackEnter m_enterStateBootingCallback{};   ///< Callback, that is executed when global statemachine enters state 'Booting'
    IStateMachine::CallbackExit m_exitStateBootingCallback{};     ///< Callback, that is executed when global statemachine leaves state 'Booting'
    IStateMachine::CallbackEnter m_enterStateConfigCallback {};   ///< Callback, that is executed when global statemachine enters state 'Config'
    IStateMachine::CallbackEnter m_enterStateRunningCallback {};  ///< Callback, that is executed when global statemachine enters state 'Running'

  // ToDo: Remove standard threads
#ifdef _MSC_VER
    AOX_THREAD_HANDLE m_thread = nullptr;             ///< Handle of the bootup thread
#else
    std::thread *m_thread = nullptr;                  ///< Handle of the bootup thread
#endif
  };
}
