#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and contains internal defines relevant for all axis profiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include <array>
#include <atomic>
#include <functional>
#include <limits>
#include <thread>
#include <cstdio>
#include <string>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::firmware_related
{
  ///< Bug 1009289, V0606 have different definition for torque limitation check
  constexpr std::uint8_t TORQUE_LIMIT_STATUS_MAJOR_VERSION = 6;
  constexpr std::uint8_t TORQUE_LIMIT_STATUS_MINOR_VERSION = 6;

  //The checksum function is only supported by ctrlX drive V0502 and higher and indra drive
  constexpr std::uint8_t CHECKSUM_MAJOR_VERSION = 5;
  constexpr std::uint8_t CHECKSUM_MINOR_VERSION = 2;
}

namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr double DBL_INF = std::numeric_limits<double>::infinity(); ///< infinitely large value, is larger than the maximum value of double
  constexpr double STD_EPS = 1E-6;                                    ///< default epsilon for compare with float values
  constexpr double PERCENT = 1E-2;                                    ///< percentage
  constexpr double CIRCLE_DEG = 360.0;                                ///< degree of full circle
  constexpr double DECIM_PL3 = 1E-3;                                  ///< decimal place 3
  constexpr double STD_CYCLE_TIME = 2E-3;                             ///< default cycle time in second
  constexpr double SCALING_EPS = 1E-10;                               ///< epsilon for compare with float values in scaling
  constexpr std::uint32_t DEFAULT_ENCODER_VALUE = 8388608;            ///< 23-bit encoder value
  constexpr std::uint32_t STD_CYCLE_TIME_MS = 2;                      ///< default cycle time in millisecond, NOTE take care of cycle time lower than 1ms
  constexpr std::uint8_t USER_PDO_COUNT_LIMIT = 6;                    ///< limitation for user PDO count

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  #define POW2(x) ((x) * (x))       ///< square power
  #define POW3(x) (POW2(x) * (x))   ///< cube power
  #define POW4(x) (POW3(x) * (x))   ///< fourth power

  #define SECOND_2_MILLISECOND(x) static_cast<double>(x) * 1E3        ///< convert second to millisecond
  #define MILLISECOND_2_SECOND(x) static_cast<double>(x) * 1E-3       ///< convert millisecond to second
  #define SECOND_2_MICROSECOND(x) static_cast<double>(x) * 1E6        ///< convert second to microsecond
  #define MICROSECOND_2_SECOND(x) static_cast<double>(x) * 1E-6       ///< convert microsecond to second
  #define SECOND_2_NANOSECOND(x)  static_cast<double>(x) * 1E9        ///< convert second to nanosecond
  #define NANOSECOND_2_SECOND(x)  static_cast<double>(x) * 1E-9       ///< convert nanosecond to second

  #define MASK1(L,N,P)  ((~((~(std::uint##L##_t)(0))<<N))<<P)         ///< Creates a length of `L` mask with `N' 1 bits at position `P'.

  #define ENUM_TO_U16(x) static_cast<std::uint16_t>(x)                ///< Macro to convert enum to uint16_t
  #define ENUM_TO_I16(x) static_cast<std::int16_t>(x)                 ///< Macro to convert enum to int16_t
  #define ENUM_TO_U32(x) static_cast<std::uint32_t>(x)                ///< Macro to convert enum to uint32_t

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief Supported IEC 61800-7 Drive Profiles
  enum class DriveProfileType
  {
    UNDEFINED = 0,   ///< Undefined drive profile
    SERCOS,          ///< Drive profile SERCOS (Type 4)
    CANOPEN,         ///< Drive profile CiA402
    MAX_VAL          ///< Helper for loop iterators (add new values above!)
  };

  /// @brief Supported Network Technologies
  enum class NetworkTechnology
  {
    UNDEFINED = 0,   ///< Undefined network technology
    ETHERCAT,        ///< EtherCAT (CP12)

    MAX_VAL          ///< Helper for loop iterators (add new values above!)
  };

  /// @brief State machine running or not
  enum class StateMachineState
  {
    ERROR_STATE = -1,   ///< error state
    BOOTING,            ///< booting state
    CONFIG,             ///< configuration state
    RUNNING,            ///< running state
  };

  /// @brief unit of time in the drive
  enum class TimeUnit
  {
    MINUTE    = 0,      ///< minute
    SECOND    = 1,      ///< second
  };

  /// @brief velocity scaling type
  enum class VelScalingType
  {
    UNDEFINED = -1,     ///< undefined
    ROTARY    = 0,      ///< rotary
    LINEAR    = 2,      ///< linear
  };

  /// @brief position scaling type
  enum class PosScalingType
  {
    UNDEFINED = -1,     ///< undefined
    ROTARY    = 0,      ///< rotary
    LINEAR    = 2,      ///< linear
  };

  /// @brief acceleration scaling type
  enum class AccScalingType
  {
    UNDEFINED = -1,     ///< undefined
    ROTARY    = 0,      ///< rotary
    LINEAR    = 2,      ///< linear
  };

  /// @brief torque scaling type
  /// @note do not change the order of the enumeration, since it is related to AxisProfileDeviceScalingType in axisprofile_defs.h
  enum class TorqueScalingType
  {
    UNDEFINED  = -1,    ///< undefined
    ROTARY     = 0,     ///< rotary
    PERCENTAGE = 1,     ///< percentage
    LINEAR     = 2,     ///< linear
  };

  /// @brief callable type for scheduler
  enum class CallableType
  {
    ADAPTION_CALLABLE = 1,      ///< adaption callable, which called before motion and after fieldbus
    DISTRIBUTION_CALLABLE,      ///< distribution callable, which called after motion
  };

  /// @brief Enum for User PDO mapping ID
  enum class UserPDOMappingID
  {
    USER_PDO_INPUT_1 = 0,    ///< user PDO input 1
    USER_PDO_INPUT_2,        ///< user PDO input 2
    USER_PDO_INPUT_3,        ///< user PDO input 3
    USER_PDO_INPUT_4,        ///< user PDO input 4
    USER_PDO_INPUT_5,        ///< user PDO input 5
    USER_PDO_INPUT_6,        ///< user PDO input 6
    USER_PDO_OUTPUT_1,       ///< user PDO output 1
    USER_PDO_OUTPUT_2,       ///< user PDO output 2
    USER_PDO_OUTPUT_3,       ///< user PDO output 3
    USER_PDO_OUTPUT_4,       ///< user PDO output 4
    USER_PDO_OUTPUT_5,       ///< user PDO output 5
    USER_PDO_OUTPUT_6,       ///< user PDO output 6
  };

  /// @brief User PDO input/output type
  enum class UserPDOType
  {
    USERPDO_TYPE_INPUT = 0,   ///< user DPO input type
    USERPDO_TYPE_OUTPUT       ///< user DPO output type
  };

  /// @brief Request of an axisprofile command
  enum class CmdReq : std::int16_t
  {
    REQ_UNKNOWN = -1,         ///< unknown request
    REQ_RESET,                ///< reset request to command
    REQ_START,                ///< start request to command
    REQ_ABORT,                ///< abort request to command

    REQ_MAX,                  ///< helper value, always keep at the end
    REQ_MIN = REQ_RESET,      ///< helper value, minimum type value
  };

  /// @brief Manufacturer device series
  enum class DeviceSeries : std::uint16_t
  {
    UNDEFINED = 0,                      ///< undefined device type
    INDRADRIVE,                         ///< indra drive
    CTRLXDRIVE,                         ///< ctrlX drive
    CTRLXDRIVE_XC20,                    ///< ctrlX drive, XC20

    KNOWN_GENERAL_3RD_DRIVE = 0x1000,   ///< 3rd party drive enum starting
    INNOVANCE,                          ///< 3rd party drive, innovance
    MOON,                               ///< 3rd party drive, moon

    NOTINDICATES                        ///< drive not supports for current fieldbus protocol
  };

  // ---------------------------------------------------------------------------
  // Declaration of field bus protocol specific types
  // ---------------------------------------------------------------------------

  /// @Note SoE(Sercos Over Ethercat) protocol uses IDN number to access parameter.
  /// S/P-<set>-<num>
  /// IDN is a unsigned 16 bits integer, bits defines as follow:
  /// --------------------------------
  /// |  bit15  | bit14~12 | bit11~0 |
  /// | P:1/S:0 |   set    |   num   |
  /// --------------------------------

  /// @brief This macro is used to convert a IDN S parameter format S-<set>-<num> to raw number
  /// @parma[in]  set   Set of the parameter
  /// @parma[in]  num   Number of the parameter
  /// @return    The raw number
  #define IDN_S(set, num) static_cast<std::uint16_t>((0x0FFF & num) | (0x7000 & set << 12))

  /// @brief This macro is used to convert a IDN P parameter format P-<set>-<num> to raw number
  /// @parma[in]  set   Set of the parameter
  /// @parma[in]  num   Number of the parameter
  /// @return    The raw number
  #define IDN_P(set, num) IDN_S(set, num) | 0x8000

  /// @brief This methods converts value to the name of the IDN parameter.
  /// @param[in]  idn   IDN value
  /// @return IDN name in string
  inline std::string idnToString(std::uint16_t idn)
  {
    bool isP = (idn & 0x8000) != 0;
    std::uint16_t set = (idn & 0x7000) >> 12;
    std::uint16_t num = idn & 0x0FFF;

    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%c-%u-%04u", (isP ? 'P' : 'S'), set, num);
    return std::string(buffer);
  }

  /// @Note CoE(Canopen Over Ethercat) protocol uses object directory entry to access the object.
  /// @brief Entry of Object Directory of CoE
  struct ObjectDirEntry
  {
    std::uint16_t index {};    ///< Index of CoE object
    std::uint8_t subIndex {};  ///< Subindex of CoE object

    /// @brief Equal operator overload
    /// @param[in] other    The other ObjectDirEntry to be compared
    /// @return True if both are equal
    bool operator==(const ObjectDirEntry& other) const
    {
      return index == other.index && subIndex == other.subIndex;
    }
  };

  /// @brief Convert Object Directory entry to string "0xAAAA::BB"
  inline std::string objDirToString(const ObjectDirEntry& objDirEntry)
  {
    std::array<char, 16> buf {};
    std::snprintf(buf.data(), buf.size(), "0x%04X::%02X", objDirEntry.index, objDirEntry.subIndex);
    return std::string(buf.data());
  }

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief command mutex helper
  typedef struct UniversalFlagHelper
  {
    /// @brief try_raiseFlag
    bool try_raiseFlag()
    {
      std::thread::id currentID = std::this_thread::get_id();
      if(!m_ocuppied || currentID == m_ownerID)
      {
        m_ocuppied= true;
        m_ownerID = currentID;
        return true;
      }
      return false;
    }
    /// @brief releaseFlag
    void releaseFlag()
    {
      m_ocuppied = false;
    }
    private:
    std::atomic<bool> m_ocuppied{false};      ///< indicates if it is occupied by a certain thread
    std::atomic<std::thread::id> m_ownerID{}; ///< id of current owner
  }UniversalFlag;

  /// @brief This struct contain the device configuration data
  struct DeviceCfgData
  {
    // drive limits
    std::int32_t m_positivePosLimit {INT32_MAX};   ///< positive position limit
    std::int32_t m_negativePosLimit {INT32_MIN};   ///< negative position limit
    std::int32_t m_positiveVelLimit {INT32_MAX};   ///< positive velocity limit
    std::int32_t m_negativeVelLimit {INT32_MIN};   ///< negative velocity limit
    std::uint32_t m_accLimit {UINT32_MAX};         ///< acceleration limit, default: no limit
    std::uint16_t m_trqLimitMax {UINT16_MAX};      ///< torque maximum limit, default: no limit
    bool m_softwareLimitActive {false};            ///< is the software limit activated

    // encoder properties
    std::uint16_t m_encoder1Property {0};     ///< property of encoder 1 of device
    std::uint16_t m_encoder2Property {0};     ///< property of encoder 2 of device
  };

  struct telegramMDT
  {
    uint32_t m_referenceOffset;
    uint8_t m_bit0;
  };

  /// @brief This struct contain the device's hardware information.
  struct DeviceHardwareInfo
  {
    std::uint32_t m_address {};                  ///< device address
    std::uint32_t m_supportedProtocol {};        ///< Bit 0: AoE (ADS over EtherCAT) (ADS: Automation Device Specification)
                                                 ///< Bit 1: EoE (Ethernet over EtherCAT)
                                                 ///< Bit 2: CoE (CAN application protocol over EtherCAT)
                                                 ///< Bit 3: FoE (File access over EtherCAT)
                                                 ///< Bit 4: SoE (Servo drive over EtherCAT)
                                                 ///< Bit 5: VoE (Vendor specific protocol over EtherCAT)
                                                 ///< Bit 6 to 31: Reserved"

    std::uint32_t m_vendorId {};                 ///< device vendor id
    std::uint32_t m_productCode {};              ///< device product code
    std::string   m_masterName {};               ///< master name
  };

  /// @brief bit field
  typedef struct WordBits
  {
    std::uint16_t bit0 : 1;     ///< bit0
    std::uint16_t bit1 : 1;     ///< bit1
    std::uint16_t bit2 : 1;     ///< bit2
    std::uint16_t bit3 : 1;     ///< bit3
    std::uint16_t bit4 : 1;     ///< bit4
    std::uint16_t bit5 : 1;     ///< bit5
    std::uint16_t bit6 : 1;     ///< bit6
    std::uint16_t bit7 : 1;     ///< bit7
    std::uint16_t bit8 : 1;     ///< bit8
    std::uint16_t bit9 : 1;     ///< bit9
    std::uint16_t bit10 : 1;    ///< bit10
    std::uint16_t bit11 : 1;    ///< bit11
    std::uint16_t bit12 : 1;    ///< bit12
    std::uint16_t bit13 : 1;    ///< bit13
    std::uint16_t bit14 : 1;    ///< bit14
    std::uint16_t bit15 : 1;    ///< bit15
  }WordBits;

  /// @brief word structure
  typedef union WordStructure
  {
    std::uint16_t m_word {0};   ///< word per se
    WordBits m_bits;            ///< word bit field
  }WordStruct;

  struct TorqueDataAT
  {
    std::int16_t m_actualTorqueForce {0}; ///< current torque/force of the drive
    WordStruct m_torqueLimitStatus {};    ///< torque velocity limit violation status. Take note: for COE only contain one byte!!
  };

  /// @brief Acknowledge Telegram (AT) data.
  typedef struct DataAT
  {
    std::int32_t m_actualPosition {0};      ///< Current position of the drive
    std::int32_t m_actualVelocity {0};      ///< Current velocity of the drive
    TorqueDataAT m_actualTorqueInfo {0};    ///< Current torque/force information of the drive
    std::int8_t m_opModeDisplay {0};        ///< operation mode display
  }DataAT;

  struct TorqueDataMDT
  {
    std::int16_t m_cmdTorqueForce {0};       ///< Current torque/force of the drive
    std::int32_t m_velLimitHigh {0};         ///< torque high velocity limit value
    std::int32_t m_velLimitLow {0};          ///< torque low velocity limit value
  };

  /// @brief Master Data Telegram (MDT) data.
  typedef struct DataMDT
  {
    std::int32_t m_cmdPosition {0};           ///< commanded position of the control
    std::int32_t m_cmdVelocity {0};           ///< commanded velocity of the control
    TorqueDataMDT m_cmdTorqueForceInfo {0};   ///< commanded torque force information of the control
    std::int8_t m_opMode {0};                 ///< operation mode
  }DataMDT;

  /// @brief single bit structure
  typedef union SingleBitStruct
  {
    std::uint8_t m_byte = 0;  ///< a single byte
    bool m_bit;               ///< a single bit
  }SingleBitStruct;

  /// @brief Byte bits data
  struct ByteBits
  {
    std::uint8_t bit0 : 1;
    std::uint8_t bit1 : 1;
    std::uint8_t bit2 : 1;
    std::uint8_t bit3 : 1;
    std::uint8_t bit4 : 1;
    std::uint8_t bit5 : 1;
    std::uint8_t bit6 : 1;
    std::uint8_t bit7 : 1;
  }; ///< Individual bit access

  /// @brief Union that allows access to a byte as a whole or by individual bits
  typedef union ByteBitStruct
  {
    std::uint8_t m_byte = 0; ///< The full 8-bit value
    ByteBits m_bits;         ///< byte bit field
  }ByteBitStruct;

  /// @brief Digital input value bits
  typedef struct DigitalInputValueBits
  {
    SingleBitStruct m_bitRefSwitch{};     ///< bit of reference switch signal
    SingleBitStruct m_bitPosLimSwitch{};  ///< bit of positive limit switch signal
    SingleBitStruct m_bitNegLimSwitch{};  ///< bit of negative limit switch signal
  }DigitalInputValueBits;

  /// @brief Probe input data
  struct ProbeData
  {
    std::int32_t posEdg = 0;            ///< probe 1 positive edge
    std::int32_t negEdg = 0;            ///< probe 1 negative edge
    std::uint16_t probeActive = 0;      ///< if probe 1 activated
    std::uint16_t posLatched = 0;       ///< probe 1 positive edge latched times
    std::uint16_t negLatched = 0;       ///< probe 1 negative edge latched times
    std::int32_t positionDif = 0;       ///< probe 1 positive difference between edges
    std::uint16_t markFailure = 0;      ///< probe 1 marker failure counter
  };

  /// @brief All probe AT data
  struct ProbeAtData
  {
    ProbeData m_probe1;     ///< data of probe 1
    ProbeData m_probe2;     ///< data of probe 2
  };

  /// @brief User PDO scaling entry
  struct UserPDOScalingEntry
  {
    UserPDOMappingID m_pdoMappingID {}; ///< the user PDO id  (question how to distinguish ID 6 for input and output)
    std::uint8_t m_userPDOID {0};       ///< the user PDO ID number
  };

  /// @brief The user PDO scaling information
  struct AxisProfileUserPDOScalingInfo
  {
    std::vector<UserPDOScalingEntry> m_vecUserPDOInputFactors {};   ///< Complete input mapping
    std::vector<UserPDOScalingEntry> m_vecUserPDOOutputFactors {};  ///< Complete output mapping
  };

  /// @brief bit field
  typedef struct DWordBits
  {
    std::uint16_t bit0 : 1;   ///< bit0
    std::uint16_t bit1 : 1;   ///< bit1
    std::uint16_t bit2 : 1;   ///< bit2
    std::uint16_t bit3 : 1;   ///< bit3
    std::uint16_t bit4 : 1;   ///< bit4
    std::uint16_t bit5 : 1;   ///< bit5
    std::uint16_t bit6 : 1;   ///< bit6
    std::uint16_t bit7 : 1;   ///< bit7
    std::uint16_t bit8 : 1;   ///< bit8
    std::uint16_t bit9 : 1;   ///< bit9
    std::uint16_t bit10 : 1;  ///< bit10
    std::uint16_t bit11 : 1;  ///< bit11
    std::uint16_t bit12 : 1;  ///< bit12
    std::uint16_t bit13 : 1;  ///< bit13
    std::uint16_t bit14 : 1;  ///< bit14
    std::uint16_t bit15 : 1;  ///< bit15
    std::uint16_t bit16 : 1;  ///< bit16
    std::uint16_t bit17 : 1;  ///< bit17
    std::uint16_t bit18 : 1;  ///< bit18
    std::uint16_t bit19 : 1;  ///< bit19
    std::uint16_t bit20 : 1;  ///< bit20
    std::uint16_t bit21 : 1;  ///< bit21
    std::uint16_t bit22 : 1;  ///< bit22
    std::uint16_t bit23 : 1;  ///< bit23
    std::uint16_t bit24 : 1;  ///< bit24
    std::uint16_t bit25 : 1;  ///< bit25
    std::uint16_t bit26 : 1;  ///< bit26
    std::uint16_t bit27 : 1;  ///< bit27
    std::uint16_t bit28 : 1;  ///< bit28
    std::uint16_t bit29 : 1;  ///< bit29
    std::uint16_t bit30 : 1;  ///< bit30
    std::uint16_t bit31 : 1;  ///< bit31
  }DWordBits;

  /// @brief double word structure
  typedef union DWordStructure
  {
    std::uint32_t m_word = 0; ///< word per se
    DWordBits m_bits;         ///< word bit field
  }DWordStruct;

  /// @brief Safe motion, safety technology input data structure.
  typedef struct SafeMotionDataPackage
  {
    std::uint32_t m_safetySelection;  ///< Carrying SMO: Safety function selection data package.
    std::uint32_t m_safetyStatus;     ///< Carrying SMO: Safety function status data package.
  } SafeMotionDataPackage;

  /// @brief The validator for the configuration of signal control/status word.
  /// @param[in] param The source parameter which provides the signal (IDN/CoE Object ID in raw data).
  /// @param[in] bit   The signal bit of the source parameter.
  using SignalWordValidator = std::function<bool(const std::uint32_t& param, const std::uint16_t& bit)>;

  /// @brief Pre-defined signal status word function and source parameter/signal.
  struct SignalFunction
  {
    FunctionSupport function {FunctionSupport::MAX_VALUE};    ///< The function supported in this signal
    SignalWordValidator validator {};                         ///< The validator of this function
    bool supported {false};                                   ///< Indicates the function is supported or not.
    bool statusSupported {false};                             ///< Indicates the function is supported in the signal status word.
    bool controlSupported {false};                            ///< Indicates the function is supported in the signal control word.
    std::uint8_t statusBitOffset {0};                         ///< The bit offset in the signal status word.
    std::uint8_t controlBitOffset {0};                        ///< The bit offset in the signal control word.
  };

#pragma pack(push, 1)
  /// @brief Acknowledge Telegram (AT) container. Used to receive data from the device.
  /// TODO: AT is SEROCS specific, renaming to InputTelegram.
  typedef struct AcknowledgeTelegram
  {
    WordStruct m_statusWord {0};                              ///< status word
    DataAT m_atData;                                          ///< data of the telegram
    DWordStructure m_diagMsg;                                 ///< diagnostic message
    DigitalInputValueBits m_digitalInput;                     ///< digital input value bits
    ProbeAtData m_probe;                                      ///< probe at data.
    SafeMotionDataPackage m_smoData;                          ///< Safe motion, safety technology related input data.
    UserPDOData m_userPDOInputs[USER_PDO_COUNT_LIMIT] {0};    ///< user PDO input data array
    std::uint16_t m_signalStatusword {0};                     ///< signal status word
    ByteBitStruct m_ioInReference {0};                        ///< io reference state
    SingleBitStruct m_bitStatusHomed {0};                     ///< bit status of homed

    /// @brief Reset all feedback data, called in booting.
    void reset()
    {
      m_statusWord.m_word = 0;
      m_atData = {0, 0, TorqueDataAT(), 0};
      m_diagMsg.m_word = UINT32_MAX; // max value indicates not mapped
      m_digitalInput = {{0}, {0}, {0}};
      m_probe.m_probe1 = {0, 0, 0, 0, 0, 0, 0};
      m_probe.m_probe2 = {0, 0, 0, 0, 0, 0, 0};
      m_smoData = { 0, 0 };
      m_userPDOInputs[0] = {0};
      m_userPDOInputs[1] = {0};
      m_userPDOInputs[2] = {0};
      m_userPDOInputs[3] = {0};
      m_userPDOInputs[4] = {0};
      m_userPDOInputs[5] = {0};
      m_signalStatusword = {0};
      m_ioInReference.m_byte = 0;
      m_bitStatusHomed = {0};
    }
  }AcknowledgeTelegram;

  /// @brief The IO MDT data
  struct IoEncoderOutputs
  {
    ByteBitStruct m_setReference {};                         ///< trigger the set reference
    std::uint32_t m_referenceOffset {};                      ///< the reference offset
  };

  /// @brief Master Data Telegram (MDT) container. Used to send data to the device.
  /// TODO: MDT is SEROCS specific, renaming to OutputTelegram.
  typedef struct MasterDataTelegram
  {
    WordStruct  m_controlWord {0};                            ///< control word
    DataMDT     m_mdtData;                                    ///< data of the telegram
    UserPDOData m_userPDOOutputs[USER_PDO_COUNT_LIMIT] {0};   ///< user PDO output data array
    IoEncoderOutputs  m_ioEncoderData;                        ///< the IO MDT data
    WordStruct m_signalControlword {0};                       ///< signal control word
  }MasterDataTelegram;

#pragma pack(pop)

  // ---------------------------------------------------------------------------
  // Declaration of helper methods
  // ---------------------------------------------------------------------------

  /// @brief Supported Drive Profiles as String
  const struct DriveProfileTypeStrings
  {
    const std::string UNKNOWN = "Unknown";   ///< Unknown drive profile
    const std::string SERCOS  = "Sercos";    ///< Drive profile SERCOS
    const std::string CANOPEN = "CANopen";   ///< Drive profile CANopen
  } DriveProfileTypeStrings;                 ///< string


  /// @brief Supported Network Technologies as String
  const struct NetworkTechnologyStrings
  {
    const std::string UNKNOWN = "Unknown";     ///< Unknown network technology
    const std::string ETHERCAT = "EtherCAT";   ///< EtherCAT
  } NetworkTechnologyStrings;                  ///< string

  /// @brief The user PDO ID string
  const struct AxisProfileUserPDOIDString
  {
    const std::string USER_PDO_UNDEFINED = "user-pdo-undefined";    ///< undefined user PDO item string
    const std::string USER_PDO_INPUT_1 = "user-pdo-input1";         ///< the 1th user PDO input item string
    const std::string USER_PDO_INPUT_2 = "user-pdo-input2";         ///< the 2th user PDO input item string
    const std::string USER_PDO_INPUT_3 = "user-pdo-input3";         ///< the 3th user PDO input item string
    const std::string USER_PDO_INPUT_4 = "user-pdo-input4";         ///< the 4th user PDO input item string
    const std::string USER_PDO_INPUT_5 = "user-pdo-input5";         ///< the 5th user PDO input item string
    const std::string USER_PDO_INPUT_6 = "user-pdo-input6";         ///< the 6th user PDO input item string
    const std::string USER_PDO_OUTPUT_1 = "user-pdo-output1";       ///< the 1th user PDO output item string
    const std::string USER_PDO_OUTPUT_2 = "user-pdo-output2";       ///< the 2th user PDO output item string
    const std::string USER_PDO_OUTPUT_3 = "user-pdo-output3";       ///< the 3th user PDO output item string
    const std::string USER_PDO_OUTPUT_4 = "user-pdo-output4";       ///< the 4th user PDO output item string
    const std::string USER_PDO_OUTPUT_5 = "user-pdo-output5";       ///< the 5th user PDO output item string
    const std::string USER_PDO_OUTPUT_6 = "user-pdo-output6";       ///< the 6th user PDO output item string
  } AprUserPDOIDStrings;

  /// @brief Convert a string to the associated drive profile type.
  /// @param[in]  typeString   The profile type string
  /// @return The converted drive profile type.
  inline DriveProfileType stringToDriveProfileType(const char* typeString)
  {
    if (!typeString)
    {
      return DriveProfileType::UNDEFINED;
    }

    std::string buffer(typeString);
    if (buffer.compare(DriveProfileTypeStrings.SERCOS) == 0)  { return DriveProfileType::SERCOS; }
    if (buffer.compare(DriveProfileTypeStrings.CANOPEN) == 0) { return DriveProfileType::CANOPEN; }
    return DriveProfileType::UNDEFINED;
  }

  /// @brief Convert a string to the associated network technology.
  /// @param[in]  technologyString   The network technology string
  /// @return The converted network technology.
  inline NetworkTechnology stringToNetworkTechnology(const char* technologyString)
  {
    if (!technologyString)
    {
      return NetworkTechnology::UNDEFINED;
    }

    std::string buffer(technologyString);
    if (buffer.compare(NetworkTechnologyStrings.ETHERCAT) == 0) { return NetworkTechnology::ETHERCAT; }

    return NetworkTechnology::UNDEFINED;
  }

  /// @brief Convert a string to user PDO ID
  /// @param[in] idString The id string need convert.
  /// @return The converted user PDO mapping ID
  inline UserPDOMappingID stringToAxisProfileUserPDOID(const char* idString)
  {
    std::string buffer(idString);
    if (buffer.compare(AprUserPDOIDStrings.USER_PDO_INPUT_1) == 0)
    {
      return UserPDOMappingID::USER_PDO_INPUT_1;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_INPUT_2) == 0)
    {
      return UserPDOMappingID::USER_PDO_INPUT_2;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_INPUT_3) == 0)
    {
      return UserPDOMappingID::USER_PDO_INPUT_3;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_INPUT_4) == 0)
    {
      return UserPDOMappingID::USER_PDO_INPUT_4;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_INPUT_5) == 0)
    {
      return UserPDOMappingID::USER_PDO_INPUT_5;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_INPUT_6) == 0)
    {
      return UserPDOMappingID::USER_PDO_INPUT_6;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_OUTPUT_1) == 0)
    {
      return UserPDOMappingID::USER_PDO_OUTPUT_1;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_OUTPUT_2) == 0)
    {
      return UserPDOMappingID::USER_PDO_OUTPUT_2;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_OUTPUT_3) == 0)
    {
      return UserPDOMappingID::USER_PDO_OUTPUT_3;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_OUTPUT_4) == 0)
    {
      return UserPDOMappingID::USER_PDO_OUTPUT_4;
    }
    else if (buffer.compare(AprUserPDOIDStrings.USER_PDO_OUTPUT_5) == 0)
    {
      return UserPDOMappingID::USER_PDO_OUTPUT_5;
    }
    else
    {
      return UserPDOMappingID::USER_PDO_OUTPUT_6;
    }
  }

  /// @brief The method convert userPDO ID to string.
  /// @param[in] pdoType The user PDO ID enum.
  /// @return The converted user PDO string
  inline const char* AxisProfileUserPDOToString(const UserPDOMappingID pdoType)
  {
    switch (pdoType)
    {
      case UserPDOMappingID::USER_PDO_INPUT_1:    return AprUserPDOIDStrings.USER_PDO_INPUT_1.c_str(); break;
      case UserPDOMappingID::USER_PDO_INPUT_2:    return AprUserPDOIDStrings.USER_PDO_INPUT_2.c_str(); break;
      case UserPDOMappingID::USER_PDO_INPUT_3:    return AprUserPDOIDStrings.USER_PDO_INPUT_3.c_str(); break;
      case UserPDOMappingID::USER_PDO_INPUT_4:    return AprUserPDOIDStrings.USER_PDO_INPUT_4.c_str(); break;
      case UserPDOMappingID::USER_PDO_INPUT_5:    return AprUserPDOIDStrings.USER_PDO_INPUT_5.c_str(); break;
      case UserPDOMappingID::USER_PDO_INPUT_6:    return AprUserPDOIDStrings.USER_PDO_INPUT_6.c_str(); break;
      case UserPDOMappingID::USER_PDO_OUTPUT_1:   return AprUserPDOIDStrings.USER_PDO_OUTPUT_1.c_str(); break;
      case UserPDOMappingID::USER_PDO_OUTPUT_2:   return AprUserPDOIDStrings.USER_PDO_OUTPUT_2.c_str(); break;
      case UserPDOMappingID::USER_PDO_OUTPUT_3:   return AprUserPDOIDStrings.USER_PDO_OUTPUT_3.c_str(); break;
      case UserPDOMappingID::USER_PDO_OUTPUT_4:   return AprUserPDOIDStrings.USER_PDO_OUTPUT_4.c_str(); break;
      case UserPDOMappingID::USER_PDO_OUTPUT_5:   return AprUserPDOIDStrings.USER_PDO_OUTPUT_5.c_str(); break;
      case UserPDOMappingID::USER_PDO_OUTPUT_6:   return AprUserPDOIDStrings.USER_PDO_OUTPUT_6.c_str(); break;
      default:                               return AprUserPDOIDStrings.USER_PDO_UNDEFINED.c_str(); break;
    }
  }

  /// @brief Convert a drive profile type to the associated string.
  /// @param[in]  profile   The drive profile
  /// @return The converted drive profile string.
  inline const char* driveProfileTypeToString(const DriveProfileType profile)
  {
    switch (profile)
    {
      case DriveProfileType::SERCOS:   return DriveProfileTypeStrings.SERCOS.c_str(); break;
      case DriveProfileType::CANOPEN:  return DriveProfileTypeStrings.CANOPEN.c_str(); break;
      default:                         return DriveProfileTypeStrings.UNKNOWN.c_str(); break;
    }
  }

  /// @brief Convert a network technology to the associated string.
  /// @param[in]  technology   The network technology
  /// @return The converted axis profile type string.
  inline const char* networkTechnologyToString(const NetworkTechnology technology)
  {
    switch (technology)
    {
      case NetworkTechnology::ETHERCAT:   return NetworkTechnologyStrings.ETHERCAT.c_str(); break;
      default:                            return NetworkTechnologyStrings.UNKNOWN.c_str(); break;
    }
  }

  /// @brief Convert a callable type to the associated string.
  /// @param[in]  type   The type of callable
  /// @return The converted callable type string.
  inline const char* callableTypeString(CallableType type)
  {
    switch (type)
    {
      case CallableType::ADAPTION_CALLABLE:       return "adaption";
      case CallableType::DISTRIBUTION_CALLABLE:   return "distribution";
      default:                                    return "unknownCallable";
    }
  }

  /// @brief Convert axisprofile command request to string
  /// @param[in]   cmd    new command request
  /// @return The converted command request in string
  inline const char* cmdReqToString(const CmdReq cmd)
  {
    switch (cmd)
    {
      case CmdReq::REQ_RESET:     return "Reset";
      case CmdReq::REQ_START:     return "Start";
      case CmdReq::REQ_ABORT:     return "Abort";
      default:                    return "UnknowReq";
    }
  }

  /// @brief Convert device series type to string
  /// @param[in]   series    device series
  /// @return The converted device series string
  inline const char* deviceSeriesToString(const DeviceSeries series)
  {
    switch (series)
    {
      case DeviceSeries::UNDEFINED:           return "undefined";
      case DeviceSeries::INDRADRIVE:          return "indraDrive";
      case DeviceSeries::CTRLXDRIVE:          return "ctrlXDrive";
      case DeviceSeries::CTRLXDRIVE_XC20:     return "xc20";
      case DeviceSeries::INNOVANCE:           return "innovance";
      case DeviceSeries::MOON:                return "moon";
      default:                                return "undefined";
    }
  }
}
