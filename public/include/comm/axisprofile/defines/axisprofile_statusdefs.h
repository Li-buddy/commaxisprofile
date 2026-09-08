#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines public status.
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
  /// Helper to set the right bits to indicate a success return value
  #define APR_STATUS_MAKE_SUCCESS(x)    int(0x00000000 + x)
  /// Helper to set the right bits to indicate an error return value
  #define APR_STATUS_MAKE_ERROR(x)      int(0xF0000000 + x)

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief AxisProfile results
  enum class AxisProfileResult
  {
    STS_OK                        = APR_STATUS_MAKE_SUCCESS(0x0000),   ///< indicating success
    STS_PENDING                   = APR_STATUS_MAKE_SUCCESS(0x0001),   ///< axisprofile is processing the request
    STS_NO_CONFIG_FILES_FOUND     = APR_STATUS_MAKE_SUCCESS(0x0002),   ///< Could not find any configuration files
    STS_NO_PROFILES_EXIST         = APR_STATUS_MAKE_SUCCESS(0x0003),   ///< Could not find any profiles (e.g. when trying to save the system configuration)
    STS_COMMAND_LOCKED            = APR_STATUS_MAKE_SUCCESS(0x0004),   ///< This command is currently locked by another thread
    STS_BOOTING_ABORT             = APR_STATUS_MAKE_SUCCESS(0x0005),   ///< Component booting should abort or indicates booting has aborted
    STS_FIELDBUS_NOT_INIT         = APR_STATUS_MAKE_SUCCESS(0x0006),   ///< Indicating field bus not initialized yet
    STS_WAIT_IN_ASYNCQUEUE        = APR_STATUS_MAKE_SUCCESS(0x0007),   ///< Indicates that the asynchronous operation in the queue has not yet started to execute.
    STS_BOOTING_CONTINUE          = APR_STATUS_MAKE_SUCCESS(0x0008),   ///< Component booting continue to next step when actual step gets acceptable error
    STS_COMMAND_NOT_EXECUTE       = APR_STATUS_MAKE_SUCCESS(0x0009),   ///< Indicates that the command is not executing

    ERR_NOK                       = APR_STATUS_MAKE_ERROR(0x0000),   ///< generic error result
    ERR_EXEC_NULL                 = APR_STATUS_MAKE_ERROR(0x0001),   ///< null pointer occurred
    ERR_NOTIMPLEMENTED            = APR_STATUS_MAKE_ERROR(0x0002),   ///< this option is not (yet) implemented
    ERR_PROFILE_NOT_FOUND         = APR_STATUS_MAKE_ERROR(0x0003),   ///< the requested profile could not be found
    ERR_PROFILE_ALREADY_EXISTS    = APR_STATUS_MAKE_ERROR(0x0004),   ///< The requested axis profile already exists
    ERR_PROFILE_IN_USE            = APR_STATUS_MAKE_ERROR(0x0005),   ///< Operation failed because the profile is currently in use
    ERR_WRONGDATASIZE             = APR_STATUS_MAKE_ERROR(0x0006),   ///< data size is wrong
    ERR_WRONGDATATYPE             = APR_STATUS_MAKE_ERROR(0x0007),   ///< data type is wrong
    ERR_INIT_TELEGRAM_OFFSETS     = APR_STATUS_MAKE_ERROR(0x0008),   ///< telegram related error
    ERR_TIMEOUT                   = APR_STATUS_MAKE_ERROR(0x0009),   ///< timeout detected
    ERR_COMMAND_IN_PROGRESS       = APR_STATUS_MAKE_ERROR(0x000A),   ///< The requested command was blocked because another command is already in progress
    ERR_INVALID_ARGUMENT          = APR_STATUS_MAKE_ERROR(0x000B),   ///< An invalid argument was passed to a function
    ERR_INVALID_STATE             = APR_STATUS_MAKE_ERROR(0x000C),   ///< The called method is in the actual state not executable
    ERR_STATEMACHINE_ADD_CALLBACK = APR_STATUS_MAKE_ERROR(0x000D),   ///< Error when trying to add a callback to the statemachine
    ERR_AOX_THREAD                = APR_STATUS_MAKE_ERROR(0x000E),   ///< Received error by an AOX thread function
    ERR_WRONG_DEVICE_STATE        = APR_STATUS_MAKE_ERROR(0x000F),   ///< The device is not in the correct state
    ERR_BACKUP_RESTORE            = APR_STATUS_MAKE_ERROR(0x0010),   ///< General error in context of the backup/restore class
    ERR_SEND_DIAGNOSIS            = APR_STATUS_MAKE_ERROR(0x0011),   ///< Could not send the diagnosis
    ERR_MAX_NUMBER_PROFILES       = APR_STATUS_MAKE_ERROR(0x0012),   ///< Maximum number of profiles reached
    ERR_OUT_OF_MEMORY             = APR_STATUS_MAKE_ERROR(0x0013),   ///< 'new' operation failed because no more memory is available
    ERR_PROFILE_CONFIG            = APR_STATUS_MAKE_ERROR(0x0014),   ///< Actual profile configuration is invalid
    ERR_MATH                      = APR_STATUS_MAKE_ERROR(0x0015),   ///< Math error detected (e.g. divided by 0)
    ERR_NOT_IN_CONFIG_STATE       = APR_STATUS_MAKE_ERROR(0x0016),   ///< axisprofile state is not configuration
    ERR_DEFAULT_ERROR_REACTION    = APR_STATUS_MAKE_ERROR(0x0017),   ///< default error reaction
    ERR_COMMAND_INFO_NOT_FOUND    = APR_STATUS_MAKE_ERROR(0x0018),   ///< command information not found
    ERR_ERR_DATA_NOT_MAPPED       = APR_STATUS_MAKE_ERROR(0x0019),   ///< data not mapped
    ERR_NOT_SUPPORTED             = APR_STATUS_MAKE_ERROR(0x0020),   ///< data not provided
    ERR_PROFILE_RESET_FAILED      = APR_STATUS_MAKE_ERROR(0x0021),   ///< reset profile failed
    ERR_INCONSISTENCY             = APR_STATUS_MAKE_ERROR(0x0022),   ///< inconsistency error
    ERR_COMMAND_EXECUTION         = APR_STATUS_MAKE_ERROR(0x0023),   ///< error occurred during command execution

    // ------------------------------------- FPU errors ------------------------------ //

    ERR_FPU_DIV_BY_ZERO           = APR_STATUS_MAKE_ERROR(0x0031),   ///< FPU error 'division by zero'
    ERR_FPU_INVALID               = APR_STATUS_MAKE_ERROR(0x0032),   ///< FPU error 'invalid number'
    ERR_FPU_OVERFLOW              = APR_STATUS_MAKE_ERROR(0x0033),   ///< FPU error 'overflow'
    ERR_FPU_UNKNOWN               = APR_STATUS_MAKE_ERROR(0x0034),   ///< unknown FPU error

    // ------------------------------------- thread related errors ------------------------------ //

    ERR_COMMAND_LOCKED            = APR_STATUS_MAKE_ERROR(0x0050),   ///< This command is currently locked by another thread

    // ------------------------------------- error during cyclic data exchange ------------------------------ //

    ERR_CYCLIC_DATA_AT            = APR_STATUS_MAKE_ERROR(0x0100),   ///< Error during the processing of at-data
    ERR_CYCLIC_DATA_MDT           = APR_STATUS_MAKE_ERROR(0x0101),   ///< Error during the processing of mdt-data

    // ------------------------------------- job object operation related errors ------------------------------ //

    ERR_JOB_EXIST                 = APR_STATUS_MAKE_ERROR(0x0200),   ///< job object already exist
    ERR_JOB_NOT_EXIST             = APR_STATUS_MAKE_ERROR(0x0201),   ///< job object not exist
    ERR_JOB_DATA_MISMATCH         = APR_STATUS_MAKE_ERROR(0x0202),   ///< job data type not match
    ERR_JOB_EMPTY_NAME            = APR_STATUS_MAKE_ERROR(0x0203),   ///< job object name is empty
    ERR_JOB_EXECUTION             = APR_STATUS_MAKE_ERROR(0x0204),   ///< job object error happens in execution

    // ------------------------------------- Datalayer related errors --------------------------------------- //

    ERR_DATALAYER                 = APR_STATUS_MAKE_ERROR(0x0500),   ///< Datalayer general error
    ERR_DATALAYER_PERSISTENCE     = APR_STATUS_MAKE_ERROR(0x0501),   ///< Received error from the datalayer persistence helper
    ERR_DATALAYER_READ_METADATA   = APR_STATUS_MAKE_ERROR(0x0502),   ///< Could not read the metadata of a node
    ERR_DATALAYER_WRONG_TYPE      = APR_STATUS_MAKE_ERROR(0x0503),   ///< The received variant has an incorrect type
    ERR_DATALAYER_READ_NODE       = APR_STATUS_MAKE_ERROR(0x0504),   ///< Could not read a datalayer node
    ERR_DATALAYER_WRITE_NODE      = APR_STATUS_MAKE_ERROR(0x0505),   ///< Could not write a datalayer node
    ERR_DATALAYER_BROWSE_NODE     = APR_STATUS_MAKE_ERROR(0x0506),   ///< Could not browse a datalayer node

    // ----------------------------------- Datalayer RT related errors -------------------------------------- //

    STS_DATALAYER_RT_NOT_OPEN     = APR_STATUS_MAKE_SUCCESS(0x0600), ///< Datalayer realtime not open, but system can still work (non-mandatory RT data)

    ERR_DLRT                      = APR_STATUS_MAKE_ERROR(0x0600),   ///< Datalayer RT general error
    ERR_DLRT_MEMMAP_VAR_NOT_FOUND = APR_STATUS_MAKE_ERROR(0x0601),   ///< Could not find the requested variable in the datalayer RT memory map

    // -------------------------------------- Fieldbus related errors --------------------------------------- //

    ERR_FIELDBUS                  = APR_STATUS_MAKE_ERROR(0x0700),   ///< Fieldbus general error
    ERR_FIELDBUS_NO_MASTER        = APR_STATUS_MAKE_ERROR(0x0701),   ///< No fieldbus master instance available
    ERR_FIELDBUS_READ_VALUE       = APR_STATUS_MAKE_ERROR(0x0702),   ///< Could not read fieldbus value via datalayer

    // ToDo...
    ERR_NEDD_BETTER_ERROR         = APR_STATUS_MAKE_ERROR(0xFFFF),   ///< filler error result
  };

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// Return whether status code is an error status code
  /// @param[in]  status     Status code
  /// @return true if status code is an error status code
  inline bool aprStatusFailed(AxisProfileResult status)
  {
    return (std::uint32_t(status) & (1u << 31));
  }

  /// Return whether status code is a successful status code
  /// @param[in]  status     Status code
  /// @return true if status code is a successful status code
  inline bool aprStatusSucceeded(AxisProfileResult status)
  {
    return !aprStatusFailed(status);
  }
 
  /// Return whether status code is a non-OK status code
  /// @param[in]  status     Status code
  /// @return true if status code is a non-OK status code
  inline bool aprStatusNotOkay(AxisProfileResult status)
  {
    return (std::uint32_t(status));
  }

  /// Return whether status code is a OK status code
  /// @param[in]  status     Status code
  /// @return true if status code is a OK status code
  inline bool aprStatusOkay(AxisProfileResult status)
  {
    return !aprStatusNotOkay(status);
  }

  /// @brief Overload operator of |= which now return the maximum result value
  ///        Note: it is not bitwise OR! It gets the highest result of AxisProfileResult
  /// @param[in] left       value at left of |=
  /// @param[in] right      value at right of |=
  /// @return Result of new operation |=
  inline AxisProfileResult& operator |= (AxisProfileResult& left, AxisProfileResult right)
  {
    return right > left ? right : left;
  }
}
