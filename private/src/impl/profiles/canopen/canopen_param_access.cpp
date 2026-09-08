/// @file
///
/// @brief This header file is part of the axisprofile library and implements the
/// handler method for async access of canopen over ethercat.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 

// --------------------------------------------------------------------------- 
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_param_access.h"
#include "fieldbuses/fieldbus_defs.h"

// --------------------------------------------------------------------------- 
// Name Spaces
// --------------------------------------------------------------------------- 
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

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

CanopenParamAccess::CanopenParamAccess(ProfileBasicInfo& profileBasicInfo)
  :AxisProfileParamAccess(profileBasicInfo)
{
}

AxisProfileResult CanopenParamAccess::readAndCheckParamAsync(std::uint16_t objectIndex,
                                                             std::uint8_t subIndex,
                                                             void* buffAddr,
                                                             const size_t buffSize,
                                                             RequestStatemachine& requestState)
{
  ParamReqDataCoE readParam(m_profileBasicInfo.m_deviceAddress, {objectIndex, subIndex});
  return AxisProfileParamAccess::readAndCheckParamAsync(readParam, buffAddr, buffSize, requestState);
}

AxisProfileResult CanopenParamAccess::writeAndCheckParamAsync(std::uint16_t objectIndex,
                                                              std::uint8_t subIndex,
                                                              void* buffAddr,
                                                              const size_t buffSize,
                                                              RequestStatemachine& requestState)
{
  ParamReqDataCoE writeParam(m_profileBasicInfo.m_deviceAddress, {objectIndex, subIndex}, static_cast<std::uint8_t*>(buffAddr), static_cast<uint32_t>(buffSize));
  return AxisProfileParamAccess::writeAndCheckParamAsync(writeParam, requestState);
}

AxisProfileResult CanopenParamAccess::accessAndCheckParamListAsync(std::list<CanopenParamStruct>& paramList,
                                                                   RequestStatemachine& requestState)
{
  AxisProfileResult result;

  if (paramList.empty())
  {
    return AxisProfileResult::STS_OK;
  }

  CanopenParamStruct const & param = paramList.front();
  if (param.m_readWriteType == ReadWriteType::READ)
  {
    result = readAndCheckParamAsync(param.m_objDirEntry.index, param.m_objDirEntry.subIndex, param.m_ptrData, param.dataSize, requestState);
  }
  else
  {
    result = writeAndCheckParamAsync(param.m_objDirEntry.index, param.m_objDirEntry.subIndex, param.m_ptrData, param.dataSize, requestState);
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

AxisProfileResult CanopenParamAccess::accessAndCheckParamListAsync(std::list<canopen::CanopenParamStruct>& paramList,
                                                                   AsyncCommandStatemachine& asyncCmdState)
{
  return accessAndCheckParamListAsync(paramList, asyncCmdState.getRequestState());
}
