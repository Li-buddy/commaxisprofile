#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implement class of fieldbus master.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/datalayer.h"
#include "i_fieldbus_master.h"
#include <map>

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

  /// @brief Internal ID of fieldbus master real time datalayer
  enum class MasterRtDataID
  {
    TASK_START_TIME,      ///< Field bus master task absolute start time
    TASK_CYCLE_TIME,      ///< Field bus master task cycle time
    OUTPUT_SHIFT_TIME,    ///< Distributed clock output shift time
    SEND_OFFSET,          ///< Distributed clock send offset
  };
  /// @brief This method get master realtime datalayer in string
  /// @param[in]  id        Realtime datalayer data ID
  /// @return string of ID of master realtime datalayer 
  inline const char* getMasterRtDataIDString(const MasterRtDataID& id)
  {
    switch (id)
    {
      case MasterRtDataID::TASK_START_TIME:       return "Task.startTime";
      case MasterRtDataID::TASK_CYCLE_TIME:       return "Task.cycleTime";
      case MasterRtDataID::OUTPUT_SHIFT_TIME:     return "OutputShiftTime";
      case MasterRtDataID::SEND_OFFSET:           return "SendOffset";
      default:                                    return "UnknownType";
    }
  }

  /// @brief Struct the raw value of realtime data of fieldbus master
  struct MasterRtDataRawValue
  {
    std::uint64_t m_taskStartTimeNs {0};   ///< Absolute start time of the fieldbus cycle, in nanosecond
    std::uint32_t m_taskCycleTimeNs {0};   ///< Cycle time of the fieldbus cycle, in nanosecond
    std::uint32_t m_outputShiftTimeNs {0}; ///< Output shift of the fieldbus, in nanosecond, Distribution Clock in ethercat
    std::uint32_t m_sendOffsetNs {0};      ///< Send offset of the fieldbus, in nanosecond, Distribution Clock in ethercat
  };

  /// @brief Struct the SI value of realtime data of fieldbus master
  struct MasterRtDataValue
  {
    double m_taskStartTime {0.0};     ///< Absolute start time of the fieldbus cycle, in second
    double m_taskCycleTime {0.0};     ///< Cycle time of the fieldbus cycle, in second
    double m_outputShiftTime {0.0};   ///< Output shift of the fieldbus, in second, Distribution Clock in ethercat
    double m_sendOffset{0.0};         ///< Send offset of the fieldbus, in second, Distribution Clock in ethercat
  };

  /// @brief Struct the realtime mapping information master's rt data
  struct MasterRtDataMap
  {
    MasterRtDataID m_id;          ///< Internal ID of element
    const char* m_rtKey;          ///< key of the realtime data element
    RtMemLocation m_memLocation;  ///< Memory location of the element
    MasterRtDataMap(MasterRtDataID id, const char* rtKey, RtMemLocation mem)
      :m_id(id), m_rtKey(rtKey), m_memLocation(mem)
    {}
    MasterRtDataMap(MasterRtDataID id, const char* rtKey)
      :m_id(id), m_rtKey(rtKey), m_memLocation(RtMemLocation {0,0})
    {
    }
  };

  /// @brief Struct the realtime information fieldbus master's rt data
  struct MasterRtDataInfo : public RtMemInfoBase
  {
    std::vector<MasterRtDataMap> m_memoryMaps; ///< Locations of all realtime data
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is class of fieldbus master object.
  class FieldbusMaster : public IFieldbusMaster
  {
  public: // methods

    /// @brief constructor
    /// @param[in]  name        Fieldbus master name.
    /// @param[in]  datalayer   Factory of datalayer.
    FieldbusMaster(const char* name, comm::datalayer::IDataLayerFactory* datalayer);
    /// @brief destructor
    virtual ~FieldbusMaster() noexcept override;

    const char* getMasterName() final;
    const std::set<IAxisProfileInternal*>& getLinkedProfiles() final;
    void addLinkedProfile(IAxisProfileInternal* profile) final;
    AxisProfileResult initRtData(bool abort) final;
    AxisProfileResult ProcessRtData() final;
    double getActDcBaseTime() final;
    double getFieldbusCycleTime() final;

  private: // methods

    /// @brief This method copy data to local structure from RT memory.
    /// @param[in]  rawData     Pointer to data of RT memory
    /// @param[in]  memInfo     Memory information datalayer RT
    void copyValueFromRtMemroy(std::uint8_t* rawData, const MasterRtDataMap& memInfo);

    /// @brief This method convert raw RT data to scaled data.
    void covertRtDataRawToScaled();

  private: // data

    const std::string m_masterName;                   ///< Name of fieldbus master
    MasterRtDataRawValue m_rtDataRawValue {};         ///< Realtime data raw value
    MasterRtDataValue m_rtDataSIValue {};             ///< Realtime data SI value
    MasterRtDataInfo m_rtDataInfo {};                 ///< Memory information of realtime data
    std::set<IAxisProfileInternal*> m_linkedProfiles; ///< Profiles linked to this master
    comm::datalayer::IDataLayerFactory* m_datalayer;  ///< Datalayer factory used to open/close datalayer realtime memory
    bool m_traceErrorAlreadySent = false;             ///< Flag to prevent sending same trace error message cyclical task
    double m_actDcBaseTime {0.0};                     ///< Actual distribution clock base time

    static const std::map<MasterRtDataID, std::string> MASTER_RT_DATA; ///< Default ID and url of rt-data that used by axisprofile
  };
}
