#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define header of wrapped datalayer client.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "boost/lockfree/spsc_queue.hpp"
#include "i_client_internal.h"
#include <atomic>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::datalayer
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  namespace async
  {
    constexpr std::size_t RING_BUFF_SIZE = 1000; ///< Size of ring buffer of spsc_queue,
                                                 ///< In current cases, each profile has maximum 1 async request every cycle
  }

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief This type struct datalayer read/write request and response.
  struct DlAsync
  {
    std::string address;                    ///< datalayer url
    Variant data;                           ///< request data
    IClient::ResponseCallback callback;     ///< response callback function
    std::string token = {};                 ///< token
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes
  // ---------------------------------------------------------------------------

  /// @brief Internal class wrapper on IClinet.
  class ClientInternal : public IClientInternal
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  client      Datalayer client to be wrapped
    explicit ClientInternal(IClient* client);
    /// @brief Destructor
    virtual ~ClientInternal() noexcept override = default;

    IClient* getBase() final;
    std::function<void(void)> getBackgroundFun() final;
    void enableBackgroundFun(bool enable) final;

    // ------------------------------------ Datalayer Methods move to background thread --------------------------------- //
    // only wrap at readAsync and writeAsync, other methods are not used in realtime context
    DlResult pingAsync(const ResponseCallback& callback) final { return m_base->pingAsync(callback); };
    DlResult createAsync(const std::string& address, const Variant& data, const ResponseCallback& callback, const std::string& token = {}) { return m_base->createAsync(address, data, callback, token); };
    DlResult removeAsync(const std::string& address, const ResponseCallback& callback, const std::string& token = {}) final { return m_base->removeAsync(address, callback, token); };
    DlResult browseAsync(const std::string& address, const ResponseCallback& callback, const std::string& token = {}) final { return m_base->browseAsync(address, callback, token); };
    DlResult readAsync(const std::string& address, const Variant& data, const ResponseCallback& callback, const std::string& token = {}) final;
    DlResult writeAsync(const std::string& address, const Variant& data, const ResponseCallback& callback, const std::string& token = {}) final;
    DlResult metadataAsync(const std::string& address, const ResponseCallback& callback, const std::string& token = {}) final { return m_base->metadataAsync(address, callback, token); }

    // ------------------------------------ Datalayer Methods keep raw thread --------------------------------- //
    DlResult pingSync() final { return m_base->pingSync(); };
    DlResult createSync(const std::string& address, Variant* data, const std::string& token = {}) final { return m_base->createSync(address, data, token); };
    DlResult removeSync(const std::string& address, const std::string& token = {}) final { return m_base->removeSync(address, token); };
    DlResult browseSync(const std::string& address, Variant* data, const std::string& token = {}) final { return m_base->browseSync(address, data, token); };
    DlResult readSync(const std::string& address, Variant* data, const std::string& token = {}) final { return m_base->readSync(address, data, token); };
    DlResult writeSync(const std::string& address, Variant* data, const std::string& token = {}) final { return m_base->writeSync(address, data, token); };
    DlResult metadataSync(const std::string& address, Variant* data, const std::string& token = {}) final { return m_base->metadataSync(address, data, token); };
    DlResult setTimeout(TimeoutSetting timeout, std::uint32_t value) final { return m_base->setTimeout(timeout, value); };
    bool isConnected() const final {return m_base->isConnected(); };
    DlResult getConnectStatus() final { return m_base->getConnectStatus(); };
    void setAuthToken(const std::string& token) final { m_base->setAuthToken(token); };
    std::string getAuthToken() const final { return m_base->getAuthToken(); };

  private: // methods

    /// @brief This is the method be called in background thread.
    void cycleInThreadNRT(void);

  private: // data

    std::atomic_bool m_enableBkFun {false}; ///< Enable background function
    std::unique_ptr<IClient> m_base;        ///< base client from service

    // Note: be sure that there is no use case of 3 producers (motion's booting thread, execution callable and prepare thread) push to FIFO same time
    //       better container is `boost::lockfree::queue`, however there is still technical issue to be solved(copy constructor, trivial assignment and trivial destructor)
    //       this may lead more change from current implementation
    boost::lockfree::spsc_queue<DlAsync, boost::lockfree::capacity<async::RING_BUFF_SIZE>> m_readList;  ///< FIFO of async reading
    boost::lockfree::spsc_queue<DlAsync, boost::lockfree::capacity<async::RING_BUFF_SIZE>> m_writeList; ///< FIFO of async writing
  };
}
