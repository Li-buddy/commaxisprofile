#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the detail diagnosis number.
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
  // axisprofile ranges its detail diagnostic number within [0x0C580000, 0x0C58FFFF]
  // https://inside-docupedia.bosch.com/confluence/display/EDCA/DIA_Control_DiagnosticNumber
  //
  // bits 31..30: Reserved
  //
  // bits 29..24: Sub System ID
  //   - 0xC - It's a ctrlX CORE application
  //
  // bits 23: Decides, whether the following Bits 22..0 have to be interpreted:
  //   - 0b0 - Fixed detail diagnostic code of a system function
  //
  // bits 23..16: System function ID
  //   - 0x58 - DRP (drive profile)
  //
  // bits 15..00: Status Code Number
  //   - 0x0000..0xFFFF

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  extern const common::log::diagnosis::DiagnosisElement DETAILED_DIAGNOSIS_LIST[];
  extern const std::uint32_t DETAILED_DIAGNOSIS_LIST_SIZE;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ************************************************ ERRORS ************************************************ //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // ------------------------------------------- General diagnoses ------------------------------------------ //

  constexpr std::uint32_t DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED            = 0x0C580000;
  constexpr std::uint32_t DETAILED_DIAG_DL_INVALID_VALUE                    = 0x0C580001;
  constexpr std::uint32_t DETAILED_DIAG_NULL                                = 0x0C580002;
  constexpr std::uint32_t DETAILED_DIAG_SYSTEM_INCONSISTENT                 = 0x0C580003;
  constexpr std::uint32_t DETAILED_DIAG_INVALID_PROFILE_NAME                = 0x0C580004;
  constexpr std::uint32_t DETAILED_DIAG_INVALID_DEVICE_ADR                  = 0x0C580005;
  constexpr std::uint32_t DETAILED_DIAG_PROFILE_NOT_FOUND                   = 0x0C580006;
  constexpr std::uint32_t DETAILED_DIAG_INVALID_DEVICENAME                  = 0x0C580030;
  constexpr std::uint32_t DETAILED_DIAG_INVALID_BOOTING_TIMEOUT             = 0x0C580031;
  constexpr std::uint32_t DETAILED_DIAG_INVALID_PROFILE_CATEGORY            = 0x0C580032;
  constexpr std::uint32_t DETAILED_DIAG_MASTER_STATUS_ERROR                 = 0x0C580033;
  constexpr std::uint32_t DETAILED_DIAG_SLAVE_STATUS_ERROR                  = 0x0C580034;
  constexpr std::uint32_t DETAILED_DIAG_FUNCTION_NOT_SUPPORTED              = 0x0C580035;

  // --------------------------------------------- Backup restore ---------------------------------------- //

  constexpr std::uint32_t DETAILED_DIAG_BACKUPRESTORE_LOAD_NOT_IN_CONFIG    = 0x0C580040;
  constexpr std::uint32_t DETAILED_DIAG_BACKUPRESTORE_SAVE_NOT_IN_CONFIG    = 0x0C580041;
  constexpr std::uint32_t DETAILED_DIAG_BACKUPRESTORE_LOAD_FAILED           = 0x0C580042;
  constexpr std::uint32_t DETAILED_DIAG_BACKUPRESTORE_SAVE_FAILED           = 0x0C580043;
  constexpr std::uint32_t DETAILED_DIAG_BACKUPRESTORE_EXTRACT_PERSISTENCE_FAILED = 0x0C580044;

  // ----------------------------------------- Statemachine diagnoses --------------------------------------- //

  constexpr std::uint32_t DETAILED_DIAG_STM_REQUEST_FAILED                  = 0x0C580050;
  constexpr std::uint32_t DETAILED_DIAG_STM_GET_STATE_NAME                  = 0x0C580051;
  constexpr std::uint32_t DETAILED_DIAG_STM_GET_ALL_STATE_NAMES             = 0x0C580052;
  constexpr std::uint32_t DETAILED_DIAG_STM_SWITCH_TO_RUNNING               = 0x0C580053;
  constexpr std::uint32_t DETAILED_DIAG_STM_AT_ERROR_STATE                  = 0x0C580054;

  // ----------------------------------------- Configuration diagnoses -------------------------------------- //

  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CREATE_PROFILE_FAILED        = 0x0C580060;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CREATE_PROFILE_INVALID_NAME  = 0x0C580061;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CREATE_PROFILE_INVALID_TYPE  = 0x0C580062;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CREATE_PROFILE_ALREADY_EXIST = 0x0C580063;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CREATE_PROFILE_OUT_OF_MEMORY = 0x0C580064;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CREATE_PROFILE_MAX_NUMBER    = 0x0C580065;

  constexpr std::uint32_t DETAILED_DIAG_CONFIG_DELETE_PROFILE_FAILED        = 0x0C580070;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_DELETE_PROFILE_INVALID_NAME  = 0x0C580071;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_DELETE_PROFILE_NOT_FOUND     = 0x0C580072;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_DELETE_PROFILE_IN_USE        = 0x0C580073;

  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CONNECT_PROFILE_FAILED       = 0x0C580080;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CONNECT_PROFILE_INVALID_NAME = 0x0C580081;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CONNECT_PROFILE_NOT_FOUND    = 0x0C580082;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_CONNECT_PROFILE_IN_USE       = 0x0C580083;

  constexpr std::uint32_t DETAILED_DIAG_CONFIG_INIT_RT_INPUT_MAPPING        = 0x0C580090;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_INIT_RT_OUTPUT_MAPPING       = 0x0C580091;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_MAP_SINGLE_INPUT             = 0x0C580092;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_MAP_SINGLE_OUTPUT            = 0x0C580093;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_BACKUP_RESTORE               = 0x0C580094;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_GET_IN_OUT_BUFFER            = 0x0C580095;
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_SET_IN_OUT_BUFFER            = 0x0C580096;

  constexpr std::uint32_t DETAILED_DIAG_CONFIG_PROFILE_NOT_ASSIGN           = 0x0C580097;    
  constexpr std::uint32_t DETAILED_DIAG_CONFIG_OP_MODE_MAPPING              = 0x0C580098;

  // --------------------------------------------- Profile diagnoses ---------------------------------------- //

  constexpr std::uint32_t DETAILED_DIAG_PROFILE_OTHER_CMD_PROCESSED         = 0x0C580100;
  constexpr std::uint32_t DETAILED_DIAG_PROFILE_CMD_TIMEOUT                 = 0x0C580101;
  constexpr std::uint32_t DETAILED_DIAG_PROFILE_CMD_EXE_FAILED              = 0x0C580102;     ///< Axisprofile command executing failed
  constexpr std::uint32_t DETAILED_DIAG_PROFILE_FEEDBACK_VALUE_INVALID      = 0x0C580103;     ///< Axisprofile feedback value invalid

  // -------------------------------------------- Datalayer diagnoses --------------------------------------- //

  constexpr std::uint32_t DETAILED_DIAG_INVALID_FLATBUFFER                  = 0x0C580200;
  constexpr std::uint32_t DETAILED_DIAG_INVALID_FIELDBUS_RT_MEMORYMAP       = 0x0C580201;

  // -------------------------------------------- Device diagnoses ------------------------------------------ //

  constexpr std::uint32_t DETAILED_DIAG_DEVICE_OP_MODE_NOT_CONFIG           = 0x0C580300;
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_SERIE_NOT_DEFIEND            = 0x0C580301;
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_CONFIG_ERROR                 = 0x0C580302;
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_OUT_OF_TRAVEL_LIMIT          = 0x0C580303;     ///< Error that out of device travel limit
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_ENCODER_NOT_EXIST            = 0x0C580304;     ///< Error that encoder (id) not exit in device
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_HOMING_PARAMETER             = 0x0C580305;     ///< Error that homing parameter inconsistency
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_NOT_SUPPORT                  = 0x0C580306;     ///< Error that the function is not support by the device
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_ENCODER_TYPE_ERROR           = 0x0C580307;     ///< Error of type of encoder measure system
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_TORQUE_LIMIT                 = 0x0C580308;     ///< Error of torque limit value exceeded
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_STATUS_ERROR                 = 0x0C580309;     ///< Error of incorrect status of device
  constexpr std::uint32_t DETAILED_DIAG_DEVICE_POWER_TIMEOUT                = 0x0C58030A;     ///< Error of power on timeout

  // -------------------------------------------- Probe diagnoses ------------------------------------------ //
  constexpr std::uint32_t DETAILED_DIAG_SELECTED_PROBE_NOT_EXIST            = 0x0C580350;
  constexpr std::uint32_t DETAILED_DIAG_PROBE_CMD_EXECUTE_FAILED            = 0x0C580351;
  constexpr std::uint32_t DETAILED_DIAG_PROBE_TIME_MEASURE_EXCEED           = 0x0C580352;

  // ------------------------------------------ scaling diagnoses ------------------------------------------ //
  constexpr std::uint32_t DETAILED_DIAG_TORQUE_FORCE_SCALING_TYPE           = 0x0C580400;
  constexpr std::uint32_t DETAILED_DIAG_INCONSISTENCE_SCALING_TYPE          = 0x0C580401;
  constexpr std::uint32_t DETAILED_DIAG_SCALLING_TYPE_ERROR                 = 0x0C580402;
  constexpr std::uint32_t DETAILED_DIAG_SCALING_VALUE_ERROR                 = 0x0C580403;
  constexpr std::uint32_t DETAILED_DIAG_FPU_SCALING_VALUE_ERROR             = 0x0C580404;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // *********************************************** WARNINGS *********************************************** //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // -------------------------------------------- Booting diagnoses ----------------------------------------- //

  constexpr std::uint32_t DETAILED_DIAG_BOOT_ABORTED                        = 0x0C580500;    ///< Booting is aborted
  constexpr std::uint32_t DETAILED_DIAG_BOOT_TIME_OUT                       = 0x0C580501;    ///< Timeout when booting
  constexpr std::uint32_t DETAILED_DIAG_BOOT_MASTER_NOT_EXIT                = 0x0C580502;    ///< Fieldbus master not exist
  constexpr std::uint32_t DETAILED_DIAG_BOOT_MASTER_STATUS_ERROR            = 0x0C580503;    ///< Fieldbus master status get an error
  constexpr std::uint32_t DETAILED_DIAG_BOOT_SLAVE_NOT_EXIT                 = 0x0C580504;    ///< Slave device not exist
  constexpr std::uint32_t DETAILED_DIAG_BOOT_SLAVE_STATUS_ERROR             = 0x0C580505;    ///< Slave device status get an error
  constexpr std::uint32_t DETAILED_DIAG_BOOT_CALLABLE_NOT_READY             = 0x0C580506;    ///< Axisprofile adaption callable not ready
  constexpr std::uint32_t DETAILED_DIAG_BOOT_FIELDBUS_INCONSISTENCE         = 0x0C580507;    ///< Fieldbus inconsistence between device and configured

  // ------------------------------------------ Booting config diagnoses ------------------------------------------ //
  constexpr std::uint32_t DETAILED_DIAG_READ_PARAM_FAILED                   = 0x0C580600;
  constexpr std::uint32_t DETAILED_DIAG_PARSE_PARAM_FAILED                  = 0x0C580601;
  constexpr std::uint32_t DETAILED_DIAG_PARSE_VENDOR_ID_FAILED              = 0x0C580602;
  constexpr std::uint32_t DETAILED_DIAG_WRITE_PARAM_FAILED                  = 0x0C580603;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // *********************************************** MESSAGES *********************************************** //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // -------------------------------------------- Booting diagnoses ----------------------------------------- //

  constexpr std::uint32_t DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_INSTANCES     = 0x0C580700;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_FIELDBUS_INSTANCES_READY        = 0x0C580701;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_MASTER_STATES = 0x0C580702;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_FIELDBUS_MASTER_STATES_READY    = 0x0C580703;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_RT_MEMORY     = 0x0C580704;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_FIELDBUS_RT_MEMORY_READY        = 0x0C580705;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_SCHEDULER_IS_RUNNING            = 0x0C580706;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_VALIDATE_MAPPING_CFG            = 0x0C580707;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_WAIT_FOR_PROFILE_MANAGER        = 0x0C580708;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_PROFILE_MANAGER_READY           = 0x0C580709;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_WAIT_FOR_PROFILE_INITIALIZATION = 0x0C58070A;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_PROFILE_READY                   = 0x0C58070B;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_INCONSISTENT_PROTOCOL           = 0x0C58070C;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_PROFILE_CHECK_RESP_DONTUSE      = 0x0C58070D;
  constexpr std::uint32_t DETAILED_DIAG_BOOT_NO_HOMING_STATUS_FROM_DEVICE    = 0x0C58070E;
}
