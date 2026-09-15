// ---------------------------------------------------------------------------
//
//! @file       axisprofile_manager.cpp
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This source file is part of the axisprofile library and implements the
//! manager class for all axisprofiles.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_manager.h"
#include <set>
#include "common/debug_trace.h"
#include "common/datalayer/client_internal.h"
#include "profiles/i_axisprofile_internal.h"
#include "profiles/value_mapping/i_mapping_handler.h"
#include "profiles/axisprofile_factory.h"
#include "diagnosis/dia.h"
#include "fieldbuses/fieldbus_master.h"
#include "helper/datalayer_helper.h"
#include "component/background_thread/i_background_thread.h"
#include "common/fpu_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;


// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

AxisProfileManager::~AxisProfileManager()
{
  // Close datalayer Ethercat RT memory
  if ( m_datalayer )
  {
    // Deinit existing memories
    // Close the memories before deletion
    for ( auto& memoryInfo : m_vecCyclicMemoryAccessAT )
    {
      m_datalayer->closeMemory(memoryInfo.m_memoryUser);
    }
    m_vecCyclicMemoryAccessAT.clear();

    // Close the memories before deletion
    for ( auto& memoryInfo : m_vecCyclicMemoryAccessMDT )
    {
      m_datalayer->closeMemory(memoryInfo.m_memoryUser);
    }
    m_vecCyclicMemoryAccessMDT.clear();
  }

  for ( auto& it : m_mapAxisProfiles )
  {
    it.second.m_axisProfileShared.reset();
    delete it.second.m_axisProfile;
  }

  m_mapAxisProfiles.clear();

  if (m_datalayerClient)
  {
    delete m_datalayerClient;
    m_datalayerClient = nullptr;
  }

  m_stateMachineState = nullptr;
}


// -------------------------------------------------------------------------------
// ---------------------------- Internal Interface -------------------------------
// -------------------------------------------------------------------------------

AxisProfileResult AxisProfileManager::validateMappingCfg()
{
  std::vector<IAxisProfile*> activatedProfiles;
  getActivatedProfiles(activatedProfiles);
  if (!activatedProfiles.empty())
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_VALIDATE_MAPPING_CFG, __FILE__, __FUNCTION__, __LINE__, "");

    // for further check for duplication items
    size_t nCount{};                             // record mapping count for all profile
    std::set<std::string> setMapping{};          // record mapping string for all profile

    // CHECK NAME consistence in mapping and device name
    for ( const auto& it : m_mapAxisProfiles )
    {
      auto profile = it.second.m_axisProfile;
      if (!profile || !profile->checkProfileActivated())
      {
        continue;
      }

      IAxisProfileMappingHandler* pMapping = profile->getMappingHandler();
      // check for all input mapping items
      AxisProfileResult resCheckMappItem = pMapping->checkInputMappingItems(profile, nCount, setMapping);
      if ( AxisProfileResult::STS_OK != resCheckMappItem )
      {
        return resCheckMappItem;
      }

      // check for all output mapping items
      resCheckMappItem = pMapping->checkOutputMappingItems(profile, nCount, setMapping);
      if ( AxisProfileResult::STS_OK != resCheckMappItem )
      {
        return resCheckMappItem;
      }
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileManager::updateProfileAddress()
{  
  return m_deviceInfoMgr.updateProfileAddress();
}

AxisProfileResult AxisProfileManager::validateProtocolInfo()
{
  return m_deviceInfoMgr.validateProtocolInfo();
}

void AxisProfileManager::getFieldbusInstances(ContainerFieldbusInst& mapInstances)
{
  mapInstances = m_deviceInfoMgr.getFieldbusMasters();
}

AxisProfileResult AxisProfileManager::initManager(const bool& abortInitialization, double cycleTime)
{
  if ( !m_datalayer || !m_datalayerClient ||!m_datalayerClient->getBase() )
  {
    APR_TRACE_ERROR("Null-Pointer detected! Abort initManager operation...");
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_WAIT_FOR_PROFILE_MANAGER, __FILE__, __FUNCTION__, __LINE__, "");

  // Initialize datalayer RT memory access for every profile
  AxisProfileResult result = initMemory();
  if ( aprStatusNotOkay(result) )
  {
    return result;
  }
  
  // Initialize value mapping
  result = initMapping(abortInitialization);
  if ( aprStatusNotOkay(result) )
  {
    return result;
  }

  if ( m_mapAxisProfiles.size() > 0 )
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_WAIT_FOR_PROFILE_INITIALIZATION, __FILE__, __FUNCTION__, __LINE__, "");
    for ( auto& profile : m_mapAxisProfiles )
    {
      if (!profile.second.m_axisProfile->checkProfileActivated())
      {
        continue;
      }
      profile.second.m_axisProfile->setCycleTimeToScaling(cycleTime);
      for ( const auto& method : profile.second.m_axisProfile->getInitMethods() )
      {
        if (abortInitialization)
        {
          Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_ABORTED,
                             __FILE__, __FUNCTION__, __LINE__,
                             "Booting is aborted by UI or datalayer!");
          return AxisProfileResult::STS_BOOTING_ABORT;
        }

        AxisProfileResult funcResult = method();
        if ( aprStatusFailed(funcResult) )
        {
          result = funcResult;
        }
      }
    }
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_PROFILE_READY, __FILE__, __FUNCTION__, __LINE__, "");
  }

  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_PROFILE_MANAGER_READY, __FILE__, __FUNCTION__, __LINE__, "");
  return result;
}

AxisProfileResult AxisProfileManager::browseFieldbusMasterInstance(const bool& abortInitialization)
{
  std::vector<IAxisProfile*> profiles;
  getActivatedProfiles(profiles);
  if (!profiles.empty())
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_INSTANCES, __FILE__, __FUNCTION__, __LINE__, "");

    comm::datalayer::Variant data {};
    const comm::datalayer::Variant* convData = &data;
    comm::datalayer::DlResult dlResult;
    std::uint16_t timeout = 10;      // 10 seconds timeout
    std::uint16_t pollingMs = 100;   // 100 MS every polling
    std::uint16_t nTimeout = timeout * 1000 / 100;

    // try to browse master instance, when no usable data(not timeout)
    while (true)
    {
      if (abortInitialization)
      {
        Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_ABORTED,
                           __FILE__, __FUNCTION__, __LINE__, "By Web or System");
        return AxisProfileResult::STS_BOOTING_ABORT;
      }

      // Browse should return DL_INVALID_ADDRESS as long as EthercatMaster boot has not been finished.
      // Afterwards it could return with success but without any instances (empty list). We can not continue here without a valid instance.
      dlResult = m_datalayerClient->browseSync(DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES, &data);
      if ((comm::datalayer::STATUS_FAILED(dlResult)) && (dlResult != comm::datalayer::DlResult::DL_INVALID_ADDRESS))
      {
        APR_TRACE_ERROR("Could not browse %s! DlResult: %s", DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES.c_str(), dlResult.toString());
        return AxisProfileResult::ERR_DATALAYER_BROWSE_NODE;
      }
      nTimeout--;
      if ((dlResult == comm::datalayer::DlResult::DL_OK) || (nTimeout == 0 && data.getCount() == 0))
      {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(pollingMs));
    }
    if (data.getCount() == 0)
    {
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_MASTER_NOT_EXIT, __FILE__, __FUNCTION__, __LINE__,
                         "Master URL: %s, DL_Result: %s", DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES.c_str(), dlResult.toString());
      return AxisProfileResult::ERR_FIELDBUS_NO_MASTER;
    }
    // Convert data
    const char** instanceNames = *convData;
    for (auto const& profile : m_mapAxisProfiles)
    {
      if (!profile.second.m_axisProfile->checkProfileActivated())
      {
        continue;
      }
      for (std::uint8_t i = 0; i < data.getCount(); i++)
      {
        std::string instanceName(instanceNames[i]);
        std::string address = DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + '/' + instanceName + '/';
        if (profile.second.m_outputBufferUri.find(address) != std::string::npos)
        {
          address.pop_back(); // Remove last character (slash)
          m_deviceInfoMgr.addFieldbusInstance(instanceName, profile.second.m_axisProfile);
          profile.second.m_axisProfile->getFieldbusAdapter()->setFieldbusMasterAddress(address);
          APR_TRACE_MESSAGE("Configure fieldbus instance '%s' for profile '%s'", address.c_str(), profile.first.c_str());
          break;
        }
        else
        {
          Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_FIELDBUS_INCONSISTENCE, __FILE__, __FUNCTION__, __LINE__,
                             "%s: configured URL: %s, master URL: %s",
                             profile.first.c_str(),
                             profile.second.m_outputBufferUri.c_str(),
                             address.c_str());
          return AxisProfileResult::ERR_PROFILE_CONFIG;
        }
      }
    }
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_FIELDBUS_INSTANCES_READY, __FILE__, __FUNCTION__, __LINE__, "");
  }
  return AxisProfileResult::STS_OK;
}

void AxisProfileManager::setDataLayer(comm::datalayer::IDataLayerFactory* datalayer, IBackgroundThread* bkThread)
{
  m_datalayer = datalayer;
  if (!m_datalayerClient)
  {
    auto client = m_datalayer->createClient();
    if (client)
    {
      m_datalayerClient = new comm::datalayer::ClientInternal(client);
    }
  }

  // add background function of wrapped datalayer client.
  if (bkThread && m_datalayerClient)
  {
    bkThread->appCallbackInNormalState(m_datalayerClient->getBackgroundFun());
    m_datalayerClient->enableBackgroundFun(true);
  }
}

AxisProfileResult AxisProfileManager::processDataAt()
{
  checkForFPUError();
  for ( auto& memoryInfo : m_vecCyclicMemoryAccessAT )
  {
    std::uint8_t* data;
    comm::datalayer::DlResult dlResult = memoryInfo.m_memoryUser->beginAccess(data, memoryInfo.m_revision);
    if ( comm::datalayer::STATUS_FAILED(dlResult) )
    {
      if ( !m_traceErrorAlreadySentAT )
      {
        APR_TRACE_ERROR("Could not begin access of input RT-Memory %s; DlResult: %s", memoryInfo.m_bufferURI.c_str(), dlResult.toString())
          m_traceErrorAlreadySentAT = true;
      }
      return AxisProfileResult::ERR_DATALAYER;
    }

    // update profiles
    for ( auto const& profile : memoryInfo.m_profiles )
    {
      if ( AxisProfileResult::STS_OK != static_cast<IAxisProfileInternal*>( profile )->processDataAt(data) )
      {
        APR_TRACE_ERROR("Could not process cyclic input data");
        return AxisProfileResult::ERR_CYCLIC_DATA_AT;
      }
    }

    // End access of datalayer RT memory
    dlResult = memoryInfo.m_memoryUser->endAccess();
    if ( comm::datalayer::DlResult::DL_OK != dlResult )
    {
      if ( !m_traceErrorAlreadySentAT )
      {
        APR_TRACE_ERROR("Could not end access of input RT-Memory %s; DlResult: %s", memoryInfo.m_bufferURI.c_str(), dlResult.toString())
          m_traceErrorAlreadySentAT = true;
      }
      return AxisProfileResult::ERR_DATALAYER;
    }

    m_traceErrorAlreadySentAT = false;
  }

  // Note: fieldbus synchronize time has already generated in same cycle of Fieldbus
  auto& masters = m_deviceInfoMgr.getFieldbusMasters();
  for (auto& master : masters)
  {
    auto result = master.second->ProcessRtData();
    if (aprStatusNotOkay(result))
    {
      return result;
    }

    auto& linkedProfiles = master.second->getLinkedProfiles();
    for (auto& profile : linkedProfiles)
    {
      double sync0ShiftTime {0.0};
      profile->getDeviceParamInterface()->getDCSync0ShiftTime(sync0ShiftTime);

      double absSync0Time = master.second->getActDcBaseTime() + sync0ShiftTime;

      profile->getCyclicDataInterface()->setActSyncTime(absSync0Time, master.second->getFieldbusCycleTime());
    }
  }
  checkForFPUError();

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileManager::processDataMdt()
{
  checkForFPUError();
  for ( auto& memoryInfo : m_vecCyclicMemoryAccessMDT )
  {
    std::uint8_t* data;
    comm::datalayer::DlResult dlResult = memoryInfo.m_memoryUser->beginAccess(data, memoryInfo.m_revision);
    if ( comm::datalayer::STATUS_FAILED(dlResult) )
    {
      if ( !m_traceErrorAlreadySentMDT )
      {
        APR_TRACE_ERROR("Could not begin access of output RT-Memory %s; DlResult: %s", memoryInfo.m_bufferURI.c_str(), dlResult.toString())
          m_traceErrorAlreadySentMDT = true;
      }
      return AxisProfileResult::ERR_DATALAYER;
    }

    // update profiles
    for ( auto const& profile : memoryInfo.m_profiles )
    {
      if ( AxisProfileResult::STS_OK != static_cast<IAxisProfileInternal*>( profile )->processDataMdt(data) )
      {
        APR_TRACE_ERROR("Could not process cyclic output data");
        return AxisProfileResult::ERR_CYCLIC_DATA_MDT;
      }
    }

    // End access of datalayer RT memory
    dlResult = memoryInfo.m_memoryUser->endAccess();
    if ( comm::datalayer::DlResult::DL_OK != dlResult )
    {
      if ( !m_traceErrorAlreadySentMDT )
      {
        APR_TRACE_ERROR("Could not end access of output RT-Memory %s; DlResult: %s", memoryInfo.m_bufferURI.c_str(), dlResult.toString())
          m_traceErrorAlreadySentMDT = true;
      }
      return AxisProfileResult::ERR_DATALAYER;
    }

    m_traceErrorAlreadySentMDT = false;
  }
  checkForFPUError();

  return AxisProfileResult::STS_OK;
}

IAxisProfile* AxisProfileManager::getProfileByName(const char* profileName)
{
  ProfileInfo* profileInfo = getProfileInfoByName(profileName);
  if ( !profileInfo )
  {
    return nullptr;
  }
  else
  {
    return profileInfo->m_axisProfile;
  }
}

void AxisProfileManager::getProfileNames(std::vector<std::string>& profileNames)
{
  profileNames.clear();

  for ( auto const& profileInfo : m_mapAxisProfiles )
  {
    profileNames.emplace_back(profileInfo.first);
  }
  return;
}

AxisProfileResult AxisProfileManager::setProfileInputBuffer(const char* profileName, const char* inputBufferURI)
{
  // Check for null-pointers
  if ( !profileName || !inputBufferURI )
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // Get profile
  ProfileInfo* profileInfo = getProfileInfoByName(profileName);
  if ( !profileInfo )
  {
    return AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  }

  // Check if given string is empty
  std::string buffer{ inputBufferURI };
  if ( 0 == buffer.size() )
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  profileInfo->m_inputBufferUri = inputBufferURI;

  return AxisProfileResult::STS_OK;
}

const char* AxisProfileManager::getProfileInputBuffer(const char* profileName)
{
  ProfileInfo* profileInfo = getProfileInfoByName(profileName);
  if ( !profileInfo )
  {
    return nullptr;
  }

  return profileInfo->m_inputBufferUri.c_str();
}

AxisProfileResult AxisProfileManager::setProfileOutputBuffer(const char* profileName, const char* outputBufferURI)
{
  // Check for null-pointers
  if ( !profileName || !outputBufferURI )
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // Get profile
  ProfileInfo* profileInfo = getProfileInfoByName(profileName);
  if ( !profileInfo )
  {
    return AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  }

  // Check if given string is empty
  std::string buffer{ outputBufferURI };
  if ( 0 == buffer.size() )
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  profileInfo->m_outputBufferUri = outputBufferURI;

  return AxisProfileResult::STS_OK;
}

const char* AxisProfileManager::getProfileOutputBuffer(const char* profileName)
{
  ProfileInfo* profileInfo = getProfileInfoByName(profileName);
  if ( !profileInfo )
  {
    return nullptr;
  }

  return profileInfo->m_outputBufferUri.c_str();
}

AxisProfileResult AxisProfileManager::resetAllFPUFlag()
{
  // ok for empty profile
  if (m_mapAxisProfiles.empty())
  {
    return AxisProfileResult::STS_OK;
  }

  // reset the FPU error flag in each profile
  std::for_each(m_mapAxisProfiles.cbegin(),
                m_mapAxisProfiles.cend(),
                [](const auto& it){it.second.m_axisProfile->resetFPUFlag();});

  // Clear all FPU exceptions in cpu flag
  fpu::clearFPUFlags();

  return AxisProfileResult::STS_OK;
}

// -------------------------------------------------------------------------------
// ---------------------------- External Interface -------------------------------
// -------------------------------------------------------------------------------

AxisProfileResult AxisProfileManager::createAxisProfile(const AxisProfileType type, const std::string& profileName)
{
  if ( m_mapAxisProfiles.size() >= MAX_NUMBER_OF_PROFILES )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CREATE_PROFILE_MAX_NUMBER, __FILE__, __FUNCTION__, __LINE__, "Maximum  number of profiles %d reached", MAX_NUMBER_OF_PROFILES);
    return AxisProfileResult::ERR_MAX_NUMBER_PROFILES;
  }

  if ( profileName.empty() )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CREATE_PROFILE_INVALID_NAME, __FILE__, __FUNCTION__, __LINE__, "Profile Name is empty.");
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  const IAxisProfile* existingProfile = getProfileByName(profileName.c_str());
  if ( existingProfile )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CREATE_PROFILE_ALREADY_EXIST, __FILE__, __FUNCTION__, __LINE__, "Requested name was '%s'", profileName.c_str());
    return AxisProfileResult::ERR_PROFILE_ALREADY_EXISTS;
  }
  
  // Add uninitialized element to map
  m_mapAxisProfiles[ profileName ].m_axisProfile = nullptr;
  auto it = m_mapAxisProfiles.find(profileName);
  AxisProfileResult result = AxisProfileFactory::createProfile(profileName, type, &it->second.m_axisProfile);
  if ( aprStatusFailed(result) )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CREATE_PROFILE_INVALID_TYPE, __FILE__, __FUNCTION__, __LINE__, "Invalid/unimplemented type: %d", type);
    m_mapAxisProfiles.erase(profileName);
    return AxisProfileResult::ERR_NOTIMPLEMENTED;
  }
  else if ( it->second.m_axisProfile )
  {
    it->second.m_axisProfileShared.reset(it->second.m_axisProfile->createCInterface());
  }
  if ( aprStatusSucceeded(result) )
  {
    it->second.m_axisProfile->getCyclicDataInterface()->setStateMachineStatusIF(m_stateMachineState);
  }

  if ( !it->second.m_axisProfile || !it->second.m_axisProfileShared )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CREATE_PROFILE_OUT_OF_MEMORY, __FILE__, __FUNCTION__, __LINE__, "no memory is available for create profile");
    it->second.m_axisProfileShared.reset();
    delete it->second.m_axisProfile;
    m_mapAxisProfiles.erase(profileName);
    return AxisProfileResult::ERR_OUT_OF_MEMORY;
  }

  result = it->second.m_axisProfile->init();
  if ( aprStatusFailed(result) )
  {
    APR_TRACE_ERROR("Could not initialize new axis profile '%s'! Delete profile.", profileName.c_str());
    it->second.m_axisProfileShared.reset();
    m_mapAxisProfiles.erase(profileName);
    return result;
  }

  it->second.m_axisProfile->getFieldbusAdapter()->setDatalayer(m_datalayerClient);
  APR_TRACE_MESSAGE("New axis profile '%s' successfully created.", profileName.c_str());
  return result;
}

AxisProfileResult AxisProfileManager::deleteAxisProfile(const std::string& profileName)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  if ( profileName.empty() )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_DELETE_PROFILE_INVALID_NAME, __FILE__, __FUNCTION__, __LINE__, "Name is empty.");
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  auto iter = m_mapAxisProfiles.find(profileName);
  if ( iter == m_mapAxisProfiles.end() )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_DELETE_PROFILE_NOT_FOUND, profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
    result = AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  }
  else if ( iter->second.m_axisProfileShared.use_count() > 1 )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_DELETE_PROFILE_IN_USE, profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
    result = AxisProfileResult::ERR_PROFILE_IN_USE;
  }
  else
  {
    iter->second.m_axisProfileShared.reset();
    delete iter->second.m_axisProfile;
    m_mapAxisProfiles.erase(iter);
    APR_TRACE_MESSAGE("Axis profile '%s' successfully deleted.", profileName.c_str())
  }

  return result;
}

AxisProfileResult AxisProfileManager::connectAxisProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& profile)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  if ( profileName.empty() )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CONNECT_PROFILE_INVALID_NAME, __FILE__, __FUNCTION__, __LINE__, "Name is empty.");
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  if ( m_mapAxisProfiles.empty() )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CONNECT_PROFILE_FAILED, profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "Connect failed.");
    return AxisProfileResult::ERR_PROFILE_CONFIG;
  }

  auto iter = m_mapAxisProfiles.find(profileName);
  if ( iter == m_mapAxisProfiles.end() )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CONNECT_PROFILE_NOT_FOUND, profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
    result = AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  }
  else if ( iter->second.m_axisProfileShared.use_count() != 1 )
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_CONNECT_PROFILE_IN_USE, profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
    result = AxisProfileResult::ERR_PROFILE_IN_USE;
  }
  else
  {
    profile = iter->second.m_axisProfileShared;  // This will increment the use count of the shared pointer
    APR_TRACE_MESSAGE("Axis profile '%s' successfully connected.", profileName.c_str())
  }

  return result;
}

bool AxisProfileManager::profileExists(const std::string& profileName) const
{
  return ( m_mapAxisProfiles.find(profileName) == m_mapAxisProfiles.end() ) ? false : true;
}

// -------------------------------------------------------------------------------
// ------------------------------ Private Methods --------------------------------
// -------------------------------------------------------------------------------

AxisProfileResult AxisProfileManager::initMemory()
{
  // Deinit existing memories
  closeAllMemories();
  resetRtInputData();

  for ( auto const& profileInfo : m_mapAxisProfiles )
  {
    if (!profileInfo.second.m_axisProfile->checkProfileActivated())
    {
      continue;
    }
    // Add input buffer
    bool newBufferUriInput = true;
    for ( auto& memoryInfo : m_vecCyclicMemoryAccessAT )
    {
      if ( !memoryInfo.m_bufferURI.compare(profileInfo.second.m_inputBufferUri) )
      {
        // Buffer is already registered. Just add profile to vector
        memoryInfo.m_profiles.push_back(profileInfo.second.m_axisProfile);
        newBufferUriInput = false;
        break;
      }
    }

    if ( newBufferUriInput )
    {
      m_vecCyclicMemoryAccessAT.emplace_back(profileInfo.second.m_inputBufferUri, profileInfo.second.m_axisProfile);
    }

    // Add output buffer
    bool newBufferUriOutput = true;
    for ( auto& memoryInfo : m_vecCyclicMemoryAccessMDT )
    {
      if ( !memoryInfo.m_bufferURI.compare(profileInfo.second.m_outputBufferUri) )
      {
        // Buffer already is registered. Just add profile to vector
        memoryInfo.m_profiles.push_back(profileInfo.second.m_axisProfile);
        newBufferUriOutput = false;
        break;
      }
    }

    if ( newBufferUriOutput )
    {
      m_vecCyclicMemoryAccessMDT.emplace_back(profileInfo.second.m_outputBufferUri, profileInfo.second.m_axisProfile);
    }
  }

  // Open the memories for usage
  for ( auto& memoryInfo : m_vecCyclicMemoryAccessAT )
  {
    if ( memoryInfo.m_bufferURI.size() == 0 )
    {
      for ( auto itProfiles = memoryInfo.m_profiles.begin(); itProfiles != memoryInfo.m_profiles.end(); ++itProfiles )
      {
        Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_WAIT_FOR_PROFILE_MANAGER, __FILE__, __FUNCTION__, __LINE__, "Input buffer for profile %s was empty",(*itProfiles)->getProfileName());
      }
      closeAllMemories();
      return AxisProfileResult::ERR_PROFILE_CONFIG;
    }

    comm::datalayer::DlResult result = m_datalayer->openMemory(memoryInfo.m_memoryUser, memoryInfo.m_bufferURI);
    if ( comm::datalayer::STATUS_FAILED(result) )
    {
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_WAIT_FOR_PROFILE_MANAGER, __FILE__, __FUNCTION__, __LINE__,
                         "Not able to open AT-memory with address: %s, error: %s", memoryInfo.m_bufferURI.c_str(), result.toString());

      closeAllMemories();
      return AxisProfileResult::ERR_DATALAYER;
    }
  }

  // Open the memories for usage
  for ( auto& memoryInfo : m_vecCyclicMemoryAccessMDT )
  {
    if ( memoryInfo.m_bufferURI.size() == 0 )
    {
      for ( auto itProfiles = memoryInfo.m_profiles.begin(); itProfiles != memoryInfo.m_profiles.end(); ++itProfiles )
      {
        Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_WAIT_FOR_PROFILE_MANAGER, __FILE__, __FUNCTION__, __LINE__, "Output buffer for profile %s was empty", (*itProfiles)->getProfileName());
      }
      closeAllMemories();
      return AxisProfileResult::ERR_PROFILE_CONFIG;
    }

    comm::datalayer::DlResult result = m_datalayer->openMemory(memoryInfo.m_memoryUser, memoryInfo.m_bufferURI);
    if ( comm::datalayer::STATUS_FAILED(result) )
    {
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_WAIT_FOR_PROFILE_MANAGER, __FILE__, __FUNCTION__, __LINE__,
                         "Not able to open MDT-memory with address: %s, error: %s", memoryInfo.m_bufferURI.c_str(), result.toString());
      closeAllMemories();
      return AxisProfileResult::ERR_DATALAYER;
    }
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileManager::initMapping(const bool& abortInitialization)
{
  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_RT_MEMORY, __FILE__, __FUNCTION__, __LINE__, "");

  // Read the memory map and init profiles
  APR_TRACE_MESSAGE("Get all AT memory maps from datalayer RT.");
  GetRtMemMapCallback atMemMapCallback = [] (RtMemInfoBase* memInfo, const comm::datalayer::MemoryMap* memMap)
  {
    auto info = static_cast<MemoryInfo*>(memInfo);
    for (auto const& profile : info->m_profiles)
    {
      AxisProfileResult profileResult = static_cast<IAxisProfileInternal*>(profile)->getMappingHandler()->initRealTimeMappingInput(memMap);
      if (AxisProfileResult::STS_OK != profileResult)
      {
        APR_TRACE_MESSAGE("Init RT input mapping failed! current profile: %s, Memory map URI: %s.", static_cast<IAxisProfileInternal*>(profile)->getProfileName(), info->m_bufferURI.c_str());
        return profileResult;
      }
    }
    return AxisProfileResult::STS_OK;
  };
  for ( auto& memoryInfo : m_vecCyclicMemoryAccessAT )
  {
    auto result = DatalayerHelper::getMemoryMap(memoryInfo, atMemMapCallback, abortInitialization);
    if (aprStatusNotOkay(result))
    {
      return result;
    }
  }

  APR_TRACE_MESSAGE("Get all MDT memory maps from datalayer RT.");
  GetRtMemMapCallback mdtMemMapCallback = [] (RtMemInfoBase* memInfo, const comm::datalayer::MemoryMap* memMap)
  {
    auto info = static_cast<MemoryInfo*>(memInfo);
    for (auto const& profile : info->m_profiles)
    {
      AxisProfileResult profileResult = static_cast<IAxisProfileInternal*>(profile)->getMappingHandler()->initRealTimeMappingOutput(memMap);
      if (AxisProfileResult::STS_OK != profileResult)
      {
        APR_TRACE_MESSAGE("Init RT output mapping failed! current profile: %s, Memory map URI: %s.", static_cast<IAxisProfileInternal*>(profile)->getProfileName(), info->m_bufferURI.c_str());
        return profileResult;
      }
    }
    return AxisProfileResult::STS_OK;
  };
  for (auto& memoryInfo : m_vecCyclicMemoryAccessMDT)
  {
    auto result = DatalayerHelper::getMemoryMap(memoryInfo, mdtMemMapCallback, abortInitialization);
    if (aprStatusNotOkay(result))
    {
      return result;
    }
  }

  auto& masters = m_deviceInfoMgr.getFieldbusMasters();
  for (auto& master : masters)
  {
    auto result = master.second->initRtData(abortInitialization);
    if (aprStatusNotOkay(result))
    {
      return result;
    }
  }

  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_FIELDBUS_RT_MEMORY_READY, __FILE__, __FUNCTION__, __LINE__, "");
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileManager::initDeviceInfoMgr()
{
  m_deviceInfoMgr.clearFieldbusContainer();
  m_deviceInfoMgr.clearDeviceInfoContainer();
  m_deviceInfoMgr.setDataLayerFactory(m_datalayer);
  m_deviceInfoMgr.setDataLayer(m_datalayerClient);
  m_deviceInfoMgr.setProfiles(&m_mapAxisProfiles);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileManager::closeAllMemories()
{
  // Close datalayer Ethercat RT memory
  if ( m_datalayer )
  {
    // Deinit existing memories
    // Close the memories before deletion
    for ( auto& memoryInfo : m_vecCyclicMemoryAccessAT )
    {
      m_datalayer->closeMemory(memoryInfo.m_memoryUser);
    }
    m_vecCyclicMemoryAccessAT.clear();

    // Close the memories before deletion
    for ( auto& memoryInfo : m_vecCyclicMemoryAccessMDT )
    {
      m_datalayer->closeMemory(memoryInfo.m_memoryUser);
    }
    m_vecCyclicMemoryAccessMDT.clear();
  }

  return AxisProfileResult::STS_OK;
}

ProfileInfo* AxisProfileManager::getProfileInfoByName(const char* profileName)
{
  if ( !profileName )
  {
    return nullptr;
  }
  std::map<std::string, ProfileInfo>::iterator elem = m_mapAxisProfiles.find(profileName);
  if ( elem == m_mapAxisProfiles.end() )
  {
    return nullptr;
  }
  else
  {
    return &elem->second;
  }
}

AxisProfileResult AxisProfileManager::processAuxJobs()
{
  checkForFPUError();
  for ( auto& it : m_mapAxisProfiles )
  {
    it.second.m_axisProfile->runAllAuxJobs();
  }
  checkForFPUError();

  return AxisProfileResult::STS_OK;
}

std::uint32_t AxisProfileManager::getConfiguredProfileNumber() const
{
  return static_cast<std::uint32_t>(m_mapAxisProfiles.size());
}

AxisProfileResult AxisProfileManager::registerCheckCallback(const CheckCallBack& checkFct, const std::string& bundleName)
{
  m_mutex.lock();
  m_extChkFuncs.emplace(bundleName, checkFct);
  m_mutex.unlock();
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileManager::unregisterAllCheckCallbacks(const std::string& bundleName)
{
  m_mutex.lock();
  // filtering and erasing
  auto range = m_extChkFuncs.equal_range(bundleName);
  m_extChkFuncs.erase(range.first, range.second);
  m_mutex.unlock();
  return AxisProfileResult::STS_OK;
}

void AxisProfileManager::resetRtInputData()
{
  for (auto& profile : m_mapAxisProfiles)
  {
    profile.second.m_axisProfile->resetRtInputData();
  }
}

void AxisProfileManager::checkForFPUError()
{
  // LCOV_EXCL_START The following code has been tested by other code.
  AxisProfileResult result = AxisProfileResult::STS_OK;
  if (!fpu::g_FPUError)
  {
    result = fpu::checkForFPUError(AxisProfileResult::STS_OK, AxisProfileResult::ERR_FPU_DIV_BY_ZERO);
    if (aprStatusFailed(result))
    {
      APR_TRACE_ERROR("FPU error occurred.");
      fpu::g_FPUError = true;
    }
  }
  // LCOV_EXCL_STOP to exclude specific lines of the code
}

void AxisProfileManager::runAllJobs()
{
  for (auto& it : m_mapAxisProfiles)
  {
    it.second.m_axisProfile->runJobsInBkThread();
  }
}

AxisProfileResult AxisProfileManager::execExternalChecking()
{
  if (!m_extChkFuncs.empty())
  {
    for (auto& profile : m_mapAxisProfiles)
    {
      APR_TRACE_MESSAGE("External function checking (%d checkers registered) for profile '%s'",
                        m_extChkFuncs.size(), profile.first.c_str());
      for (auto& chkFunc : m_extChkFuncs)
      {
        const std::string& profileName = profile.second.m_axisProfile->getProfileName();
        auto res = chkFunc.second(profileName);
        profile.second.m_axisProfile->setProfileActivated(res != CheckResponse::OK_DONT_USE_PROFILE);
        if (!profile.second.m_axisProfile->checkProfileActivated())
        {
          Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_PROFILE_CHECK_RESP_DONTUSE,
                             __FILE__, __FUNCTION__, __LINE__, "%s, %s", chkFunc.first.c_str(), profileName.c_str());
          break;
        }
      }
    }
  }

  return AxisProfileResult::STS_OK;
}

void AxisProfileManager::getActivatedProfiles(std::vector<IAxisProfile*> &profiles)
{
  profiles.clear();
  for (auto& profile : m_mapAxisProfiles)
  {
    if (profile.second.m_axisProfile->checkProfileActivated())
    {
      profiles.push_back(profile.second.m_axisProfile);
    }
  }
}
