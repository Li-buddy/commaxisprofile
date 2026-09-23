#pragma once
/// @file
///
/// This header file is part of the axisprofile library and define the contains defines relevant
/// for the CiA402 specification.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/profile_defs.h"
#include "profiles/value_mapping/mapping_handler_defs.h"
#include <map>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  
  constexpr ObjectDirEntry SOFTWARE_VERSION               = {0x100A, 0};     ///< object of manufacturer software version
  constexpr ObjectDirEntry VENDOR_ID                      = {0x1018, 1};     ///< object of vendor id
  constexpr ObjectDirEntry REF_DISTANCE_INDRA_DRIVE       = {0x2034, 1};     ///< object of reference distance for indra drive
  constexpr ObjectDirEntry STANDSTILL_STATUS              = {0x2212, 4};     ///< Status "n_feedback = 0"
  constexpr ObjectDirEntry ENCODER_2_PROPERTY             = {0x24A0, 1};     ///< object of Property of Encoder 2 of ctrlX drive
  constexpr ObjectDirEntry ENCODER_1_PROPERTY             = {0x24A2, 1};     ///< object of Property of Encoder 1 of ctrlX drive
  constexpr ObjectDirEntry CANCEL_REF_POINT               = {0x2500, 1};     ///< object of C1500 command of cancel reference point
  constexpr ObjectDirEntry HOMING_OFFSET_CTRLX_DRIVE      = {0x2505, 2};     ///< object of reference homing offset of ctrlX drive
  constexpr ObjectDirEntry HOMING_FEEDBACK_VALUE_STATUS   = {0x2508, 1};     ///< object of homing status: Feedback value status
  constexpr ObjectDirEntry ENCODER_2_REF_DISTANCE         = {0x2506, 1};     ///< object of reference distance for homing for Encoder 2 of ctrlX drive
  constexpr ObjectDirEntry SET_ABSOLUTE_POSITION_CMD      = {0x2510, 1};     ///< object of command of set absolute position of ctrlX drive
  constexpr ObjectDirEntry SET_ABSOLUTE_POSITION_CONTROL  = {0x2510, 2};     ///< object of control word of set absolute position of ctrlX drive
  constexpr ObjectDirEntry STATUS_WORD_CTRLX_DRIVE        = {0x2802, 0};     ///< object of device status word for ctrlX drive
  constexpr ObjectDirEntry SCALING_TORQUE_SCALING_TYPE    = {0x2906, 1};     ///< object of torque scaling type
  constexpr ObjectDirEntry SCALING_TORQUE_FACTOR          = {0x2906, 2};     ///< object of torque scaling factor
  constexpr ObjectDirEntry SCALING_TORQUE_EXPONENT        = {0x2906, 3};     ///< object of torque scaling exponent
  constexpr ObjectDirEntry CHECKSUM_PARAMETERS            = {0x2B60, 4};     ///< object of checksum parameters of loading parameter values 
  constexpr ObjectDirEntry CHECKSUM_VALUE                 = {0x2B60, 0x0E};  ///< object of checksum of loading parameter values
  constexpr ObjectDirEntry SIGNAL_STATUS_WORD             = {0x2C82, 1};     ///< object of signal status word value
  constexpr ObjectDirEntry SIGNAL_STATUS_WORD_CONFIG_LIST = {0x2C82, 2};     ///< object of signal status word configuration list
  constexpr ObjectDirEntry SIGNAL_STATUS_WORD_ASSIGN_LIST = {0x2C82, 3};     ///< object of signal status word assign list signal
  constexpr ObjectDirEntry STATUS_WORD_INDRA_DRIVE        = {0x3073, 0};     ///< object of device status word for indra drive
  constexpr ObjectDirEntry QUICK_STOP_OPTION              = {0x605A, 0};     ///< object of quick stop option code
  constexpr ObjectDirEntry MODES_OF_OPERATION             = {0x6060, 0};     ///< object of operation mode
  constexpr ObjectDirEntry MODES_OF_OPERATION_DISPLAY     = {0x6061, 0};     ///< object of operation mode display
  constexpr ObjectDirEntry ACTUAL_VELOCITY                = {0x606C, 0};     ///< object of actual velocity
  constexpr ObjectDirEntry SCALING_NOMINAL_TORQUE         = {0x6076, 0};     ///< object of motor rated toque
  constexpr ObjectDirEntry MIN_POS_RANGE_LIMIT            = {0x607B, 1};     ///< object of minimum position range limit
  constexpr ObjectDirEntry MAX_POS_RANGE_LIMIT            = {0x607B, 2};     ///< object of maximum position range limit
  constexpr ObjectDirEntry REF_DISTANCE                   = {0x607C, 0};     ///< object of reference distance for homing
  constexpr ObjectDirEntry SCALING_GEAR_REVOLUTION_IN     = {0x6091, 1};     ///< object of gear revolution of input
  constexpr ObjectDirEntry SCALING_GEAR_REVOLUTION_OUT    = {0x6091, 2};     ///< object of gear revolution of output
  constexpr ObjectDirEntry SCALING_FEED_CONSTANT          = {0x6092, 1};     ///< object of feed constant
  constexpr ObjectDirEntry HOMING_METHOD                  = {0x6098, 0};     ///< object of reference homing method
  constexpr ObjectDirEntry SCALING_SI_UNIT_POSITION       = {0x60A8, 0};     ///< object of SI unit position
  constexpr ObjectDirEntry SCALING_SI_UNIT_VELOCITY       = {0x60A9, 0};     ///< object of SI unit velocity
  constexpr ObjectDirEntry SCALING_SI_UNIT_ACCELERATION   = {0x60AA, 0};     ///< object of SI unit acceleration
  constexpr ObjectDirEntry PROBE_CTRL_WORD                = {0x60B8, 0};     ///< object of probe control word
  constexpr ObjectDirEntry PROBE_STATUS_WORD              = {0x60B9, 0};     ///< object of probe status word
  constexpr ObjectDirEntry PROBE1_POS_DEGE_VALUE          = {0x60BA, 0};     ///< object of probe1 positive edge
  constexpr ObjectDirEntry PROBE1_NEG_DEGE_VALUE          = {0x60BB, 0};     ///< object of probe1 negative edge
  constexpr ObjectDirEntry PROBE2_POS_DEGE_VALUE          = {0x60BC, 0};     ///< object of probe2 positive edge
  constexpr ObjectDirEntry PROBE2_NEG_DEGE_VALUE          = {0x60BD, 0};     ///< object of probe2 negative edge
  constexpr ObjectDirEntry TORQUE_FORCE_OPTIONS_CODE      = {0x60CA, 0};     ///< object of reference torque/force options code 
  constexpr ObjectDirEntry PROBE1_POS_LATCHED_TIMES       = {0x60D5, 0};     ///< object of probe1 positive latched times
  constexpr ObjectDirEntry PROBE1_NEG_LATCHED_TIMES       = {0x60D6, 0};     ///< object of probe1 negative latched times
  constexpr ObjectDirEntry PROBE2_POS_LATCHED_TIMES       = {0x60D7, 0};     ///< object of probe2 positive latched times
  constexpr ObjectDirEntry PROBE2_NEG_LATCHED_TIMES       = {0x60D8, 0};     ///< object of probe2 negative latched times 
  constexpr ObjectDirEntry SUPPORTED_DRIVE_MODE           = {0x6502, 0};     ///< object of reference providing information on the supported drive modes

#ifdef WIN32 // saving test time on windows
  constexpr std::uint16_t CANOPEN_SWITCH_TIMEOUT_THRESHOLD = 10;     ///< max wait times, for test on windows
#else // taken more time on real core
  constexpr std::uint16_t CANOPEN_SWITCH_TIMEOUT_THRESHOLD = 2500;   ///< max wait times, TODO: make this configurable later
#endif
  constexpr std::int8_t CANOPEN_HOMING_MODE             = 6;         ///< homing mode
  constexpr std::int8_t CANOPEN_POSITION_MODE           = 8;         ///< position mode
  constexpr std::int8_t CANOPEN_VELOCITY_MODE           = 9;         ///< velocity mode
  constexpr std::int8_t CANOPEN_FORCE_VEL_LIMIT         = 10;        ///< torque/force with velocity limitation mode

  // ******************** SCALING ******************** //
  constexpr std::uint8_t CANOPEN_POSITION_UNIT_ROTARY      = 0x41U;  ///< position scaling type rotary
  constexpr std::uint8_t CANOPEN_POSITION_UNIT_LINEAR      = 0x01U;  ///< position scaling type linear
  constexpr std::uint8_t CANOPEN_VELOCITY_TIME_UNIT_SECOND = 0x03U;  ///< time unit second
  constexpr std::uint8_t CANOPEN_VELOCITY_TIME_UNIT_MINUTE = 0x47U;  ///< time unit minute
  constexpr std::uint8_t CANOPEN_VELOCITY_UNIT_ROTARY      = 0xB4U;  ///< velocity scaling type rotary
  constexpr std::uint8_t CANOPEN_VELOCITY_UNIT_LINEAR      = 0x01U;  ///< velocity scaling type linear

  constexpr std::uint8_t CANOPEN_ACCELERATION_UNIT_ROTARY  = 0x10U;  ///< acceleration scaling type rotary
  constexpr std::uint8_t CANOPEN_ACCELERATION_UNIT_LINEAR  = 0x01U;  ///< acceleration scaling type linear

  constexpr std::uint16_t CANOPEN_TORQUE_TYPE_METHOD_PCTG   = 0x0000;  ///< torque data scaling type (0x2906:01), Scaling method percentage
  constexpr std::uint16_t CANOPEN_TORQUE_TYPE_METHOD_LINEAR = 0x0001;  ///< torque data scaling type (0x2906:01), Scaling method linear
  constexpr std::uint16_t CANOPEN_TORQUE_TYPE_METHOD_ROT    = 0x0002;  ///< torque data scaling type (0x2906:01), Scaling method rotary
  constexpr std::uint16_t CANOPEN_TORQUE_TYPE_METHOD_MASK   = 0x0007;  ///< torque data scaling type (0x2906:01), Scaling method mask (Bits 2-0)

  constexpr std::uint16_t CANOPEN_TORQUE_TYPE_UNIT_OFFSET   = 0x0004;  ///< torque data scaling type (0x2906:01), Unit for linear scaling (Bit 4) bit offset

  // ******************** OBJECT OFFSET ******************** //
  constexpr std::uint16_t OBJ_DIC_OFFSET = 0x800;  ///< object offset,for situations where a single address corresponds to multiple axes

  /// @brief Container of input all mapping
  static const std::map<InputMappingValueID, MappingCfgInfo> INPUT_ALL_MAPPING
  {
    {InputMappingValueID::STATUSWORD,        {MappingGroup::BASE,         true}},  {InputMappingValueID::POSITION,             {MappingGroup::BASE,         false}},
    {InputMappingValueID::DIAGMSG,           {MappingGroup::BASE,         false}}, {InputMappingValueID::VELOCITY,             {MappingGroup::VELOCITY,     false}},
    {InputMappingValueID::TORQUE_FORCE,      {MappingGroup::TORQUE_FORCE, false}}, {InputMappingValueID::TORQUE_LIMIT_STATUS,  {MappingGroup::TORQUE_FORCE, false}},
    {InputMappingValueID::OP_MODE_DISPLAY,   {MappingGroup::BASE,         false}},
    {InputMappingValueID::PROBE1_POSEDG,     {MappingGroup::PROBE,        false}}, {InputMappingValueID::PROBE1_NEGEDG,        {MappingGroup::PROBE,        false}},
    {InputMappingValueID::PROBE1_POSLAT,     {MappingGroup::PROBE,        false}}, {InputMappingValueID::PROBE1_NEGLAT,        {MappingGroup::PROBE,        false}},
    {InputMappingValueID::PROBE2_POSEDG,     {MappingGroup::PROBE,        false}}, {InputMappingValueID::PROBE2_NEGEDG,        {MappingGroup::PROBE,        false}},
    {InputMappingValueID::PROBE2_POSLAT,     {MappingGroup::PROBE,        false}}, {InputMappingValueID::PROBE2_NEGLAT,        {MappingGroup::PROBE,        false}},
    {InputMappingValueID::SWITCH_REF,        {MappingGroup::HOMING,       false}}, {InputMappingValueID::SWITCH_POS_LIM,       {MappingGroup::HOMING,       false}}, 
    {InputMappingValueID::SWITCH_NEG_LIM,    {MappingGroup::HOMING,       false}}, {InputMappingValueID::SMO_SAFETY_SELECTION, {MappingGroup::SAFE_MOTION,  false}},
    {InputMappingValueID::SMO_SAFETY_STATUS, {MappingGroup::SAFE_MOTION,  false}},
    {InputMappingValueID::USER_PDO_INPUT_1,  {MappingGroup::USER_PDO,     false}}, {InputMappingValueID::USER_PDO_INPUT_2,     {MappingGroup::USER_PDO,     false}},
    {InputMappingValueID::USER_PDO_INPUT_3,  {MappingGroup::USER_PDO,     false}}, {InputMappingValueID::USER_PDO_INPUT_4,     {MappingGroup::USER_PDO,     false}},
    {InputMappingValueID::USER_PDO_INPUT_5,  {MappingGroup::USER_PDO,     false}}, {InputMappingValueID::USER_PDO_INPUT_6,     {MappingGroup::USER_PDO,     false}},
    {InputMappingValueID::SIGNAL_STATUS_WORD, {MappingGroup::SIGNAL_WORD, false}}, {InputMappingValueID::HOMED_STATUS,         {MappingGroup::HOMING,       false}}
  };

  /// @brief Container of output all mapping
  static const std::map<OutputMappingValueID, MappingCfgInfo> OUTPUT_ALL_MAPPING
  {
    {OutputMappingValueID::CONTROLWORD,           {MappingGroup::BASE,         true}},  {OutputMappingValueID::POSITION,             {MappingGroup::BASE,         false}},
    {OutputMappingValueID::VELOCITY,              {MappingGroup::VELOCITY,     false}}, {OutputMappingValueID::OP_MODE,              {MappingGroup::BASE,         false}},
    {OutputMappingValueID::TORQUE_FORCE,          {MappingGroup::TORQUE_FORCE, false}},
    {OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH, {MappingGroup::TORQUE_FORCE, false}}, {OutputMappingValueID::TORQUE_VEL_LIMIT_LOW, {MappingGroup::TORQUE_FORCE, false}},
    {OutputMappingValueID::USER_PDO_OUTPUT_1,     {MappingGroup::USER_PDO,     false}}, {OutputMappingValueID::USER_PDO_OUTPUT_2,    {MappingGroup::USER_PDO,     false}},
    {OutputMappingValueID::USER_PDO_OUTPUT_3,     {MappingGroup::USER_PDO,     false}}, {OutputMappingValueID::USER_PDO_OUTPUT_4,    {MappingGroup::USER_PDO,     false}},
    {OutputMappingValueID::USER_PDO_OUTPUT_5,     {MappingGroup::USER_PDO,     false}}, {OutputMappingValueID::USER_PDO_OUTPUT_6,    {MappingGroup::USER_PDO,     false}}
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief Struct that stores information for accessing parameter
  struct CanopenParamStruct
  {
    ReadWriteType m_readWriteType = ReadWriteType::READ;  ///< read or write
    ObjectDirEntry m_objDirEntry;       ///< object directory entry
    void* m_ptrData;                    ///< pointer to the data
    size_t dataSize;                    ///< data size
    /// @brief constructor
    CanopenParamStruct(std::uint16_t objectIndex, std::uint8_t subIndex, void* ptrData, size_t dataSize)
      : m_objDirEntry(objectIndex, subIndex), m_ptrData(ptrData), dataSize(dataSize)
    {};

    /// @brief constructor with read write type
    CanopenParamStruct(ReadWriteType type, std::uint16_t objectIndex, std::uint8_t subIndex, void* ptrData, size_t dataSize)
      : m_readWriteType(type), m_objDirEntry(objectIndex, subIndex), m_ptrData(ptrData), dataSize(dataSize)
    {};

    /// @brief constructor with read write type
    CanopenParamStruct(ObjectDirEntry objDir, void* ptrData, size_t dataSize)
      : m_objDirEntry(objDir), m_ptrData(ptrData), dataSize(dataSize)
    {
    };

    /// @brief constructor with read write type
    CanopenParamStruct(ReadWriteType type, ObjectDirEntry objDir, void* ptrData, size_t dataSize)
      : m_readWriteType(type), m_objDirEntry(objDir), m_ptrData(ptrData), dataSize(dataSize)
    {};
  };

  /// @brief This structure represents the complete Canopen controlword
  struct ControlWord
  {
  };

  /// @brief This structure represents the complete Canopen statusword 
  struct StatusWord
  {   
    bool m_followingError = false;            ///< Bit 13    
    bool m_setPointAck = false;               ///< Bit 12 
    bool m_internalLimitActive = false;       ///< Bit 11  
    bool m_targetReached = false;             ///< Bit 10    
    bool m_remote = false;                    ///< Bit 9    
    // bool m_manufacturerSpecific = false;   ///< Bits 8 
    bool m_warning = false;                   ///< Bit 7    
    bool m_switchOnDisabled = false;          ///< Bit 6     
    bool m_notQuickStop = false;              ///< Bit 5      
    bool m_voltageEnabled = false;            ///< Bit 4      
    bool m_fault = false;                     ///< Bit 3      
    bool m_operationEnabled = false;          ///< Bit 2      
    bool m_switchedOn = false;                ///< Bit 1      
    bool m_readyToSwichOn = false;            ///< Bit 0             
  };   
  
  /// @brief This structure contain operation mode available information for Canopen
  typedef struct OpModeAvailInfoCanopen : public OpModeAvailInfo
  {
    /// @brief Constructor 
    OpModeAvailInfoCanopen(bool positionAvail, bool velocityAvail, bool torqueForceAvail) : OpModeAvailInfo(positionAvail, velocityAvail, torqueForceAvail)
    {};

    /// @brief Constructor using a single value
    explicit OpModeAvailInfoCanopen(bool avail) : OpModeAvailInfo(avail)
    {};

    /// @brief Clear Canopen operation mode available information
    void clearOpModeAvailInfoCanopen()
    {
      this->clearOpModeAvailInfo();
    }

  }OpModeAvailInfoCanopen;

  /// @brief Converts to the CANopen operation mode number.
  /// @param[in]  opmode  The operation mode defined in axis profile.
  /// @param[out]   mode  The CANopen operation mode number.
  /// @return     Return a boolean value indicating success or failure.
  inline bool convertToCanopenOpMode(AxisProfileOperationMode opmode, std::uint8_t& mode)
  {
    bool res = true;
    switch (opmode)
    {
      case AxisProfileOperationMode::POSITIONING:
        mode = CANOPEN_POSITION_MODE; break;
      case AxisProfileOperationMode::VELOCITY:
        mode = CANOPEN_VELOCITY_MODE; break;
      case AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT:
        mode = CANOPEN_FORCE_VEL_LIMIT; break;
      case AxisProfileOperationMode::HOMING:
        mode = CANOPEN_HOMING_MODE; break;

      default:
        {
          mode = 0;
          res = false;
        }
        break;
    }
    return res;
  };

  /// @brief Converts to the operation mode defined in axis profile.
  /// @param[in] mode  The CANopen operation mode number.
  /// @return     Return the operation mode defined in axis profile.
  inline AxisProfileOperationMode convertFromCanopenOpMode(std::uint8_t mode)
  {
    switch (mode)
    {
      case CANOPEN_POSITION_MODE: return AxisProfileOperationMode::POSITIONING;
      case CANOPEN_VELOCITY_MODE: return AxisProfileOperationMode::VELOCITY;
      case CANOPEN_FORCE_VEL_LIMIT: return AxisProfileOperationMode::TORQUE_FORCE_VEL_LIMIT;
      case CANOPEN_HOMING_MODE: return AxisProfileOperationMode::HOMING;
      default: return AxisProfileOperationMode::UNDEFINED;
    }
    return AxisProfileOperationMode::UNDEFINED;
  };

  /// @brief This method calculates the object index with the given base index and drive number.
  /// @param[in]  baseIndex     Index of CoE object of driver number 0
  /// @param[in]  driveNum      Drive number of the profile
  /// @return     Index of CoE object after offset
  inline std::uint16_t getObjIndex(std::uint16_t baseIndex, std::uint8_t driveNum)
  {
    return baseIndex + (driveNum * OBJ_DIC_OFFSET);
  };

  /// @brief This method calculates the object index with the given base object and drive number.
  /// @param[in]  baseObj       Base object directory entry of driver number 0
  /// @param[in]  driveNum      Drive number of the profile
  /// @return     Index of CoE object after offset
  inline ObjectDirEntry getObjDir(const ObjectDirEntry& baseObj, std::uint8_t driveNum)
  {
    ObjectDirEntry obj = baseObj;
    obj.index += driveNum * OBJ_DIC_OFFSET;
    return obj;
  };

  /// @brief Enumeration of the FSA states defined in CiA402
  enum class FsaState
  {
    NOT_READY_TO_SWITCH_ON = 0, ///< Not ready to switch on
    SWITCH_ON_DISABLED,         ///< Switch on disabled
    READY_TO_SWITCH_ON,         ///< Ready to switch on
    SWITCHED_ON,                ///< Switched on
    OPERATION_ENABLED,          ///< Operation enabled
    QUICK_STOP_ACTIVE,          ///< Quick stop active
    FAULT,                      ///< Fault
    FAULT_REACTION_ACTIVE       ///< Fault reaction active
  };

  /// @brief The method converts the FSA state to string.
  /// @param[in]  state     the state to be converted
  /// @return     The string representation of the FSA state.
  inline const char* FasStateToString(FsaState state)
  {
    switch (state)
    {
      case FsaState::NOT_READY_TO_SWITCH_ON:    return "NotReadToSwitchOn";
      case FsaState::SWITCH_ON_DISABLED:        return "SwitchOnDisable";
      case FsaState::READY_TO_SWITCH_ON:        return "ReadToSwitchOn";
      case FsaState::SWITCHED_ON:               return "SwitchedOn";
      case FsaState::OPERATION_ENABLED:         return "OperationEnabled";
      case FsaState::QUICK_STOP_ACTIVE:         return "QuickStopActive";
      case FsaState::FAULT:                     return "Fault";
      case FsaState::FAULT_REACTION_ACTIVE:     return "FaultReactionActive";
      default:                                  return "UnknownState";
    };
  }

  /// @brief This method extracts the FSA state from the given status word.
  ///
  /// @note The inline methods is optimized for realtime invoking
  /// 
  /// @param[in] statusWord     Status word of CiA402 drive
  /// @return FSA state of device
  inline FsaState getFsaStateFromStatusWord(std::uint16_t statusWord) noexcept
  {
    const std::uint16_t s = statusWord & 0x006F; // bit6~5, bit3~0

    // bit 3: Fault
    if (s & 0x0008)
    {
      // bit2~0 = 7 means Fault reaction active, otherwise Fault
      return ((s & 0x0007) == 0x0007) ? FsaState::FAULT_REACTION_ACTIVE : FsaState::FAULT;
    }

    // bit 6: Switch on disabled
    if (s & 0x0040)
    {
      return FsaState::SWITCH_ON_DISABLED;
    }

    // bit 2: Operation enabled
    if (s & 0x0004)
    {
      // bit 5: Quick stop
      return (s & 0x0020) ? FsaState::OPERATION_ENABLED : FsaState::QUICK_STOP_ACTIVE;
    }

    // bit 1: Switched on
    if (s & 0x0002)
    {
      return FsaState::SWITCHED_ON;
    }

    // bit 0: Ready to switch on
    if (s & 0x0001)
    {
      return FsaState::READY_TO_SWITCH_ON;
    }

    return FsaState::NOT_READY_TO_SWITCH_ON;
  }
}
