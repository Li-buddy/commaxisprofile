// ---------------------------------------------------------------------------
//
//! @file       node_profiles_base.cpp
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
//! This source file is part of the axisprofile library and defines a base
//! class for all profile nodes.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_base.h"
#include "provider/provider_mgr.h"


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


NodeProfilesBase::NodeProfilesBase(ProviderMgr* mgr) : NodeBase(mgr)
{
}

comm::axisprofile::IAxisProfileInternal* NodeProfilesBase::getProfileByAddress(const std::string& address, const ResponseCallback& callback)
{
  // Get profile name from address string
  std::string name;
  ProviderHelper::getProfileNameFromAddress(address, name);

  // Get profile manager
  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return nullptr;
  }

  // Get profile pointer from manager
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(profileManager->getProfileByName(name.c_str()));
  if (!profile)
  {
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_PROFILE_NOT_FOUND, "profile %s not found",name.c_str());
    return nullptr;
  }

  return profile;
}

comm::datalayer::DlResult NodeProfilesBase::getObjectData(const std::string& address, IAxisProfileInternal*& axisprofile, const ResponseCallback& callback)
{
  axisprofile = NodeProfilesBase::getProfileByAddress(address, callback);
  return axisprofile ? comm::datalayer::DlResult::DL_OK : comm::datalayer::DlResult::DL_INVALID_ADDRESS;
}

comm::axisprofile::IAxisProfileManagerInternal* NodeProfilesBase::getProfileManager(const ResponseCallback & callback)
{
  if (!m_manager || !m_manager->getAxisProfileCmp() || !m_manager->getAxisProfileCmp()->getAxisProfileManager())
  {
    comm::datalayer::Variant returnVal;
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_HANDLE, MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_SYSTEM_INCONSISTENT, "profile or profile manager not found");
    return nullptr;
  }

  return m_manager->getAxisProfileCmp()->getAxisProfileManager();
}

void NodeProfilesBase::onMetadata(const std::string &address, const ResponseCallback &callback)
{
  // Default implementation. This is usually used by non-flatbuffer profile nodes that do not override onMetadata().
  // Important: It requires that the address string contains a concrete profile name that exists as a real profile in the system!
  if (!getProfileByAddress(address, callback))
  {
    return;
  }

  callback(comm::datalayer::DlResult::DL_OK, nullptr);
}

comm::axisprofile::AxisProfileResult NodeProfilesBase::onMetadataBase(const std::string &address, const ResponseCallback &callback, const MetaDataInfo& metaData)
{
  std::string name;
  ProviderHelper::getProfileNameFromAddress(address, name);

  if (!name.empty())
  {
    // The address string contains a profile name. Check if it exists in the system.
    if (!getProfileByAddress(address, callback))
    {
      return AxisProfileResult::ERR_PROFILE_NOT_FOUND;
    }
  }

  std::string typeString;
  if (!createTypeString(address, name, typeString, callback))
  {
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  ProviderHelper::createMetaData(typeString, metaData, callback);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult NodeProfilesBase::onMetadataBasicType(const std::string& address, const ResponseCallback& callback, comm::datalayer::VariantType variantType)
{
  std::string name;
  ProviderHelper::getProfileNameFromAddress(address, name);

  if (!name.empty())
  {
    // The address string contains a profile name. Check if it exists in the system.
    if (!getProfileByAddress(address, callback))
    {
      return AxisProfileResult::ERR_PROFILE_NOT_FOUND;
    }
  }

  std::string typeString {};
  if (!createBasicTypeString(name, variantType, typeString, callback))
  {
    return AxisProfileResult::ERR_WRONGDATATYPE;
  }
  ProviderHelper::createMetaData(typeString, m_metaDataInfo, callback);
  return AxisProfileResult::STS_OK;
}
