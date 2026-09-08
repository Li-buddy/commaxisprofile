/// @file
///
/// @brief This file is part of the axisprofile library and implements the drive category adapter for CoE.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "drive_category_adapter_coe.h"
#include "profiles/scaling/scaling_strategy_factory.h"
#include "profiles/canopen/drive/canopen_scaling_strategy_drive.h"
#include "profiles/canopen/drive/canopen_cyclic_data_drive.h"
#include "profiles/canopen/drive/canopen_device_param_drive.h"
#include "profiles/canopen/drive/canopen_command_drive.h"
#include "profiles/canopen/drive/canopen_probe_drive.h"
#include "profiles/canopen/drive/canopen_status_control_drive.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

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
DriveCategoryAdapterCoE::DriveCategoryAdapterCoE(ProfileBasicInfo& profileBasicInfo,
                                                 const AcknowledgeTelegram* at,
                                                 MasterDataTelegram* mdt)
  :CategoryAdapterDrive(profileBasicInfo, at, mdt)
{
  // Create scaling strategy
  m_scaling = new ScalingStrategyCoEDrive(profileBasicInfo);

  // create concrete category object(default as drive category)
  m_cyclicDataInterface = new CanopenCyclicDataDrive(profileBasicInfo, at, mdt);

  // Create booting config class
  m_deviceParamInterface = new CanopenDeviceParamDrive(profileBasicInfo, at, mdt);

  // Create command class
  m_commandInterface = new CanopenCommandDrive(profileBasicInfo, at, mdt);

  // Create status- and controlword class
  m_statusControlInterface = new CanopenStatusControlDrive(profileBasicInfo, at, mdt);

  // Create probe class
  m_probeInterface = new CanopenProbeDrive(profileBasicInfo, at);
}

AxisProfileResult DriveCategoryAdapterCoE::updateScalingStrategy(const AxisProfileScalingStrategy& strategy)
{
  //origin strategy equal to new one, no need to change
  AxisProfileResult retval = ScalingStrategyFactory::checkCompatiblityForStrategy(&m_profileBasicInfo.m_profileTrace, strategy, m_profileBasicInfo.m_axisProfileType);
  if (AxisProfileResult::STS_OK != retval)
  {
    APR_TRACE_ERROR("%s; current profile type is %s, it's invalid type", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileTypeToString(m_profileBasicInfo.m_axisProfileType));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  //create new strategy and hand over to axisprofile manage.
  //the subsequent management will be finished by axisprofile
  IScaling* newStrategy = ScalingStrategyFactory::createScalingStrategy(m_profileBasicInfo, strategy);
  if (!newStrategy)
  {
    APR_TRACE_ERROR("%s; current drive scaling mode: %s, which is not supported", m_profileBasicInfo.m_profileTrace.createTraceMsg(), axisProfileScalingStrategyToString(strategy));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  //identify the original one
  IScaling* oldStrategy = m_scaling;

  //update new scaling strategy
  m_scaling = newStrategy;

  //after change scaling strategy, the origin one need to be cleaned
  if (oldStrategy)
  {
    m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_SCALING);
    delete oldStrategy;
    oldStrategy = nullptr;
  }

  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, m_scaling);

  return AxisProfileResult::STS_OK;
}
