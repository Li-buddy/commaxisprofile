#pragma once

// ---------------------------------------------------------------------------
//
//! @file       i_axisprofile_component.h
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
//! This file is part of the axisprofile library and declares the main interface
//! for the whole axisprofile component.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/interfaces/i_axisprofile_manager.h"
#include "comm/axisprofile/interfaces/i_axisprofile_system.h"
#include "component/i_axisprofile_service_dependencies.h"
#include "scheduler/axisprofile_callable_factory.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct PersistenceHelperItf;

namespace comm
{
  namespace datalayer
  {
    class IDataLayerFactory;
  }
}
namespace common
{
  namespace scheduler
  {
    class IScheduler2;
  }
  namespace systemhandler
  {
    class IComponent;
  }
}


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
    class IAxisProfileManagerInternal;
    class IBackupRestoreMgr;
    class IBootMgr;
    class IStateMachine;

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

    //! @class IAxisProfileComponent
    //! This is the main interface for the whole axisprofile component.
    class IAxisProfileComponent : public IAxisProfileManager, public IAxisProfileServiceDependencies, public IAxisProfileSystem
    {
    public: // methods
      virtual ~IAxisProfileComponent() noexcept override = default;

      //! @brief This method returns a pointer to the internal axisprofile manager interface.
      //! @return Pointer to the internal axisprofile manager interface
      virtual IAxisProfileManagerInternal* getAxisProfileManager() = 0;

      //! @brief This method returns a pointer to the axisprofile state machine.
      //! @return Pointer to the axisprofile state machine
      virtual IStateMachine* getStateMachine() = 0;

      //! @brief This method returns a pointer to the backup/restore manager.
      //! @return Pointer to the backup/restore manager
      virtual IBackupRestoreMgr* getBackupRestoreManager() = 0;

      //! @brief This method returns a pointer to the boot manager.
      //! @return Pointer to the boot manager
      virtual IBootMgr* getBootManager() = 0;

      //! @brief Get the datalayer pointer from the axis profile component
      //! @return  pointer to the datalayer
      virtual comm::datalayer::IDataLayerFactory* getDataLayer() const = 0;

      //! @brief Get the datalayer persistence helper pointer from the axis profile component
      //! @return  pointer to the datalayer persistence helper
      virtual const PersistenceHelperItf* getPersistenceHelper() const = 0;

      //! @brief Get the scheduler pointer from the axis profile component
      //! @return  pointer to the scheduler
      virtual common::scheduler::IScheduler2* getScheduler() const = 0;

      // --------------------------------------- Celix Methods -----------------------------------------

      //! This method is used for the life cycle management of this component.
      //! It is called during start of the bundle, after 'Activator::init'.
      //! @note This function will not be called in context of 'Activator::init'.
      virtual int init() = 0;

      //! This method is used for the life cycle management of this component.
      //! It is called during start of the bundle, after 'Component::init'.
      //! @note This function will not be called in context of 'Activator::init'.
      virtual int start() = 0;

      //! This method is used for the life cycle management of this component.
      //! It is called during stop of the bundle, after 'Activator::deinit'.
      //! @note This function will not be called in context of 'Activator::deinit'.
      virtual int stop() = 0;

      //! This method is used for the life cycle management of this component.
      //! It is called during stop of the bundle, after 'Component::stop'.
      //! @note This function will not be called in context of 'Activator::deinit'.
      virtual int deinit() = 0;

      // -------------------------------------- Bundle Information -------------------------------------

      //! @brief This method is used to set root path of bundle from activator, the path is
      //! e.g. needed for finding files, loading additional shared objects etc..
      virtual void setBundlePath(const char* bundlePath) = 0;

      //! @brief This method is used to clear root path of bundle.
      virtual void clrBundlePath(void) = 0;

      //! @brief Get the bundle path
      //! @return  the bundle path
      virtual const std::string &getBundlePath() const = 0;

      //! @brief This method is used to set name of bundle from activator, the name is
      //! e.g. needed for messages etc..
      virtual void setBundleName(const char* bundleName) = 0;

      //! @brief This method is used to clear name of bundle.
      virtual void clrBundleName(void) = 0;

      //! @brief Get the bundle name
      //! @return  the bundle name
      virtual const std::string &getBundleName() const = 0;

      //! @brief Get the callable factory
      //! @return  the callable factory
      virtual std::shared_ptr<AxisprofileCallableFactory> getCallableFactory() const = 0;

      //! @brief Get the system state component from the axis profile component
      //! @return  pointer to the system state component
      virtual common::systemhandler::IComponent* getSystemStateComponent() const = 0;

      /// @brief Set new APR system cycle time in seconds
      /// @param[in]   cycleTime     new cycle time
      virtual void setCycleTime(double cycleTime) = 0;

      /// @brief Get cycle time in second
      /// @return acutal cycle time of APR system
      virtual double getCycleTime() const = 0;

      /// @brief Wakeup background thread
      /// @return was everything okay?
      virtual AxisProfileResult wakeupBkThread() const = 0;

      /// @brief Set background thread polling cycle time in second
      /// @param[in] cycleTime  Cycle time in second
      /// @return Return value indicating success or error
      virtual AxisProfileResult setBkThreadCycleTime(double cycleTime) const = 0;
    };
  }
}
