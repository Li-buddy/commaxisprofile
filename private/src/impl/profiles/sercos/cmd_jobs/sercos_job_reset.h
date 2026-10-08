#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the job clear device error.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "sercos_job_base.h"
#include "profiles/signal_func/signal_func_cmd.h"

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

  /// @brief This is class of job object of clear device error.
  class SercosJobReset : public SercosJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileInfo   Reference to the Profile Basic Information
    /// @param[in]  shareHandle   Pointer to the sharing handler
    explicit SercosJobReset(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~SercosJobReset() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() { return "SercosJobReset"; }

    JobIDType getID() final { return jobName(); }

  protected: // methods

    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobAborting() final;
    AxisProfileResult onUpdateStatusInDevice() final;
    AxisProfileResult onJobEnterActivating() final;

  private: // data

    const IAxisProfileStatusControlInternal* m_statusControlIf;                    ///< status and controlword interface
    SignalFuncCmd m_signalFuncCmd {{FunctionSupport::CLEAR_DEVERR_CMD_CYCLIC}};    ///< signal function command for clear device error
  };
}
