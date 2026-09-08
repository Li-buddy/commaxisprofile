#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_provider_node.h
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
//! of the axisprofile provider node interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider/nodes/i_node_base.h"


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

  //! @class MockProviderNode
  //! Mockup of the axisprofile provider node interface.
  class MockProviderNode : public comm::axisprofile::provider::INodeBase
  {
  public:
    //! @brief constructor
    MockProviderNode() = default;
    //! @brief destructor
    virtual ~MockProviderNode() noexcept override = default;

    void onCreate(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback &callback) override { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); };
    void onRemove(const std::string &/*address*/, const ResponseCallback &callback) override { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); };
    void onBrowse(const std::string &/*address*/, const ResponseCallback &callback) override { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); };
    void onRead(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback &callback) override { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); };
    void onWrite(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback &callback) override { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); };
    void onMetadata(const std::string &/*address*/, const ResponseCallback &callback) override { callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr); };
    const char* getBfbsName() override { return m_bfbsName; };
    const char* getDescription() override { return m_description; };
    const char* getDescriptionUrl() override { return m_descriptionUrl; };

    // ----------------------------------------- Test methods ----------------------------------------

    //! @brief This method sets the Bfbs name in the mockup.
    //! @param[in]    bfbsName   Name of the Bfbs file.
    void setBfbsName(const char* bfbsName) { if (bfbsName) { m_bfbsName = bfbsName; } }

    //! @brief Set the description of the flatbuffer required for meta data.
    //! @param[in]    description   Description for flatbuffer meta data
    void setDescription(const char* description) { if (description) { m_description = description; } }

    //! @brief Set the description URL of the flatbuffer required for meta data.
    //! @param[in]    descriptionUrl   Description URL for flatbuffer meta data
    void setDescriptionUrl(const char* descriptionUrl) { if (descriptionUrl) { m_descriptionUrl = descriptionUrl; } }

  private: // data
    const char* m_bfbsName = nullptr;
    const char* m_description = nullptr;
    const char* m_descriptionUrl = nullptr;
  };
}
