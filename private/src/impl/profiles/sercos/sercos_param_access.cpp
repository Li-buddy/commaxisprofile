/// @file
///
/// @brief This source file is part of the axisprofile library and implements the
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
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "sercos_param_access.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

SercosParamAccess::SercosParamAccess(ProfileBasicInfo& profileBasicInfo)
  : AxisProfileParamAccess(profileBasicInfo)
{
}

AxisProfileResult SercosParamAccess::readAndCheckParamDataAsync(std::uint16_t paramIDN,
                                                                void* buffAddr,
                                                                const size_t buffSize,
                                                                RequestStatemachine& requestState)
{
  ParamReqDataSoE readParam {m_profileBasicInfo.m_deviceAddress, paramIDN, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber};
  return AxisProfileParamAccess::readAndCheckParamAsync(readParam, buffAddr, buffSize, requestState);
}

AxisProfileResult SercosParamAccess::readAndCheckParamDataAsync(std::uint16_t paramIDN,
                                                                void* buffAddr,
                                                                const size_t buffSize,
                                                                AsyncCommandStatemachine& asyncCmdState)
{
  return readAndCheckParamDataAsync(paramIDN, buffAddr, buffSize, asyncCmdState.getRequestState());
}

AxisProfileResult SercosParamAccess::readAndCheckParamStateAsync(std::uint16_t paramIDN,
                                                                 void* buffAddr,
                                                                 const size_t buffSize,
                                                                 RequestStatemachine& requestState)
{
  ParamReqDataSoE readData {m_profileBasicInfo.m_deviceAddress, paramIDN, comm::ethercat::master::fbs::ElementFlags::ElementFlags_dataState, m_profileBasicInfo.m_driveNumber};
  return AxisProfileParamAccess::readAndCheckParamAsync(readData, buffAddr, buffSize, requestState);
}

AxisProfileResult SercosParamAccess::readAndCheckParamStateAsync(std::uint16_t paramIDN,
                                                                 void* buffAddr,
                                                                 const size_t buffSize,
                                                                 AsyncCommandStatemachine& asyncCmdState)
{
  return readAndCheckParamStateAsync(paramIDN, buffAddr, buffSize, asyncCmdState.getRequestState());
}

AxisProfileResult SercosParamAccess::writeAndCheckParamAsync(std::uint16_t paramIDN,
                                                             void* buffAddr,
                                                             const size_t buffSize,
                                                             RequestStatemachine& requestState)
{
  ParamReqDataSoE writeParam {m_profileBasicInfo.m_deviceAddress, paramIDN, comm::ethercat::master::fbs::ElementFlags::ElementFlags_value, m_profileBasicInfo.m_driveNumber,
    static_cast<std::uint8_t*>(buffAddr), static_cast<uint32_t>(buffSize)};
  return AxisProfileParamAccess::writeAndCheckParamAsync(writeParam, requestState);
}

AxisProfileResult SercosParamAccess::writeAndCheckParamAsync(std::uint16_t paramIDN,
                                                             void* buffAddr,
                                                             const size_t buffSize,
                                                             AsyncCommandStatemachine& asyncCmdState)
{
  return writeAndCheckParamAsync(paramIDN, buffAddr, buffSize, asyncCmdState.getRequestState());
}

AxisProfileResult SercosParamAccess::accessAndCheckParamListAsync(std::list<sercos::SercosParamStruct>& paramList,
                                                                  RequestStatemachine& requestState)
{
  AxisProfileResult result;

  if (paramList.empty())
  {
    return AxisProfileResult::STS_OK;
  }

  auto& param = paramList.front();
  if (param.m_readWriteType == ReadWriteType::READ)
  {
    result = readAndCheckParamDataAsync(param.m_requestIDN, param.m_ptrData, param.dataSize, requestState);
  }
  else
  {
    result = writeAndCheckParamAsync(param.m_requestIDN, param.m_ptrData, param.dataSize, requestState);
  }

  if (AxisProfileResult::STS_OK == result)
  {
    if (!paramList.empty())
    {
      paramList.pop_front();
      result = paramList.empty() ? AxisProfileResult::STS_OK : AxisProfileResult::STS_PENDING;
    }
  }
  return result;
}

AxisProfileResult SercosParamAccess::accessAndCheckParamListAsync(std::list<sercos::SercosParamStruct>& paramList,
                                                                  AsyncCommandStatemachine& asyncCmdState)
{
  return accessAndCheckParamListAsync(paramList, asyncCmdState.getRequestState());
}

AxisProfileResult SercosParamAccess::executeSercosProcedureCommandAsync(ProcedureCommands paramIDN,
                                                                        AsyncCommandStatemachine& asyncState,
                                                                        std::uint32_t statusCheckCycles)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  // if getCommandStateInQueue() == pre-process
  // then skip
  switch (asyncState.getCommandStep())
  {
    // STEP 1: Clear the Command Control (CC) bits (needed to ensure a rising edge in the next step)
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      asyncState.reinitCommandTick();
      std::uint16_t cmd = SERCOS_COMMAND_NOT_SET;
      result = writeAndCheckParamAsync(static_cast<std::uint16_t>(paramIDN), &cmd, sizeof(cmd), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    // STEP 2: Set the Command Control (CC) bits to set and activate the new command
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      std::uint16_t cmd = SERCOS_COMMAND_ENABLE;
      result = writeAndCheckParamAsync(static_cast<std::uint16_t>(paramIDN), &cmd, sizeof(cmd), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {

        asyncState.switchToNextStep();
        asyncState.setStatusCheckCycles(statusCheckCycles);
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    // STEP 3: The device is now processing the command. Send parameter read request to check the status.
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    {
      // status should be finish
      std::uint16_t cmdState {0xFFFF};
      result = readAndCheckParamStateAsync(static_cast<std::uint16_t>(paramIDN), &cmdState, sizeof(cmdState), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        // TODO: add error checking, case of cmdState == SERCOS_COMMAND_STATUS_ERROR
        // when the error happen, read S-0-0095 and report the text to diagnosis
        if (cmdState == SERCOS_COMMAND_STATUS_FINISH || asyncState.tickStatusCheckExceed())  // command finished as expected value or check cycle exceed
        {
          asyncState.switchToNextStep();
        }
      }
      result = AxisProfileResult::STS_PENDING;
      break;
    }
    // STEP 4: Clear the Command Control (CC) bits again to clear the command, command finish if success
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      std::uint16_t cmd = SERCOS_COMMAND_NOT_SET;
      result = writeAndCheckParamAsync(static_cast<std::uint16_t>(paramIDN), &cmd, sizeof(cmd), asyncState);
      break;
    }

    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
    default:
    {
      APR_TRACE_ERROR("invalid state to execute Command.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}

AxisProfileResult SercosParamAccess::setSercosProcedureCommandAsync(ProcedureCommands paramIDN,
                                                                    AsyncCommandStatemachine& asyncState,
                                                                    std::uint32_t statusCheckCycles)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  switch (asyncState.getCommandStep())
  {
    // STEP1: Clear CC bit to ensure a rising edge in the next step
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    {
      asyncState.reinitCommandTick();
      std::uint16_t cmd = SERCOS_COMMAND_NOT_SET;
      result = writeAndCheckParamAsync(static_cast<std::uint16_t>(paramIDN), &cmd, sizeof(cmd), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToNextStep();
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    // STEP2: Set CC bit to start the command
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      std::uint16_t cmd = SERCOS_COMMAND_ENABLE;
      result = writeAndCheckParamAsync(static_cast<std::uint16_t>(paramIDN), &cmd, sizeof(cmd), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToNextStep();
        asyncState.setStatusCheckCycles(statusCheckCycles);
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    // STEP3: Read and check executing status, procedure finish if success
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      std::uint16_t cmdState {0xFFFF};
      result = readAndCheckParamStateAsync(static_cast<std::uint16_t>(paramIDN), &cmdState, sizeof(cmdState), asyncState);

      if (AxisProfileResult::STS_OK == result)
      {
        if (cmdState == SERCOS_COMMAND_STATUS_FINISH ||
            cmdState == SERCOS_COMMAND_STATUS_IN_PROCESS ||
            asyncState.tickStatusCheckExceed())
        {
          return AxisProfileResult::STS_OK; // check success
        }

        if (cmdState == SERCOS_COMMAND_STATUS_ERROR)
        {
          // TODO: try to read error text from drive and report it to diagnosis
          APR_TRACE_ERROR("error, command can't execute.");
          return AxisProfileResult::ERR_NOK;
        }
      }
      result = AxisProfileResult::STS_PENDING;
      break;
    }

    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
    default:
    {
      APR_TRACE_ERROR("invalid state to set Command.");
      result = AxisProfileResult::ERR_INVALID_STATE;
      break;
    }
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}

AxisProfileResult SercosParamAccess::interruptSercosProcedureCommandAsync(ProcedureCommands paramIDN,
                                                                          AsyncCommandStatemachine& asyncState,
                                                                          std::uint32_t statusCheckCycles)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  switch (asyncState.getCommandStep())
  {
    // STEP1: Set interrupt
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      asyncState.reinitCommandTick();
      std::uint16_t cmd = SERCOS_COMMAND_INTERRUPT;
      result = writeAndCheckParamAsync(static_cast<std::uint16_t>(paramIDN), &cmd, sizeof(cmd), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToNextStep();
        asyncState.setStatusCheckCycles(statusCheckCycles);
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    // STEP2: Read and check status, procedure finish if success
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      auto statusWord = m_sharingHandler->getStatusControlIF()->getStatusWord();
      bool cmdChanged = (statusWord & (1 << SERCOS_STSWORD_PROCEDURE_CMDBIT_OFFSET)) != 0;
      if (cmdChanged)
      {
        std::uint16_t cmdState {0xFFFF};
        result = readAndCheckParamStateAsync(static_cast<std::uint16_t>(paramIDN), &cmdState, sizeof(cmdState), asyncState);
        if (AxisProfileResult::STS_OK == result)
        {
          // TODO: add error checking, case of cmdState == SERCOS_COMMAND_STATUS_ERROR
          if (cmdState == SERCOS_COMMAND_STATUS_INTERRUPT || asyncState.tickStatusCheckExceed())
          {
            return AxisProfileResult::STS_OK; // check success
          }
        }
      }
      result = AxisProfileResult::STS_PENDING;
      break;
    }

    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
    default:
    {
      result = AxisProfileResult::ERR_NOK;
      break;
    }
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}

AxisProfileResult SercosParamAccess::clearSercosProcedureCommandAsync(ProcedureCommands paramIDN,
                                                                      AsyncCommandStatemachine& asyncState,
                                                                      std::uint32_t statusCheckCycles)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  switch (asyncState.getCommandStep())
  {
    // STEP1: Clear the Command Control (CC) bits
    case AsyncCommandStatemachine::CommandStep::CMD_START:
    case AsyncCommandStatemachine::CommandStep::CMD_SET:
    {
      asyncState.reinitCommandTick();
      std::uint16_t cmd = SERCOS_COMMAND_NOT_SET;
      result = writeAndCheckParamAsync(static_cast<std::uint16_t>(paramIDN), &cmd, sizeof(cmd), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        asyncState.switchToCertainStep(AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS);
        asyncState.setStatusCheckCycles(statusCheckCycles);
        result = AxisProfileResult::STS_PENDING;
      }
      break;
    }
    // STEP2: Read and check status, procedure finish if success
    case AsyncCommandStatemachine::CommandStep::CMD_CHECK_STATUS:
    case AsyncCommandStatemachine::CommandStep::CMD_DONE:
    {
      std::uint16_t cmdState {0xFFFF};
      result = readAndCheckParamStateAsync(static_cast<std::uint16_t>(paramIDN), &cmdState, sizeof(cmdState), asyncState);
      if (AxisProfileResult::STS_OK == result)
      {
        // TODO: add error checking, case of cmdState == SERCOS_COMMAND_STATUS_ERROR
        if (cmdState == SERCOS_COMMAND_STATUS_NOT_SET || asyncState.tickStatusCheckExceed())  // command cleared as required
        {
          result = AxisProfileResult::STS_OK; // check success
        }
        else // read and check again until checked or timeout
        {
          result = AxisProfileResult::STS_PENDING;
        }
      }
      break;
    }

    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
    default:
    {
      result = AxisProfileResult::ERR_NOK;
      break;
    }
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  return result;
}

AxisProfileResult SercosParamAccess::readSercosProcedureStatus(ProcedureCommands paramIDN,
                                                               RequestStatemachine& requestState,
                                                               CmdStatusInDevice& status,
                                                               bool checkAKbit)
{
  // AK bit is a realtime bit in status work to indicates that command procedure is completed or not
  if (checkAKbit)
  {
    auto statusWord = m_sharingHandler->getStatusControlIF()->getStatusWord();
    bool cmdChanged = (statusWord & (1 << SERCOS_STSWORD_PROCEDURE_CMDBIT_OFFSET)) != 0;

    // AK bit not changed which means status not changed, we may return and check it next cycle
    if (!cmdChanged)
    {
      return AxisProfileResult::STS_PENDING;
    }
  }

  std::uint16_t cmdStatus {0xFFFF};
  auto result = readAndCheckParamStateAsync(static_cast<std::uint16_t>(paramIDN), &cmdStatus, sizeof(cmdStatus), requestState);
  if (AxisProfileResult::STS_OK == result)
  {
    switch (cmdStatus)
    {
      case SERCOS_COMMAND_STATUS_NOT_SET:      status = CmdStatusInDevice::DEVICE_NOT_SET; break;
      case SERCOS_COMMAND_STATUS_IN_PROCESS:   status = CmdStatusInDevice::DEVICE_IN_PROCESS; break;
      case SERCOS_COMMAND_STATUS_FINISH:       status = CmdStatusInDevice::DEVICE_COMPLETED; break;
      case SERCOS_COMMAND_STATUS_ERROR:        status = CmdStatusInDevice::DEVICE_ERROR; break;
      case SERCOS_COMMAND_STATUS_INTERRUPT:    status = CmdStatusInDevice::DEVICE_INTERRUPTED; break;
      // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
      default:
      {
        APR_TRACE_ERROR("invalid command status read from device.");
        result = AxisProfileResult::ERR_NOK;
        break;
      }
      // LCOV_EXCL_STOP - exclude code coverage section end
    }
  }
  return result;
}
