#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_ethercat_master_nodes.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2020
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and declares a
//! mockup of an ethercast master datalayer node.
//
// ---------------------------------------------------------------------------


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

}
