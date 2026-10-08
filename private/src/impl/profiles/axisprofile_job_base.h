#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the base job of axisprofile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_command_statemachine.h"
#include "data_sharing/i_sharing_handler.h"
#include "job_object/job_obj.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward function references
  // ---------------------------------------------------------------------------
  class SignalFuncHandler;

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

  /// @brief This is class of axisprofile base job object.
  class AxisprofileJobBase : public JobObj
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileInfo   Reference to the Profile Basic Information
    /// @param[in]  shareHandle   Pointer to the sharing handler
    explicit AxisprofileJobBase(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~AxisprofileJobBase() noexcept override = default;

    JobIDType getID() override
    {
      return "AxisprofileJobObjBase";
    }
    void onReset() override;
    void onJobStateChanged(JobState state, JobState oldState);
    const IContainer* getJobData() override;
    IContainer* getJobDataRef() override;
    AxisProfileResult onJobInActived() override;

  protected: // methods

    /// @brief This method init delay cycles, e.g. used for wait axis to standstill.
    /// @param[in]  time    Delay time in seconds
    /// @return Return value indicating success or error.
    AxisProfileResult initDelayCycles(double time);

    /// @brief This method executes delay cycles, e.g. used for wait axis to standstill.
    /// @return Return value indicating success or error.
    AxisProfileResult exeDelayCycles();

    /// @brief This method is internal executer for active and abort
    /// @return Return value indicating success or error.
    AxisProfileResult runJobsInternal();

    /// @brief This method is called to status in device is updated.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult onUpdateStatusInDevice();

    /// @brief This method is used to get the signal function handler for the job.
    /// @param[out] handler the pointer to the signal function handler, if available.
    /// @return return value indicating success or error.
    AxisProfileResult getSignalFuncHandler(SignalFuncHandler*& handler);

  protected: // data

    ProfileBasicInfo& m_profileInfo;            ///< profile basic information
    IAxisProfileSharingHandler* m_shareHandle;  ///< pointer to the sharing handler
    AsyncCommandStatemachine m_asyncSm {};      ///< async command state machine
    std::uint32_t m_waitCycleCnt {0};           ///< wait cycle count for @ref initWaitTimeToStanstill
    std::uint16_t m_jobStep {};                 ///< current job step in homing
    std::vector<JobFunc> m_jobFuns {};          ///< function list of activating homing
    JobDataDeviceStatus m_deviceStatus {};      ///< job data of status in device
    CmdStatusInDevice m_oldDeviceStatus {};     ///< old command status in device
    bool m_isSignalFuncCmd {false};             ///< flag to indicate whether the command is a signal function command
  };
}
