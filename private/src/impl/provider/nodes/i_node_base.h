#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines axisprofile inherits
/// from the provider node and adds functions which provide information for
/// the creation of metadata for nodes with flatbuffers.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/datalayer.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm::datalayer
{
  enum NodeClass : std::int8_t;
}

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
  /// @brief Struct that holds all meta data information for a node
  struct MetaDataInfo
  {
    const char* m_description {};               ///< metadata info: description of data
    const char* m_descriptionUrl {};            ///< metadata info: Url of description
    const char* m_bfbsPath {};                  ///< metadata info: path to bfbs file for flatbuffer nodes
    comm::datalayer::NodeClass m_nodeClass {};  ///< metadata info: class of the node
    std::uint32_t nodeActions {};               ///< metadata info: which operations are supported by the node
  };

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface of the base node class.
  class INodeBase : public comm::datalayer::IProviderNode
  {
  public: // methods

    /// Destructor
    virtual ~INodeBase() noexcept override = default;

    /// Create function of an object. Function will be called whenever a object should be created.
    /// @param[in]  address   Address of the node to create object in
    /// @param[in]  data      Data of the object
    /// @param[in]  callback  Callback to call when function is finished
    virtual void onCreate(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) override = 0;

    /// Remove function for an object. Function will be called whenever a object should be removed.
    /// @param[in]  address   Address of the node to remove
    /// @param[in]  callback  Callback to call when function is finished
    virtual void onRemove(const std::string &address, const ResponseCallback &callback) override = 0;

    /// Browse function of a node. Function will be called to determine children of a node.
    /// @param[in]  address   Address of the node to browse
    /// @param[in]  callback  Callback to call when function is finished
    virtual void onBrowse(const std::string &address, const ResponseCallback &callback) override = 0;

    /// Read function of a node. Function will be called whenever a node should be read.
    /// @param[in]  address   Address of the node to read
    /// @param[in]  data      Data of the object
    /// @param[in]  callback  Callback to call when function is finished
    virtual void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) override = 0;

    /// Write function of a node. Function will be called whenever a node should be written.
    /// @param[in]  address   Address of the node to write
    /// @param[in]  data      Data of the object
    /// @param[in]  callback  Callback to call when function is finished
    virtual void onWrite(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) override = 0;

    /// Read function of metadata of an object. Function will be called whenever a node should be written.
    /// @param[in]  address   Address of the node to read metadata of
    /// @param[out] callback  Metadata of the node. Data will be provided as Variant flatbuffers with metadata.fbs data type.
    virtual void onMetadata(const std::string &address, const ResponseCallback &callback) override = 0;

    /// @brief Returns the name of the bfbs file of the flatbuffer required for meta data
    /// @return   name of bfbs file
    virtual const char* getBfbsName() = 0;

    /// @brief Returns the description of the flatbuffer required for meta data
    /// @return   description of the flatbuffer
    virtual const char* getDescription() = 0;

    /// @brief Returns the description url of the flatbuffer required for meta data
    /// @return   description url of the flatbuffer
    virtual const char* getDescriptionUrl() = 0;
  };
}
