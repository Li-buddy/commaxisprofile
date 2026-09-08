#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup class of job manager.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mockup/mock_job_obj.h"
#include "profiles/job_mgr/i_job_mgr.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
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

  /// @brief This is mockup class of axisprofile job manager
  class MockJobMgr : public ::comm::axisprofile::IJobMgr
  {
  public: // methods

    /// @brief Constructor
    MockJobMgr() = default;

    /// @brief Destructor
    virtual ~MockJobMgr() noexcept = default;

    ::comm::axisprofile::AxisProfileResult registerJob(std::unique_ptr<::comm::axisprofile::IJobObj>& /*obj*/) override { return m_retValue; }
    void unregisterAllJobs() override { ; }
    ::comm::axisprofile::IJobObj* getJobObj(::comm::axisprofile::JobIDType/*id*/) override { return m_jobObjPtr; }
    ::comm::axisprofile::AxisProfileResult sendJobCmd(::comm::axisprofile::JobIDType/*id*/,
                                                      ::comm::axisprofile::CmdReq /*newReq*/,
                                                      ::comm::axisprofile::IContainer* /*arg*/) override { return m_retValue; }
    ::comm::axisprofile::CmdState getJobState(::comm::axisprofile::JobIDType/*id*/) override { return m_jobObjPtr->getState(); }
    void runAllJobs() override{}

  protected: // methods

    void setSharingHandler(::comm::axisprofile::IAxisProfileSharingHandler* /*handler*/) override {}

  public: // data

    ::comm::axisprofile::AxisProfileResult m_retValue {::comm::axisprofile::AxisProfileResult::STS_OK};   ///< Result of member functions
    MockJobObj m_jobObj {}; ///< mock job object
    ::comm::axisprofile::IJobObj* m_jobObjPtr {&m_jobObj}; /// Pointer to job object
  };
}
