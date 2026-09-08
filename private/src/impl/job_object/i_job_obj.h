#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the interface of job object interface.
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
#include "job_def.h"

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

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is interface of axisprofile job object
  class IJobObj
  {
  public: // methods

    /// @brief Destructor
    virtual ~IJobObj() noexcept = default;
    
    /// @brief This method gets job object ID
    /// @return Name of the job object
    virtual JobIDType getID() = 0;

    /// @brief This method sets job object to new state
    /// 
    /// @Note The method can be called in any thread to request state of job object. 
    /// 
    /// @param[in]  newReq    New state request
    /// @param[out] infoTxt   Information text, can be message or error text
    /// @param[in]  arg       Arguments to be set
    /// @return Return value indicating success or error.
    virtual AxisProfileResult setNewState(JobReq newReq, std::string& infoTxt, IContainer* arg = nullptr) = 0;

    /// @brief This method gets actual state of the job object, the return state is atomic
    /// @return Actual state of job object
    virtual JobState getState() = 0;

    /// @brief Main execution cycle.
    ///
    /// @note The method is only called in a NRT thread (background thread).
    /// @return Return value indicating success or error.
    virtual AxisProfileResult mainCycle() = 0;

    /// @brief This method get data of the job.
    /// @return Pointer to job data
    virtual const IContainer* getJobData() = 0;

    /// @brief This method get data of the job.
    /// @return Pointer to job data
    virtual IContainer* getJobDataRef() = 0;
  };
}
