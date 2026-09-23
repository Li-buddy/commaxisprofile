#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the main diagnosis number.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <cstdint>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace common::log::diagnosis
{
  struct DiagnosisElement;
}

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // bits 31-30: Reserved
  // bits 29-24: Source-Type
  //   - 0x09 - Motion Application
  //
  // bits 19-12: DiagnosisClass and status code priority
  //   - 0xA0 - Message
  //   - 0xE0 - Warning
  //   - 0xF0 - F0, Non fatal error
  //   - 0xF2 - F2, stop single axis/kinematics error
  //   - 0xF6 - F6, stop all axis/kinematics error
  //   - 0xF8 - F8, fatal application error, e.g scaling calculation exception
  //   - 0xF9 - F9, fatal system error, e.g automation core out of memory exception
  // 
  // Bit 11-0: Status code number
  //   - 0x000..0xFFF

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  extern const common::log::diagnosis::DiagnosisElement MAIN_DIAGNOSIS_LIST[];
  extern const std::uint32_t MAIN_DIAGNOSIS_LIST_SIZE;
  constexpr char LIBRARY_DIAGNOSTICS_FILE_NAME[] = "comm_axisprofile_diagnostics_en-US.json";  ///< File name with diagnostics used in axisprofile library.

  constexpr std::uint32_t MAIN_DIAG_GENERAL_MSG       = 0x090A0200;
  constexpr std::uint32_t MAIN_DIAG_BOOTING_MSG       = 0x090A0201;

  constexpr std::uint32_t MAIN_DIAG_GENERAL_WRN       = 0x090E0200;
  constexpr std::uint32_t MAIN_DIAG_BOOTING_WRN       = 0x090E0201;

  constexpr std::uint32_t MAIN_DIAG_GENERAL_ERR       = 0x090F0000;
  constexpr std::uint32_t MAIN_DIAG_CONFIG_ERR        = 0x090F0001;
  constexpr std::uint32_t MAIN_DIAG_PROFILE_ERR       = 0x090F0002;
  constexpr std::uint32_t MAIN_DIAG_DATALAYER_ERR     = 0x090F0003;
  constexpr std::uint32_t MAIN_DIAG_STATEMACHINE_ERR  = 0x090F0004;
  constexpr std::uint32_t MAIN_DIAG_DEVICE_ERR        = 0x090F0005;
  constexpr std::uint32_t MAIN_DIAG_PROBE_ERR         = 0x090F0006;
  constexpr std::uint32_t MAIN_DIAG_SCALING_ERR       = 0x090F0007;
  constexpr std::uint32_t MAIN_DIAG_FIELDBUS_ERR      = 0x090F0008;
  constexpr std::uint32_t MAIN_DIAG_COMMAND_ERR       = 0x090F0009;
  constexpr std::uint32_t MAIN_DIAG_SYSTEM_ERR        = 0x090F000A; ///< system get error, e.g. FPU error or memory leak
  constexpr std::uint32_t MAIN_DIAG_BOOTING_ERR       = 0x090F0010;

  // Trace main diagnosis , no entry in axisprofile_main_diag.cpp needed
  constexpr std::uint32_t MAIN_DIAG_AXISPROFILE_TRACE_MESSAGE = 0x090A02FF;
  constexpr std::uint32_t MAIN_DIAG_AXISPROFILE_TRACE_WARNING = 0x090E02FF;
  constexpr std::uint32_t MAIN_DIAG_AXISPROFILE_TRACE_ERROR   = 0x090F02FF;

  // Special diagnostic: https://inside-docupedia.bosch.com/confluence/x/xFuXhg
  constexpr std::uint32_t MAIN_DIAG_DIAGNOSTICS_F0_QUERY_LOAD = 0x080F0E00;
}
