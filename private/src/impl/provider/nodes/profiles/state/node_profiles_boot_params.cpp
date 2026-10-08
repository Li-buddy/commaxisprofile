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
#include "comm/axisprofile/datatypes/keyValueStringMap_generated.h"
#include "configuration/boot_param_backup_restore_mgr.h"
#include "node_profiles_boot_params.h"
#include "fieldbuses/fieldbus_helper.h"

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
  this->m_metaDataInfo.m_bfbsPath = "keyValueStringMap.bfbs";
  this->m_metaDataInfo.m_description = "Persisted boot parameter key-value view";
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
  
  std::vector<flatbuffers::Offset<fbtypes::keyValueEntry>> params;
  params.reserve(snapshot.paramMap.size());

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  for (const auto& [name, value] : snapshot.paramMap)
  {
    auto nameOffset = builder.CreateString(name);
    auto valueOffset = builder.CreateString(toReadableValue(value));
    params.push_back(fbtypes::CreatekeyValueEntry(builder, nameOffset, valueOffset));
  }

  auto mapOffset = builder.CreateVector(params);
  builder.Finish(fbtypes::CreatekeyValueStringMap(builder, mapOffset));
  comm::datalayer::Variant retVariant;
  retVariant.copyFlatbuffers(builder);
  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesBootParams::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}


std::string  NodeProfilesBootParams::toReadableValue(const BootParamItem& item)
{
  switch (item.type)
  {
    case BootParamValueType::UNSIGNED16:
      return std::to_string(item.value.valueUint16);
    case BootParamValueType::SIGNED16:
      return std::to_string(item.value.valueInt16);
    case BootParamValueType::UNSIGNED32:
      return std::to_string(item.value.valueUint32);
    case BootParamValueType::SIGNED32:
      return std::to_string(item.value.valueInt32);
    case BootParamValueType::BYTEARRAY:
      return FieldbusHelper::rawDataToHexDump(item.value.valueByteArray.data(), item.value.valueByteArray.size());
    default:
      return "";
  }
}
