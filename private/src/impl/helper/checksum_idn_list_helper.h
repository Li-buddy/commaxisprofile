#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the helper class to operate checksum IDN list .
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "common/debug_trace.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <string>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
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
  // Forward references
  // ---------------------------------------------------------------------------


  /// @brief This is helper class for checksum IDN list operation.
  class ChecksumIdnListHelper
  {
  public: // method

    /// @brief This method indicate whether the checksum idn list parameters in drive has been modified, namely whether the idn list in drive is different to the
    ///        default booting idn list. When the value not same, update the default booting idn list to drive
    /// @tparam IdnListType       IDN list type (e.g., vector<uint16_t>, vector<ObjectDirEntry>)
    /// @param  readFunc          the function of read IDN list from device
    /// @param  writeFunc         the function of write IDN list to device
    /// @param  deviceAddress     profile's device address
    /// @return                   Return value indicating whether the checksum idn list parameters in drive is different to the default booting idn list.
    template <typename IdnListType>
    static bool checkBootIdnListModifiedImpl(IdnListType& deviceList, const IdnListType& defaultList,
                                             std::function<AxisProfileResult(IdnListType&)> readFunc,
                                             std::function<AxisProfileResult(const IdnListType&)> writeFunc,
                                             uint32_t deviceAddress)
    {
      //1. read checksum idn list
      AxisProfileResult res = readFunc(deviceList);
      if (aprStatusFailed(res))
      {
        APR_TRACE_WARNING("device address: %u, get readChecksumIdnListFromDevice error: 0x%X", deviceAddress, res);
        return true;
      }

      //2. check whether checksum value is same with the value in device 
      if (defaultList == deviceList)
      {
        APR_TRACE_MESSAGE("device address: %u, check checksum idn list, the idn list of devices is same with default list", deviceAddress);
        return false;
      }

      //3. update the boot idn list parameters list to device
      APR_TRACE_MESSAGE("device address: %u, the idn list in devices is different to default list, update the idn list. size:%d, %d",
                        deviceAddress, deviceList.size(), defaultList.size());
      res = writeFunc(defaultList);
      if (aprStatusFailed(res))
      {
        APR_TRACE_WARNING("device address: %u, writeChecksumIdnListToDevice error: 0x%X", deviceAddress, res);
        return false;
      }
      deviceList = defaultList; //update return idn list
      return true;
    }
    
    /// @brief This method convert an IDN list to a list of strings
    /// @tparam IdnType        IDN type (e.g., uint16_t, ObjectDirEntry)
    /// @param  sourceList     source IDN list
    /// @param  idnStrListRet  output string list
    /// @param  transformFunc  transform function
    /// @return                AxisProfileResult::STS_OK
    template <typename IdnType>
    static AxisProfileResult getChecksumIdnListStrImpl(const std::vector<IdnType>& sourceList, std::vector<std::string>& idnStrListRet, std::function<std::string(IdnType)> transformFunc)
    {
      idnStrListRet.clear();
      idnStrListRet.reserve(sourceList.size());
      std::transform(sourceList.begin(), sourceList.end(), std::back_inserter(idnStrListRet), transformFunc);
      return AxisProfileResult::STS_OK;
    }

  };
}

