/// @file
///
/// @brief This file is part of the axisprofile library and implements the 
/// node for the datalayer.
///
/// SPDX-FileCopyrightText: Copyright (c) 20182025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_base.h"
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

NodeBase::NodeBase(ProviderMgr *mgr) : m_manager(mgr)
{
}

void NodeBase::onCreate(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback &callback)
{
  ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, "not implemented");
}

void NodeBase::onRemove(const std::string &/*address*/, const ResponseCallback &callback)
{
  ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, "not implemented");
}

void NodeBase::onBrowse(const std::string &/*address*/, const ResponseCallback &callback)
{
  ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, "not implemented");
}

void NodeBase::onRead(const std::string & /*address*/, const comm::datalayer::Variant * /*data*/, const ResponseCallback & callback)
{
  ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, "not implemented");
}

void NodeBase::onWrite(const std::string &/*address*/, const comm::datalayer::Variant* /*data*/, const ResponseCallback &callback)
{
  ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, "not implemented");
}

void NodeBase::onMetadata(const std::string &/*address*/, const ResponseCallback &callback)
{
  ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, "not implemented");
}

const char* NodeBase::getBfbsName()
{
  return m_metaDataInfo.m_bfbsPath;
}

const char* NodeBase::getDescription()
{
  return m_metaDataInfo.m_bfbsPath;
}

const char* NodeBase::getDescriptionUrl()
{
  return m_metaDataInfo.m_descriptionUrl;
}

bool NodeBase::createTypeString(const std::string &address, const std::string& profileName, std::string &typeString, const ResponseCallback &/*callback*/)
{
  typeString = address;

  if (!profileName.empty())
  {
    size_t pos;
    std::string start{ NodeTree.APR_PROFILES + "/" };
    while ((pos = typeString.find(start + profileName)) != std::string::npos)
    {
      typeString.erase(start.length() + pos, profileName.length() + 1);
    }
  }

  // insert types at the beginning 
  typeString.insert(0, "types/");

  // delete last slash, since types get registered without slashes at the end
  if ( typeString.back() == '/' )
  {
    typeString.pop_back();
  }

  return true;
}

bool NodeBase::createBasicTypeString(const std::string& profileName, comm::datalayer::VariantType variantTypeID, std::string& typeString, const ResponseCallback& callback, bool /*requireIF*/)
{ 
  if (!profileName.empty())
  {
    size_t pos;
    std::string start{ NodeTree.APR_PROFILES + "/" };
    while ((pos = typeString.find(start + profileName)) != std::string::npos)
    {
      typeString.erase(start.length() + pos, profileName.length() + 1);
    }
  }

  // insert types at the beginning 
  typeString.insert(0, "types/");

  // delete last slash, since types get registered without slashes at the end
  if (typeString.back() == '/')
  {
    typeString.pop_back();
  }

  // get type string from variant type ID
  switch (variantTypeID)
  {
  case comm::datalayer::VariantType::BOOL8:
    typeString = comm::datalayer::TYPE_DL_BOOL8;
    break;
  case comm::datalayer::VariantType::INT8:
    typeString = comm::datalayer::TYPE_DL_INT8;
    break;
  case comm::datalayer::VariantType::UINT8:
    typeString = comm::datalayer::TYPE_DL_UINT8;
    break;
  case comm::datalayer::VariantType::INT16:
    typeString = comm::datalayer::TYPE_DL_INT16;
    break;
  case comm::datalayer::VariantType::UINT16:
    typeString = comm::datalayer::TYPE_DL_UINT16;
    break;
  case comm::datalayer::VariantType::INT32:
    typeString = comm::datalayer::TYPE_DL_INT32;
    break;
  case comm::datalayer::VariantType::UINT32:
    typeString = comm::datalayer::TYPE_DL_UINT32;
    break;
  case comm::datalayer::VariantType::INT64:
    typeString = comm::datalayer::TYPE_DL_INT64;
    break;
  case comm::datalayer::VariantType::UINT64:
    typeString = comm::datalayer::TYPE_DL_UINT64;
    break;
  case comm::datalayer::VariantType::FLOAT32:
    typeString = comm::datalayer::TYPE_DL_FLOAT32;
    break;
  case comm::datalayer::VariantType::FLOAT64:
    typeString = comm::datalayer::TYPE_DL_FLOAT64;
    break;
  case comm::datalayer::VariantType::STRING:
    typeString = comm::datalayer::TYPE_DL_STRING;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_BOOL8:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_BOOL8;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_INT8:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_INT8;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_UINT8:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_UINT8;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_INT16:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_INT16;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_UINT16:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_UINT16;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_INT32:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_INT32;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_UINT32:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_UINT32;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_INT64:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_INT64;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_UINT64:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_UINT64;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_FLOAT32:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_FLOAT32;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_FLOAT64:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_FLOAT64;
    break;
  case comm::datalayer::VariantType::ARRAY_OF_STRING:
    typeString = comm::datalayer::TYPE_DL_ARRAY_OF_STRING;
    break;
  case comm::datalayer::VariantType::RAW:
    typeString = comm::datalayer::TYPE_DL_RAW;
    break;
  default:
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_TYPE_MISMATCH, MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_DL_INVALID_VALUE, "no variant type");
    return false;
  }
  return true;
}
