/// @file
///
/// @brief Common abstract base class for protocol-specific boot parameter snapshots.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "boot_param_snapshot_base.h"

#include <cstring>
#include <unordered_map>

#include "diagnosis/dia.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

void BootParamSnapshotBase::setBootParamSnapshot(const BootParamSnapshot& snapshot)
{
  m_snapshotConfigured = (m_snapshot.protocol == snapshot.protocol);
  if (!m_snapshotConfigured)
  {
    APR_TRACE_WARNING("BootParamSnapshot protocol is not consistent with the class: %d, %d.", snapshot.protocol, m_snapshot.protocol);
  }
  m_snapshot = snapshot;
  m_snapshotDirty = false;
}

const BootParamSnapshot& BootParamSnapshotBase::getBootParamSnapshot() const
{
  return m_snapshot;
}

bool BootParamSnapshotBase::isSnapshotDirty() const
{
  return m_snapshotDirty;
}

void BootParamSnapshotBase::markSnapshotPersisted()
{
  m_snapshotDirty = false;
}

BootParamSnapshotBase::BootParamSnapshotBase(ProfileBasicInfo& profileBasicInfo, BootSnapshotProtocol protocol)
  : m_profileBasicInfo(profileBasicInfo)
{
  m_snapshot.profileName = m_profileBasicInfo.m_profileName;
  m_snapshot.protocol = protocol;
}

void BootParamSnapshotBase::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

AxisProfileResult BootParamSnapshotBase::getParameterImpl(const BootParamId& paramId, std::uint8_t* bufPtr, std::size_t bufLen, bool forceReadFromDrive)
{
  std::size_t retLen = 0;
  return getParameterImpl(paramId, bufPtr, bufLen, retLen, forceReadFromDrive);
}

AxisProfileResult BootParamSnapshotBase::getParameterImpl(const BootParamId& paramId, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& retLen, bool forceReadFromDrive)
{
  const std::string paramStr = parameterToString(paramId);
  auto it = m_snapshot.paramMap.find(paramStr);
  if (!forceReadFromDrive && hasValidSnapshot() && it != m_snapshot.paramMap.end())
  {
    const BootParamItem& cacheItem = it->second;
    retLen = (cacheItem.type == BootParamValueType::BYTEARRAY) ? cacheItem.value.valueByteArray.size() : getBootParamDataLength(cacheItem.type);
    APR_TRACE_MESSAGE("get parameter: %s from snapshot cache, len:%zu.", paramStr.c_str(), retLen);
    return readBootParameterFromCache(cacheItem, bufPtr, bufLen);
  }
  return readBootParameterFromDrive(paramId, bufPtr, bufLen, retLen);
}

bool BootParamSnapshotBase::hasValidSnapshot() const
{
  return m_snapshotConfigured;
}

AxisProfileResult BootParamSnapshotBase::readBootParameterFromDrive(const BootParamId& paramId, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& retLen)
{
  const std::string paramStr = parameterToString(paramId);
  const BootParamDescriptorBase* descriptor = getBootParamDescriptor(paramId);
  if (descriptor == nullptr)
  {
    APR_TRACE_WARNING("unknown boot parameter: %s, not in booting list.", paramStr.c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  if (descriptor->m_type != BootParamValueType::BYTEARRAY && bufLen < descriptor->m_valueLen)
  {
    APR_TRACE_WARNING("bufLen size error when read boot parameter: %s, size:%zu, expected size:%zu", paramStr.c_str(), bufLen, descriptor->m_valueLen);
    return AxisProfileResult::ERR_WRONGDATASIZE;
  }

  APR_TRACE_MESSAGE("read parameter: %s from drive.", paramStr.c_str());
  std::unique_ptr<ParamReqDataBase> requestData = generateReqData(paramId);
  if (requestData == nullptr)
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  retLen = descriptor->m_valueLen;
  AxisProfileResult result = AxisProfileResult::STS_OK;
  if (descriptor->m_type == BootParamValueType::BYTEARRAY)
  {
    result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(*requestData, bufPtr, bufLen, retLen);   
  }
  else
  {
    result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(*requestData, bufPtr, bufLen);
  }
  
  if (aprStatusFailed(result))
  {
    return result;
  }

  auto& item = m_snapshot.paramMap[paramStr];
  item.type = descriptor->m_type;
  // LCOV_EXCL_START
  const std::unordered_map<BootParamValueType, void*> copyTargetMap {
    {BootParamValueType::UNSIGNED16, &item.value.valueUint16},
    {BootParamValueType::SIGNED16,   &item.value.valueInt16},
    {BootParamValueType::UNSIGNED32, &item.value.valueUint32},
    {BootParamValueType::SIGNED32,   &item.value.valueInt32},
  };
  // LCOV_EXCL_STOP
  if (descriptor->m_type == BootParamValueType::BYTEARRAY)
  {
    item.value.valueByteArray.assign(bufPtr, bufPtr + retLen);
  }
  else if (auto it = copyTargetMap.find(descriptor->m_type); it != copyTargetMap.end())
  {
    std::memcpy(it->second, bufPtr, retLen);
  }
  else
  {
    APR_TRACE_WARNING("unknown boot parameter value type: %d, %s.", descriptor->m_type, paramStr.c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  m_snapshotDirty = true;

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BootParamSnapshotBase::readBootParameterFromCache(const BootParamItem& cache, std::uint8_t* bufPtr, std::size_t bufLen)
{
  const std::unordered_map<BootParamValueType, std::pair<const void*, std::size_t>> copySourceMap {
    {BootParamValueType::UNSIGNED16, {&cache.value.valueUint16,          sizeof(cache.value.valueUint16)}},
    {BootParamValueType::SIGNED16,   {&cache.value.valueInt16,           sizeof(cache.value.valueInt16)}},
    {BootParamValueType::UNSIGNED32, {&cache.value.valueUint32,          sizeof(cache.value.valueUint32)}},
    {BootParamValueType::SIGNED32,   {&cache.value.valueInt32,           sizeof(cache.value.valueInt32)}},
    {BootParamValueType::BYTEARRAY,  {cache.value.valueByteArray.data(), cache.value.valueByteArray.size()}},
  };

  auto srcIt = copySourceMap.find(cache.type);
  if (srcIt == copySourceMap.end())
  {
    APR_TRACE_WARNING("unknown value type in snapshot cache: %d", cache.type);
    return AxisProfileResult::ERR_WRONGDATATYPE;
  }

  const auto& [srcPtr, srcSize] = srcIt->second;
  if (bufLen < srcSize)
  {
    APR_TRACE_WARNING("bufLen too small, value type: %d, size: %zu, expected: %zu", cache.type, bufLen, srcSize);
    return AxisProfileResult::ERR_WRONGDATASIZE;
  }

  if (srcSize > 0)
  {
    std::memcpy(bufPtr, srcPtr, srcSize);
  }

  return AxisProfileResult::STS_OK;
}

void BootParamSnapshotBase::updateChecksumValue(std::uint32_t checksum)
{
  m_snapshot.checksumValue = checksum;
  m_snapshotDirty = true;
} 

std::size_t BootParamSnapshotBase::getBootParamDataLength(BootParamValueType type) const
{
  switch (type)
  {
    case BootParamValueType::UNSIGNED16:
    case BootParamValueType::SIGNED16:
      return sizeof(std::uint16_t);
    case BootParamValueType::UNSIGNED32:
    case BootParamValueType::SIGNED32:
      return sizeof(std::uint32_t);
    case BootParamValueType::BYTEARRAY:
      return 0;
    default:
      return 0;
  }
}
