#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements
/// base class for command handling.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_command_statemachine.h"
#include "i_axisprofile_command_internal.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include <map>
#include <vector>

// C-Interface
#include "comm/axisprofile/interfaces/i_axisprofile_command_c.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class ProfileTrace;

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

  /// @class AxisProfileCommand
  /// This is the base class for command handling.
  class AxisProfileCommand : public IAxisProfileCommandInternal
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo     reference to the Profile Basic Information
    explicit AxisProfileCommand(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~AxisProfileCommand() noexcept override;

    const IAxisProfileCommandC* getCInterface() const final;
    AxisProfileResult booting() final;

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    /// @brief This method get a command async statemachine
    /// @param[in]   type         Type of the command
    /// @return      Return command async statemachine
    AsyncCommandStatemachine* getAsyncStatemachine(const AxisProfileCommandType type);

    /// @brief This method raise a command and get its async statemachine
    /// @param[in]   type         Type of the command
    /// @param[out]  asyncState   Pointer to async statemachine
    /// @return      Return command async statemachine
    AxisProfileResult raiseAndGetStatemachine(const AxisProfileCommandType type, AsyncCommandStatemachine*& asyncState);

    /// @brief This method handle the result of commands execution
    /// @param[in]      type          Type of the command
    /// @param[in,out]  asyncState    Async statemachine
    /// @param[in]      result        Executing result of this cycle
    /// @return     Return handle result
    AxisProfileResult resultHandlingCycle(const AxisProfileCommandType type, AsyncCommandStatemachine& asyncState,
                                          AxisProfileResult& result);

    AxisProfileResult checkAxisHomed(bool& isHomed) final;

  protected:
    AxisprofileCommandState getAxisProfileCommandState(const AxisProfileCommandType type) final;

  protected: // data

    ProfileBasicInfo& m_profileBasicInfo;                             ///< Profile basic information
    std::vector<std::function<AxisProfileResult()>> m_initCommands;   ///< Vector contains all init methods
    IAxisProfileSharingHandler* m_sharingHandler{};                   ///< mediator for sharing data
    std::map<AxisProfileCommandType, AsyncCommandStatemachine*> m_asyncStates;  ///< Map of commands async state machine

  private: // data

    IAxisProfileCommandC m_cInterface{};                                        ///< C-Interface for external usage
  };
}
