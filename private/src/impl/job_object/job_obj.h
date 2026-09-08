#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the statemachine of job object.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_job_obj.h"
#include "job_data_type.h"
#include <atomic>
#include <map>
#include <set>

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------
class TestJobObj;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  namespace obj
  {
    /// @brief Map of allowed command
    static const std::map<JobReq, std::set<JobState>> REQ_ALLOWED_MAP
    {
      {
        JobReq::REQ_RESET, // states in which allow reset the command:
        {JobState::STATE_IDLE, JobState::STATE_ERROR}
      },
      {
        JobReq::REQ_START, // states in which allow start the command:
        {JobState::STATE_IDLE, JobState::STATE_ACTIVATING, JobState::STATE_ACTIVE}
      },
      {
        JobReq::REQ_ABORT, // states in which allow abort the command:
        {JobState::STATE_ACTIVATING, JobState::STATE_ACTIVE, JobState::STATE_ABORTING}
      }
    };
  }

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is class of axisprofile job object
  class JobObj : public IJobObj
  {
  public: // methods

    /// @brief Constructor
    JobObj();

    /// @brief Destructor
    virtual ~JobObj() noexcept override = default;

    JobIDType getID() override {return "JobObjBase"; }
    AxisProfileResult setNewState(JobReq newReq, std::string& infoTxt, IContainer* arg) final;
    JobState getState() final;
    AxisProfileResult mainCycle() final;
    const IContainer* getJobData() override;
    IContainer* getJobDataRef() override;

  protected: // methods

    /// @brief This method is called when new argument is set to the job object.
    /// @param[in]  arg       Arguments to be set
    /// @return Return value indicating success or error.
    virtual AxisProfileResult onSetArgument(IContainer* arg);

    /// @brief This method is called only once when new state entering active state.
    /// @note  The method is one shot invoked in entering ACTIVATING state.
    ///        The method returns ERROR leads job switch to error state.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult onJobEnterActivating();

    /// @brief This method is called when job object is in activating state, cycle invoked every tick.
    /// @return Return value indicating success or error. When return STS_OK, the job object will be switched to active state.
    virtual AxisProfileResult onJobActivating();

    /// @brief This method is called when job object is in activated state, cycle invoked every tick.
    /// @return Return value indicating success or error. When return Error, the job object will be switched error state.
    virtual AxisProfileResult onJobInActived();

    /// @brief This method is called only once when new state entering abort state.
    /// @note  The method is one shot invoked in entering ABORTING state.
    ///        The method returns ERROR leads job switch to error state.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult onJobEnterAborting();

    /// @brief This method is called when job object is in aborting state, cycle invoked every tick.
    /// @return Return value indicating success or error. When return STS_OK, the job object will be switched to idle state.
    virtual AxisProfileResult onJobAborting();

    /// @brief This method is called when job object is reset.
    virtual void onReset();

    /// @brief This method is called when state change of job
    /// @param[in]  state       actual state
    /// @param[in]  oldState    old state
    virtual void onJobStateChanged(JobState state, JobState oldState);

  private: // methods

    /// @brief This method is called cyclically when job object is in intermediate state.
    /// @param[in]  cb            callback that be called current state every cycle
    /// @param[in]  finalState    final state to be switched (when cb returns STS_JOB_EXIT_STATE)
    /// @return Return value indicating success or error.
    AxisProfileResult internalStateCycle(JobCb cb, JobState finalState);

  private: // data

    friend class ::TestJobObj; // unit test class

    bool m_stateChangged;           ///< flag to indicate if state changed
    std::atomic<JobState> m_state;  ///< actual job state
    JobState m_oldState;            ///< old job state which used for state transition
    JobState m_oldStateGetErr;      ///< old job state which got an error when executing
  };
}
