#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the
/// fieldbus adapter for Canopen over Ethercat (CoE).
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "fieldbus_adapter.h"

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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the fieldbus adapter for Servodrive over Ethercat (CoE).
  class FieldbusAdapterCoE : public FieldbusAdapter
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   trace   Pointer to the profile trace
    explicit FieldbusAdapterCoE(ProfileTrace* const trace);

    /// @brief destructor
    virtual ~FieldbusAdapterCoE() noexcept override = default;

      void setFieldbusMasterAddress(const std::string& address) final;
      AxisProfileResult getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) final;
      AxisProfileResult readSingleParamAsync(ParameterResponseCallbackData* responseCallbackData, const ParamReqDataBase& readData) final;
      AxisProfileResult writeSingleParamAsync(ParameterResponseCallbackData* responseCallbackData, const ParamReqDataBase& writeData) final;
      AxisProfileResult readSingleParamSync(const ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) final;
      AxisProfileResult writeSingleParamSync(const ParamReqDataBase& writeData) final;

  private: // methods

    /// @brief  This method generates the flatbuffer for CiA402 SDO read request.
    /// @param[out]   variant           Variant address where the created flatbuffer will be stored.
    /// @param[in]    readRequestData   Struct containing all information for the read request
    /// @return       Return value indicating success or error.
    static AxisProfileResult generateParamReadRequestFlatbuffer(flatbuffers::FlatBufferBuilder& builder, comm::datalayer::Variant* variant, const ParamReqDataCoE& readRequestData);

    /// @brief  This method generates the flatbuffer for CiA402 SDO write request.
    /// @param[out]   variant           Variant address where the created flatbuffer will be stored.
    /// @param[in]    writeRequestData   Struct containing all information for the write request
    /// @return       Return value indicating success or error.
    static AxisProfileResult generateParamWriteRequestFlatbuffer(flatbuffers::FlatBufferBuilder& builder, comm::datalayer::Variant* variant, const ParamReqDataCoE& writeRequestData);

  private: // data

    std::string &m_fieldbusSDORequestAddress = FieldbusAdapter::m_fieldbusParamRequestAddress;
  };
}
