// ---------------------------------------------------------------------------
//
//! @file       node_system_act_statemachine.cpp
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
//! the actual value of the axisprofile state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_system_act_statemachine.h"
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


NodeSystemActStateMachine::NodeSystemActStateMachine(ProviderMgr *mgr) : NodeBase(mgr)
{
  this->m_metaDataInfo.m_description = "actual value of the axisprofile state machine";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ;
}

void NodeSystemActStateMachine::onRead(const std::string & /*address*/, const comm::datalayer::Variant * /*data*/, const ResponseCallback & callback)
{

  const char *state = m_manager->getAxisProfileCmp()->getAxisProfileState();
  if ( !state )
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_OPERATION_MODE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_STM_GET_STATE_NAME, "");
    return;
  }

  comm::datalayer::Variant result;
  result.setValue(state);

  callback(comm::datalayer::DlResult::DL_OK, &result);
}

void NodeSystemActStateMachine::onMetadata(const std::string &/*address*/, const ResponseCallback &callback)
{
  ProviderHelper::createMetaData(comm::datalayer::TYPE_DL_STRING, this->m_metaDataInfo, callback);
}
