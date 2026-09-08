#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the interface of the job manager of axisprofile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "job_object/job_def.h"
#include "profiles/i_axisprofile_sharing.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class IContainer;
  class IJobObj;

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  
  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for the job manager of axisprofile.
  class IJobMgr : public IAxisProfileSharing
  {
  public: // methods

    /// @brief Destructor
    virtual ~IJobMgr() noexcept override = default;

    /// @brief This method registers a job, which can be executed in background thread.
    /// @param[in]  id            Job Id
    /// @param[in]  obj           Unique pointer to job object
    /// @return Return value indicating success or error
    virtual AxisProfileResult registerJob(std::unique_ptr<IJobObj>& obj) = 0;

    /// @brief This method unregisters all job.
    /// @return Return value indicating success or error
    virtual void unregisterAllJobs() = 0;

    /// @brief This method gets job object by id
    /// @param[in] id             Job object id
    /// @return Job object, nullptr if not found
    virtual IJobObj* getJobObj(JobIDType id) = 0;

    /// @brief This method set job object to new state
    /// @param[in]  id        Job object type Id
    /// @param[in]  newReq    New state request
    /// @param[in]  arg       Argument to set to the job object
    /// @return Return value indicating success or error
    virtual AxisProfileResult sendJobCmd(JobIDType id, CmdReq newReq, IContainer* arg = nullptr) = 0;

    /// @brief This method get state of job object 
    /// @param[in]  id        Job object type Id
    /// @return State of job object 
    virtual CmdState getJobState(JobIDType id) = 0;

    /// @brief This method execute all registered job object, be called in a NRT thread (background thread)
    /// @return Return value indicating success or error
    virtual void runAllJobs() = 0;
  };
}
