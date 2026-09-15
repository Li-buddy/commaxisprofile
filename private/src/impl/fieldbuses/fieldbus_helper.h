#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements helper methods for fieldbus adapter.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "fieldbus_defs.h"
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // --------------------------------------------------------------------------- 
  constexpr std::size_t LIST_BUFF_DEFAULT_SIZE = 64;    ///< Default buffer size for parameter list access

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This class implements helper methods for fieldbus adapter
  class FieldbusHelper
  {
  public: // methods

    /// @brief Constructor
    FieldbusHelper() = default;
    /// @brief Destructor
    virtual ~FieldbusHelper() noexcept = default;

    /// @brief This method converts raw data to hex dump for logging
    /// @param[in]  data        buffer pointer of the data
    /// @param[in]  len         buffer length of the data
    /// @return The hex string of the data
    static std::string rawDataToHexDump(const std::uint8_t* data, std::size_t len)
    {
      if (len == 0)
      {
        return {};
      }

      // Each byte -> "XX " (3 characters), the last byte does not need a space
      std::string out;
      out.resize(len * 3 - 1);

      static const char hexDigits[] = "0123456789ABCDEF";

      for (size_t i = 0; i < len; ++i)
      {
        unsigned char c = data[i];
        size_t pos = i * 3;
        out[pos] = hexDigits[c >> 4];       // High 4 bits
        out[pos + 1] = hexDigits[c & 0x0F]; // Low 4 bits
        if (i != len - 1)
        {
          out[pos + 2] = '|';               // Separator
        }
      }

      return out;
    }

    /// @brief This method parses the raw data of fieldbus to the parameter list. Used for CoE adapter
    /// @tparam[in] T           The type of output list
    /// @param[in]  rawData     The pointer of raw data to parse
    /// @param[in]  rawSize     The size of the raw data to parse
    /// @param[out] outList     The parsed parameter list
    /// @param[in]  convertFunc The function to convert raw data to the desired type
    /// @return    The result of parsing
    template <typename T>
    static AxisProfileResult rawToParamList(const std::uint8_t* rawData,
                                            std::size_t rawSize,
                                            std::vector<T>& outList,
                                            std::function<void(const std::uint8_t*, T&)> convertFunc = nullptr)
    {
      if (rawData == nullptr)
      {
        return AxisProfileResult::ERR_EXEC_NULL;
      }

      const std::size_t cellSize = sizeof(T); // each data length in bytes    
      std::size_t cellNum = rawSize / cellSize; // number of cells in the raw data

      if (cellSize * cellNum != rawSize)
      {
        return AxisProfileResult::ERR_WRONGDATASIZE;
      }

      outList.clear();
      outList.reserve(cellNum);
      for (std::size_t i = 0; i < cellNum; i++)
      {
        const std::uint8_t* ptr = rawData + i * cellSize;

        T value {};
        if (convertFunc)
        {
          convertFunc(ptr, value);
        }
        else
        {
          value = *reinterpret_cast<const T*>(ptr);
        }
        outList.push_back(value);
      }

      return AxisProfileResult::STS_OK;
    }

    /// @brief This method parses the raw data to the output list of SoE adapter.
    /// @tparam[in] T           The type of output list
    /// @param[in]  rawData     The pointer of raw data to parse
    /// @param[in]  rawSize     The size of the raw data to parse
    /// @param[out] outList     The parsed output list
    /// @param[in]  convertFunc The function to convert raw data to the desired type
    /// @return    The result of parsing
    template <typename T>
    static AxisProfileResult rawToParamListSoE(const std::uint8_t* rawData,
                                               std::size_t rawSize,
                                               std::vector<T>& outList,
                                               std::function<void(const std::uint8_t*, T&)> convertFunc = nullptr)
    {
      if (rawData == nullptr)
      {
        return AxisProfileResult::ERR_EXEC_NULL;
      }

      // EtherCAT SoE list parameter minimum size is 4 bytes
      // Header Byte[1..0]: valid bytes in the raw data, start from Byte4
      // Header Byte[3..2]: maximum bytes device supports
      // Valid Data Byte[4..n]: data list
      if (rawSize < 4)
      {
        // can't happen on real EtherCAT Master
        return AxisProfileResult::ERR_WRONGDATASIZE;
      }

      // valid length of raw data in bytes
      std::size_t validLen = *reinterpret_cast<const std::uint16_t*>(rawData);
      if (validLen + 4 > rawSize)
      {
        return AxisProfileResult::ERR_WRONGDATASIZE;
      }

      return rawToParamList<T>(rawData + 4, validLen, outList, convertFunc);
    }

    /// @brief This method parses the list to the raw data of fieldbus. Used for CoE adapter
    /// @tparam[in] T           The type of output list
    /// @param[in]  inList      The pointer of raw data to parse
    /// @param[out] outList     The parsed output list
    /// @param[in]  convertFunc The function to convert list type to raw data
    /// @return    The result of parsing
    template <typename T>
    static AxisProfileResult paramListToRaw(const std::vector<T>& inList,
                                            std::vector<std::uint8_t>& outRaw,
                                            std::function<void(const T&, std::uint8_t*)> convertFunc = nullptr)
    {
      outRaw.clear();

      std::size_t cellSize = sizeof(T);
      std::size_t rawSize = inList.size() * cellSize;
      outRaw.resize(rawSize);

      for (std::size_t i = 0; i < inList.size(); ++i)
      {
        std::uint8_t* ptr = outRaw.data() + i * cellSize;

        if (convertFunc)
        {
          convertFunc(inList[i], ptr);
        }
        else
        {
          const T& v = inList[i];
          std::memcpy(ptr, &v, cellSize);
        }
      }

      return AxisProfileResult::STS_OK;
    }

    /// @brief This method parses the list to the raw data of fieldbus. Used for SoE adapter
    /// @tparam[in] T           The type of output list
    /// @param[in]  inList      The pointer of raw data to parse
    /// @param[out] outList     The parsed output list
    /// @param[in]  convertFunc The function to convert list type to raw data
    /// @return    The result of parsing
    template <typename T>
    static AxisProfileResult paramListToRawSoE(const std::vector<T>& inList,
                                               std::vector<std::uint8_t>& outRaw,
                                               std::function<void(const T&, std::uint8_t*)> convertFunc = nullptr)
    {
      auto res = paramListToRaw(inList, outRaw, convertFunc);
      if (aprStatusFailed(res))
      {
        return res;
      }

      std::uint16_t validLen = static_cast<std::uint16_t>(outRaw.size());
      outRaw.insert(outRaw.begin(), {0, 0, 0, 0});
      std::memcpy(outRaw.data(), &validLen, sizeof(std::uint16_t));

      return AxisProfileResult::STS_OK;
    }
  };
}
