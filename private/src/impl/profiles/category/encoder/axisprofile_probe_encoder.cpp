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
#include "axisprofile_probe_encoder.h"
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

AxisProfileProbeEncoder::AxisProfileProbeEncoder(ProfileBasicInfo& profileBasicInfo)
  :AxisProfileProbe(profileBasicInfo)
{
}

void AxisProfileProbeEncoder::loadBootingTasks()
{
  m_initCommands.clear();
}

AxisProfileResult AxisProfileProbeEncoder::probeBaseConfig(ProbeIndex /*probeIdx*/, ProbeBaseConfig /*probeConfig*/, bool /*isAutoActivated*/)
{
  rejectCmdDiagnosis("probeBaseConfig");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileProbeEncoder::probeExtConfig(ProbeIndex /*probeIdx*/, ProbeExtConfig /*probeExtendConfig*/, bool /*isStateMonitActivated*/)
{
  rejectCmdDiagnosis("probeExtConfig");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileProbeEncoder::probeStatusCtrl(ProbeIndex /*probeIdx*/, bool /*enable*/)
{
  rejectCmdDiagnosis("probeStatusCtrl");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileProbeEncoder::getProbeActualValuesRT(ProbeIndex /*probeIdx*/, ProbeActualValues* /*actualValueProbe*/)
{
  rejectCmdDiagnosis("getProbeActualValuesRT");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileProbeEncoder::onlineChangeParams(ProbeIndex /*probeIdx*/, ProbeUpdateMeasurement /*updateValue*/)
{
  rejectCmdDiagnosis("onlineChangeParams");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileProbeEncoder::getProbeValues(ProbeIndex /*probeIdx*/, ProbeBaseConfig* /*probeConfig*/, ProbeExtConfig* /*probeExtendConfig*/)
{
  rejectCmdDiagnosis("getProbeValues");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileProbeEncoder::abortAndResetProbe()
{
  rejectCmdDiagnosis("abortAndResetProbe");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileProbeEncoder::isProbeSupported()
{
  rejectCmdDiagnosis("isProbeSupported");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

void AxisProfileProbeEncoder::rejectCmdDiagnosis(const char* cmdNameInfo) const
{
  auto category = m_sharingHandler->getCategoryIF()->getProfileCategory();
  Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_FUNCTION_NOT_SUPPORTED, __FILE__, __FUNCTION__, __LINE__,
                     "Command %s not allowed for %s on profile %s.", cmdNameInfo, axisProfileCategoryTypeToString(category),
                     m_profileBasicInfo.m_profileName.c_str());
}
