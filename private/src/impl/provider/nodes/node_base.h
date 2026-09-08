#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and serves as a template
/// for nodes in the data layer.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/metadata_generated.h"
#include "i_node_base.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class ProviderMgr;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This is the node class.
  class NodeBase : public comm::axisprofile::provider::INodeBase
  {
  public: // methods

    /// Constructor
    explicit NodeBase(ProviderMgr *mgr);

    /// Destructor
    virtual ~NodeBase() noexcept override = default;

    void onCreate(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) override;
    void onRemove(const std::string &address, const ResponseCallback &callback) override;
    void onBrowse(const std::string &address, const ResponseCallback &callback) override;
    void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) override;
    void onWrite(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) override;
    void onMetadata(const std::string &address, const ResponseCallback &callback) override;
    const char* getBfbsName() override;
    const char* getDescription() override;
    const char* getDescriptionUrl() override;

    /// @brief Helper for onMetaData of a node. Generates type of the node from the address.
    /// @param[in]    address             Address of the node to create object in
    /// @param[in]    profileName         Name of the axisprofile.
    /// @param[out]   typeString          Address of the type of the node
    /// @param[in]    callback            Callback to call when function is finished
    /// @result         status of function call (false = callback got executed)
    bool createTypeString(const std::string &address, const std::string& profileName, std::string &typeString, const ResponseCallback &callback);

    /// @brief Helper for onMetaData to create a basic type string.
    /// Generates the type of the node from the simpleTypeID (ID of the simple type).
    /// Performs checks like kin/axs object exists etc.
    /// @param[in]      profileName   profile name
    /// @param[in]      simpleTypeID  ID of the simple type
    /// @param[out]     typeString    Address of the type of the node
    /// @param[in]      callback      Callback to call when function is finished
    /// @param[in]      requireIF     are the bundle interfaces required?
    /// @result         status of function call (false = callback got executed)
    bool createBasicTypeString(const std::string& profileName, comm::datalayer::VariantType simpleTypeID, std::string& typeString, const ResponseCallback& callback, bool requireIF = true);

  protected: //data

    ProviderMgr* m_manager{ };                        ///< pointer to the provider manager (access to the system)
    MetaDataInfo m_metaDataInfo{};                    ///< metadata info structure
  };
}
