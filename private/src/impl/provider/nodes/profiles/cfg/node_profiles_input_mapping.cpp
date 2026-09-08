// ---------------------------------------------------------------------------
//
//! @file       node_profiles_input_mapping.cpp
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
//! for the input mapping configuration.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_input_mapping.h"
#include "profiles/i_axisprofile_internal.h"
#include "profiles/category/category_mapping_helper.h"

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

NodeProfilesInputMapping::NodeProfilesInputMapping(ProviderMgr* mgr): NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "valueMapping.bfbs";
  this->m_metaDataInfo.m_description = "input value mapping";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesInputMapping::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::FLATBUFFERS, callback))
  {
    return;
  }
  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifyvalueMappingBuffer(verifier))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return;
  }

  std::vector<InputMappingEntry> entries;
  auto variantData = fbtypes::GetvalueMapping(data->getData());
  if (aprStatusFailed(extractFlatbufferData(variantData, entries)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "extract flatbuffer failed");
    return;
  }

  for (auto entry : entries)
  {
    if (aprStatusFailed(profile->getMappingHandler()->configureInputValue(entry.m_id, entry.m_uri.c_str())))
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_MAP_SINGLE_INPUT, "configure InputValue ID:%d, URI:%s failed", entry.m_id, entry.m_uri.c_str());
      return;
    }
  }

  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeProfilesInputMapping::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // prepare the available input mapping items
  AxisProfileCategory category = profile->getPropertyInterface()->getProfileCategory();
  std::vector<InputMappingValueID> vecAvailableInputs;
  CategoryMappingHelper::getAvailableInputMappingItems(category, vecAvailableInputs);
  std::vector<InputMappingEntry> cfgedInputMapping = profile->getMappingHandler()->getConfiguredInputs();
  if (cfgedInputMapping.empty())
  {
    cfgedInputMapping = profile->getMappingHandler()->getAvailableInputs();
  }
  if (profile->getPropertyInterface()->getProfileCategory() == AxisProfileCategory::CATEGORY_ENCODER)
  {
    // erase unavailable mapping items -> erase the data between new end and original end
    cfgedInputMapping.erase(std::remove_if(cfgedInputMapping.begin(), cfgedInputMapping.end(), [&vecAvailableInputs] (InputMappingEntry& entry)
                                           {
                                             return std::find(vecAvailableInputs.begin(), vecAvailableInputs.end(), entry.m_id) == vecAvailableInputs.end();
                                           }),
                            cfgedInputMapping.end());
  }

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto fbOffset = generateFlatbufferData(builder, cfgedInputMapping);
  builder.Finish(fbOffset);

  comm::datalayer::Variant retVariant;
  retVariant.copyFlatbuffers(builder);
  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesInputMapping::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

comm::axisprofile::AxisProfileResult NodeProfilesInputMapping::extractFlatbufferData(const comm::axisprofile::fbtypes::valueMapping* data, std::vector< comm::axisprofile::InputMappingEntry >& mapping)
{
  if (!flatbuffers::IsFieldPresent(data, fbtypes::valueMapping::VT_MAPPING))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  auto vecData = data->mapping();
  for (auto elem = vecData->begin(); elem != vecData->end(); ++elem)
  {
    mapping.emplace_back(stringToInputMappingValueID(flatbuffers::GetString(elem->valueID()).c_str()),
                         flatbuffers::GetString(elem->datalayerURI()),
                         stringToMappingGroup(fbtypes::EnumNamevariableType(elem->profileVar())),
                         elem->required());
  }

  return AxisProfileResult::STS_OK;
}

flatbuffers::Offset<comm::axisprofile::fbtypes::valueMapping> NodeProfilesInputMapping::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, const std::vector<comm::axisprofile::InputMappingEntry>& data)
{
  std::vector<flatbuffers::Offset<fbtypes::mappingEntry>> inputMapping;

  for (const auto& mappingEntry : data)
  {
    auto mappingStringId = fbb.CreateString(inputMappingValueIdToString(mappingEntry.m_id));
    auto mappingStringUri = fbb.CreateString(mappingEntry.m_uri);
    fbtypes::mappingEntryBuilder entryBuilder(fbb);
    entryBuilder.add_valueID(mappingStringId);
    entryBuilder.add_datalayerURI(mappingStringUri);
    entryBuilder.add_profileVar(static_cast<fbtypes::variableType>(mappingEntry.m_group));
    entryBuilder.add_required(mappingEntry.m_bRequired);
    auto entry = entryBuilder.Finish();
    inputMapping.push_back(entry);
  }

  auto sortedMapping = fbb.CreateVectorOfSortedTables(&inputMapping);
  fbtypes::valueMappingBuilder mappingBuilder(fbb);
  mappingBuilder.add_mapping(sortedMapping);
  return mappingBuilder.Finish();
}
