/// @file
///
/// @brief This file is part of the axisprofile library and implements the base class for all fieldbus adapters.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/debug_trace.h"
#include "fieldbus_adapter.h"
#include "helper/device_fbs_data_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------
const std::string FieldbusAdapter::ERROR_FB{"Received FB is not a flatbuffer"};
const std::string FieldbusAdapter::ERROR_FB_VERIFICATION{"Received FB verification failed"};
const std::string FieldbusAdapter::ERROR_FB_RESPONSE{"Detected field 'Request' instead of 'Response' in received FB"};
const std::string FieldbusAdapter::ERROR_FB_DATA_NOT_PRESENTED {"Data not presented in FB"};

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

comm::axisprofile::FieldbusAdapter::FieldbusAdapter(ProfileTrace* const trace) : m_profileTrace(trace)
{
  m_getParamResponseDataCheck.reserve(3);
  m_getParamResponseDataCheck.emplace_back(
    &ERROR_FB,   ///< trace error message
    AxisProfileResult::ERR_WRONGDATATYPE,  ///< return type in case of error
    [] (const comm::datalayer::Variant* variantData)
    {
      return(!variantData || (variantData->getType() != comm::datalayer::VariantType::FLATBUFFERS));
    }
  );
}

void FieldbusAdapter::setDatalayer(comm::datalayer::IClient* client)
{
  m_datalayerClient = client;
}

void FieldbusAdapter::setFieldbusMasterAddress(const std::string& address)
{
  m_fieldbusMasterDatalayerAddress = address;
}

std::string FieldbusAdapter::getFieldbusMasterAddress() const
{
  return m_fieldbusMasterDatalayerAddress;
}

AxisProfileResult FieldbusAdapter::executeDatalayerReadAsync(ParameterResponseCallbackData* const responseCallbackData, const std::string& address) const
{
  comm::datalayer::DlResult datalayerResult = m_datalayerClient->readAsync(address, responseCallbackData->m_requestVariant, parameterRequestCallback(responseCallbackData));
  if (comm::datalayer::STATUS_FAILED(datalayerResult))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_DATALAYER, datalayerResult.toString()));
    return AxisProfileResult::ERR_DATALAYER;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapter::executeDatalayerWriteAsync(ParameterResponseCallbackData* const responseCallbackData, const std::string& address) const
{
  comm::datalayer::DlResult datalayerResult = m_datalayerClient->writeAsync(address, responseCallbackData->m_requestVariant, parameterRequestCallback(responseCallbackData));
  if (comm::datalayer::STATUS_FAILED(datalayerResult))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_DATALAYER, datalayerResult.toString()));
    return AxisProfileResult::ERR_DATALAYER;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapter::executeDatalayerReadSync(comm::datalayer::Variant* requestVariant, const std::string& address) const
{
  comm::datalayer::DlResult datalayerResult = m_datalayerClient->readSync(address, requestVariant); 
  if (comm::datalayer::STATUS_FAILED(datalayerResult))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_DATALAYER, datalayerResult.toString()));
    return AxisProfileResult::ERR_DATALAYER;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapter::executeDatalayerWriteSync(comm::datalayer::Variant* requestVariant, const std::string& address) const
{
  comm::datalayer::DlResult datalayerResult = m_datalayerClient->writeSync(address, requestVariant);
  if (comm::datalayer::STATUS_FAILED(datalayerResult))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_DATALAYER, datalayerResult.toString()));
    return AxisProfileResult::ERR_DATALAYER;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapter::getReadMasterStateResponseData(const comm::datalayer::Variant* variantData, std::string* state)
{
  if (!variantData || (variantData->getType() != comm::datalayer::VariantType::FLATBUFFERS))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB.c_str()));
    return AxisProfileResult::ERR_WRONGDATATYPE;
  }

  if (comm::datalayer::STATUS_FAILED(variantData->verifyFlatbuffers(comm::ethercat::master::fbs::VerifyMasterStateBuffer)))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB_VERIFICATION.c_str()));
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  const auto stateData = comm::ethercat::master::fbs::GetMasterState(variantData->getData());
  if (flatbuffers::IsFieldPresent(stateData, comm::ethercat::master::fbs::MasterState::VT_REQUEST))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB_RESPONSE.c_str()));
    return AxisProfileResult::ERR_FIELDBUS;
  }

  if (flatbuffers::IsFieldPresent(stateData, comm::ethercat::master::fbs::MasterState::VT_RESPONSE))
  {
    auto responseData = stateData->response();
    if (flatbuffers::IsFieldPresent(responseData, comm::ethercat::master::fbs::MasterStateResponse::VT_CURRENTSTATE))
    {
      auto stateEnum = responseData->currentState();
      *state = comm::ethercat::master::fbs::EnumNamesEthercatState()[stateEnum];
    }
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapter::getReadMasterStatusResponseData(const comm::datalayer::Variant* variantData, std::uint32_t& status)
{
  if (variantData->getType() != comm::datalayer::VariantType::FLATBUFFERS)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB.c_str()));
    return AxisProfileResult::ERR_WRONGDATATYPE;
  }

  if (comm::datalayer::STATUS_FAILED(variantData->verifyFlatbuffers(comm::ethercat::master::fbs::VerifyMasterStatusBuffer)))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB_VERIFICATION.c_str()));
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  const auto statusData = comm::ethercat::master::fbs::GetMasterStatus(variantData->getData());
  if (flatbuffers::IsFieldPresent(statusData, comm::ethercat::master::fbs::MasterStatus::VT_RESPONSE))
  {
    auto responseData = statusData->response();
    if (flatbuffers::IsFieldPresent(responseData, comm::ethercat::master::fbs::MasterStatusResponse::VT_STATUS))
    {
      status = responseData->status();
      return AxisProfileResult::STS_OK;
    }
  }

  APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB_DATA_NOT_PRESENTED.c_str()));
  return AxisProfileResult::ERR_FIELDBUS;
}

AxisProfileResult FieldbusAdapter::getReadSlaveStateResponseData(const comm::datalayer::Variant* variantData, std::string* state)
{
  if (!variantData || (variantData->getType() != comm::datalayer::VariantType::FLATBUFFERS))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB.c_str()));
    return AxisProfileResult::ERR_WRONGDATATYPE;
  }

  if (comm::datalayer::STATUS_FAILED(variantData->verifyFlatbuffers(comm::ethercat::master::fbs::VerifySlaveStateBuffer)))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB_VERIFICATION.c_str()));
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  const auto stateData = comm::ethercat::master::fbs::GetSlaveState(variantData->getData());
  if (flatbuffers::IsFieldPresent(stateData, comm::ethercat::master::fbs::SlaveState::VT_REQUEST))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB_RESPONSE.c_str()));
    return AxisProfileResult::ERR_FIELDBUS;
  }

  if (flatbuffers::IsFieldPresent(stateData, comm::ethercat::master::fbs::SlaveState::VT_RESPONSE))
  {
    auto responseData = stateData->response();
    if (flatbuffers::IsFieldPresent(responseData, comm::ethercat::master::fbs::SlaveStateResponse::VT_CURRENTSTATE))
    {
      auto stateEnum = responseData->currentState();
      *state = comm::ethercat::master::fbs::EnumNamesEthercatState()[stateEnum];
    }
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult FieldbusAdapter::getReadSlaveStatusResponseData(const comm::datalayer::Variant* variantData, std::uint32_t& status)
{
  if (!variantData || (variantData->getType() != comm::datalayer::VariantType::FLATBUFFERS))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB.c_str()));
    return AxisProfileResult::ERR_WRONGDATATYPE;
  }

  if (comm::datalayer::STATUS_FAILED(variantData->verifyFlatbuffers(comm::ethercat::master::fbs::VerifySlaveStatusBuffer)))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB_VERIFICATION.c_str()));
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  const auto stateData = comm::ethercat::master::fbs::GetSlaveStatus(variantData->getData());
  if (flatbuffers::IsFieldPresent(stateData, comm::ethercat::master::fbs::SlaveStatus::VT_RESPONSE))
  {
    auto responseData = stateData->response();
    if (flatbuffers::IsFieldPresent(responseData, comm::ethercat::master::fbs::SlaveStatusResponse::VT_STATUS))
    {
      status = responseData->status();
      return AxisProfileResult::STS_OK;
    }
  }
  APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_FIELDBUS, ERROR_FB_DATA_NOT_PRESENTED.c_str()));
  return AxisProfileResult::ERR_FIELDBUS;
}

AxisProfileResult FieldbusAdapter::readSlaveStateSync(const ReadSlaveStateDataBase& readData, std::string* state)
{
  if (!m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant requestVariant{};
  const ReadSlaveStateData* readSlaveStateData = static_cast<const ReadSlaveStateData*>(&readData);
  comm::axisprofile::DeviceFbsDataHelper::generateReadSlaveStateFlatbuffer(&requestVariant, *readSlaveStateData);

  std::string datalayerAddress = m_fieldbusMasterDatalayerAddress + DL_SUB_ADDRESS_SLAVE_STATE;
  APR_TRACE_MESSAGE("%s; ReadSlaveState (SYNC): Address: %s", m_profileTrace->createTraceMsg(), datalayerAddress.c_str());
  AxisProfileResult result = FieldbusAdapter::executeDatalayerReadSync(&requestVariant, datalayerAddress);
  if (aprStatusFailed(result))
  {
    return result;
  }

  return FieldbusAdapter::getReadSlaveStateResponseData(&requestVariant, state);
}

AxisProfileResult FieldbusAdapter::readSlaveStatusSync(const ReadSlaveStateDataBase& readData, std::uint32_t& status)
{
  if (!m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant requestVariant {};
  const ReadSlaveStateData* readSlaveStateData = static_cast<const ReadSlaveStateData*>(&readData);
  comm::axisprofile::DeviceFbsDataHelper::generateReadSlaveStatusFlatbuffer(&requestVariant, *readSlaveStateData);

  std::string datalayerAddress = m_fieldbusMasterDatalayerAddress + DL_SUB_ADDRESS_SLAVE_STATUS;
  AxisProfileResult result = FieldbusAdapter::executeDatalayerReadSync(&requestVariant, datalayerAddress);
  if (aprStatusFailed(result))
  {
    APR_TRACE_MESSAGE("%s; ReadSlaveStatus (SYNC) failed: result %#010x, Address: %s", m_profileTrace->createTraceMsg(), result, datalayerAddress.c_str());
    return result;
  }
  else
  {
    APR_TRACE_MESSAGE("%s; ReadSlaveStatus (SYNC) succeed: Address: %s", m_profileTrace->createTraceMsg(), datalayerAddress.c_str());
  }

  return FieldbusAdapter::getReadSlaveStatusResponseData(&requestVariant, status);
}

AxisProfileResult FieldbusAdapter::readFieldbusMasterStateSync(std::string* state)
{
  if (!m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant readData{};
  std::string datalayerAddress = m_fieldbusMasterDatalayerAddress + DL_SUB_ADDRESS_MASTER_STATE;
  APR_TRACE_MESSAGE("%s; ReadMasterState (SYNC): Address: %s", m_profileTrace->createTraceMsg(), datalayerAddress.c_str());
  AxisProfileResult result = FieldbusAdapter::executeDatalayerReadSync(&readData, datalayerAddress);
  if (aprStatusFailed(result))
  {
    return result;
  }  
  return FieldbusAdapter::getReadMasterStateResponseData(&readData, state);
}

AxisProfileResult FieldbusAdapter::readFieldbusMasterStatusSync(std::uint32_t& status)
{
  if (!m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant readData {};
  std::string datalayerAddress = m_fieldbusMasterDatalayerAddress + DL_SUB_ADDRESS_MASTER_STATUS;
  APR_TRACE_MESSAGE("ReadMasterStatus (SYNC): Address: %s", datalayerAddress.c_str());
  AxisProfileResult result = FieldbusAdapter::executeDatalayerReadSync(&readData, datalayerAddress);
  if (aprStatusFailed(result))
  {
    return result;
  }

  return FieldbusAdapter::getReadMasterStatusResponseData(&readData, status);
}

AxisProfileResult FieldbusAdapter::readSlaveDcConfigInfoSync(std::uint16_t slaveAddr, DcConfigInfo& dcCfg)
{
  if (!m_datalayerClient)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant reqRespVar {};
  DeviceFbsDataHelper::generateReadSlaveDcCfgInfoFlatbuffer(&reqRespVar, slaveAddr);
  std::string dlAddr = m_fieldbusMasterDatalayerAddress + DL_SUB_ADDRESS_DC_CFG_INFO;
  APR_TRACE_MESSAGE("%s; Read DC Info(SYNC): Address: %s", m_profileTrace->createTraceMsg(), dlAddr.c_str());

  auto result = FieldbusAdapter::executeDatalayerReadSync(&reqRespVar, dlAddr);
  if (aprStatusFailed(result))
  {
    return result;
  }

  return DeviceFbsDataHelper::extractSlaveDcCfgInfo(reqRespVar, dcCfg);
}

AxisProfileResult FieldbusAdapter::getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen)
{
  std::size_t readLen {};
  AxisProfileResult res = static_cast<IFieldbusAdapter*>(this)->getParamResponseData(variantData, bufPtr, bufLen, readLen);
  if (readLen != bufLen)
  {
    return AxisProfileResult::ERR_WRONGDATASIZE;
  }
  return res;
}

AxisProfileResult FieldbusAdapter::readSingleParamSync(const ParamReqDataBase& readData, std::uint8_t* bufPtr, std::size_t bufLen)
{
  std::size_t readLen {};
  AxisProfileResult res = static_cast<IFieldbusAdapter*>(this)->readSingleParamSync(readData, bufPtr, bufLen, readLen);
  if (aprStatusFailed(res))
  {
    return res;
  }
  if (readLen != bufLen)
  {
    return AxisProfileResult::ERR_WRONGDATASIZE;
  }
  return AxisProfileResult::STS_OK;
}

///////////////////////////////////////////////////////////////////////////////////
// ****************************** PRIVATE METHODS ****************************** //

comm::datalayer::IClient::ResponseCallback FieldbusAdapter::parameterRequestCallback(ParameterResponseCallbackData* const responseData) const
{
  return [responseData](comm::datalayer::DlResult status, const comm::datalayer::Variant* data)
  {
    if (data)
    {
      responseData->m_requestVariant = *data;
    }
    responseData->m_dlResult = status;
    responseData->m_requestFinishedFlag = true;
  };
}
