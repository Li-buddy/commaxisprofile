#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements
/// the handler method for parameter access.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "profiles/axisprofile_command_statemachine.h"
#include "profiles/i_axisprofile_param_access.h"
#include "profiles/profile_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestPorfileParamAccess;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
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
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  
  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is axisprofile async access handler class
  class AxisProfileParamAccess: public IAxisProfileParamAccess
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo Pointer to the Profile Basic Information
    explicit AxisProfileParamAccess(ProfileBasicInfo& profileBasicInfo) : m_profileBasicInfo(profileBasicInfo) {};

    /// @brief Destructor
    virtual ~AxisProfileParamAccess() noexcept override = default;

    /// @brief This method reads asynchronously the parameter. It handles the states of the parameter requests.
    /// @param[in]      readParam           Struct containing all information for the read request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @param[out]     buffAddr            Read buffer address
    /// @param[in]      buffSize            Buffer size
    /// @param[in,out]  requestState        Request statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult readAndCheckParamAsync(ParamReqDataBase& readParam, void* buffAddr, const size_t buffSize, RequestStatemachine& requestState);

    /// @brief This method writes and checks asynchronously the parameter requests. It handles the states of the parameter requests.
    ///        The method should be called cyclically and returns S_OK while procedure finished.
    /// @param[in]      writeParam          Struct containing all information for the write request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @param[in,out]  requestState        Request statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult writeAndCheckParamAsync(ParamReqDataBase& writeParam, RequestStatemachine& requestState);

  protected: // methods

    /// @brief This method write parameter asynchronously
    /// @param[in]      writeParam          Data for executing a parameter write request
    /// @param[in,out]  requestState        Request statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult writeParamAsync(ParamReqDataBase& writeParam, RequestStatemachine& requestState);

    /// @brief This method write parameter asynchronously
    /// @param[in]      writeParam          Data for executing a parameter write request
    /// @param[in,out]  cmdState            Command statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult writeParamAsync(ParamReqDataBase& writeParam, AsyncCommandStatemachine& cmdState);

    /// @brief This method read parameter asynchronously
    /// @param[in]      readParam           Data for executing a parameter read request
    /// @param[in,out]  requestState        Request statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult readParamAsync(ParamReqDataBase& readParam, RequestStatemachine& requestState);

    /// @brief This method read parameter asynchronously
    /// @param[in]      readParam           Data for executing a parameter read request
    /// @param[in,out]  cmdState            Command statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult readParamAsync(ParamReqDataBase& readParam, AsyncCommandStatemachine& cmdState);

    /// @brief This method checks asynchronously if the parameter request is finished successfully.
    /// @param[in,out]  requestState        Command statemachine
    /// @return         Return value indicating success or error
    AxisProfileResult checkResponseAsync(RequestStatemachine& requestState);

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

  protected: // data

    friend class ::TestPorfileParamAccess;            // for unit test

    ProfileBasicInfo& m_profileBasicInfo;             ///< Profile basic information
    IAxisProfileSharingHandler* m_sharingHandler {};  ///< Mediator for sharing data
  };
}
