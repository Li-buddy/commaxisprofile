#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines adapter interface for the fieldbus.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/datalayer/datalayer.h"
#include "fieldbus_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for the fieldbus adapter.
  class IFieldbusAdapter
  {
  public: // methods

    /// @brief destructor
    virtual ~IFieldbusAdapter() noexcept = default;

    /// @brief  This method sets the datalayer client.
    /// @param[in]   client   Pointer to the datalayer client
    virtual void setDatalayer(comm::datalayer::IClient* client) = 0;

    /// @brief This method sets the current fieldbus master instance address. It is called every time the system switches to state 'Running'.
    /// @param[in]   address   Address of the fieldbus master instance
    virtual void setFieldbusMasterAddress(const std::string& address) = 0;

    /// @brief This method gets the current fieldbus master instance address.
    /// @return   Address of the current fieldbus master instance.
    virtual std::string getFieldbusMasterAddress() const = 0;

    /// @brief This method extracts the response data of a parameter request from the given variant.
    ///
    /// Uses for know length of response data who's length equals to buffer length exactly.
    /// 
    /// @param[in]    variantData   Variant containing the data.
    /// @param[in]    bufPtr            Pointer of Buffer to store the read raw data
    /// @param[in]    bufLen            Length of Buffer to store the read raw data
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen) = 0;

    /// @brief This method extracts the response data of a parameter request from the given variant.
    ///
    /// Used for unknown length of response data who's length should less than or equal to buffer length, the actual read data length will be returned by readLen.
    /// 
    /// @param[in]    variantData   Variant containing the data.
    /// @param[in]    bufPtr            Pointer of Buffer to store the read raw data
    /// @param[in]    bufLen            Length of Buffer to store the read raw data
    /// @param[out]   readLen           Length of the read raw data that copied into the buffer
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) = 0;

    /// @brief This method reads a single parameter (no list parameters!) from the device via fieldbus using an asynchronous datalayer read request.
    ///        Only call it once and then check the callback data cyclical!
    /// @param[out]   responseCallbackData   Response callback data. Will be written when the callback is executed by the datalayer.
    /// @param[in]    readData               Struct containing all information for the read request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult readSingleParamAsync(ParameterResponseCallbackData* responseCallbackData, const ParamReqDataBase& readData) = 0;

    /// @brief This method writes a single parameter (no list parameters!) to the device via fieldbus using an asynchronous datalayer write request.
    ///        Only call it once and then check the callback data cyclical!
    /// @param[out]   responseCallbackData   Response callback data. Will be written when the callback is executed by the datalayer.
    /// @param[in]    writeData              Struct containing all information for the write request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult writeSingleParamAsync(ParameterResponseCallbackData* responseCallbackData, const ParamReqDataBase& writeData) = 0;

    /// @brief This method reads a single parameter from the device via fieldbus using a synchronous datalayer read request.
    ///
    /// Used for response data with variable length, the method gets response data when raw data length is less than or equal to buffer length, and the actual read data length will be returned by readLen.
    /// 
    /// @param[in]    readReq           Struct containing all information for the read request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @param[in]    bufPtr            Pointer of Buffer to store the read raw data
    /// @param[in]    bufLen            Length of Buffer to store the read raw data
    /// @param[out]   readLen           Length of the read raw data that copied into the buffer
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult readSingleParamSync(const ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) = 0;

    /// @brief This method reads a single parameter from the device via fieldbus using a synchronous datalayer read request.
    ///
    /// Used for response data with fixed length, the method gets response data only when raw data length equal buffer length exactly.
    /// 
    /// @param[in]    readReq           Struct containing all information for the read request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @param[in]    bufPtr            Pointer of Buffer to store the read raw data
    /// @param[in]    bufLen            Length of Buffer to store the read raw data
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult readSingleParamSync(const ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen) = 0;

    /// @brief This method writes a single parameter (no list parameters!) to the device via fieldbus using a synchronous datalayer write request.
    /// @param[in]    writeData              Struct containing all information for the write request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult writeSingleParamSync(const ParamReqDataBase& writeData) = 0;

    /// @brief This method reads the state of the slave via fieldbus using a synchronous datalayer read request.
    /// @param[in]    readData   Struct containing all information for the read request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @param[out]   state      The received state of the slave.
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult readSlaveStateSync(const ReadSlaveStateDataBase& readData, std::string* state) = 0;

    /// @brief This method reads the fieldbus master state using a synchronous datalayer read request.
    /// @param[out]   state      The received state of the slave.
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult readFieldbusMasterStateSync(std::string* state) = 0;

    /// @brief This method reads the status of the slave via fieldbus using a synchronous datalayer read request.
    ///        Status is a new type of state which is std::uint32_t data, each bit indicates specified state.
    /// @param[in]    readData      Struct containing all information for the read request. It is important always to use the correct structure for the current profile and fieldbus!
    /// @param[out]   status        The received status of the slave.
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult readSlaveStatusSync(const ReadSlaveStateDataBase& readData, std::uint32_t& status) = 0;

    /// @brief This method reads the fieldbus master status using a synchronous datalayer read request.
    ///        Status is a new type of state which is std::uint32_t data, each bit indicates specified state.
    /// @param[out]   status        The received status of the slave.
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult readFieldbusMasterStatusSync(std::uint32_t& status) = 0;

    /// @brief This method reads slave distribution clock config information.
    /// @param[in]  slaveAddr       Fixed physical address of the slave
    /// @param[out] dcCfg           Config of distribution clock of slave
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult readSlaveDcConfigInfoSync(std::uint16_t slaveAddr, DcConfigInfo& dcCfg) = 0;
  };
}
