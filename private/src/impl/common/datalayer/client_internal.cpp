/// @file
///
/// @brief This file is part of the axisprofile library and implements the wrapped datalayer client.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "client_internal.h"
#include "common/debug_trace.h"
#include "component/background_thread/i_background_thread.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::datalayer;
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

ClientInternal::ClientInternal(comm::datalayer::IClient* client)
  : m_base(client)
{
}

IClient* ClientInternal::getBase()
{
  return m_base.get();
}

std::function<void(void)> ClientInternal::getBackgroundFun()
{
  return [this](){return cycleInThreadNRT();};
}

void ClientInternal::enableBackgroundFun(bool enable)
{
  m_enableBkFun.store(enable);
}

DlResult ClientInternal::readAsync(const std::string& address, const Variant& data, const ResponseCallback& callback, const std::string& token)
{
  if (!m_base)
  {
    return DlResult::DL_INVALID_HANDLE;
  }

  // using wrapped call
  if (m_enableBkFun.load())
  {
    m_readList.push({address, data, callback, token});
    return DlResult::DL_OK;
  }

  // using base call
  return getBase()->readAsync(address, data, callback, token);
}

DlResult ClientInternal::writeAsync(const std::string& address, const Variant& data, const ResponseCallback& callback, const std::string& token)
{
  if (!m_base)
  {
    return DlResult::DL_INVALID_HANDLE;
  }

  // using wrapped call
  if (m_enableBkFun.load())
  {
    m_writeList.push({address, data, callback, token});
    return DlResult::DL_OK;
  }

  // using base call
  return getBase()->writeAsync(address, data, callback, token);
}

void ClientInternal::cycleInThreadNRT(void)
{
  // nothing to do when using base call
  if (!m_enableBkFun.load())
  {
    return;
  }

  // write async
  DlAsync oneAsync;
  while (m_writeList.pop(oneAsync))
  {
    // TODO: Time watchdog check added
    DlResult dlResult = m_base->writeAsync(oneAsync.address, oneAsync.data, oneAsync.callback, oneAsync.token);
    if (STATUS_FAILED(dlResult))
    {
      // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen (3 Bit = 8 states)
      APR_TRACE_ERROR("Datalayer writeAsync error %s at url %s", dlResult.toString(), oneAsync.address);
      oneAsync.callback(dlResult, nullptr);
      // LCOV_EXCL_STOP - exclude code coverage section end
    }
  }

  // read async
  while (m_readList.pop(oneAsync))
  {
    // TODO: Time watchdog check added
    DlResult dlResult = m_base->readAsync(oneAsync.address, oneAsync.data, oneAsync.callback, oneAsync.token);
    if (STATUS_FAILED(dlResult))
    {
      // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen (3 Bit = 8 states)
      APR_TRACE_ERROR("Datalayer readAsync error %s at url %s", dlResult.toString(), oneAsync.address);
      oneAsync.callback(dlResult, nullptr);
      // LCOV_EXCL_STOP - exclude code coverage section end
    }
  }
}
