#pragma once

/// @file
///
/// This header file is part of the axisprofile library and implements the
/// helper method for device flatbuffer data
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "comm/ethercat/master/fbs/slave_config_info_generated.h"
#include "fieldbuses/fieldbus_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
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

  /// @brief This is device flatbuffer data helper class
  class DeviceFbsDataHelper
  {
  public: // method

    /// @brief Constructor
    DeviceFbsDataHelper() = default;

    /// @brief Destructor
    virtual ~DeviceFbsDataHelper() noexcept = default;

    /// @brief  This method generates the address request.
    /// @param[in]      deviceAddress       Struct containing device address for the read request
    /// @param[out]     variant             Variant address where the created flatbuffer will be stored.
    /// @return         Return value indicating success or error.
    static AxisProfileResult generateAddressRequest(std::uint32_t deviceAddress, comm::datalayer::Variant& variant);

    /// @brief  This method generates the flatbuffer to read the state of a slave.
    /// @param[out]   variant           Variant address where the created flatbuffer will be stored.
    /// @param[in]    readRequestData   Struct containing all information for the read request
    /// @return       Return value indicating success or error.
    static AxisProfileResult generateReadSlaveStateFlatbuffer(comm::datalayer::Variant* variant, const ReadSlaveStateData& readRequestData);

    /// @brief  This method generates the flatbuffer to read the status of a slave.
    /// @param[out]   variant           Variant address where the created flatbuffer will be stored.
    /// @param[in]    readRequestData   Struct containing all information for the read request
    /// @return       Return value indicating success or error.
    static AxisProfileResult generateReadSlaveStatusFlatbuffer(comm::datalayer::Variant* variant, const ReadSlaveStateData& readRequestData);

    /// @brief  This method generates the flatbuffer to read the distribution clock config info of a slave.
    /// @param[out]   variant           Variant address where the created flatbuffer will be stored.
    /// @param[in]    deviceAddress     Struct containing device address for the read request
    /// @return       Return value indicating success or error.
    static AxisProfileResult generateReadSlaveDcCfgInfoFlatbuffer(comm::datalayer::Variant* variant, std::uint32_t deviceAddress);

    /// @brief  This method generates fbs for requesting slave address mapping.
    /// @param[out]     variant             Variant address where the created flatbuffer will be stored.
    /// @return         Return value indicating success or error.
    static AxisProfileResult generateSlaveAddressMappingRequestFlatbuffer(comm::datalayer::Variant& variant);

    /// @brief  This method extract the cfged slave name.
    /// @param[in]    slaveInfoData         The data extract source 
    /// @param[out]   deviceName            The device name extracted
    /// @param[out]   devInfo               The device hardware infomation include protocol etc.
    /// @return       Return value indicating success or error.
    static AxisProfileResult extractSlaveCfgInfo(comm::datalayer::Variant& slaveInfoData, std::string& deviceName, DeviceHardwareInfo& devInfo);

    /// @brief  This method extract the slave distribution clock config information
    /// @param[in]    slaveDcCfgInfo        The data extract source
    /// @param[out]   dcCfgInfo             The extracted slave distribution clock config information
    /// @return       Return value indicating success or error.
    static AxisProfileResult extractSlaveDcCfgInfo(comm::datalayer::Variant& slaveDcCfgInfo, DcConfigInfo& dcCfgInfo);

    /// @brief  This method extract the slave address mapping
    /// @param[in]    var                      The data extract source
    /// @param[out]   addressArray             The extracted slave address array
    /// @return       Return value indicating success or error.
    static AxisProfileResult extractSlaveAddressByMapping(const comm::datalayer::Variant& var, std::vector<std::uint32_t>& addressArray);
  };
}
