/// @file
///
/// @brief This file is part of the axisprofile library and implements the
/// fieldbus adapter for Servodrive over Ethercat (SoE).
///
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "fieldbus_adapter_soe.h"
#include "fieldbus_helper.h"
#include "profiles/sercos/sercos_defs.h"
#include "diagnosis/dia.h"

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

comm::axisprofile::FieldbusAdapterSoE::FieldbusAdapterSoE(ProfileTrace* const trace) : FieldbusAdapter(trace)
{
  m_getParamResponseDataCheck.emplace_back(
    &ERROR_FB_VERIFICATION,                                            // trace error message
    AxisProfileResult::ERR_DATALAYER_WRONG_TYPE,                       // return type in case of error
    [] (const comm::datalayer::Variant* variantData)
    {
      return comm::datalayer::STATUS_FAILED(variantData->verifyFlatbuffers(comm::ethercat::master::fbs::VerifyParameterBuffer));
    }  //lambda
  );

  m_getParamResponseDataCheck.emplace_back(
    &ERROR_FB_RESPONSE,                                               // trace error message
    AxisProfileResult::ERR_FIELDBUS,                                  // return type in case of error
    [] (const comm::datalayer::Variant* variantData)
    {
      return flatbuffers::IsFieldPresent(comm::ethercat::master::fbs::GetParameter(variantData->getData()), comm::ethercat::master::fbs::Parameter::VT_REQUEST);
    }   // lambda
  );
}

void FieldbusAdapterSoE::setFieldbusMasterAddress(const std::string& address)
{
  // Update base address string
  FieldbusAdapter::setFieldbusMasterAddress(address);

  // Update SoE parameter request address
  m_fieldbusParamRequestAddress = m_fieldbusMasterDatalayerAddress + DL_SUB_ADDRESS_SOE_IDN;
}

AxisProfileResult FieldbusAdapterSoE::getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen)
{
  for(const auto& check : m_getParamResponseDataCheck)
  {
    if(check.m_fnct(variantData))
    {
      APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, check.m_traceMessage->c_str()));
      return check.m_returnResult;
    }
  }
  auto responseData = comm::ethercat::master::fbs::GetParameter(variantData->getData())->response();
  if (flatbuffers::IsFieldPresent(comm::ethercat::master::fbs::GetParameter(variantData->getData()), comm::ethercat::master::fbs::Parameter::VT_RESPONSE)
  && (flatbuffers::IsFieldPresent(responseData, comm::ethercat::master::fbs::ParameterResponse::VT_DATA)))
  {
    comm::ethercat::master::fbs::ElementFlags flag {};
    if (flatbuffers::IsFieldPresent(responseData, comm::ethercat::master::fbs::ParameterResponse::VT_ELEMENTFLAGS))
    {
      flag = responseData->elementFlags();
    }
    
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
    APR_TRACE_MESSAGE("%s; SoE-Read %s %d bytes: %s",
                      m_profileTrace->createTraceMsg(),
                      comm::ethercat::master::fbs::EnumNameElementFlags(flag),
                      readLen, FieldbusHelper::rawDataToHexDump(rawData, size).c_str());
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapterSoE::readSingleParamAsync(ParameterResponseCallbackData* const responseCallbackData, const ParamReqDataBase& readData)
{
  if (!responseCallbackData || !m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  const ParamReqDataSoE* readDataSoE = static_cast<const ParamReqDataSoE*>(&readData);
  generateParamReadRequestFlatbuffer(m_fbReadBuilder, &responseCallbackData->m_requestVariant, *readDataSoE);
  responseCallbackData->m_requestFinishedFlag = false;
  responseCallbackData->m_dlResult = comm::datalayer::DlResult::DL_OK;

  APR_TRACE_MESSAGE("%s; SoE-Read (ASYNC): IDN %s, Address %d",
                    m_profileTrace->createTraceMsg(),
                    idnToString(readDataSoE->idn).c_str(),
                    readDataSoE->address);
  return FieldbusAdapter::executeDatalayerReadAsync(responseCallbackData, m_fieldbusParamRequestAddress);
}

AxisProfileResult FieldbusAdapterSoE::writeSingleParamAsync(ParameterResponseCallbackData* responseCallbackData, const ParamReqDataBase& writeData)
{
  if (!responseCallbackData || !m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  const ParamReqDataSoE* writeDataSoE = static_cast<const ParamReqDataSoE*>(&writeData);
  generateParamWriteRequestFlatbuffer(m_fbWriteBuilder, &responseCallbackData->m_requestVariant, *writeDataSoE);
  responseCallbackData->m_requestFinishedFlag = false;
  responseCallbackData->m_dlResult = comm::datalayer::DlResult::DL_OK;

  APR_TRACE_MESSAGE("%s; SoE-Write (ASYNC): IDN %s, Address %d, value: %s",
                    m_profileTrace->createTraceMsg(),
                    idnToString(writeDataSoE->idn).c_str(),
                    writeDataSoE->address,
                    FieldbusHelper::rawDataToHexDump(writeDataSoE->writeBufPtr, writeDataSoE->writeBufLen).c_str());
  return FieldbusAdapter::executeDatalayerWriteAsync(responseCallbackData, m_fieldbusParamRequestAddress);
}

AxisProfileResult FieldbusAdapterSoE::readSingleParamSync(const ParamReqDataBase& readData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen)
{
  if (!m_datalayerClient || !bufPtr)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant requestVariant{};
  const ParamReqDataSoE* readDataSoE = static_cast<const ParamReqDataSoE*>(&readData);
  (void)generateParamReadRequestFlatbuffer(m_fbReadBuilder, &requestVariant, *readDataSoE);

  APR_TRACE_MESSAGE("%s; SoE-Read (SYNC): IDN %s, Address %d",
                    m_profileTrace->createTraceMsg(),
                    idnToString(readDataSoE->idn).c_str(),
                    readDataSoE->address);
  AxisProfileResult result = FieldbusAdapter::executeDatalayerReadSync(&requestVariant, m_fieldbusParamRequestAddress);
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(&requestVariant, m_profileTrace->getProfileName(), __FILE__, __FUNCTION__, __LINE__,
                       "SoE-Read (SYNC): IDN %s, Address %d", idnToString(readDataSoE->idn).c_str(), readDataSoE->address);
    return result;
  }
  return getParamResponseData(&requestVariant, bufPtr, bufLen, readLen);
}

AxisProfileResult FieldbusAdapterSoE::writeSingleParamSync(const ParamReqDataBase& writeData)
{
  if (!m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL, "Datalayer Client"));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant requestVariant{};
  const ParamReqDataSoE* writeDataSoE = static_cast<const ParamReqDataSoE*>(&writeData);
  (void)generateParamWriteRequestFlatbuffer(m_fbWriteBuilder, &requestVariant, *writeDataSoE);

  APR_TRACE_MESSAGE("%s; SoE-Write (SYNC): IDN %s, Address %d, value: %s",
                    m_profileTrace->createTraceMsg(),
                    idnToString(writeDataSoE->idn),
                    writeDataSoE->address,
                    FieldbusHelper::rawDataToHexDump(writeDataSoE->writeBufPtr, writeDataSoE->writeBufLen).c_str());
  AxisProfileResult result = FieldbusAdapter::executeDatalayerWriteSync(&requestVariant, m_fieldbusParamRequestAddress);
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(&requestVariant, m_profileTrace->getProfileName(), __FILE__, __FUNCTION__, __LINE__,
                       "SoE-Write (SYNC): IDN %s, Address %d", idnToString(writeDataSoE->idn).c_str(), writeDataSoE->address);
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////////
// ****************************** PRIVATE METHODS ****************************** //
AxisProfileResult FieldbusAdapterSoE::generateParamReadRequestFlatbuffer(flatbuffers::FlatBufferBuilder& builder, comm::datalayer::Variant* variant, const ParamReqDataSoE& readRequestData)
{
  builder.Clear();
  builder.ForceDefaults(true);

  auto paramRequestOffset = CreateParameterRequest(builder,
                                                   comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                                   readRequestData.address,
                                                   readRequestData.driveNum,
                                                   static_cast<comm::ethercat::master::fbs::ElementFlags>(readRequestData.elementFlag),
                                                   readRequestData.idn,
                                                   0,
                                                   readRequestData.maxLen);

  comm::ethercat::master::fbs::ParameterBuilder paramBuilder(builder);
  paramBuilder.add_request(paramRequestOffset);
  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);

  variant->shareFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapterSoE::generateParamWriteRequestFlatbuffer(flatbuffers::FlatBufferBuilder& builder, comm::datalayer::Variant* variant, const ParamReqDataSoE& writeRequestData)
{
  builder.Clear();
  builder.ForceDefaults(true);

  auto dataOffset = builder.CreateVector(writeRequestData.writeBufPtr, writeRequestData.writeBufLen);
  auto paramRequestOffset = CreateParameterRequest(builder,
                                                   comm::ethercat::master::fbs::Addresstype_fixedphysical,
                                                   writeRequestData.address,
                                                   writeRequestData.driveNum,
                                                   comm::ethercat::master::fbs::ElementFlags_value,
                                                   writeRequestData.idn,
                                                   dataOffset,
                                                   writeRequestData.maxLen);

  comm::ethercat::master::fbs::ParameterBuilder paramBuilder(builder);
  paramBuilder.add_request(paramRequestOffset);
  auto exchangeData = paramBuilder.Finish();
  builder.Finish(exchangeData);

  variant->shareFlatbuffers(builder);
  return AxisProfileResult::STS_OK;
}
