#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_datalayer_factory.h
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
//! of the datalayer factory interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/datalayer/datalayer.h"

// Mockups
#include "mock_datalayer_client.h"
#include "mock_datalayer_provider.h"
#include "mock_datalayer_memory_user.h"


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

  //! @class MockDatalayerFactory
  //! Helper class to simulate a datalayer factory in test
  class MockDatalayerFactory : public comm::datalayer::IDataLayerFactory
  {
  public:

    //! @brief Constructor
    MockDatalayerFactory() {};
    //! @brief Destructor
    virtual ~MockDatalayerFactory() { };

    comm::datalayer::IClient* createClient(const std::string &/* remote */ = DL_INPROC_FRONTEND_SERVER) override { return m_createClientError ? nullptr : new MockDatalayerClient(); }
    comm::datalayer::IProvider* createProvider(const std::string &/* remote */ = DL_INPROC_BACKEND_SERVER) override { return m_createProviderError ? nullptr : new MockDatalayerProvider(); }
    comm::datalayer::DlResult createMemoryAsync(const std::string& /* addr */, comm::datalayer::IProvider* /* provider */, size_t /* sizeBytes */, comm::datalayer::MemoryType /* type */, comm::datalayer::Variant& /* options */, ResponseCallbackOwner& /* callback */) override {return comm::datalayer::DlResult::DL_UNSUPPORTED; };
    comm::datalayer::DlResult createMemoryAsync(const std::string &/*addr*/, comm::datalayer::IProvider* /*provider*/, size_t /*sizeBytes*/, comm::datalayer::MemoryType /*type*/, ResponseCallbackOwner &/*callback*/) override { return comm::datalayer::DlResult::DL_UNSUPPORTED; }
    comm::datalayer::DlResult createMemorySync(std::shared_ptr<::comm::datalayer::IMemoryOwner>& /* owner */, const std::string& /* addr */, comm::datalayer::IProvider* /* provider */, size_t /* sizeBytes */, comm::datalayer::MemoryType /* type */, comm::datalayer::Variant& /* options */)  override { return comm::datalayer::DlResult::DL_UNSUPPORTED; };
    comm::datalayer::DlResult createMemorySync(std::shared_ptr<comm::datalayer::IMemoryOwner>& /*owner*/, const std::string &/*addr*/, comm::datalayer::IProvider* /*provider*/, size_t /*sizeBytes*/, comm::datalayer::MemoryType /*type*/) override { return comm::datalayer::DlResult::DL_UNSUPPORTED; }
    comm::datalayer::DlResult deleteMemoryAsync(std::shared_ptr<comm::datalayer::IMemoryOwner>& /*owner*/, ResponseCallback &/*callback*/) override { return comm::datalayer::DlResult::DL_UNSUPPORTED; }
    comm::datalayer::DlResult deleteMemorySync(std::shared_ptr<comm::datalayer::IMemoryOwner>& /*owner*/) override { return comm::datalayer::DlResult::DL_UNSUPPORTED; }
    comm::datalayer::DlResult openMemory(std::shared_ptr<comm::datalayer::IMemoryUser>& user, const std::string &/*addr*/) override
    {
      if(comm::datalayer::STATUS_FAILED(m_returnResult))
      {
        user = nullptr;
        return m_returnResult;
      }

      if ( !m_memoryUser )
      {
        user = nullptr;
        return comm::datalayer::DlResult::DL_CREATION_FAILED;
      }

      user = m_memoryUser;

      return m_returnResult;
    }
    comm::datalayer::DlResult closeMemory(std::shared_ptr<comm::datalayer::IMemoryUser>& user) override
    {
      user.reset();
      return m_returnResult;
    }

    // -------------------------------- Test methods ------------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(comm::datalayer::DlResult result) { m_returnResult = result; }

    //! @brief This method can be used to set a memory user object for openMemory
    //!
    //! @param[in]    user   shared pointer to memory user
    void setMemoryUser(std::shared_ptr<test::MockMemoryUser>& user) { m_memoryUser = user; }

    //! @brief This method sets a flag that causes all future calls of createClient() to return a nullptr.
    //! @param[in]   error   Flag to indicate if error simulation should be turned on (true) or off (false).
    void simulateCreateClientError(bool error) { m_createClientError = error; }

    //! @brief This method sets a flag that causes all future calls of createProvider() to return a nullptr.
    //! @param[in]   error   Flag to indicate if error simulation should be turned on (true) or off (false).
    void simulateCreateProviderError(bool error) { m_createProviderError = error; }

  private: // data

    comm::datalayer::DlResult m_returnResult = comm::datalayer::DlResult::DL_OK;  //!< Result of member functions
    std::shared_ptr<comm::datalayer::IMemoryUser> m_memoryUser;                   //!< used memoryUserObject
    bool m_createClientError = false;                                             //!< Flag to simulate an error in createClient()
    bool m_createProviderError = false;                                           //!< Flag to simulate an error in createProvider()
  };
}


