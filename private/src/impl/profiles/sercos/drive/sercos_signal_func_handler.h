#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and declare the sercos signal function handler class.
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

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
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
  /// @brief This class implements the sercos signal function handler.
  class SercosSignalFuncHandler :public SignalFuncHandler
  {
  public:
    explicit SercosSignalFuncHandler(ProfileBasicInfo& profileBasicInfo);
    ~SercosSignalFuncHandler() noexcept override = default;

  private:
    AxisProfileResult readDriveSignalStatusWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits) final;

    AxisProfileResult readDriveSignalControlWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits) final;

    /// @brief This method reads a parameter list from the drive using Sercos over EtherCAT (SoE) protocol.
    /// @param[in]  idn      the identifier of the parameter to read
    /// @param[out] outList  the output list to store the read parameter values
    /// @return returns an AxisProfileResult indicating the success or failure of the read operation
    AxisProfileResult readParamListSoE(std::uint16_t idn, std::vector<std::uint16_t>& outList);
  };
}
