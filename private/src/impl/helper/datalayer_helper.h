#pragma once

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
#include "fieldbuses/fieldbus_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  namespace helper
  {
    constexpr std::uint32_t GET_DL_RT_MEM_MAP_SLEEP_MS = 250; ///< sleep time when getting memory map, unit in millisecond
    constexpr std::uint32_t GET_DL_RT_MEM_MAP_TIMEOUT_CNT = 20; ///< getting memory map retry count
  }

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  /// @brief Simple inline method to get the minimum of two values
  /// @param[in]  value1   first value to compare
  /// @param[in]  value2   second value to compare
  /// @return      returns the minimum of the given values.
  inline std::uint32_t minimumValue(std::uint32_t value1, std::uint32_t value2)
  {
    return ((((value1) < (value2)) ? (value1) : (value2)));
  };

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is class datalayer helper
  class DatalayerHelper
  {
  public: // methods

    /// @brief Constructor
    DatalayerHelper() = default;
    /// @brief Destructor
    virtual ~DatalayerHelper() noexcept = default;

    /// @brief This method get memory map of datalayer RT, 
    /// @param[in, out] memInfo     Memory information
    /// @param[in]      callback    Callback function when get memory map complete
    /// @param[in]      abort       Set true to abort getting memory map
    /// @param[in]      sleepMS     Sleep time in millisecond polling on getting the map
    /// @return Return value indicating success or error.
    static AxisProfileResult getMemoryMap(RtMemInfoBase& memInfo, GetRtMemMapCallback callback,
                                          const bool& abort, std::uint32_t sleepMS = helper::GET_DL_RT_MEM_MAP_SLEEP_MS);

    /// @brief This method copies data from source to destination bitwise.
    /// @param[in,out]   destination              Pointer to destination the data should be copied.
    /// @param[in]       destinationBitOffset     Offset for the position the data should be copied (in bits).
    /// @param[in]       source                   Pointer to the source of the data.
    /// @param[in]       sourceBitOffset          Offset to the position of the data to copy in the source pointer (in bits).
    /// @param[in]       bitSize                  Size of the data (in bits).
    static void copyBits(std::uint8_t* destination, std::uint32_t destinationBitOffset, std::uint8_t* source, std::uint32_t sourceBitOffset, std::uint32_t bitSize);
  };
}
