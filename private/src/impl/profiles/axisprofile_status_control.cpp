/// @file
///
/// This source file is part of the axisprofile library and implements
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
#include "axisprofile_status_control.h"
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

AxisProfileStatusControl::AxisProfileStatusControl(ProfileBasicInfo& profileBasicInfo)
  : m_profileBasicInfo(profileBasicInfo), m_sharingHandler(nullptr)
{
  // *** Initialize C-Interface *** //
  m_cInterface.handle = this;

  // Statusword
  m_cInterface.getStatusWord = AXISPROFILE_getStatusWord;
  m_cInterface.checkOperationMode = AXISPROFILE_checkOperationMode;
  m_cInterface.checkOperationModeSupported = AXISPROFILE_checkOperationModeSupported;
  m_cInterface.getOperationMode = AXISPROFILE_getOperationMode;
  m_cInterface.readOperationMode = AXISPROFILE_readOperationMode;
  m_cInterface.checkPowerMode = AXISPROFILE_checkPowerMode;
  m_cInterface.getPowerMode = AXISPROFILE_getPowerMode;
  m_cInterface.checkError = AXISPROFILE_checkError;
  m_cInterface.cyclicStandStillCheck = AXISPROFILE_cyclicStandStillCheck;
  m_cInterface.isDeviceFollowsControl = AXISPROFILE_isDeviceFollowsControl;
  m_cInterface.deviceHasWarning = AXISPROFILE_deviceHasWarning;
  m_cInterface.checkReadyToOperate = AXISPROFILE_checkReadyToOperate;

  // Controlword
  m_cInterface.setControlWord = AXISPROFILE_setControlWord;
  m_cInterface.setOperationMode = AXISPROFILE_setOperationMode;
  m_cInterface.setPowerMode = AXISPROFILE_setPowerMode;

  // Drive Halt
  m_cInterface.setDriveHaltMode = AXISPROFILE_setDriveHaltMode;
  m_cInterface.getDriveHaltMode = AXISPROFILE_getDriveHaltMode;
}

const IAxisProfileStatusControlC* AxisProfileStatusControl::getCInterface() const
{
  return &m_cInterface;
}

AxisProfileResult AxisProfileStatusControl::booting()
{
  loadBootingTasks();
  AxisProfileResult result{ AxisProfileResult::STS_OK };
  for (const auto& method : m_initStatusControl)
  {
    result = method();
    if (aprStatusNotOkay(result))
    {
      APR_TRACE_ERROR("%s; Init status control failed.", m_profileBasicInfo.m_profileTrace.createTraceMsg(result));
      return result;
    }
  }
  return result;
}

void AxisProfileStatusControl::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}
