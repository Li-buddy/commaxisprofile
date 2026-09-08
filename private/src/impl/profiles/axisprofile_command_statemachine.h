#pragma once

/// @file
///
/// This header file is part of the axisprofile library and implements
/// statemachine of axisprofile commands
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "helper/requset_statemachine.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  namespace async
  {
    constexpr std::uint32_t TIMEOUT_COMMAND = 4001;    ///< Timeout used for command execution on the device (number of cycles)
  }

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

  /// @brief This class implements a state-machine for an asynchronous command execution.
  class AsyncCommandStatemachine
  {
  public: // types

    /// @brief This enum represents the command step for an asynchronous command execution.
    enum class CommandStep : std::uint16_t
    {
      STEP_IDLE = 0,  ///< step idle
      STEP_1,         ///< step 1
      STEP_2,         ///< step 2
      STEP_3,         ///< step 3
      STEP_4,         ///< step 4
      STEP_5,         ///< step 5
      STEP_6,         ///< step 6
      STEP_7,         ///< step 7
      STEP_8,         ///< step 8
      STEP_9,         ///< step 9
      STEP_10,        ///< step 10
      STEP_11,        ///< step 11
      STEP_12,        ///< step 12
      STEP_13,        ///< step 13
      STEP_14,        ///< step 14
      STEP_15,        ///< step 15
      STEP_16,        ///< step 16
      STEP_17,        ///< step 17
      STEP_18,        ///< step 18
      STEP_19,        ///< step 19
      STEP_20,        ///< step 20
      STEP_ERR,       ///< step error
      STEP_ABORT = static_cast<std::uint16_t>(-1),  ///< step abort

      PARAM_INIT        = STEP_1,   ///< parameter initialization, legality check 
      PARAM_PROCESS     = STEP_2,   ///< parameter process, reading and writing before command start

      CMD_START         = STEP_10,  ///< command start, e.g. clear CC bit in SoE
      CMD_SET           = STEP_11,  ///< command set, e.g. set CC bit in SoE
      CMD_CHECK_STATUS  = STEP_12,  ///< command status checking, e.g. check parameter data status in SoE
      CMD_DONE          = STEP_13,  ///< command finish
      CMD_CLEAR         = STEP_14,  ///< command clear, e.g. clear CC bit in SoE

      PARAM_POST        = STEP_15,  ///< post process, read and writing parameters after command finish
      STEP_MAX          = STEP_20,  ///< step max
    };

  public: // methods

    /// @brief Constructor
    AsyncCommandStatemachine( ) = default;

    /// @brief Destructor
    virtual ~AsyncCommandStatemachine( ) noexcept = default;

    /// @brief This method raise the thread locker flag, should be called at beginning of each cycle of the command
    /// @return  true if success command can called
    bool raiseAndCheckThreadLocker( )
    {
      if (CommandStep::STEP_IDLE == getCommandStep())
      {
        switchToNextStep(); // switch to init step
      }
      return m_threadLockerFlag.try_raiseFlag(); // raise thread flag
    }

    /// @brief This method switch the command step counter to the next step
    /// @return     The new command step
    CommandStep switchToNextStep( )
    {
      std::uint16_t temp = static_cast<std::uint16_t>(m_stepCnt) + 1;
      if (temp > static_cast<std::uint16_t>(CommandStep::STEP_MAX))
      {
        m_stepCnt = CommandStep::STEP_ERR;
      }
      else
      {
        m_stepCnt = static_cast<CommandStep>(temp);
      }
      m_requestState.resetRequestState();
      m_requestState.reinitCallbackTick();
      return m_stepCnt;
    }

    /// @brief This method switch the command step counter to certain step
    /// @param[in]  destStep      destination step
    /// @return     The new command step
    CommandStep switchToCertainStep(const CommandStep destStep)
    {
      const std::uint16_t temp = static_cast<std::uint16_t>(destStep);
      if (temp > static_cast<std::uint16_t>(CommandStep::STEP_MAX))
      {
        m_stepCnt = CommandStep::STEP_ERR;
      }
      else
      {
        m_stepCnt = destStep;
      }
      m_requestState.resetRequestState();
      return m_stepCnt;
    }

    /// @brief This method switch the command step CMD_START
    /// @return     The new command step
    CommandStep switchToCmdStartStep( )
    {
      return switchToCertainStep(CommandStep::CMD_START);
    }

    /// @brief This method switch the command step counter to the command clear step
    /// @return     The new command step
    CommandStep switchToAbortStep( )
    {
      m_stepCnt = CommandStep::CMD_CLEAR;
      m_requestState.resetRequestState();
      return m_stepCnt;
    }

    /// @brief This method gets the actual command-state.
    /// @return   The actual command-state.
    CommandStep getCommandStep( ) { return m_stepCnt; }

    /// @brief This method get axisprofile state
    /// @return   The actual axisprofile state
    AxisprofileCommandState getAxisProfileCommandState( )
    {   
      switch (m_stepCnt)
      {
      case CommandStep::STEP_IDLE:
        return AxisprofileCommandState::CMD_STATE_IDLE;

      case CommandStep::CMD_CLEAR:
        return AxisprofileCommandState::CMD_STATE_ABORTING;

      default:
        return AxisprofileCommandState::CMD_STATE_ACTIVE;
      }
    }

    /// @brief This method get request state
    /// @return   The request state
    RequestStatemachine& getRequestState() { return m_requestState;}

    /// @brief This method set time out value of async command, set to 0 for never timeout
    /// @param[in]  commandTimeout   time out value of async command 
    void setCommandTimeout(std::uint32_t commandTimeout)
    {
      m_timeout = commandTimeout;
      reinitCommandTick();
    }

    /// @brief This method resets the command-state to the initial state.
    ///        Therefor it also resets the request-state.
    void resetCommandState( )
    {
      reinitCommandTick();
      m_requestState.resetCallbackData();
      m_requestState.resetRequestState();
      m_threadLockerFlag.releaseFlag();
      m_stepCnt = CommandStep::STEP_IDLE;
    }

    /// @brief This method tick command counter and check whether exceed
    /// @return   return true if callback tick exceed
    bool tickCommandAndCheckExceed( )
    {
      if (!m_timeout || m_counter)
      {
        m_counter--;
        return false;
      }
      return true;
    }

    /// @brief This method re-init command tick counter
    void reinitCommandTick( )
    {
      m_counter = m_timeout;
    }

    /// @brief This method set status check cycles, 0 for infinite check
    /// @param[in]  cycles   Check cycles during CMD_CHECK_STATUS
    void setStatusCheckCycles(std::uint32_t cycles)
    {
      m_statusCheckCounter = cycles == 0 ? static_cast<std::uint32_t>(-1) : cycles;
    }

    /// @brief This method tick status check cycles
    /// @return The counter value of checked cycles, 0 for exceed
    bool tickStatusCheckExceed()
    {
      return m_statusCheckCounter ? --m_statusCheckCounter == 0 : true;
    }

  private: // data

    std::uint32_t       m_timeout {async::TIMEOUT_COMMAND};   ///< Command timeout value
    std::uint32_t       m_counter {m_timeout};                ///< Command timeout variable which is decremented once per cycle, when a command is currently executed on the device
    std::uint32_t       m_statusCheckCounter {0};             ///< Status check cycles
    UniversalFlag       m_threadLockerFlag;                   ///< Thread locker flag
    CommandStep         m_stepCnt {CommandStep::STEP_IDLE};   ///< Command-state for an asynchronous sercos-command execution.
    RequestStatemachine m_requestState {};                    ///< Request-state as sub-state of the command-state of an asynchronous sercos-command execution.
  };
}
