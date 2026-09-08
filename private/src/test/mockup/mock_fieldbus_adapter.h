#pragma once
/// @file
///
/// @brief This header file is part of the axisprofile library and implements mockup for
/// the common fieldbus adapter interface.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "fieldbuses/i_fieldbus_adapter.h"
#include "helper/flatbuffers_helper_common.h"
#include <cstring>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This class implements a mockup for the fieldbus adapter common interface.
  class MockFieldbusAdapter : public comm::axisprofile::IFieldbusAdapter
  {
  public: // methods

    /// @brief Constructor
    MockFieldbusAdapter() = default;
    /// @brief Destructor
    virtual ~MockFieldbusAdapter() noexcept override = default;

    void setDatalayer(comm::datalayer::IClient* client) override { m_datalayerClient = client; }
    void setFieldbusMasterAddress(const std::string& address) override {m_fieldbusMasterAddress = address;}
    std::string getFieldbusMasterAddress() const override { return m_fieldbusMasterAddress; }

    comm::axisprofile::AxisProfileResult getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen) override
    {
      std::size_t readLen {};
      return static_cast<comm::axisprofile::IFieldbusAdapter*>(this)->getParamResponseData(variantData, bufPtr, bufLen, readLen);
    }

    /// @brief This method generates slave state response data
    /// @param[in]  variantData  variant data
    /// @param[out]  state        slave state
    /// @return      Return value indicating success or error.
    comm::axisprofile::AxisProfileResult getReadSlaveStateResponseData(const comm::datalayer::Variant* variantData, std::string* state) {
      if (comm::axisprofile::aprStatusSucceeded(m_returnValue))
      {
        // Current logic is only for SoE!
        // This code must only be invoked when everything is OK (or you will get an exception)!
        auto stateData = comm::ethercat::master::fbs::GetSlaveState(variantData->getData());
        auto responseData = stateData->response();
        auto stateEnum = responseData->currentState();
        *state = comm::ethercat::master::fbs::EnumNamesEthercatState()[stateEnum];
      }
      return m_returnValue;
    }
    /// @brief This method generates master state response data
    /// @param[in]  variantData  variant data
    /// @param[out]  state        master state
    /// @return      Return value indicating success or error.
    comm::axisprofile::AxisProfileResult getReadMasterStateResponseData(const comm::datalayer::Variant* variantData, std::string* state)
    {
      if (comm::axisprofile::aprStatusSucceeded(m_returnValue))
      {
        // This code must only be invoked when everything is OK (or you will get an exception)!
        auto stateData = comm::ethercat::master::fbs::GetMasterState(variantData->getData());
        auto responseData = stateData->response();
        auto stateEnum = responseData->currentState();
        *state = comm::ethercat::master::fbs::EnumNamesEthercatState()[stateEnum];
      }
      return m_returnValue;
    }

    /// @brief This method generates master status response data
    /// @param[in]  variantData  variant data
    /// @param[out]  status       master status
    /// @return      Return value indicating success or error.
    comm::axisprofile::AxisProfileResult getReadMasterStatusResponseData(const comm::datalayer::Variant* variantData, std::uint32_t& status)
    {
      if (comm::axisprofile::aprStatusSucceeded(m_returnValue))
      {
        // This code must only be invoked when everything is OK (or you will get an exception)!
        auto statusData = comm::ethercat::master::fbs::GetMasterStatus(variantData->getData());
        auto responseData = statusData->response();
        status = responseData->status();
      }
      return m_returnValue;
    }

    /// @brief This method generates slave status response data
    /// @param[in]   variantData  variant data
    /// @param[out]  status       master status
    /// @return      Return value indicating success or error.
    comm::axisprofile::AxisProfileResult getReadSlaveStatusResponseData(const comm::datalayer::Variant* variantData, std::uint32_t& status)
    {
      if (comm::axisprofile::aprStatusSucceeded(m_returnValue))
      {
        // This code must only be invoked when everything is OK (or you will get an exception)!
        auto statusData = comm::ethercat::master::fbs::GetSlaveStatus(variantData->getData());
        auto responseData = statusData->response();
        status = responseData->status();
      }
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult readSlaveStateSync(const comm::axisprofile::ReadSlaveStateDataBase& readData, std::string* state) override {
      comm::datalayer::DlResult dlResult = comm::datalayer::DlResult::DL_OK;
      if (m_datalayerClient)
      {
        comm::datalayer::Variant requestData{};
        const comm::axisprofile::ReadSlaveStateData* readDataSoE = static_cast<const comm::axisprofile::ReadSlaveStateData*>(&readData);
        m_fbsHelper.generateSlaveStateRequestFlatbuffer(&requestData, readDataSoE->m_deviceAddress);
        dlResult = m_datalayerClient->readSync("", &requestData);
        if (comm::datalayer::STATUS_SUCCEEDED(dlResult) && state) { getReadSlaveStateResponseData(&requestData, state); }
      }
      return comm::datalayer::STATUS_FAILED(dlResult) ? comm::axisprofile::AxisProfileResult::ERR_FIELDBUS : m_returnValue;
    }

    comm::axisprofile::AxisProfileResult readSlaveStatusSync(const comm::axisprofile::ReadSlaveStateDataBase& readData, std::uint32_t& status) override
    {
      comm::datalayer::DlResult dlResult = comm::datalayer::DlResult::DL_OK;
      if (m_datalayerClient)
      {
        comm::datalayer::Variant requestData {};
        const comm::axisprofile::ReadSlaveStateData* readDataSoE = static_cast<const comm::axisprofile::ReadSlaveStateData*>(&readData);
        m_fbsHelper.generateSlaveStatusRequestFlatbuffer(&requestData, readDataSoE->m_deviceAddress);
        dlResult = m_datalayerClient->readSync("", &requestData);
        if (comm::datalayer::STATUS_SUCCEEDED(dlResult))
        {
          getReadSlaveStatusResponseData(&requestData, status);
        }
      }
      return comm::datalayer::STATUS_FAILED(dlResult) ? comm::axisprofile::AxisProfileResult::ERR_FIELDBUS : m_returnValue;
    }

    comm::axisprofile::AxisProfileResult readFieldbusMasterStateSync(std::string* state) override
    {
      comm::datalayer::DlResult dlResult = comm::datalayer::DlResult::DL_OK;
      if (m_datalayerClient)
      {
        comm::datalayer::Variant readData{};
        dlResult = m_datalayerClient->readSync("", &readData);
        if (comm::datalayer::STATUS_SUCCEEDED(dlResult) && state) { getReadMasterStateResponseData(&readData, state); }
      }
      return comm::datalayer::STATUS_FAILED(dlResult) ? comm::axisprofile::AxisProfileResult::ERR_FIELDBUS : m_returnValue;
    }

    comm::axisprofile::AxisProfileResult readFieldbusMasterStatusSync(std::uint32_t& status) override
    {
      comm::datalayer::DlResult dlResult = comm::datalayer::DlResult::DL_OK;
      if (m_datalayerClient)
      {
        comm::datalayer::Variant readData {};
        dlResult = m_datalayerClient->readSync("", &readData);
        if (comm::datalayer::STATUS_SUCCEEDED(dlResult))
        {
          getReadMasterStatusResponseData(&readData, status);
        }
      }
      return comm::datalayer::STATUS_FAILED(dlResult) ? comm::axisprofile::AxisProfileResult::ERR_FIELDBUS : m_returnValue;
    }

    comm::axisprofile::AxisProfileResult readSlaveDcConfigInfoSync(std::uint16_t /*slaveAddr*/, comm::axisprofile::DcConfigInfo& dcCfg) override
    {
      dcCfg = m_dcConfig;
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult readSingleParamSync(const comm::axisprofile::ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen) override
    {
      std::size_t readLen {};
      return static_cast<comm::axisprofile::IFieldbusAdapter*>(this)->readSingleParamSync(readReq, bufPtr, bufLen, readLen);
    }

    // -------------------------------- Test methods ------------------------------------

    /// @brief  This method is a wrapper for datalayer response callbacks, used for parameter requests.
    /// @param[out]   responseData   Received response data of the parameter request.
    /// @return The lambda function that will fill the 'responseData'  argument when it is executed later.
    comm::datalayer::IClient::ResponseCallback parameterRequestCallback(comm::axisprofile::ParameterResponseCallbackData* responseData) const
    {
      return [responseData](comm::datalayer::DlResult status, const comm::datalayer::Variant* data)
      {
        responseData->m_requestFinishedFlag = true;
        responseData->m_dlResult = status;
        if (data)
        {
          responseData->m_requestVariant = *data;
        }
      };
    }

    /// @brief The method set return value of the adapter
    void setReturnValue(comm::axisprofile::AxisProfileResult value) { m_returnValue = value; }

  public: // Data
    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   ///< Result of member functions
    comm::axisprofile::DcConfigInfo m_dcConfig {true, 600000};  ///< The DC configuration information

  protected: // Data

    test::FlatbuffersHelperCommon m_fbsHelper{};      ///< fbs helper
    comm::datalayer::IClient* m_datalayerClient{};    ///< Pointer to the datalayer client mockup
    std::string m_fieldbusMasterAddress {};           ///< The fieldbus instance address
  };
}
