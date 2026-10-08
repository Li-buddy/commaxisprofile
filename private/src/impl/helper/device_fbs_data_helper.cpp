/// @file
///
/// This source file is part of the axisprofile library and implements the
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

#include "comm/datalayer/datalayer.h"
#include "comm/ethercat/master/fbs/dc_slave_config_info_generated.h"
#include "comm/ethercat/master/fbs/slave_address_mapping_generated.h"
#include "comm/ethercat/master/fbs/slave_state_generated.h"
#include "comm/ethercat/master/fbs/slave_status_generated.h"
#include "comm/ethercat/master/fbs/slave_config_info_generated.h"
#include "device_fbs_data_helper.h"

// --------------------------------------------------------------------------- 
// Name Spaces
// --------------------------------------------------------------------------- 
using namespace comm::axisprofile;

// --------------------------------------------------------------------------- 
// Declaration of constants 
// --------------------------------------------------------------------------- 

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

AxisProfileResult DeviceFbsDataHelper::generateAddressRequest(std::uint32_t deviceAddress, comm::datalayer::Variant& variant)
{
  flatbuffers::FlatBufferBuilder builder {};
  builder.ForceDefaults(true);

  auto addressRequestOffset = CreateAddressedRequest(builder,
                                                     comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                                     deviceAddress);
  comm::ethercat::master::fbs::SlaveConfigInfoBuilder slaveCfgInfoBuilder(builder);
  slaveCfgInfoBuilder.add_request(addressRequestOffset);
  auto slaveCfgInfoOffset = slaveCfgInfoBuilder.Finish();
  builder.Finish(slaveCfgInfoOffset);

  variant.copyFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceFbsDataHelper::generateSlaveAddressMappingRequestFlatbuffer(comm::datalayer::Variant& variant)
{
  flatbuffers::FlatBufferBuilder builder {};
  auto addressRequestOffset = comm::ethercat::master::fbs::CreateSlaveAddressMappingRequest(builder);
  comm::ethercat::master::fbs::SlaveAddressMappingBuilder mapping(builder);

  mapping.add_request(addressRequestOffset);
  auto slaveCfgInfoOffset = mapping.Finish();
  builder.Finish(slaveCfgInfoOffset);
  variant.copyFlatbuffers(builder);

  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceFbsDataHelper::generateReadSlaveStateFlatbuffer(comm::datalayer::Variant* variant, const ReadSlaveStateData& readRequestData)
{
  flatbuffers::FlatBufferBuilder builder {};
  builder.ForceDefaults(true);

  auto slaveStateRequestOffset = CreateSlaveStateRequest(builder, comm::ethercat::master::fbs::Addresstype_fixedphysical, readRequestData.m_deviceAddress);

  comm::ethercat::master::fbs::SlaveStateBuilder slaveStateBuilder(builder);
  slaveStateBuilder.add_request(slaveStateRequestOffset);
  auto slaveStateBuilderOffset = slaveStateBuilder.Finish();
  builder.Finish(slaveStateBuilderOffset);

  variant->copyFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceFbsDataHelper::generateReadSlaveStatusFlatbuffer(comm::datalayer::Variant* variant, const ReadSlaveStateData& readRequestData)
{
  flatbuffers::FlatBufferBuilder builder {};
  builder.ForceDefaults(true);

  auto addressRequestOffset = CreateAddressedRequest(builder,
                                                     comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                                     readRequestData.m_deviceAddress);
  comm::ethercat::master::fbs::SlaveStatusBuilder slaveStatusBuilder(builder);
  slaveStatusBuilder.add_request(addressRequestOffset);
  auto slaveStatusBuilderOffset = slaveStatusBuilder.Finish();
  builder.Finish(slaveStatusBuilderOffset);

  variant->copyFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceFbsDataHelper::generateReadSlaveDcCfgInfoFlatbuffer(comm::datalayer::Variant* variant, std::uint32_t deviceAddress)
{
  flatbuffers::FlatBufferBuilder builder {};
  builder.ForceDefaults(true);

  auto addressRequestOffset = CreateAddressedRequest(builder,
                                                     comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                                     deviceAddress);
  comm::ethercat::master::fbs::DcSlaveConfigInfoBuilder dcSlaveCfgInfoBuilder(builder);
  dcSlaveCfgInfoBuilder.add_request(addressRequestOffset);
  auto dcSlaveCfgInfoOffset = dcSlaveCfgInfoBuilder.Finish();
  builder.Finish(dcSlaveCfgInfoOffset);

  variant->copyFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceFbsDataHelper::extractSlaveCfgInfo(comm::datalayer::Variant& slaveInfoData, std::string& deviceName, DeviceHardwareInfo& devInfo)
{
  //extract slave information via fbs
  if (comm::datalayer::VariantType::FLATBUFFERS != slaveInfoData.getType())
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Verify the flatbuffer
  flatbuffers::Verifier verifier(slaveInfoData.getData(), slaveInfoData.getSize());
  if (!comm::ethercat::master::fbs::VerifySlaveConfigInfoBuffer(verifier))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  auto offset = comm::ethercat::master::fbs::GetSlaveConfigInfo(slaveInfoData.getData());
  if (!flatbuffers::IsFieldPresent(offset, comm::ethercat::master::fbs::SlaveConfigInfo::VT_RESPONSE) ||
      !flatbuffers::IsFieldPresent(offset->response(), comm::ethercat::master::fbs::SlaveConfigInfoResponse::VT_SLAVENAME) ||
      !flatbuffers::IsFieldPresent(offset->response(), comm::ethercat::master::fbs::SlaveConfigInfoResponse::VT_MBXPROTOCOLS) ||
      !flatbuffers::IsFieldPresent(offset->response(), comm::ethercat::master::fbs::SlaveConfigInfoResponse::VT_SLAVEIDENTITY)
      )
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  deviceName = offset->response()->slaveName()->c_str();
  devInfo.m_supportedProtocol = offset->response()->mbxProtocols();

  auto slaveIdentityFb = offset->response()->slaveIdentity();  // slaveIdentity is a struct, never null
  devInfo.m_vendorId = slaveIdentityFb->vendorId();
  devInfo.m_productCode = slaveIdentityFb->productCode();

  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceFbsDataHelper::extractSlaveDcCfgInfo(comm::datalayer::Variant& slaveDcCfgInfo, DcConfigInfo& dcCfgInfo)
{
  //extract slave information via fbs
  if (comm::datalayer::VariantType::FLATBUFFERS != slaveDcCfgInfo.getType())
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  flatbuffers::Verifier verifier(slaveDcCfgInfo.getData(), slaveDcCfgInfo.getSize());
  if (!comm::ethercat::master::fbs::VerifyDcSlaveConfigInfoBuffer(verifier))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  auto offset = comm::ethercat::master::fbs::GetDcSlaveConfigInfo(slaveDcCfgInfo.getData());
  if (!flatbuffers::IsFieldPresent(offset, comm::ethercat::master::fbs::DcSlaveConfigInfo::VT_RESPONSE))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  dcCfgInfo.m_dcSupported = offset->response()->dcSupport();
  dcCfgInfo.m_shiftTimeNs = offset->response()->ShiftTime();
  dcCfgInfo.m_cycleTimeNs = offset->response()->CycleTime0();
  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceFbsDataHelper::extractSlaveAddressByMapping(const comm::datalayer::Variant& var, std::vector<std::uint32_t>& addressArray)
{
  if (comm::datalayer::VariantType::FLATBUFFERS != var.getType())
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  flatbuffers::Verifier verifier(var.getData(), var.getSize());
  if (!comm::ethercat::master::fbs::VerifySlaveAddressMappingBuffer(verifier))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  addressArray.clear();
  auto root = comm::ethercat::master::fbs::GetSlaveAddressMapping(var.getData());
  auto response = root->response();
 
  if (response == nullptr || response->AddressMappingEntryList() == nullptr)
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  for (const auto& entry : *response->AddressMappingEntryList())
  {
    if (entry == nullptr || entry->address() == nullptr)
    {
      continue;
    }
    uint16_t addr = entry->address()->address();
    addressArray.push_back(static_cast<uint32_t>(addr));
  }

  return AxisProfileResult::STS_OK;
}
