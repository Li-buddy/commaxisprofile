/// @file
///
/// @brief This file is part of the axisprofile library and implements the
/// fieldbus adapter for Canopen over Ethercat (CoE).
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/ethercat/master/fbs/sdo_generated.h"
#include "fieldbus_adapter_coe.h"
#include "fieldbus_helper.h"

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

comm::axisprofile::FieldbusAdapterCoE::FieldbusAdapterCoE(ProfileTrace* const trace) : FieldbusAdapter(trace)
{
  m_getParamResponseDataCheck.emplace_back(
    &ERROR_FB_VERIFICATION,                                            // trace error message
    AxisProfileResult::ERR_DATALAYER_WRONG_TYPE,                       // return type in case of error
    [] (const comm::datalayer::Variant* variantData)
    {
      return comm::datalayer::STATUS_FAILED(variantData->verifyFlatbuffers(comm::ethercat::master::fbs::VerifySDOBuffer));
    }  //lambda
  );
  m_getParamResponseDataCheck.emplace_back(
    &ERROR_FB_RESPONSE,                                               // trace error message
    AxisProfileResult::ERR_FIELDBUS,                                  // return type in case of error
    [] (const comm::datalayer::Variant* variantData)
    {
      const auto sdoData = comm::ethercat::master::fbs::GetSDO(variantData->getData());
      return flatbuffers::IsFieldPresent(sdoData, comm::ethercat::master::fbs::SDO::VT_REQUEST);
    }    // lambda
  );
}

void FieldbusAdapterCoE::setFieldbusMasterAddress(const std::string& address)
{
  // Update base address string
  FieldbusAdapter::setFieldbusMasterAddress(address);

  // Update SOE parameter request address
  m_fieldbusSDORequestAddress = m_fieldbusMasterDatalayerAddress + DL_SUB_ADDRESS_COE_SDO;
}

AxisProfileResult FieldbusAdapterCoE::getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen)
{
  for(const auto& check : m_getParamResponseDataCheck)
  {
    if(check.m_fnct(variantData))
    {
      APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, check.m_traceMessage->c_str()));
      return check.m_returnResult;
    }
  }
  auto responseData = comm::ethercat::master::fbs::GetSDO(variantData->getData())->response();
  if (flatbuffers::IsFieldPresent(comm::ethercat::master::fbs::GetSDO(variantData->getData()), comm::ethercat::master::fbs::SDO::VT_RESPONSE)
  && flatbuffers::IsFieldPresent(responseData, comm::ethercat::master::fbs::SDOResponse::VT_DATA))
  {
    auto data = responseData->data();
    auto rawData = data->Data();
    auto size = data->size();
    
    if (bufLen < size)
    {
      APR_TRACE_ERROR("%s; Received FB has wrong data size! Received length: %d Buffer length: %d", m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS), size, bufLen);
      return AxisProfileResult::ERR_FIELDBUS;
    }
    std::memcpy(bufPtr, rawData, size);
    readLen = size;

    APR_TRACE_MESSAGE("%s; CoE-Read value %d bytes: %s",
                      m_profileTrace->createTraceMsg(), readLen,
                      FieldbusHelper::rawDataToHexDump(rawData, size).c_str());
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapterCoE::readSingleParamAsync(ParameterResponseCallbackData* const responseCallbackData, const ParamReqDataBase& readData)
{
  if (!responseCallbackData || !m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  const ParamReqDataCoE* readDataCoE = static_cast<const ParamReqDataCoE*>(&readData);
  generateParamReadRequestFlatbuffer(m_fbReadBuilder, &responseCallbackData->m_requestVariant, *readDataCoE);
  responseCallbackData->m_requestFinishedFlag = false;
  responseCallbackData->m_dlResult = comm::datalayer::DlResult::DL_OK;

  APR_TRACE_MESSAGE("%s; CoE-Read (ASYNC): Object %s, Address %d",
                    m_profileTrace->createTraceMsg(),
                    objDirToString(readDataCoE->objEntry).c_str(),
                    readDataCoE->address);
  return FieldbusAdapter::executeDatalayerReadAsync(responseCallbackData, m_fieldbusSDORequestAddress);
}

AxisProfileResult FieldbusAdapterCoE::writeSingleParamAsync(ParameterResponseCallbackData* responseCallbackData, const ParamReqDataBase& writeData)
{
  if (!responseCallbackData || !m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  const ParamReqDataCoE* writeDataCoE = static_cast<const ParamReqDataCoE*>(&writeData);
  generateParamWriteRequestFlatbuffer(m_fbWriteBuilder, &responseCallbackData->m_requestVariant, *writeDataCoE);
  responseCallbackData->m_requestFinishedFlag = false;
  responseCallbackData->m_dlResult = comm::datalayer::DlResult::DL_OK;

  APR_TRACE_MESSAGE("%s; CoE-Write (ASYNC): Object %s, Address %d, value: %s",
                    m_profileTrace->createTraceMsg(),
                    objDirToString(writeDataCoE->objEntry).c_str(),
                    writeDataCoE->address,
                    FieldbusHelper::rawDataToHexDump(writeDataCoE->writeBufPtr, writeDataCoE->writeBufLen).c_str());
  return FieldbusAdapter::executeDatalayerWriteAsync(responseCallbackData, m_fieldbusSDORequestAddress);
}

AxisProfileResult FieldbusAdapterCoE::readSingleParamSync(const ParamReqDataBase& readData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen)
{
  if (!m_datalayerClient || !bufPtr)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant requestVariant{};
  const ParamReqDataCoE* readDataCoE = static_cast<const ParamReqDataCoE*>(&readData);
  (void)generateParamReadRequestFlatbuffer(m_fbReadBuilder, &requestVariant, *readDataCoE);

  APR_TRACE_MESSAGE("%s; CoE-Read (SYNC): Object %s, Address %d",
                    m_profileTrace->createTraceMsg(),
                    objDirToString(readDataCoE->objEntry).c_str(),
                    readDataCoE->address);
  AxisProfileResult result = FieldbusAdapter::executeDatalayerReadSync(&requestVariant, m_fieldbusSDORequestAddress);
  if (aprStatusFailed(result))
  {
    return result;
  }
  return getParamResponseData(&requestVariant, bufPtr, bufLen, readLen);
}

AxisProfileResult FieldbusAdapterCoE::writeSingleParamSync(const ParamReqDataBase& writeData)
{
  if (!m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL, "Datalayer Client"));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant requestVariant{};
  const ParamReqDataCoE* writeDataCoE = static_cast<const ParamReqDataCoE*>(&writeData);
  (void)generateParamWriteRequestFlatbuffer(m_fbWriteBuilder, &requestVariant, *writeDataCoE);

  APR_TRACE_MESSAGE("%s; CoE-Write (SYNC): Object %s, Address %d, value: %s",
                    m_profileTrace->createTraceMsg(),
                    objDirToString(writeDataCoE->objEntry).c_str(),
                    writeDataCoE->address,
                    FieldbusHelper::rawDataToHexDump(writeDataCoE->writeBufPtr, writeDataCoE->writeBufLen).c_str());
  return FieldbusAdapter::executeDatalayerWriteSync(&requestVariant, m_fieldbusSDORequestAddress);
}

///////////////////////////////////////////////////////////////////////////////////
// ****************************** PRIVATE METHODS ****************************** //

AxisProfileResult FieldbusAdapterCoE::generateParamReadRequestFlatbuffer(flatbuffers::FlatBufferBuilder& builder, comm::datalayer::Variant* variant, const ParamReqDataCoE& readRequestData)
{
  builder.Clear();
  builder.ForceDefaults(true);

  auto sdoRequestOffset = CreateSDORequest(builder,
                                           comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                           readRequestData.address,
                                           readRequestData.objEntry.index,
                                           readRequestData.objEntry.subIndex,
                                           comm::ethercat::master::fbs::SDOFlags_noFlags,
                                           0,
                                           readRequestData.maxLen);

  comm::ethercat::master::fbs::SDOBuilder sdoBuilder(builder);
  sdoBuilder.add_request(sdoRequestOffset);
  auto exchangeData = sdoBuilder.Finish();
  builder.Finish(exchangeData);

  variant->shareFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapterCoE::generateParamWriteRequestFlatbuffer(flatbuffers::FlatBufferBuilder& builder, comm::datalayer::Variant* variant, const ParamReqDataCoE& writeRequestData)
{
  builder.Clear();
  builder.ForceDefaults(true);

  auto dataOffset = builder.CreateVector(writeRequestData.writeBufPtr, writeRequestData.writeBufLen);
  auto sdoRequestOffset = CreateSDORequest(builder,
                                           comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                           writeRequestData.address,
                                           writeRequestData.objEntry.index,
                                           writeRequestData.objEntry.subIndex,
                                           comm::ethercat::master::fbs::SDOFlags_noFlags,
                                           dataOffset,
                                           writeRequestData.maxLen);
 
  comm::ethercat::master::fbs::SDOBuilder sdoBuilder(builder);
  sdoBuilder.add_request(sdoRequestOffset);
  auto exchangeData = sdoBuilder.Finish();
  builder.Finish(exchangeData);

  variant->shareFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}
