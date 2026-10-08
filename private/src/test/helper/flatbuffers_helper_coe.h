#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines helper to generate different CoE flatbuffers.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/ethercat/master/fbs/sdo_generated.h"
#include "flatbuffers_helper_common.h"
#include <map>
#include <variant>

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

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief Type definition for CoE object dictionary index and subindex
  using CoEObj = std::pair<std::uint16_t, std::uint8_t>;
  /// @brief Type definition for CoE object dictionary possible simple type of value
  using CoEValue = std::variant<std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t, std::string, std::vector<comm::axisprofile::ObjectDirEntry>>;
  /// @brief Type of SoE parameter map
  using CoEParam = std::map<CoEObj, CoEValue>;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief Helper class to generate CoE flatbuffers.
  class FlatbuffersHelperCoE : public FlatbuffersHelperCommon
  {
  public: // methods

    /// @brief Constructor
    FlatbuffersHelperCoE();
    /// @brief Destructor
    virtual ~FlatbuffersHelperCoE() noexcept override = default;

    /// @brief This method reset all parameters to default values
    void resetParam();

    /// @brief This is a simple helper method to generate parameter response flatbuffers for testing. It uses the member variables of this class as values.
    /// @param[in]   variant      Variant address where the created flatbuffer will be stored.
    /// @param[in]   index        ObjectIndex of the parameter
    /// @param[in]   subIndex     SubIndex of the parameter
    void generateSDOResponseFlatbuffer(comm::datalayer::Variant* const variant, const std::uint16_t & index, const std::uint8_t &subIndex);

    /// @brief This is a simple helper method to generate parameter request flatbuffers for testing.
    /// @param[in]   variant      Variant address where the created flatbuffer will be stored.
    /// @param[in]   index        ObjectIndex of the parameter
    /// @param[in]   subIndex     SubIndex of the parameter
    /// @param[in]   data         Pointer to the data
    /// @param[in]   dataSize     Size of the data
    static void generateSDORequestFlatbuffer(comm::datalayer::Variant* const variant, const std::uint16_t & index, const std::uint8_t &subIndex, const std::uint8_t* data, const size_t& dataSize);

    /// @brief This is a simple helper method to write data for testing. It writes the member variables of this class as values.
    /// @param[in]   paramID    ID of the parameter
    /// @param[in]   data       Pointer to the data
    /// @param[in]   dataSize   Size of the data
    /// @return success or not
    HRESULT writeParamFlatbuffer(const comm::ethercat::master::fbs::SDORequest* request);

    /// @brief This is a simple helper method to write data for testing
    /// @param[in] index        dictionary index of the parameter
    /// @param[in] subIndex     dictionary subindex of the parameter
    /// @param[in] value        New value of parameter
    /// @return success or not
    HRESULT writeParam(std::uint16_t index, std::uint8_t subIndex, const CoEValue& value);

    /// @brief This is a simple helper method to overwrite data with mask for testing
    /// @param[in] index        dictionary index of the parameter
    /// @param[in] subIndex     dictionary subindex of the parameter
    /// @param[in] value        New value of parameter
    /// @param[in] mask         The mask of overwriting
    /// @return success or not
    HRESULT overWriteParam(std::uint16_t index, std::uint8_t subIndex, const CoEValue& mask, const CoEValue& value);

    /// @brief This is a simple helper method to read data for testing
    /// @param[in] index        dictionary index of the parameter
    /// @param[in] subIndex     dictionary subindex of the parameter
    /// @param[out] value       Exist value of parameter
    /// @return success or not
    HRESULT readParam(std::uint16_t index, std::uint8_t subIndex, CoEValue& value);

  private: // Methods

    /// @brief Simple helper method to get the correct parameter value by ID. Current implementation is only for Sercos!
    /// @param[in]   builder        The Sercos parameter ID
    /// @param[in]   index          The CiA402 objectIndex
    /// @param[in]   subIndex       The CiA402 subIndex
    /// @return The parameter value.
    inline struct flatbuffers::Offset<flatbuffers::Vector<std::uint8_t>> generateSDOOffset(flatbuffers::FlatBufferBuilder& builder, const std::uint16_t & index, std::uint8_t const &subIndex) const;

    /// @brief Simple helper method to write the parameter
    /// @param[in]    index         The CiA402 object index
    /// @param[in]    subIndex      The CiA402 object subindex
    /// @param[in]    data          The data to be written
    /// @param[in]    dataSize      The data size
    /// @return The parameter value.
    inline HRESULT writeParam(std::uint16_t index, std::uint8_t subIndex, const std::uint8_t* data, const size_t& dataSize);

  private: // data

    CoEParam m_params;  ///< CoE parameter map
  };
}
