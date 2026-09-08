/// @file
///
/// @brief This file is part of the axisprofile library and implements the a helper
/// class for datalayer provider functionalities.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/datalayer/diagnosis_generated.h"
#include "comm/datalayer/metadata_helper.h"
#include "common/string_helper.h"
#include "provider_helper.h"
#include <cctype>
#include <cstdarg>

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

bool ProviderHelper::getProfileNameFromAddress(const std::string &address, std::string &profileName)
{
  if (address.find("axisprofile/profiles/") != std::string::npos)
  {
    profileName = address.substr(std::string("axisprofile/profiles/").length());
    size_t idx = profileName.find('/');
    if (idx != std::string::npos)
    {
      profileName.resize(idx);
    }
  }
  else if (address.find("axisprofile/system/") != std::string::npos)
  {
    profileName = address.substr(std::string("axisprofile/system/").length());
    size_t idx = profileName.find('/');
    if (idx != std::string::npos)
    {
      profileName.resize(idx);
    }
  }

  return !profileName.empty();
}

void ProviderHelper::createMetaData(const std::string& typeString, const MetaDataInfo& metaData, const comm::datalayer::IClient::ResponseCallback& callback, const std::string& unit)
{
  flatbuffers::FlatBufferBuilder builder;

  auto metaOperations = comm::datalayer::CreateAllowedOperations(builder, metaData.nodeActions & node_actions::READ, metaData.nodeActions & node_actions::WRITE,
                                                                  metaData.nodeActions & node_actions::CREATE, metaData.nodeActions & node_actions::REMOVE);

  if (!metaData.m_description || !metaData.m_descriptionUrl)
  {
    callback(comm::datalayer::DlResult::DL_INVALID_HANDLE, nullptr);
    return;
  }

  auto metaDescription = builder.CreateString(metaData.m_description);
  auto metaDescriptionUrl = builder.CreateString(metaData.m_descriptionUrl);
  flatbuffers::Offset<flatbuffers::String> unitFB = 0;
  if (!unit.empty())
  {
    unitFB = builder.CreateString(unit);
  }

  flatbuffers::Offset<comm::datalayer::Reference> vecReferences[] =   // TODO: only give Type when matching operation (nodeAction) is performed
  {
    comm::datalayer::CreateReferenceDirect(builder, "readType", typeString.c_str()),
    comm::datalayer::CreateReferenceDirect(builder, "writeType", typeString.c_str()),
    comm::datalayer::CreateReferenceDirect(builder, "createType", typeString.c_str()),
  };
  auto metaReferences = builder.CreateVectorOfSortedTables(vecReferences, 3);

  comm::datalayer::MetadataBuilder metadata(builder);
  metadata.add_operations(metaOperations);
  metadata.add_description(metaDescription);
  metadata.add_descriptionUrl(metaDescriptionUrl);
  metadata.add_references(metaReferences);
  metadata.add_nodeClass(metaData.m_nodeClass);
  if (!unit.empty())
  {
    metadata.add_unit(unitFB);
  }
  auto metaFinished = metadata.Finish();
  builder.Finish(metaFinished);

  comm::datalayer::Variant variant;
  variant.shareFlatbuffers(builder);
  callback(comm::datalayer::DlResult::DL_OK, &variant);
}

bool ProviderHelper::checkVariantType(const comm::datalayer::Variant* data, comm::datalayer::VariantType expectedType, const comm::datalayer::IClient::ResponseCallback &callback)
{
  if (!data || (data->getType() != expectedType))
  {
    comm::datalayer::Variant result;
    result.setValue(static_cast<std::uint32_t>(comm::axisprofile::AxisProfileResult::ERR_DATALAYER_WRONG_TYPE));
    callback(comm::datalayer::DlResult::DL_TYPE_MISMATCH, &result);
    return false;
  }

  return true;
}

std::string ProviderHelper::getTypeString(const std::string& flatbufferNodeAddress)
{
  const std::string substring = "/*";
  size_t pos = flatbufferNodeAddress.find(substring);
  if (pos == std::string::npos)
  {
    return std::string();
  }

  std::string buffer(flatbufferNodeAddress);
  buffer.erase(pos, substring.length());
  buffer = "types/" + buffer;

  return buffer;
}

bool ProviderHelper::checkProfileName(const std::string& profileName, std::string& dynErrTxt)
{
  if (profileName.empty())
  {
    dynErrTxt = "profile name should not empty";
    return false;
  }

  if (profileName.size() >= MAX_PROFILE_NAME_LENGTH)   // MAX_PROFILE_NAME_LENGTH includes the terminating null character
  {
    dynErrTxt = "profile " + profileName + ", name should less than " + std::to_string(MAX_PROFILE_NAME_LENGTH) + "characters";
    return false;
  }

  // check if name begins with an alphabetic character or an Unicode character
  const char* profileNameCString = profileName.c_str();
  if ((static_cast<std::uint8_t>(profileNameCString[0]) < 0xC0) && !std::isalpha(profileNameCString[0]))
  {
    dynErrTxt = "profile" + profileName + ", name should begins with alphabetic character or Unicode character";
    return false;
  }

  // check for white spaces or new lines in name
  const char *profileNameWithoutFirstCharacter = profileNameCString + 1;
  while (*profileNameWithoutFirstCharacter != '\0')
  {
    if ((static_cast<std::uint8_t>(*profileNameWithoutFirstCharacter) < 0x80) && !std::isalnum(*profileNameWithoutFirstCharacter) && (*profileNameWithoutFirstCharacter != '_'))
    {
      dynErrTxt = "profile " + profileName + ", name should be alphanumerics or _";
      return false;
    }
    profileNameWithoutFirstCharacter++;
  }
  return true;
}

bool ProviderHelper::getLastSubNodeOfAddress(const std::string & address, std::string & result)
{
  size_t pos = address.rfind("/");
  if (pos == std::string::npos)
  {
    return false;
  }

  result = address.substr(pos + 1);
  return true;
}

bool ProviderHelper::extractWildcardContentFromAddress(const std::string & address, const std::string & priorSearchNode, std::string & result)
{
  std::string searchString = priorSearchNode;
  if (priorSearchNode.front() != '/')
  {
    searchString = '/' + searchString;
  }
  if (priorSearchNode.back() != '/')
  {
    searchString = searchString + '/';
  }

  size_t startPos = address.find(searchString);
  if (startPos == std::string::npos)
  {
    return false;
  }
  startPos = startPos + searchString.length();

  size_t endPos = address.find('/', startPos);
  if (endPos == std::string::npos)
  {
    result = address.substr(startPos);
  }
  else
  {
    result = address.substr(startPos, (endPos - startPos));
  }

  return true;
}

bool ProviderHelper::sendDlErrorResponse(const comm::datalayer::IClient::ResponseCallback &callback, comm::datalayer::DlResult result, const std::uint32_t maindiag, const std::uint32_t detaileddiag, const char * dynmicdescription, ...)
{
  comm::datalayer::Variant data;
  // flatbuffer builder variables
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  char addInfo[256];
  va_list args;
  va_start(args, dynmicdescription);
  vsnprintf(addInfo, 256, dynmicdescription, args);
  va_end(args);
  stringCatS(addInfo, 256, " ,DLResult: ", 256 - strlen(addInfo));
  stringCatS(addInfo, 256, result.toString(), 256 - strlen(addInfo));
  auto addInfoFB = builder.CreateString(addInfo);

  comm::datalayer::DiagnosisBuilder fbBuilder(builder);
  fbBuilder.add_mainDiagnosisCode(maindiag);
  fbBuilder.add_detailedDiagnosisCode(detaileddiag);
  fbBuilder.add_dynamicDescription(addInfoFB);

  auto finalFB = fbBuilder.Finish();
  builder.Finish(finalFB);

  data.copyFlatbuffers(builder);
  callback(result, &data);

  return true;
}

bool ProviderHelper::getLastNodeNameFromAddress(const std::string& address, std::string& nodeName)
{
  size_t pos = address.rfind("/");
  nodeName = address.substr((pos + size_t(1)));

  return !nodeName.empty();
}

bool ProviderHelper::replaceFirstWideCharInAddress(const std::string& address, const std::string& replaceStr, std::string& newAddress)
{
  std::size_t pos = address.find('*');
  if (pos != std::string::npos)
  {
    newAddress = address;
    newAddress.replace(pos, 1, replaceStr);
    return true;
  }
  newAddress = address;
  return false;
}
