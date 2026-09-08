// ---------------------------------------------------------------------------
//
//! @file       node_system_cmd_statemachine.cpp
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
//! This file is part of the axisprofile library and defines the nodes for 
//! the commanding of the axisprofile state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_system_cmd_statemachine.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "component/statemachine/i_state_machine.h"
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


NodeSystemCmdStateMachine::NodeSystemCmdStateMachine(ProviderMgr* mgr): NodeBase(mgr)
{
  this->m_metaDataInfo.m_description = "command to switch state machine, input parameter: Booting or Configuration";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Program;
  this->m_metaDataInfo.nodeActions = node_actions::CREATE;
}

void NodeSystemCmdStateMachine::onCreate(const std::string& /*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  if (!data)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE, "");
    return;
  }

  if (data->getType() != comm::datalayer::VariantType::STRING || (strcmp(*data, "Booting") != 0 && strcmp(*data, "Configuration") != 0))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH,
                                        MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_DL_INVALID_VALUE,
                                        "input parameter should be: Booting or Configuration");
    return;
  }

  const char* state = *data;
  AxisProfileResult  requestResult = m_manager->getAxisProfileCmp()->requestAxisProfileState(state);
  if (aprStatusFailed(requestResult))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_SUBMODULE_FAILURE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_STM_REQUEST_FAILED, "Request profile state failed");
    return;
  }
  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeSystemCmdStateMachine::onBrowse(const std::string& address, const ResponseCallback& callback)
{
  std::vector<std::string> stateNames;
  std::string lastSubNode {};
  ProviderHelper::getLastSubNodeOfAddress(address, lastSubNode);

  // "../cmd" or "../cmd/*" or "../cmd/" ?
  if ((lastSubNode.compare("cmd") == 0) || (lastSubNode.compare("*") == 0) || (lastSubNode.empty()))
  {
    stateNames = {"Booting", "Configuration"};
  }

  comm::datalayer::Variant data;
  data.setValue(stateNames);

  callback(comm::datalayer::DlResult::DL_OK, &data);
}

void NodeSystemCmdStateMachine::onMetadata(const std::string&/*address*/, const ResponseCallback& callback)
{
  ProviderHelper::createMetaData(comm::datalayer::TYPE_DL_STRING, this->m_metaDataInfo, callback);
}

