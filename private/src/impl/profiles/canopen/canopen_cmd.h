#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the interface of canopen command.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_cmd.h"
#include "cmd_jobs/canopen_job_reset.h"
#include "cmd_jobs/canopen_job_homing.h"
#include "cmd_jobs/canopen_job_device_param.h"
#include "cmd_jobs/canopen_job_setabspos.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
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

  /// @brief This is interface for canopen command
  class CanopenCmd : public AxisProfileCmd
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileInfo          reference to the Profile Basic Information
    explicit CanopenCmd(ProfileBasicInfo& profileInfo) : AxisProfileCmd(profileInfo)
    {
      m_jobLookupTable[ENUM_TO_U16(CmdType::CLEAR_DEV_ERR)] = CanopenJobReset::jobName();
      m_jobLookupTable[ENUM_TO_U16(CmdType::REFERENCE_POS)] = CanopenJobHoming::jobName();
      m_jobLookupTable[ENUM_TO_U16(CmdType::SET_ABS_POS)] = CanopenJobSetAbsPos::jobName();
      m_jobLookupTable[ENUM_TO_U16(CmdType::SET_NEW_REF_WITHOUT_MOVE)] = CanopenJobHoming::jobName();

      m_resetCmds.reserve(3);
      m_resetCmds.emplace_back(CmdType::REFERENCE_POS);
      m_resetCmds.emplace_back(CmdType::SET_ABS_POS);
      m_resetCmds.emplace_back(CmdType::SET_NEW_REF_WITHOUT_MOVE);
    };

    /// @brief Destructor
    virtual ~CanopenCmd() noexcept override = default;
  };
}
