/// @file
///
/// @brief This file is part of the axisprofile library and implements the node configuration make persistent 
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "component/i_axisprofile_component.h"
#include "configuration/i_backup_restore_mgr.h"
#include "node_cfg_make_persistent.h"
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
NodeCfgMakePersistent::NodeCfgMakePersistent(ProviderMgr* mgr): NodeBase(mgr)
{
  this->m_metaDataInfo.m_description = "Make changes of a single axisprofile object persistent";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Program;
  this->m_metaDataInfo.nodeActions = node_actions::CREATE;
}

void NodeCfgMakePersistent::onCreate(const std::string& address , const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  if (!m_manager || !m_manager->getAxisProfileCmp() || !m_manager->getAxisProfileCmp()->getBackupRestoreManager())
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_SYSTEM_INCONSISTENT, "invalid value");
    return;
  }
  IBackupRestoreMgr* backupRestoreMgr = m_manager->getAxisProfileCmp()->getBackupRestoreManager();

  // Verify input flatbuffer
  if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) || STATUS_FAILED(data->verifyFlatbuffers(comm::datalayer::VerifyPersistenceParamBuffer)))
  {
    return;
  }

  // Extract config path, axisprofile name
  std::string axisprofileName = address.substr(address.rfind("/") + 1);

  // Call backup/restore manager from a detached thread, because recursive operations are not possible in the data layer
  std::thread worker(doOperation, backupRestoreMgr, *data, axisprofileName, callback);
#ifndef WIN32
  pthread_setname_np(worker.native_handle(), "axsprof.save");
#endif

  // Detach and exit now! Worker will call the callback, when it's done
  worker.detach();
  return;
}

// Pass parameters by value to ensure that the strings are valid after returning from the calling function!
// The resulting CppCheck warnings have to be suppressed.
void NodeCfgMakePersistent::doOperation(comm::axisprofile::IBackupRestoreMgr* backupRestoreMgr, const comm::datalayer::Variant& persistenceParam, const std::string& axisprofileName,
                                        const ResponseCallback& callback)
{
  // Call backup/restore manager
  AxisProfileResult backupRestoreResult = backupRestoreMgr->makePersistent(persistenceParam, axisprofileName);

  if (aprStatusFailed(backupRestoreResult))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_SUBMODULE_FAILURE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_BACKUP_RESTORE, "backup or restore failed");
    return;
  }

  comm::datalayer::Variant returnVal;
  returnVal.setValue("Saving finished.");
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeCfgMakePersistent::onMetadata(const std::string&/* address */, const ResponseCallback& callback)
{
  const std::string typeString = "types/datalayer/persistence_param";
  ProviderHelper::createMetaData(typeString, this->m_metaDataInfo, callback);
}
