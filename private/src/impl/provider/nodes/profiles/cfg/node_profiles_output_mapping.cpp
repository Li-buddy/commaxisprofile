// ---------------------------------------------------------------------------
//
//! @file       node_profiles_output_mapping.cpp
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
//! for the output mapping configuration.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_output_mapping.h"
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


NodeProfilesOutputMapping::NodeProfilesOutputMapping(ProviderMgr* mgr): NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "valueMapping.bfbs";
  this->m_metaDataInfo.m_description = "output value mapping";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesOutputMapping::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
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

  std::vector<OutputMappingEntry> entries;
  auto variantData = fbtypes::GetvalueMapping(data->getData());
  if (aprStatusFailed(extractFlatbufferData(variantData, entries)))
  {
    return;
  }

  for (auto entry : entries)
  {
    if (aprStatusFailed(profile->getMappingHandler()->configureOutputValue(entry.m_id, entry.m_uri.c_str())))
    {
      ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_CONFIG_MAP_SINGLE_OUTPUT, "configure OutputValue ID:%d, URI:%s failed", entry.m_id, entry.m_uri.c_str());
      return;
    }
  }

  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

void NodeProfilesOutputMapping::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // prepare the available output mapping items
  AxisProfileCategory category = profile->getPropertyInterface()->getProfileCategory();
  std::vector<OutputMappingValueID> vecAvailableOutputs;
  CategoryMappingHelper::getAvailableOutputMappingItems(category, vecAvailableOutputs);
  std::vector<OutputMappingEntry> cfgedOutputMapping = profile->getMappingHandler()->getConfiguredOutputs();
  if (cfgedOutputMapping.empty())
  {
    cfgedOutputMapping = profile->getMappingHandler()->getAvailableOutputs();
  }

  // only filter for switching
  if (profile->getPropertyInterface()->getProfileCategory() == AxisProfileCategory::CATEGORY_ENCODER)
  {
    // erase unavailable mapping items -> erase the data between new end and original end
    cfgedOutputMapping.erase(std::remove_if(cfgedOutputMapping.begin(), cfgedOutputMapping.end(), [&vecAvailableOutputs] (OutputMappingEntry& entry)
                                            {
                                              return std::find(vecAvailableOutputs.begin(), vecAvailableOutputs.end(), entry.m_id) == vecAvailableOutputs.end();
                                            }),
                             cfgedOutputMapping.end());
  }
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto fbOffset = generateFlatbufferData(builder, cfgedOutputMapping);
  builder.Finish(fbOffset);

  comm::datalayer::Variant retVariant;
  retVariant.copyFlatbuffers(builder);
  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesOutputMapping::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

comm::axisprofile::AxisProfileResult NodeProfilesOutputMapping::extractFlatbufferData(const comm::axisprofile::fbtypes::valueMapping* data, std::vector< comm::axisprofile::OutputMappingEntry >& mapping)
{
  if (!flatbuffers::IsFieldPresent(data, fbtypes::valueMapping::VT_MAPPING))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  auto vecData = data->mapping();
  for (auto elem = vecData->begin(); elem != vecData->end(); ++elem)
  {
    mapping.emplace_back(stringToOutputMappingValueID(flatbuffers::GetString(elem->valueID()).c_str()),
                         flatbuffers::GetString(elem->datalayerURI()),
                         stringToMappingGroup(fbtypes::EnumNamevariableType(elem->profileVar())),
                         elem->required());
  }

  return AxisProfileResult::STS_OK;
}

flatbuffers::Offset<comm::axisprofile::fbtypes::valueMapping> NodeProfilesOutputMapping::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, const std::vector<comm::axisprofile::OutputMappingEntry>& data)
{
  std::vector<flatbuffers::Offset<fbtypes::mappingEntry>> outputMapping;

  for (const auto& mappingEntry : data)
  {
    auto mappingStringId = fbb.CreateString(outputMappingValueIdToString(mappingEntry.m_id));
    auto mappingStringUri = fbb.CreateString(mappingEntry.m_uri);

    fbtypes::mappingEntryBuilder entryBuilder(fbb);
    entryBuilder.add_valueID(mappingStringId);
    entryBuilder.add_datalayerURI(mappingStringUri);
    entryBuilder.add_profileVar(static_cast<fbtypes::variableType>(mappingEntry.m_group));
    entryBuilder.add_required(mappingEntry.m_bRequired);
    auto entry = entryBuilder.Finish();
    outputMapping.emplace_back(entry);
  }

  auto sortedMapping = fbb.CreateVectorOfSortedTables(&outputMapping);
  fbtypes::valueMappingBuilder mappingBuilder(fbb);
  mappingBuilder.add_mapping(sortedMapping);

  return mappingBuilder.Finish();
}
