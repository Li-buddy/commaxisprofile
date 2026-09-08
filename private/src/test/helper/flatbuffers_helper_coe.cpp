/// @file
///
/// @brief This file is part of the axisprofile library and implements the helper to generate different CoE flatbuffers.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "flatbuffers_helper_coe.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace test;
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
FlatbuffersHelperCoE::FlatbuffersHelperCoE()
{
  resetParam();
}

void FlatbuffersHelperCoE::resetParam()
{
  m_params.clear();
  m_params = {
    // index  sub     default value
    {{0x100A, 0},    std::string("FWA-XD1-AXS-V-NNNNN-NN-NN")},       ///< 0x100A-00 (Device software version)
    {{0x1018, 1},    std::uint32_t(0x00000024)},                      ///< 0x1018-01 (Identity vendor ID)
    {{0x24A0, 1},    std::uint16_t(0x0140)},                          ///< 0x24A0-01 (Essential properties of encoder 0)
    {{0x24A2, 1},    std::uint16_t(0x0140)},                          ///< 0x24A2-01 (Essential properties of encoder 1)
    {{0x2505, 2},    std::int32_t(0)},                                ///< 0x2502-02 (Homing offset of ctrlX drive)
    {{0x2B60, 0x0E}, std::uint32_t(0x578FD6D2)},                      ///< 0x2B60-0E (checksum)
    {{0x2B60, 4},    std::vector<comm::axisprofile::ObjectDirEntry>{{0x6091, 2}, {0x6091, 1}, {0x2390, 1}, {0x24A8, 1}}},
                                                                      ///< 0x2B60-04 (checksum IDN list)
    {{0x2906, 1},    std::uint16_t(0x0000)},                          ///< 0x2906-01 (Torque data scaling type)
    {{0x2906, 2},    std::uint16_t(0x0001)},                          ///< 0x2906-02 (Torque scaling factor)
    {{0x2906, 3},    std::uint16_t(0xFFFF)},                          ///< 0x2906-03 (Torque scaling exponent, -1)
                     
    {{0x6502, 0},    std::uint32_t(0x00001380)},                      ///< 0x6502-00 (Supported drive modes, b9:cst, b8:csv, b7:csp, b6:ip ...)
    {{0x6060, 0},    std::uint8_t(0x08)},                             ///< 0x6060-00 (Operation mode)
    {{0x6061, 0},    std::uint8_t(0x08)},                             ///< 0x6061-00 (Operation mode display)
    {{0x6076, 0},    std::uint32_t(0x00000384)},                      ///< 0x6076-00 (Nominal torque)
    {{0x607C, 0},    std::int32_t(0)},                                ///< 0x607C-00 (Distance for homing)
    {{0x6091, 1},    std::uint32_t(0x00000024)},                      ///< 0x6091-01 (Gear in resolution)
    {{0x6091, 2},    std::uint32_t(0x00000024)},                      ///< 0x6091-02 (Gear out resolution)
    {{0x6092, 1},    std::uint32_t(0x000186A0)},                      ///< 0x6092-01 (Feed constant)
    {{0x6098, 1},    std::int8_t(0)},                                 ///< 0x6092-01 (Homing method)
    {{0x60A8, 0},    std::uint32_t(0xFA010000)},                      ///< 0x60A8-00 (Position units, Byte3: position exponent, Byte2: position unit, Byte0-1: reserved)
    {{0x60A9, 0},    std::uint32_t(0xFA010300)},                      ///< 0x60A9-00 (Velocity units, Byte3: velocity exponent, Byte2: velocity unit, Byte1: time unit, Byte0: reserved)
    {{0x60AA, 0},    std::uint32_t(0xFA015700)},                      ///< 0x60AA-00 (Acceleration units, Byte3: acceleration exponent, Byte2: acceleration unit, Byte1: time unit, Byte0: reserved)
    {{0x60CA, 0},    std::uint8_t(0x04)},                             ///< 0x60CA-00 (Torque/Force options code)
  };
}

void FlatbuffersHelperCoE::generateSDOResponseFlatbuffer(comm::datalayer::Variant* const variant, const std::uint16_t & index, const std::uint8_t &subIndex)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  flatbuffers::Offset<flatbuffers::Vector<std::uint8_t>> dataOffset = generateSDOOffset(builder, index, subIndex);
  auto sdoResponseOffset = comm::ethercat::master::fbs::CreateSDOResponse(builder, dataOffset);

  comm::ethercat::master::fbs::SDOBuilder sdoBuilder(builder);
  sdoBuilder.add_response(sdoResponseOffset);

  auto sdoBuilderOffset = sdoBuilder.Finish();
  builder.Finish(sdoBuilderOffset);

  variant->copyFlatbuffers(builder);
}

void FlatbuffersHelperCoE::generateSDORequestFlatbuffer(comm::datalayer::Variant* const variant, const std::uint16_t & index, const std::uint8_t &subIndex, const std::uint8_t* data, const size_t& dataSize)
{
  const std::uint16_t deviceAddress = 0;
  const std::uint32_t maxLength = 64;

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  flatbuffers::Offset<flatbuffers::Vector<std::uint8_t>> dataOffset{};
  struct flatbuffers::Offset<comm::ethercat::master::fbs::SDORequest> sdoRequestOffset{};

  // Write request
  if (data)
  {
    dataOffset = builder.CreateVector(data, dataSize);
    sdoRequestOffset = CreateSDORequest(builder,
                                        comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                        deviceAddress,
                                        index,
                                        subIndex,
                                        comm::ethercat::master::fbs::SDOFlags_completeAccess,
                                        dataOffset,
                                        maxLength);
  }
  // Read request
  else
  {
    sdoRequestOffset = CreateSDORequest(builder,
                                        comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                        deviceAddress,
                                        index,
                                        subIndex,
                                        comm::ethercat::master::fbs::SDOFlags_completeAccess,
                                        0,
                                        maxLength);
  }

  comm::ethercat::master::fbs::SDOBuilder sdoBuilder(builder);
  sdoBuilder.add_request(sdoRequestOffset);
  auto exchangeData = sdoBuilder.Finish();
  builder.Finish(exchangeData);

  variant->copyFlatbuffers(builder);
}

HRESULT FlatbuffersHelperCoE::writeParamFlatbuffer(const comm::ethercat::master::fbs::SDORequest* request)
{
  auto index = request->objectIndex();
  auto subIndex = request->subIndex();
  auto data = request->data();
  return writeParam(index, subIndex, data->data(), data->size());
}

HRESULT FlatbuffersHelperCoE::writeParam(std::uint16_t index, std::uint8_t subIndex, const CoEValue& value)
{
  CoEObj obj {index, subIndex};
  auto it = m_params.find(obj);
  if (it != m_params.end())
  {
    if (value.index() != it->second.index())
    {
      return E_FAIL;
    }
    m_params[obj] = value;
    return S_OK;
  }
  return E_FAIL;
}

HRESULT FlatbuffersHelperCoE::overWriteParam(std::uint16_t index, std::uint8_t subIndex, const CoEValue& mask, const CoEValue& value)
{
  if (mask.index() != value.index())
  {
    return E_FAIL;
  }

  CoEValue oldValue;
  auto res = readParam(index, subIndex, oldValue);
  if (res != S_OK || oldValue.index() != mask.index())
  {
    return E_FAIL;
  }
  
  if (std::holds_alternative<std::uint8_t>(value))
  {
    std::get<std::uint8_t>(oldValue) = (std::get<std::uint8_t>(oldValue) & ~std::get<std::uint8_t>(mask)) | std::get<std::uint8_t>(value);
  }
  else if (std::holds_alternative<std::int8_t>(value))
  {
    std::get<std::int8_t>(oldValue) = (std::get<std::int8_t>(oldValue) & ~std::get<std::int8_t>(mask)) | std::get<std::int8_t>(value);
  }
  else if (std::holds_alternative<std::uint16_t>(value))
  {
    std::get<std::uint16_t>(oldValue) = (std::get<std::uint16_t>(oldValue) & ~std::get<std::uint16_t>(mask)) | std::get<std::uint16_t>(value);
  }
  else if (std::holds_alternative<std::int16_t>(value))
  {
    std::get<std::int16_t>(oldValue) = (std::get<std::int16_t>(oldValue) & ~std::get<std::int16_t>(mask)) | std::get<std::int16_t>(value);
  }
  else if (std::holds_alternative<std::uint32_t>(value))
  {
    std::get<std::uint32_t>(oldValue) = (std::get<std::uint32_t>(oldValue) & ~std::get<std::uint32_t>(mask)) | std::get<std::uint32_t>(value);
  }
  else if (std::holds_alternative<std::int32_t>(value))
  {
    std::get<std::int32_t>(oldValue) = (std::get<std::int32_t>(oldValue) & ~std::get<std::int32_t>(mask)) | std::get<std::int32_t>(value);
  }
  else if (std::holds_alternative<std::string>(value))
  {
    std::get<std::string>(oldValue) = std::get<std::string>(value);
  }
  else if (std::holds_alternative<std::vector<ObjectDirEntry>>(value))
  {
    std::get<std::vector<ObjectDirEntry>>(oldValue) = std::get<std::vector<ObjectDirEntry>>(value);
  }
  return writeParam(index, subIndex, oldValue);
}

HRESULT FlatbuffersHelperCoE::readParam(std::uint16_t index, std::uint8_t subIndex, CoEValue& value)
{
  CoEObj obj {index, subIndex};
  auto it = m_params.find(obj);
  if (it != m_params.end())
  {
    value = m_params[obj];
    return S_OK;
  }
  return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////////
// ****************************** PRIVATE METHODS ****************************** //

inline flatbuffers::Offset<flatbuffers::Vector<std::uint8_t>> FlatbuffersHelperCoE::generateSDOOffset(flatbuffers::FlatBufferBuilder& builder, const std::uint16_t &index, std::uint8_t const &subIndex) const
{
  CoEObj obj {index, subIndex};
  std::uint8_t dummyData4Byte[4] {};
  if (m_params.find(obj) != m_params.end())
  {
    auto& value = m_params.at(obj);
    if (std::holds_alternative<std::uint8_t>(value))
    {
      dummyData4Byte[0] = std::get<std::uint8_t>(value);
      return builder.CreateVector(dummyData4Byte, 1);
    }
    else if (std::holds_alternative<std::int8_t>(value))
    {
      dummyData4Byte[0] = std::get<std::int8_t>(value);
      return builder.CreateVector(dummyData4Byte, 1);
    }
    else if (std::holds_alternative<std::uint16_t>(value))
    {
      dummyData4Byte[0] = std::get<std::uint16_t>(value) & 0xff;
      dummyData4Byte[1] = std::get<std::uint16_t>(value) >> 8;
      return builder.CreateVector(dummyData4Byte, 2);
    }
    else if (std::holds_alternative<std::int16_t>(value))
    {
      dummyData4Byte[0] = std::get<std::int16_t>(value) & 0xff;
      dummyData4Byte[1] = std::get<std::int16_t>(value) >> 8;
      return builder.CreateVector(dummyData4Byte, 2);
    }
    else if (std::holds_alternative<std::uint32_t>(value))
    {
      dummyData4Byte[0] = std::get<std::uint32_t>(value) & 0xff;
      dummyData4Byte[1] = std::get<std::uint32_t>(value) >> 8;
      dummyData4Byte[2] = std::get<std::uint32_t>(value) >> 16;
      dummyData4Byte[3] = std::get<std::uint32_t>(value) >> 24;
      return builder.CreateVector(dummyData4Byte, 4);
    }
    else if (std::holds_alternative<std::int32_t>(value))
    {
      dummyData4Byte[0] = std::get<std::int32_t>(value) & 0xff;
      dummyData4Byte[1] = std::get<std::int32_t>(value) >> 8;
      dummyData4Byte[2] = std::get<std::int32_t>(value) >> 16;
      dummyData4Byte[3] = std::get<std::int32_t>(value) >> 24;
      return builder.CreateVector(dummyData4Byte, 4);
    }
    else if (std::holds_alternative<std::string>(value))
    {
      auto& tmpStr = std::get<std::string>(value);
      return builder.CreateVector(reinterpret_cast<const std::uint8_t*>(tmpStr.data()), tmpStr.size());
    }
    else if (std::holds_alternative<std::vector<ObjectDirEntry>>(value))
    {
      auto& tmpStr = std::get<std::vector<ObjectDirEntry>>(value);
      std::uint16_t len = static_cast<std::uint16_t>(tmpStr.size()) * sizeof(ObjectDirEntry);
      auto dummyData = std::make_unique<std::uint8_t[]>(tmpStr.size() * sizeof(ObjectDirEntry));
      std::memset(dummyData.get(), 0, tmpStr.size() * sizeof(ObjectDirEntry));
      for (uint32_t i = 0; i < tmpStr.size(); ++i)
      {
        ObjectDirEntry entry = tmpStr.at(i);
        //dummyData[4 * i + 0] = 0;
        dummyData[4 * i + 1] = entry.subIndex; //uint8_t
        dummyData[4 * i + 2] = static_cast<uint8_t>(entry.index & 0xFF);
        dummyData[4 * i + 3] = static_cast<uint8_t>((entry.index >> 8) & 0xFF);
      }
      return builder.CreateVector(dummyData.get(), len);
    }
  }
  return builder.CreateVector(dummyData4Byte, 2);
}

inline HRESULT FlatbuffersHelperCoE::writeParam(std::uint16_t index, std::uint8_t subIndex, const std::uint8_t* data, const size_t& dataSize)
{
  CoEObj obj = {index, subIndex};

  if (m_params.find(obj) != m_params.end())
  {
    auto& value = m_params.at(obj);
    if (std::holds_alternative<std::uint16_t>(value) && dataSize == 2)
    {
      std::get<std::uint16_t>(value) = data[0] | (data[1] << 8);
      return S_OK;
    }
    else if (std::holds_alternative<std::int16_t>(value) && dataSize == 2)
    {
      std::get<std::int16_t>(value) = data[0] | (data[1] << 8);
      return S_OK;
    }
    else if (std::holds_alternative<std::uint32_t>(value) && dataSize == 4)
    {
      std::get<std::uint32_t>(value) = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
      return S_OK;
    }
    else if (std::holds_alternative<std::int32_t>(value) && dataSize == 4)
    {
      std::get<std::int32_t>(value) = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
      return S_OK;
    }
    else if (std::holds_alternative<std::string>(value) && dataSize > 0)
    {
      std::get<std::string>(value) = std::string(reinterpret_cast<const char*>(data), dataSize);
      return S_OK;
    }
    else if (std::holds_alternative<std::vector<ObjectDirEntry>>(value) && dataSize % 4 == 0)
    {
      for (uint32_t i = 0; i < dataSize; ++i)
      {
        std::uint16_t _index = data[i * 4 + 2]  | (data[i * 4 + 3] << 8);
        std::uint8_t _subindex = data[i * 4 + 1];
        ObjectDirEntry tmp {_index, _subindex};
        std::get<std::vector<ObjectDirEntry>>(value).emplace_back(tmp);
      }
      return S_OK;
    }
  }
  return E_FAIL;
}
