#pragma once

/// @file
///
/// This header file is part of the axisprofile library and declares a mockup of the datalayer client component.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/datalayer/datalayer.h"
#include <vector>


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

  /// @brief MockDatalayerClientEx
  /// Helper class to simulate a datalayer client in test
  class MockDatalayerClientEx : public comm::datalayer::IClient
  {
  public: // Methods

    /// @brief Constructor
    MockDatalayerClientEx()
    {
      m_responseCallback = [&](comm::datalayer::DlResult status, const comm::datalayer::Variant* source)
      {
        if ( source )
        {
          if ( !m_vecCallBackVariant.empty() )
          {
            m_vecCallBackVariant.at(m_nCalledTimes) = *source;
          }
          else
          {
            m_callbackVariant = *source;
          }
        }

        if (!m_vecReturnValue.empty())
        {
          m_vecReturnValue.at(m_nCalledTimes) = status;
        }
        else
        {
          m_returnValue = status;
        }
      };
    }
    /// @brief Destructor
    virtual ~MockDatalayerClientEx() {}

    comm::datalayer::DlResult setTimeout(comm::datalayer::TimeoutSetting /*timeout*/, std::uint32_t /*value*/) override { return m_returnValue; }
    bool isConnected() const override { return true; }
    void setAuthToken(const std::string&/*token*/) override {}
    std::string getAuthToken() const override { return "123"; }
    comm::datalayer::DlResult getConnectStatus() override { return comm::datalayer::DlResult::DL_OK; }

    // Asynchronous requests
    /// @brief This method override a datalayer method
    /// @param[in]  callback    callback
    comm::datalayer::DlResult pingAsync(const ResponseCallback& callback) override
    {
      if ( !m_simulateTimeout )
      {
        if ( !m_vecCallBackVariant.empty() )
        {
          callback(m_callbackReturnValue, &m_vecCallBackVariant.at(m_nCalledTimes));
        }
        else
        {
          callback(m_callbackReturnValue, &m_callbackVariant);
        }
      }

      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  data        input data
    /// @param[in]  callback    callback
    comm::datalayer::DlResult createAsync(const std::string& address, const comm::datalayer::Variant& data, const ResponseCallback& callback, const std::string&/*token = {}*/) override
    {
      if ( !m_simulateTimeout )
      {
        if ( m_testNode )
        {
          m_testNode->onCreate(address, &data, callback);
        }
        else
        {
          if ( !m_vecCallBackVariant.empty() )
          {
            callback(m_callbackReturnValue, &m_vecCallBackVariant.at(m_nCalledTimes));
          }
          else
          {
            callback(m_callbackReturnValue, &m_callbackVariant);
          }
        }
      }

      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  callback    callback
    comm::datalayer::DlResult removeAsync(const std::string& address, const ResponseCallback& callback, const std::string&/*token = {}*/) override
    {
      if ( !m_simulateTimeout )
      {
        if ( m_testNode )
        {
          m_testNode->onRemove(address, callback);
        }
        else
        {
          if ( !m_vecCallBackVariant.empty() )
          {
            callback(m_callbackReturnValue, &m_vecCallBackVariant.at(m_nCalledTimes));
          }
          else
          {
            callback(m_callbackReturnValue, &m_callbackVariant);
          }
        }
      }

      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  callback    callback
    comm::datalayer::DlResult browseAsync(const std::string& address, const ResponseCallback& callback, const std::string&/*token = {}*/) override
    {
      if ( !m_simulateTimeout )
      {
        if ( m_testNode )
        {
          m_testNode->onBrowse(address, callback); }
        else
        {
          if ( !m_vecCallBackVariant.empty() )
          {
            callback(m_callbackReturnValue, &m_vecCallBackVariant.at(m_nCalledTimes));
          }
          else
          {
            callback(m_callbackReturnValue, &m_callbackVariant);
          }
        }
      }

      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  data        input data
    /// @param[in]  callback    callback
    comm::datalayer::DlResult readAsync(const std::string& address, const comm::datalayer::Variant& data, const ResponseCallback& callback, const std::string&/*token = {}*/) override
    {
      if ( !m_simulateTimeout )
      {
        if ( m_testNode )
        {
          m_testNode->onRead(address, &data, callback);
        }
        else
        {
          callback(m_callbackReturnValue, &m_vecCallBackVariant.at(m_nCalledTimes));
        }
      }

      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  data        input data
    /// @param[in]  callback    callback
    comm::datalayer::DlResult writeAsync(const std::string& address, const comm::datalayer::Variant& data, const ResponseCallback& callback, const std::string&/*token = {}*/) override
    {
      if ( !m_simulateTimeout )
      {
        if ( m_testNode )
        {
          m_testNode->onWrite(address, &data, callback);
        }
        else
        {
          if ( !m_vecCallBackVariant.empty() )
          {
            callback(m_callbackReturnValue, &m_vecCallBackVariant.at(m_nCalledTimes));
          }
          else
          {
            callback(m_callbackReturnValue, &m_callbackVariant);
          }
        }
      }

      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  callback    callback
    comm::datalayer::DlResult metadataAsync(const std::string& address, const ResponseCallback& callback, const std::string&/*token = {}*/) override
    {
      if ( !m_simulateTimeout )
      {
        if ( m_testNode )
        {
          m_testNode->onMetadata(address, callback);
        }
        else
        {
          if ( !m_vecCallBackVariant.empty() )
          {
            callback(m_callbackReturnValue, &m_vecCallBackVariant.at(m_nCalledTimes));
          }
          else
          {
            callback(m_callbackReturnValue, &m_callbackVariant);
          }
        }
      }

      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }

    // Synchronous requests
    /// @brief This method override a datalayer method
    comm::datalayer::DlResult pingSync() override
    {
      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  data        input data
    comm::datalayer::DlResult createSync(const std::string& address, comm::datalayer::Variant* data, const std::string&/*token = {}*/) override
    {
      if ( m_testNode )
      {
        m_testNode->onCreate(address, data, m_responseCallback);
      }
      if ( data )
      {
        if ( !m_vecCallBackVariant.empty() )
        {
          *data = m_vecCallBackVariant.at(m_nCalledTimes);
        }
        else
        {
          *data = m_callbackVariant;
        }
      }

      comm::datalayer::DlResult retValue = m_returnValue;
      if (!m_vecReturnValue.empty())
      {
        retValue = m_vecReturnValue.at(m_nCalledTimes);
        m_nCalledTimes++;
      }
      return retValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    comm::datalayer::DlResult removeSync(const std::string& address, const std::string&/*token = {}*/) override
    {
      if ( m_testNode ) { m_testNode->onRemove(address, m_responseCallback); }
      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  data        input data
    comm::datalayer::DlResult browseSync(const std::string& address, comm::datalayer::Variant* data, const std::string&/*token = {}*/) override
    {
      if (m_testNode)
      {
        m_testNode->onBrowse(address, m_responseCallback);
      }
      if (data)
      {
        if (!m_vecCallBackVariant.empty())
        {
          *data = m_vecCallBackVariant.at(m_nCalledTimes);
        }
        else
        {
          *data = m_callbackVariant;
        }
      }

      comm::datalayer::DlResult retValue = m_returnValue;
      if (!m_vecReturnValue.empty())
      {
        retValue = m_vecReturnValue.at(m_nCalledTimes);
      }
      m_nCalledTimes++;
      return retValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  data        input data
    comm::datalayer::DlResult readSync(const std::string& address, comm::datalayer::Variant* data, const std::string&/*token = {}*/) override
    {
      if ( m_testNode ) { m_testNode->onRead(address, data, m_responseCallback); }
      if (data)
      {
        if (!m_vecCallBackVariant.empty())
        {
          *data = m_vecCallBackVariant.at(m_nCalledTimes);
        }
        else
        {
          *data = m_callbackVariant;
        }
      }

      comm::datalayer::DlResult retValue = m_returnValue;
      if (!m_vecReturnValue.empty())
      {
        retValue = m_vecReturnValue.at(m_nCalledTimes);
      }
      m_nCalledTimes++;
      return retValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  data        input data
    comm::datalayer::DlResult writeSync(const std::string& address, comm::datalayer::Variant* data, const std::string&/*token = {}*/) override
    {
      if ( m_testNode && data ) { m_testNode->onWrite(address, data, m_responseCallback); }
      if (!m_vecReturnValue.empty())
      {
        return m_vecReturnValue.at(m_nCalledTimes);
      }
      return m_returnValue;
    }
    /// @brief This method override a datalayer method
    /// @param[in]  address     address
    /// @param[in]  data        input data
    comm::datalayer::DlResult metadataSync(const std::string& address, comm::datalayer::Variant* data, const std::string&/*token = {}*/) override
    {
      if ( m_testNode ) { m_testNode->onMetadata(address, m_responseCallback); }
      if (data)
      {
        if (!m_vecCallBackVariant.empty())
        {
          *data = m_vecCallBackVariant.at(m_nCalledTimes);
        }
        else
        {
          *data = m_callbackVariant;
        }
      }

      comm::datalayer::DlResult retValue = m_returnValue;
      if (!m_vecReturnValue.empty())
      {
        retValue = m_vecReturnValue.at(m_nCalledTimes);
      }
      m_nCalledTimes++;
      return retValue;
    }

  public: // test method

    /// @brief Method to reset buffered data and counter
    void resetClient()
    {
      m_nCalledTimes = 0;
      m_vecCallBackVariant.clear();
      m_vecReturnValue.clear();
      m_responseCallback = nullptr;
      m_simulateTimeout = 0;
    }

  public: // Data

    comm::datalayer::DlResult m_returnValue = comm::datalayer::DlResult::DL_OK;                 ///< Result of member functions
    std::vector<comm::datalayer::DlResult> m_vecReturnValue {};                                 ///< Result of member functions for async methods
    comm::datalayer::DlResult m_callbackReturnValue = comm::datalayer::DlResult::DL_OK;         ///< Result of callback
    comm::datalayer::Variant m_callbackVariant;                                                 ///< callback variant for the async methods
    std::vector<comm::datalayer::Variant> m_vecCallBackVariant{ };                              ///< callback variant vector for the async/sync methods
    std::uint32_t m_nCalledTimes{};                                                             ///< counting reference, used for different entrance match
    ResponseCallback m_responseCallback = nullptr;                                              ///< response callback of the actual async method
    comm::datalayer::IProviderNode* m_testNode{};                                               ///< Datalayer node that is used inside the client
    bool m_simulateTimeout{};                                                                   ///< Flag that prevents the callback execution on asynchronous requests to simulate a timeout
  };
}
