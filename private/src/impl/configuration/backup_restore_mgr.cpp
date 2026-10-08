// ---------------------------------------------------------------------------
//
//! @file       back_restore_mgr.cpp
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This source file is part of the axisprofile library and implements the
//! backup restore manager. This class manages the permanent saving/loading
//! of axis profile configuration data to/from a JSON file.
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/cfg/node_profiles_config.h"
#include "backup_restore_mgr.h"
#include "common/debug_trace.h"
#include "provider/provider_helper.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "diagnosis/dia.h"
#include <filesystem>

// External Includes
#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/persistence_helper_itf.h"
#include "comm/datalayer/persistence_param_generated.h"
#include "comm/axisprofile/datatypes/profileConfig_generated.h"
#include "comm/axisprofile/datatypes/valueMapping_generated.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

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
BackupRestoreMgr::BackupRestoreMgr(IAxisProfileManagerInternal *const profileMgr, comm::datalayer::IClient *const dlClient, const PersistenceHelperItf *persistenceHelper)
    : m_profileManager(profileMgr), m_dlClient(dlClient), m_persistenceHelper(persistenceHelper)
    , m_typeAddress(provider::ProviderHelper::getTypeString(provider::NodeTree.APR_PROFILES_CFG))
{
}

BackupRestoreMgr::~BackupRestoreMgr()
{
  delete m_dlClient;
}

AxisProfileResult BackupRestoreMgr::validateParam(const std::string& profileName) const
{
  if (!m_persistenceHelper || !m_dlClient || !m_profileManager)
  {
    APR_TRACE_ERROR("Null-Pointer detected! %s", MSG_ABORT_LOAD_CFG.c_str());
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  if (profileName.empty())
  {
    APR_TRACE_ERROR("Invalid argument! %s", MSG_ABORT_LOAD_CFG.c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BackupRestoreMgr::load(const std::string& profileName, const std::string& filePath)
{
  std::string fileNamePath = filePath.empty() ? m_bundleSubfolder + profileName + ".json" : filePath;
  AxisProfileResult loadCfgResult = loadCfg(profileName, fileNamePath);
  if (aprStatusFailed(loadCfgResult))
  {
    APR_TRACE_ERROR("Could not load data of profile %s, path:%s", profileName.c_str(), fileNamePath.c_str());
    return loadCfgResult;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult BackupRestoreMgr::loadCfg(const std::string &profileName, const std::string &filePath)
{
  AxisProfileResult ret = validateParam(profileName);
  if(aprStatusFailed(ret))
  {
    return ret;
  }
  std::string dlCfgUri = replaceWildCardWithProfileName(provider::NodeTree.APR_PROFILES_CFG, profileName); 

  std::string workPath = filePath.empty() ? m_bundleSubfolder + profileName + ".json" : filePath;
  APR_TRACE_MESSAGE("Load single profile config: Name:%s, filePath:%s, loadPath:%s, URI:%s", profileName.c_str(), filePath.c_str(), workPath.c_str(), dlCfgUri.c_str());

  comm::datalayer::Variant data{};
  AxisProfileResult readResult = readCfgDataFromFile(workPath, data);
  if (aprStatusFailed(readResult))
  {
    return readResult;
  } 

  // If the profile does not exist we have to create it
  const IAxisProfile *profile = m_profileManager->getProfileByName(profileName.c_str());
  if (!profile)
  {
    APR_TRACE_MESSAGE("Create profile with name %s", profileName.c_str());
    comm::datalayer::IClient::ResponseCallback callback = [](comm::datalayer::DlResult /*status*/, const comm::datalayer::Variant * /*source*/) {};
    provider::NodeProfilesConfig::profileConfig profileConfig{};
    AxisProfileResult result = provider::NodeProfilesConfig::extractFlatbufferData(&data, profileConfig, callback);
    if (aprStatusFailed(result))
    {
      APR_TRACE_ERROR("Could not extract the received flatbuffer of the persistence helper! %s", MSG_ABORT_LOAD_CFG.c_str());
      return result;
    }

    result = m_profileManager->createAxisProfile(profileConfig.m_profile.m_profileType, profileName);
    if (aprStatusFailed(result))
    {
      APR_TRACE_ERROR("Could not create profile: name=%s type=%s addr=%d", profileName.c_str(), axisProfileTypeToAbbreviationString(profileConfig.m_profile.m_profileType), profileConfig.m_deviceAddress);
      return result;
    }
  }

  comm::datalayer::DlResult dlResult = m_dlClient->writeSync(dlCfgUri, &data);
  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    m_profileManager->deleteAxisProfile(profileName.c_str());
    APR_TRACE_ERROR("Could not write profile configuration! %s", MSG_ABORT_LOAD_CFG.c_str());
    return AxisProfileResult::ERR_DATALAYER_WRITE_NODE;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BackupRestoreMgr::saveCfg(const std::string &profileName, const std::string &filePath)
{
  AxisProfileResult ret = validateParam(profileName);
  if (aprStatusFailed(ret))
  {
    return ret;
  }

  std::string dlCfgUri = replaceWildCardWithProfileName(provider::NodeTree.APR_PROFILES_CFG, profileName);

  APR_TRACE_MESSAGE("Save single profile config: Name:%s, savePath:%s, URI:%s", profileName.c_str(), filePath.c_str(), dlCfgUri.c_str());
  const IAxisProfile *profile = m_profileManager->getProfileByName(profileName.c_str());
  if (!profile)
  {
    APR_TRACE_ERROR("Could not find requested profile with name %s! %s", profileName.c_str(), MSG_ABORT_SAVE_CFG.c_str());
    return AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  }

  // Save data to file
  // comm::datalayer::Variant data;
  comm::datalayer::DlResult dlResult = static_cast<comm::datalayer::DlResult::Value>(m_persistenceHelper->saveDataaddress(m_persistenceHelper->handle, dlCfgUri, filePath));
  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BACKUPRESTORE_SAVE_FAILED, __FILE__, __FUNCTION__, __LINE__,
                       "Could not save profile %s to path: %s, DlError: %s", profileName.c_str(), filePath.c_str(), dlResult.toString());
    return AxisProfileResult::ERR_DATALAYER_PERSISTENCE;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BackupRestoreMgr::loadSystemCfg(const std::string &folderPath)
{
  APR_TRACE_MESSAGE("Start to load the system configuration.");

  if (!m_persistenceHelper)
  {
    APR_TRACE_ERROR("Null-Pointer detected! %s", MSG_ABORT_LOAD_SYSTEM_CFG.c_str());
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  AxisProfileResult result = executeCfgLoad(folderPath, "");
  APR_TRACE_MESSAGE("Loading the system configuration finished!");
  return result;
}

AxisProfileResult BackupRestoreMgr::loadSystemCfg(const comm::datalayer::Variant &persistenceParam)
{
  if (!m_persistenceHelper)
  {
    APR_TRACE_ERROR("Null-Pointer detected! %s", MSG_ABORT_LOAD_SYSTEM_CFG.c_str());
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  // Verify input flatbuffer
  if ((persistenceParam.getType() != comm::datalayer::VariantType::FLATBUFFERS) || STATUS_FAILED(persistenceParam.verifyFlatbuffers(comm::datalayer::VerifyPersistenceParamBuffer)))
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, __FILE__, __FUNCTION__, __LINE__, MSG_ABORT_LOAD_SYSTEM_CFG.c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  // correct config flatbuffer provided - extract data
  auto inFb = comm::datalayer::GetPersistenceParam(persistenceParam.getData());

  // Extract file/folder path
  std::string configPath{};
  std::string id{};
  std::string phase{};
  AxisProfileResult returnValue = extractPersistenceData(inFb, configPath, id, phase);
  if (aprStatusFailed(returnValue))
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BACKUPRESTORE_EXTRACT_PERSISTENCE_FAILED, __FILE__, __FUNCTION__, __LINE__, MSG_ABORT_LOAD_SYSTEM_CFG.c_str());
    return returnValue;
  }
  LoadPhases loadPhase = stringToLoadPhases(phase.c_str());

  std::string absolutePath;
  m_persistenceHelper->getAbsolutePath(m_persistenceHelper->handle, persistenceParam, m_bundleSubfolder, absolutePath);

  switch (loadPhase)
  {
  case LoadPhases::QUERY:
    returnValue = queryCfgLoad(absolutePath, id);
    break;
  case LoadPhases::PREPARE:
    // prepare the axisprofile for loading routine
    // nothing to do here yet
    break;
  case LoadPhases::LOAD:
    returnValue = executeCfgLoad(absolutePath, id);
    break;
  case LoadPhases::VALIDATE:
    // validate if the configuration & system is valid
    break;
  case LoadPhases::ACTIVATE:
    // maybe switch to running?
    break;
  case LoadPhases::ABORT:
    // do we need to clean up?
    break;
  default:
    APR_TRACE_MESSAGE("Wrong phase for loading axisprofile configuration. Provided %s [%s]", phase.c_str(), id.c_str());
    return AxisProfileResult::STS_OK;
  }
  return returnValue;
}

AxisProfileResult BackupRestoreMgr::queryCfgLoad(const std::string &folderPath, const std::string &loadId)
{
  // first check if in config => everything allowed
  if (StateMachineState::CONFIG == *(m_profileManager->getStateMachineStatusIF()))
  {
    return AxisProfileResult::STS_OK;
  }
  // check if there are new axisprofiles or axisprofiles that should be deleted
  // Get all configuration files (*.json)
  std::vector<std::string> foundFiles{};
  AxisProfileResult returnValue = getAvailableFiles(folderPath, loadId, foundFiles);
  if(AxisProfileResult::ERR_DATALAYER_PERSISTENCE == returnValue)
  {
    return returnValue;
  }
  std::sort(foundFiles.begin(), foundFiles.end());

  std::vector<std::string> newProfiles{};
  for (auto const &file : foundFiles)
  {
    std::string profileName = getFileNameFromPath(file);
    if (profileName.empty())
    {
      APR_TRACE_WARNING("Skip file with empty profile name: %s", file.c_str());
      continue;
    }
    newProfiles.emplace_back(profileName);
  }
  if (newOrDeletedObjectsAvailable(newProfiles))
  {
    Dia::sendDiagnosis(MAIN_DIAG_DIAGNOSTICS_F0_QUERY_LOAD, DETAILED_DIAG_BACKUPRESTORE_LOAD_NOT_IN_CONFIG, __FILE__, __FUNCTION__, __LINE__, "Requires object creation/deletion during query config load process.");
    APR_TRACE_ERROR("object creation/deletion needs config state...");
    return AxisProfileResult::ERR_NOT_IN_CONFIG_STATE;
  }
  if (!checkChangedData(loadId, foundFiles))
  {
    Dia::sendDiagnosis(MAIN_DIAG_DIAGNOSTICS_F0_QUERY_LOAD, DETAILED_DIAG_BACKUPRESTORE_LOAD_NOT_IN_CONFIG, __FILE__, __FUNCTION__, __LINE__, "Requires configuration data changes during query config load process.");
    return AxisProfileResult::ERR_NOT_IN_CONFIG_STATE;
  }

  return returnValue;
}

AxisProfileResult BackupRestoreMgr::saveSystemCfg(const std::string &folderPath)
{
  APR_TRACE_MESSAGE("Start to save the system configuration.");

  if (!m_profileManager || !m_persistenceHelper)
  {
    APR_TRACE_ERROR("Null-Pointer detected! %s", MSG_ABORT_SAVE_SYSTEM_CFG.c_str());
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // Get folder path where the files should be stored
  std::string workPath = folderPath.empty() ? m_bundleSubfolder : makeFolderPath(folderPath);
  return executeCfgSave(workPath, "");
}

AxisProfileResult BackupRestoreMgr::saveSystemCfg(const comm::datalayer::Variant &persistenceParam)
{
  APR_TRACE_MESSAGE("Start to save the system configuration.");
  if (!m_persistenceHelper)
  {
    APR_TRACE_ERROR("Null-Pointer detected! %s", MSG_ABORT_SAVE_SYSTEM_CFG.c_str());
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // Verify input flatbuffer
  if ((persistenceParam.getType() != comm::datalayer::VariantType::FLATBUFFERS) || STATUS_FAILED(persistenceParam.verifyFlatbuffers(comm::datalayer::VerifyPersistenceParamBuffer)))
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, __FILE__, __FUNCTION__, __LINE__, MSG_ABORT_SAVE_SYSTEM_CFG.c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  // correct config flatbuffer provided - extract data
  auto inFb = comm::datalayer::GetPersistenceParam(persistenceParam.getData());

  // Extract file/folder path
  std::string configPath{};
  std::string id{};
  std::string phase{};
  AxisProfileResult returnValue = extractPersistenceData(inFb, configPath, id, phase);

  if (aprStatusFailed(returnValue))
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BACKUPRESTORE_EXTRACT_PERSISTENCE_FAILED, __FILE__, __FUNCTION__, __LINE__, MSG_ABORT_SAVE_SYSTEM_CFG.c_str());
    return returnValue;
  }

  std::string absolutePath;
  m_persistenceHelper->getAbsolutePath(m_persistenceHelper->handle, persistenceParam, m_bundleSubfolder, absolutePath);
  return executeCfgSave(absolutePath, id);
}

std::string BackupRestoreMgr::replaceWildCardWithProfileName(std::string const& str, std::string const& profileName)
{
  std::string uri(str);
  uri.replace(uri.find("*"), 1, profileName);
  return uri;
}

size_t BackupRestoreMgr::findPathSeparator(const std::string& filePath)
{
  size_t pos = filePath.rfind("/");
  if (pos == std::string::npos)
  {
    pos = filePath.rfind("\\");
  }
  return pos;
}

std::string BackupRestoreMgr::getFileNameFromPath(const std::string &filePath)
{
  size_t pos = findPathSeparator(filePath);
  std::string buffer = (pos != std::string::npos) ? filePath.substr(pos + 1) : filePath;

  // Remove the file extension (.json) if necessary
  pos = buffer.rfind(".");
  if (pos != std::string::npos)
  {
    buffer.erase(pos, (buffer.length() - pos));
  }

  return buffer;
}

std::string BackupRestoreMgr::makeFolderPath(const std::string &path)
{
  std::string folderPath(path);
  if (!folderPath.empty())
  {
    // In case of a folder path we need to ensure, that the last character is a slash
    folderPath = ((folderPath.back() == '/') || (folderPath.back() == '\\')) ? folderPath : folderPath + '/';
  }
  return folderPath;
}

AxisProfileResult BackupRestoreMgr::executeCfgSave(const std::string &folderPath, const std::string &/* loadId */)
{
  // Delete the configuration folder (not only the content)
  comm::datalayer::DlResult dlResult = static_cast<comm::datalayer::DlResult::Value>(m_persistenceHelper->deleteDirectory(m_persistenceHelper->handle, folderPath));
  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    APR_TRACE_ERROR("Could not clear configuration folder! DlError: %s", dlResult.toString());
    return AxisProfileResult::ERR_DATALAYER_PERSISTENCE;
  }

  // Iterate over all profiles and generate the configuration files
  std::vector<std::string> profileNames;
  m_profileManager->getProfileNames(profileNames);
  AxisProfileResult returnValue = profileNames.empty() ? AxisProfileResult::STS_NO_PROFILES_EXIST : AxisProfileResult::STS_OK;
  for (const auto &name : profileNames)
  {
    std::string configFilePath = folderPath + name + ".json";    
    AxisProfileResult saveProfileResult = saveCfg(name, configFilePath); // Save current profile data
    if (aprStatusFailed(saveProfileResult))
    {
      APR_TRACE_ERROR("Could not save data of profile %s!", name.c_str());
      returnValue = saveProfileResult;
    }
  }

  APR_TRACE_MESSAGE("Saving the system configuration finished!");
  return returnValue;
}

AxisProfileResult BackupRestoreMgr::executeCfgLoad(const std::string &folderPath, const std::string &loadId)
{
  // Get folder path from where the files should be loaded
  std::string workPath = folderPath.empty() ? m_bundleSubfolder : makeFolderPath(folderPath);
  APR_TRACE_MESSAGE("Path of configuration files: %s, workPath:%s", folderPath.c_str(), workPath.c_str());

  // Get all configuration files (*.json)
  std::vector<std::string> foundFiles;
  AxisProfileResult returnValue = getAvailableFiles(workPath, loadId, foundFiles);
  if(AxisProfileResult::ERR_DATALAYER_PERSISTENCE == returnValue)
  {
    return returnValue;
  }

  // Store current profiles from system before loading the configuration
  std::vector<std::string> profilesToBeDeleted;
  m_profileManager->getProfileNames(profilesToBeDeleted);

  for (const auto &pNames : profilesToBeDeleted)
  {
    AxisProfileResult deleteResult = m_profileManager->deleteAxisProfile(pNames.c_str());
    if (aprStatusFailed(deleteResult))
    {
      APR_TRACE_ERROR("Could not delete profile %s,error: 0x%x!", pNames.c_str(), deleteResult);
      returnValue = deleteResult;
    }
  }

  for (const auto &filePath : foundFiles)
  {
    std::string profileName = getFileNameFromPath(filePath);
    APR_TRACE_MESSAGE("Load file: %s, profileName:%s", filePath.c_str(), profileName.c_str());

    if (profileName.empty())
    {
      APR_TRACE_WARNING("Load file, but the profileName is empty. filePath: %s", filePath.c_str());
      continue;
    }

    AxisProfileResult loadRes = load(profileName, filePath);
    if (aprStatusFailed(loadRes))
    {
      APR_TRACE_ERROR("Could not load configuration file %s. ret:%d", filePath.c_str(), loadRes);
      returnValue = loadRes;
    }
  }
  return returnValue;
}

AxisProfileResult BackupRestoreMgr::getAvailableFiles(const std::string &folderPath, const std::string &/* loadId */, std::vector<std::string> &availableFiles)
{
  // Get all configuration files (*.json)
  comm::datalayer::DlResult dlResult = static_cast<comm::datalayer::DlResult::Value>(m_persistenceHelper->browseDirectoryRegex(m_persistenceHelper->handle, folderPath, false, "(.*).json", availableFiles));
  //bugfix 288397
  if ( (comm::datalayer::STATUS_FAILED(dlResult)) && ((dlResult != comm::datalayer::DlResult::DL_INVALID_VALUE) || folderPath.empty()))
  {
    // we got an error from file browse
    // it's no error, when we try to load from the default location (folderPath is empty) and the folder does not exists; this means 'there is no configuration'
    APR_TRACE_ERROR("Could not read configuration folder! FolderPath: %s  DlError: %s", folderPath.c_str(), dlResult.toString());
    return AxisProfileResult::ERR_DATALAYER_PERSISTENCE;
  }
  //--bugfix 288397
  APR_TRACE_MESSAGE("Number of found configuration files: %d", availableFiles.size());
  AxisProfileResult returnValue{};
  if(availableFiles.empty())
  {
    Dia::sendDiagnosis(MAIN_DIAG_GENERAL_MSG, DETAILED_DIAG_PROFILE_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__, "no profiles found in current active solution, folderPath: %s", folderPath.c_str());
    returnValue = AxisProfileResult::STS_NO_CONFIG_FILES_FOUND;
  }
  else
  {
    returnValue = AxisProfileResult::STS_OK;
  }
  return returnValue;
}

bool BackupRestoreMgr::newOrDeletedObjectsAvailable(const std::vector<std::string>& newProfilesFromFiles)
{
  // Store current profiles from system before loading the configuration (vec1)

  std::vector<std::string> actProfiles{};
  m_profileManager->getProfileNames(actProfiles);
  std::sort(actProfiles.begin(), actProfiles.end());

  // All profiles in the new configuration (vec2)
  std::vector<std::string> newProfiles = newProfilesFromFiles;
  std::sort(newProfiles.begin(), newProfiles.end());

  // The needless objects that will be deleted (vec3 = vec1 - vec2)
  std::vector<std::string> deleteObjects{};
  std::set_difference(actProfiles.begin(), actProfiles.end(), newProfiles.begin(), newProfiles.end(), std::back_inserter(deleteObjects));

  // The new objects that will be created (vec4 = vec2 - vec1)
  std::vector<std::string> createObjects{};
  std::set_difference(newProfiles.begin(), newProfiles.end(), actProfiles.begin(), actProfiles.end(), std::back_inserter(createObjects));

  if (!deleteObjects.empty() || !createObjects.empty())
  {
    return true;
  }
  return false;
}

bool BackupRestoreMgr::checkChangedData(const std::string &loadId, const std::vector<std::string>& filePaths)
{
  for (const auto& filePath : filePaths)
  {
    std::string profileName = getFileNameFromPath(filePath);
    if (profileName.empty())
    {
      APR_TRACE_ERROR("Could not extract profile name from file path: %s [%s]", filePath.c_str(), loadId.c_str());
      return false;
    }
    std::string dlUri = replaceWildCardWithProfileName(provider::NodeTree.APR_PROFILES_CFG, profileName);

    comm::datalayer::Variant data{};
    AxisProfileResult readResult = readCfgDataFromFile(filePath, data);
    if (aprStatusFailed(readResult))
    {
      APR_TRACE_ERROR("Could not read cfg data, ret:%d, load id:[%s]", readResult, loadId.c_str());
      return false;
    }
    if (!checkConfigData(dlUri, &data))
    {
      //TRACE
      APR_TRACE_ERROR("The given configuration is not valid in state 'Running'.");
      return false;
    }
  }
  return true;
}

AxisProfileResult BackupRestoreMgr::readCfgDataFromFile(const std::string& filePath, comm::datalayer::Variant& data)
{
  comm::datalayer::Variant metaData{};
  comm::datalayer::DlResult dlResult = m_dlClient->readSync(m_typeAddress, &metaData);
  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    APR_TRACE_ERROR("Could not read metadata! %s", MSG_ABORT_LOAD_CFG.c_str());
    return AxisProfileResult::ERR_DATALAYER_READ_METADATA;
  }

  dlResult = static_cast<comm::datalayer::DlResult::Value>(m_persistenceHelper->loadData(m_persistenceHelper->handle, filePath, metaData, data));
  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    PersistenceDetailDiag detailDiag;
    m_persistenceHelper->getLastError(m_persistenceHelper->handle, detailDiag);
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BACKUPRESTORE_LOAD_FAILED, __FILE__, __FUNCTION__, __LINE__,
                       "Could not read file from path: %s, DlError: %s, Detail: %s", filePath.c_str(), dlResult.toString(), detailDiag.detailDiag.c_str());
    return AxisProfileResult::ERR_DATALAYER_PERSISTENCE;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BackupRestoreMgr::extractPersistenceData(const comm::datalayer::PersistenceParam *paramFb, std::string &configPath, std::string &id, std::string &phase)
{
  // Write available fields to output variables
  if (flatbuffers::IsFieldPresent(paramFb, comm::datalayer::PersistenceParam::VT_CONFIGURATIONPATH))
  {
    configPath = flatbuffers::GetString(paramFb->configurationPath());
  }
  else
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  if (flatbuffers::IsFieldPresent(paramFb, comm::datalayer::PersistenceParam::VT_ID))
  {
    id = flatbuffers::GetString(paramFb->id());
  }
  if (flatbuffers::IsFieldPresent(paramFb, comm::datalayer::PersistenceParam::VT_PHASE))
  {
    phase = flatbuffers::GetString(paramFb->phase());
  }

  return AxisProfileResult::STS_OK;
}

bool BackupRestoreMgr::checkConfigData(const std::string& dlUri, comm::datalayer::Variant *cfgData)
{
  comm::datalayer::Variant actData{};
  comm::datalayer::DlResult dlResult = m_dlClient->readSync(dlUri, &actData);

  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    // todo: We need a better error code (we are reading)
    APR_TRACE_ERROR("Could not read meta data, DlResult: %s ", dlResult.toString());
    return false;
  }

  // verify actual data flatbuffer
  if (STATUS_FAILED(actData.verifyFlatbuffers(comm::axisprofile::fbtypes::VerifyprofileConfigBuffer)) ||
      STATUS_FAILED(cfgData->verifyFlatbuffers(comm::axisprofile::fbtypes::VerifyprofileConfigBuffer)))
  {
    // wrong config flatbuffer provided - return error
    APR_TRACE_ERROR("checkConfigData: wrong config flatbuffer provided.");
    return false;
  }

  // correct config flatbuffer provided - set data accordingly

  auto actDataFb = comm::axisprofile::fbtypes::GetprofileConfig(actData.getData());
  auto cfgDataFb = comm::axisprofile::fbtypes::GetprofileConfig(cfgData->getData());

  if (!flatbuffers::IsFieldPresent(actDataFb, comm::axisprofile::fbtypes::profileConfig::VT_DEVICEADDRESS))
  {
    // todo error diag
    APR_TRACE_ERROR("checkConfigData: Device address is not presented.");
    return false;
  }

  if (flatbuffers::IsFieldPresent(cfgDataFb, comm::axisprofile::fbtypes::profileConfig::VT_DEVICEADDRESS))
  {
    // needs to be the same
    if (actDataFb->deviceAddress() != cfgDataFb->deviceAddress())
    {
      APR_TRACE_ERROR("Device address conflict, current address: %d, loading address: %d", actDataFb->deviceAddress(), cfgDataFb->deviceAddress());
      return false;
    }
  }

  if (!flatbuffers::IsFieldPresent(actDataFb, comm::axisprofile::fbtypes::profileConfig::VT_INPUTBUFFER))
  {
    // todo error diag
    APR_TRACE_ERROR("checkConfigData: Inputbuffer is not presented.");
    return false;
  }

  const char* current{};
  const char* loading{};

  if (flatbuffers::IsFieldPresent(cfgDataFb, comm::axisprofile::fbtypes::profileConfig::VT_INPUTBUFFER))

  {
    current = actDataFb->inputBuffer()->c_str();
    loading = cfgDataFb->inputBuffer()->c_str();
    // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("Inputbuffer conflict, current inputbuffer: %s, loading inputbuffer: %s", current, loading);
      return false;
    }
  }

  if (!flatbuffers::IsFieldPresent(actDataFb, comm::axisprofile::fbtypes::profileConfig::VT_OUTPUTBUFFER))
  {
    // todo error diag
    APR_TRACE_ERROR("checkConfigData: Outputbuffer is not presented.");
    return false;
  }

  if (flatbuffers::IsFieldPresent(cfgDataFb, comm::axisprofile::fbtypes::profileConfig::VT_OUTPUTBUFFER))
  {
    current = actDataFb->outputBuffer()->c_str();
    loading = cfgDataFb->outputBuffer()->c_str();
    // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("Outputbuffer conflict, current outputbuffer: %s, loading outputbuffer: %s", current, loading);
      return false;
    }
  }

  if (!flatbuffers::IsFieldPresent(actDataFb, comm::axisprofile::fbtypes::profileConfig::VT_PROFILEBASICDATA))
  {
    APR_TRACE_ERROR("checkConfigData: profile type is not presented.");
    return false;
  }
  
  if (flatbuffers::IsFieldPresent(cfgDataFb, comm::axisprofile::fbtypes::profileConfig::VT_PROFILEBASICDATA))
  {
    // needs to be the same
    
    if (actDataFb->profileBasicData()->type() != cfgDataFb->profileBasicData()->type())
    {
      APR_TRACE_ERROR("profile type conflict, current type: %s, loading type: %s", 
                      axisProfileTypeToString(static_cast<AxisProfileType>(actDataFb->profileBasicData()->type())),
                      axisProfileTypeToString(static_cast<AxisProfileType>(cfgDataFb->profileBasicData()->type())));
      return false;
    }
  }
  
  if (!flatbuffers::IsFieldPresent(actDataFb, comm::axisprofile::fbtypes::profileConfig::VT_INPUTMAPPING))
  {
    // todo error diag
    APR_TRACE_ERROR("checkConfigData: Input mapping is not presented.");
    return false;
  }

  if (flatbuffers::IsFieldPresent(cfgDataFb, comm::axisprofile::fbtypes::profileConfig::VT_INPUTMAPPING))
  {
    current = actDataFb->inputMapping()->mapping()->begin()->valueID()->c_str();
    loading = cfgDataFb->inputMapping()->mapping()->begin()->valueID()->c_str();
    // // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("First input mapping id conflict, current input mapping id : %s, loading input mapping id: %s", current, loading);
      return false;
    }
    current = actDataFb->inputMapping()->mapping()->begin()->datalayerURI()->c_str();
    loading = cfgDataFb->inputMapping()->mapping()->begin()->datalayerURI()->c_str();
    // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("First input mapping datalayer url conflict, current input mapping datalayer url: %s, loading input mapping datalayer url: %s", current, loading);
      return false;
    }

    current = actDataFb->inputMapping()->mapping()->rbegin()->valueID()->c_str();
    loading = cfgDataFb->inputMapping()->mapping()->rbegin()->valueID()->c_str();
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("Second input mapping id conflict, current input mapping id : %s, loading input mapping id: %s", current, loading);
      return false;
    }

    current = actDataFb->inputMapping()->mapping()->rbegin()->datalayerURI()->c_str();
    loading = cfgDataFb->inputMapping()->mapping()->rbegin()->datalayerURI()->c_str();
    // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("Second input mapping datalayer url conflict, current input mapping datalayer url: %s, loading input mapping datalayer url: %s", current, loading);
      return false;
    }
  }

  if (!flatbuffers::IsFieldPresent(actDataFb, comm::axisprofile::fbtypes::profileConfig::VT_OUTPUTMAPPING))
  {
    // todo error diag
    APR_TRACE_ERROR("checkConfigData: Output mapping is not presented.");
    return false;
  }

  if (flatbuffers::IsFieldPresent(cfgDataFb, comm::axisprofile::fbtypes::profileConfig::VT_OUTPUTMAPPING))
  {
    current = actDataFb->outputMapping()->mapping()->begin()->valueID()->c_str();
    loading = cfgDataFb->outputMapping()->mapping()->begin()->valueID()->c_str();
    // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("First output mapping id conflict, current output mapping id : %s, loading output mapping id: %s", current, loading);
      return false;
    }

    current = actDataFb->outputMapping()->mapping()->begin()->datalayerURI()->c_str();
    loading = cfgDataFb->outputMapping()->mapping()->begin()->datalayerURI()->c_str();
    // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("First output mapping datalayer url conflict, current output mapping datalayer url : %s, loading output mapping datalayer url: %s", current, loading);
      return false;
    }

    current = actDataFb->outputMapping()->mapping()->rbegin()->valueID()->c_str();
    loading = cfgDataFb->outputMapping()->mapping()->rbegin()->valueID()->c_str();
    // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("Second output mapping id conflict, current output mapping id : %s, loading output mapping id: %s", current, loading);
      return false;
    }

    current = actDataFb->outputMapping()->mapping()->rbegin()->datalayerURI()->c_str();
    loading = cfgDataFb->outputMapping()->mapping()->rbegin()->datalayerURI()->c_str();
    // needs to be the same
    if (0 != strcmp(current, loading))
    {
      APR_TRACE_ERROR("Second output mapping datalayer url conflict, current output mapping datalayer url : %s, loading output mapping datalayer url: %s", current, loading);         
      return false;
    }
  }

  //compare scaling data
  if (flatbuffers::IsFieldPresent(cfgDataFb, comm::axisprofile::fbtypes::profileConfig::VT_SCALINGINFO))
  {
    // needs to be the same
    if (actDataFb->scalingInfo()->denominator() != cfgDataFb->scalingInfo()->denominator())
    {
      APR_TRACE_ERROR("denominator conflict, current denominator: %d, loading denominator: %d",
        actDataFb->scalingInfo()->denominator(), cfgDataFb->scalingInfo()->denominator());
      return false;
    }
    if (actDataFb->scalingInfo()->numerator() != cfgDataFb->scalingInfo()->numerator())
    {
      APR_TRACE_ERROR("numerator conflict, current numerator: %d, loading numerator: %d",
        actDataFb->scalingInfo()->numerator(), cfgDataFb->scalingInfo()->numerator());
      return false;
    }
    if (actDataFb->scalingInfo()->resolution() != cfgDataFb->scalingInfo()->resolution())
    {
      APR_TRACE_ERROR("device resolution conflict, current device resolution: %d, loading device resolution: %d",
        actDataFb->scalingInfo()->resolution(), cfgDataFb->scalingInfo()->resolution());
      return false;
    }    
    if (actDataFb->scalingInfo()->scalingType()->scalingType() != cfgDataFb->scalingInfo()->scalingType()->scalingType())
    {
      APR_TRACE_ERROR("profile type conflict, current type: %s, loading type: %s",
        axisProfileDeviceScalingTypeToString(static_cast<AxisProfileDeviceScalingType>(actDataFb->scalingInfo()->scalingType()->scalingType())),
        axisProfileDeviceScalingTypeToString(static_cast<AxisProfileDeviceScalingType>(cfgDataFb->scalingInfo()->scalingType()->scalingType())));
      return false;
    }
    if (actDataFb->scalingInfo()->scalingStrategy()->strategy() != cfgDataFb->scalingInfo()->scalingStrategy()->strategy())
    {
      APR_TRACE_ERROR("scaling strategy conflict, current strategy: %s, loading strategy: %s",
        axisProfileScalingStrategyToString(static_cast<AxisProfileScalingStrategy>(actDataFb->scalingInfo()->scalingStrategy()->strategy())),
        axisProfileScalingStrategyToString(static_cast<AxisProfileScalingStrategy>(cfgDataFb->scalingInfo()->scalingStrategy()->strategy())));
      return false;
    }
  }

  if ( !flatbuffers::IsFieldPresent(actDataFb, comm::axisprofile::fbtypes::profileConfig::VT_DEVICENAME) )
  {
    // todo error diag
    APR_TRACE_ERROR("checkConfigData: Device name is not presented.");
    return false;
  }

  if ( flatbuffers::IsFieldPresent(cfgDataFb, comm::axisprofile::fbtypes::profileConfig::VT_DEVICENAME) )
  {
    current = actDataFb->deviceName()->c_str();
    loading = cfgDataFb->deviceName()->c_str();
    // needs to be the same
    if ( 0 != strcmp(current, loading) )
    {
      APR_TRACE_ERROR("Device name conflict, current device name: %s, loading device name: %s", current, loading);
      return false;
    }
  }
  return true;
}

AxisProfileResult BackupRestoreMgr::makePersistent(const comm::datalayer::Variant& persistenceParam, const std::string& axisprofileName)
{
  APR_TRACE_MESSAGE("Start to save the system configuration.");
  if (!m_persistenceHelper)
  {
    APR_TRACE_ERROR("%s", "Null-Pointer detected! Abort save system configuration...");
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // Verify input flatbuffer
  if ((persistenceParam.getType() != comm::datalayer::VariantType::FLATBUFFERS) || STATUS_FAILED(persistenceParam.verifyFlatbuffers(comm::datalayer::VerifyPersistenceParamBuffer)))
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, __FILE__, __FUNCTION__, __LINE__, "Abort make persistence...");
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  std::string configPath {};
  std::string id {};
  // correct config flatbuffer provided - gets checked in node
  auto inFb = comm::datalayer::GetPersistenceParam(persistenceParam.getData());

  // Extract file/folder path
  std::string phase {};
  AxisProfileResult returnValue = extractPersistenceData(inFb, configPath, id, phase);
  if (aprStatusFailed(returnValue))
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BACKUPRESTORE_EXTRACT_PERSISTENCE_FAILED, __FILE__, __FUNCTION__, __LINE__, "Abort make persistence...");
    return returnValue;
  }

   std::transform(phase.begin(), phase.end(), phase.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  if("SAVE" != phase)
  {
    APR_TRACE_ERROR("Wrong phase for %s of axisprofile object %s", phase.c_str(), axisprofileName.c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  auto profileObject = m_profileManager->getProfileByName(axisprofileName.c_str());
  if (profileObject)
  {
    std::string absolutePath;
    m_persistenceHelper->getAbsolutePath(m_persistenceHelper->handle, persistenceParam, m_bundleSubfolder, absolutePath);
    return exeMakePersistentCfgSave(axisprofileName, absolutePath);
  }

  // the profile object does not exists --> clear all related files in the files system (ignore all return codes)
  std::string rootPath;
  m_persistenceHelper->getRootPath(m_persistenceHelper->handle, rootPath);
  std::error_code ec;
  std::string filePath = rootPath + "/" + m_bundleSubfolder + axisprofileName + ".json";
  APR_TRACE_MESSAGE("Delete file \'%s\' [%s]", filePath.c_str(), id.c_str());
  std::filesystem::remove(filePath, ec);   // profile file config

  return AxisProfileResult::STS_OK;
}

AxisProfileResult BackupRestoreMgr::exeMakePersistentCfgSave(const std::string& axisprofileName, const std::string& folderPath)
{
  AxisProfileResult returnValue {AxisProfileResult::STS_OK};
  // Iterate over all profiles and generate the configuration files
  std::string configFilePath = folderPath + axisprofileName + ".json";

  // Save current profile data
  AxisProfileResult saveProfileResult = saveCfg(axisprofileName, configFilePath);
  if (aprStatusFailed(saveProfileResult))
  {
    APR_TRACE_ERROR("Could not save data of profile %s!", axisprofileName.c_str());
    returnValue = saveProfileResult;
  }

  APR_TRACE_MESSAGE("Saving the system configuration finished!");
  return returnValue;
}

AxisProfileResult BackupRestoreMgr::save(const std::string& profileName, const std::string& filePath)
{
  std::string fileNamePath = filePath.empty() ? m_bundleSubfolder + profileName + ".json" : filePath;
  AxisProfileResult saveCfgResult = saveCfg(profileName, fileNamePath);
  if (aprStatusFailed(saveCfgResult))
  {
    APR_TRACE_ERROR("Could not save data of profile %s, path:%s", profileName.c_str(), fileNamePath.c_str());
    return saveCfgResult;
  }
  return AxisProfileResult::STS_OK;
}

