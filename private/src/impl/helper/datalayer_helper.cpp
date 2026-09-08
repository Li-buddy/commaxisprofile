/// @file
///
/// @brief This file is part of the axisprofile library and implements datalayer helper class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <thread>
#include <chrono>
#include "diagnosis/dia.h"
#include "datalayer_helper.h"
#include "common/debug_trace.h"

// External Includes
#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/datalayer_rt.h"

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
AxisProfileResult DatalayerHelper::getMemoryMap(RtMemInfoBase& memInfo,
                                                GetRtMemMapCallback callback,
                                                const bool& abort, std::uint32_t sleepMS)
{
  auto start = std::chrono::system_clock::now();
  comm::datalayer::Variant memoryMapVariant;
  comm::datalayer::DlResult resultMemMap = memInfo.m_memoryUser->getMemoryMap(memoryMapVariant);
  std::int8_t tryCnt = helper::GET_DL_RT_MEM_MAP_TIMEOUT_CNT;
  while (resultMemMap != comm::datalayer::DlResult::DL_OK && tryCnt > 0)
  {
    if (abort)
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_ABORTED,
                         __FILE__, __FUNCTION__, __LINE__,
                         "Getting memory is aborted by UI or datalayer!");
      return AxisProfileResult::STS_BOOTING_ABORT;
    }
    if (resultMemMap != comm::datalayer::DlResult::DL_RT_INVALIDMEMORYMAP)
    {

      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_RT_MEMORY,
                         __FILE__, __FUNCTION__, __LINE__, "Not able to read memory-map of memory: %s, error: %s",
                         memInfo.m_bufferURI.c_str(), resultMemMap.toString());
      return AxisProfileResult::ERR_DLRT;
    }

    // The RT memory is not yet available, try again...
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepMS));
    tryCnt--;
    APR_TRACE_MESSAGE("Sleep %d ms, url: %s, resultMemMap:%#010x, %s", sleepMS, memInfo.m_bufferURI.c_str(), resultMemMap, resultMemMap.toString());
    resultMemMap = memInfo.m_memoryUser->getMemoryMap(memoryMapVariant);
  }

  if (tryCnt == 0) //resultMemMap = comm::datalayer::DlResult::DL_RT_INVALIDMEMORYMAP
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_FIELDBUS_RT_MEMORYMAP, __FILE__, __FUNCTION__, __LINE__, "Url: %s: result: %s",
                       memInfo.m_bufferURI.c_str(), resultMemMap.toString());
    return AxisProfileResult::ERR_DLRT;
  }

  auto end = std::chrono::system_clock::now();
  std::int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  APR_TRACE_MESSAGE("Received memory maps of datalayer RT, url: %s! Elapsed time: %lld ms", memInfo.m_bufferURI.c_str(), duration);

  // check if memory map is valid flatbuffer
  auto result = memoryMapVariant.verifyFlatbuffers(comm::datalayer::VerifyMemoryMapBuffer);
  if (comm::datalayer::DlResult::DL_OK != result)
  {

    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_FLATBUFFER,
                       __FILE__, __FUNCTION__, __LINE__, "Validation of memory-map failed for memory: %s, error: %s",
                       memInfo.m_bufferURI.c_str(), result.toString());
    return AxisProfileResult::ERR_DLRT;
  }

  // read the data
  auto memMap = comm::datalayer::GetMemoryMap(memoryMapVariant.getData());
  // save revision
  memInfo.m_revision = memMap->revision();

  if (callback)
  {
    return callback(&memInfo, memMap);
  }

  return AxisProfileResult::STS_OK;
}

void DatalayerHelper::copyBits(std::uint8_t* destination, std::uint32_t destinationBitOffset, std::uint8_t* source, std::uint32_t sourceBitOffset, std::uint32_t bitSize)
{
  std::uint32_t remainingLength = bitSize;   // remaining length
  std::uint8_t  bitMask = 0x00;
  std::uint32_t numberOfBitsToCopy = 0;
  std::uint8_t* srcHelper = NULL;
  std::uint16_t srcWorkingCopy = 0;

  if (bitSize == 0)
  {
    return;
  }
  // how many bits we need for the next "destination" std::uint8_t 
  numberOfBitsToCopy = minimumValue((8 - (destinationBitOffset & 7)), remainingLength);

  bitMask = (std::uint8_t)((1 << numberOfBitsToCopy) - 1);
  bitMask = (std::uint8_t)(bitMask << (destinationBitOffset & 7));

  //copy first std::uint8_t
  srcHelper = &source[sourceBitOffset >> 3U];  // sourceBitOffset / 8
  if (sourceBitOffset + bitSize <= 8)
  {
    srcWorkingCopy = (std::uint16_t)srcHelper[0];
  }
  else
  {
    srcWorkingCopy = (std::uint16_t)((std::uint16_t)srcHelper[0] | (((std::uint16_t)srcHelper[1]) << 8)); // xxxx xxxx xx-- ----  e. g. nSrcBitOffs=6, nDstBitOffs=2 
  }
  srcWorkingCopy = (std::uint16_t)(srcWorkingCopy >> (sourceBitOffset & 7));                    // 0000 00xx xxxx xxxx
  srcWorkingCopy = (std::uint16_t)(srcWorkingCopy & ((1 << numberOfBitsToCopy) - 1));

  destination = &destination[destinationBitOffset >> 3];  // destinationBitOffset / 8
  *destination = (std::uint8_t)((*destination & ~bitMask) | (srcWorkingCopy << (destinationBitOffset & 7)));
  destination++;

  sourceBitOffset = (std::uint32_t)(sourceBitOffset + numberOfBitsToCopy);
  remainingLength -= numberOfBitsToCopy;

  while (remainingLength > 0)
  {
    numberOfBitsToCopy = minimumValue(8, remainingLength);

    bitMask = (std::uint8_t)((1 << numberOfBitsToCopy) - 1);

    srcHelper = &source[sourceBitOffset >> 3U]; // sourceBitOffset/8

    // the last byte to be copied, not consider higher byte fill with 0
    std::uint16_t tmp;
    tmp = remainingLength > 8 ? srcHelper[1] : 0;
    srcWorkingCopy = (std::uint16_t)(srcHelper[0] | (tmp << 8));
    srcWorkingCopy = (std::uint16_t)(srcWorkingCopy >> (sourceBitOffset & 7));
    srcWorkingCopy = (std::uint16_t)(srcWorkingCopy & (std::uint16_t)bitMask);

    *destination = (std::uint8_t)((*destination & ~bitMask) | (srcWorkingCopy));
    destination++;

    sourceBitOffset = (std::uint32_t)(sourceBitOffset + numberOfBitsToCopy);
    remainingLength -= numberOfBitsToCopy;
  }
}
