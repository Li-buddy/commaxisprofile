/// @file
///
/// @brief This source file is part of the axisprofile library and implement the EncoderCategoryAdapterCoE class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "encoder_category_adapter_coe.h"
#include "scaling_strategy_encoder.h"
#include "axisprofile_device_param_encoder.h"
#include "axisprofile_command_encoder.h"
#include "axisprofile_probe_encoder.h"
#include "axisprofile_status_control_encoder.h"
#include "axisprofile_cyclic_data_encoder.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
EncoderCategoryAdapterCoE::EncoderCategoryAdapterCoE(ProfileBasicInfo& profileBasicInfo,
                                                     const AcknowledgeTelegram* at,
                                                     MasterDataTelegram* mdt)
  :CategoryAdapterEncoder(profileBasicInfo, at, mdt)
{
  // Create scaling strategy
  m_scaling = new ScalingStrategyEncoder(profileBasicInfo);

  // create concrete category object(default as drive category)
  m_cyclicDataInterface = new AxisProfileCyclicDataEncoder(profileBasicInfo, at, mdt);

  // Create booting config class
  m_deviceParamInterface = new AxisProfileDeviceParamEncoder(profileBasicInfo, at);

  // Create command class
  m_commandInterface = new AxisProfileCommandEncoder(profileBasicInfo, at, mdt);

  // Create status- and controlword class
  m_statusControlInterface = new AxisProfileStatusControlEncoder(profileBasicInfo);

  // Create probe class
  m_probeInterface = new AxisProfileProbeEncoder(profileBasicInfo);
}
