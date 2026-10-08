#pragma once

/// @file
///
/// This source file is part of the axisprofile library and implement mock ethercat master nodes.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "mock_ethercat_master_defs.h"
#include "comm/datalayer/datalayer.h"
#include "helper/flatbuffers_helper_common.h"

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

  //! @class MockEthercatMasterNodeBase
  //! Base class for ethercat master node mockups
  class MockEthercatMasterNodeBase : public comm::datalayer::IProviderNode
  {
  public:
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onCreate(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const comm::datalayer::IProviderNode::ResponseCallback &callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onRemove(const std::string &/*address*/, const ResponseCallback &callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onBrowse(const std::string &/*address*/, const ResponseCallback &callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onRead(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback &callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onWrite(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback &callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onMetadata(const std::string &/*address*/, const ResponseCallback &callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
  };

  //! @class MockEthercatMasterInstancesNode
  //! Mockup of the ethercat master instances node
  class MockEthercatMasterInstancesNode : public MockEthercatMasterNodeBase
  {
  public: 
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onBrowse(const std::string &/*address*/, const ResponseCallback &callback) override
    {
      comm::datalayer::Variant data{};
      std::vector<std::string> instanceNames{MOCK_ECAT_MASTER_INSTANCE_NAME};
      data.setValue(instanceNames);
      callback(comm::datalayer::DlResult::DL_OK, &data);
    }
  };

  //! @class MockEthercatMasterStateNode
  //! Mockup of the ethercat master fieldbus state node
  class MockEthercatMasterStateNode : public MockEthercatMasterNodeBase
  {
  public:
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onRead(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback &callback) override
    {
      comm::datalayer::Variant returnValue{};
      m_fbsHelper.generateMasterStateResponseFlatbuffer(&returnValue);
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  public: // Data

    test::FlatbuffersHelperCommon m_fbsHelper{};   //!< Helper class to generate flatbuffers
  };

  //! @class MockEthercatMasterStatusNode
  //! Mockup of the ethercat master fieldbus status node
  class MockEthercatMasterStatusNode : public MockEthercatMasterNodeBase
  {
  public:
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    void onRead(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback) override
    {
      comm::datalayer::Variant returnValue {};
      m_fbsHelper.generateMasterStatusResponseFlatbuffer(&returnValue);
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  public: // Data

    test::FlatbuffersHelperCommon m_fbsHelper {};   //!< Helper class to generate flatbuffers
  };

  /// @brief This is the mockup of the slave DC config information node
  class MockEthercatMasterSlaveDcConfigInfoNode : public MockEthercatMasterNodeBase
  {
  public: // methods

    void onRead(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback) override
    {
      comm::datalayer::Variant returnValue {};
      m_fbsHelper.generateSlaveDcCfgInfoResponseFlatbuffer(&returnValue);
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  public: // Data

    test::FlatbuffersHelperCommon m_fbsHelper {};   //!< Helper class to generate flatbuffers
  };

  /// @brief This is the mockup of the slave address mapping information
  class MockEthercatMasterSlaveAddressMapping :public MockEthercatMasterNodeBase
  {
    void onRead(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback) override
    {
      comm::datalayer::Variant returnValue {};
      m_fbsHelper.generateAddressMappingResponseFlatbuffer(cnt, returnValue);
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  public: // Data

    test::FlatbuffersHelperCommon m_fbsHelper {};   ///< Helper class to generate flatbuffers
    uint16_t cnt {1};                               ///< The slave device count.
  };

  /// @brief This is the mockup of the slave config information node
  class MockEthercatMasterCfgSlaveInfo :public MockEthercatMasterNodeBase
  {
    void onRead(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback) override
    {
      comm::datalayer::Variant returnValue {};
      m_fbsHelper.generateSlaveCfgInfoResponseFlatbuffer(m_slaveName, m_devInfo, &returnValue);
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  public: // Data

    test::FlatbuffersHelperCommon m_fbsHelper {};               ///< Helper class to generate flatbuffers
    std::string m_slaveName {MOCK_ECAT_SLAVE_DEVICE_NAME};      ///< The mock slave device name
    comm::axisprofile::DeviceHardwareInfo m_devInfo {1001, 26, 36, 0x20004311, MOCK_ECAT_MASTER_INSTANCE_NAME};  ///< mock device information
  };

}
