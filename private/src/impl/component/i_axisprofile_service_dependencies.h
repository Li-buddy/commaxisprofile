#pragma once

/// @file
///
/// @brief This file is part of the axis profile library and declares the interface
///        to set/remove service dependencies to the axis profile component.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.
///


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------


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
    class ISystemState2;
  }
  namespace log
  {
    namespace trace
    {
      class IRegistrationRealTime3;
    }
    namespace diagnosis
    {
      class IRegistration4;
    }
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
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class IAxisProfileServiceDependencies
    //! This is the interface to set/remove every celix service dependency to/from the axisprofile
    class IAxisProfileServiceDependencies
    {
    public: // methods
      virtual ~IAxisProfileServiceDependencies() noexcept = default;

      //! @brief Add the datalayer to the axis profile component.
      //! @param  datalayer  Pointer to the datalayer
      virtual void addDataLayer(comm::datalayer::IDataLayerFactory* datalayer) = 0;

      //! @brief Remove the datalayer from the axis profile component.
      //! @param[in]  datalayer  Pointer to the datalayer
      virtual void removeDatalayer(comm::datalayer::IDataLayerFactory* datalayer) = 0;

      //! @brief Datalayer service dependency changed. Will be called before addDatalayer() or removeDatalayer().
      //! @param[in]  datalayer  Pointer to the datalayer
      virtual void datalayerServiceChanged(comm::datalayer::IDataLayerFactory* datalayer) = 0;

      //! @brief Add the trace to the axis profile component.
      //! @param[in]  traceRegistration  Pointer to the trace
      virtual void addTrace(common::log::trace::IRegistrationRealTime3* traceRegistration) = 0;

      //! @brief Remove the trace from the axis profile component.
      //! @param[in]  traceRegistration  Pointer to the trace
      virtual void removeTrace(common::log::trace::IRegistrationRealTime3* traceRegistration) = 0;

      //! @brief Trace service dependency changed. Will be called before addTrace() or removeTrace().
      //! @param[in]  traceRegistration  Pointer to the trace
      virtual void traceServiceChanged(common::log::trace::IRegistrationRealTime3* traceRegistration) = 0;

      //! @brief Add the diagnosis to the axis profile component.
      //! @param[in]  diagnosisRegistration  Pointer to the diagnosis registration interface
      virtual void addDiagnosis(common::log::diagnosis::IRegistration4* diagnosisRegistration) = 0;

      //! @brief Remove the diagnosis from the axis profile component.
      //! @param[in]  diagnosisRegistration  Pointer to the diagnosis registration interface
      virtual void removeDiagnosis(common::log::diagnosis::IRegistration4* diagnosisRegistration) = 0;

      //! @brief Diagnosis service dependency changed. Will be called before addDiagnosis() or removeDiagnosis().
      //! @param[in]  diagnosisRegistration  Pointer to the diagnosis registration interface
      virtual void diagnosisServiceChanged(common::log::diagnosis::IRegistration4* diagnosisRegistration) = 0;

      //! @brief Add the scheduler to the axis profile component.
      //! @param[in]  scheduler  Pointer to the scheduler
      virtual void addScheduler(common::scheduler::IScheduler2* scheduler) = 0;

      //! @brief Remove the scheduler from the axis profile component.
      //! @param[in]  scheduler  Pointer to the scheduler
      virtual void removeScheduler(common::scheduler::IScheduler2* scheduler) = 0;

      //! @brief Scheduler service dependency changed. Will be called before addScheduler() or removeScheduler().
      //! @param[in]  scheduler  Pointer to the scheduler
      virtual void schedulerServiceChanged(common::scheduler::IScheduler2* scheduler) = 0;

      //! @brief Add the persistence helper to the axis profile component.
      //! @param[in]  persistenceHelper  Pointer to the persistence helper
      virtual void addPersistenceHelper(const PersistenceHelperItf* persistenceHelper) = 0;

      //! @brief Remove the persistence helper from the axis profile component.
      //! @param[in]  persistenceHelper  Pointer to the persistence helper
      virtual void removePersistenceHelper(const PersistenceHelperItf* persistenceHelper) = 0;

      //! @brief Persistence helper service dependency changed. Will be called before addPersistenceHelper() or removePersistenceHelper().
      //! @param[in]  persistenceHelper  Pointer to the persistence helper
      virtual void persistenceHelperServiceChanged(const PersistenceHelperItf* persistenceHelper) = 0;

      //! @brief Set the system state pointer known in the motion system component
      //! @param[in]  systemState  new pointer to the system state registration
      virtual void addSystemState(::common::systemhandler::ISystemState2* systemState) = 0;

      //! @brief Remove the system state pointer known in the motion system component
      //! @param[in]  systemState  new pointer to the system state registration
      virtual void removeSystemState(::common::systemhandler::ISystemState2* systemState) = 0;
    };
  }
}
