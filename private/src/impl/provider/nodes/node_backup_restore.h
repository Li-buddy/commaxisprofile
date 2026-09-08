#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the
/// node class to save and load data of this component. The class uses the
/// configuration/backup_restore_mgr.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_base.h"

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

  /// Operation which should be executed when calling onCreate() of this class
  enum class SaveLoadOperation
  {
    SAVE_SINGLE_PROFILE = 0,   ///< Save a single profile
    LOAD_SINGLE_PROFILE,       ///< Load a single profile
    SAVE_ALL,                  ///< Save all profiles
    LOAD_ALL                   ///< Load all profiles
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
  class NodeBackupRestore : public NodeBase
  {
  public: // methods

    /// Constructor
    /// @param[in]   mgr         pointer to the provider manager
    /// @param[in]   operation   save/load operation which would be used
    /// @param[in]   metaDataInfo  information about the meta data
    NodeBackupRestore(ProviderMgr* mgr, const SaveLoadOperation operation, const MetaDataInfo& metaDataInfo);

    /// Destructor
    virtual ~NodeBackupRestore() noexcept override = default;

    void onCreate(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

  protected: // data

    const SaveLoadOperation m_operation;   ///< Operation which will be executed in onCreate()

  private: // methods

    /// @brief Worker function for thread, that calls the backupRestoreMgr.
    /// @param[in] backupRestoreMgr  The backup/restore manager.
    /// @param[in] operation         The operation to be done.
    /// @param[in] profileName       Optional (only for save/load of a single profile). The name of the profile.
    /// @param[in] dlCfgURI          Optional (only for save/load of a single profile). The datalayer config URI.
    /// @param[in] workPath          Optional path to the file or folder for the operation.
    /// @param[in] callback          The callback to be called with the result of the operation.
    static void doOperation(comm::axisprofile::IBackupRestoreMgr* backupRestoreMgr, SaveLoadOperation operation, const std::string& profileName, const std::string& dlCfgURI, const std::string& workPath, const ResponseCallback& callback);

    /// @brief This method extracts the file/folder path from the given variant.
    /// @param[in]    data   Variant that contains the string data
    /// @param[out]   path   The extracted file/folder path
    void extractPathFromVariant(const comm::datalayer::Variant* data, std::string& path);
  };
}
