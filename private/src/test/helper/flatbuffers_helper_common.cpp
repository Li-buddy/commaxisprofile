// ---------------------------------------------------------------------------
//
//! @file       flatbuffers_helper_commom.cpp
//!
//! @author     DC-AE/ENG6-CN
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2021
//
// ---------------------------------------------------------------------------
//
//! This source file is part of the axisprofile library and declares a helper
//! to generate different flatbuffers.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "flatbuffers_helper_common.h"
#include "helper/device_fbs_data_helper.h"
#include "axisprofile_internal_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace test;

void FlatbuffersHelperCommon::generateSlaveStateResponseFlatbuffer(comm::datalayer::Variant* const variant)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  auto slaveStateResponseOffset = CreateSlaveStateResponse(builder,
                                                          m_slaveState);

  comm::ethercat::master::fbs::SlaveStateBuilder slaveStateBuilder(builder);
  slaveStateBuilder.add_response(slaveStateResponseOffset);
  auto slaveStateBuilderOffset = slaveStateBuilder.Finish();
  builder.Finish(slaveStateBuilderOffset);

  variant->copyFlatbuffers(builder);
}

void FlatbuffersHelperCommon::generateMasterStateResponseFlatbuffer(comm::datalayer::Variant* const variant)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  auto masterStateRequestOffset = CreateMasterStateResponse(builder,
                                                           m_masterState);

  comm::ethercat::master::fbs::MasterStateBuilder masterStateBuilder(builder);
  masterStateBuilder.add_response(masterStateRequestOffset);
  auto masterStateBuilderOffset = masterStateBuilder.Finish();
  builder.Finish(masterStateBuilderOffset);

  variant->copyFlatbuffers(builder);
}

void FlatbuffersHelperCommon::generateSlaveStateRequestFlatbuffer(comm::datalayer::Variant* const variant, const std::uint16_t deviceAddress)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  auto slaveStateRequestOffset = CreateSlaveStateRequest(builder,
                                                         comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                                         deviceAddress);

  comm::ethercat::master::fbs::SlaveStateBuilder slaveStateBuilder(builder);
  slaveStateBuilder.add_request(slaveStateRequestOffset);
  auto slaveStateBuilderOffset = slaveStateBuilder.Finish();
  builder.Finish(slaveStateBuilderOffset);

  variant->copyFlatbuffers(builder);
}

void FlatbuffersHelperCommon::generateSlaveStatusRequestFlatbuffer(comm::datalayer::Variant* const variant, const std::uint16_t deviceAddress)
{
  comm::axisprofile::ReadSlaveStateData readRequestData;
  readRequestData.m_deviceAddress = deviceAddress;
  comm::axisprofile::DeviceFbsDataHelper::generateReadSlaveStatusFlatbuffer(variant, readRequestData);
}

void FlatbuffersHelperCommon::generateSlaveStatusResponseFlatbuffer(comm::datalayer::Variant* const variant)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  
  auto slaveStatusRequestOffset = comm::ethercat::master::fbs::CreateSlaveStatusResponse(builder, m_slaveStatus.m_status);

  comm::ethercat::master::fbs::SlaveStatusBuilder statusBuilder(builder);
  statusBuilder.add_response(slaveStatusRequestOffset);
  auto slaveStateBuilderOffset = statusBuilder.Finish();
  builder.Finish(slaveStateBuilderOffset);

  variant->copyFlatbuffers(builder);
}

void FlatbuffersHelperCommon::generateMasterStatusResponseFlatbuffer(comm::datalayer::Variant* const variant)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  auto statusRequestOffset = comm::ethercat::master::fbs::CreateMasterStatusResponse(builder, m_masterStatus.m_status);

  comm::ethercat::master::fbs::MasterStatusBuilder statusBuilder(builder);
  statusBuilder.add_response(statusRequestOffset);
  auto masterStateBuilderOffset = statusBuilder.Finish();
  builder.Finish(masterStateBuilderOffset);

  variant->copyFlatbuffers(builder);
}

void FlatbuffersHelperCommon::generateSlaveDcCfgInfoResponseFlatbuffer(comm::datalayer::Variant* const variant)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  comm::ethercat::master::fbs::DcSlaveConfigInfoResponseBuilder respBuilder(builder);
  respBuilder.add_dcSupport(m_dcSupported);
  respBuilder.add_ShiftTime(m_shiftTimeNs);
  respBuilder.add_CycleTime0(m_cycleTimeNs);
  auto respOffset = respBuilder.Finish();

  comm::ethercat::master::fbs::DcSlaveConfigInfoBuilder dcSlaveCfgBuilder(builder);
  dcSlaveCfgBuilder.add_response(respOffset);
  auto dcSlaveCfgOffset = dcSlaveCfgBuilder.Finish();
  builder.Finish(dcSlaveCfgOffset);

  variant->copyFlatbuffers(builder);
}

void FlatbuffersHelperCommon::generateAddressMappingResponseFlatbuffer(std::uint16_t addressCnt, comm::datalayer::Variant& mappingResponse)
{
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<comm::ethercat::master::fbs::AddressMappingEntry>> entries;
  for (int i = 0; i < addressCnt; ++i)
  {
    auto entryAddr = comm::ethercat::master::fbs::CreateAddressedRequest(builder, comm::ethercat::master::fbs::Addresstype_fixedphysical, 1001 + i);
    auto entryName = builder.CreateString("SLAVE_" + std::to_string(i));

    auto entry = comm::ethercat::master::fbs::CreateAddressMappingEntry(builder, entryAddr, entryName);
    entries.push_back(entry);
  }

  auto entryList = builder.CreateVector(entries);
  auto response = comm::ethercat::master::fbs::CreateSlaveAddressMappingResponse(builder, entryList);

  comm::ethercat::master::fbs::SlaveAddressMappingBuilder mappingBuilder(builder);
  mappingBuilder.add_response(response);
  auto mappingRoot = mappingBuilder.Finish();

  builder.Finish(mappingRoot);
  mappingResponse.copyFlatbuffers(builder);
}

void FlatbuffersHelperCommon::generateSlaveCfgInfoResponseFlatbuffer(std::string& deviceName,
                                                                    comm::axisprofile::DeviceHardwareInfo& devInfo,
                                                                    comm::datalayer::Variant* variant)
{
  flatbuffers::FlatBufferBuilder totalBuilder;
  auto slavenameOffset = totalBuilder.CreateString(deviceName);

  comm::ethercat::master::fbs::EthercatIdentityInfo identityInfo(devInfo.m_vendorId, devInfo.m_productCode, 3, 0);
  
  comm::ethercat::master::fbs::SlaveConfigInfoResponseBuilder slaveResponseBuilder(totalBuilder);
  slaveResponseBuilder.add_ethercatAddr(devInfo.m_address);
  slaveResponseBuilder.add_slaveName(slavenameOffset);
  slaveResponseBuilder.add_mbxProtocols(devInfo.m_supportedProtocol);
  slaveResponseBuilder.add_slaveIdentity(&identityInfo);
  auto slaveResponseOffset = slaveResponseBuilder.Finish();

  comm::ethercat::master::fbs::SlaveConfigInfoBuilder infoBuilder(totalBuilder);
  infoBuilder.add_response(slaveResponseOffset);
  auto slaveInfoOffset = infoBuilder.Finish();
  totalBuilder.Finish(slaveInfoOffset);

  variant->copyFlatbuffers(totalBuilder);
}
