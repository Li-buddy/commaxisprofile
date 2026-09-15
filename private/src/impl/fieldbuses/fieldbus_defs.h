#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and contains defines that refer to the fieldbus.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/variant.h"
#include <cstdint>
#include <functional>
#include <string>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // --------------------------------------------------------------------------- 
  const std::string  DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES = "fieldbuses/ethercat/master/instances";   ///< Datalayer address of ethercat master instances for browsing

  const std::string  DL_SUB_ADDRESS_MASTER_STATE  = "/admin/fieldbus_status/master_state";                 ///< Datalayer sub address of master state, string data
  const std::string  DL_SUB_ADDRESS_MASTER_STATUS = "/admin/fieldbus_status/master_status";                ///< Datalayer sub address of master state, std::uint32_t data
  const std::string  DL_SUB_ADDRESS_SLAVE_STATE   = "/device_access/slave_state";                          ///< Datalayer sub address of slave state, string data
  const std::string  DL_SUB_ADDRESS_SLAVE_STATUS  = "/device_access/slave_status";                         ///< Datalayer sub address of slave status, std::uint32_t data
  const std::string  DL_SUB_ADDRESS_DC_CFG_INFO   = "/device_access/distributed_clock/config_info";        ///< Datalayer sub address of slave distributed clock config info
  const std::string  DL_SUB_ADDRESS_SOE_IDN       = "/device_access/soe/parameter";                        ///< Datalayer sub address of SoE parameter access
  const std::string  DL_SUB_ADDRESS_COE_SDO       = "/device_access/coe/sdo";                              ///< Datalayer sub address of CoE SDO access
  const std::string  DL_SUB_ADDRESS_SLAVE_CFG_INFO = "/device_access/slave_config_info";                   ///< Datalayer sub address of slave config info
  const std::string  DL_SUB_ADDRESS_ADDRESS_MAP    = "/device_access/slave_address_mapping";               ///< Datalayer sub address of slave address mapping

  const std::string  DL_SUB_ADDRESS_RTDATA_INPUT_MASTER = "/realtime_data/input";                          ///< Datalayer sub address of realtime data provided by ethercat master

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This struct can be used for response callbacks of fieldbus parameter requests.
  struct ParameterResponseCallbackData
  {
    bool m_requestFinishedFlag{false};                                         ///< Flag that indicates if the response callback has been executed
    comm::datalayer::Variant m_requestVariant{};                               ///< Variant for the parameter request
    comm::datalayer::DlResult m_dlResult = comm::datalayer::DlResult::DL_OK;   ///< Datalayer return status code
  };

  // **** Basic parameter read/write structures (do not use them for requests!) **** //
  //                      +----------------------+
  //                      |   ParamReqDataBase   |
  //                      +----------------------+
  //                                  |
  //                                  V             Important: The buffer in request is only for writing (depend on ECMaster fbs).
  //                      +----------------------+             Set real pointer and length for write request;
  //                      |   ParamReqDataECat   |             Set nullptr and 0 length for read request,
  //                      |----------------------|             the read value from drive is in the buffer of response.
  //                      |  address             |             
  //                      |  maxLen: 16          |             maxLen is maximal length to read/write, ECMaster
  //                      |  writeBufPtr: nullptr|             returns error when maxLen is smaller than actual data
  //                      |  writeBufLen: 0      |             size in drive.
  //                      +----------------------+
  //                                  |
  //                 +----------------+----------------+
  //                 |                                 |
  //                 V                                 V
  //      +---------------------+           +---------------------+
  //      |   ParamReqDataSoE   |           |   ParamReqDataCoE   |
  //      |---------------------|           |---------------------|
  //      |  driveNum: 0        |           |  objEntry           |
  //      |  idn                |           |                     |
  //      |  elementFlag        |           |                     |
  //      +---------------------+           +---------------------+

  /// @brief This struct contains all basic information for a fieldbus parameter requests.
  ///        Note: Currently this struct is empty but it will be useful when we support more fieldbuses.
  struct ParamReqDataBase {};

  /// @brief This struct contains data request of EtherCAT devices.
  struct ParamReqDataECat : public ParamReqDataBase
  {
    /// @brief Default constructor
    ParamReqDataECat() = default;

    /// @brief Explicit constructor
    /// @param[in]  addr              Address of slave device
    /// @param[in]  writeBufPtr       Buffer Pointer of writing data, reading request set to nullptr
    /// @param[in]  writeBufLen       Buffer length for writing data, reading request set to 0
    /// @param[in]  maxLen            Maximal length of the request data in bytes for both read and write, default 16
    explicit ParamReqDataECat(std::uint16_t addr, std::uint8_t* writeBufPtr = nullptr, std::uint32_t writeBufLen = 0, std::uint32_t maxLen = 16) noexcept
      : address(addr), writeBufPtr(writeBufPtr), writeBufLen(writeBufLen), maxLen(maxLen) {};

    /// @brief Copy constructor
    /// @param[in]  src               The source ParamReqDataECat to copy from
    ParamReqDataECat(const ParamReqDataECat& src) noexcept
      : address(src.address), writeBufPtr(src.writeBufPtr), writeBufLen(src.writeBufLen), maxLen(src.maxLen) {};

    /// @brief Move constructor
    /// @param[in] src                The source ParamReqDataECat to move from
    ParamReqDataECat(ParamReqDataECat&& src) noexcept
      : address(src.address), writeBufPtr(src.writeBufPtr), writeBufLen(src.writeBufLen), maxLen(src.maxLen) {};

    std::uint16_t address {0};            ///< Address of slave device
    std::uint8_t* writeBufPtr {nullptr};  ///< Buffer Pointer of writing data, reading request set to nullptr
    std::uint32_t writeBufLen {0};        ///< Buffer length for writing data, reading request set to 0
    std::uint32_t maxLen {16};            ///< Maximal length of the request data in bytes, requirement of EtherCAT master
  };

  /// @brief This struct contains all data for SoE request.
  struct ParamReqDataSoE : public ParamReqDataECat
  {
    /// @brief Default constructor
    ParamReqDataSoE() = default;

    // @brief Explicit constructor
    /// @param[in]  addr              Address of slave device
    /// @param[in]  idn               IDN of the parameter
    /// @param[in]  elementFlag       Element flag of the parameter
    /// @param[in]  driveNum          Drive number, default 0 for single drive
    /// @param[in]  writeBufPtr       Buffer Pointer of writing data, reading request set to nullptr
    /// @param[in]  writeBufLen       Buffer length for writing data, reading request set to 0
    /// @param[in]  maxLen            Maximal length of the request data in bytes, default 16
    ParamReqDataSoE(std::uint16_t addr, std::uint16_t idn, std::uint8_t elementFlag, std::uint8_t driveNum = 0,
                    std::uint8_t* writeBufPtr = nullptr, std::uint32_t writeBufLen = 0, std::uint32_t maxLen = 16) noexcept
      : ParamReqDataECat(addr, writeBufPtr, writeBufLen, maxLen), driveNum(driveNum), idn(idn), elementFlag(elementFlag) {};

    /// @brief Copy constructor
    /// @param[in]  src               The source ParamReqDataSoE to copy from
    ParamReqDataSoE(const ParamReqDataSoE& src) noexcept
      : ParamReqDataECat(src), driveNum(src.driveNum), idn(src.idn), elementFlag(src.elementFlag) {};

    /// @brief Move constructor
    /// @param[in]  src               The source ParamReqDataSoE to move from
    ParamReqDataSoE(ParamReqDataSoE&& src) noexcept
      : ParamReqDataECat(src), driveNum(src.driveNum), idn(src.idn), elementFlag(src.elementFlag) {};

    std::uint8_t driveNum {0};        ///< Drive number, default 0 for single drive
    std::uint16_t idn {};             ///< IDN of the parameter
    std::uint8_t elementFlag {};      ///< Element flag of the parameter
  };

  /// @brief This struct contains all data for CoE request.
  struct ParamReqDataCoE : public ParamReqDataECat
  {
    /// @brief Default constructor
    ParamReqDataCoE() = default;
    /// @brief Explicit constructor
    /// @param[in]  addr              Address of slave device
    /// @param[in]  objEntry          Object directory entry of the parameter, including index and sub-index
    /// @param[in]  writeBufPtr       Buffer Pointer of writing data, reading request set to nullptr
    /// @param[in]  writeBufLen       Buffer length for writing data, reading request set to 0
    /// @param[in]  maxLen            Maximal length of the request data in bytes,
    ParamReqDataCoE(std::uint16_t addr, ObjectDirEntry objEntry,
                    std::uint8_t* writeBufPtr = nullptr, std::uint32_t writeBufLen = 0, std::uint32_t maxLen = 16) noexcept
      : ParamReqDataECat(addr, writeBufPtr, writeBufLen, maxLen), objEntry(objEntry) {};

    /// @brief Copy constructor
    ParamReqDataCoE(const ParamReqDataCoE& src) noexcept
      : ParamReqDataECat(src), objEntry(src.objEntry){};
    /// @brief Move constructor
    ParamReqDataCoE(ParamReqDataCoE&& src) noexcept
      : ParamReqDataECat(src), objEntry(src.objEntry){};

    ObjectDirEntry objEntry {};      ///< Object directory entry of the parameter, including index and sub-index
  };

  /// @brief This struct contains all basic information to read the slave state via fieldbus.
  ///        Note: Currently this struct is empty but it will be useful when we support more fieldbuses.
  struct ReadSlaveStateDataBase
  {
  };

  /// @brief This struct contains all data for a Sercos or CiA402 fieldbus parameter  write requests.
  struct ReadSlaveStateData : public ReadSlaveStateDataBase
  {
    std::uint16_t m_deviceAddress {};   ///< Address of the device
  };

  /// @brief Bits structure of master status
  typedef struct MasterStatusBits
  {
    std::uint32_t NoEthernetLink         : 1;    ///< bit0:     No Ethernet link
    std::uint32_t NotInStateOp           : 1;    ///< bit1:     Not in operation state
    std::uint32_t NotInRequestedState    : 1;    ///< bit2:     Not in required state
    std::uint32_t ReservedBit3           : 1;    ///< bit3:     Reserved
    std::uint32_t StateInit              : 1;    ///< bit4:     Init state
    std::uint32_t StatePreOp             : 1;    ///< bit5:     Pre-operation state
    std::uint32_t StateSafeOp            : 1;    ///< bit6:     Safe-operation state
    std::uint32_t ReservedBit7           : 1;    ///< bit7:     Reserved
    std::uint32_t TopologyNotOK          : 1;    ///< bit8:     Topology not OK
    std::uint32_t SlavesNotInMasterState : 1;    ///< bit9:     Slaves not in master state
    std::uint32_t SlavesError            : 1;    ///< bit10:    Slaves get error
    std::uint32_t ReservedBit11          : 1;    ///< bit11:    Reserved bit 11
    std::uint32_t DcNotInSync            : 1;    ///< bit12:    DC not in synchronized
    std::uint32_t ReservedBits13To27     : 15;   ///< bit13-27: Reserved
    std::uint32_t SettingsInconsistent   : 1;    ///< bit28:    Settings inconsistent
    std::uint32_t PortError              : 1;    ///< bit29:    Port error
    std::uint32_t LicenseError           : 1;    ///< bit30:    License Error
    std::uint32_t ReservedBit31          : 1;    ///< bit30:    Reserved
  }MasterStatusBits;

  /// @brief Master status data union
  typedef union MasterStatus
  {
    std::uint32_t m_status {0};             ///< Master status SE
    MasterStatusBits m_statusBits;          ///< Master status bis
  }MasterStatus;

  /// @brief Bits structure of slave status
  typedef struct SlaveStatusBits
  {
    std::uint32_t NotOnline              : 1;    ///< bit0:    Not online
    std::uint32_t NotInStateOp           : 1;    ///< bit1:    Not in operation state
    std::uint32_t Error                  : 1;    ///< bit2:    Error
    std::uint32_t ReservedBit3           : 1;    ///< bit3:    Reserved bit 3
    std::uint32_t StateInit              : 1;    ///< bit4:    Init state
    std::uint32_t StatePreOP             : 1;    ///< bit5:    Pre-operation state
    std::uint32_t StateSafeOP            : 1;    ///< bit6:    Safe-operation state
    std::uint32_t ReservedBits7To31      : 25;   ///< bit7-31: Reserved
  }SlaveStatusBits;

  /// @brief Slave status data union
  typedef union SlaveStatus
  {
    std::uint32_t m_status {0};             ///< Slave status SE
    SlaveStatusBits m_statusBits;           ///< Slave status bis
  }SlaveStatus;

  /// @brief struct memory location that used for datalayer rt access
  struct RtMemLocation
  {
    std::uint32_t m_bitOffset; ///< offset of the value in the memory in bit
    std::uint32_t m_bitSize;   ///< size of the value in the memory in bit
  };

  /// @brief struct the information for datalayer rt access
  struct RtMemInfoBase
  {
    std::string m_bufferURI = "";                                             ///< datalayer-rt uri of the buffer
    std::shared_ptr<comm::datalayer::IMemoryUser> m_memoryUser = nullptr;     ///< memory user object to access the buffer
    std::uint32_t m_revision = 0;                                             ///< expected revision of the buffer
  };

  /// @brief callback function after getting datalayer rt memory map complete
  using GetRtMemMapCallback = std::function<AxisProfileResult(RtMemInfoBase* memInfo, const comm::datalayer::MemoryMap* memMap)>;

  /// @brief Distribution clock config information
  struct DcConfigInfo
  {
    bool m_dcSupported {false};         ///< Check if the distribution clock supported
    std::uint32_t m_shiftTimeNs {0};    ///< The shift time of the distribution clock, in unit of nanosecond
    std::uint32_t m_cycleTimeNs {0};    ///< The cycle time of the distribution clock, in unit of nanosecond
  };

  // ---------------------------------------------------------------------------
  // Declaration of helper methods
  // ---------------------------------------------------------------------------

  /// @brief This method get highest error information of master in string
  /// @param[in]  status    Status of field bus master
  /// @return Highest error information
  inline const char* getMasterHighestErrorInfoString(const MasterStatus& status)
  {
    if (status.m_statusBits.NoEthernetLink)
    {
      return "No Ethernet link";
    }
    else if (status.m_statusBits.TopologyNotOK)
    {
      return "Topology not OK";
    }
    else if (status.m_statusBits.SlavesError)
    {
      return "Slaves get error";
    }
    else if (status.m_statusBits.SettingsInconsistent)
    {
      return "Settings inconsistent";
    }
    else if (status.m_statusBits.PortError)
    {
      return "Port error";
    }
    else if (status.m_statusBits.LicenseError)
    {
      return "License Error";
    }
    return "No Error";
  }

  /// @brief This method get highest error information of slave in string
  /// @param[in]  status    Status of field bus slave
  /// @return Highest error information
  inline const char* getSlaveHighestErrorInfoString(const SlaveStatus& status)
  {
    if (status.m_statusBits.NotOnline)
    {
      return "Not online";
    }
    else if (status.m_statusBits.Error)
    {
      return "Error";
    }
    return "No Error";
  }
}
