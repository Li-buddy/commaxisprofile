/// @file
///
/// @brief This file is part of the axisprofile library and implements the helper to generate different SoE flatbuffers.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "flatbuffers_helper_soe.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace test;
using namespace comm::axisprofile::sercos;

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
FlatbuffersHelperSoE::FlatbuffersHelperSoE() :

  // Initialization all parameters
  m_params  {
    //DIN-set-num       default value
    {IDN_S(0, 26),      std::vector<uint32_t>{400, 403, 404}},                                    ///< S-0-0026 (Configuration list for signal status word)
    {IDN_S(0, 32),      std::uint16_t(OperationModeList::POSITION_CONTROL_LAGLESS_ENCODER_1)},    ///< S-0-0032 (Position control lagless, encoder 1)
    {IDN_S(0, 33),      std::uint16_t(OperationModeList::VELOCITY_CONTROL)},                      ///< S-0-0034 (Velocity control)
    {IDN_S(0, 34),      std::uint16_t(OperationModeList::TORQUE_FORCE_CONTROL_VEL_LIMIT)},        ///< S-0-0035 (Torque/force control)
    {IDN_S(0, 38),      std::uint32_t(1000)},                                                     ///< S-0-0038 (Positive velocity limit value)
    {IDN_S(0, 39),      std::int32_t(-1000)},                                                     ///< S-0-0039 (Negative velocity limit value)
    {IDN_S(0, 44),      std::uint16_t(0x0001)},                                                   ///< S-0-0044 (Velocity data scaling type - linear, preferred, mm/min)
    {IDN_S(0, 45),      std::uint16_t(1)},                                                        ///< S-0-0045 (Linear position data scaling factor)
    {IDN_S(0, 46),      std::int16_t(-6)},                                                        ///< S-0-0046 (Linear position data scaling exponent)
    {IDN_S(0, 49),      std::int32_t(1000)},                                                      ///< S-0-0049 (Positive position limit value)
    {IDN_S(0, 50),      std::int32_t(-1000)},                                                     ///< S-0-0050 (Negative position limit value)
    {IDN_S(0, 52),      std::int32_t(0)},                                                         ///< S-0-0052 (Reference distance of encoder 1)
    {IDN_S(0, 54),      std::int32_t(0)},                                                         ///< S-0-0054 (Reference distance of encoder 2)
    {IDN_S(0, 76),      std::uint16_t(0x0001)},                                                   ///< S-0-0076 (Position data scaling type - linear, preferred, meter)
    {IDN_S(0, 77),      std::uint16_t(1)},                                                        ///< S-0-0077 (Linear position data scaling factor)
    {IDN_S(0, 78),      std::int16_t(-7)},                                                        ///< S-0-0078 (Linear position data scaling exponent)
    {IDN_S(0, 79),      std::uint32_t(3600000)},                                                  ///< S-0-0079 (Rotational position resolution)
    {IDN_S(0, 86),      std::uint16_t(0x0001)},                                                   ///< S-0-0086 (Torque data scaling type - percentage, preferred, newton)
    {IDN_S(0, 91),      std::uint32_t(10000)},                                                    ///< S-0-0091 (Bipolar velocity limit value)
    {IDN_S(0, 92),      std::uint16_t(1000)},                                                     ///< S-0-0092 (Torque bipolar limit value)
    {IDN_S(0, 93),      std::uint16_t(1)},                                                        ///< S-0-0093 (Torque data scaling factor)
    {IDN_S(0, 94),      std::int16_t(-1)},                                                        ///< S-0-0094 (Torque position data scaling exponent)
    {IDN_S(0, 99),      std::uint16_t(0x0003)},                                                   ///< S-0-0099 (C0500 Reset class 1 diagnostics)
    {IDN_S(0, 103),     std::uint32_t(3600000)},                                                  ///< S-0-0103 (Maximum travel range: Modulo format)
    {IDN_S(0, 115),     std::uint16_t(0x0140)},                                                   ///< S-0-0115 (Essential properties of encoder 2)
    {IDN_S(0, 121),     std::uint32_t(1)},                                                        ///< S-0-0121 (Gear input revolution)
    {IDN_S(0, 122),     std::uint32_t(1)},                                                        ///< S-0-0122 (Gear output revolution)
    {IDN_S(0, 123),     std::uint32_t(100000)},                                                   ///< S-0-0123 (Feed constant)
    {IDN_S(0, 138),     std::uint32_t(10)},                                                       ///< S-0-0138 (Bipolar acceleration limit value)
    {IDN_S(0, 144),     std::uint16_t(0x0300)},                                                   ///< S-0-0144 (Signal status word)
    {IDN_S(0, 147),     std::uint16_t(0x0064)},                                                   ///< S-0-0147 (Homing parameter)
    {IDN_S(0, 148),     std::uint16_t(0x0000)},                                                   ///< S-0-0148 (C0600 Homing procedure command)
    {IDN_S(0, 149),     std::uint16_t(0x0000)},                                                   ///< S-0-0148 (C1300 Positive stop drive procedure command)
    {IDN_S(0, 150),     std::int32_t(0)},                                                         ///< S-0-0150 (Reference offset of encoder 1)
    {IDN_S(0, 151),     std::int32_t(0)},                                                         ///< S-0-0151 (Reference offset of encoder 2)
    {IDN_S(0, 160),     std::uint16_t(0x0001)},                                                   ///< S-0-0160 (Acceleration data scaling type - linear, preferred, m/s^3)
    {IDN_S(0, 161),     std::uint16_t(1)},                                                        ///< S-0-0161 (Linear acceleration data scaling factor)
    {IDN_S(0, 162),     std::int16_t(-7)},                                                        ///< S-0-0162 (Linear acceleration data scaling exponent)
    {IDN_S(0, 191),     std::uint16_t(0x0003)},                                                   ///< S-0-0191 (C1500 command of cancel reference point)
    {IDN_S(0, 277),     std::uint16_t(0x0140)},                                                   ///< S-0-0277 (Essential properties of encoder 1)
    {IDN_S(0, 328),     std::vector<uint16_t>{1, 0, 1}},                                          ///< S-0-0328 (Assign list signal status word)
    {IDN_S(0, 331),     std::uint16_t(0x0001)},                                                   ///< S-0-0331 (Status "n_feedback = 0")
    {IDN_S(0, 447),     std::uint16_t(0x0000)},                                                   ///< S-0-0447 (C0300 Set absolute position command)
    {IDN_S(0, 448),     std::uint16_t(0x0000)},                                                   ///< S-0-0448 (Control word of set absolute position)
    {IDN_S(0, 530),     std::uint16_t(0)},                                                        ///< S-0-0530 (Clamping torque torque/force data)
    {IDN_S(0, 533),     std::uint32_t(435)},                                                      ///< S-0-0533 (Nominal torque)
    {IDN_S(0, 326),     std::uint32_t(0xFFFFFFFD)},                                               ///< S-0-0326 (checksum)
    {IDN_S(0, 327),     std::vector<std::uint16_t>{IDN_S(0, 121), IDN_S(0, 122), IDN_S(0, 123)}}, ///< S-0-0327 (checksum idn list)
  }
{
}

void FlatbuffersHelperSoE::generateParamResponseFlatbuffer(comm::datalayer::Variant* variant, const std::uint16_t paramID)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  auto dataOffset = generateParamOffset(builder, paramID);
  auto paramResponseOffset = CreateParameterResponse(builder,
                                                     comm::ethercat::master::fbs::ElementFlags_value,
                                                     dataOffset);

  comm::ethercat::master::fbs::ParameterBuilder paramBuilder(builder);
  paramBuilder.add_response(paramResponseOffset);

  auto paramBuilderOffset = paramBuilder.Finish();
  builder.Finish(paramBuilderOffset);

  variant->copyFlatbuffers(builder);
}

void FlatbuffersHelperSoE::generateParamRequestFlatbuffer(comm::datalayer::Variant* variant, const std::uint16_t paramID, const std::uint8_t* data, const size_t& dataSize, const std::uint32_t& maxLength /*= 8*/)
{
  const std::uint8_t driveNumber = 0;
  const std::uint16_t deviceAddress = 0;

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  flatbuffers::Offset<flatbuffers::Vector<std::uint8_t>> dataOffset{};
  struct flatbuffers::Offset<comm::ethercat::master::fbs::ParameterRequest> paramRequestOffset{};

  // Write request
  if (data)
  {
    dataOffset = builder.CreateVector(data, dataSize);
    paramRequestOffset = CreateParameterRequest(builder,
                                                comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                                deviceAddress,
                                                driveNumber,
                                                comm::ethercat::master::fbs::ElementFlags_value,
                                                paramID,
                                                dataOffset,
                                                maxLength);
  }
  // Read request
  else
  {
    paramRequestOffset = CreateParameterRequest(builder,
                                                comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                                deviceAddress,
                                                driveNumber,
                                                comm::ethercat::master::fbs::ElementFlags_value,
                                                paramID,
                                                0,
                                                maxLength);
  }

  comm::ethercat::master::fbs::ParameterBuilder paramBuilder(builder);
  paramBuilder.add_request(paramRequestOffset);

  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);

  variant->copyFlatbuffers(builder);
}

HRESULT FlatbuffersHelperSoE::writeParamFlatbuffer(const comm::ethercat::master::fbs::ParameterRequest* request)
{
  auto idn = request->idn();
  auto data = request->data();
  return writeParam(idn, data->data(), data->size());
}

HRESULT FlatbuffersHelperSoE::writeParam(std::uint16_t paramID, const SoEValue& value)
{
  auto it = m_params.find(paramID);
  if (it != m_params.end())
  {
    if (value.index() != it->second.index())
    {
      return E_FAIL;
    }
    m_params[paramID] = value;
    return S_OK;
  }
  return E_FAIL;
}

HRESULT FlatbuffersHelperSoE::readParam(std::uint16_t paramID, SoEValue& value)
{
  auto it = m_params.find(paramID);
  if (it != m_params.end())
  {
    value = m_params[paramID];
    return S_OK;
  }
  return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////////
// ****************************** PRIVATE METHODS ****************************** //

inline flatbuffers::Offset<flatbuffers::Vector<std::uint8_t>> FlatbuffersHelperSoE::generateParamOffset(flatbuffers::FlatBufferBuilder& builder, std::uint16_t id) const
{
  std::uint8_t dummyData4Byte[4] {};
  if (m_params.find(id) != m_params.end())
  {
    auto& value = m_params.at(id);
    if (std::holds_alternative<std::uint16_t>(value))
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
    else if (std::holds_alternative<std::vector<uint32_t>>(value))
    {
      auto vec = std::get<std::vector<uint32_t>>(value);
      // Prepare dummy data for the array of uint32_t (4 bytes).
      size_t size = vec.size() * sizeof(uint32_t); 
      size_t length = size + 4; // additional bytes store actual length and maximum length of the list
      uint8_t* dummyData = new uint8_t[size + 4](0);
      dummyData[0] = static_cast<uint8_t>(size);
      dummyData[1] = static_cast<uint8_t>(size >> 8);
      dummyData[2] = static_cast<uint8_t>(size);
      dummyData[3] = static_cast<uint8_t>(size >> 8);
      constexpr auto len = sizeof(uint32_t);
      for (uint32_t i = 0; i < vec.size(); ++i)
      {
        dummyData[4 + len*i + 0] = static_cast<uint8_t>(vec[i]);
        dummyData[4 + len*i + 1] = static_cast<uint8_t>(vec[i] >> 8);
        dummyData[4 + len*i + 2] = static_cast<uint8_t>(vec[i] >> 16);
        dummyData[4 + len*i + 3] = static_cast<uint8_t>(vec[i] >> 24);
      }
      auto res = builder.CreateVector(dummyData, length);
      delete[] dummyData;
      dummyData = nullptr;
      return res;
    }
    else if (std::holds_alternative<std::vector<uint16_t>>(value))
    {
      auto vec = std::get<std::vector<uint16_t>>(value);
      // Prepare dummy data for the array of uint32_t (4 bytes).
      size_t size = vec.size() * sizeof(uint16_t); 
      size_t length = size + 4; // additional bytes store actual length and maximum length of the list
      uint8_t* dummyData = new uint8_t[size + 4](0);
      dummyData[0] = static_cast<uint8_t>(size);
      dummyData[1] = static_cast<uint8_t>(size >> 8);
      dummyData[2] = static_cast<uint8_t>(size);
      dummyData[3] = static_cast<uint8_t>(size >> 8);
      constexpr auto len = sizeof(uint16_t);
      for (uint32_t i = 0; i < vec.size(); ++i)
      {
        dummyData[4 + len*i + 0] = static_cast<uint8_t>(vec[i]);
        dummyData[4 + len*i + 1] = static_cast<uint8_t>(vec[i] >> 8);
      }
      auto res = builder.CreateVector(dummyData, length);
      delete[] dummyData;
      dummyData = nullptr;
      return res;
    }
  }
  return builder.CreateVector(dummyData4Byte, 2);
}

inline HRESULT FlatbuffersHelperSoE::writeParam(std::uint16_t id, const std::uint8_t* data, const size_t& dataSize)
{
  if (m_params.find(id) != m_params.end())
  {
    auto& value = m_params.at(id);
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
  }
  return E_FAIL;
}
