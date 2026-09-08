#pragma once
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
#include "comm/ethercat/master/fbs/master_state_generated.h"
#include "comm/ethercat/master/fbs/master_status_generated.h"
#include "comm/ethercat/master/fbs/slave_state_generated.h"
#include "comm/ethercat/master/fbs/slave_status_generated.h"
#include "flatbuffers/flatbuffers.h"
#include "i_fieldbus_adapter.h"
#include "profiles/profile_trace.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
    
  /// @brief GetParamResponseDataCheck
  typedef struct GetParamResponseDataCheck
  {
    const std::string* const m_traceMessage;                                  ///< for trace output
    const AxisProfileResult m_returnResult;                                   ///< return value in case of error
    const std::function<bool(const comm::datalayer::Variant* variantData)> m_fnct;  ///< method to call

    /// @brief  Constructor
    /// @param[in]   traceMessage   Pointer to traceMessage
    /// @param[in]   returnResult   Pointer to returnResult
    /// @param[in]   fnct           Pointer to callback
    GetParamResponseDataCheck(const std::string* const traceMessage, const AxisProfileResult returnResult, const std::function<bool(const comm::datalayer::Variant* variantData)> &fnct)       
    : m_traceMessage(traceMessage),
      m_returnResult(returnResult),
      m_fnct(fnct) {};
  } GetParamResponseDataChecks;
    
  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the base class for all fieldbus adapters.
  class FieldbusAdapter : public IFieldbusAdapter
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   trace   Pointer to the profile trace
    explicit FieldbusAdapter(ProfileTrace* const trace);

    /// @brief destructor
    virtual ~FieldbusAdapter() noexcept override = default;

    void setDatalayer(comm::datalayer::IClient* client) final;
    void setFieldbusMasterAddress(const std::string& address) override;
    std::string getFieldbusMasterAddress() const final;
    AxisProfileResult readSlaveStateSync(const ReadSlaveStateDataBase& readData, std::string* state) override;
    AxisProfileResult readFieldbusMasterStateSync(std::string* state) override;
    AxisProfileResult readSlaveStatusSync(const ReadSlaveStateDataBase& readData, std::uint32_t& status) override;
    AxisProfileResult readFieldbusMasterStatusSync(std::uint32_t& status) override;
    AxisProfileResult readSlaveDcConfigInfoSync(std::uint16_t slaveAddr, DcConfigInfo& dcCfg) override;
    AxisProfileResult getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen) override;
    AxisProfileResult readSingleParamSync(const ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen) override;

  protected: //  Methods

    /// @brief This method uses the given flatbuffer from the child class to execute the asynchronous datalayer read request.
    /// @param[in]   responseCallbackData   Prepared callback data, including the finished request flatbuffer.
    /// @param[in]   address                Datalayer address used for the request.
    /// @return      Return value indicating success or error.
    AxisProfileResult executeDatalayerReadAsync(ParameterResponseCallbackData* const responseCallbackData, const std::string& address) const;

    /// @brief This method uses the given flatbuffer from the child class to execute the asynchronous datalayer write request.
    /// @param[in]   responseCallbackData   Prepared callback data, including the finished request flatbuffer.
    /// @param[in]   address                Datalayer address used for the request.
    /// @return      Return value indicating success or error.
    AxisProfileResult executeDatalayerWriteAsync(ParameterResponseCallbackData* const responseCallbackData, const std::string& address) const;

    /// @brief This method uses the given flatbuffer from the child class to execute the synchronous datalayer read request.
    /// @param[out]   requestVariant   Prepared variant request flatbuffer.
    /// @param[in]    address          Datalayer address used for the request.
    /// @return       Return value indicating success or error.
    AxisProfileResult executeDatalayerReadSync(comm::datalayer::Variant* requestVariant, const std::string& address) const;

    /// @brief This method uses the given flatbuffer from the child class to execute the synchronous datalayer write request.
    /// @param[in,out]   requestVariant   Prepared variant request flatbuffer.
    /// @param[in]       address          Datalayer address used for the request.
    /// @return          Return value indicating success or error.
    AxisProfileResult executeDatalayerWriteSync(comm::datalayer::Variant* requestVariant, const std::string& address) const;
    /// @brief This method extracts the response data of a master state request from the given variant.
    /// @param[in]    variantData   Variant containing the data.
    /// @param[out]   state         Extracted data.
    /// @return       Return value indicating success or error.
    AxisProfileResult getReadMasterStateResponseData(const comm::datalayer::Variant* variantData, std::string* state);
    /// @brief This method extracts the response data of a slave state request from the given variant.
    /// @param[in]    variantData   Variant containing the data.
    /// @param[out]   state         Extracted data.
    /// @return       Return value indicating success or error.
    AxisProfileResult getReadSlaveStateResponseData(const comm::datalayer::Variant* variantData, std::string* state);
    /// @brief This method extracts the response data of a master status request from the given variant.
    /// @param[in]    variantData   Variant containing the data.
    /// @param[out]   status        Extracted data.
    /// @return       Return value indicating success or error.
    AxisProfileResult getReadMasterStatusResponseData(const comm::datalayer::Variant* variantData, std::uint32_t& status);
    /// @brief This method extracts the response data of a slave status request from the given variant.
    /// @param[in]    variantData   Variant containing the data.
    /// @param[out]   status        Extracted data.
    /// @return       Return value indicating success or error.
    AxisProfileResult getReadSlaveStatusResponseData(const comm::datalayer::Variant* variantData, std::uint32_t& status);

  private: // Methods

    /// @brief  This method is a wrapper for datalayer response callbacks, used for parameter requests.
    ///         The method uses a lambda function which is executed when the callback returns. Inside of the lambda, the callback information
    ///         will be stored in the 'responseData' argument.
    /// @param[out]   responseData   Received response data of the parameter request.
    /// @return The lambda function that will fill the 'responseData'  argument when it is executed later.
    comm::datalayer::IClient::ResponseCallback parameterRequestCallback(ParameterResponseCallbackData* const responseData) const;

  protected: // Data
    static const std::string ERROR_FB;                ///< fb error
    static const std::string ERROR_FB_VERIFICATION;   ///< fb error
    static const std::string ERROR_FB_RESPONSE;       ///< fb error
    static const std::string ERROR_FB_DATA_NOT_PRESENTED; ///< fb error
    ProfileTrace* const m_profileTrace;               ///< Pointer to the profile trace
    comm::datalayer::IClient* m_datalayerClient{};    ///< Pointer to the datalayer client
    std::string m_fieldbusMasterDatalayerAddress{};   ///< Datalayer address of the fieldbus master instance
    std::string m_fieldbusParamRequestAddress{};      ///< Datalayer address for parameter requests (specific to the current fieldbus)
    std::vector<GetParamResponseDataCheck> m_getParamResponseDataCheck;  ///< vector contains procedures to check fb response
    flatbuffers::FlatBufferBuilder m_fbReadBuilder {};  ///< the flatbuffer data builder for reading
    flatbuffers::FlatBufferBuilder m_fbWriteBuilder {}; ///< the flatbuffer data builder for writing
  };
}
