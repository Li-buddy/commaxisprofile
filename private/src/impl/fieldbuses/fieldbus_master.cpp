/// @file
///
/// @brief This file is part of the axisprofile library and implement class of fieldbus master.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/debug_trace.h"
#include "fieldbus_master.h"
#include "helper/datalayer_helper.h"
#include "diagnosis/dia.h"
#include "profiles/i_axisprofile_internal.h"
// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------
const std::map<MasterRtDataID, std::string> FieldbusMaster::MASTER_RT_DATA
{
  // data id                                fixed key value of RT element
  {MasterRtDataID::TASK_START_TIME, "Master/Task.StartTime"},
  {MasterRtDataID::TASK_CYCLE_TIME, "Master/Task.CycleTime"},
  {MasterRtDataID::OUTPUT_SHIFT_TIME, "Master/DistributedClocks.OutputShiftTime"},
  {MasterRtDataID::SEND_OFFSET, "Master/DistributedClocks.SendOffset"},
};

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
FieldbusMaster::FieldbusMaster(const char* name, comm::datalayer::IDataLayerFactory* datalayer)
  : m_masterName(name), m_datalayer(datalayer)
{
  m_rtDataInfo.m_bufferURI = DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + "/" + m_masterName + DL_SUB_ADDRESS_RTDATA_INPUT_MASTER;
  for (auto& it : MASTER_RT_DATA)
  {
    m_rtDataInfo.m_memoryMaps.emplace_back(it.first, it.second.c_str());
  }
}

FieldbusMaster::~FieldbusMaster()
{
  if (m_datalayer && m_rtDataInfo.m_memoryUser)
  {
    m_datalayer->closeMemory(m_rtDataInfo.m_memoryUser);
  }
  m_profiles.clear();
}

const char* FieldbusMaster::getMasterName()
{
  return m_masterName.c_str();
}

const std::set<IAxisProfileInternal*>& FieldbusMaster::getLinkedProfiles()
{
  return m_profiles;
}

void FieldbusMaster::addLinkedProfile(IAxisProfileInternal* profile)
{
  m_profiles.insert(profile);
}

AxisProfileResult FieldbusMaster::initRtData(bool abort)
{
  if (!m_datalayer)
  {
    APR_TRACE_ERROR("Datalayer factory is empty on %s.", m_masterName.c_str());
    return AxisProfileResult::ERR_DATALAYER;
  }

  bool profileActivated = false;
  const auto&  linkedProfiles = getLinkedProfiles();
  for (auto profile : linkedProfiles)
  {
    if (profile->checkProfileActivated())
    {
      profileActivated = true;
    }
  }
  if (!profileActivated)
  {
    return AxisProfileResult::STS_OK;
  }

  comm::datalayer::DlResult dlRresult = m_datalayer->openMemory(m_rtDataInfo.m_memoryUser, m_rtDataInfo.m_bufferURI);
  if (comm::datalayer::STATUS_FAILED(dlRresult))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_RT_MEMORY, __FILE__, __FUNCTION__, __LINE__,
                       "Not able to open memory with datalayer RT address: %s, error: %s", m_rtDataInfo.m_bufferURI.c_str(), dlRresult.toString());
    m_rtDataInfo.m_memoryUser = nullptr;
    return AxisProfileResult::STS_DATALAYER_RT_NOT_OPEN;
  }

  // get memory map of datalayer-rt
  APR_TRACE_MESSAGE("Get RT memory map of fieldbus master %s.", m_masterName.c_str());
  GetRtMemMapCallback masterMemMapCallback = [this] (RtMemInfoBase* memInfo, const comm::datalayer::MemoryMap* memMap)
    {
      auto& varMaps = static_cast<MasterRtDataInfo*>(memInfo)->m_memoryMaps;
      auto variables = memMap->variables();
      for (auto& varMap : varMaps)
      {
        auto var = variables->LookupByKey(varMap.m_rtKey);
        if (var)
        {
          varMap.m_memLocation.m_bitOffset = var->bitoffset();
          varMap.m_memLocation.m_bitSize = var->bitsize();
          APR_TRACE_MESSAGE("%s; RtDataInfo: ID=%s, Key=%s, BitOffset=%u, BitSize=%u",
                            m_masterName.c_str(),
                            getMasterRtDataIDString(varMap.m_id),
                            varMap.m_rtKey,
                            varMap.m_memLocation.m_bitOffset,
                            varMap.m_memLocation.m_bitSize);
        }
      }
      return AxisProfileResult::STS_OK;
    };

  auto result = DatalayerHelper::getMemoryMap(m_rtDataInfo, masterMemMapCallback, abort);
  if (aprStatusNotOkay(result))
  {
    return result;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusMaster::ProcessRtData()
{
  m_rtDataRawValue = {0, 0, 0 , 0}; // clear raw date before copying

  if (m_rtDataInfo.m_memoryUser == nullptr)
  {
    return AxisProfileResult::STS_OK;
  }

  std::uint8_t* data;
  comm::datalayer::DlResult dlResult = m_rtDataInfo.m_memoryUser->beginAccess(data, m_rtDataInfo.m_revision);
  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    if (!m_traceErrorAlreadySent)
    {
      APR_TRACE_ERROR("Could not begin access of RT-Memory %s on %s; DlResult: %s",
                      m_rtDataInfo.m_bufferURI.c_str(), m_masterName.c_str(), dlResult.toString());
      m_traceErrorAlreadySent = true;
    }
    return AxisProfileResult::ERR_DATALAYER;
  }

  const auto& varMaps = m_rtDataInfo.m_memoryMaps;
  for (const auto& varMap : varMaps)
  {
    // copy to local structure
    copyValueFromRtMemroy(data, varMap);
  }

  dlResult = m_rtDataInfo.m_memoryUser->endAccess();
  if (comm::datalayer::DlResult::DL_OK != dlResult)
  {
    if (!m_traceErrorAlreadySent)
    {
      APR_TRACE_ERROR("Could not end access of RT-Memory %s on %s; DlResult: %s",
                      m_rtDataInfo.m_bufferURI.c_str(), m_masterName.c_str(), dlResult.toString());
      m_traceErrorAlreadySent = true;
    }
    return AxisProfileResult::ERR_DATALAYER;
  }

  m_traceErrorAlreadySent = false;

  covertRtDataRawToScaled();

  m_actDcBaseTime = m_rtDataSIValue.m_taskStartTime + m_rtDataSIValue.m_outputShiftTime;
  return AxisProfileResult::STS_OK;
}

double FieldbusMaster::getActDcBaseTime()
{
  return m_actDcBaseTime;
}

double FieldbusMaster::getFieldbusCycleTime()
{
  return m_rtDataSIValue.m_taskCycleTime;
}

void FieldbusMaster::copyValueFromRtMemroy(std::uint8_t* rawData, const MasterRtDataMap& memInfo)
{
  std::uint8_t* data {nullptr};
  switch (memInfo.m_id)
  {
    case MasterRtDataID::TASK_START_TIME:
      data = reinterpret_cast<std::uint8_t*>(&(m_rtDataRawValue.m_taskStartTimeNs));
      break;
    case MasterRtDataID::TASK_CYCLE_TIME:
      data = reinterpret_cast<std::uint8_t*>(&(m_rtDataRawValue.m_taskCycleTimeNs));
      break;
    case MasterRtDataID::OUTPUT_SHIFT_TIME:
      data = reinterpret_cast<std::uint8_t*>(&(m_rtDataRawValue.m_outputShiftTimeNs));
      break;
    case MasterRtDataID::SEND_OFFSET:
      data = reinterpret_cast<std::uint8_t*>(&(m_rtDataRawValue.m_sendOffsetNs));
      break;
    default: return;
  }
  DatalayerHelper::copyBits(data, 0, rawData, memInfo.m_memLocation.m_bitOffset, memInfo.m_memLocation.m_bitSize);
}

void FieldbusMaster::covertRtDataRawToScaled()
{
  m_rtDataSIValue.m_taskStartTime = NANOSECOND_2_SECOND(m_rtDataRawValue.m_taskStartTimeNs);
  m_rtDataSIValue.m_taskCycleTime = NANOSECOND_2_SECOND(m_rtDataRawValue.m_taskCycleTimeNs);
  m_rtDataSIValue.m_outputShiftTime = NANOSECOND_2_SECOND(m_rtDataRawValue.m_outputShiftTimeNs);
  m_rtDataSIValue.m_sendOffset = NANOSECOND_2_SECOND(m_rtDataRawValue.m_sendOffsetNs);
}
