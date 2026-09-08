#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define sercos base job.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_job_base.h"
#include "profiles/sercos/sercos_param_access.h"

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

  /// @brief This is base class of sercos job object.
  class SercosJobBase : public AxisprofileJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileInfo   Reference to the Profile Basic Information
    /// @param[in]  shareHandle   Pointer to the sharing handler
    explicit SercosJobBase(ProcedureCommands cmd, ProfileBasicInfo& profileInfo,
                           IAxisProfileSharingHandler* shareHandle = nullptr)
      : AxisprofileJobBase(profileInfo, shareHandle), m_command(cmd),
      m_paramAccess(static_cast<SercosParamAccess*>(m_shareHandle->getParamAccessIF()))
    {}

    /// @brief Destructor
    virtual ~SercosJobBase() noexcept override = default;

    JobIDType getID() override {return "SercosJobBase";}

  protected: // methods

    AxisProfileResult onJobEnterActivating() override
    {
      m_asyncSm.resetCommandState();
      m_asyncSm.switchToCmdStartStep();
      m_smStatus.resetRequestState();
      return JobObj::onJobEnterActivating();
    }

    AxisProfileResult onJobEnterAborting() override
    {
      m_asyncSm.resetCommandState();
      m_asyncSm.switchToCmdStartStep();
      m_smStatus.resetRequestState();
      return JobObj::onJobEnterAborting();
    }

    AxisProfileResult onUpdateStatusInDevice() override
    {
      // This method only happens when command is in process
      // So it is not necessary checking anymore when status exit DEVICE_IN_PROCESS
      if (m_deviceStatus.get() == CmdStatusInDevice::DEVICE_IN_PROCESS)
      {
        CmdStatusInDevice status {};
        auto res = m_paramAccess->readSercosProcedureStatus(m_command, m_smStatus, status, true);
        if (res == AxisProfileResult::STS_OK)
        {
          m_deviceStatus.getRef().store(status);
        }
        return res;
      }
      return AxisProfileResult::STS_OK;
    }

  protected: // data
    
    ProcedureCommands m_command {};       ///< current command
    SercosParamAccess* m_paramAccess {};  ///< Sercos parameter access
    RequestStatemachine m_smStatus {};    ///< Status request state machine
  };
}
