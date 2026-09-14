#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and declare the signal function command class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_internal.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class SignalFuncHandler;
  class AsyncCommandStatemachine;

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
  /// @brief This class implements the signal function command.
  class SignalFuncCmd
  {
  public: // methods
    /// @brief Constructor
    explicit SignalFuncCmd(SignalFunction func);

    /// @brief Destructor
    virtual ~SignalFuncCmd() noexcept = default;

    /// @brief This method initializes the signal function command with the signal function handler.
    /// @param[in] handler the signal function handler
    void initSignalFuncHandler(SignalFuncHandler* handler);

    /// @brief This method sets the signal function.
    /// @param func the signal function to be set
    void setSignalFunc(SignalFunction func);

    /// @brief This method checks if the signal function is supported by the device.
    /// @param[in,out] errorInfo  the error information structure to be filled with details if the function is not supported
    /// @return returns true if the signal function is supported, false otherwise
    bool isSignalFuncSupported(FunctionSupportErrorInfo& errorInfo);

    /// @brief This method is called when the job is activating.
    ///        It executes the command steps of the signal function command.
    /// @param stm  the command state machine
    /// @return returns AxisProfileResult indicating the result of the command execution
    AxisProfileResult onJobActivating(AsyncCommandStatemachine& stm);

    /// @brief This method is called when the job is aborting.
    /// @return returns AxisProfileResult indicating the result of the abort operation
    AxisProfileResult onJobAborting();

    /// @brief Gets the state of the signal function.
    /// @param[in, out] isActive Output parameter that receives whether the signal function is currently active.
    /// @return The result status of retrieving the signal function state.
    AxisProfileResult getSignalFuncState(bool& isActive);

    /// @brief Activates the signal function once.
    /// @return The result of the axis profile operation, indicating success, failure, or current status.
    AxisProfileResult singleActiveSignalFunc();

  private:

    SignalFunction     m_signalFunc {};                       ///< the signal function
    SignalFuncHandler* m_signalFuncHandler {};                ///< the signal function manager
  };
}
