// ---------------------------------------------------------------------------
//
//! @file       node_profiles_map_single_output.cpp
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
//! to configure the mapping of a single output value.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_map_single_output.h"
#include "profiles/i_axisprofile_internal.h"
#include "profiles/value_mapping/i_mapping_handler.h"
#include "provider/provider_mgr.h"
#include "provider/provider_helper.h"
#include "profiles/axisprofile_factory.h"
#include <cstring>
#include <string>


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


NodeProfilesMapSingleOutput::NodeProfilesMapSingleOutput(ProviderMgr* mgr): NodeProfilesBase(mgr)
{

}

void NodeProfilesMapSingleOutput::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  std::string nodeName;
  ProviderHelper::getLastNodeNameFromAddress(address, nodeName);

  // Get configured output URI
  const char* outputURI = profile->getMappingHandler()->getConfiguredOutputURI(stringToOutputMappingValueID(nodeName.c_str()));

  // Finish
  comm::datalayer::Variant returnVal;
  outputURI ? returnVal.setValue(outputURI) : returnVal.setValue("");
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);

  AxisProfileType profileType = profile->getPropertyInterface()->getProfileType();
  AxisProfileCategory profileCategory = profile->getPropertyInterface()->getProfileCategory();

  std::map<OutputMappingValueID, MappingCfgInfo> outputMappingCfgInfo;
  AxisProfileFactory::getOutputMappingValueIDList(outputMappingCfgInfo, profileCategory, profileType);

  // configure mapping type
  if (aprStatusFailed(profile->getMappingHandler()->configureOutputCfgInfo(stringToOutputMappingValueID(nodeName.c_str()), outputMappingCfgInfo.at(stringToOutputMappingValueID(nodeName.c_str())))))
  {
    return;
  }
}

void NodeProfilesMapSingleOutput::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Check and extract the received variant
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::STRING, callback))
  {
    return;
  }

  std::string nodeName;
  ProviderHelper::getLastNodeNameFromAddress(address, nodeName);

  const char* convData = *data;

  // Write variant data
  AxisProfileResult aprReturn = profile->getMappingHandler()->configureOutputValue(stringToOutputMappingValueID(nodeName.c_str()), convData);
  // Finish
  if (aprStatusSucceeded(aprReturn))
  {
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_MAP_SINGLE_OUTPUT, "configure single OutputValue failed");
  }
}

void NodeProfilesMapSingleOutput::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  std::string nodeName;
  ProviderHelper::getLastNodeNameFromAddress(address, nodeName);

  m_mappingValue = outputMappingValueIdToDescription(stringToOutputMappingValueID(nodeName.c_str()));
  this->m_metaDataInfo.m_description = m_mappingValue.c_str();
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;

  onMetadataBase(address, callback, this->m_metaDataInfo);
}

void NodeProfilesMapSingleOutput::onBrowse(const std::string& address, const ResponseCallback& callback)
{
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);

  AxisProfileType profileType = profile->getPropertyInterface()->getProfileType();
  AxisProfileCategory profileCategory = profile->getPropertyInterface()->getProfileCategory();

  std::map<OutputMappingValueID, MappingCfgInfo> outputMappingCfgInfo;
  AxisProfileFactory::getOutputMappingValueIDList(outputMappingCfgInfo, profileCategory, profileType);
  std::vector<std::string> allStepsStr {};

  for (auto OutputMappingValueIDSingle : outputMappingCfgInfo)
  {
    allStepsStr.push_back(outputMappingValueIdToString(OutputMappingValueIDSingle.first));
  }

  comm::datalayer::Variant result;
  result.setValue(allStepsStr);
  callback(comm::datalayer::DlResult::DL_OK, &result);
}
