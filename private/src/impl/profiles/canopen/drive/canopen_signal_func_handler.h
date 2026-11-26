#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and declare the canopen signal function handler class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/signal_func/signal_func_handler.h"
#include "fieldbuses/fieldbus_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This class implements the signal function handler for CANOpen profile.
  class CanopenSignalFuncHandler :public SignalFuncHandler
  {
  public:
    explicit CanopenSignalFuncHandler(ProfileBasicInfo& profileBasicInfo);

  private:

    void registDriveSignalWordFuncs() final;

    void registEncoderSignalWordFuncs() final;

    AxisProfileResult readDriveSignalStatusWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits) final;

    AxisProfileResult readDriveSignalControlWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits) final;

    /// @brief This method reads a parameter list from the drive using CANopen over EtherCAT (CoE) protocol.
    ///        It takes an object directory entry, an output list to store the read values, and a maximum number of elements to read.
    /// @param[in]  obj          the object directory entry representing the parameter to read
    /// @param[in]  maxElements  the maximum number of elements to read from the drive
    /// @param[out] outList      the output list to store the read parameter values
    /// @return returns an AxisProfileResult indicating the success or failure of the read operation
    AxisProfileResult readParamListCoE(const ObjectDirEntry& obj, std::size_t maxElements, std::vector<std::uint32_t>& outList);

    /// @brief This method reads a parameter list from the drive using CANopen over EtherCAT (CoE) protocol.
    ///        It takes an object directory entry, an output list to store the read values, and a maximum number of elements to read.
    /// @param[in]  obj          the object directory entry representing the parameter to read
    /// @param[in]  maxElements  the maximum number of elements to read from the drive
    /// @param[out] outList      the output list to store the read parameter values
    /// @return returns an AxisProfileResult indicating the success or failure of the read operation
    AxisProfileResult readParamListCoE(const ObjectDirEntry& obj, std::size_t maxElements, std::vector<std::uint16_t>& outList);

    template<typename TValue>
    AxisProfileResult readParamListCoEImpl(const ObjectDirEntry& obj, std::size_t maxElements, std::vector<TValue>& outList);

    /// @brief Read one CoE parameter from snapshot cache first, then drive.
    /// @param[in] objEntry CoE object entry.
    /// @param[out] bufPtr Raw target buffer.
    /// @param[in] bufLen Raw target buffer size.
    /// @param[out] readLen Actual raw bytes copied to output buffer.
    /// @return Return value indicating success or error.
    AxisProfileResult getParameterValueFromSnapshot(const ObjectDirEntry& objEntry, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen);

  };
}
