#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements the
/// handler method for async access of sercos over ethercat.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_command_statemachine.h"
#include "profiles/axisprofile_param_access.h"
#include "profiles/sercos/sercos_defs.h"
#include <list>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is sercos async access handler class
  class SercosParamAccess :public AxisProfileParamAccess
  {
  public: // method

    /// @brief Constructor
    SercosParamAccess() = default;

    /// @brief Constructor
    /// @param[in]   profileBasicInfo Pointer to the Profile Basic Information
    explicit  SercosParamAccess(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor.
    virtual ~SercosParamAccess() noexcept override = default;

    /// @brief Warper method to read parameter data asynchronously
    /// @param[in]      paramIDN            IDN of the SoE parameter
    /// @param[out]     buffAddr            Read buffer address
    /// @param[in]      buffSize            Buffer size
    /// @param[in,out]  requestState        Request statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult readAndCheckParamDataAsync(std::uint16_t paramIDN,
                                                 void* buffAddr,
                                                 const size_t buffSize,
                                                 RequestStatemachine& requestState);

    /// @brief Warper method to read parameter data asynchronously
    /// @param[in]      paramIDN            IDN of the SoE parameter
    /// @param[out]     buffAddr            Read buffer address
    /// @param[in]      buffSize            Buffer size
    /// @param[in,out]  asyncCmdState       Async command statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult readAndCheckParamDataAsync(std::uint16_t paramIDN,
                                                 void* buffAddr,
                                                 const size_t buffSize,
                                                 AsyncCommandStatemachine& asyncCmdState);

    /// @brief Warper method to read parameter status asynchronously
    /// @param[in]      paramIDN            IDN of the SoE parameter
    /// @param[out]     buffAddr            Read buffer address
    /// @param[in]      buffSize            Buffer size
    /// @param[in,out]  requestState        Request statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult readAndCheckParamStateAsync(std::uint16_t paramIDN,
                                                  void* buffAddr,
                                                  const size_t buffSize,
                                                  RequestStatemachine& requestState);

    /// @brief Warper method to read parameter status asynchronously
    /// @param[in]      paramIDN            IDN of the SoE parameter
    /// @param[out]     buffAddr            Read buffer address
    /// @param[in]      buffSize            Buffer size
    /// @param[in,out]  asyncCmdState       Async command statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult readAndCheckParamStateAsync(std::uint16_t paramIDN,
                                                  void* buffAddr,
                                                  const size_t buffSize,
                                                  AsyncCommandStatemachine& asyncCmdState);

    /// @brief Warper method to write parameter asynchronously
    /// @param[in]     paramIDN            IDN of the SoE parameter
    /// @param[in]     buffAddr            Write buffer address 
    /// @param[in]     buffSize            Buffer size
    /// @param[in,out] requestState        Request statemachine
    /// @return        Return value indicating success or error
    AxisProfileResult writeAndCheckParamAsync(std::uint16_t paramIDN,
                                              void* buffAddr,
                                              const size_t buffSize,
                                              RequestStatemachine& requestState);

    /// @brief Warper method to write parameter asynchronously
    /// @param[in]     paramIDN            IDN of the SoE parameter
    /// @param[in]     buffAddr            Write buffer address 
    /// @param[in]     buffSize            Buffer size
    /// @param[in,out] asyncCmdState       Async command statemachine
    /// @return        Return value indicating success or error
    AxisProfileResult writeAndCheckParamAsync(std::uint16_t paramIDN,
                                              void* buffAddr,
                                              const size_t buffSize,
                                              AsyncCommandStatemachine& asyncCmdState);

    /// @brief Warper method to access parameter list asynchronously
    /// @param[in]     paramList           parameter list
    /// @param[in,out] requestState        Request statemachine
    /// @return        Return value indicating success or error 
    AxisProfileResult accessAndCheckParamListAsync(std::list<sercos::SercosParamStruct>& paramList,
                                                   RequestStatemachine& requestState);

    /// @brief Warper method to access parameter list asynchronously
    /// @param[in]     paramList           parameter list
    /// @param[in,out] asyncCmdState       Async command statemachine
    /// @return        Return value indicating success or error 
    AxisProfileResult accessAndCheckParamListAsync(std::list<sercos::SercosParamStruct>& paramList,
                                                   AsyncCommandStatemachine& asyncCmdState);

    /// @brief This method executes an asynchronous sercos procedure command.
    ///        It handles the states of the command.
    /// @param[in]      paramIDN              IDN of the sercos procedure command.
    /// @param[in,out]  asyncState            Async state machine
    /// @param[in]      statusCheckCycles     Status check cycles, 0 for infinite check until timeout
    /// @return         Return value indicating success or error.
    AxisProfileResult executeSercosProcedureCommandAsync(ProcedureCommands paramIDN,
                                                         AsyncCommandStatemachine& asyncState,
                                                         std::uint32_t statusCheckCycles = 0);

    /// @brief This method start an asynchronous sercos procedure command.
    ///        It handles the states of the command.
    /// @param[in]      paramIDN              IDN of the sercos procedure command.
    /// @param[in,out]  asyncState            Async state machine
    /// @param[in]      statusCheckCycles     Status check cycles, 0 for infinite check until timeout
    /// @return         Return value indicating success or error.
    AxisProfileResult setSercosProcedureCommandAsync(ProcedureCommands paramIDN,
                                                     AsyncCommandStatemachine& asyncState,
                                                     std::uint32_t statusCheckCycles = 0);

    /// @brief This method interrupt an asynchronous sercos procedure command.
    ///        It handles the states of the command.
    /// @param[in]      paramIDN              IDN of the sercos procedure command.
    /// @param[in,out]  asyncState            Async state machine
    /// @param[in]      statusCheckCycles     Status check cycles, 0 for infinite check until timeout
    /// @return         Return value indicating success or error.
    AxisProfileResult interruptSercosProcedureCommandAsync(ProcedureCommands paramIDN,
                                                           AsyncCommandStatemachine& asyncState,
                                                           std::uint32_t statusCheckCycles = 0);

    /// @brief This method clear an asynchronous sercos procedure command.
    ///        It handles the states of the command.
    /// @param[in]      paramIDN              IDN of the sercos procedure command.
    /// @param[in,out]  asyncState            Async state machine
    /// @param[in]      statusCheckCycles     Status check cycles, 0 for infinite check until timeout
    /// @return         Return value indicating success or error.
    AxisProfileResult clearSercosProcedureCommandAsync(ProcedureCommands paramIDN,
                                                       AsyncCommandStatemachine& asyncState,
                                                       std::uint32_t statusCheckCycles = 0);

    /// @brief This method get status of sercos procedure command.
    /// @param[in]      paramIDN              IDN of the sercos procedure command.
    /// @param[in,out]  requestState          Request state machine
    /// @param[out]     status                The status of sercos procedure command.
    /// @param[in]      checkAKbit            True to check AK bit of status word
    /// @return         Return value indicating success or error.
    AxisProfileResult readSercosProcedureStatus(ProcedureCommands paramIDN,
                                                RequestStatemachine& requestState,
                                                CmdStatusInDevice& status,
                                                bool checkAKbit = true);
  };
}
