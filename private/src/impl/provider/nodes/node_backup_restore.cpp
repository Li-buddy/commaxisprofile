/// @file
///
/// @brief This file is part of the axisprofile library and implements the
//! node class to save and load data of this component. The class uses the
//! configuration/backup_restore_mgr.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "component/i_axisprofile_component.h"
#include "configuration/i_backup_restore_mgr.h"
#include "node_backup_restore.h"
#include "provider/provider_helper.h"
#include "provider/provider_mgr.h"
#include <thread>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::provider;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

NodeBackupRestore::NodeBackupRestore(ProviderMgr *mgr, const SaveLoadOperation operation, const MetaDataInfo& metaDataInfo) : NodeBase(mgr), m_operation(operation)
{
  this->m_metaDataInfo.m_description = metaDataInfo.m_description;
  this->m_metaDataInfo.m_descriptionUrl = metaDataInfo.m_descriptionUrl;
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Program;
  this->m_metaDataInfo.nodeActions = node_actions::CREATE;
}

void NodeBackupRestore::onCreate(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback)
{
  if (!m_manager || !m_manager->getAxisProfileCmp() || !m_manager->getAxisProfileCmp()->getBackupRestoreManager())
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_SYSTEM_INCONSISTENT, "invalid value");
    return;
  }

  IBackupRestoreMgr* m_backupRestoreMgr = m_manager->getAxisProfileCmp()->getBackupRestoreManager();

  // Extract config path and profile name
  std::string dlConfigURI{};
  std::string profileName{};
  if ((m_operation == SaveLoadOperation::SAVE_SINGLE_PROFILE) || (m_operation == SaveLoadOperation::LOAD_SINGLE_PROFILE))
  {
    dlConfigURI = address.substr(0, address.rfind("/"));
    ProviderHelper::getProfileNameFromAddress(address, profileName);
  }

  // Extract file/folder path
  std::string workPath{};
  extractPathFromVariant(data, workPath);

  // Call backup/restore manager from a detached thread, because recursive operations are not possible in the data layer
  std::thread worker( doOperation, m_backupRestoreMgr, m_operation, profileName, dlConfigURI, workPath, callback );
#ifndef WIN32
  if ((m_operation == SaveLoadOperation::SAVE_SINGLE_PROFILE) || (m_operation == SaveLoadOperation::SAVE_ALL))
  {
    pthread_setname_np(worker.native_handle(), "axsprof.save");
  }
  else
  {
    pthread_setname_np(worker.native_handle(), "axsprof.load");
  }
#endif

  // Detach and exit now! Worker will call the callback, when it's done
  worker.detach( );
  return;
}

void NodeBackupRestore::onMetadata(const std::string &/*address*/, const ResponseCallback &callback)
{
  ProviderHelper::createMetaData(comm::datalayer::TYPE_DL_STRING, this->m_metaDataInfo, callback);
}

// Pass parameters by value to ensure that the strings are valid after returning from the calling function!
// The resulting CppCheck warnings have to be suppressed.
void NodeBackupRestore::doOperation(comm::axisprofile::IBackupRestoreMgr* backupRestoreMgr, SaveLoadOperation operation,
                                    // cppcheck-suppress passedByValue
                                    const std::string& profileName,
                                    // cppcheck-suppress passedByValue
                                    const std::string& /*dlCfgURI*/,
                                    // cppcheck-suppress passedByValue
                                    const std::string& workPath, const ResponseCallback &callback)
{
  // Call backup/restore manager
  AxisProfileResult backupRestoreResult;
  switch ( operation )
  {
  case SaveLoadOperation::SAVE_SINGLE_PROFILE:
    backupRestoreResult = backupRestoreMgr->save(profileName, workPath);
    break;
  case SaveLoadOperation::LOAD_SINGLE_PROFILE:
    backupRestoreResult = backupRestoreMgr->load(profileName, workPath);
    break;
  case SaveLoadOperation::SAVE_ALL:
    backupRestoreResult = backupRestoreMgr->saveSystemCfg(workPath);
    break;
  case SaveLoadOperation::LOAD_ALL:
    backupRestoreResult = backupRestoreMgr->loadSystemCfg(workPath);
    break;
  default:
    backupRestoreResult = AxisProfileResult::ERR_BACKUP_RESTORE;
    break;
  }

  if ( aprStatusFailed( backupRestoreResult ) )
  {
    // handle save
    if (SaveLoadOperation::SAVE_SINGLE_PROFILE == operation || SaveLoadOperation::SAVE_ALL == operation)
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_SUBMODULE_FAILURE, MAIN_DIAG_DIAGNOSTICS_F0_QUERY_LOAD, DETAILED_DIAG_BACKUPRESTORE_SAVE_NOT_IN_CONFIG, "backup or restore failed");
    }    
    else // for load case
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_SUBMODULE_FAILURE, MAIN_DIAG_DIAGNOSTICS_F0_QUERY_LOAD, DETAILED_DIAG_BACKUPRESTORE_LOAD_NOT_IN_CONFIG, "backup or restore failed");
    }
    return;
  }

  // Operation was successful
  comm::datalayer::Variant returnVal;
  if ((operation == SaveLoadOperation::SAVE_SINGLE_PROFILE) || (operation == SaveLoadOperation::SAVE_ALL))
  {
    returnVal.setValue("Saving finished.");
  }
  else
  {
    returnVal.setValue("Loading finished.");
  }

  callback( comm::datalayer::DlResult::DL_OK, &returnVal );
}

void NodeBackupRestore::extractPathFromVariant(const comm::datalayer::Variant* data, std::string& path)
{
  // Check type
  if (!data || (data->getType() != comm::datalayer::VariantType::STRING))
  {
    path = {};
    return;
  }

  // Extract data
  const char* convData = *data;
  path = convData;
}
