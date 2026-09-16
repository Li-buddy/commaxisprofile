#pragma once
///
/// @brief This file is part of the axisprofile library and define the interface of sercos command.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "cmd_jobs/sercos_job_homing.h"
#include "cmd_jobs/sercos_job_reset.h"
#include "cmd_jobs/sercos_job_setabspos.h"
#include "cmd_jobs/sercos_job_fixed_stop.h"
#include "cmd_jobs/sercos_job_setopmode.h"
#include "profiles/axisprofile_cmd.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
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

  /// @brief This is interface for sercos command
  class SercosCmd : public AxisProfileCmd
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileInfo          reference to the Profile Basic Information
    explicit SercosCmd(ProfileBasicInfo& profileInfo): AxisProfileCmd(profileInfo)
    {
      m_jobLookupTable[ENUM_TO_U16(CmdType::CLEAR_DEV_ERR)] = SercosJobReset::jobName();
      m_jobLookupTable[ENUM_TO_U16(CmdType::SET_ABS_POS)] = SercosJobSetAbsPos::jobName();
      m_jobLookupTable[ENUM_TO_U16(CmdType::REFERENCE_POS)] = SercosJobHoming::jobName();
      m_jobLookupTable[ENUM_TO_U16(CmdType::FIXED_STOP)] = SercosJobFixedStop::jobName();
      m_jobLookupTable[ENUM_TO_U16(CmdType::SET_NEW_REF_WITHOUT_MOVE)] = SercosJobHoming::jobName();
      m_jobLookupTable[ENUM_TO_U16(CmdType::SET_OPERATION_MODE_WITH_CHECK)] = SercosJobSetOpMode::jobName();

      m_resetCmds.reserve(5);
      m_resetCmds.emplace_back(CmdType::SET_ABS_POS);
      m_resetCmds.emplace_back(CmdType::REFERENCE_POS);
      m_resetCmds.emplace_back(CmdType::FIXED_STOP);
      m_resetCmds.emplace_back(CmdType::SET_NEW_REF_WITHOUT_MOVE);
      m_resetCmds.emplace_back(CmdType::SET_OPERATION_MODE_WITH_CHECK);
    };

    /// @brief Destructor
    virtual ~SercosCmd() noexcept override = default;
  };
}
