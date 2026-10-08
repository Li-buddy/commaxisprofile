// ---------------------------------------------------------------------------
//
//! @file       node_profiles_set_uri.cpp
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
//! This source file is part of the axisprofile library and defines the node
//! to set a configuration URI for a specific profile.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_set_uri.h"
#include "manager/i_axisprofile_manager_internal.h"

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


NodeProfilesSetURI::NodeProfilesSetURI(ProviderMgr* mgr, const BufferOrient operation) : NodeProfilesBase(mgr), m_operation(operation)
{
  this->m_metaDataInfo.m_description = "input uri";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesSetURI::onRead(const std::string & address, const comm::datalayer::Variant* /*data*/, const ResponseCallback & callback)
{
  // Get profile name from address string
  std::string name;
  ProviderHelper::getProfileNameFromAddress(address, name);

  // Get profile manager
  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  // Get the requested input URI
  const char* uri = nullptr;
  switch (m_operation)
  {
  case BufferOrient::INPUT_BUFFER:
    uri = profileManager->getProfileInputBuffer(name.c_str());
    break;
  case BufferOrient::OUTPUT_BUFFER:
    uri = profileManager->getProfileOutputBuffer(name.c_str());
    break;
  }

  if (!uri)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_GET_IN_OUT_BUFFER, "set ProfileInput/Outputbuffer from profile %s failed", name.c_str());
    return;
  }

  // Finish
  comm::datalayer::Variant returnVal;
  returnVal.setValue(uri);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeProfilesSetURI::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile name from address string
  std::string name;
  ProviderHelper::getProfileNameFromAddress(address, name);

  // Get profile manager
  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  // Check and extract the received variant
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::STRING, callback))
  {
    return;
  }

  const char* convData = *data;

  // Write variant data
  AxisProfileResult aprReturn{AxisProfileResult::ERR_NOK};
  switch (m_operation)
  {
  case BufferOrient::INPUT_BUFFER:
    aprReturn = profileManager->setProfileInputBuffer(name.c_str(), convData);
    break;
  case BufferOrient::OUTPUT_BUFFER:
    aprReturn = profileManager->setProfileOutputBuffer(name.c_str(), convData);
    break;
  }

  // Finish
  if (aprStatusSucceeded(aprReturn))
  {
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_SET_IN_OUT_BUFFER, "set ProfileInput/Outputbuffer from profile %s failed", name.c_str());
  }
}

void NodeProfilesSetURI::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}
