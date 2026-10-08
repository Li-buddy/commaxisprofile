#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the reset job of CANOpen.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_base.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenJobReset;
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

  /// @brief This is class of CANOpen reset job object.
  class CanopenJobReset : public CanopenJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileInfo     Reference to the Profile Basic Information
    /// @param[in]  shareHandle     Pointer to the sharing handler
    explicit CanopenJobReset(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle);

    /// @brief Destructor
    virtual ~CanopenJobReset() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() { return "CanopenJobReset"; }

    JobIDType getID() final { return jobName(); }

  protected: // methods

    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobAborting() final;
    AxisProfileResult onUpdateStatusInDevice() final;

    /// @brief The method executes the command with the given signal function and state machine.
    /// @param[in] sFunc The signal function to be executed.
    /// @param[in] stm   The state machine that manages the command execution.
    /// @return returns the result of the command execution, indicating success or failure.
    AxisProfileResult executeCmdBySignalFunc(SignalFunction sFunc, AsyncCommandStatemachine& stm);

    /// @brief The method executes the command by handling related parameters and state machine without signal function.
    /// @param[in] stm The state machine that manages the command execution.
    /// @return returns the result of the command execution, indicating success or failure.
    AxisProfileResult executeCmdByParamOperation(AsyncCommandStatemachine& stm);

  private: // data
    friend class::TestCanopenJobReset;
    WordStruct m_controlWord {};   ///< Local control word value
    WordStruct m_statusWord {};    ///< Local status word value
    SignalFunction m_signalFunc {FunctionSupport::CLEAR_DEVERR_CMD_CYCLIC}; ///< the signal function for reset command
    SignalFuncHandler* m_signalFuncHandler {};                              ///< the signal function manager for reset command
  };
}
