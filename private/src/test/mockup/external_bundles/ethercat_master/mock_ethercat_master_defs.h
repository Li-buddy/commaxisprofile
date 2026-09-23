#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_ethercat_master_defs.h
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
//! for the ethercat master mockup.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  #define MOCK_ECAT_MASTER_INSTANCE_NAME "MyEcatMasterTest"                        //!< name
  #define MOCK_ECAT_BASE_ADDRESS         "fieldbuses/ethercat/master/instances"    //!< General ethercatmaster datalayer address
  #define MOCK_ECAT_MASTER_INSTANCE_ADDRESS  MOCK_ECAT_BASE_ADDRESS "/" MOCK_ECAT_MASTER_INSTANCE_NAME  //!< node tree

  #define MOCK_ECAT_COE_SDO_REQUEST      MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/device_access/coe/sdo"                //!< Ethercatmaster datalayer coe sdo address
  #define MOCK_ECAT_SOE_PARAM_REQUEST    MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/device_access/soe/parameter"          //!< Ethercatmaster datalayer soe request address
  #define MOCK_ECAT_SLAVE_STATE          MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/device_access/slave_state"            //!< Ethercatmaster datalayer slave state address
  #define MOCK_ECAT_SLAVE_STATUS         MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/device_access/slave_status"           //!< Ethercatmaster datalayer slave status address
  #define MOCK_ECAT_SLAVE_DC_CFG_INFO    MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/device_access/distributed_clock/config_info" //!< Ethercatmaster datalayer slave distributed clock config info
  #define MOCK_ECAT_INPUTBUFFER_STRING   MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/realtime_data/input"                  //!< Ethercatmaster datalayer input address
  #define MOCK_ECAT_OUTPUTBUFFER_STRING  MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/realtime_data/output"                 //!< Ethercatmaster datalayer output address
  #define MOCK_FIELDBUS_MASTER_STATE     MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/admin/fieldbus_status/master_state"   //!< Fieldbus master state
  #define MOCK_FIELDBUS_MASTER_STATUS    MOCK_ECAT_MASTER_INSTANCE_ADDRESS "/admin/fieldbus_status/master_status"  //!< Fieldbus master status

  #define MOCK_ECAT_MASTERBUFFER_TASK_START_TIME    "Master/Task.StartTime"                     ///< Fieldbus master task absolute start time
  #define MOCK_ECAT_MASTERBUFFER_TASK_CYCLE_TIME    "Master/Task.CycleTime"                     ///< Fieldbus master task cycle time
  #define MOCK_ECAT_MASTERBUFFER_OUTPUT_SHIFT_TIME  "Master/DistributedClocks.OutputShiftTime"  ///< Fieldbus master distributed clock output shift time
  #define MOCK_ECAT_MASTERBUFFER_SEND_OFFSET        "Master/DistributedClocks.SendOffset"       ///< Fieldbus master distributed clock send offset

  #define MOCK_ECAT_INBUFFER_POSITION                  "act_position"                 ///<  actual position
  #define MOCK_ECAT_INBUFFER_VELOCITY                  "act_velocity"                 ///<  actual velocity
  #define MOCK_ECAT_INBUFFER_TORQUE_FORCE              "act_torque_force"             ///<  actual torque/force
  #define MOCK_ECAT_INBUFFER_TORQUE_FORCE_LIMIT_STATUS "torqueforce_limit_status"     ///<  torque/force limit status
  #define MOCK_ECAT_INBUFFER_STATUSWORD                "statusword"                   ///<  statusword
  #define MOCK_ECAT_INBUFFER_DIAGMSG                   "diagcode"                     ///<  diagnostic message
  #define MOCK_ECAT_INBUFFER_SWITCH_REF                "switch_ref"                   ///<  reference switch
  #define MOCK_ECAT_INBUFFER_SWITCH_POS_LIM            "switch_poslim"                ///<  positive limit switch
  #define MOCK_ECAT_INBUFFER_SWITCH_NEG_LIM            "switch_neglim"                ///<  negative limit switch
  #define MOCK_ECAT_INBUFFER_PROBE1_POS_EDG            "probe1_posedge"               ///<  probe 1 positive edge
  #define MOCK_ECAT_INBUFFER_PROBE1_NEG_EDG            "probe1_negedge"               ///<  probe 1 negative edge
  #define MOCK_ECAT_INBUFFER_PROBE1_ACTIVE             "probe1_active"                ///<  if probe 1 activated
  #define MOCK_ECAT_INBUFFER_PROBE1_POS_LAT            "probe1_poslat"                ///<  probe 1 positive edge latched times
  #define MOCK_ECAT_INBUFFER_PROBE1_NEG_LAT            "probe1_neglat"                ///<  probe 1 negative edge latched times
  #define MOCK_ECAT_INBUFFER_PROBE1_POS_DIF            "probe1_posdiff"               ///<  probe 1 positive difference between edges
  #define MOCK_ECAT_INBUFFER_PROBE1_MAK_FAL            "probe1_makfal"                ///<  probe 1 marker failure counter
  #define MOCK_ECAT_INBUFFER_PROBE2_POS_EDG            "probe2_posedge"               ///<  probe 2 positive edge
  #define MOCK_ECAT_INBUFFER_PROBE2_NEG_EDG            "probe2_negedge"               ///<  probe 2 negative edge
  #define MOCK_ECAT_INBUFFER_PROBE2_ACTIVE             "probe2_active"                ///<  if probe 2 activated
  #define MOCK_ECAT_INBUFFER_PROBE2_POS_LAT            "probe2_poslat"                ///<  probe 2 positive edge latched times
  #define MOCK_ECAT_INBUFFER_PROBE2_NEG_LAT            "probe2_neglat"                ///<  probe 2 negative edge latched times
  #define MOCK_ECAT_INBUFFER_PROBE2_POS_DIF            "probe2_posdiff"               ///<  probe 2 positive difference between edges
  #define MOCK_ECAT_INBUFFER_PROBE2_MAK_FAL            "probe2_makfal"                ///<  probe 2 marker failure counter
  #define MOCK_ECAT_INBUFFER_OP_MODE_DISPLAY           "opmode_display"               ///<  modes of operation display
  #define MOCK_ECAT_INBUFFER_SMO_SAFETY_SELECTION      "safety-selection"             ///< Safety technology. SMO: Safety function selection
  #define MOCK_ECAT_INBUFFER_SMO_SAFETY_STATUS         "safety-status"                ///< Safety technology. SMO: Safety function status
  #define MOCK_ECAT_INBUFFER_USER_PDO_INPUT_1          "user-pdo-input1"              ///< user receive PDO 1
  #define MOCK_ECAT_INBUFFER_USER_PDO_INPUT_2          "user-pdo-input2"              ///< user receive PDO 2
  #define MOCK_ECAT_INBUFFER_USER_PDO_INPUT_3          "user-pdo-input3"              ///< user receive PDO 3
  #define MOCK_ECAT_INBUFFER_USER_PDO_INPUT_4          "user-pdo-input4"              ///< user receive PDO 4
  #define MOCK_ECAT_INBUFFER_USER_PDO_INPUT_5          "user-pdo-input5"              ///< user receive PDO 5
  #define MOCK_ECAT_INBUFFER_USER_PDO_INPUT_6          "user-pdo-input6"              ///< user receive PDO 6
  #define MOCK_ECAT_INBUFFER_SIGNAL_STATUS_WORD        "signal-status-word"           ///< signal status word

  #define MOCK_ECAT_OUTBUFFER_POSITION                 "cmd_position"                 ///<  commanded position
  #define MOCK_ECAT_OUTBUFFER_VELOCITY                 "cmd_velocity"                 ///<  commanded velocity
  #define MOCK_ECAT_OUTBUFFER_CMDWORD                  "controlword"                  ///<  controlword
  #define MOCK_ECAT_OUTBUFFER_TORQUE_FORCE             "cmd_torque_force"             ///<  commanded torque/force
  #define MOCK_ECAT_OUTBUFFER_TORQUE_FORCE_LIMIT_HIGH  "cmd_torque_force_limit_high"  ///<  torque/force velocity limit high
  #define MOCK_ECAT_OUTBUFFER_TORQUE_FORCE_LIMIT_LOW   "cmd_torque_force_limit_low"   ///<  torque/force velocity limit low
  #define MOCK_ECAT_INBUFFER_OP_MODE                   "opmode"                       ///<  modes of operation
  #define MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_1         "user-pdo-output1"             ///< user receive PDO 1
  #define MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_2         "user-pdo-output2"             ///< user receive PDO 2
  #define MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_3         "user-pdo-output3"             ///< user receive PDO 3
  #define MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_4         "user-pdo-output4"             ///< user receive PDO 4
  #define MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_5         "user-pdo-output5"             ///< user receive PDO 5
  #define MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_6         "user-pdo-output6"             ///< user receive PDO 6

  #define ARRAY_SIZE_BUFFER (0x86)                            //!< Ethercatmaster buffer size

  const std::uint32_t AXISPROFILE_SLAVE_DATA_AT_SIZE = (16);       //!< size of data in at-telegram of one device in byte
  const std::uint32_t AXISPROFILE_SLAVE_DATA_MDT_SIZE = (10);      //!< size of data in mdt-telegram of one device in byte
  const std::uint32_t SLAVE_DATA_AT_START_OFFSET_IN_BIT = (0);     //!< offset of data in at-telegram in bit
  const std::uint32_t SLAVE_DATA_MDT_START_OFFSET_IN_BIT = (0);    //!< offset of data in mdt-telegram in bit
  const std::uint32_t SLAVE_DATA_AT_START_OFFSET_IN_BYTE = (0);    //!< offset of data in at-telegram in bit
  const std::uint32_t SLAVE_DATA_MDT_START_OFFSET_IN_BYTE = (0);   //!< offset of data in mdt-telegram in bit
  const std::uint16_t STSWORD_INOPERATION = 0xC008;                //!< Sercos II StatusWord 'in operation' bitmask
  const std::uint16_t SERCOS_STSWORD_ERRORBIT_OFFSET = 0x000D;     //!< Sercos II Statusword error bit offset
  const std::uint16_t SERCOS_STSWORD_CMD_CHANGE_BIT = 0x0005;      //!< Sercos II Statusword command change bit

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
}
