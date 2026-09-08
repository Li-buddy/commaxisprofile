#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_datalayer_provider.h
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
//! of the datalayer provider interface.
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

  //! @class MockDatalayerProvider
  //! Helper class to simulate a datalayer provider in test
  class MockDatalayerProvider : public comm::datalayer::IProvider
  {
  public:

    //! @brief Constructor
    MockDatalayerProvider() = default;
    //! @brief Destructor
    virtual ~MockDatalayerProvider() = default;

    virtual comm::datalayer::DlResult registerType(const std::string& /*address*/, const std::string& /*pathname*/) override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual comm::datalayer::DlResult registerType(const std::string& /* address */, const comm::datalayer::Variant& /* type */) override { return comm::datalayer::DlResult::DL_FAILED; };
    virtual comm::datalayer::DlResult unregisterType(const std::string& /*address*/) override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual comm::datalayer::DlResult getRegisteredType(const std::string& /* address */, comm::datalayer::Variant& /* type */) override { return comm::datalayer::DlResult::DL_FAILED; };
    virtual comm::datalayer::DlResult registerNode(const std::string& /*address*/, comm::datalayer::IProviderNode* /*node*/) override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual comm::datalayer::DlResult registerNode(const char* /*address*/, comm::datalayer::IProviderNode* /*node*/) override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual comm::datalayer::DlResult unregisterNode(const std::string& /*address*/) override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual comm::datalayer::DlResult unregisterNode(const char* /*address*/) override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual comm::datalayer::DlResult setTimeoutNode(comm::datalayer::IProviderNode* /*node*/, std::uint32_t /*timeoutMS*/) override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual comm::datalayer::DlResult start() override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual comm::datalayer::DlResult stop() override { return comm::datalayer::DlResult::DL_FAILED; }
    virtual bool isConnected() override { return false; }
    virtual const comm::datalayer::Variant& getToken() override { return m_token; }

  private: // data

    comm::datalayer::Variant m_token{};   //!< Token of the current request
  };
}


