#pragma once

/// @file
///
/// This file is part of the axisprofile library and contains defines relevant
/// for the profile specification.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "fieldbuses/i_fieldbus_adapter.h"
#include "job_object/job_data_type.h"
#include "profile_trace.h"
#include <atomic>
#include <functional>

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

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief description for different interface
  enum class SharingInterfaceType : std::uint16_t
  {
    ITF_TYPE_UNDEFINED = UINT16_MAX,
    ITF_TYPE_COMMAND = 0,       ///< command interface
    ITF_TYPE_STATUSCONTROL,     ///< status control interface
    ITF_TYPE_DIAGMSG,           ///< diag message
    ITF_TYPE_CYCLICDATA,        ///< cyclic interface
    ITF_TYPE_DEVICEPARAM,       ///< device parameter interface
    ITF_TYPE_MAPHANDLER,        ///< mapping handler
    ITF_TYPE_PROBE,             ///< probe interface
    ITF_TYPE_AUXJOB,            ///< attach scheduler job handler
    ITF_TYPE_PARAMACCESS,       ///< parameter access
    ITF_TYPE_SCALING,           ///< scaling operation interface
    ITF_TYPE_CMD,               ///< commands interface
    ITF_TYPE_JOB_MGR,           ///< job manager interface
    ITF_TYPE_CATEGORY,          ///< category interface
    ITF_TYPE_PROPERTY,          ///< property interface

    ITF_TYPE_MAX = ITF_TYPE_PROPERTY,  ///< maximum value, helper for loop
  };
  /// @brief The capacity of the interface type
  constexpr std::uint16_t SHARE_ITF_TYPE_CNT = static_cast<std::uint16_t>(SharingInterfaceType::ITF_TYPE_MAX) + 1;

  /// @brief read write type
  enum class ReadWriteType : std::uint16_t
  {
    READ,   ///< read type
    WRITE   ///< write type
  };

  /// @brief This struct contains the data which are executed during axis profile booting
  struct DeviceInformation
  {
    std::string m_deviceInfo {};  ///< Manufacturer software version
    uint32_t m_vendorID {};       ///< Vendor ID
  };
  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This structure contain available information for target operation mode to be check
  struct AvailabilityInfo
  {
    bool available {true};                  ///< check if the target item is available
    const char* message {};                 ///< message only present when not available

    /// @brief Constructor
    AvailabilityInfo() = default;

    /// @brief Constructor
    /// @param[in]   avail    If target operation mode is available
    /// @param[in]   msg      Detailed error message
    explicit AvailabilityInfo(bool avail, const char* msg = nullptr) : available(avail), message(avail ? nullptr : msg)
    {}

    /// @brief Clear all member to default state
    void clear()
    {
      available = true;
      message = nullptr;
    }
  };

  /// @brief This structure contain operation mode available information for all profile, which will be used from booting config to status control
  typedef struct OpModeAvailInfo
  {
    AvailabilityInfo positionAvail; ///< Check if position mode is available
    AvailabilityInfo velocityAvail; ///< Check if position mode is available
    AvailabilityInfo torqueForceAvail; ///< Check if position mode is available

    /// @brief Constructor
    /// @param[in]   positionAvail      If position mode is available
    /// @param[in]   velocityAvail      If velocity mode is available
    /// @param[in]   torqueForceAvail   If torqueForce mode is available
    /// @param[in]   positionErrStr     Detailed error string when position mode is unavailable
    /// @param[in]   velocityErrStr     Detailed error string when velocity mode is unavailable
    /// @param[in]   torqueForceErrStr  Detailed error string when torqueForce mode is unavailable

    OpModeAvailInfo(bool positionAvail, bool velocityAvail, bool torqueForceAvail, const char* positionErrStr = nullptr, const char* velocityErrStr = nullptr, const char* torqueForceErrStr = nullptr)
      : positionAvail(positionAvail, positionErrStr), velocityAvail(velocityAvail, velocityErrStr), torqueForceAvail(torqueForceAvail, torqueForceErrStr)
    {};

    /// @brief Constructor using a single value
    /// @param[in]   avail      quick set available info for all operation modes
    explicit OpModeAvailInfo(bool avail): positionAvail(avail), velocityAvail(avail), torqueForceAvail(avail)
    {};

  protected:

    /// @brief Clear all member to false state
    void clearOpModeAvailInfo()
    {
      positionAvail.clear();
      velocityAvail.clear();
      torqueForceAvail.clear();
    }

  }OpModeAvailInfo;

  /// @brief User PDO assignment
  struct UserPDOAssignInfo
  {
    bool m_userPDO[USER_PDO_COUNT_LIMIT];
    explicit UserPDOAssignInfo(bool assign)
      :m_userPDO {assign, assign, assign, assign, assign, assign}
    {}
  };

  /// @brief This structure contain if input profile variable is assigned.
  typedef struct ProfileInputAssignedInfo
  {
    bool m_statusWord;            ///< Check if status word is assigned
    bool m_position;              ///< Check if actual position is assigned
    bool m_velocity;              ///< Check if actual velocity is assigned
    bool m_diagcode;              ///< Check if diagnostic code is assigned
    bool m_switchRef;             ///< Check if reference switch is assigned
    bool m_switchPosLim;          ///< Check if positive limit switch is assigned
    bool m_switchNegLim;          ///< Check if negative limit switch is assigned
    bool m_probe1PosEdg;          ///< Check if probe 1 positive edge has been assigned
    bool m_probe1NegEdg;          ///< Check if probe 1 negative edge has been assigned
    bool m_probe1Active;          ///< Check if probe 1 active has been assigned
    bool m_probe1PosLat;          ///< Check if probe 1 positive edge latched has been assigned
    bool m_probe1NegLat;          ///< Check if probe 1 negative edge latched has been assigned
    bool m_probe1PosDif;          ///< Check if probe 1 position difference between edges has been assigned
    bool m_probe1MakFal;          ///< Check if probe 1 marker failure counter has been assigned
    bool m_probe2PosEdg;          ///< Check if probe 2 positive edge has been assigned
    bool m_probe2NegEdg;          ///< Check if probe 2 negative edge has been assigned
    bool m_probe2Active;          ///< Check if probe 2 active has been assigned
    bool m_probe2PosLat;          ///< Check if probe 2 positive edge latched has been assigned
    bool m_probe2NegLat;          ///< Check if probe 2 negative edge latched has been assigned
    bool m_probe2PosDif;          ///< Check if probe 2 position difference between edges has been assigned
    bool m_probe2MakFal;          ///< Check if probe 2 marker failure counter has been assigned
    bool m_torqueForce;           ///< Check if actual torque/force is assigned
    bool m_torqueForceLimSts;     ///< Check if actual torque/force limit status is assigned
    bool m_opModeDisplay;         ///< Check if actual operation mode display is assigned
    bool m_safetySelection;       ///< Check if actual Safety function selection data has been assigned.
    bool m_safetyStatus;          ///< Check if actual Safety function status data has been assigned.
    UserPDOAssignInfo m_userPDOs; ///< flags of whether user PDOs are assigned
    bool m_signalStatusWord;      ///< flag to indicate whether signal status word is assigned.
    bool m_isInReference;         ///< flag to indicate whether in reference state is assigned.
    bool m_homed;                 ///< flag to indicate whether homed state is assigned.

    /// @brief Constructor using a single value
    explicit ProfileInputAssignedInfo(bool assigned)
      : m_statusWord(assigned), m_position(assigned), m_velocity(assigned), m_diagcode(assigned), m_switchRef(assigned),
        m_switchPosLim(assigned), m_switchNegLim(assigned), m_probe1PosEdg(assigned), m_probe1NegEdg(assigned),
        m_probe1Active(assigned), m_probe1PosLat(assigned), m_probe1NegLat(assigned), m_probe1PosDif(assigned),
        m_probe1MakFal(assigned), m_probe2PosEdg(assigned), m_probe2NegEdg(assigned), m_probe2Active(assigned),
        m_probe2PosLat(assigned), m_probe2NegLat(assigned), m_probe2PosDif(assigned), m_probe2MakFal(assigned),
        m_torqueForce(assigned), m_torqueForceLimSts(assigned), m_opModeDisplay(assigned), m_safetySelection(assigned),
        m_safetyStatus(assigned), m_userPDOs(assigned), m_signalStatusWord(assigned), m_isInReference(assigned),
        m_homed(assigned)
    {};

  }ProfileInputAssignedInfo;

  /// @brief This structure contain if output profile variable is assigned.
  typedef struct ProfileOutputAssignedInfo
  {
    bool m_controlWord;           ///< Check if control word is assigned
    bool m_position;              ///< Check if command position is assigned
    bool m_velocity;              ///< Check if command velocity is assigned
    bool m_torqueForce;           ///< Check if command torque/force is assigned
    bool m_torqueVelLimHigh;      ///< Check if command torque/force high velocity limit is assigned
    bool m_torqueVelLimLow;       ///< Check if command torque/force low velocity limit is assigned
    bool m_opMode;                ///< Check if command operation mode is assigned
    UserPDOAssignInfo m_userPDOs; ///< flags of whether user PDOs are assigned
    bool m_setReference;          ///< flags of whether set reference is assigned
    bool m_referenceOffset;       ///< flags of whether reference offset is assigned
    bool m_signalControlWord;      ///< flag to indicate whether signal status word is assigned.

    /// @brief Constructor using a single value
    explicit ProfileOutputAssignedInfo(bool assigned): m_controlWord(assigned), m_position(assigned), m_velocity(assigned),
                                                       m_torqueForce(assigned), m_torqueVelLimHigh(assigned), m_torqueVelLimLow(assigned),
                                                       m_opMode(assigned), m_userPDOs(assigned), m_setReference(assigned), m_referenceOffset(assigned),
                                                       m_signalControlWord(assigned)
    {};

  }ProfileOutputAssignedInfo;

  /// @brief This structure contain if input and output profile variable is assigned.
  typedef struct ProfileAssignedInfo
  {
    ProfileInputAssignedInfo m_input;
    ProfileOutputAssignedInfo m_output;

    /// @brief Constructor
    ProfileAssignedInfo(bool inputAssigned, bool outputAssigned): m_input(inputAssigned), m_output(outputAssigned)
    {};

    /// @brief Constructor using a single value
    explicit ProfileAssignedInfo(bool assigned): m_input(assigned), m_output(assigned)
    {};

  }ProfileAssignedInfo;

  /// TODO: deprecated
  /// @brief This struct contain cyclic job and call back function.
  typedef struct auxJobs
  {
    std::function<comm::axisprofile::AxisProfileResult()> m_jobCyclic;
    // maybe add other member
  }auxJobs;

  /// @brief This struct used to initialize constructors related to axisprofile.
  typedef struct ProfileBasicInfo
  {
    AxisProfileType      m_axisProfileType {};        ///< Type of the profile
    std::string          m_profileName {};            ///< Pointer to name of the profile
    std::uint16_t        m_deviceAddress {};          ///< Address of the connected device
    std::uint8_t         m_driveNumber {};            ///< Drive number
    IFieldbusAdapter*    m_fieldbusAdapter {};        ///< Adapter helper class for the current fieldbus
    ProfileTrace         m_profileTrace {};           ///< Helper class to generate profile trace messages
    std::uint32_t        m_deviceVendorId {};         ///< Vendor ID of the connected device
    std::uint32_t        m_deviceProductCode {};      ///< Product Code of the connected device
  }ProfileBasicInfo;

  /// @brief This struct defined the firmware information data type.
  struct DeviceFirmwareInfo
  {
    DeviceSeries  m_deviceSeries {DeviceSeries::NOTINDICATES}; ///< device series
    std::string   m_manufacturerVersion {};                    ///< Manufacturer software version
    std::int32_t  m_deviceVersion {};                          ///< software version of the device
  };

  /// This method indicates the device series in string
  /// @param[in]   deviceSerie   The device series
  /// @return      The converted device series in string
  inline const char* deviceSerieToString(DeviceSeries deviceSerie)
  {
    switch (deviceSerie)
    {
      case DeviceSeries::INDRADRIVE:         return "IndraDrive";
      case DeviceSeries::CTRLXDRIVE:         return "ctlXDrive";
      case DeviceSeries::NOTINDICATES:       return "3rdPartyOrNotIndicates";
      default:                               return "Undefined";
    }
  }

  /// @brief This structure contains the arguments for homing base date
  struct HomingBaseData
  {
    double newPos;              ///< new position
    AxisProfileEncoder encoder; ///< encoder type
  };

  /// @brief This structure contains the arguments for homing
  struct HomingData : public HomingBaseData
  {
    RefMode mode;         ///< set position method, set new reference to device or control or else
    bool withoutMove;     ///< true for set new reference only (without movement)
  };

  /// @brief This structure contains the arguments for set absolute position
  struct SetAbsPosData : public HomingBaseData
  {
    bool allowPowerEnabled;    ///< true for allow command when power enabled
  };

  DECLARE_NAMED_TYPE(ArgSetAbsolutePosition, SetAbsPosData);  ///< Named type for SetAbsPosData
  DECLARE_NAMED_TYPE(ArgHoming, HomingData);                  ///< Named type for Homing
  DECLARE_NAMED_TYPE(ArgFixedStop, double);                   ///< Named type for FixedStop
  DECLARE_NAMED_TYPE(ArgSetOperationMode, AxisProfileOperationMode);       ///< Named type for set operation mode argument
  DECLARE_NAMED_TYPE(JobDataOperationMode, AxisProfileOperationMode);
  DECLARE_NAMED_TYPE(JobDataDeviceStatus, std::atomic<CmdStatusInDevice>);  /// Named type of job data of status in device
}
