#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines types for axisprofile job object.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <functional>
#include <string>

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

   /// @brief type of job object ID
  typedef const std::string JobIDType;

  /// @brief Job object unknown
  JobIDType JOB_UNKNOWN = "JobUnknown";

  /// @brief The state of job object
  using JobState = CmdState;

  /// @brief The type of request of job object
  using JobReq = CmdReq;

  /// @brief Convert job state to string
  /// @param[in] id     the job state
  /// @return String / name of the job
  inline const char* jobStateToStr(JobState state)
  {
    return cmdStateToString(state);
  }

  /// @brief Convert job state to string
  /// @param[in] req     the job request
  /// @return String / name of the job
  inline const char* jobReqToStr(JobReq req)
  {
    return cmdReqToString(req);
  }

  /// @brief Type of callback that be called current state every cycle
  /// 
  /// @Note  When return STS_JOB_EXIT_STATE, means state operation is complete and can go out of the state
  ///        When return any other STS value, means cycle still need be called in next tick
  ///        The function returns any ERR code means unexpected behavior and goes to Error State
  using JobCb = std::function<AxisProfileResult(void)>;

  /// @brief Type of function that execute inside the job object
  using JobFunc = std::function<AxisProfileResult(void)>;
}
