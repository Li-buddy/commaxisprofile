#pragma once

/// @file 
///
/// @brief This file is part of the axisprofile library and implements the node configuration make persistent class
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_base.h"
#include "comm/datalayer/persistence_param_generated.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
class IBackupRestoreMgr;
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

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is datalayer node for the make persistent.
  class NodeCfgMakePersistent : public NodeBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   mgr           pointer to the provider manager
    explicit NodeCfgMakePersistent(ProviderMgr* mgr);

    /// Destructor
    virtual ~NodeCfgMakePersistent() noexcept override = default;
    void onCreate(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

  private: // methods

    /// @brief worker function for thread, that calls the backupRestoreMgr
    /// Pass parameters by value to ensure that the strings are valid after returning from the calling function!
    /// The resulting CppCheck warnings have to be suppressed.
    /// @param[in] backupRestoreMgr  the backup+restore manager
    /// @param[in] persistenceParam  The variant with the persistence parameters (path, id, phase)
    /// @param[in] axisprofileName   Optional (only for save/load of a single axisprofile object). The name of the axisprofile.
    /// @param[in] callback          The callback to be called with the result of the operation
    static void doOperation(comm::axisprofile::IBackupRestoreMgr* backupRestoreMgr, const comm::datalayer::Variant& persistenceParam, const std::string& axisprofileName, const ResponseCallback& callback);
  };
}
