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
#include "component/i_axisprofile_component.h"
#include "configuration/i_backup_restore_mgr.h"
#include "node_transaction_based_loadsave.h"
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

NodeTransactionLoadSave::NodeTransactionLoadSave(ProviderMgr *mgr, const PersistanceOperation operation, const MetaDataInfo& metaDataInfo) : NodeBase(mgr), m_operation(operation)
{
  this->m_metaDataInfo.m_description = metaDataInfo.m_description;
  this->m_metaDataInfo.m_descriptionUrl = metaDataInfo.m_descriptionUrl;
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Program;
  this->m_metaDataInfo.nodeActions = node_actions::CREATE;
}

void NodeTransactionLoadSave::onCreate(const std::string &/* address */, const comm::datalayer::Variant *data, const ResponseCallback &callback)
{
  if (!m_manager || !m_manager->getAxisProfileCmp() || !m_manager->getAxisProfileCmp()->getBackupRestoreManager())
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_SYSTEM_INCONSISTENT, "invalid value");
    return;
  }
  IBackupRestoreMgr *backupRestoreMgr = m_manager->getAxisProfileCmp()->getBackupRestoreManager();

  // Verify input flatbuffer
  if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) || STATUS_FAILED(data->verifyFlatbuffers(comm::datalayer::VerifyPersistenceParamBuffer)))
  {
    return;
  }
  // Call backup/restore manager from a detached thread, because recursive operations are not possible in the data layer
  std::thread worker(doOperation, backupRestoreMgr, m_operation, *data, callback);
#ifndef WIN32
  if ((m_operation == PersistanceOperation::SAVE_AXISPROFILE))
  {
    pthread_setname_np(worker.native_handle(), "axsprof.save");
  }
  else
  {
    pthread_setname_np(worker.native_handle(), "axsprof.load");
  }
#endif

  // Detach and exit now! Worker will call the callback, when it's done
  worker.detach();
  return;
}

// Pass parameters by value to ensure that the strings are valid after returning from the calling function!
// The resulting CppCheck warnings have to be suppressed.
void NodeTransactionLoadSave::doOperation(comm::axisprofile::IBackupRestoreMgr *backupRestoreMgr, PersistanceOperation operation, const comm::datalayer::Variant &persistenceParam, const ResponseCallback &callback)
{
  // Call backup/restore manager
  AxisProfileResult backupRestoreResult;
  switch (operation)
  {
  case PersistanceOperation::SAVE_AXISPROFILE:
    backupRestoreResult = backupRestoreMgr->saveSystemCfg(persistenceParam);
    break;
  case PersistanceOperation::LOAD_AXISPROFILE:
    backupRestoreResult = backupRestoreMgr->loadSystemCfg(persistenceParam);
    break;
  default:
    backupRestoreResult = AxisProfileResult::ERR_BACKUP_RESTORE;
    break;
  }

  if (aprStatusFailed(backupRestoreResult))
  {
    // handle save
    if (PersistanceOperation::SAVE_AXISPROFILE == operation)
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_SUBMODULE_FAILURE, MAIN_DIAG_DIAGNOSTICS_F0_QUERY_LOAD, DETAILED_DIAG_BACKUPRESTORE_SAVE_NOT_IN_CONFIG, "");
    }
    else // for load case
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_SUBMODULE_FAILURE, MAIN_DIAG_DIAGNOSTICS_F0_QUERY_LOAD, DETAILED_DIAG_BACKUPRESTORE_LOAD_NOT_IN_CONFIG, "");
    }
    return;
  }

  // Operation was successful
  comm::datalayer::Variant returnVal;
  if ((operation == PersistanceOperation::SAVE_AXISPROFILE))
  {
    returnVal.setValue("Saving finished.");
  }
  else
  {
    returnVal.setValue("Loading finished.");
  }

  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeTransactionLoadSave::onMetadata(const std::string &/* address */, const ResponseCallback &callback)
{
  const std::string typeString = "types/datalayer/persistence_param";
  ProviderHelper::createMetaData(typeString, this->m_metaDataInfo, callback);
}
