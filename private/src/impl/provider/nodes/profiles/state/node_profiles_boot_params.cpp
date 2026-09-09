/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// the node for persisted boot parameter snapshots.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "configuration/boot_param_backup_restore_mgr.h"
#include "node_profiles_boot_params.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::provider;

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

NodeProfilesBootParams::NodeProfilesBootParams(ProviderMgr* mgr) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "deviceParamSnapshot.bfbs";
  this->m_metaDataInfo.m_description = "Persisted boot parameter snapshot";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ;
}

void NodeProfilesBootParams::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  IDeviceBootParamSnapshot* bootParamIF = profile->getDeviceBootParmSnapshotInterface();
  if (!bootParamIF)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE,
                                        "boot parameter snapshot interface not found");
    return;
  }

  comm::axisprofile::BootParamSnapshot snapshot = bootParamIF->getBootParamSnapshot();
  /*APR_TRACE_MESSAGE("== Read boot parameter snapshot. [name:%s,ptotocol:%d,checksum:%d, mapSize:%zu]",
                    snapshot.m_profileName.c_str(), snapshot.m_protocol, snapshot.m_checksumValue, snapshot.m_paramMap.size() );*/
  
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  builder.Finish(BootParamBackupRestoreMgr::generateFlatbufferData(builder, snapshot));

  comm::datalayer::Variant retVariant;
  retVariant.copyFlatbuffers(builder);
  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesBootParams::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  IDeviceBootParamSnapshot* bootParamIF = profile->getDeviceBootParmSnapshotInterface();
  if (!bootParamIF)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE,
                                        "boot parameter snapshot interface not found");
    APR_TRACE_WARNING("Boot parameter snapshot interface not found for profile %s, write operation cannot be performed!", profile->getProfileName());
    return;
  }

  BootParamSnapshot bootParam {};
  if (aprStatusFailed(BootParamBackupRestoreMgr::extractFlatbufferData(data, bootParam)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER,
                                        "boot parameter fbs can't be extracted for profile %s", profile->getProfileName());
    APR_TRACE_WARNING("Failed to extract boot parameter snapshot data from cfg file for profile %s, write operation cannot be performed!", profile->getProfileName());
    return;
  }

  bootParamIF->setBootParamSnapshot(bootParam);
  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeProfilesBootParams::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}
