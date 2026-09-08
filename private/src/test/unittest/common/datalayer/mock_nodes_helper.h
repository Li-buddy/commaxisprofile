#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define helper class of datalayer mock nodes for function test.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/datalayer/datalayer.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  const std::string TEST_MOCK_OKAY_URL  = "test/mock/address/correct-url";    ///< mock address of correct node
  const std::string TEST_MOCK_ERROR_URL = "test/mock/address/incorrect-url";  ///< mock address of incorrect node

  /// @brief List of mockup nodes
  const std::vector<std::string> MOCK_URLS {
    {TEST_MOCK_OKAY_URL + "1"},
    {TEST_MOCK_OKAY_URL + "2"},
    {TEST_MOCK_OKAY_URL + "3"},
    {TEST_MOCK_OKAY_URL + "4"},
    {TEST_MOCK_OKAY_URL + "5"},
  };

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief Base class for helper nodes
  class MockHelperNodeBase : public comm::datalayer::IProviderNode
  {
  public: // methods

    /// @brief This method override a datalayer method
    /// @param[in]  callback    callback
    void onCreate(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const comm::datalayer::IProviderNode::ResponseCallback& callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    /// @brief This method override a datalayer method
    /// @param[in]  callback    callback
    void onRemove(const std::string&/*address*/, const ResponseCallback& callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    /// @brief This method override a datalayer method
    /// @param[in]  callback    callback
    void onBrowse(const std::string&/*address*/, const ResponseCallback& callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    /// @brief This method override a datalayer method
    /// @param[in]  callback    callback
    void onRead(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    /// @brief This method override a datalayer method
    /// @param[in]  callback    callback
    void onWrite(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
    /// @brief This method override a datalayer method
    /// @param[in]  callback    callback
    void onMetadata(const std::string&/*address*/, const ResponseCallback& callback) override
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }
  };

  /// @brief Simple type class for helper nodes
  template <typename SIMPLE_TYPE>
  class MockHelperNodeSimpleType : public MockHelperNodeBase
  {
  public: // methods

    void onRead(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback) override;
    void onWrite(const std::string&/*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback) override;

  public: // data

    SIMPLE_TYPE m_data {}; ///< data of the node
  };

  template <typename SIMPLE_TYPE>
  void MockHelperNodeSimpleType<SIMPLE_TYPE>::onRead(const std::string&/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
  {
    comm::datalayer::Variant retVariant;
    retVariant.setValue(m_data);

    callback(comm::datalayer::DlResult::DL_OK, &retVariant);
  }

  template <typename SIMPLE_TYPE>
  void MockHelperNodeSimpleType<SIMPLE_TYPE>::onWrite(const std::string&/*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback)
  {
    if (!data)
    {
      callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
    }

    if (data->getType() != comm::datalayer::getVariantTypeID<SIMPLE_TYPE>())
    {
      callback(comm::datalayer::DlResult::DL_TYPE_MISMATCH, nullptr);
      return;
    }

    m_data = *data;
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }

  /// @brief struct for every known node
  struct NodeEntry
  {
    std::string m_address {};      ///< address of this node
    MockHelperNodeBase* m_node {}; ///< the node itself
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is helper class of mockup datalayer nodes
  class MockNodesHelper
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  datalayerFactory    Datalayer factory
    explicit MockNodesHelper(comm::datalayer::IDataLayerFactory* datalayerFactory);
    /// @brief Destructor
    virtual ~MockNodesHelper();

    /// @brief This method initializes the class.
    /// @return Return value indicating success or error.
    HRESULT init();

    /// @brief This method deinitialize the class.
    /// @return Return value indicating success or error.
    HRESULT deInit();

  private: // data

    comm::datalayer::IProvider* m_provider {};                 ///< Pointer to the datalayer provider
    comm::datalayer::IDataLayerFactory* m_datalayerFactory {}; ///< Pointer to the datalayer factory

    std::vector<NodeEntry> m_nodes;          /// all mock nodes
  };
}
