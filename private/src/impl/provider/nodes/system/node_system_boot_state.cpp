// ---------------------------------------------------------------------------
//
//! @file       node_system_boot_state.cpp
//!
//! @author     DC-AE/ENG6-CN
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2020
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and defines the nodes for 
//! the boot state of the axisprofile.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_system_boot_state.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "component/statemachine/i_state_machine.h"
#include "component/i_axisprofile_component.h"
#include "provider/provider_helper.h"
#include "provider/provider_mgr.h"
#include "comm/axisprofile/datatypes/aprBootState_generated.h"
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
NodeSystemBootState::NodeSystemBootState(ProviderMgr* mgr) : NodeBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "aprBootState.bfbs";
  this->m_metaDataInfo.m_description = "current booting state";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ;
}

void NodeSystemBootState::onRead(const std::string & address, const comm::datalayer::Variant * data, const ResponseCallback & callback)
{
  comm::datalayer::Variant retVariant;
  comm::datalayer::DlResult dlResult;

  // get object data
  const IBootMgr* bootMgr = m_manager->getAxisProfileCmp()->getBootManager();
  if (!bootMgr)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_OPERATION_MODE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_CREATE_PROFILE_FAILED, "boot mgr is not yet created");
    return;
  }
  // return variant
  dlResult = createFB(address, retVariant, data, bootMgr);
  callback(dlResult, &retVariant);
}

void NodeSystemBootState::onMetadata(const std::string &address, const ResponseCallback &callback)
{
  std::string name;
  ProviderHelper::getProfileNameFromAddress(address, name);
  std::string typeString;
  if ( !createTypeString(address, name, typeString, callback) )
  {
    return;
  }
  ProviderHelper::createMetaData(typeString, this->m_metaDataInfo, callback);
}

comm::datalayer::DlResult NodeSystemBootState::createFB( const std::string& /*address*/, comm::datalayer::Variant &outFb, const comm::datalayer::Variant* inFbVar, const IBootMgr* bootMgr)
{
  // flatbuffer builder variables
  flatbuffers::FlatBufferBuilder builder;

  // get all diagnosis data
  std::uint32_t actStep = {};
  std::uint32_t maxSteps = {};
  std::string text = {};
  bootMgr->getBootingState(actStep, maxSteps, text);

  // convert string
  auto textFB = builder.CreateString( text );

  // verify input flatbuffer to get info what data to return
  if ( inFbVar )
  {
    if ( STATUS_FAILED( inFbVar->verifyFlatbuffers( fbtypes::VerifyAPRBootStateBuffer ) ) )
    {
      // wrong input flatbuffer provided - return error
      return comm::datalayer::DlResult::DL_TYPE_MISMATCH;
    }
    else
    {
      // correct input flatbuffer provided - create flatbuffer accordingly
      // IMPORTANT: Use inverted fbs sequence - for testing (binary equality)
      auto inFb = fbtypes::GetAPRBootState( inFbVar->getData( ) );
      fbtypes::APRBootStateBuilder fbBuilder( builder );

      if (flatbuffers::IsFieldPresent(inFb, fbtypes::APRBootState::VT_MAXSTEPS))
      {
        fbBuilder.add_maxSteps(maxSteps);
      }
      if (flatbuffers::IsFieldPresent(inFb, fbtypes::APRBootState::VT_ACTSTEP))
      {
        fbBuilder.add_actStep(actStep);
      }
      if (flatbuffers::IsFieldPresent(inFb, fbtypes::APRBootState::VT_TEXT))
      {
        fbBuilder.add_text(textFB);
      }
      // build flatbuffer
      auto finalFB = fbBuilder.Finish( );
      builder.Finish( finalFB );
    }
  }
  else
  {
    // no input flatbuffer data provided -> create complete fb
    auto fb = fbtypes::CreateAPRBootState(builder, textFB, actStep, maxSteps);
    FinishAPRBootStateBuffer( builder, fb );
  }

  // return fb
  outFb.copyFlatbuffers( builder );
  return comm::datalayer::DlResult::DL_OK;
}

