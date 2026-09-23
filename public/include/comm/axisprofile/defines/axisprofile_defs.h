#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines public types.
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
#include <string>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr std::uint32_t MAX_PROFILE_NAME_LENGTH = 64u;   ///< maximum length of profile names (including the trailing '\0')
  constexpr std::uint32_t MAX_NUMBER_OF_PROFILES = 128u;   ///< maximum number of profiles, 128 real axis

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------
  /// This macro prints the given parameter as string.
  #define _CLASS_NAME_(x)   (           # x )
  /// This macro maps the given parameter to a macro that prints it as string.
  #define CLASS_NAME(x)     (_CLASS_NAME_(x))

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief define user PDO id number
  /// for now 6 input/output user PDO supported
  /// id range[0,11] input[0,5] output[6,11]
  using UserPDOID = std::uint8_t;

  /// @brief Define category for drive and encoder
  enum class AxisProfileCategory
  {
    UNDEFINED = 0,               ///< undefined category
    CATEGORY_DRIVE = 1,          ///< drive category
    CATEGORY_ENCODER = 2         ///< encoder category
  };

  /// @brief Supported AxisProfile Types
  enum class AxisProfileType
  {
    UNDEFINED = 0,               ///< Undefined axisprofile
    SERVODRIVE_OVER_ETHERCAT,    ///< AxisProfile Servodrive over Ethercat (SoE)
    CANOPEN_OVER_ETHERCAT        ///< AxisProfile CAN application over Ethercat (CoE)
  };

  /// @brief Supported operation modes
  enum class AxisProfileOperationMode
  {
    UNDEFINED = 0,          ///< Undefined operation mode
    POSITIONING,            ///< Operation mode Positioning
    VELOCITY,               ///< Operation mode Velocity
    TORQUE_FORCE_VEL_LIMIT, ///< Operation mode Torque/Force with velocity limitation

    // Protocol specific mode
    HOMING = 0x00FF,        ///< Operation mode Homing, CoE only
    OPMODE_PENDING,         ///< Operation mode is switching and undetermined yet
  };

  /// @brief Supported power modes
  enum class AxisProfilePowerMode
  {
    UNDEFINED = 0,          ///< Undefined power mode
    POWER_OFF,              ///< Power mode power off
    POWER_ON,               ///< Power mode power on
    POWER_OFF_TORQUE_FREE   ///< Power mode power off with torque disable without delay
  };

  /// @brief Positive Stop Mode
  enum class AxisProfilePositiveStop
  {
    UNDEFINED = -1,           ///< Undefined
    DISABLE = 0,              ///< Disable positive stop
    ABORT = 1,                ///< interrupt
    ENABLE = 2                ///< ENABLE positive stop
  };

  /// @brief Encoder selector
  enum class AxisProfileEncoder
  {
    UNDEFINED = -1,        ///< undefined
    ENCODER_1 = 0,         ///< encoder 1
    ENCODER_2 = 1,         ///< encoder 2
  };

  /// @brief Device scaling type
  enum class AxisProfileDeviceScalingType
  {
    UNDEFINED = -1,           ///< undefined
    ROTATIONAL = 0,           ///< rotational
    PERCENTAGE = 1,           ///< percentage
    LINEAR = 2,               ///< linear
  };

  /// @brief Position data format
  enum class AxisProfilePosDataFormat
  {
    UNDEFINED = -1,           ///< undefined format
    FORMAT_ABSOLUTE = 0,      ///< absolute format
    FORMAT_MODULO = 1,        ///< modulo format
    FORMAT_IGNORE = 2,        ///< format can be ignored(for CoE)
  };

  /// @brief Device scaling type
  enum class AxisProfileDeviceValueType
  {
    UNDEFINED = -1,   ///< undefined
    POSITION = 0,     ///< Position scaling type
    VELOCITY = 1,     ///< Velocity scaling type
    TORQUE_FORCE = 2, ///< Torque scaling type
    ACCELERATION = 3, ///< acceleration scaling type
  };

  /// @brief ID of feedback value type
  enum class AxisProfileValueTypeID
  {
    ACTUAL_POSITION = 0,      ///< actual position
    ACTUAL_VELOCITY = 1,      ///< actual velocity
    ACTUAL_ACCELERATION = 2,  ///< acutal acceleration
    ACTUAL_TORQUE = 3,        ///< actual torque

    LAST_ID                   ///< Helper value, always keep at the end
  };

  /// @brief General error types when access data
  enum class DataAccessCommonErrorType
  {
    ERROR_NONE = 0,                 ///< no error
    ERROR_NOT_PROVIDED = 1,         ///< unprovided
    ERROR_MASTER_STATUS = 2,        ///< error of master status
    ERROR_SLAVE_STATUS = 3,         ///< error of slave status
    ERROR_CFG_MAPPING_ERROR = 4,    ///< error of mapping configuration
    ERROR_BEYOND_THE_LIMIT = 5,     ///< error of beyond limitation
    ERROR_LAST_TYPE                 ///< the index of last error type
  };

  /// @brief Structures error information when access data
  struct DataAccessErrorInfo
  {
    DataAccessCommonErrorType m_accessErrorType {DataAccessCommonErrorType::ERROR_NONE};
    std::string               m_additionInfo {};
  };

  /// @brief Digital input value type for every single bit
  enum class DigitalInputValueBitType : std::int16_t
  {
    UNDEFINED = -1,                   ///< Undefined
    REFERENCE_SWITCH_BIT = 0,         ///< Reference switch signal
    POSITIVE_LIMIT_SWITCH_BIT,        ///< Positive limit switch signal
    NEGATIVE_LIMIT_SWITCH_BIT,        ///< Negative limit switch signal

    MAX_VALUE,                        ///< Maximum helper value, put new value above!
    MIN_VALUE = REFERENCE_SWITCH_BIT, ///< Minimum helper value
  };

  /// @brief Device error type
  enum class AxisProfileDeviceErrorType
  {
    ERROR_NO_INFO = -1,    ///< no error information can be required
    ERROR_NONE = 0,        ///< no error
    ERROR_LIGHT,           ///< light error
    ERROR_FIELDBUS,        ///< field bus error
    ERROR_CRITICAL,        ///< critical error
    ERROR_FATAL,           ///< fatal error
    ERROR_NOT_DEFINED,     ///< none defined error
  };

  /// @brief Operation mode supported error cause type
  enum  class OpModeSupportedErrorType
  {
    ERROR_NONE = 0,            ///< no error
    ERROR_NOT_SUPPORT,         ///< not support by the device
    ERROR_DRIVE_CONFIG,        ///< not configured respective operation mode in drive
    ERROR_CYCLIC_DATA_MISSING, ///< not assigned respective cyclic data in profile variable
    ERROR_NOT_DEFINED,         ///< not defined error
  };

  /// @brief Deprecated, axis profile command type
  enum class AxisProfileCommandType : std::uint16_t
  {
    CMD_CLEAR_DEVICE_ERROR = 0,       ///< reset device error
    CMD_SET_CHECK_POSITIVE_STOP,      ///< set and check positive stop
    CMD_SET_CHECK_CLAMPING_TORQUE,    ///< set and check clamping torque
    CMD_GET_CLAMPING_TORQUE,          ///< get clamping torque
    CMD_CHECK_STANDSTILL,             ///< check standstill state
    CMD_SET_CHECK_STANDSTILL_WINDOW,  ///< set and check standstill window
    CMD_GET_STANDSTILL_WINDOW,        ///< get standstill window
    CMD_ACTIVATE_STANDSTILL_CHECK,    ///< activate standstill check
    CMD_DEACTIVATE_STANDSTILL_CHECK,  ///< deactivate standstill check
    CMD_SET_ABSOLUTE_POSITION,        ///< set absolute position
    CMD_REFERENCING_AXIS,             ///< referencing axis (drive controlled homing)
    CMD_RESET_REFERENCING_AXIS,       ///< reset referencing axis (drive controlled homing)
    CMD_SET_REFERENCING_POS,          ///< set new referencing position
    CMD_CHECK_AXIS_HOMED,             ///< check whether axis has homed
    CMD_RESET_ABSOLUTE_POSITION,      ///< reset absolute position
    CMD_CHECK_DEVICE_HALT,            ///< check if device halt is activated

    CMD_MAX,                          ///< maximum type value
    CMD_MIN = CMD_CLEAR_DEVICE_ERROR, ///< minimum type value
  };

  /// @brief Deprecated, axis profile command state
  enum class AxisprofileCommandState : std::int16_t
  {
    CMD_STATE_UNDEFINED = -1,   ///< undefined command state
    CMD_STATE_IDLE,             ///< command is idle, can be called
    CMD_STATE_INIT,             ///< command init state, do some preparation
    CMD_STATE_INQUEUE,          ///< command was added to async queue
    CMD_STATE_ACTIVE,           ///< command is executing
    CMD_STATE_INTERRPUT,        ///< command is interrupted
    CMD_STATE_ABORTING,         ///< command is aborting, if the command is aborting it may require clear step.
    CMD_STATE_ERROR,            ///< command is in error state

    CMD_STATE_MAX,                  ///< helper value, always keep at the end
    CMD_STATE_MIN = CMD_STATE_IDLE, ///< helper value,minimum type value
  };

  /// @brief Type of axisprofile command
  enum class CmdType : std::uint16_t
  {
    CLEAR_DEV_ERR = 0,          ///< clear device error
    SET_ABS_POS,                ///< set absolute position
    REFERENCE_POS,              ///< referencing axis (drive homing)
    FIXED_STOP,                 ///< fixed stop
    SET_NEW_REF_WITHOUT_MOVE,   ///< set new reference without movement
    SET_OPERATION_MODE_WITH_CHECK,         ///< set operation mode
  };
  constexpr std::uint16_t CMD_MIN = static_cast<std::uint16_t>(CmdType::CLEAR_DEV_ERR); ///< minimum type value
  constexpr std::uint16_t CMD_MAX = static_cast<std::uint16_t>(CmdType::SET_OPERATION_MODE_WITH_CHECK); ///< maximum type value

  /// @brief state of an axisprofile command state
  enum class CmdState : std::int16_t
  {
    STATE_UNKNOWN = -1,         ///< command state is unknown
    STATE_IDLE,                 ///< command is idle
    STATE_ACTIVATING,           ///< command is activating
    STATE_ACTIVE,               ///< command is active
    STATE_ABORTING,             ///< command is aborting
    STATE_ERROR,                ///< command is in error state
  };
  constexpr std::int16_t STATE_MIN = static_cast<std::int16_t>(CmdState::STATE_IDLE); ///< minimum type value
  constexpr std::int16_t STATE_MAX = static_cast<std::int16_t>(CmdState::STATE_ERROR); ///< maximum type value

  /// @brief State of an command in device
  enum class CmdStatusInDevice : std::int16_t
  {
    DEVICE_UNKNOWN = -1,        ///< command status in device is unknown
    DEVICE_NOT_SET = 0,         ///< command is not set
    DEVICE_IN_PROCESS,          ///< command is processing
    DEVICE_COMPLETED,           ///< command is completed
    DEVICE_ERROR,               ///< command is error
    DEVICE_INTERRUPTED,         ///< command is interrupted
  };

  /// @brief scaling strategy of axisprofile
  enum class AxisProfileScalingStrategy
  {
    UNKNOWN_STRATEGY = 0,       ///< unknown scaling strategy
    SOE_STANDARD,               ///< SoE scaling strategy
    COE_CALC_DRIVE,             ///< CoE scaling calculate by drive
    COE_CALC_CONTROL,           ///< CoE scaling calculate by control
    DEVICE_IO                   ///< scaling strategy for IO
  };

  /// @brief which signal is selected for measurement
  enum class MeasuringSignalSel
  {
    SIGNAL_UNDEFINED = -1,          ///< undefined signal
    POS_FEEDBACK_VAL_ENCODER_1,     ///< position feedback value of encoder 1
    POS_FEEDBACK_VAL_ENCODER_2,     ///< position feedback value of encoder 2
    SYSTEM_FINE_TIME,               ///< system fine time
    NO_COMMAND_ACTIVE,              ///< no command has been selected
  };

  /// @brief which probe is currently used
  enum class ProbeIndex
  {
    PROBE_UNDEFINED = -1,    ///< undefined probe
    PROBE_1,                 ///< probe 1
    PROBE_2,                 ///< probe 2
  };

  /// @brief Class enumeration indicates that use which method to set new position.
  enum class RefMode
  {
    UNDEF_METHOD = -1,  ///< undefined set position method
    SET_POS_DEVICE,     ///< set new position to device (new reference position store in drive)
    SET_POS_PROFILE,    ///< set new position to profile (new reference position store in axisprofile)
    SET_NONE,           ///< do nothing after homing, user may using there own method(e.g. axsSetAbsPosCmd on absolute measurement system) to set new position.
  };

  /// @brief Safety operation mode enumeration.
  ///
  /// These values are provided for safe motion related cyclic information
  /// for safe motion determination. A higher safety operation mode means a
  /// lower moving velocity. The velocity = 0 is the highest.
  enum class SafetyOperationMode
  {
    SOM_NORMAL_MODE = 0,        ///< Indicates no safety selected or active.
    SOM_SAFE_LIMITED_SPEED,     ///< Indicates safe-limited speed selected or active.
    SOM_SAFE_HOLD               ///< Indicates safe stop, current velocity is zero.
  };

  /// @brief This enum represents the 'ready to operate' part of the related protocol statusword.
  enum class ReadyToOperate
  {
    UNDEF_OPERATION = -1,                 ///< undefined operation status
    DRIVE_NOT_READY,                      ///< Not ready for power on, internal checks not concluded successfully, parametrization level (PL) is active
    DRIVE_READY_FOR_MAIN_POWER_ON,        ///< Ready for power on, internal checks are concluded successfully(bb), operating level (OL) is active
    DRIVE_READY_AND_MAIN_POWER_APPLIED,   ///< Control section and power section ready for op(Ab), power stage pulses are blocked, operating level (OL) is active
    DRIVE_ENABLED                         ///< In operation, with torque (e.g. AF), drive enable and drive ON are set and active, power stage is active
  };

  /// @brief This is a list of the function might be supported.
  enum class FunctionSupport
  {
    OPMODE_CYCLIC = 0,     ///< The operation mode is from cyclic data.
    HOMING_STATUS_CYCLIC,  ///< The homing status information is from cyclic data.
    HOMING_CMD_CYCLIC,     ///< The homing command trigger is from cyclic data.
    ERROR_CODE_CYCLIC,     ///< The diagnostic message from cyclic data
    MAX_VALUE              ///< Maximum value of this enumeration, put new value above.
  };

  /// @brief This is the error types of a function not supported.
  enum class FuncSupportErrorType : std::uint16_t
  {
    ERROR_NONE = 0,                        ///< no error, functionality is supported.
    ERROR_DEVICE_NOT_SUPPORT,              ///< not support by the device
    ERROR_GENERAL_CONFIG_NOT_SUPPORT,      ///< not supported caused by general configuration.
    ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT,  ///< not supported without required cyclic data config.
    ERROR_CHECK_POINT_NOT_IMPLEMENTED,     ///< unknown, due to checkpoints not be implemented.
    ERROR_NOT_DEFINED = 0xFFFF             ///< not defined error
  };

  // ---------------------------------------------------------------------------
  // Declaration of helper methods
  // ---------------------------------------------------------------------------
  /// @brief Supported AxisProfile Types as String
  const struct AxisProfileDataScalingTypeString
  {
    const std::string UNDEFINED = "UNDEFINED";       ///< Undefined scaling type
    const std::string ROTARY = "ROTARY";             ///< rotary
    const std::string PERCENTAGE = "PERCENTAGE";     ///< Percentage
    const std::string LINEAR = "LINEAR";             ///< linear
  } AprTypeDataScalingTypeStrings;                   ///< string

  /// @brief Supported AxisProfile Types as String
  const struct AxisProfileTypeStrings
  {
    const std::string UNKNOWN = "Unknown";                                      ///< Unknown profile type
    const std::string SERVODRIVE_OVER_ETHERCAT = "Servodrive_over_Ethercat";    ///< Type Servodrive over Ethercat (SoE)
    const std::string CANOPEN_OVER_ETHERCAT = "CANopen_over_Ethercat";          ///< Type CANopen application over Ethercat (CoE)
  } AprTypeStrings;

  ///< @brief
  const struct AxisProfileCategoryTypeStrings
  {
    const std::string UNKNOWN = "Unknown";                         ///< Unknown profile usage mode
    const std::string CATEGORY_DRIVE = "CATEGORY_DRIVE";           ///< for drive
    const std::string CATEGORY_ENCODER = "CATEGORY_ENCODER";       ///< for encoder
  }AprCategoryStrings;

  /// @brief Device error type as string
  const struct AxisProfileDeviceErrorTypeStrings
  {
    const std::string ERROR_NO_INFO = "NoErrorInfo";          ///< no error information can be required
    const std::string ERROR_NONE = "NoError";                 ///< no error
    const std::string ERROR_LIGHT = "LightError";             ///< light error
    const std::string ERROR_FIELDBUS = "FieldbusError";       ///< field bus error
    const std::string ERROR_CRITICAL = "CritalError";         ///< critical error
    const std::string ERROR_FATAL = "FatalError";             ///< fatal error
    const std::string ERROR_NOT_DEFINED = "NotDefinedError";  ///< none defined error
  }AprDevErrTypeStrings;

  /// @brief Supported AxisProfile strategy as String
  const struct AxisProfileScalingStrategyStrings
  {
    const std::string UNKNOWN = "UNKNOWN";                    ///< Unknown profile type
    const std::string SOE_STANDARD = "SOE_STANDARD";          ///< Type Servodrive over Ethercat (SoE)
    const std::string COE_CALC_DRIVE = "COE_CALC_DRIVE";      ///< Type scaling via drive
    const std::string COE_CALC_CONTROL = "COE_CALC_CONTROL";  ///< Type CoE third party scaling via controller
    const std::string DEVICE_IO = "DEVICE_IO";                ///< scaling for IO device
  } AprScalingStrgyStrings;

  /// @brief Operation mode supported error type as String
  const struct OpModeSupportedErrorTypeStrings
  {
    const std::string ERROR_NONE = "no error";                                                      ///< No error
    const std::string ERROR_NOT_SUPPORT = "not supported";                                          ///< Not supported
    const std::string ERROR_DRIVE_CONFIG = "not configured corresponding operation mode in Drive";  ///< Not configured corresponding operation mode in Drive
    const std::string ERROR_CYCLIC_DATA_MISSING = "not assigned corresponding cyclic data in Axis Profiles"; ///< Not assigned corresponding cyclic data in Axis Profiles
    const std::string ERROR_NOT_DEFINED = "not defined";                                            ///< Not defined
  } OpModeSupportedErrorTypeStrings;

  /// @brief Operation mode supported error type as String
  const struct DataAccessCommonErrorTypeStrings
  {
    const std::string ERROR_NONE = "no error";                                                            ///< No error
    const std::string ERROR_NOT_PROVIDED = "not provided";                                                ///< Not supported
    const std::string ERROR_MASTER_STATUS = "EtherCAT master error";                                      ///< linkage error occur on fieldbus
    const std::string ERROR_SLAVE_STATUS = "error status on slave device side";                           ///< slave not in right status
    const std::string ERROR_CFG_MAPPING_ERROR = "the corresponding item not assigned in Axis Profiles";   ///< Not assigned corresponding cyclic data in Axis Profiles
    const std::string ERROR_BEYOND_THE_LIMIT = "actual value beyond the limitation on device side";       ///< the actual value beyond the device limitation
    const std::string ERROR_NOT_DEFINED = "not defined";                                                  ///< Not defined
  } DataAccessCommonErrorTypeStrings;

  /// @brief Convert a string to the associated axis profile type.
  /// @param[in]  typeString   The
  /// @return The converted axis profile type.
  inline AxisProfileType stringToAxisProfileType(const char* typeString)
  {
    if (!typeString)
    {
      return AxisProfileType::UNDEFINED;
    }

    std::string buffer(typeString);
    if (buffer.compare(AprTypeStrings.SERVODRIVE_OVER_ETHERCAT) == 0)
    {
      return AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
    }
    if (buffer.compare(AprTypeStrings.CANOPEN_OVER_ETHERCAT) == 0)
    {
      return AxisProfileType::CANOPEN_OVER_ETHERCAT;
    }
    return AxisProfileType::UNDEFINED;
  }

  /// @brief Convert an axis profile type to the associated string.
  /// @param[in]  type   The axis profile type
  /// @return The converted axis profile type string.
  inline const char* axisProfileTypeToString(const AxisProfileType type)
  {
    switch (type)
    {
      case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:   return AprTypeStrings.SERVODRIVE_OVER_ETHERCAT.c_str(); break;
      case AxisProfileType::CANOPEN_OVER_ETHERCAT:      return AprTypeStrings.CANOPEN_OVER_ETHERCAT.c_str(); break;
      default:                                          return AprTypeStrings.UNKNOWN.c_str(); break;
    }
  }

  /// @brief Convert an axis profile type to the associated abbreviation string.
  /// @param[in]   type   Type of the profile
  /// @return The associated abbreviation as string
  inline const char* axisProfileTypeToAbbreviationString(const AxisProfileType type)
  {
    switch (type)
    {
      case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:   return "SoE";
      case AxisProfileType::CANOPEN_OVER_ETHERCAT:      return "CoE";
      default:                                          return AprTypeStrings.UNKNOWN.c_str();
    }
  }

  /// @brief Convert a string to the associated axis profile usage mode.
  /// @param[in]  typeString   The input type string.
  /// @return The converted axis profile usage mode.
  inline AxisProfileCategory stringToAxisProfileCategory(const char* typeString)
  {
    if (!typeString)
    {
      return AxisProfileCategory::UNDEFINED;
    }

    std::string buffer(typeString);
    if (buffer.compare(AprCategoryStrings.CATEGORY_DRIVE) == 0)
    {
      return AxisProfileCategory::CATEGORY_DRIVE;
    }
    if (buffer.compare(AprCategoryStrings.CATEGORY_ENCODER) == 0)
    {
      return AxisProfileCategory::CATEGORY_ENCODER;
    }
    return AxisProfileCategory::UNDEFINED;
  }

  /// @brief Convert an axis profile usage mode to the associated string.
  /// @param[in]  usageMode   The axis profile usage mode
  /// @return The converted axis profile usage mode string.
  inline const char* axisProfileCategoryTypeToString(const AxisProfileCategory usageMode)
  {
    switch (usageMode)
    {
      case AxisProfileCategory::CATEGORY_DRIVE:   return AprCategoryStrings.CATEGORY_DRIVE.c_str(); break;
      case AxisProfileCategory::CATEGORY_ENCODER: return AprCategoryStrings.CATEGORY_ENCODER.c_str(); break;
      default:                                    return AprCategoryStrings.UNKNOWN.c_str(); break;
    }
  }

  /// @brief Convert a axis profile operation mode to the associated string.
  /// @param[in]  opMode   The axis profile operation mode
  /// @return The converted axis profile operation mode string.
  inline const char* axisProfileOpModeToString(const AxisProfileOperationMode opMode)
  {
    switch (opMode)
    {
      case AxisProfileOperationMode::POSITIONING:   return "Positioning"; break;
      case AxisProfileOperationMode::VELOCITY:      return "Velocity"; break;
      case AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT: return "TorqueForce with velocity limitation"; break;
      case AxisProfileOperationMode::HOMING:        return "Homing"; break; // CoE only
      default:                                      return "Unknown"; break;
    }
  }

  /// @brief Convert an axis profile type to the associated string.
  /// @param[in]  type   The axis profile type
  /// @return The converted axis profile type string.
  inline const char* axisProfileDeviceScalingTypeToString(const AxisProfileDeviceScalingType type)
  {
    switch (type)
    {
      case AxisProfileDeviceScalingType::PERCENTAGE:       return AprTypeDataScalingTypeStrings.PERCENTAGE.c_str();   break;
      case AxisProfileDeviceScalingType::LINEAR:           return AprTypeDataScalingTypeStrings.LINEAR.c_str();       break;
      case AxisProfileDeviceScalingType::ROTATIONAL:       return AprTypeDataScalingTypeStrings.ROTARY.c_str();       break;
      default:                                             return AprTypeDataScalingTypeStrings.UNDEFINED.c_str();    break;
    }
  }

  /// @brief Convert device error type to string
  /// @param[in]   type   The error type of device
  /// @return The converted error type in string
  inline const char* axisProfileDeviceErrorTypeToString(const AxisProfileDeviceErrorType type)
  {
    switch (type)
    {
      case AxisProfileDeviceErrorType::ERROR_NO_INFO:      return AprDevErrTypeStrings.ERROR_NO_INFO.c_str();        break;
      case AxisProfileDeviceErrorType::ERROR_NONE:         return AprDevErrTypeStrings.ERROR_NONE.c_str();           break;
      case AxisProfileDeviceErrorType::ERROR_LIGHT:        return AprDevErrTypeStrings.ERROR_LIGHT.c_str();          break;
      case AxisProfileDeviceErrorType::ERROR_FIELDBUS:     return AprDevErrTypeStrings.ERROR_FIELDBUS.c_str();       break;
      case AxisProfileDeviceErrorType::ERROR_CRITICAL:     return AprDevErrTypeStrings.ERROR_CRITICAL.c_str();       break;
      case AxisProfileDeviceErrorType::ERROR_FATAL:        return AprDevErrTypeStrings.ERROR_FATAL.c_str();          break;
      default:                                             return AprDevErrTypeStrings.ERROR_NOT_DEFINED.c_str();    break;
    }
  }

  /// @brief Convert operation mode supported error type to string
  /// @param[in]   type   The error type of device
  /// @return The converted error type in string
  inline const char* opModeSupportedErrorTypeToString(const OpModeSupportedErrorType type)
  {
    switch (type)
    {
      case OpModeSupportedErrorType::ERROR_NONE:                return OpModeSupportedErrorTypeStrings.ERROR_NONE.c_str();                break;
      case OpModeSupportedErrorType::ERROR_NOT_SUPPORT:         return OpModeSupportedErrorTypeStrings.ERROR_NOT_SUPPORT.c_str();         break;
      case OpModeSupportedErrorType::ERROR_DRIVE_CONFIG:        return OpModeSupportedErrorTypeStrings.ERROR_DRIVE_CONFIG.c_str();        break;
      case OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING: return OpModeSupportedErrorTypeStrings.ERROR_CYCLIC_DATA_MISSING.c_str(); break;
      default:                                                  return OpModeSupportedErrorTypeStrings.ERROR_NOT_DEFINED.c_str();         break;
    }
  }

  inline const char* dataAccessCommonErrorTypeToString(const DataAccessCommonErrorType type)
  {
    switch (type)
    {
      case DataAccessCommonErrorType::ERROR_NONE:                 return DataAccessCommonErrorTypeStrings.ERROR_NONE.c_str();              break;
      case DataAccessCommonErrorType::ERROR_NOT_PROVIDED:         return DataAccessCommonErrorTypeStrings.ERROR_NOT_PROVIDED.c_str();      break;
      case DataAccessCommonErrorType::ERROR_MASTER_STATUS:        return DataAccessCommonErrorTypeStrings.ERROR_MASTER_STATUS.c_str();     break;
      case DataAccessCommonErrorType::ERROR_SLAVE_STATUS:         return DataAccessCommonErrorTypeStrings.ERROR_SLAVE_STATUS.c_str();      break;
      case DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR:    return DataAccessCommonErrorTypeStrings.ERROR_CFG_MAPPING_ERROR.c_str(); break;
      case DataAccessCommonErrorType::ERROR_BEYOND_THE_LIMIT:     return DataAccessCommonErrorTypeStrings.ERROR_BEYOND_THE_LIMIT.c_str();  break;
      default:                                                    return DataAccessCommonErrorTypeStrings.ERROR_NOT_DEFINED.c_str();       break;
    }
  }

  /// @brief Convert scaling strategy type to string
  /// @param[in]   strategy   The scaling strategy type of profile
  /// @return The converted scaling strategy type in string
  inline const char* axisProfileScalingStrategyToString(const AxisProfileScalingStrategy strategy)
  {
    switch (strategy)
    {
      case AxisProfileScalingStrategy::SOE_STANDARD:      return AprScalingStrgyStrings.SOE_STANDARD.c_str();        break;
      case AxisProfileScalingStrategy::COE_CALC_DRIVE:    return AprScalingStrgyStrings.COE_CALC_DRIVE.c_str();      break;
      case AxisProfileScalingStrategy::COE_CALC_CONTROL:  return AprScalingStrgyStrings.COE_CALC_CONTROL.c_str();    break;
      case AxisProfileScalingStrategy::DEVICE_IO:         return AprScalingStrgyStrings.DEVICE_IO.c_str();           break;
      default:                                            return AprScalingStrgyStrings.UNKNOWN.c_str();             break;
    }
  }

  /// @brief Convert axisprofile command state to string
  /// @param[in]   state   axisprofile command state
  /// @return The converted command state in string
  inline const char* axisProfileCommandStateToString(const AxisprofileCommandState state)
  {
    switch (state)
    {
      case AxisprofileCommandState::CMD_STATE_IDLE:         return "Idle";
      case AxisprofileCommandState::CMD_STATE_INIT:         return "Init";
      case AxisprofileCommandState::CMD_STATE_INQUEUE:      return "InQueue";
      case AxisprofileCommandState::CMD_STATE_ACTIVE:       return "Active";
      case AxisprofileCommandState::CMD_STATE_INTERRPUT:    return "Interrupt";
      case AxisprofileCommandState::CMD_STATE_ABORTING:     return "Aborting";
      case AxisprofileCommandState::CMD_STATE_ERROR:        return "Error";
      default:                                              return "NotDefinedState";
    }
  }

  /// @brief Convert axisprofile command type to string
  /// @param[in] type   axisprofile command type
  /// @return The converted error type in string
  inline const char* cmdTypeToString(const CmdType type)
  {
    switch (type)
    {
      case CmdType::CLEAR_DEV_ERR:              return "Clear Device Error";
      case CmdType::SET_ABS_POS:                return "Set Absolute Position";
      case CmdType::REFERENCE_POS:              return "Referencing Position";
      case CmdType::FIXED_STOP:                 return "Fixed Stop";
      case CmdType::SET_NEW_REF_WITHOUT_MOVE:   return "Set New Reference Without Move  ";
      default:  return "NotDefinedType";
    }
  }

  /// @brief Convert axisprofile command state to string
  /// @param[in]   state   function state
  /// @return The converted state in string
  inline const char* cmdStateToString(const CmdState state)
  {
    switch (state)
    {
      case CmdState::STATE_UNKNOWN:     return "Unknown";
      case CmdState::STATE_IDLE:        return "Idle";
      case CmdState::STATE_ACTIVATING:  return "Activating";
      case CmdState::STATE_ACTIVE:      return "Active";
      case CmdState::STATE_ABORTING:    return "Aborting";
      case CmdState::STATE_ERROR:       return "Error";
      default:                          return "NotDefinedState";
    }
  }

  /// @brief Convert axisprofile command status in device to string
  /// @param[in]   state   function state
  /// @return The converted state in string
  inline const char* cmdStatusInDeviceToString(const CmdStatusInDevice state)
  {
    switch (state)
    {
      case CmdStatusInDevice::DEVICE_UNKNOWN:       return "Unknown";
      case CmdStatusInDevice::DEVICE_NOT_SET:       return "NotSet";
      case CmdStatusInDevice::DEVICE_IN_PROCESS:    return "InProcess";
      case CmdStatusInDevice::DEVICE_COMPLETED:     return "Completed";
      case CmdStatusInDevice::DEVICE_ERROR:         return "Error";
      case CmdStatusInDevice::DEVICE_INTERRUPTED:   return "Interrupted";
      default:                                      return "NotDefinedStatus";
    }
  }
  
  /// @brief Convert axisprofile command type to string
  /// @param[in] type   axisprofile command type
  /// @return The converted error type in string
  inline const char* axisProfileCommandTypeToString(const AxisProfileCommandType type)
  {
    switch (type)
    {
      case AxisProfileCommandType::CMD_CLEAR_DEVICE_ERROR:            return "ClearDeviceError";          break;
      case AxisProfileCommandType::CMD_SET_CHECK_POSITIVE_STOP:       return "SetCheckPositiveStop";      break;
      case AxisProfileCommandType::CMD_SET_CHECK_CLAMPING_TORQUE:     return "SetCheckClampingTorque";    break;
      case AxisProfileCommandType::CMD_GET_CLAMPING_TORQUE:           return "GetClampingTorque";         break;
      case AxisProfileCommandType::CMD_CHECK_STANDSTILL:              return "CheckStandStill";           break;
      case AxisProfileCommandType::CMD_SET_CHECK_STANDSTILL_WINDOW:   return "SetCheckStandStill";        break;
      case AxisProfileCommandType::CMD_GET_STANDSTILL_WINDOW:         return "GetStandStillWindow";       break;
      case AxisProfileCommandType::CMD_ACTIVATE_STANDSTILL_CHECK:     return "ActivateStandstillCheck";   break;
      case AxisProfileCommandType::CMD_DEACTIVATE_STANDSTILL_CHECK:   return "DeactivateStandstillCheck"; break;
      case AxisProfileCommandType::CMD_SET_ABSOLUTE_POSITION:         return "SetAbsolutPosition";        break;
      case AxisProfileCommandType::CMD_REFERENCING_AXIS:              return "ReferencingAxis";           break;
      case AxisProfileCommandType::CMD_RESET_REFERENCING_AXIS:        return "ResetReferencingAxis";      break;
      case AxisProfileCommandType::CMD_SET_REFERENCING_POS:           return "SetReferencingPosition";    break;
      case AxisProfileCommandType::CMD_CHECK_AXIS_HOMED:              return "CheckAxisHomed";            break;
      case AxisProfileCommandType::CMD_RESET_ABSOLUTE_POSITION:       return "ResetAbsolutePosition";     break;
      case AxisProfileCommandType::CMD_CHECK_DEVICE_HALT:             return "CheckDeviceHalted";         break;
      default:                                                        return "AxisprofileCmdNotDefined";  break;
    }
  }

  /// @brief Convert axisprofile power mode to string.
  /// @param[in] mode   axisprofile power mode
  /// @return The converted axisprofile power mode in string.
  inline const char* axisprofilePowerModeToString(const AxisProfilePowerMode mode)
  {
    switch (mode)
    {
      case AxisProfilePowerMode::POWER_ON:                     return "PowerOn";                 break;
      case AxisProfilePowerMode::POWER_OFF:                    return "PowerOff";                break;
      case AxisProfilePowerMode::POWER_OFF_TORQUE_FREE:        return "PowerOffTorqueFree";      break;
      default:                                                 return "UNDEFINED";               break;
    }
  }

  /// @brief Convert probe index to string.
  /// @param[in]  index    Probe index
  /// @return The converted probe index in string
  inline const char* probeIndexToString(const ProbeIndex index)
  {
    switch (index)
    {
      case ProbeIndex::PROBE_1:             return "Probe1";              break;
      case ProbeIndex::PROBE_2:             return "Porbe2";              break;
      default:                              return "PorbeUndefined";      break;
    }
  }

  /// @brief Convert an axis profile operation status to the associated string.
  /// @param[in]  type   The axis profile operation status
  /// @return The converted axis profile operation status string.
  inline const char* axisProfileReadyToOpTypeToString(const ReadyToOperate type)
  {
    switch (type)
    {
      case ReadyToOperate::DRIVE_NOT_READY :                    return  "Drive not ready";                break;
      case ReadyToOperate::DRIVE_READY_FOR_MAIN_POWER_ON :      return  "Drive ready for main power on";  break;
      case ReadyToOperate::DRIVE_READY_AND_MAIN_POWER_APPLIED : return  "Control section and power section ready for op.";  break;
      case ReadyToOperate::DRIVE_ENABLED  :                     return  "In operation, with torque";      break;
      default:                                                  return  "Undefined operation status";     break;
    }
  }

  /// @brief Convert axisprofile reference mode to string
  /// @param[in]  mode      The axisprofile reference mode
  /// @return The converted axisprofile reference mode string
  inline const char* axisprofileRefModeToString(RefMode mode)
  {
    switch (mode)
    {
      case RefMode::SET_POS_DEVICE:     return "SetReferenceToDevice";
      case RefMode::SET_POS_PROFILE:    return "SetReferenceToControl";
      case RefMode::SET_NONE:           return "DoNotSetReference";
      default:                          return "UndefinedReferenceMode";
    }
  }

  /// @brief Convert a string to the associated axis profile type.
  /// @param[in]  methodString   The input type string.
  /// @return The converted axis profile type.
  inline AxisProfileScalingStrategy stringToAxisProfileScalingStrategy(const char* methodString)
  {
    if (!methodString)
    {
      return AxisProfileScalingStrategy::UNKNOWN_STRATEGY;
    }

    std::string buffer(methodString);
    if (buffer.compare(AprScalingStrgyStrings.SOE_STANDARD) == 0)
    {
      return AxisProfileScalingStrategy::SOE_STANDARD;
    }
    if (buffer.compare(AprScalingStrgyStrings.COE_CALC_DRIVE) == 0)
    {
      return AxisProfileScalingStrategy::COE_CALC_DRIVE;
    }
    if (buffer.compare(AprScalingStrgyStrings.COE_CALC_CONTROL) == 0)
    {
      return AxisProfileScalingStrategy::COE_CALC_CONTROL;
    }
    if (buffer.compare(AprScalingStrgyStrings.DEVICE_IO) == 0)
    {
      return AxisProfileScalingStrategy::DEVICE_IO;
    }
    return AxisProfileScalingStrategy::UNKNOWN_STRATEGY;
  }

  /// @brief Convert a string to the associated axis profile type.
  /// @param[in]  deviceScalingType   The input type string.
  /// @return The converted axis profile type.
  inline AxisProfileDeviceScalingType stringToAxisProfileDeviceScalingType(const char* deviceScalingType)
  {
    if (!deviceScalingType)
    {
      return AxisProfileDeviceScalingType::UNDEFINED;
    }

    std::string buffer(deviceScalingType);
    if (buffer.compare(AprTypeDataScalingTypeStrings.ROTARY) == 0)
    {
      return AxisProfileDeviceScalingType::ROTATIONAL;
    }
    if (buffer.compare(AprTypeDataScalingTypeStrings.PERCENTAGE) == 0)
    {
      return AxisProfileDeviceScalingType::PERCENTAGE;
    }
    if (buffer.compare(AprTypeDataScalingTypeStrings.LINEAR) == 0)
    {
      return AxisProfileDeviceScalingType::LINEAR;
    }
    return AxisProfileDeviceScalingType::UNDEFINED;
  }

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief The velocity limit status of torque mode(with velocity limitation)
  enum class TorqueVelocityLimitStatus : std::uint8_t
  {
    NO_VIOLATE,             ///< no limitation active due to positive speed limit value violation 
    VIOLATE_HIGH_VEL,       ///< limitation active due to positive speed limit value violation
    VIOLATE_LOW_VEL         ///< limitation active due to negative speed limit value violation
  };

  /// @brief Container for actual data of the device
  typedef struct ActualValuesDevice
  {
    double                      m_actualPosition;       ///< Current position of the drive
    bool                        m_dataValid;            ///< Indication if data are valid
    double                      m_actualVelocity;       ///< Current velocity of the drive
    double                      m_actualTorqueForce;    ///< Current torque of the drive
    TorqueVelocityLimitStatus   m_torqueLimitStatus;    ///< Torque limitation status
  }ActualValuesDevice;

  /// @brief Container for command data of the control
  typedef struct CommandValuesDevice
  {
    double m_commandPosition;      ///< Commanded position of the controller
    double m_commandVelocity;      ///< Commanded velocity of the controller
    double m_commandTorqueForce;   ///< COmmanded torque of the controller
    double m_torqueVelLimitHigh;   ///< high velocity limitation for torque mode 
    double m_torqueVelLimitLow;    ///< low velocity limitation for torque mode
  }CommandValuesDevice;

  /// @brief Container for error information while check operation mode supported
  typedef struct OpModeErrorInfo
  {
    OpModeSupportedErrorType m_error; ///< Enum for error type
    std::string m_addInfo;            ///< Contain the additional information of error
  }OpModeErrorInfo;

  /// @brief Define the scaling Factor
  struct ScalingFactor
  {
    bool      m_enabled {false};        ///< is scaling factor activated
    std::uint32_t  m_numerator {1};     ///< numerator for scaling factor
    std::uint32_t  m_denominator {1};   ///< denominator for scaling factor
  };

  /// @brief Define the scaling strategy configuration data type
  typedef struct AxisProfileScalingStrategyCfg
  {
    AxisProfileScalingStrategy   m_scalingStrategy = AxisProfileScalingStrategy::UNKNOWN_STRATEGY;   ///< scaling strategy
    AxisProfileDeviceScalingType m_deviceScalingType = AxisProfileDeviceScalingType::UNDEFINED;      ///< device scaling type
    std::uint32_t                m_numerator = 1;                                                    ///< numerator for scaling factor
    std::uint32_t                m_denominator = 1;                                                  ///< denominator for scaling factor
    std::uint32_t                m_resolution = 1;                                                   ///< resolution of the motor encoder
    ScalingFactor                m_velFactor {};                                                     ///< velocity scaling factor
  }AxisProfileScalingStrategyCfg;

  /// @brief Configure the probe control word
  struct ProbeBaseConfig
  {
    bool m_measureType = false;       ///< false for single shot measurement, true for continuous measurement
    bool m_evaluatePosEdge = false;   ///< true to activate positive edge measurement
    bool m_evaluateNegEdge = false;   ///< true to activate negative edge measurement
  };

  /// @brief Configure the extended probe control word
  struct ProbeExtConfig
  {
    bool m_isExpWindowActivated = false;      ///< is expect window of probe activated
    bool m_isMarkerFailureActivated = false;  ///< is marker failure activated
    bool m_isTimeMeasureActivated = false;    ///< is time measurement activated
    double m_expectWindowPosStart = 0.0;      ///< start position of expect window
    double m_expectWindowPosEnd = 0.0;        ///< end position of expect window
    double m_timeCompensationPosEdge = 0.0;   ///< the dead time compensation of positive edge
    double m_timeCompensationNegEdge = 0.0;   ///< the dead time compensation of negative edge
    MeasuringSignalSel m_measuredSignal = MeasuringSignalSel::SIGNAL_UNDEFINED; ///< select a measurement signal
  };

  /// @brief Struct probe actual state and data of the device
  struct ProbeActualValues
  {
    bool m_isProbeActivated = false;         ///< is probe activated
    std::uint16_t m_posEdgeLatchedTimes = 0; ///< probe positive edge latched times
    std::uint16_t m_negEdgeLatchedTimes = 0; ///< probe negative edge latched times

    double m_probeLatchedPosEdge = 0.0;      ///< positive edge latched value
    double m_probeLatchedNegEdge = 0.0;      ///< negative edge latched value
    double m_probeEdgeDiff = 0.0;            ///< difference of positive and negative latched value
    std::uint16_t m_probeMarkeFailures = 0;  ///< marker failure counter
  };

  /// @brief online update probe measurement items
  struct ProbeUpdateMeasurement
  {
    bool m_evaluatePosEdge = false;           ///< true to activate positive edge measurement
    bool m_evaluateNegEdge = false;           ///< true to activate negative edge measurement
    bool m_isMarkerFailureActivated = false;  ///< is marker failure activated
    bool m_isExpWindowActivated = false;      ///< is expected window of probe activated
    bool m_measureType = false;               ///< false for single shot measurement, true for continuous measurement

    double m_expectWindowStart = 0.0;         ///< start position of expect window
    double m_expectWindowEnd = 0.0;           ///< end position of expect window
  };

  /// @brief Union data of user PDO
  union UserPDOData
  {
    std::uint16_t  m_ui16;   ///< used for unsigned 2-byte conversion
    std::int16_t   m_i16;    ///< used for signed 2-byte conversion
    std::uint32_t  m_ui32;   ///< used for unsigned 4-byte conversion
    std::int32_t   m_i32;    ///< used for signed 4-byte conversion
    std::uint64_t  m_ui64;   ///< used for unsigned 8-byte conversion
    std::int64_t   m_i64;    ///< used for signed 8-byte conversion
  };

  /// @brief User PDO data 
  struct UserPDODataInfo
  {
    UserPDOData     m_userPDOData;    ///< union of user PDO data
    bool            m_active;         ///< flag indicate whether current data are active
  };

  /// @brief Error information for checking the supported functionality
  struct FunctionSupportErrorInfo
  {
    FuncSupportErrorType errorType;   ///< Enum for error type
    std::string            addInfo;   ///< Contain the additional information of error
  };
}
