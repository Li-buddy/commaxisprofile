/// @file
///
/// @brief This source file is part of the axisprofile library and implement the node information for profile category node.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_category.h"
#include "profiles/i_axisprofile_internal.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
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

NodeProfilesCategory::NodeProfilesCategory(ProviderMgr* mgr): NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_bfbsPath = "profileCategory.bfbs";
  this->m_metaDataInfo.m_description = "profile category";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesCategory::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Check the variant type
  AxisProfileCategory profileCategory {};
  if (aprStatusFailed(extractFlatbufferData(data, profileCategory, callback)))
  {
    return;
  }

  // Get profile
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Finish
  AxisProfileResult aprResult = profile->setProfileCategory(profileCategory);

  if (aprStatusSucceeded(aprResult))
  {
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    // need new diagnosis code
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_PROFILE_CATEGORY, "set profie category %d failed", profileCategory);
  }
}

void NodeProfilesCategory::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Get configured output URI
  AxisProfileCategory profileCategory = profile->getPropertyInterface()->getProfileCategory();

  comm::datalayer::Variant retVariant;
  // Build basic profile flatbuffer
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto typeOffset = generateFlatbufferData(builder, profileCategory);
  // Finish
  builder.Finish(typeOffset);
  retVariant.copyFlatbuffers(builder);

  callback(comm::datalayer::DlResult::DL_OK, &retVariant);
}

void NodeProfilesCategory::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}

flatbuffers::Offset<comm::axisprofile::fbtypes::ProfileCategory> NodeProfilesCategory::generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, AxisProfileCategory profileCategory)
{
  //get type offset data
  fbtypes::ProfileCategoryBuilder profileCategoryBuilder(fbb);
  profileCategoryBuilder.add_category(fbtypes::EnumValuesCategoryOfProfile()[static_cast<std::uint32_t>(profileCategory)-1]);
  return profileCategoryBuilder.Finish();
}

comm::axisprofile::AxisProfileResult NodeProfilesCategory::extractFlatbufferData(const comm::datalayer::Variant* data, AxisProfileCategory& profileCategory, const ResponseCallback& callback)
{
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::FLATBUFFERS, callback))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  // Verify the flatbuffer
  flatbuffers::Verifier verifier(data->getData(), data->getSize());
  if (!fbtypes::VerifyProfileCategoryBuffer(verifier))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check flatbuffer failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  //get fbs profile
  auto variantData = fbtypes::GetProfileCategory(data->getData());

  //check field present
  if (aprStatusFailed(extractFlatbufferData(variantData, profileCategory)))
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_FLATBUFFER, "check Field Present failed");
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }

  return AxisProfileResult::STS_OK;
}


AxisProfileResult NodeProfilesCategory::extractFlatbufferData(const fbtypes::ProfileCategory* data, AxisProfileCategory& profileCategory)
{
  //check field present
  if (!flatbuffers::IsFieldPresent(data, fbtypes::ProfileCategory::VT_CATEGORY))
  {
    return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  }
  profileCategory = stringToAxisProfileCategory(fbtypes::EnumNameCategoryOfProfile(data->category()));
  return AxisProfileResult::STS_OK;
}
