/// @file
///
/// @brief This file is part of the axisprofile library and implements boot parameter backup/restore.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "boot_param_backup_restore_mgr.h"

#include "common/debug_trace.h"
#include "diagnosis/dia.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "provider/provider_helper.h"

#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/persistence_helper_itf.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------
namespace
{
const char *const MSG_ABORT_LOAD_CFG = "Abort load configuration!";
const char *const MSG_ABORT_SAVE_CFG = "Abort save configuration!";
}

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
BootParamBackupRestoreMgr::BootParamBackupRestoreMgr(IAxisProfileManagerInternal *const profileMgr, const PersistenceHelperItf *persistenceHelper)
  : m_profileManager(profileMgr), m_persistenceHelper(persistenceHelper)
{
}

AxisProfileResult BootParamBackupRestoreMgr::extractFlatbufferData(const comm::datalayer::Variant* data, BootParamSnapshot& bootParam)
{
  if (!data || data->getType() != comm::datalayer::VariantType::FLATBUFFERS)
  {
    APR_TRACE_WARNING("boot parameter fbs type incorrect!");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifybootParamSnapshotBuffer(verifier))
  {
    APR_TRACE_WARNING("check flatbuffer failed, boot parameter fbs type is not the bootParamSnapshot!");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  return extractFlatbufferData(fbtypes::GetbootParamSnapshot(data->getData()), bootParam);
}

AxisProfileResult BootParamBackupRestoreMgr::extractFlatbufferData(const fbtypes::bootParamSnapshot* data, BootParamSnapshot& bootParam)
{
  if (data == nullptr)
  {
    APR_TRACE_WARNING("check flatbuffer failed, data is null!");
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  if (!flatbuffers::IsFieldPresent(data, fbtypes::bootParamSnapshot::VT_PROTOCOL) ||
      !flatbuffers::IsFieldPresent(data, fbtypes::bootParamSnapshot::VT_CHECKSUMVALUE) ||
      !flatbuffers::IsFieldPresent(data, fbtypes::bootParamSnapshot::VT_PARAMLIST))
  {
    APR_TRACE_WARNING("the fbs field is incomplete!");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  bootParam.schemaVersion = data->schemaVersion();
  bootParam.protocol = toInternalBootSnapshotProtocol(data->protocol());
  bootParam.checksumValue = data->checksumValue();
  bootParam.profileName = data->profileName() ? data->profileName()->str() : "";

  if (bootParam.protocol != BootSnapshotProtocol::SERCOS &&
      bootParam.protocol != BootSnapshotProtocol::CANOPEN)
  {
    APR_TRACE_WARNING("the fbs field VT_PROTOCOL is incorrect:%d!", data->protocol());
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  return extractParamList(data->paramList(), bootParam.paramMap);
}

flatbuffers::Offset<fbtypes::bootParamSnapshot> BootParamBackupRestoreMgr::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, const BootParamSnapshot& bootParam)
{
  flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<fbtypes::bootParam>>> paramListOffset {};
  paramListOffset = generateParamList(fbb, bootParam.paramMap);

  auto axisprofileNameOffset = fbb.CreateString(bootParam.profileName);
  return fbtypes::CreatebootParamSnapshot(fbb,
                                          bootParam.schemaVersion,
                                          axisprofileNameOffset,
                                          toFlatbufferBootSnapshotProtocol(bootParam.protocol),
                                          bootParam.checksumValue,
                                          paramListOffset);
}

AxisProfileResult BootParamBackupRestoreMgr::extractParamList(const flatbuffers::Vector<flatbuffers::Offset<fbtypes::bootParam>>* data, BootParamMap& parameterMap)
{
  if (data == nullptr)
  {
    APR_TRACE_WARNING("check flatbuffer failed, boot parameter list data is null!");
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  parameterMap.clear();
  for (const auto* param : *data)
  {
    if (!param || !param->name() || !param->value())
    {
      APR_TRACE_WARNING("check flatbuffer failed, bootParam data or its file is null!");
      continue;
    }

    BootParamItem item {};
    item.type = toInternalParamValueType(param->type());
    item.value.valueUint16 = param->value()->valueUint16().value_or(0);
    item.value.valueInt16 = param->value()->valueInt16().value_or(0);
    item.value.valueInt32 = param->value()->valueInt32().value_or(0);
    item.value.valueUint32 = param->value()->valueUint32().value_or(0);
    if (const auto* bytes = param->value()->valueByteArray(); bytes != nullptr)
    {
      item.value.valueByteArray.assign(bytes->begin(), bytes->end());
    }

    parameterMap[param->name()->str()] = item;
  }

  return AxisProfileResult::STS_OK;
}

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<fbtypes::bootParam>>> BootParamBackupRestoreMgr::generateParamList(flatbuffers::FlatBufferBuilder& fbb,
                                                                                                                                const BootParamMap& parameterMap)
{
  std::vector<flatbuffers::Offset<fbtypes::bootParam>> paramList {};
  paramList.reserve(parameterMap.size());

  for (const auto& [name, item] : parameterMap)
  {
    flatbuffers::Offset<flatbuffers::Vector<std::int8_t>> byteArrayOffset {};
    if (item.type == BootParamValueType::BYTEARRAY)
    {
      std::vector<std::int8_t> byteArraySigned(item.value.valueByteArray.begin(), item.value.valueByteArray.end());
      byteArrayOffset = fbb.CreateVector(byteArraySigned);
    }

    fbtypes::paramValueBuilder valueBuilder(fbb);
    switch (item.type)
    {
      case BootParamValueType::UNSIGNED16:
        valueBuilder.add_valueUint16(item.value.valueUint16);
        break;
      case BootParamValueType::SIGNED16:
        valueBuilder.add_valueInt16(item.value.valueInt16);
        break;
      case BootParamValueType::SIGNED32:
        valueBuilder.add_valueInt32(item.value.valueInt32);
        break;
      case BootParamValueType::UNSIGNED32:
        valueBuilder.add_valueUint32(item.value.valueUint32);
        break;
      case BootParamValueType::BYTEARRAY:
        valueBuilder.add_valueByteArray(byteArrayOffset);
        break;
      default:
        break;
    }

    auto valueOffset = valueBuilder.Finish();
    auto nameOffset = fbb.CreateString(name);

    paramList.push_back(fbtypes::CreatebootParam(fbb,
                                                 nameOffset,
                                                 toFlatbufferParamValueType(item.type),
                                                 valueOffset));
  }

  return fbb.CreateVector(paramList);
}

BootSnapshotProtocol BootParamBackupRestoreMgr::toInternalBootSnapshotProtocol(fbtypes::bootSnapshotProtocol protocol)
{
  switch (protocol)
  {
    case fbtypes::bootSnapshotProtocol_SERCOS:
    {
      return BootSnapshotProtocol::SERCOS;
    }
    case fbtypes::bootSnapshotProtocol_CANOPEN:
    {
      return BootSnapshotProtocol::CANOPEN;
    }
    default:
    {
      return BootSnapshotProtocol::UNKNOWN;
    }
  }
}

fbtypes::bootSnapshotProtocol BootParamBackupRestoreMgr::toFlatbufferBootSnapshotProtocol(BootSnapshotProtocol protocol)
{
  switch (protocol)
  {
    case BootSnapshotProtocol::SERCOS:
    {
      return fbtypes::bootSnapshotProtocol_SERCOS;
    }
    case BootSnapshotProtocol::CANOPEN:
    {
      return fbtypes::bootSnapshotProtocol_CANOPEN;
    }
    default:
    {
      return fbtypes::bootSnapshotProtocol_UNKNOWN;
    }
  }
}

BootParamValueType BootParamBackupRestoreMgr::toInternalParamValueType(fbtypes::paramValueType type)
{
  switch (type)
  {
    case fbtypes::paramValueType_Unsigned16:
      return BootParamValueType::UNSIGNED16;
    case fbtypes::paramValueType_Signed16:
      return BootParamValueType::SIGNED16;
    case fbtypes::paramValueType_Unsigned32:
      return BootParamValueType::UNSIGNED32;
    case fbtypes::paramValueType_Signed32:
      return BootParamValueType::SIGNED32;
    case fbtypes::paramValueType_ByteArray:
      return BootParamValueType::BYTEARRAY;
    default:
      return BootParamValueType::NONE_TYPE;
  }
}

fbtypes::paramValueType BootParamBackupRestoreMgr::toFlatbufferParamValueType(BootParamValueType type)
{
  switch (type)
  {
    case BootParamValueType::UNSIGNED16:
      return fbtypes::paramValueType_Unsigned16;
    case BootParamValueType::SIGNED16:
      return fbtypes::paramValueType_Signed16;
    case BootParamValueType::UNSIGNED32:
      return fbtypes::paramValueType_Unsigned32;
    case BootParamValueType::SIGNED32:
      return fbtypes::paramValueType_Signed32;
    case BootParamValueType::BYTEARRAY:
      return fbtypes::paramValueType_ByteArray;
    default:
      return fbtypes::paramValueType_NoneType;
  }
}

AxisProfileResult BootParamBackupRestoreMgr::getBootParamSnapshotInterface(const std::string& profileName, const std::string& filePath,
                                                                            const char *abortMsg, IDeviceBootParamSnapshot *&bootParamIF) const
{
  if (!m_persistenceHelper || !m_profileManager)
  {
    APR_TRACE_ERROR("Null-Pointer detected! %s", abortMsg);
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  if (profileName.empty() || filePath.empty())
  {
    APR_TRACE_ERROR("Invalid argument! %s", abortMsg);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  IAxisProfile* profile = m_profileManager->getProfileByName(profileName.c_str());
  auto* profileInternal = static_cast<IAxisProfileInternal*>(profile);
  if (nullptr == profileInternal)
  {
    APR_TRACE_ERROR("Could not find requested profile with name %s! %s", profileName.c_str(), abortMsg);
    return AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  }

  bootParamIF = profileInternal->getDeviceBootParmSnapshotInterface();
  if (!bootParamIF)
  {
    APR_TRACE_ERROR("Boot parameter snapshot interface not found for profile %s! %s", profileName.c_str(), abortMsg);
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BootParamBackupRestoreMgr::loadBootParams(const std::string& profileName, const std::string& filePath) const
{
  IDeviceBootParamSnapshot* bootParamIF = nullptr;
  AxisProfileResult profileIfResult = getBootParamSnapshotInterface(profileName, filePath, MSG_ABORT_LOAD_CFG, bootParamIF);
  if (aprStatusFailed(profileIfResult))
  {
    return profileIfResult;
  }

  APR_TRACE_MESSAGE("Load a single profile boot params: Name: %s  FilePath %s", profileName.c_str(), filePath.c_str());

  comm::datalayer::Variant data;
  comm::datalayer::Variant type;
  comm::datalayer::DlResult dlResult = static_cast<comm::datalayer::DlResult::Value>(m_persistenceHelper->loadDataType(m_persistenceHelper->handle, filePath, data, type));

  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    PersistenceDetailDiag detailDiag;
    m_persistenceHelper->getLastError(m_persistenceHelper->handle, detailDiag);
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_WRN, DETAILED_DIAG_BACKUPRESTORE_LOAD_FAILED, __FILE__, __FUNCTION__, __LINE__,
                       "Could not read boot params file from path: %s, DlError: %s, Detail: %s", filePath.c_str(), dlResult.toString(), detailDiag.detailDiag.c_str());
    return AxisProfileResult::ERR_DATALAYER_PERSISTENCE;
  }

  BootParamSnapshot bootParam {};
  AxisProfileResult extractResult = extractFlatbufferData(&data, bootParam);
  if (aprStatusFailed(extractResult))
  {
    APR_TRACE_ERROR("Could not extract boot params from file %s, profile %s!", filePath.c_str(), profileName.c_str());
    return extractResult;
  }

  bootParamIF->setBootParamSnapshot(bootParam);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult BootParamBackupRestoreMgr::saveBootParam(const std::string& profileName, const std::string& filePath) const
{
  IDeviceBootParamSnapshot* bootParamIF = nullptr;
  AxisProfileResult profileIfResult = getBootParamSnapshotInterface(profileName, filePath, MSG_ABORT_SAVE_CFG, bootParamIF);
  if (aprStatusFailed(profileIfResult))
  {
    return profileIfResult;
  }

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  builder.Finish(generateFlatbufferData(builder, bootParamIF->getBootParamSnapshot()));

  comm::datalayer::Variant bootParamData;
  bootParamData.copyFlatbuffers(builder);

  const std::string bootParamTypeAddress = provider::ProviderHelper::getTypeString(provider::NodeTree.APR_PROFILES_CFG_BOOTPARAMS);
  comm::datalayer::DlResult dlResult = static_cast<comm::datalayer::DlResult::Value>(
      m_persistenceHelper->saveDataTypeaddress(m_persistenceHelper->handle, bootParamData, bootParamTypeAddress, filePath));
  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    return AxisProfileResult::ERR_DATALAYER_PERSISTENCE;
  }

  return AxisProfileResult::STS_OK;
}
