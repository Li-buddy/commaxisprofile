#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and declare the node of axisprofile EtherCAT name
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

  /// @brief This is the datalayer node to configure the EtherCAT master instance name.
  class NodeProfilesEtherCATMasterName : public NodeProfilesBase
  {
  public: // methods

    /// @param[in]   mgr         pointer to the provider manager
    explicit NodeProfilesEtherCATMasterName(ProviderMgr* mgr);

    virtual ~NodeProfilesEtherCATMasterName() noexcept override = default;

    void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;
  };
}
