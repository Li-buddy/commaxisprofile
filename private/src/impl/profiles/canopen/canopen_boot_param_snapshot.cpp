/// @file
///
/// @brief This file is part of the axisprofile library and implements the Canopen
/// boot parameter snapshot reader.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_boot_param_snapshot.h"

#include <memory>

#include "common/debug_trace.h"
#include "profiles/canopen/canopen_boot_params.h"
#include "profiles/category/drive/axisprofile_device_param_drive.h"
#include "profiles/data_sharing/i_sharing_handler.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

CanopenBootParamSnapshot::CanopenBootParamSnapshot(ProfileBasicInfo& profileBasicInfo)
  : BootParamSnapshotBase(profileBasicInfo, BootSnapshotProtocol::CANOPEN)
{
}

AxisProfileResult CanopenBootParamSnapshot::getParameterValueFromSnapshot(const comm::axisprofile::IAxisProfileSharingHandler* sharingHandler,
                                                                          const char* traceMsg, const ObjectDirEntry& objEntry, std::uint8_t* bufPtr, std::size_t bufLen)
{
  std::size_t retLen = 0;
  return getParameterValueFromSnapshot(sharingHandler, traceMsg, objEntry, bufPtr, bufLen, retLen);
}

AxisProfileResult CanopenBootParamSnapshot::getParameterValueFromSnapshot(const comm::axisprofile::IAxisProfileSharingHandler* sharingHandler, const char* traceMsg,
                                                                       const ObjectDirEntry& objEntry, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen)
{
  if (sharingHandler == nullptr)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto* snapshot = static_cast<CanopenBootParamSnapshot*>(sharingHandler->getBootParamSnapshotIF());
  auto* devParamIF = static_cast<AxisProfileDeviceParamDrive*>(sharingHandler->getDeviceParamIF());
  if (snapshot == nullptr || devParamIF == nullptr)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  APR_TRACE_MESSAGE("%s, begin read CoE drive parameter: [%s] from paramSnapshot.",
                    (traceMsg != nullptr) ? traceMsg : "",
                    objDirToString(objEntry).c_str());

  snapshot->getParameter(objEntry, bufPtr, bufLen, readLen, devParamIF->isNeedReadBootParams());
  if (readLen > bufLen)
  {
    APR_TRACE_WARNING("%s, read CoE drive parameter: [%s] from paramSnapshot, but the read length %zu is larger than the buffer size %zu.",
                      (traceMsg != nullptr) ? traceMsg : "",
                      objDirToString(objEntry).c_str(), readLen, bufLen);
    return AxisProfileResult::ERR_WRONGDATASIZE;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenBootParamSnapshot::getParameter(const ObjectDirEntry& objEntry, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& retLen, bool forceReadFromDrive)
{
  return getParameterImpl(BootParamId(objEntry), bufPtr, bufLen, retLen, forceReadFromDrive);
}

std::unique_ptr<ParamReqDataBase> CanopenBootParamSnapshot::generateReqData(const BootParamId& paramId, std::uint32_t maxLen) const
{
  const auto* objEntry = paramId.getObjectDirEntry();
  if (objEntry == nullptr)
  {
    return nullptr;
  }

  return std::make_unique<ParamReqDataCoE>(m_profileBasicInfo.m_deviceAddress, *objEntry, nullptr, 0, maxLen);
}

const BootParamSnapshotBase::BootParamDescriptorBase* CanopenBootParamSnapshot::getBootParamDescriptor(const BootParamId& paramId) const
{
  const auto* objEntry = paramId.getObjectDirEntry();
  if (objEntry == nullptr)
  {
    return nullptr;
  }

  const BootParamDescriptor* descriptor = getCanopenBootParamDescriptor(*objEntry);
  if (descriptor == nullptr)
  {
    return nullptr;
  }

  m_descriptorCache.m_type = descriptor->type;
  m_descriptorCache.m_valueLen = getBootParamDataLength(descriptor->type);
  return &m_descriptorCache;
}

std::string CanopenBootParamSnapshot::parameterToString(const BootParamId& paramId) const
{
  const auto* objEntry = paramId.getObjectDirEntry();
  return objEntry ? objDirToString(*objEntry) : std::string("invalid-parameter-id");
}
