#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the node
/// to set the device address of a specific profile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the node class to set/get the device address of a profile.
  class NodeProfilesDeviceAddress : public NodeProfilesBase
  {
  public: // methods

    /// Constructor
    /// @param[in]   mgr   Pointer to the provider manager
    explicit NodeProfilesDeviceAddress(ProviderMgr *mgr);

    /// Destructor
    virtual ~NodeProfilesDeviceAddress() noexcept override = default;

    void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
    void onWrite(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;

    /// @brief Response for metadata request
    /// @param[in]   address    address describe the uri
    /// @param[in]   callback   execute after the response
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;
  };
}
