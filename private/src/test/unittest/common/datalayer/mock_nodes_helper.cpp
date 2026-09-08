/// @file
///
/// @brief This file is part of the axisprofile library and implements helper class of datalayer mock nodes for function test.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_nodes_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace test;

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

MockNodesHelper::MockNodesHelper(comm::datalayer::IDataLayerFactory* datalayerFactory)
  : m_datalayerFactory(datalayerFactory)
{
}

MockNodesHelper::~MockNodesHelper()
{
  deInit();
}

HRESULT MockNodesHelper::init()
{
  m_provider = m_datalayerFactory->createProvider();
  m_provider->start();

  // create correct nodes
  for (const auto & it : MOCK_URLS)
  {
    m_nodes.push_back({it, new MockHelperNodeSimpleType<std::int32_t>});
    comm::datalayer::DlResult dlRest = m_provider->registerNode(m_nodes.back().m_address, m_nodes.back().m_node);
    if (dlRest != comm::datalayer::DlResult::DL_OK)
    {
      return E_FAIL;
    }
  }

  m_nodes.push_back({TEST_MOCK_ERROR_URL, new MockHelperNodeBase});
  comm::datalayer::DlResult dlRest = m_provider->registerNode(m_nodes.back().m_address, m_nodes.back().m_node);
  if (dlRest != comm::datalayer::DlResult::DL_OK)
  {
    return E_FAIL;
  }
  return S_OK;
}

HRESULT MockNodesHelper::deInit()
{
  for (auto& it : m_nodes)
  {
    comm::datalayer::DlResult dlRest = m_provider->unregisterNode(it.m_address);
    if (dlRest != comm::datalayer::DlResult::DL_OK)
    {
      return E_FAIL;
    }
    if(it.m_node) { delete it.m_node; }
  }

  m_nodes.clear();

  if (m_provider)
  {
    delete m_provider;
    m_provider = nullptr;
  }
  return S_OK;
}
