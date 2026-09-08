/// @file
///
/// @brief This source file is part of the axisprofile library and implements the provider manager for datalayer nodes.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/persistence_helper_itf.h"
#include "comm/datalayer/scopes_generated.h"
#include "common/debug_trace.h"
#include "component/i_axisprofile_component.h"
#include "provider_mgr.h"
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
ProviderMgr::ProviderMgr()
{
}

ProviderMgr::~ProviderMgr()
{
  for (const auto& nodeEntry : m_allNodes)
  {
    delete nodeEntry.m_node;
  }
  m_allNodes.clear();

  if ( !m_cmpAxisProfile )
  {
    return;
  }

  const comm::datalayer::IDataLayerFactory *dlFactory = m_cmpAxisProfile->getDataLayer();
  if ( !dlFactory )
  {
    // the datalayer is gone completely, so we don't have to free the provider (hopefully, the datalayer cleaned up himself)
    return;
  }

  if ( m_dlProvider )
  {
    // this will unregister all nodes implicitly
    delete m_dlProvider;
    m_dlProvider = nullptr;
  }
}

void ProviderMgr::addNode(const std::string &address, INodeBase* newNode)
{
  m_allNodes.emplace_back(address, newNode );
}

void ProviderMgr::init(comm::axisprofile::IAxisProfileComponent *cmpAxisProfile)
{
  m_cmpAxisProfile = cmpAxisProfile;
  m_allNodes.reserve(21 + MAX_NUMBER_OF_PROFILES); //currently 21 node in total, max: 30 profiles
  m_nodeSystemCreator.addNodes(this);
  m_nodeProfilesCreator.addNodes(this);  
}

bool ProviderMgr::run()
{
  if (!m_cmpAxisProfile )
  {
    return false;
  }

  for ( const auto &nodeEntry : m_allNodes )
  {
    if ( !nodeEntry.m_node )
    {
      return false;     // out of memory when allocating the node
    }
  }

  comm::datalayer::IDataLayerFactory *dlFactory = m_cmpAxisProfile->getDataLayer();
  if (!dlFactory)
  {
    // the datalayer is not set, but that's okay
    // there are no nodes available
    return true;
  }

  // when we have a provider already, then this function was called twice, without a call to stop()
  // in this case, we free the provider with all nodes first
  if (m_dlProvider)
  {
    delete m_dlProvider;
    m_dlProvider = nullptr;
  }

  // get a provider from the data layer
  m_dlProvider = dlFactory->createProvider();
  if (!m_dlProvider)
  {
    return false;
  }

  comm::datalayer::DlResult res{};
  bool result{true};
  // register all known nodes at the provider
  for (const auto &nodeEntry : m_allNodes)
  {
    res = m_dlProvider->registerNode(nodeEntry.m_address, nodeEntry.m_node);
    const char *bfbsName = nodeEntry.m_node->getBfbsName();
    // only register when there is a flatbuffer behind the type
    if ( bfbsName )
    {
      result &= registerTypes(bfbsName, nodeEntry.m_address);
    }
  }

  registerScopes();
  m_dlProvider->start();

  result &= waitForAllNodes( dlFactory );
  return result;
}

bool ProviderMgr::waitForAllNodes( comm::datalayer::IDataLayerFactory *dlFactory )
{
  comm::datalayer::IClient *client = dlFactory->createClient();
  if ( !client )
  {
    return false;
  }

  const std::uint32_t timeOutMS = 10 * 1000;   // 10 seconds timeout
  const std::uint32_t sleepTimeMS = 50;
  std::uint32_t timeOut = timeOutMS / sleepTimeMS;

  bool everyThingOnline = false;
  while ( ( timeOut > 0 ) && checkForAllNodes( client, everyThingOnline ) && !everyThingOnline )
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMS));
    --timeOut;
  }

  delete client;
  return everyThingOnline;
}

bool ProviderMgr::checkForAllNodes( comm::datalayer::IClient *client, bool &everyThingOnline )
{
  everyThingOnline = false;

  const char *dataLayerNodes = "datalayer/nodes";
  comm::datalayer::Variant data;
  comm::datalayer::DlResult dlResult = client->readSync( dataLayerNodes, &data );
  if ( comm::datalayer::STATUS_FAILED( dlResult ) || ( data.getType() != comm::datalayer::VariantType::ARRAY_OF_STRING ) )
  {
    return false;
  }

  const char** convActData = data;
  std::set<std::string> gotData;
  for ( std::uint32_t idx = 0; idx < data.getCount(); idx++ )
  {
    gotData.insert( convActData[idx] );
  }

  for ( auto &nodeEntry : m_allNodes )
  {
    if ( gotData.find( nodeEntry.m_address ) == gotData.end() )
    {
      return true;
    }
  }
  everyThingOnline = true;

  return true;
}

bool ProviderMgr::stop()
{
  if (!m_cmpAxisProfile )
  {
    return false;
  }

  const comm::datalayer::IDataLayerFactory *dlFactory = m_cmpAxisProfile->getDataLayer();
  if (!dlFactory)
  {
    // the datalayer is gone completely, so we don't have to free the provider (hopefully, the datalayer cleaned up himself)
    return true;
  }

  if (m_dlProvider)
  {
    unregisterScopes();

    // this will unregister all nodes implicitly
    delete m_dlProvider;
    m_dlProvider = nullptr;
  }

  return true;
}

bool ProviderMgr::registerTypes(const std::string &bfbsName, const std::string &nodeAddress)
{
  if (!m_cmpAxisProfile->getPersistenceHelper())
  {
    APR_TRACE_ERROR("PersistenceHelper missing! Can not register types at datalayer...");
    return false;
  }

  std::string bundlePath = m_cmpAxisProfile->getBundlePath();
  if ( bundlePath.empty() )
  {
    // we are not in bundle environment, check for files
    bundlePath = "../../../../../public/bfbs/comm/axisprofile/";     // Path on testbuddy, run from VS

    if ( !m_cmpAxisProfile->getPersistenceHelper()->fileExists(m_cmpAxisProfile->getPersistenceHelper()->handle, (bundlePath + bfbsName)) )
    {
      bundlePath = "../../../../public/bfbs/comm/axisprofile/";      // Path on Jenkins (Windows) and TestBuddy run from BuildBuddy
    }
  }

  // IM3 Workaround
  std::string tmpBfbsFile = bundlePath + "/bfbs/" + bfbsName;
  if ( !m_cmpAxisProfile->getPersistenceHelper()->fileExists(m_cmpAxisProfile->getPersistenceHelper()->handle, tmpBfbsFile) )
  {
    tmpBfbsFile = "bfbs/comm/axisprofile/" + bfbsName;
  }

  APR_TRACE_MESSAGE("path to bfbs file: %s", tmpBfbsFile.c_str());
  // delete wildcard from address
  std::string addressForType = nodeAddress;
  size_t pos {std::string::npos};
  while ( ( pos = addressForType.find("/*") ) != std::string::npos )
  {
    addressForType.erase(pos, 2);
  }
  // insert types at the beginning 
  addressForType.insert(0, "types/");
  if (comm::datalayer::STATUS_FAILED(m_dlProvider->registerType(addressForType, tmpBfbsFile)))
  {
    return false;
  }
  return true;
}

void ProviderMgr::registerScopes()
{
  if (!m_dlProvider)
  {
    APR_TRACE_ERROR("Provider not available");
    return;
  }

  comm::datalayer::ScopeT scope1;
  comm::datalayer::ScopeT scope2;
  comm::datalayer::ScopeT scope3;
  comm::datalayer::ScopesT scopes;

  // Define scope motion.view
  scope1.identifier = "motion.view";
  scope1.name = "Motion view";
  scope1.description = "Read access to axisprofile related state and configuration datalayer nodes";
  scope1.permissionsR   = {"axisprofile/system/boot-state",
                           "axisprofile/system/boot-timeout",
                           "axisprofile/system/opstate/actual"
                           "axisprofile/profiles/*/state/**",
                           "axisprofile/profiles/*/cfg/**"};
  scope1.permissionsRW  = {};
  scope1.permissionsX   = {};
  scope1.permissionsRWX = {};

  scopes.scopes.push_back(std::make_unique<comm::datalayer::ScopeT>(scope1));

  // Define scope motion.operate
  scope2.identifier = "motion.operate";
  scope2.name = "Motion operate";
  scope2.description = "Read access to axisprofile related state and configuration datalayer nodes, execute axisprofile commands with current configuration";
  scope2.permissionsR   = {"axisprofile/system/boot-state",
                           "axisprofile/system/boot-timeout",
                           "axisprofile/system/opstate/actual"
                           "axisprofile/profiles/*/state/**",
                           "axisprofile/profiles/*/cfg/**"};
  scope2.permissionsRW  = {};
  scope2.permissionsX   = {};
  scope2.permissionsRWX = {"axisprofile/system/opstate/cmd/**"};

  scopes.scopes.push_back(std::make_unique<comm::datalayer::ScopeT>(scope2));

  // Define scope motion.manage
  scope3.identifier = "motion.manage";
  scope3.name = "Motion manage";
  scope3.description = "Full access for axisprofile: Read and write access to all axisprofile related datalayer nodes, execute axisprofile commands";
  scope3.permissionsR   = {};
  scope3.permissionsRW  = {};
  scope3.permissionsX   = {};
  scope3.permissionsRWX = {"axisprofile/**"};

  scopes.scopes.push_back(std::make_unique<comm::datalayer::ScopeT>(scope3));

  // Create a flatbuffer with all motion scopes data
  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(comm::datalayer::Scopes::Pack(builder, &scopes));
  comm::datalayer::Variant scopesData;
  scopesData.copyFlatbuffers(builder);

  // Register all motion scopes
  auto result = m_dlProvider->registerType("scopes/motion", scopesData);
  if (comm::datalayer::STATUS_FAILED(result))
  {
    APR_TRACE_ERROR("Creation of scopes at datalayer/security/scopes failed : '%s'", result.toString());
  }
  else
  {
    APR_TRACE_MESSAGE("Creation of scopes at datalayer/security/scopes succeeded");
  }
}

void ProviderMgr::unregisterScopes()
{
  if (!m_dlProvider)
  {
    APR_TRACE_ERROR("Provider not available");
    return;
  }

  auto result = m_dlProvider->unregisterType("scopes/motion");
  if (comm::datalayer::STATUS_FAILED(result))
  {
    APR_TRACE_ERROR("Deletion of scopes failed: '%s'", result.toString());
  }
  else
  {
    APR_TRACE_MESSAGE("Deletion of scopes succeeded");
  }
}
