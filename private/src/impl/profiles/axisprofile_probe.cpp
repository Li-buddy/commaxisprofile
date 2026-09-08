/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for probe handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_probe.h"
#include "bundle/axisprofile_probe_c.h"
#include "diagnosis/dia.h"

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

AxisProfileProbe::AxisProfileProbe(ProfileBasicInfo& profileBasicInfo)
  :m_profileBasicInfo(profileBasicInfo), m_sharingHandler(nullptr)
{
  // *** Initialize C-Interface *** //
  m_cInterface.handle = this;
  m_cInterface.probeBaseConfig = AXISPROFILE_probeBaseConfig;
  m_cInterface.probeExtConfig = AXISPROFILE_probeExtConfig;
  m_cInterface.probeStatusCtrl = AXISPROFILE_probeStatusCtrl;
  m_cInterface.getProbeActualValuesRT = AXISPROFILE_getProbeActualValuesRT;
  m_cInterface.onlineChangeParams = AXISPROFILE_onlineChangeParams;
  m_cInterface.getProbeValues = AXISPROFILE_getProbeValues;
  m_cInterface.abortAndResetProbe = AXISPROFILE_abortAndResetProbe;
  m_cInterface.isProbeSupported = AXISPROFILE_isProbeSupported;
}

const IAxisProfileProbeC* AxisProfileProbe::getCInterface() const
{
  return &m_cInterface;
}

void AxisProfileProbe::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

AxisProfileResult AxisProfileProbe::booting()
{
  loadBootingTasks();
  AxisProfileResult result {AxisProfileResult::STS_OK};
  for (const auto& method : m_initCommands)
  {
    result = method();
    if (aprStatusNotOkay(result))
    {
      APR_TRACE_ERROR("%s; Init probe commands failed.", m_profileBasicInfo.m_profileTrace.createTraceMsg(result));
      return result;
    }
  }
  return result;
}
