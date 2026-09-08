/// @file
///
/// This source file is part of the axisprofile library and check the offset between control and device of a specific profile.
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_device_offset.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "profiles/scaling/i_scaling.h"
// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::provider;

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Implementation of classes and structures
// ---------------------------------------------------------------------------

NodeProfilesDeviceOffset::NodeProfilesDeviceOffset( ProviderMgr* mgr ) : NodeProfilesBase(mgr)
{
  this->m_metaDataInfo.m_description = "Offset between control and device, SI unit";
  this->m_metaDataInfo.m_descriptionUrl = "";
  this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  this->m_metaDataInfo.nodeActions = node_actions::READ;
}

void NodeProfilesDeviceOffset::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if ( !profile )
  {
    return;
  }

  auto scaling = profile->getScaling();
  double offset = scaling->getHomeOffset();
  
  // Finish
  comm::datalayer::Variant returnVal;
  returnVal.setValue(offset);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeProfilesDeviceOffset::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  std::string name;
  ProviderHelper::getProfileNameFromAddress(address, name);

  if (!name.empty())
  {
    // The address string contains a profile name. Check if it exists in the system.
    if (!getProfileByAddress(address, callback))
    {
      return;
    }
  }

  std::string typeString{};
  if (!createBasicTypeString(name, comm::datalayer::VariantType::UINT32, typeString, callback))
  {
    return;
  }

  ProviderHelper::createMetaData(typeString, m_metaDataInfo, callback);
}
