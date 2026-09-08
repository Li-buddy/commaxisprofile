#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements a mockup for the CoE fieldbus adapter interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "fieldbuses/i_fieldbus_adapter.h"
#include "helper/flatbuffers_helper_coe.h"
#include "mockup/mock_fieldbus_adapter.h"
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

  /// @brief This class implements a mockup for the CoE fieldbus adapter interface.
  class MockFieldbusAdapterCoE : public test::MockFieldbusAdapter
  {
  public: // methods

    /// @brief Constructor
    MockFieldbusAdapterCoE() = default;
    /// @brief Destructor
    virtual ~MockFieldbusAdapterCoE() noexcept override = default;

    comm::axisprofile::AxisProfileResult getParamResponseData(const comm::datalayer::Variant* variantData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) override
    {
      if (comm::axisprofile::aprStatusSucceeded(m_returnValue))
      {
        // This code must only be invoked when everything is OK (or you will get an exception)!
        // auto sdoData = comm::ethercat::master::fbs::GetParameter(variantData->getData());
        auto sdoData = comm::ethercat::master::fbs::GetSDO(variantData->getData());
        auto responseData = sdoData->response();
        auto data = responseData->data();
        auto rawData = data->Data();

        readLen = data->size() < bufLen ? data->size() : bufLen;
        std::memcpy(bufPtr, rawData, readLen);
      }

      //flag = ~flag;
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult readSingleParamAsync(comm::axisprofile::ParameterResponseCallbackData* const responseCallbackData,
                                                              const comm::axisprofile::ParamReqDataBase& readReq) override {
      comm::datalayer::DlResult dlResult = comm::datalayer::DlResult::DL_OK;
      if (m_datalayerClient)
      {
        const comm::axisprofile::ParamReqDataCoE* readDataCoE = static_cast<const comm::axisprofile::ParamReqDataCoE*>(&readReq);
        m_fbsHelper.generateSDORequestFlatbuffer(&responseCallbackData->m_requestVariant, readDataCoE->objEntry.index, readDataCoE->objEntry.subIndex, nullptr, 0);
        dlResult = m_datalayerClient->readAsync("", responseCallbackData->m_requestVariant, parameterRequestCallback(responseCallbackData));
      }
      return comm::datalayer::STATUS_FAILED(dlResult) ? comm::axisprofile::AxisProfileResult::ERR_FIELDBUS : m_returnValue;
    }
    comm::axisprofile::AxisProfileResult writeSingleParamAsync(comm::axisprofile::ParameterResponseCallbackData* responseCallbackData,
                                                               const comm::axisprofile::ParamReqDataBase& writeData) override {
      comm::datalayer::DlResult dlResult = comm::datalayer::DlResult::DL_OK;
      if (m_datalayerClient)
      {
        const comm::axisprofile::ParamReqDataCoE* writeDataCoE = static_cast<const comm::axisprofile::ParamReqDataCoE*>(&writeData);
        m_fbsHelper.generateSDORequestFlatbuffer(&responseCallbackData->m_requestVariant, writeDataCoE->objEntry.index, writeDataCoE->objEntry.subIndex, writeDataCoE->writeBufPtr, writeDataCoE->writeBufLen);
        dlResult = m_datalayerClient->writeAsync("", responseCallbackData->m_requestVariant, parameterRequestCallback(responseCallbackData));
      }
      return comm::datalayer::STATUS_FAILED(dlResult) ? comm::axisprofile::AxisProfileResult::ERR_FIELDBUS : m_returnValue;
    }

    comm::axisprofile::AxisProfileResult readSingleParamSync(const comm::axisprofile::ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) override
    {
      comm::datalayer::DlResult dlResult = comm::datalayer::DlResult::DL_OK;
      if (m_datalayerClient)
      {
        comm::datalayer::Variant requestData {};
        const comm::axisprofile::ParamReqDataCoE* readDataCoE = static_cast<const comm::axisprofile::ParamReqDataCoE*>(&readReq);
        m_fbsHelper.generateSDORequestFlatbuffer(&requestData, readDataCoE->objEntry.index, readDataCoE->objEntry.subIndex, nullptr, 0);
        dlResult = m_datalayerClient->readSync("", &requestData);
        if (comm::datalayer::STATUS_SUCCEEDED(dlResult) && bufPtr)
        {
          getParamResponseData(&requestData, bufPtr, bufLen, readLen);
        }
      }
      return comm::datalayer::STATUS_FAILED(dlResult) ? comm::axisprofile::AxisProfileResult::ERR_FIELDBUS : m_returnValue;
    }

    comm::axisprofile::AxisProfileResult writeSingleParamSync(const comm::axisprofile::ParamReqDataBase& writeData) override {
      comm::datalayer::DlResult dlResult = comm::datalayer::DlResult::DL_OK;
      if (m_datalayerClient)
      {
        comm::datalayer::Variant requestData{};
        const comm::axisprofile::ParamReqDataCoE* writeDataCoE = static_cast<const comm::axisprofile::ParamReqDataCoE*>(&writeData);
        m_fbsHelper.generateSDORequestFlatbuffer(&requestData, writeDataCoE->objEntry.index, writeDataCoE->objEntry.subIndex, writeDataCoE->writeBufPtr, writeDataCoE->writeBufLen);
        dlResult = m_datalayerClient->writeSync("", &requestData);
      }
      return comm::datalayer::STATUS_FAILED(dlResult) ? comm::axisprofile::AxisProfileResult::ERR_FIELDBUS : m_returnValue;
    }

  public: // Data

    test::FlatbuffersHelperCoE m_fbsHelper{}; ///< Helper class to generate flatbuffers
  };
}
