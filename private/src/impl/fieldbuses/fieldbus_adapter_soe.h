#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the
/// fieldbus adapter for Servodrive over Ethercat (SoE).
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
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
  // Forward references
  // ---------------------------------------------------------------------------
  class ProfileTrace;

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

  /// @brief This is the fieldbus adapter for Servodrive over Ethercat (SoE).
  class FieldbusAdapterSoE : public FieldbusAdapter
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   trace   Pointer to the profile trace
    explicit FieldbusAdapterSoE(ProfileTrace* const trace);

    /// @brief destructor
    virtual ~FieldbusAdapterSoE() noexcept override = default;

    void setFieldbusMasterAddress(const std::string& address) final;
    AxisProfileResult getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) final;
    AxisProfileResult readSingleParamAsync(ParameterResponseCallbackData* responseCallbackData, const ParamReqDataBase& readData) final;
    AxisProfileResult writeSingleParamAsync(ParameterResponseCallbackData* responseCallbackData, const ParamReqDataBase& writeData) final;
    AxisProfileResult readSingleParamSync(const ParamReqDataBase& readData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) final;
    AxisProfileResult writeSingleParamSync(const ParamReqDataBase& writeData) final;

  private: // Methods

    /// @brief  This method generates the flatbuffer for fieldbus parameter read request.
    /// @param[out]   variant           Variant address where the created flatbuffer will be stored.
    /// @param[in]    readRequestData   Struct containing all information for the read request
    /// @return       Return value indicating success or error.
    static AxisProfileResult generateParamReadRequestFlatbuffer(flatbuffers::FlatBufferBuilder& builder, comm::datalayer::Variant* variant, const ParamReqDataSoE& readRequestData);

    /// @brief  This method generates the flatbuffer for fieldbus parameter write request.
    /// @param[out]   variant           Variant address where the created flatbuffer will be stored.
    /// @param[in]    readRequestData   Struct containing all information for the write request
    /// @return       Return value indicating success or error.
    static AxisProfileResult generateParamWriteRequestFlatbuffer(flatbuffers::FlatBufferBuilder& builder, comm::datalayer::Variant* variant, const ParamReqDataSoE& writeRequestData);
  };
}
