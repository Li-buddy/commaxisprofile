#pragma once

/// @file
///
/// This source file is part of the axisprofile library and declare mock EtherCAT master node.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_ethercat_master_general_nodes.h"
#include "helper/flatbuffers_helper_soe.h"
#include "helper/flatbuffers_helper_coe.h"
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "comm/ethercat/master/fbs/sdo_generated.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  
  //! @class MockEthercatMasterParamRequestNodeSoE
  //! Mockup of the ethercat master parameter request node
  class MockEthercatMasterParamRequestNodeSoE : public MockEthercatMasterNodeBase
  {
  public:
    //! @brief This method implements datalayer onRead
    //! @param[in]  data        variant
    //! @param[in]  callback    callback
    void onRead(const std::string &/*address*/, const comm::datalayer::Variant* data, const ResponseCallback &callback) override
    {
      if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) || (comm::datalayer::STATUS_FAILED(data->verifyFlatbuffers(comm::ethercat::master::fbs::VerifyParameterBuffer))))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }
      auto paramData = comm::ethercat::master::fbs::GetParameter(data->getData());
      if (!flatbuffers::IsFieldPresent(paramData, comm::ethercat::master::fbs::Parameter::VT_REQUEST)) { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); }
      comm::datalayer::Variant returnValue{};
      m_fbsHelper.generateParamResponseFlatbuffer(&returnValue, paramData->request()->idn());
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

    void onWrite(const std::string&/*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback) override
    {
      if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) || (comm::datalayer::STATUS_FAILED(data->verifyFlatbuffers(comm::ethercat::master::fbs::VerifyParameterBuffer))))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }
      auto paramData = comm::ethercat::master::fbs::GetParameter(data->getData());
      if (!flatbuffers::IsFieldPresent(paramData, comm::ethercat::master::fbs::Parameter::VT_REQUEST))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
      }
      m_fbsHelper.writeParamFlatbuffer(paramData->request());
      callback(comm::datalayer::DlResult::DL_OK, nullptr);
    }

  public: // Data

    test::FlatbuffersHelperSoE m_fbsHelper{};   ///< Helper class to generate flatbuffers
  };

  //! @class MockEthercatMasterSDORequestNodeCoE
  //! Mockup of the ethercat master parameter request node
  class MockEthercatMasterSDORequestNodeCoE : public MockEthercatMasterNodeBase
  {
  public:
    //! @brief This method implements datalayer onRead
    //! @param[in]  data        variant
    //! @param[in]  callback    callback  
    void onRead(const std::string &/*address*/, const comm::datalayer::Variant* data, const ResponseCallback &callback) override
    {
      if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) || (comm::datalayer::STATUS_FAILED(data->verifyFlatbuffers(comm::ethercat::master::fbs::VerifySDOBuffer))))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }
      auto sdoData = comm::ethercat::master::fbs::GetSDO(data->getData());
      if (!flatbuffers::IsFieldPresent(sdoData, comm::ethercat::master::fbs::SDO::VT_REQUEST)) { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); }
      comm::datalayer::Variant returnValue{};
      m_fbsHelper.generateSDOResponseFlatbuffer(&returnValue, sdoData->request()->objectIndex(), sdoData->request()->subIndex());
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }
    //! @brief This method implements datalayer onWrite
    //! @param[in]  data        variant
    //! @param[in]  callback    callback    
    void onWrite(const std::string &/*address*/, const comm::datalayer::Variant* data, const ResponseCallback &callback) override
    {
      if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) || (comm::datalayer::STATUS_FAILED(data->verifyFlatbuffers(comm::ethercat::master::fbs::VerifySDOBuffer))))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }
      auto sdoData = comm::ethercat::master::fbs::GetSDO(data->getData());
      if (!flatbuffers::IsFieldPresent(sdoData, comm::ethercat::master::fbs::SDO::VT_REQUEST))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
      }
      m_fbsHelper.writeParamFlatbuffer(sdoData->request());
      callback(comm::datalayer::DlResult::DL_OK, nullptr);
    }  

  public: // Data

    test::FlatbuffersHelperCoE m_fbsHelper{};   ///< Helper class to generate flatbuffers
  };  

  //! @class MockEthercatMasterSlaveStateNodeSoE
  //! Mockup of the ethercat master slave state node
  class MockEthercatMasterSlaveStateNodeSoE : public MockEthercatMasterNodeBase
  {
  public:
    //! @brief This method implements datalayer onRead
    //! @param[in]  data        variant
    //! @param[in]  callback    callback
    void onRead(const std::string &/*address*/, const comm::datalayer::Variant* data, const ResponseCallback &callback) override
    {
      if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) || (comm::datalayer::STATUS_FAILED(data->verifyFlatbuffers(comm::ethercat::master::fbs::VerifySlaveStateBuffer))))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }
      auto paramData = comm::ethercat::master::fbs::GetSlaveState(data->getData());
      if (!flatbuffers::IsFieldPresent(paramData, comm::ethercat::master::fbs::SlaveState::VT_REQUEST)) { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); }
      comm::datalayer::Variant returnValue{};
      m_fbsHelper.generateSlaveStateResponseFlatbuffer(&returnValue);
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  public: // Data

    test::FlatbuffersHelperSoE m_fbsHelper{};   ///< Helper class to generate flatbuffers
  };

  //! @class MockEthercatMasterSlaveStatusNodeSoE
  //! Mockup of the ethercat master slave status node
  class MockEthercatMasterSlaveStatusNodeSoE : public MockEthercatMasterNodeBase
  {
  public:
    //! @brief This method implements datalayer onRead
    //! @param[in]  data        variant
    //! @param[in]  callback    callback
    void onRead(const std::string&/*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback) override
    {
      if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) || (comm::datalayer::STATUS_FAILED(data->verifyFlatbuffers(comm::ethercat::master::fbs::VerifySlaveStatusBuffer))))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }
      auto paramData = comm::ethercat::master::fbs::GetSlaveStatus(data->getData());
      if (!flatbuffers::IsFieldPresent(paramData, comm::ethercat::master::fbs::SlaveStatus::VT_REQUEST))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
      }
      comm::datalayer::Variant returnValue {};
      m_fbsHelper.generateSlaveStatusResponseFlatbuffer(&returnValue);
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  public: // Data

    test::FlatbuffersHelperSoE m_fbsHelper {};   ///< Helper class to generate flatbuffers
  };
}
