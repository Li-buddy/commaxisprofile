/// @file
///
/// @brief This source file is part of the axisprofile library and implements
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
#include "axisprofile_param_access.h"
#include "common/debug_trace.h"
#include "diagnosis/dia.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

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

AxisProfileResult AxisProfileParamAccess::writeParamAsync(ParamReqDataBase& writeParamData, RequestStatemachine& requestState)
{
  requestState.resetCallbackData();

  APR_TRACE_MESSAGE("%s; async write request started", m_profileBasicInfo.m_profileTrace.createTraceMsg()); // NOTE: createTraceMsg() contains profile name

  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->writeSingleParamAsync(requestState.getCallbackData(), writeParamData);
  if (aprStatusFailed(result))
  {
    APR_TRACE_MESSAGE("%s; async write request failed", m_profileBasicInfo.m_profileTrace.createTraceMsg());
  }
  else
  {
    requestState.switchToNextRequestState();
    result = AxisProfileResult::STS_PENDING;
  }

  requestState.reinitCallbackTick(); // call back timeout check start

  return result;
}

AxisProfileResult AxisProfileParamAccess::writeParamAsync(ParamReqDataBase& writeParamData, AsyncCommandStatemachine& cmdState)
{
  RequestStatemachine& requestState = cmdState.getRequestState();
  return writeParamAsync(writeParamData, requestState);
}

AxisProfileResult AxisProfileParamAccess::readParamAsync(ParamReqDataBase& readParam, RequestStatemachine& requestState)
{
  requestState.resetCallbackData();
  APR_TRACE_MESSAGE("%s; async read request started", m_profileBasicInfo.m_profileTrace.createTraceMsg());
  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamAsync(requestState.getCallbackData(), readParam);
  if (aprStatusFailed(result))
  {
    APR_TRACE_MESSAGE("%s; async read request failed", m_profileBasicInfo.m_profileTrace.createTraceMsg());
  }
  else
  {
    requestState.switchToNextRequestState();
    result = AxisProfileResult::STS_PENDING;
  }

  requestState.reinitCallbackTick(); // call back timeout check start
  return result;
}

AxisProfileResult AxisProfileParamAccess::readParamAsync(ParamReqDataBase& readParam, AsyncCommandStatemachine& cmdState)
{
  RequestStatemachine& requestState = cmdState.getRequestState();
  return readParamAsync(readParam, requestState);
}

AxisProfileResult AxisProfileParamAccess::checkResponseAsync(RequestStatemachine& requestState)
{
  AxisProfileResult result = AxisProfileResult::STS_PENDING;
  if (requestState.tickCallbackAndCheckExceed())
  {
    Dia::sendDiagnosis(MAIN_DIAG_PROFILE_ERR, DETAILED_DIAG_PROFILE_CMD_TIMEOUT, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "Timeout, check callback.");
    return AxisProfileResult::ERR_TIMEOUT;
  }

  auto callbackData = requestState.getCallbackData();
  if (callbackData->m_requestFinishedFlag)
  {
    // Parameter request finished, check if it was successful
    if (comm::datalayer::STATUS_FAILED(callbackData->m_dlResult))
    {
      APR_TRACE_ERROR("%s; async request failed", m_profileBasicInfo.m_profileTrace.createTraceMsg(AxisProfileResult::ERR_DATALAYER, callbackData->m_dlResult.toString()));
      result = AxisProfileResult::ERR_DATALAYER;
    }
    else
    {
      APR_TRACE_MESSAGE("%s; async request finished", m_profileBasicInfo.m_profileTrace.createTraceMsg());
      requestState.switchToNextRequestState();
    }
  }
  return result;
}

AxisProfileResult AxisProfileParamAccess::readAndCheckParamAsync(ParamReqDataBase& readParam,
                                                                 void* buffAddr,
                                                                 const size_t buffSize,
                                                                 RequestStatemachine& requestState)
{
  // pending means not finish yet
  // pre process means not begin to execute
  // the state here should be able to return to motion.core
  AxisProfileResult result = AxisProfileResult::STS_PENDING; // here should be pre-process means wait in operation queue, haven't handle yet.
  switch (requestState.getRequestState())
  {
    case RequestStatemachine::RequestStates::SEND_REQ:
    {
      result = readParamAsync(readParam, requestState);     // requestSatet change to statemachine, in order to get priority
      break;
    }
    case RequestStatemachine::RequestStates::WAIT_FOR_REQ:  // trigger about 4 times
    {
      result = checkResponseAsync(requestState);
      break;
    }
    case RequestStatemachine::RequestStates::DONE_REQ:      // trigger only once
    {
      result = m_profileBasicInfo.m_fieldbusAdapter->getParamResponseData(requestState.getCallbackDlVar(),
                                                                          static_cast<std::uint8_t*>(buffAddr),
                                                                          static_cast<std::uint32_t>(buffSize));
      if (aprStatusFailed(result))
      {
        APR_TRACE_ERROR("%s; get response data failed", m_profileBasicInfo.m_profileTrace.createTraceMsg());
      }
      break;
    }
    default:
    {
      result = AxisProfileResult::ERR_NOK;
      break;
    }
  }

  if (result != AxisProfileResult::STS_PENDING)
  {
    requestState.resetRequestState();
  }

  return result;
}

AxisProfileResult AxisProfileParamAccess::writeAndCheckParamAsync(ParamReqDataBase& writeParam,
                                                                  RequestStatemachine& requestState)
{
  AxisProfileResult result = AxisProfileResult::STS_PENDING;

  switch (requestState.getRequestState())
  {
    case RequestStatemachine::RequestStates::SEND_REQ:
    {
      result = writeParamAsync(writeParam, requestState);
      break;
    }
    case RequestStatemachine::RequestStates::WAIT_FOR_REQ:
    {
      result = checkResponseAsync(requestState);
      break;
    }
    case RequestStatemachine::RequestStates::DONE_REQ:
    {
      result = AxisProfileResult::STS_OK;
      break;
    }
    default:
    {
      result = AxisProfileResult::ERR_NOK;
      break;
    }
  }

  if (result != AxisProfileResult::STS_PENDING)
  {
    requestState.resetRequestState();
  }
  return result;
}

void AxisProfileParamAccess::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}
