#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements helper class of parameter read/write.
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
#include "profiles/axisprofile_device_param.h"
#include "common/debug_trace.h"
#include "profiles/profile_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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

  /// @brief This class implements the helper class of parameter read/write
  class ParamHelper
  {
  public: // methods

    /// @brief Constructor        
    ParamHelper() = default;

    /// @brief Destructor
    virtual ~ParamHelper() noexcept = default;

    /// @brief The template method for read/write device parameter in JobManager (background thread)
    /// @tparam ID_TYPE         Type of indicator of the parameter in the drive
    /// @tparam VALUTE_TYPE     Type of internal value to be read/write
    /// @tparam JOB_DATA_TYPE   Type of job data container type
    /// @param[in] info         Profile basic information
    /// @param[in] jobObj       Job object of device parameter read/write
    /// @param[in] type         Read or write
    /// @param[in] idn          IDN of the parameter in the drive
    /// @param[in,out] value    Value to be read/write
    /// @param[in] size         Size of the value
    /// @return Return value indicating success or error.
    template<typename ID_TYPE, typename VALUTE_TYPE, typename JOB_DATA_TYPE>
    static AxisProfileResult deviceParamBase(ProfileBasicInfo& info, IJobObj* jobObj, ReadWriteType type,
                                             ID_TYPE id, VALUTE_TYPE& value, std::size_t size)
    {
      if (jobObj == nullptr)
      {
        APR_TRACE_ERROR("%s; JobObj empty", info.m_profileTrace.createTraceMsg());
        return AxisProfileResult::ERR_EXEC_NULL;
      }

      AxisProfileResult result {AxisProfileResult::STS_OK};
      auto jobState = jobObj->getState();

      // the job is idle, we can trigger a new read/write
      if (jobState == JobState::STATE_IDLE)
      {
        auto jobDataContainer = jobObj->getJobDataRef();
        if (jobDataContainer == nullptr)
        {
          APR_TRACE_ERROR("%s; JobObj data empty", info.m_profileTrace.createTraceMsg());
          return AxisProfileResult::ERR_EXEC_NULL;
        }

        auto& jobData = JOB_DATA_TYPE::safeCast(jobDataContainer)->getRef();
        // copy data to read/write buffer
        jobData.setData(id, type, value, size);

        std::string infoTxt {};
        result = jobObj->setNewState(JobReq::REQ_START, infoTxt, nullptr);
        if (aprStatusFailed(result))
        {
          APR_TRACE_ERROR("%s; %s failed; %s",
                          info.m_profileTrace.createTraceMsg(),
                          jobObj->getID().c_str(), infoTxt.c_str());
          return result;
        }
      }
      // job is active, which means read/write has completed
      else if (jobState == JobState::STATE_ACTIVE)
      {
        auto jobDataContainer = jobObj->getJobDataRef();
        if (jobDataContainer == nullptr)
        {
          APR_TRACE_ERROR("%s; JobObj data empty", info.m_profileTrace.createTraceMsg());
          return AxisProfileResult::ERR_EXEC_NULL;
        }

        const auto& jobData = JOB_DATA_TYPE::safeCast(jobDataContainer)->getRef();

        // make sure the IDN and read/write type are the same with we requested
        if (jobData.compareIdAndType(id, type))
        {
          if (type == ReadWriteType::READ)
          {
            // copy data from the read buffer
            jobData.getData(value);
          }

          std::string infoTxt;
          jobObj->setNewState(JobReq::REQ_ABORT, infoTxt, nullptr); // can't failed
          return result;
        }
      }

      // error happen, reset the job automatically and tell caller we get error
      if (jobState == JobState::STATE_ERROR)
      {
        std::string infoTxt;
        jobObj->setNewState(JobReq::REQ_RESET, infoTxt, nullptr); // can't failed
        return AxisProfileResult::ERR_JOB_EXECUTION;
      }

      return AxisProfileResult::STS_PENDING;
    }
  };
}
