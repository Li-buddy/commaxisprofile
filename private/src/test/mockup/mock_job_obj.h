#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup class of job object.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "job_object/i_job_obj.h"

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

  /// @brief This is mockup class of axisprofile job object
  class MockJobObj : public ::comm::axisprofile::IJobObj
  {
  public: // methods

    /// @brief Constructor
    MockJobObj() = default;
    MockJobObj(std::string jobID) : m_jobID(jobID) {};

    /// @brief Destructor
    virtual ~MockJobObj() noexcept = default;

    ::comm::axisprofile::JobIDType getID() override { return m_jobID; }
    ::comm::axisprofile::AxisProfileResult setNewState(::comm::axisprofile::CmdReq /*newReq*/,
                                                       std::string& /*infoTxt*/,
                                                       ::comm::axisprofile::IContainer* /*arg*/) override
    {
      return m_retValue;
    }
    ::comm::axisprofile::CmdState getState() override
    {
      return m_objState;
    }
    ::comm::axisprofile::AxisProfileResult mainCycle() override
    {
      return m_retValue;
    }

    const ::comm::axisprofile::IContainer* getJobData() override
    {
      return m_jobData;
    }

    ::comm::axisprofile::IContainer* getJobDataRef() override
    {
      return m_jobData;
    }

  public: // data

    ::comm::axisprofile::AxisProfileResult m_retValue {::comm::axisprofile::AxisProfileResult::STS_OK}; ///< Result of member functions
    ::comm::axisprofile::CmdState m_objState {::comm::axisprofile::CmdState::STATE_IDLE}; ///< State of the job object
    ::comm::axisprofile::JobIDType m_jobID {};  ///< Job object ID
    ::comm::axisprofile::IContainer* m_jobData; ///< Job data
  };
}
