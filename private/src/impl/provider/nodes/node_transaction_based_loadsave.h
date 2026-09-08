#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements the node class
/// to transaction oriented save and load data of the axisprofiles. The class uses the
/// backup_restore_mgr.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/persistence_param_generated.h"
#include "node_base.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
class IBackupRestoreMgr;
} // namespace comm

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  /// Operation which should be executed when calling onCreate() of this class
  enum class PersistanceOperation
  {
    SAVE_AXISPROFILE = 0, ///< Save the complete axisprofile
    LOAD_AXISPROFILE      ///< Load the complete axisprofile
  };

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

  /// @brief This is datalayer node for the backup/restore mechanism.
  class NodeTransactionLoadSave : public NodeBase
  {
  public: // methods

    /// Constructor
    /// @param[in]   mgr         pointer to the provider manager
    /// @param[in]   operation   save/load operation which would be used
    /// @param[in]   metaDataInfo  information about the meta data
    NodeTransactionLoadSave(ProviderMgr *mgr, const PersistanceOperation operation, const MetaDataInfo& metaDataInfo);

    /// Destructor
    virtual ~NodeTransactionLoadSave() noexcept override = default;

    void onCreate(const std::string &address, const comm::datalayer::Variant *data, const ResponseCallback &callback) final;
    void onMetadata(const std::string &address, const ResponseCallback &callback) final;

  protected:                                // data
    const PersistanceOperation m_operation; ///< Operation which will be executed in onCreate()

  private: // methods

    /// @brief worker function for thread, that calls the backupRestoreMgr
    /// Pass parameters by value to ensure that the strings are valid after returning from the calling function!
    /// The resulting CppCheck warnings have to be suppressed.
    /// @param[in] backupRestoreMgr  the backup-restore manager
    /// @param[in] operation         the operation to be done
    /// @param[in] persistenceParam  The variant with the persistence parameters (path, id, phase)
    /// @param[in] callback          the callback to be called with the result of the operation
    static void doOperation(comm::axisprofile::IBackupRestoreMgr *backupRestoreMgr, PersistanceOperation operation, const comm::datalayer::Variant &persistenceParam, const ResponseCallback &callback);
  };
}
