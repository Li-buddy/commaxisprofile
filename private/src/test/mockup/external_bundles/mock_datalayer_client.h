#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_datalayer_client.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and declares a mockup
//! of the datalayer client component.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/datalayer/datalayer.h"


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

  //! @class MockDatalayerClient
  //! Helper class to simulate a datalayer client in test
  class MockDatalayerClient : public comm::datalayer::IClient
  {
  public: // Methods

    //! @brief Constructor
    MockDatalayerClient()
    {
      m_responseCallback = [&](comm::datalayer::DlResult status, const comm::datalayer::Variant *source)
      {
        if (source) { m_callbackVariant = *source; }
        m_returnValue = status;
      };
    }
    //! @brief Destructor
    virtual ~MockDatalayerClient() {}

    comm::datalayer::DlResult setTimeout(comm::datalayer::TimeoutSetting /*timeout*/, std::uint32_t /*value*/) override { return m_returnValue; }
    bool isConnected() const override { return true; }
    void setAuthToken(const std::string &/*token*/) override {}
    std::string getAuthToken() const override { return "123"; }
    comm::datalayer::DlResult getConnectStatus() override { return comm::datalayer::DlResult::DL_OK; }

    // Asynchronous requests
    //! @brief This method override a datalayer method
    //! @param[in]  callback    callback
    comm::datalayer::DlResult pingAsync(const ResponseCallback &callback) override
    {
      if (!m_simulateTimeout) { callback(m_callbackReturnValue, &m_callbackVariant); }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  data        input data
    //! @param[in]  callback    callback
    comm::datalayer::DlResult createAsync(const std::string &address, const comm::datalayer::Variant &data, const ResponseCallback &callback, const std::string &/*token = {}*/) override
    {
      if (!m_simulateTimeout) { if (m_testNode) { m_testNode->onCreate(address, &data, callback); } else { callback(m_callbackReturnValue, &m_callbackVariant); } }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  callback    callback
    comm::datalayer::DlResult removeAsync(const std::string &address, const ResponseCallback &callback, const std::string &/*token = {}*/) override
    {
      if (!m_simulateTimeout) { if (m_testNode) { m_testNode->onRemove(address, callback); } else { callback(m_callbackReturnValue, &m_callbackVariant); } }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  callback    callback
    comm::datalayer::DlResult browseAsync(const std::string &address, const ResponseCallback &callback, const std::string &/*token = {}*/) override
    {
      if (!m_simulateTimeout) { if (m_testNode) { m_testNode->onBrowse(address, callback); } else { callback(m_callbackReturnValue, &m_callbackVariant); } }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  data        input data
    //! @param[in]  callback    callback
    comm::datalayer::DlResult readAsync(const std::string &address, const comm::datalayer::Variant &data, const ResponseCallback &callback, const std::string &/*token = {}*/) override
    {
      if (!m_simulateTimeout) { if (m_testNode) { m_testNode->onRead(address, &data, callback); } else { callback(m_callbackReturnValue, &m_callbackVariant); } }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  data        input data
    //! @param[in]  callback    callback
    comm::datalayer::DlResult writeAsync(const std::string &address, const comm::datalayer::Variant &data, const ResponseCallback &callback, const std::string &/*token = {}*/) override
    {
      if (!m_simulateTimeout) { if (m_testNode) { m_testNode->onWrite(address, &data, callback); } else { callback(m_callbackReturnValue, &m_callbackVariant); } }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  callback    callback
    comm::datalayer::DlResult metadataAsync(const std::string &address, const ResponseCallback &callback, const std::string &/*token = {}*/) override
    {
      if (!m_simulateTimeout) { if (m_testNode) { m_testNode->onMetadata(address, callback); } else { callback(m_callbackReturnValue, &m_callbackVariant); } }
      return m_returnValue;
    }

    // Synchronous requests
    //! @brief This method override a datalayer method
    comm::datalayer::DlResult pingSync() override { return m_returnValue; }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  data        input data
    comm::datalayer::DlResult createSync(const std::string &address, comm::datalayer::Variant *data, const std::string &/*token = {}*/) override
    {
      if (m_testNode) { m_testNode->onCreate(address, data, m_responseCallback); } if (data) { *data = m_callbackVariant; }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    comm::datalayer::DlResult removeSync(const std::string &address, const std::string &/*token = {}*/) override
    {
      if (m_testNode) { m_testNode->onRemove(address, m_responseCallback); }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  data        input data
    comm::datalayer::DlResult browseSync(const std::string &address, comm::datalayer::Variant *data, const std::string &/*token = {}*/) override
    {
      if (m_testNode) { m_testNode->onBrowse(address, m_responseCallback); } if (data) { *data = m_callbackVariant; }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  data        input data
    comm::datalayer::DlResult readSync(const std::string &address, comm::datalayer::Variant *data, const std::string &/*token = {}*/) override
    {
      if (m_testNode) { m_testNode->onRead(address, data, m_responseCallback); }
      if (data) { *data = m_callbackVariant; }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  data        input data
    comm::datalayer::DlResult writeSync(const std::string &address, comm::datalayer::Variant *data, const std::string &/*token = {}*/) override
    {
      if (m_testNode && data) { m_testNode->onWrite(address, data, m_responseCallback); }
      return m_returnValue;
    }
    //! @brief This method override a datalayer method
    //! @param[in]  address     address
    //! @param[in]  data        input data
    comm::datalayer::DlResult metadataSync(const std::string &address, comm::datalayer::Variant *data, const std::string &/*token = {}*/) override
    {
      if (m_testNode) { m_testNode->onMetadata(address, m_responseCallback); } if (data) { *data = m_callbackVariant; }
      return m_returnValue;
    }

  public: // Data

    comm::datalayer::DlResult m_returnValue = comm::datalayer::DlResult::DL_OK;           //!< Result of member functions
    comm::datalayer::DlResult m_callbackReturnValue = comm::datalayer::DlResult::DL_OK;   //!< Result of callback
    comm::datalayer::Variant m_callbackVariant;                                           //!< callback variant for the async methods
    ResponseCallback m_responseCallback = nullptr;                                        //!< response callback of the actual async method
    comm::datalayer::IProviderNode* m_testNode{};                                         //!< Datalayer node that is used inside the client
    bool m_simulateTimeout{};                                                             //!< Flag that prevents the callback execution on asynchronous requests to simulate a timeout
  };
}
