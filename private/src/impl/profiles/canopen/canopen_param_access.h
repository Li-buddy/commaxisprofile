#pragma once

/// @file
///
/// This header file is part of the axisprofile library and implements the
/// handler method for async access of canopen over ethercat
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "helper/requset_statemachine.h"
#include "profiles/axisprofile_param_access.h"
#include "profiles/canopen/canopen_defs.h"
#include <list>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
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

  /// @brief This is axisprofile parameter helper class
  class CanopenParamAccess :public AxisProfileParamAccess
  {
  public: // method

    /// @brief Constructor
    CanopenParamAccess() = default;

    /// @brief Destructor.
    ~CanopenParamAccess() noexcept override = default;

    /// @brief Constructor
    /// @param[in]   profileBasicInfo Pointer to the Profile Basic Information       
    explicit  CanopenParamAccess(ProfileBasicInfo& profileBasicInfo);

    /// @brief Warper method to read parameter data asynchronously
    /// @param[in]      objectIndex         Index of CoE object
    /// @param[in]      subIndex            Subindex of CoE object
    /// @param[out]     buffAddr            read buffer address
    /// @param[in]      buffSize            buffer size
    /// @param[in,out]  requestState        request state machine
    /// @return         Return value indicating success or error
    AxisProfileResult readAndCheckParamAsync(std::uint16_t objectIndex,
                                             std::uint8_t subIndex,
                                             void* buffAddr,
                                             const size_t buffSize,
                                             RequestStatemachine& requestState);

    /// @brief Warper method to write parameter asynchronously
    /// @param[in]      objectIndex         Index of CoE object
    /// @param[in]      subIndex            Subindex of CoE object
    /// @param[in]      buffAddr            write buffer address 
    /// @param[in]      buffSize            buffer size
    /// @param[in,out]  requestState        request state machine
    /// @return         Return value indicating success or error
    AxisProfileResult writeAndCheckParamAsync(std::uint16_t objectIndex,
                                              std::uint8_t subIndex,
                                              void* buffAddr,
                                              const size_t buffSize,
                                              RequestStatemachine& requestState);

    /// @brief Warper method to access parameter list asynchronously
    /// @param[in]      paramList           parameter list
    /// @param[in,out]  requestState        request state machine
    /// @return        Return value indicating success or error 
    AxisProfileResult accessAndCheckParamListAsync(std::list<canopen::CanopenParamStruct>& paramList,
                                                   RequestStatemachine& requestState);

    /// @brief Warper method to access parameter list asynchronously
    /// @param[in]      paramList           parameter list
    /// @param[in,out]  asyncCmdState       async command statemachine
    /// @return        Return value indicating success or error 
    AxisProfileResult accessAndCheckParamListAsync(std::list<canopen::CanopenParamStruct>& paramList,
                                                   AsyncCommandStatemachine& asyncCmdState);
  };
}
