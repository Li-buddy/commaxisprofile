#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines helper to generate different SoE flatbuffers.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "flatbuffers_helper_common.h"
#include "profiles/sercos/sercos_defs.h"
#include <unordered_map>
#include <variant>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief Type definition for SoE IDN
  using SoEDin = std::uint16_t;
  /// @brief Type definition for SoE possible simple type of value
  using SoEValue = std::variant<std::uint16_t, std::int16_t, std::uint32_t, std::int32_t, std::vector<std::uint16_t>, std::vector<std::uint32_t>>;
  /// @brief Type of SoE parameter map
  using SoEParam = std::unordered_map<SoEDin, SoEValue>;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief Helper class to generate Sercos flatbuffers.
  class FlatbuffersHelperSoE : public FlatbuffersHelperCommon
  {
  public: // methods

    /// @brief Constructor
    FlatbuffersHelperSoE();
    /// @brief Destructor
    virtual ~FlatbuffersHelperSoE() noexcept override = default;

    /// @brief This is a simple helper method to generate parameter response flatbuffers for testing. It uses the member variables of this class as values.
    /// @param[in]   variant    Variant address where the created flatbuffer will be stored.
    /// @param[in]   paramID    ID of the parameter
    void generateParamResponseFlatbuffer(comm::datalayer::Variant* variant, const std::uint16_t paramID);

    /// @brief This is a simple helper method to generate parameter request flatbuffers for testing.
    /// @param[in]   variant    Variant address where the created flatbuffer will be stored.
    /// @param[in]   paramID    ID of the parameter
    /// @param[in]   data       Pointer to the data
    /// @param[in]   dataSize   Size of the data
    /// @param[in]   maxLength  Maximal length of the request data in bytes, 8 bytes for default.
    static void generateParamRequestFlatbuffer(comm::datalayer::Variant* variant, const std::uint16_t paramID, const std::uint8_t* data, const size_t& dataSize, const std::uint32_t& maxLength = 8);

    /// @brief This is a simple helper method to write data for testing. It writes the member variables of this class as values.
    /// @param[in]   request    Writing request
    /// @return success or not
    HRESULT writeParamFlatbuffer(const comm::ethercat::master::fbs::ParameterRequest* request);

    /// @brief This is a simple helper method to write data for testing
    /// @param[in] paramID      ID of the parameter     
    /// @param[in] value        New value of parameter
    /// @return success or not
    HRESULT writeParam(std::uint16_t paramID, const SoEValue& value);

    /// @brief This is a simple helper method to read data for testing
    /// @param[in] paramID      ID of the parameter     
    /// @param[out] value       Exist value of parameter
    /// @return success or not
    HRESULT readParam(std::uint16_t paramID, SoEValue& value);

  private: // Methods

    /// @brief Simple helper method to get the correct parameter value by ID. Current implementation is only for Sercos!
    /// @param[in]   id   The Sercos parameter ID
    /// @return The parameter value.
    inline struct flatbuffers::Offset<flatbuffers::Vector<std::uint8_t>> generateParamOffset(flatbuffers::FlatBufferBuilder& builder, std::uint16_t id) const;

    /// @brief Simple helper method to write the parameter
    /// @param[in]   paramID    ID of the parameter
    /// @param[in]   data       Pointer to the data
    /// @param[in]   dataSize   Size of the data
    /// @return success or not
    inline HRESULT writeParam(std::uint16_t paramID, const std::uint8_t* data, const size_t& dataSize);

  private: // data

    SoEParam m_params;  ///< SoE parameters
  };
}
