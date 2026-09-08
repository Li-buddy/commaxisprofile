#pragma once
/// @file
///
/// This header file is part of the axisprofile library and defines the node information for checksum IDN list.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
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

  /// @brief This is the datalayer node to show the checksum idn list of the device.
  class NodeProfilesChecksumList : public NodeProfilesBase
  {
  public: // methods

    /// Constructor
    /// @param[in]   mgr     pointer to the provider manager
    explicit NodeProfilesChecksumList(ProviderMgr* mgr);

    /// Destructor
    virtual ~NodeProfilesChecksumList() noexcept override = default;

    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;
  };
}
