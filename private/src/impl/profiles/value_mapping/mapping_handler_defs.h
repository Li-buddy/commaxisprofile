#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mapping_handler_defs.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This file is part of the axisprofile library and contains defines relevant
//! for the profile mapping handler.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "fieldbuses/fieldbus_defs.h"
#include <cstdint>
#include <string>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {

    // ---------------------------------------------------------------------------
    // Declaration of macros
    // ---------------------------------------------------------------------------


    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------
    
    //! @brief Id´s for the input mapping values
    enum class InputMappingValueID
    {
      UNKNOWN = 0,                ///< unknown mapping value
      STATUSWORD,                 ///< id for the statusword
      POSITION,                   ///< id for the input position value (actual position)
      DIAGMSG,                    ///< id for the diagnostic code
      VELOCITY,                   ///< id for the input velocity value (actual velocity)
      SWITCH_REF,                 ///< switch signal for reference
      SWITCH_POS_LIM,             ///< switch signal for positive limit
      SWITCH_NEG_LIM,             ///< switch signal for negative limit
      PROBE1_POSEDG,              ///< probe 1 positive edge
      PROBE1_NEGEDG,              ///< probe 1 negative edge
      PROBE1_ACTIVE,              ///< if probe 1 activated
      PROBE1_POSLAT,              ///< probe 1 positive edge latched times
      PROBE1_NEGLAT,              ///< probe 1 negative edge latched times
      PROBE1_POSDIF,              ///< probe 1 position value different
      PROBE1_MAKFAL,              ///< counter of marker failure
      PROBE2_POSEDG,              ///< probe 2 positive edge
      PROBE2_NEGEDG,              ///< probe 2 negative edge
      PROBE2_ACTIVE,              ///< if probe 2 activated
      PROBE2_POSLAT,              ///< probe 2 positive edge latched times
      PROBE2_NEGLAT,              ///< probe 2 negative edge latched times
      PROBE2_POSDIF,              ///< probe 2 position value different
      PROBE2_MAKFAL,              ///< counter of marker failure
      TORQUE_FORCE,               ///< id for the input torque/force value(actual torque/force)
      TORQUE_LIMIT_STATUS,        ///< torque limit status
      OP_MODE_DISPLAY,            ///< modes of operation display
      SMO_SAFETY_SELECTION,       ///< Safety technology. SMO: Safety function selection
      SMO_SAFETY_STATUS,          ///< Safety technology. SMO: Safety function status
      USER_PDO_INPUT_1,           ///< user PDO input 1
      USER_PDO_INPUT_2,           ///< user PDO input 2
      USER_PDO_INPUT_3,           ///< user PDO input 3
      USER_PDO_INPUT_4,           ///< user PDO input 4
      USER_PDO_INPUT_5,           ///< user PDO input 5
      USER_PDO_INPUT_6,           ///< user PDO input 6
      SIGNAL_STATUS_WORD,         ///< signal status word
      IO_IN_REFERENCE,            ///< device whether in reference or not offer information for checkHomed
      HOMED_STATUS,               ///< device whether in homed or not, offer information for checkHomed
    };

    //! @brief Id´s for the output mapping values
    enum class OutputMappingValueID
    {
      UNKNOWN = 0,          ///< unknown mapping value
      CONTROLWORD,          ///< id for the controlword
      POSITION,             ///< id for the output position value (commanded position)
      VELOCITY,             ///< id for the output velocity value (commanded velocity)
      TORQUE_FORCE,         ///< id for the output torque/force value (commanded torque/force)
      TORQUE_VEL_LIMIT_HIGH,///< high velocity limitation for torque mode
      TORQUE_VEL_LIMIT_LOW, ///< low velocity limitation for torque mode
      OP_MODE,              ///< id for the operation mode value (position mode or velocity mode)
      USER_PDO_OUTPUT_1,    ///< user PDO output 1
      USER_PDO_OUTPUT_2,    ///< user PDO output 2
      USER_PDO_OUTPUT_3,    ///< user PDO output 3
      USER_PDO_OUTPUT_4,    ///< user PDO output 4
      USER_PDO_OUTPUT_5,    ///< user PDO output 5
      USER_PDO_OUTPUT_6,    ///< user PDO output 6
      IO_SET_REFERENCE,      ///< same interface as homing and set-pos-abs
      IO_REFERENCE_OFFSET,   ///< the offset value(related to scaling)
      SIGNAL_CONTROL_WORD,   ///< signal CONTROL word
    };

    /// @brief Group of input/output mapping items
    enum class MappingGroup
    {
      UNKNOWN = -1,              ///< unknown mapping type
      BASE,                      ///< base type
      VELOCITY,                  ///< velocity type
      HOMING,                    ///< homing type
      PROBE,                     ///< probe type
      TORQUE_FORCE,              ///< torque/force type
      SAFE_MOTION,               ///< Safe motion, safety technology.
      USER_PDO,                  ///< user PDO type
      SIGNAL_WORD,               ///< type for signal control/status word
      IO_DEVICE,                 ///< I/O device related mapping group
    };
  
    /// @brief Mask number for mapping group
    enum MappingGroupMask
    {
      BASE         = 1 << 0,    ///< base type 
      VELOCITY     = 1 << 1,    ///< velocity type
      HOMING       = 1 << 2,    ///< homing type
      PROBE        = 1 << 3,    ///< probe type
      TORQUE_FORCE = 1 << 4,    ///< torque/force type
      SAFE_MOTION  = 1 << 5,    ///< safety
      USER_PDO     = 1 << 6,    ///< user PDO type
      SIGNAL_WORD  = 1 << 7,    ///< type for signal control/status word
      IO_DEVICE    = 1 << 8,    ///< I/O device related mapping group
      ALL_GROUP    = 255,       ///< all group(11111111)
    };

    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------


    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------


    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------
    /// @brief Struct to store mapping information
    struct MappingCfgInfo
    {
      MappingGroup   m_mappingGroup;    ///< mapping group information
      bool           m_bIsMandatory;    ///< whether the item is required
    };

    //! @brief struct to hold the realtime information for the input mapping in the datalayer realtime memory
    struct InputMappingInfoRT : public RtMemLocation
    {
      InputMappingValueID m_id;   //!< id of the mapped value
    };

    //! @brief struct to hold the realtime information for the output mapping in the datalayer realtime memory
    struct OutputMappingInfoRT : public RtMemLocation
    {
      OutputMappingValueID m_id;  //!< id of the mapped value
    };

    //! @brief struct to store the mapping information of a single input value
    struct InputMappingEntry
    {
      InputMappingValueID   m_id {InputMappingValueID::UNKNOWN};  ///< id of the mapped value
      std::string           m_uri {};                             ///< uri of the mapped value
      MappingGroup          m_group {MappingGroup::BASE};         ///< type of mapped value
      bool                  m_bRequired {false};                  ///< is current mapping item is mandatory

      InputMappingEntry(InputMappingValueID id, const std::string& uri, MappingGroup group, bool required)
        :m_id(id), m_uri(uri), m_group(group), m_bRequired(required)
      {
      }
    };

    //! @brief struct to store the mapping information of a single output value
    struct OutputMappingEntry
    {
      OutputMappingValueID   m_id {OutputMappingValueID::UNKNOWN};  ///< id of the mapped value
      std::string            m_uri {};                              ///< uri of the mapped value
      MappingGroup           m_group {MappingGroup::BASE};          ///< type of mapped value
      bool                   m_bRequired {false};                   ///< is current mapping item is mandatory

      OutputMappingEntry(OutputMappingValueID id, const std::string& uri, MappingGroup group, bool required)
        :m_id(id), m_uri(uri), m_group(group), m_bRequired(required)
      {
      }
    };

    //! @brief Supported input mapping value IDs as String
    const struct InputMappingValueIDStrings
    {
      const std::string UNKNOWN          = "unknown";                                ///< Unknown input mapping value ID
      const std::string STATUSWORD       = "statusword";                             ///< Input mapping value ID for statusword
      const std::string POSITION         = "position";                               ///< Input mapping value ID for position
      const std::string DIAGMSG          = "diagcode";                               ///< Input mapping value ID for diagnostic message number
      const std::string VELOCITY         = "velocity";                               ///< Input mapping value ID for velocity
      const std::string SWITCH_REF       = "switch-ref";                             ///< Input mapping value ID for reference switch signal
      const std::string SWITCH_POS_LIM   = "switch-poslim";                          ///< Input mapping value ID for positive limit switch signal
      const std::string SWITCH_NEG_LIM   = "switch-neglim";                          ///< Input mapping value ID for negative limit switch signal
      const std::string PROBE1_POS_EDG   = "probe1-posedg";                          ///< Input mapping value ID for probe 1 positive edge
      const std::string PROBE1_NEG_EDG   = "probe1-negedg";                          ///< Input mapping value ID for probe 1 negative edge
      const std::string PROBE1_ACTIVE    = "probe1-active";                          ///< Input mapping value ID for probe 1 active
      const std::string PROBE1_POS_LAT   = "probe1-poslat";                          ///< Input mapping value ID for probe 1 positive edge latched times
      const std::string PROBE1_NEG_LAT   = "probe1-neglat";                          ///< Input mapping value ID for probe 1 negative edge latched times
      const std::string PROBE1_POS_DIF   = "probe1-posdif";                          ///< Input mapping value ID for probe 1 position difference between edges
      const std::string PROBE1_MAK_FAL   = "probe1-makfal";                          ///< Input mapping value ID for probe 1 marker failures counter
      const std::string PROBE2_POS_EDG   = "probe2-posedg";                          ///< Input mapping value ID for probe 2 positive edge
      const std::string PROBE2_NEG_EDG   = "probe2-negedg";                          ///< Input mapping value ID for probe 2 negative edge
      const std::string PROBE2_ACTIVE    = "probe2-active";                          ///< Input mapping value ID for probe 2 active
      const std::string PROBE2_POS_LAT   = "probe2-poslat";                          ///< Input mapping value ID for probe 2 positive edge latched times
      const std::string PROBE2_NEG_LAT   = "probe2-neglat";                          ///< Input mapping value ID for probe 2 negative edge latched times
      const std::string PROBE2_POS_DIF   = "probe2-posdif";                          ///< Input mapping value ID for probe 2 position difference between edges
      const std::string PROBE2_MAK_FAL   = "probe2-makfal";                          ///< Input mapping value ID for probe 2 marker failures counter
      const std::string TORQUE_FORCE     = "torque-force";                           ///< Input mapping value ID for torque/force
      const std::string TORQUE_LIMIT_STATUS = "torque-limit-status";                 ///< Input mapping value ID for torque limited by velocity violation
      const std::string OP_MODE_DISPLAY  = "opmode-display";                         ///< Input mapping value ID for mode of operation display
      const std::string SMO_SAFETY_SELECTION = "safety-selection";                   ///< Safety technology. SMO: Safety function selection
      const std::string SMO_SAFETY_STATUS    = "safety-status";                      ///< Safety technology. SMO: Safety function status
      const std::string USER_PDO_INPUT_1 = "user-pdo-input1";                        ///< user receive PDO 1
      const std::string USER_PDO_INPUT_2 = "user-pdo-input2";                        ///< user receive PDO 2
      const std::string USER_PDO_INPUT_3 = "user-pdo-input3";                        ///< user receive PDO 3
      const std::string USER_PDO_INPUT_4 = "user-pdo-input4";                        ///< user receive PDO 4
      const std::string USER_PDO_INPUT_5 = "user-pdo-input5";                        ///< user receive PDO 5
      const std::string USER_PDO_INPUT_6 = "user-pdo-input6";                        ///< user receive PDO 6
      const std::string SIGNAL_STATUS_WORD = "signal-status-word";                   ///< signal status word
      const std::string IO_IN_REFERENCE = "io-encoder-referenced";                   ///< the in reference state
      const std::string STATUS_HOMED = "homed-status";                               ///< the homed status
    } AprInputMappingValueIDStrings;                                                 ///< string

    //! @brief Supported output mapping value IDs as String
    const struct OutputMappingValueIDStrings
    {
      const std::string UNKNOWN       = "unknown";                        ///< Unknown output mapping value ID
      const std::string CONTROLWORD   = "controlword";                    ///< Output mapping value ID for Controlword
      const std::string POSITION      = "position";                       ///< Output mapping value ID for position
      const std::string VELOCITY      = "velocity";                       ///< Output mapping value ID for velocity
      const std::string TORQUE_FORCE  = "torque-force";                   ///< Output mapping value ID for torque/force
      const std::string TORQUE_VEL_LIMIT_HIGH = "torque-high-velocity-limit";///< Output mapping value ID for torque velocity high limitation
      const std::string TORQUE_VEL_LIMIT_LOW = "torque-low-velocity-limit";///< Output mapping value ID for torque velocity low limitation
      const std::string OP_MODE       = "opmode";                         ///< Output mapping value ID for modes of operation
      const std::string USER_PDO_OUTPUT_1 = "user-pdo-output1";           ///< user receive PDO 1
      const std::string USER_PDO_OUTPUT_2 = "user-pdo-output2";           ///< user receive PDO 2
      const std::string USER_PDO_OUTPUT_3 = "user-pdo-output3";           ///< user receive PDO 3
      const std::string USER_PDO_OUTPUT_4 = "user-pdo-output4";           ///< user receive PDO 4
      const std::string USER_PDO_OUTPUT_5 = "user-pdo-output5";           ///< user receive PDO 5
      const std::string USER_PDO_OUTPUT_6 = "user-pdo-output6";           ///< user receive PDO 6
      const std::string IO_SET_REFERENCE = "io-encoder-set-reference";    ///< set-pos-abs
      const std::string IO_REFERENCE_OFFSET = "io-encoder-reference-offset";  ///< the offset value
      const std::string SIGNAL_CONTROL_WORD = "signal-control-word";          ///< signal control word
    } AprOutputMappingValueIDStrings;                                     ///< string

    //! @brief Supported input mapping value IDs as description
    const struct InputMappingValueIDDescriptions
    {

      const std::string UNKNOWN           = "Optional, unknown profile variable.";                                           ///< Unknown input mapping value ID
      const std::string STATUSWORD        = "Mandatory, the statusword is cyclically transmitted from drive to controller."; ///< Input mapping value ID for statusword
      const std::string POSITION          = "Mandatory, the position shows the position feedback value of drive encoder.";   ///< Input mapping value ID for position
      const std::string DIAGMSG           = "Optional, the diagcode shows the current diagnostic message number of drive, when you use the function of Multiple Brake Dynamics, it must be configured."; ///< Input mapping value ID for diagnostic message number
      const std::string VELOCITY          = "Optional, the velocity shows the velocity feedback value of drive encoder, when you use the function of Velocity Mode, it must be configured.";             ///< Input mapping value ID for velocity
      const std::string SWITCH_REF        = "Optional, the switch-ref shows the reference switch signal, when you use the function of Controller Based Homing, it must be configured.";         ///< Input mapping value ID for Reference switch signal
      const std::string SWITCH_POS_LIM    = "Optional, the switch-poslim shows the positive limit switch signal, when you use the function of Controller Based Homing, it must be configured."; ///< Input mapping value ID for Positive limit switch signal
      const std::string SWITCH_NEG_LIM    = "Optional, the switch-neglim shows the negative limit switch signal, when you use the function of Controller Based Homing, it must be configured."; ///< Input mapping value ID for Negative limit switch signal
      const std::string PROBE1_POS_EDG    = "Optional, the probe1-posedg shows the measured positive edge value."; ///< Input mapping value ID for probe 1 positive edge
      const std::string PROBE1_NEG_EDG    = "Optional, the probe1-negedg shows the measured negative edge value."; ///< Input mapping value ID for probe 1 negative edge
      const std::string PROBE1_ACTIVE     = "Optional, the probe1-active shows the check if probe 1 activated.";  ///< Input mapping value ID for probe 1 active
      const std::string PROBE1_POS_LAT    = "Optional, the probe1-poslat shows the positive edge latched times.";  ///< Input mapping value ID for probe 1 positive edge latched times
      const std::string PROBE1_NEG_LAT    = "Optional, the probe1-neglat shows the negative edge latched times.";  ///< Input mapping value ID for probe 1 negative edge latched times
      const std::string PROBE1_POS_DIF    = "Optional, the probe1-posdif shows the positive difference between edges."; ///< Input mapping value ID for probe 1 position difference between edges
      const std::string PROBE1_MAK_FAL    = "Optional, the probe1-makfal shows the marker failure counter."; ///< Input mapping value ID for probe 1 marker failures counter
      const std::string PROBE2_POS_EDG    = "Optional, the probe2-posedg shows the measured positive edge value."; ///< Input mapping value ID for probe 2 positive edge
      const std::string PROBE2_NEG_EDG    = "Optional, the probe2-negedg shows the measured negative edge value."; ///< Input mapping value ID for probe 2 negative edge
      const std::string PROBE2_ACTIVE     = "Optional, the probe2-active shows the check if probe 1 activated.";  ///< Input mapping value ID for probe 2 active
      const std::string PROBE2_POS_LAT    = "Optional, the probe2-poslat shows the positive edge latched times.";  ///< Input mapping value ID for probe 2 positive edge latched times
      const std::string PROBE2_NEG_LAT    = "Optional, the probe2-neglat shows the negative edge latched times.";  ///< Input mapping value ID for probe 2 negative edge latched times
      const std::string PROBE2_POS_DIF    = "Optional, the probe2-posdif shows the positive difference between edges."; ///< Input mapping value ID for probe 2 position difference between edges
      const std::string PROBE2_MAK_FAL    = "Optional, the probe2-makfal shows the marker failure counter."; ///< Input mapping value ID for probe 2 marker failures counter
      const std::string TORQUE_FORCE      = "Optional, the torque/force shows the torque/force feedback value of drive, when you use the function of Torque/Force Control Mode, it must be configured."; ///< Input mapping value ID for torque/force
      const std::string TORQUE_LIMIT_STATUS = "Optional, the torque limit status shows the information of limiting torque due to a speed limit value violation, when you use the function of Torque/Force Control with velocity limit Mode, it must be configured.";  ///< Input mapping value ID for torque limited by velocity violation
      const std::string OP_MODE_DISPLAY   = "Optional, the modes of operation display shows the operation mode";    ///< Input mapping value ID for modes of operation display
      const std::string SMO_SAFETY_SELECTION = "Optional, the safety-selection shows the safety function selection."; ///< Safety technology. SMO: Safety function selection
      const std::string SMO_SAFETY_STATUS    = "Optional, the safety-status shows the status of safety function";     ///< Safety technology. SMO: Safety function status
      const std::string USER_PDO_INPUT_1  = "Optional, the user PDO for receive 1";       ///< user receive PDO 1
      const std::string USER_PDO_INPUT_2  = "Optional, the user PDO for receive 2";       ///< user receive PDO 2
      const std::string USER_PDO_INPUT_3  = "Optional, the user PDO for receive 3";       ///< user receive PDO 3
      const std::string USER_PDO_INPUT_4  = "Optional, the user PDO for receive 4";       ///< user receive PDO 4
      const std::string USER_PDO_INPUT_5  = "Optional, the user PDO for receive 5";       ///< user receive PDO 5
      const std::string USER_PDO_INPUT_6  = "Optional, the user PDO for receive 6";       ///< user receive PDO 6
      const std::string SIGNAL_STATUS_WORD = "Optional, the signal status word";          ///< Signal status word
      const std::string IO_IN_REFERENCE = "Optional, the IO reference state flag";        ///< the in reference state
      const std::string STATUS_HOMED    = "Optional, the homed status";                   ///< the homed status
    } AprInputMappingValueIDDescriptions; //!< description

    //! @brief Supported output mapping value IDs as description
    const struct OutputMappingValueIDDescriptions
    {
      const std::string UNKNOWN             = "Optional, unknown profile variable.";                                             //!< Unknown output mapping value ID
      const std::string CONTROLWORD         = "Mandatory, the control word is cyclically transmitted from controller to drive."; //!< Output mapping value ID for Controlword
      const std::string POSITION            = "Mandatory, the position shows position command value of controller.";             //!< Output mapping value ID for Position
      const std::string VELOCITY            = "Optional, the velocity shows velocity command value of controller, when you use the function of  Velocity Mode, it must be configured."; //!< Output mapping value ID for Velocity
      const std::string TORQUE_FORCE        = "Optional, the torque/force shows torque/force command value of controller, when you use the function of  Torque/Force Control Mode, it must be configured."; //!< Output mapping value ID for Torque/Force
      const std::string TORQUE_VEL_LIMIT_HIGH = "Optional, the high velocity limit for torque mode, when you use the function of Torque/Force Control with velocity limit Mode, it must be configured.";///< Output mapping value ID for torque velocity high limitation
      const std::string TORQUE_VEL_LIMIT_LOW = "Optional, the low velocity limit for torque mode, when you use the function of Torque/Force Control with velocity limit Mode, it must be configured.";///< Output mapping value ID for torque velocity high limitation
      const std::string OP_MODE             = "Optional,the modes of operation shows operation mode command value of controller,when you switch  speed mode or position mode in coe mode";
      const std::string USER_PDO_OUTPUT_1   = "Optional, the user PDO for transmit 1";       ///< user transmit PDO 1
      const std::string USER_PDO_OUTPUT_2   = "Optional, the user PDO for transmit 2";       ///< user transmit PDO 2
      const std::string USER_PDO_OUTPUT_3   = "Optional, the user PDO for transmit 3";       ///< user transmit PDO 3
      const std::string USER_PDO_OUTPUT_4   = "Optional, the user PDO for transmit 4";       ///< user transmit PDO 4
      const std::string USER_PDO_OUTPUT_5   = "Optional, the user PDO for transmit 5";       ///< user transmit PDO 5
      const std::string USER_PDO_OUTPUT_6   = "Optional, the user PDO for transmit 6";       ///< user transmit PDO 6//!< Onput mapping value ID for modes of operation
      const std::string IO_SET_REFERENCE    = "Optional, trigger to set reference";          ///< same interface as homing and set-pos-abs
      const std::string IO_REFERENCE_OFFSET = "Optional, the IO reference offset";           ///< the offset value
      const std::string SIGNAL_CONTROL_WORD = "Optional, the signal control word";           ///< Signal control word
    } AprOutputMappingValueIDDescriptions; //!< description

    //! @brief Mapping group as String
    const struct MappingGroupStrings
    {
      const std::string UNKNOWN                    = "UNKNOWN";       ///< Unknown mapping type
      const std::string BASE                       = "BASE";          ///< Base mapping type
      const std::string VELOCITY                   = "VELOCITY";      ///< Velocity mapping type
      const std::string HOMING                     = "HOMING";        ///< Homing mapping type
      const std::string PROBE                      = "PROBE";         ///< Probe mapping type
      const std::string TORQUE_FORCE               = "TORQUE_FORCE";  ///< Torque/Force mapping type
      const std::string SAFE_MOTION                = "SAFE_MOTION";   ///< Safe motion, safety operation mode mapping type
      const std::string USER_PDO                   = "USER_PDO";      ///< USER_PDO mapping group
      const std::string SIGNAL_WORD                = "SIGNAL_WORD";   ///< For signal control/status word
      const std::string IO_DEVICE                  = "IO_DEVICE";     ///< For IO device
    }AprMappingGroupStrings;

    //! @brief Simple converter method to convert a string to the associated MappingGroup enum entry.
    //! @param[in]   mappingInfoString   Mapping group ID string. Usually extracted from the received flatbuffer. Must exactly match the internally used string defines!
    //! @return The associated MappingGroup enum value.
    inline MappingGroup stringToMappingGroup(const char* mappingInfoString)
    {
      if (!mappingInfoString)
      {
        return MappingGroup::UNKNOWN;
      }

      std::string buffer(mappingInfoString);
      if (buffer.compare(AprMappingGroupStrings.BASE) == 0)                        { return MappingGroup::BASE; }
      if (buffer.compare(AprMappingGroupStrings.VELOCITY) == 0)                    { return MappingGroup::VELOCITY; }
      if (buffer.compare(AprMappingGroupStrings.HOMING) == 0)                      { return MappingGroup::HOMING; }
      if (buffer.compare(AprMappingGroupStrings.PROBE) == 0)                       { return MappingGroup::PROBE; }
      if (buffer.compare(AprMappingGroupStrings.TORQUE_FORCE) == 0)                { return MappingGroup::TORQUE_FORCE; }
      if (buffer.compare(AprMappingGroupStrings.SAFE_MOTION) == 0)                 { return MappingGroup::SAFE_MOTION; }
      if (buffer.compare(AprMappingGroupStrings.USER_PDO) == 0)                    { return MappingGroup::USER_PDO; }
      if (buffer.compare(AprMappingGroupStrings.SIGNAL_WORD) == 0)                 { return MappingGroup::SIGNAL_WORD; }
      if (buffer.compare(AprMappingGroupStrings.IO_DEVICE) == 0)                   { return MappingGroup::IO_DEVICE;}
      return MappingGroup::UNKNOWN;
    }

    //! @brief Simple converter method to convert a string to the associated OutputMappingValueID enum entry.
    //! @param[in]   mappingInfoString   Output value ID string. Usually extracted from the received flatbuffer. Must exactly match the internally used string defines!
    //! @return The associated OutputMappingValueID enum value.
    inline OutputMappingValueID stringToOutputMappingValueID(const char* mappingInfoString)
    {
      if (!mappingInfoString)
      {
        return OutputMappingValueID::UNKNOWN;
      }

      std::string buffer(mappingInfoString);
      if (buffer.compare(AprOutputMappingValueIDStrings.CONTROLWORD) == 0)       { return OutputMappingValueID::CONTROLWORD; }
      if (buffer.compare(AprOutputMappingValueIDStrings.POSITION) == 0)          { return OutputMappingValueID::POSITION; }
      if (buffer.compare(AprOutputMappingValueIDStrings.VELOCITY) == 0)          { return OutputMappingValueID::VELOCITY; }
      if (buffer.compare(AprOutputMappingValueIDStrings.TORQUE_FORCE) == 0)      { return OutputMappingValueID::TORQUE_FORCE; }
      if (buffer.compare(AprOutputMappingValueIDStrings.TORQUE_VEL_LIMIT_HIGH) == 0)      { return OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH; }
      if (buffer.compare(AprOutputMappingValueIDStrings.TORQUE_VEL_LIMIT_LOW) == 0)      { return OutputMappingValueID::TORQUE_VEL_LIMIT_LOW; }
      if (buffer.compare(AprOutputMappingValueIDStrings.OP_MODE) == 0)           { return OutputMappingValueID::OP_MODE; }
      if (buffer.compare(AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_1) == 0) { return OutputMappingValueID::USER_PDO_OUTPUT_1; }
      if (buffer.compare(AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_2) == 0) { return OutputMappingValueID::USER_PDO_OUTPUT_2; }
      if (buffer.compare(AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_3) == 0) { return OutputMappingValueID::USER_PDO_OUTPUT_3; }
      if (buffer.compare(AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_4) == 0) { return OutputMappingValueID::USER_PDO_OUTPUT_4; }
      if (buffer.compare(AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_5) == 0) { return OutputMappingValueID::USER_PDO_OUTPUT_5; }
      if (buffer.compare(AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_6) == 0) { return OutputMappingValueID::USER_PDO_OUTPUT_6; }
      if (buffer.compare(AprOutputMappingValueIDStrings.IO_SET_REFERENCE) == 0)  { return OutputMappingValueID::IO_SET_REFERENCE; }
      if (buffer.compare(AprOutputMappingValueIDStrings.IO_REFERENCE_OFFSET) == 0){ return OutputMappingValueID::IO_REFERENCE_OFFSET; }
      if (buffer.compare(AprOutputMappingValueIDStrings.SIGNAL_CONTROL_WORD) == 0){ return OutputMappingValueID::SIGNAL_CONTROL_WORD; }
      return OutputMappingValueID::UNKNOWN;
    }

    //! @brief Simple converter method to convert a OutputMappingValueID to the associated string.
    //! @param[in]   mappingId            Output value ID.
    //! @return The associated OutputMappingValueID string value.
    inline std::string outputMappingValueIdToString(OutputMappingValueID mappingId)
    {
      switch (mappingId)
      {
        case OutputMappingValueID::CONTROLWORD:         return AprOutputMappingValueIDStrings.CONTROLWORD;
        case OutputMappingValueID::POSITION:            return AprOutputMappingValueIDStrings.POSITION;
        case OutputMappingValueID::VELOCITY:            return AprOutputMappingValueIDStrings.VELOCITY;
        case OutputMappingValueID::TORQUE_FORCE:        return AprOutputMappingValueIDStrings.TORQUE_FORCE;
        case OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH:        return AprOutputMappingValueIDStrings.TORQUE_VEL_LIMIT_HIGH;
        case OutputMappingValueID::TORQUE_VEL_LIMIT_LOW:        return AprOutputMappingValueIDStrings.TORQUE_VEL_LIMIT_LOW;
        case OutputMappingValueID::OP_MODE:             return AprOutputMappingValueIDStrings.OP_MODE;
        case OutputMappingValueID::USER_PDO_OUTPUT_1:   return AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_1;
        case OutputMappingValueID::USER_PDO_OUTPUT_2:   return AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_2;
        case OutputMappingValueID::USER_PDO_OUTPUT_3:   return AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_3;
        case OutputMappingValueID::USER_PDO_OUTPUT_4:   return AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_4;
        case OutputMappingValueID::USER_PDO_OUTPUT_5:   return AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_5;
        case OutputMappingValueID::USER_PDO_OUTPUT_6:   return AprOutputMappingValueIDStrings.USER_PDO_OUTPUT_6;
        case OutputMappingValueID::IO_SET_REFERENCE:    return AprOutputMappingValueIDStrings.IO_SET_REFERENCE;
        case OutputMappingValueID::IO_REFERENCE_OFFSET: return AprOutputMappingValueIDStrings.IO_REFERENCE_OFFSET;
        case OutputMappingValueID::SIGNAL_CONTROL_WORD: return AprOutputMappingValueIDStrings.SIGNAL_CONTROL_WORD;
        default:                                        return AprOutputMappingValueIDStrings.UNKNOWN;
      }
    }

    //! @brief Simple converter method to convert a OutputMappingValueID to the associated description.
    //! @param[in]   mappingId            Output value ID.
    //! @return The    OutputMappingValueID description.
    inline std::string outputMappingValueIdToDescription(OutputMappingValueID mappingId)
    {
      switch (mappingId)
      {
        case OutputMappingValueID::CONTROLWORD:         return AprOutputMappingValueIDDescriptions.CONTROLWORD;
        case OutputMappingValueID::POSITION:            return AprOutputMappingValueIDDescriptions.POSITION;
        case OutputMappingValueID::VELOCITY:            return AprOutputMappingValueIDDescriptions.VELOCITY;
        case OutputMappingValueID::TORQUE_FORCE:        return AprOutputMappingValueIDDescriptions.TORQUE_FORCE;
        case OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH:        return AprOutputMappingValueIDDescriptions.TORQUE_VEL_LIMIT_HIGH;
        case OutputMappingValueID::TORQUE_VEL_LIMIT_LOW:        return AprOutputMappingValueIDDescriptions.TORQUE_VEL_LIMIT_LOW;
        case OutputMappingValueID::OP_MODE:             return AprOutputMappingValueIDDescriptions.OP_MODE;
        case OutputMappingValueID::USER_PDO_OUTPUT_1:   return AprOutputMappingValueIDDescriptions.USER_PDO_OUTPUT_1;
        case OutputMappingValueID::USER_PDO_OUTPUT_2:   return AprOutputMappingValueIDDescriptions.USER_PDO_OUTPUT_2;
        case OutputMappingValueID::USER_PDO_OUTPUT_3:   return AprOutputMappingValueIDDescriptions.USER_PDO_OUTPUT_3;
        case OutputMappingValueID::USER_PDO_OUTPUT_4:   return AprOutputMappingValueIDDescriptions.USER_PDO_OUTPUT_4;
        case OutputMappingValueID::USER_PDO_OUTPUT_5:   return AprOutputMappingValueIDDescriptions.USER_PDO_OUTPUT_5;
        case OutputMappingValueID::USER_PDO_OUTPUT_6:   return AprOutputMappingValueIDDescriptions.USER_PDO_OUTPUT_6;
        case OutputMappingValueID::IO_SET_REFERENCE:    return AprOutputMappingValueIDDescriptions.IO_SET_REFERENCE;
        case OutputMappingValueID::IO_REFERENCE_OFFSET: return AprOutputMappingValueIDDescriptions.IO_REFERENCE_OFFSET;
        case OutputMappingValueID::SIGNAL_CONTROL_WORD: return AprOutputMappingValueIDDescriptions.SIGNAL_CONTROL_WORD;
        default:                                        return AprOutputMappingValueIDDescriptions.UNKNOWN;
      }
    }

    //! @brief Simple converter method to convert a string to the associated InputMappingValueID enum entry.
    //! @param[in]   mappingInfoString   Input value ID string. Usually extracted from the received flatbuffer. Must exactly match the internally used string defines!
    //! @return The associated InputMappingValueID enum value.
    inline InputMappingValueID stringToInputMappingValueID(const char* mappingInfoString)
    {
      if (!mappingInfoString)
      {
        return InputMappingValueID::UNKNOWN;
      }

      std::string buffer(mappingInfoString);
      if (buffer.compare(AprInputMappingValueIDStrings.STATUSWORD)         == 0) { return InputMappingValueID::STATUSWORD; }
      if (buffer.compare(AprInputMappingValueIDStrings.POSITION)           == 0) { return InputMappingValueID::POSITION; }
      if (buffer.compare(AprInputMappingValueIDStrings.DIAGMSG)            == 0) { return InputMappingValueID::DIAGMSG; }
      if (buffer.compare(AprInputMappingValueIDStrings.VELOCITY)           == 0) { return InputMappingValueID::VELOCITY; }
      if (buffer.compare(AprInputMappingValueIDStrings.TORQUE_FORCE)       == 0) { return InputMappingValueID::TORQUE_FORCE; }
      if (buffer.compare(AprInputMappingValueIDStrings.TORQUE_LIMIT_STATUS)== 0) { return InputMappingValueID::TORQUE_LIMIT_STATUS; }
      if (buffer.compare(AprInputMappingValueIDStrings.SWITCH_REF)         == 0) { return InputMappingValueID::SWITCH_REF; }
      if (buffer.compare(AprInputMappingValueIDStrings.SWITCH_POS_LIM)     == 0) { return InputMappingValueID::SWITCH_POS_LIM; }
      if (buffer.compare(AprInputMappingValueIDStrings.SWITCH_NEG_LIM)     == 0) { return InputMappingValueID::SWITCH_NEG_LIM; }

      if (buffer.compare(AprInputMappingValueIDStrings.PROBE1_POS_EDG)     == 0) { return InputMappingValueID::PROBE1_POSEDG; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE1_NEG_EDG)     == 0) { return InputMappingValueID::PROBE1_NEGEDG; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE1_ACTIVE)      == 0) { return InputMappingValueID::PROBE1_ACTIVE; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE1_POS_LAT)     == 0) { return InputMappingValueID::PROBE1_POSLAT; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE1_NEG_LAT)     == 0) { return InputMappingValueID::PROBE1_NEGLAT; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE1_POS_DIF)     == 0) { return InputMappingValueID::PROBE1_POSDIF; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE1_MAK_FAL)     == 0) { return InputMappingValueID::PROBE1_MAKFAL; }

      if (buffer.compare(AprInputMappingValueIDStrings.PROBE2_POS_EDG)     == 0) { return InputMappingValueID::PROBE2_POSEDG; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE2_NEG_EDG)     == 0) { return InputMappingValueID::PROBE2_NEGEDG; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE2_ACTIVE)      == 0) { return InputMappingValueID::PROBE2_ACTIVE; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE2_POS_LAT)     == 0) { return InputMappingValueID::PROBE2_POSLAT; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE2_NEG_LAT)     == 0) { return InputMappingValueID::PROBE2_NEGLAT; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE2_POS_DIF)     == 0) { return InputMappingValueID::PROBE2_POSDIF; }
      if (buffer.compare(AprInputMappingValueIDStrings.PROBE2_MAK_FAL)     == 0) { return InputMappingValueID::PROBE2_MAKFAL; }
      if (buffer.compare(AprInputMappingValueIDStrings.OP_MODE_DISPLAY)    == 0) { return InputMappingValueID::OP_MODE_DISPLAY; }
      if (buffer.compare(AprInputMappingValueIDStrings.SMO_SAFETY_SELECTION) == 0) { return InputMappingValueID::SMO_SAFETY_SELECTION; }
      if (buffer.compare(AprInputMappingValueIDStrings.SMO_SAFETY_STATUS) == 0)    { return InputMappingValueID::SMO_SAFETY_STATUS;    }
      if (buffer.compare(AprInputMappingValueIDStrings.USER_PDO_INPUT_1)   == 0) { return InputMappingValueID::USER_PDO_INPUT_1; }
      if (buffer.compare(AprInputMappingValueIDStrings.USER_PDO_INPUT_2)   == 0) { return InputMappingValueID::USER_PDO_INPUT_2; }
      if (buffer.compare(AprInputMappingValueIDStrings.USER_PDO_INPUT_3)   == 0) { return InputMappingValueID::USER_PDO_INPUT_3; }
      if (buffer.compare(AprInputMappingValueIDStrings.USER_PDO_INPUT_4)   == 0) { return InputMappingValueID::USER_PDO_INPUT_4; }
      if (buffer.compare(AprInputMappingValueIDStrings.USER_PDO_INPUT_5)   == 0) { return InputMappingValueID::USER_PDO_INPUT_5; }
      if (buffer.compare(AprInputMappingValueIDStrings.USER_PDO_INPUT_6)   == 0) { return InputMappingValueID::USER_PDO_INPUT_6; }
      if (buffer.compare(AprInputMappingValueIDStrings.SIGNAL_STATUS_WORD) == 0) { return InputMappingValueID::SIGNAL_STATUS_WORD; }
      if (buffer.compare(AprInputMappingValueIDStrings.IO_IN_REFERENCE) == 0)    { return InputMappingValueID::IO_IN_REFERENCE; }
      if (buffer.compare(AprInputMappingValueIDStrings.STATUS_HOMED) == 0)       { return InputMappingValueID::HOMED_STATUS; }
      return InputMappingValueID::UNKNOWN;
    }
    //! @brief Convert the mapping item type to string.
    //! @param[in]   mappingGroup            mapping item type
    //! @return      The associated mapping item type string value.
    inline std::string mappingGroupToString(MappingGroup mappingGroup)
    {
      switch (mappingGroup)
      {
        case MappingGroup::BASE:               return AprMappingGroupStrings.BASE;
        case MappingGroup::VELOCITY:           return AprMappingGroupStrings.VELOCITY;
        case MappingGroup::HOMING:             return AprMappingGroupStrings.HOMING;
        case MappingGroup::PROBE:              return AprMappingGroupStrings.PROBE;
        case MappingGroup::TORQUE_FORCE:       return AprMappingGroupStrings.TORQUE_FORCE;
        case MappingGroup::SAFE_MOTION:        return AprMappingGroupStrings.SAFE_MOTION;
        case MappingGroup::USER_PDO:           return AprMappingGroupStrings.USER_PDO;
        case MappingGroup::SIGNAL_WORD:        return AprMappingGroupStrings.SIGNAL_WORD;
        case MappingGroup::IO_DEVICE:          return AprMappingGroupStrings.IO_DEVICE;
        default:                        return AprMappingGroupStrings.UNKNOWN;
      }
    }

    //! @brief Simple converter method to convert a InputMappingValueID to the associated string.
    //! @param[in]   mappingId            Input value ID.
    //! @return The associated InputMappingValueID string value.
    inline std::string inputMappingValueIdToString(InputMappingValueID mappingId)
    {
      switch (mappingId)
      {
        case InputMappingValueID::STATUSWORD:                    return AprInputMappingValueIDStrings.STATUSWORD;
        case InputMappingValueID::POSITION:                      return AprInputMappingValueIDStrings.POSITION;
        case InputMappingValueID::DIAGMSG:                       return AprInputMappingValueIDStrings.DIAGMSG;
        case InputMappingValueID::VELOCITY:                      return AprInputMappingValueIDStrings.VELOCITY;
        case InputMappingValueID::TORQUE_FORCE:                  return AprInputMappingValueIDStrings.TORQUE_FORCE;
        case InputMappingValueID::TORQUE_LIMIT_STATUS:           return AprInputMappingValueIDStrings.TORQUE_LIMIT_STATUS;
        case InputMappingValueID::SWITCH_REF:                    return AprInputMappingValueIDStrings.SWITCH_REF;
        case InputMappingValueID::SWITCH_POS_LIM:                return AprInputMappingValueIDStrings.SWITCH_POS_LIM;
        case InputMappingValueID::SWITCH_NEG_LIM:                return AprInputMappingValueIDStrings.SWITCH_NEG_LIM;
        case InputMappingValueID::PROBE1_POSEDG:                 return AprInputMappingValueIDStrings.PROBE1_POS_EDG;
        case InputMappingValueID::PROBE1_NEGEDG:                 return AprInputMappingValueIDStrings.PROBE1_NEG_EDG;
        case InputMappingValueID::PROBE1_ACTIVE:                 return AprInputMappingValueIDStrings.PROBE1_ACTIVE;
        case InputMappingValueID::PROBE1_POSLAT:                 return AprInputMappingValueIDStrings.PROBE1_POS_LAT;
        case InputMappingValueID::PROBE1_NEGLAT:                 return AprInputMappingValueIDStrings.PROBE1_NEG_LAT;
        case InputMappingValueID::PROBE1_POSDIF:                 return AprInputMappingValueIDStrings.PROBE1_POS_DIF;
        case InputMappingValueID::PROBE1_MAKFAL:                 return AprInputMappingValueIDStrings.PROBE1_MAK_FAL;
        case InputMappingValueID::PROBE2_POSEDG:                 return AprInputMappingValueIDStrings.PROBE2_POS_EDG;
        case InputMappingValueID::PROBE2_NEGEDG:                 return AprInputMappingValueIDStrings.PROBE2_NEG_EDG;
        case InputMappingValueID::PROBE2_ACTIVE:                 return AprInputMappingValueIDStrings.PROBE2_ACTIVE;
        case InputMappingValueID::PROBE2_POSLAT:                 return AprInputMappingValueIDStrings.PROBE2_POS_LAT;
        case InputMappingValueID::PROBE2_NEGLAT:                 return AprInputMappingValueIDStrings.PROBE2_NEG_LAT;
        case InputMappingValueID::PROBE2_POSDIF:                 return AprInputMappingValueIDStrings.PROBE2_POS_DIF;
        case InputMappingValueID::PROBE2_MAKFAL:                 return AprInputMappingValueIDStrings.PROBE2_MAK_FAL;
        case InputMappingValueID::OP_MODE_DISPLAY:               return AprInputMappingValueIDStrings.OP_MODE_DISPLAY;
        case InputMappingValueID::SMO_SAFETY_SELECTION:          return AprInputMappingValueIDStrings.SMO_SAFETY_SELECTION;
        case InputMappingValueID::SMO_SAFETY_STATUS:             return AprInputMappingValueIDStrings.SMO_SAFETY_STATUS;
        case InputMappingValueID::USER_PDO_INPUT_1:              return AprInputMappingValueIDStrings.USER_PDO_INPUT_1;
        case InputMappingValueID::USER_PDO_INPUT_2:              return AprInputMappingValueIDStrings.USER_PDO_INPUT_2;
        case InputMappingValueID::USER_PDO_INPUT_3:              return AprInputMappingValueIDStrings.USER_PDO_INPUT_3;
        case InputMappingValueID::USER_PDO_INPUT_4:              return AprInputMappingValueIDStrings.USER_PDO_INPUT_4;
        case InputMappingValueID::USER_PDO_INPUT_5:              return AprInputMappingValueIDStrings.USER_PDO_INPUT_5;
        case InputMappingValueID::USER_PDO_INPUT_6:              return AprInputMappingValueIDStrings.USER_PDO_INPUT_6;
        case InputMappingValueID::SIGNAL_STATUS_WORD:            return AprInputMappingValueIDStrings.SIGNAL_STATUS_WORD;
        case InputMappingValueID::IO_IN_REFERENCE:               return AprInputMappingValueIDStrings.IO_IN_REFERENCE;
        case InputMappingValueID::HOMED_STATUS:                  return AprInputMappingValueIDStrings.STATUS_HOMED;
        default:                                    return AprInputMappingValueIDStrings.UNKNOWN;
      }
    }

    //! @brief Simple converter method to convert a InputMappingValueID to the associated description.
    //! @param[in]   mappingId            Input value ID.
    //! @return The associated InputMappingValueID description.
    inline std::string inputMappingValueIdToDescription(InputMappingValueID mappingId)
    {
      switch (mappingId)
      {
        case InputMappingValueID::STATUSWORD:                    return AprInputMappingValueIDDescriptions.STATUSWORD;
        case InputMappingValueID::POSITION:                      return AprInputMappingValueIDDescriptions.POSITION;
        case InputMappingValueID::DIAGMSG:                       return AprInputMappingValueIDDescriptions.DIAGMSG;
        case InputMappingValueID::VELOCITY:                      return AprInputMappingValueIDDescriptions.VELOCITY;
        case InputMappingValueID::TORQUE_FORCE:                  return AprInputMappingValueIDDescriptions.TORQUE_FORCE;
        case InputMappingValueID::TORQUE_LIMIT_STATUS:           return AprInputMappingValueIDDescriptions.TORQUE_LIMIT_STATUS;
        case InputMappingValueID::SWITCH_REF:                    return AprInputMappingValueIDDescriptions.SWITCH_REF;
        case InputMappingValueID::SWITCH_POS_LIM:                return AprInputMappingValueIDDescriptions.SWITCH_POS_LIM;
        case InputMappingValueID::SWITCH_NEG_LIM:                return AprInputMappingValueIDDescriptions.SWITCH_NEG_LIM;
        case InputMappingValueID::PROBE1_POSEDG:                 return AprInputMappingValueIDDescriptions.PROBE1_POS_EDG;
        case InputMappingValueID::PROBE1_NEGEDG:                 return AprInputMappingValueIDDescriptions.PROBE1_NEG_EDG;
        case InputMappingValueID::PROBE1_ACTIVE:                 return AprInputMappingValueIDDescriptions.PROBE1_ACTIVE;
        case InputMappingValueID::PROBE1_POSLAT:                 return AprInputMappingValueIDDescriptions.PROBE1_POS_LAT;
        case InputMappingValueID::PROBE1_NEGLAT:                 return AprInputMappingValueIDDescriptions.PROBE1_NEG_LAT;
        case InputMappingValueID::PROBE1_POSDIF:                 return AprInputMappingValueIDDescriptions.PROBE1_POS_DIF;
        case InputMappingValueID::PROBE1_MAKFAL:                 return AprInputMappingValueIDDescriptions.PROBE1_MAK_FAL;
        case InputMappingValueID::PROBE2_POSEDG:                 return AprInputMappingValueIDDescriptions.PROBE2_POS_EDG;
        case InputMappingValueID::PROBE2_NEGEDG:                 return AprInputMappingValueIDDescriptions.PROBE2_NEG_EDG;
        case InputMappingValueID::PROBE2_ACTIVE:                 return AprInputMappingValueIDDescriptions.PROBE2_ACTIVE;
        case InputMappingValueID::PROBE2_POSLAT:                 return AprInputMappingValueIDDescriptions.PROBE2_POS_LAT;
        case InputMappingValueID::PROBE2_NEGLAT:                 return AprInputMappingValueIDDescriptions.PROBE2_NEG_LAT;
        case InputMappingValueID::PROBE2_POSDIF:                 return AprInputMappingValueIDDescriptions.PROBE2_POS_DIF;
        case InputMappingValueID::PROBE2_MAKFAL:                 return AprInputMappingValueIDDescriptions.PROBE2_MAK_FAL;
        case InputMappingValueID::OP_MODE_DISPLAY:               return AprInputMappingValueIDDescriptions.OP_MODE_DISPLAY;
        case InputMappingValueID::SMO_SAFETY_SELECTION:          return AprInputMappingValueIDDescriptions.SMO_SAFETY_SELECTION;
        case InputMappingValueID::SMO_SAFETY_STATUS:             return AprInputMappingValueIDDescriptions.SMO_SAFETY_STATUS;
        case InputMappingValueID::USER_PDO_INPUT_1:              return AprInputMappingValueIDDescriptions.USER_PDO_INPUT_1;
        case InputMappingValueID::USER_PDO_INPUT_2:              return AprInputMappingValueIDDescriptions.USER_PDO_INPUT_2;
        case InputMappingValueID::USER_PDO_INPUT_3:              return AprInputMappingValueIDDescriptions.USER_PDO_INPUT_3;
        case InputMappingValueID::USER_PDO_INPUT_4:              return AprInputMappingValueIDDescriptions.USER_PDO_INPUT_4;
        case InputMappingValueID::USER_PDO_INPUT_5:              return AprInputMappingValueIDDescriptions.USER_PDO_INPUT_5;
        case InputMappingValueID::USER_PDO_INPUT_6:              return AprInputMappingValueIDDescriptions.USER_PDO_INPUT_6;
        case InputMappingValueID::SIGNAL_STATUS_WORD:            return AprInputMappingValueIDDescriptions.SIGNAL_STATUS_WORD;
        case InputMappingValueID::IO_IN_REFERENCE:               return AprInputMappingValueIDDescriptions.IO_IN_REFERENCE;
        case InputMappingValueID::HOMED_STATUS:                  return AprInputMappingValueIDDescriptions.STATUS_HOMED;
        default:                                                 return AprInputMappingValueIDDescriptions.UNKNOWN;
      }
    }

    /// @brief Convert mapping group to mapping group mask.
    /// @param[in] mappingGroup 
    /// @return The mapping group mask
    inline int mappingGroupToGroupMask(MappingGroup mappingGroup)
    {
      switch (mappingGroup)
      {
        case MappingGroup::BASE:               return MappingGroupMask::BASE;
        case MappingGroup::VELOCITY:           return MappingGroupMask::VELOCITY;
        case MappingGroup::HOMING:             return MappingGroupMask::HOMING;
        case MappingGroup::PROBE:              return MappingGroupMask::PROBE;
        case MappingGroup::TORQUE_FORCE:       return MappingGroupMask::TORQUE_FORCE;
        case MappingGroup::SAFE_MOTION:        return MappingGroupMask::SAFE_MOTION;
        case MappingGroup::USER_PDO:           return MappingGroupMask::USER_PDO;
        case MappingGroup::SIGNAL_WORD:        return MappingGroupMask::SIGNAL_WORD;
        case MappingGroup::IO_DEVICE:          return MappingGroupMask::IO_DEVICE;
        default:                               return 0;
      }
    }
  }
}
