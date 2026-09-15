/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for status and controlword handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_status_control_drive.h"
#include "bundle/axisprofile_status_control_c.h"

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

AxisProfileStatusControlDrive::AxisProfileStatusControlDrive(ProfileBasicInfo& profileBasicInfo)
  : AxisProfileStatusControl(profileBasicInfo)
{
  
}

AxisProfilePowerMode AxisProfileStatusControlDrive::getPowerMode() const
{
  return m_actualPowerMode;
}

bool AxisProfileStatusControlDrive::checkOperationMode(const AxisProfileOperationMode opMode) const
{
  return (opMode == getOperationMode());
}

AxisProfileResult AxisProfileStatusControlDrive::readOperationMode(AxisProfileOperationMode& operationMode)
{
  operationMode = getOperationMode();
  return AxisProfileResult::STS_OK;
}

bool AxisProfileStatusControlDrive::checkPowerMode(const AxisProfilePowerMode powerMode) const
{
  return (m_actualPowerMode == powerMode) ? true : false;
}

