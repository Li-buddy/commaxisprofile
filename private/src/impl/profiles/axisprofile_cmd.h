#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the interface of axisprofile command.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "diagnosis/dia.h"
#include "i_axisprofile_cmd_internal.h"
#include "job_object/i_job_obj.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include <vector>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestProfileCmd;

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
  using JobLookupTable = std::array<std::string, CMD_MAX + 1>;  ///< Lookup table of job used by command

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is interface for axisprofile command
  class AxisProfileCmd : public IAxisProfileCmdInternal
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileInfo          reference to the Profile Basic Information
    explicit AxisProfileCmd(ProfileBasicInfo& profileInfo);

    /// @brief Destructor
    virtual ~AxisProfileCmd() noexcept override = default;

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;
    const IAxisProfileCmdC* getCInterface() const final;

    // command handling
    CmdState getCmdState(CmdType cmdType) final;
    AxisProfileResult resetCmd(CmdType cmdType) final;
    AxisProfileResult abortCmd(CmdType cmdType) final;
    AxisProfileResult startCmd(CmdType cmdType, IContainer* agr = nullptr);
    CmdStatusInDevice getCmdStatusInDevice(CmdType cmdType) final;

    // command invokers
    AxisProfileResult clearDeviceError(CmdType& cmdType) final;
    AxisProfileResult clearDeviceError2() final;
    AxisProfileResult setAbsolutePosition(double newPos, AxisProfileEncoder encoder, CmdType& cmdType) final;
    AxisProfileResult referencingAxis(double newPos, AxisProfileEncoder encoder, CmdType& cmdType) final;
    AxisProfileResult fixedStop(double clampingTorque, CmdType& cmdType) final;
    AxisProfileResult setNewReferenceWithoutMovement(double newPos, AxisProfileEncoder encoder, RefMode refMode, CmdType& cmdType) final;

  private: // methods

    /// @brief This method gets the job ID from command type
    /// @param[in]   cmdType      command type
    /// @return    job ID
    JobIDType getJob(CmdType cmdType);

  protected: // data

    ProfileBasicInfo& m_profileInfo;            ///< profile basic information
    IAxisProfileSharingHandler* m_shareHandle;  ///< pointer to the sharing handler
    JobLookupTable m_jobLookupTable {JOB_UNKNOWN}; ///< Lookup table of job used by command
    std::vector<CmdType> m_resetCmds;           ///< list of command that need to be reset during reset command

  private: // data

    friend TestProfileCmd;        // test class
    IAxisProfileCmdC m_cItf {};   ///< C-Interface for bundle interface
  };
}
