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
#include "profiles/profile_defs.h"
#include "diagnosis/dia.h"
#include <limits>
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

    /// @brief Get EtherCAT name of the given uri string.
    /// @param[in] uri The RT memory uri
    /// @return The const pointer to string of EtherCAT name
    static std::string_view getEtherCATNameOfURI(std::string_view uri)
    {
      std::string_view base = DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES;

      size_t pos = uri.find(base);
      if (pos == std::string_view::npos)
      {
        return {};
      }

      pos += base.size();

      // skip optional '/'
      if (pos < uri.size() && uri[pos] == '/')
      {
        ++pos;
      }

      size_t end = uri.find('/', pos);

      return (end == std::string_view::npos) ? uri.substr(pos) : uri.substr(pos, end - pos);
    }

    /// @brief Get the EtherCAT master instance name from the RT memory buffer URI.
    /// @param[in] inputURI   The const pointer to string of input RT memory.
    /// @param[in] outputURI  The const pointer to string of output RT memory.
    /// @return The const pointer to string of EtherCAT name
    static std::string_view extractEtherCATNameFromBufString(const char* inputURI,
                                                             const char* outputURI)
    {
      if (inputURI && *inputURI)
      {
        return getEtherCATNameOfURI(inputURI);
      }

      if (outputURI && *outputURI)
      {
        return getEtherCATNameOfURI(outputURI);
      }
      return {};
    }

    /// @brief Generate input/output buffer string with EtherCAT instance name.
    ///        return empty string when name is empty
    /// @param[in] ethercatName The EtherCAT instance name.
    /// @param[in] buff         Input or output buffer
    /// @return The corresponding buffer string
    static std::string generateRTBufferString(const std::string& ethercatName, BufferOrient buff)
    {
      if (ethercatName.empty())
      {
        return {};
      }
      std::string uri = DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + "/" + ethercatName;
      if (BufferOrient::INPUT_BUFFER == buff)
      {
        uri += "/realtime_data/input";
      }
      else
      {
        uri += "/realtime_data/output";
      }
      return uri;
    }

    /// @brief This method reads the parameter list from the device using CoE protocol and stores it in the output list.
    /// @tparam T                     the type of the parameter list elements
    /// @param[in]  profileBasicInfo  the basic information of the profile, including device address and fieldbus adapter
    /// @param[in]  obj               the object directory entry of the parameter to read
    /// @param[in]  maxElements       the maximum number of elements to read from the device
    /// @param[out] outList           the output list to store the read parameters
    /// @param[in]  convertFunc       the function to convert list type to raw data
    /// @return returns an AxisProfileResult indicating the success or failure of the read operation
    template <typename T>
    static AxisProfileResult readParamListCoEImpl(ProfileBasicInfo& profileBasicInfo,
                                                  const ObjectDirEntry& obj,
                                                  std::size_t maxElements,
                                                  std::vector<T>& outList,
                                                  std::function<void(const std::uint8_t*, T&)> convertFunc = nullptr)
    {
      outList.clear();

      constexpr std::size_t cellSize = sizeof(T);
      constexpr std::size_t maxRawSize = static_cast<std::size_t>((std::numeric_limits<std::uint32_t>::max)());
      if (maxElements > maxRawSize / cellSize)
      {
        return AxisProfileResult::ERR_WRONGDATASIZE;
      }

      const std::size_t rawSize = maxElements * cellSize;
      std::vector<std::uint8_t> rawBuffer(rawSize);

      ParamReqDataCoE requestData {profileBasicInfo.m_deviceAddress, obj};
      requestData.maxLen = static_cast<std::uint32_t>(rawSize);

      std::size_t readLen {};
      AxisProfileResult res = profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, rawBuffer.data(), rawBuffer.size(), readLen);
      if (aprStatusFailed(res))
      {
        Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_READ_PARAM_FAILED,
                           profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                           "CoE: %s, error: 0x%08X",
                           objDirToString(requestData.objEntry).c_str(), res);
        return res;
      }

      return FieldbusHelper::rawToParamList<T>(rawBuffer.data(), readLen, outList, convertFunc);
    }
  };
}
