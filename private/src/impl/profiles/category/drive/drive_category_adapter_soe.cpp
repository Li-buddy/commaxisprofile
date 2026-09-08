/// @file
///
/// @brief This file is part of the axisprofile library and implements the drive category adapter for SoE.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "drive_category_adapter_soe.h"
#include "profiles/sercos/drive/sercos_scaling_strategy_drive.h"
#include "profiles/sercos/drive/sercos_device_param_drive.h"
#include "profiles/sercos/drive/sercos_command_drive.h"
#include "profiles/sercos/drive/sercos_probe_drive.h"
#include "profiles/sercos/drive/sercos_status_control_drive.h"
#include "profiles/sercos/drive/sercos_cyclic_data_drive.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

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
DriveCategoryAdapterSoE::DriveCategoryAdapterSoE(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at,
                                                 MasterDataTelegram* mdt)
  :CategoryAdapterDrive(profileBasicInfo, at, mdt)
{
  // Create scaling class
  m_scaling = new ScalingStrategySoEDrive(profileBasicInfo);

  // Create concrete category object(default as drive category)
  m_cyclicDataInterface = new SercosCyclicDataDrive(profileBasicInfo, at, mdt);

  // Create booting config class
  m_deviceParamInterface = new SercosDeviceParamDrive(profileBasicInfo, at, mdt);

  // Create command class
  m_commandInterface = new SercosCommandDrive(profileBasicInfo, at);

  // Create status- and control word class
  m_statusControlInterface = new SercosStatusControlDrive(profileBasicInfo, at, mdt);

  // Create probe class
  m_probeInterface = new SercosProbeDrive(profileBasicInfo, at);
}

AxisProfileResult DriveCategoryAdapterSoE::updateScalingStrategy(const AxisProfileScalingStrategy& /*strategy*/)
{
  return AxisProfileResult::STS_OK;
}
