/// @file      sharing_handler.cpp
///
/// This source file is part of the axisprofile library and implements the
/// data sharing handler between every profile sub-module
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/profile_trace.h"
#include "sharing_handler.h"
#include "profiles/category/category_adapter.h"
#include "profiles/i_device_boot_param_snapshot.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

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

AxisProfileSharingHandler::AxisProfileSharingHandler(ProfileTrace* trace)
  : m_profileTrace(trace), m_sharingITFs {}
{
}

AxisProfileResult AxisProfileSharingHandler::registSharingITF(SharingInterfaceType itfType, IAxisProfileSharing* const sharingITF)
{
  if (!sharingITF)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  if (itfType > SharingInterfaceType::ITF_TYPE_MAX)
  {
    return AxisProfileResult::ERR_WRONGDATATYPE;
  }

  m_sharingITFs[static_cast<std::uint16_t>(itfType)] = sharingITF;
  // set sharing handler for interface
  sharingITF->setSharingHandler(this);
  return AxisProfileResult::STS_OK;
}

void AxisProfileSharingHandler::removeSharingITF(SharingInterfaceType itfType)
{
  if (itfType > SharingInterfaceType::ITF_TYPE_MAX)
  {
    return;
  }
  m_sharingITFs[static_cast<std::uint16_t>(itfType)] = nullptr;
}

IAxisProfileDeviceParamInternal* AxisProfileSharingHandler::getDeviceParamIF() const
{
  return getInternalITF<IAxisProfileDeviceParamInternal* const>(SharingInterfaceType::ITF_TYPE_DEVICEPARAM);
}

IAxisProfileMappingHandler* AxisProfileSharingHandler::getMappingHandlerIF() const
{
  return getInternalITF<IAxisProfileMappingHandler* const>(SharingInterfaceType::ITF_TYPE_MAPHANDLER);
}

IAxisProfileCommandInternal* AxisProfileSharingHandler::getCommandIF() const
{
  return getInternalITF<IAxisProfileCommandInternal* const>(SharingInterfaceType::ITF_TYPE_COMMAND);
}

IAxisProfileCyclicDataInternal* AxisProfileSharingHandler::getCyclicDataIF() const
{
  return getInternalITF<IAxisProfileCyclicDataInternal* const>(SharingInterfaceType::ITF_TYPE_CYCLICDATA);
}

IAxisProfileStatusControlInternal *AxisProfileSharingHandler::getStatusControlIF() const
{
  return getInternalITF<IAxisProfileStatusControlInternal* const>(SharingInterfaceType::ITF_TYPE_STATUSCONTROL);
}

IAuxJobHandler* AxisProfileSharingHandler::getAuxJobHandlerIF() const
{
  return getInternalITF<IAuxJobHandler* const>(SharingInterfaceType::ITF_TYPE_AUXJOB);
}

IAxisProfileParamAccess* AxisProfileSharingHandler::getParamAccessIF() const
{
  return getInternalITF<IAxisProfileParamAccess* const>(SharingInterfaceType::ITF_TYPE_PARAMACCESS);
}

IJobMgr* AxisProfileSharingHandler::getJobMgrIF() const
{
  return getInternalITF<IJobMgr* const>(SharingInterfaceType::ITF_TYPE_JOB_MGR);
}

IScaling* AxisProfileSharingHandler::getScalingIF() const
{
  return getInternalITF<IScaling* const>(SharingInterfaceType::ITF_TYPE_SCALING);
}

ICategoryAdapter* AxisProfileSharingHandler::getCategoryIF() const
{
  return getInternalITF<ICategoryAdapter* const>(SharingInterfaceType::ITF_TYPE_CATEGORY);
}

IAxisProfilePropertyInternal* AxisProfileSharingHandler::getPropertyIF() const
{
  return getInternalITF<IAxisProfilePropertyInternal* const>(SharingInterfaceType::ITF_TYPE_PROPERTY);
}

IAxisProfileCmdInternal* AxisProfileSharingHandler::getCmdIF() const
{
  return getInternalITF<IAxisProfileCmdInternal* const>(SharingInterfaceType::ITF_TYPE_CMD);
}

IDeviceBootParamSnapshot* AxisProfileSharingHandler::getBootParamSnapshotIF() const
{
  return getInternalITF<IDeviceBootParamSnapshot* const>(SharingInterfaceType::ITF_TYPE_BOOTPARAMSNAPSHOT);
}
