#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the header of the job manager of axisprofile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_job_mgr.h"
#include "job_object/i_job_obj.h"
#include "profiles/profile_defs.h"
#include <map>
#include <memory>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
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

  /// @brief This is the job manager of axisprofile.
  class JobMgr : public IJobMgr
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileBasicInfo    Pointer to the profile basic information
    explicit JobMgr(ProfileBasicInfo& profileBasicInfo) : m_profileInfo(profileBasicInfo) {};

    /// @brief Destructor
    virtual ~JobMgr() noexcept override = default;

    AxisProfileResult registerJob(std::unique_ptr<IJobObj>& obj) final;
    void unregisterAllJobs() final;
    IJobObj* getJobObj(JobIDType id) final;
    AxisProfileResult sendJobCmd(JobIDType id, CmdReq newReq, IContainer* arg) final;
    CmdState getJobState(JobIDType id) final;
    void runAllJobs() final;

  protected: // methods

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

  private: // data

    ProfileBasicInfo& m_profileInfo;        ///< Profile basic information
    IAxisProfileSharingHandler* m_shareHandle {}; ///< Mediator for sharing data
    std::map<JobIDType, std::unique_ptr<IJobObj>> m_mapJobObj; ///< Map of job objects
  };
}
