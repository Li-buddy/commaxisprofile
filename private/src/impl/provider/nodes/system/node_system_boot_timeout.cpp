/// @file                node_system_boot_timeout.cpp
///
/// This source file is part of the axisprofile library and implements the node information for max booting timeout.
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_system_boot_timeout.h"
#include "component/i_axisprofile_component.h"
#include "provider/provider_helper.h"
#include "provider/provider_mgr.h"

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


NodeSystemBootTimeout::NodeSystemBootTimeout(ProviderMgr* mgr): NodeBase(mgr)
{
  this->m_metaDataInfo.m_description = "booting timeout in seconds";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ;
}

void NodeSystemBootTimeout::onRead(const std::string& /*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  double bootingTimeout = 0.0;
  if (m_manager->getAxisProfileCmp()->getBootManager())
  {
    bootingTimeout = m_manager->getAxisProfileCmp()->getMaxBootingTimeout();
  }
  else
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TIMEOUT, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_BOOTING_TIMEOUT, "");
    return;
  }

  comm::datalayer::Variant result;
  result.setValue(bootingTimeout);

  callback(comm::datalayer::DlResult::DL_OK, &result);
}

void NodeSystemBootTimeout::onMetadata(const std::string&/*address*/, const ResponseCallback& callback)
{
  ProviderHelper::createMetaData(comm::datalayer::DATALAYER_TYPE_DOUBLE, this->m_metaDataInfo, callback);
}
