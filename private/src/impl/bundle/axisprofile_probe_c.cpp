/// @file
///
/// @brief This file is part of the axisprofile library and implements
///        C/C++ method mapping for the base class for probe handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_axisprofile_probe.h"
#include "axisprofile_if_helper.h"

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
// Function implementation (Wrapper for C-Interface)
// ---------------------------------------------------------------------------

extern "C" AxisProfileResult AXISPROFILE_probeBaseConfig(void* handle, ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool isAutoActivated)
{
  IAxisProfileProbe* probeInterface = static_cast<IAxisProfileProbe*>(handle);
  RETURN_HANDLE_INVALIDE(probeInterface, AxisProfileResult::ERR_EXEC_NULL, "get probe control word configuration");
  return probeInterface->probeBaseConfig(probeIdx, probeConfig, isAutoActivated);
}

extern "C" AxisProfileResult AXISPROFILE_probeExtConfig(void* handle, ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig, bool isStateMonitActivated)
{
  IAxisProfileProbe* probeInterface = static_cast<IAxisProfileProbe*>(handle);
  RETURN_HANDLE_INVALIDE(probeInterface, AxisProfileResult::ERR_EXEC_NULL, "get probe extended control word configuration");
  return probeInterface->probeExtConfig(probeIdx, probeExtendConfig, isStateMonitActivated);
}

extern "C" AxisProfileResult AXISPROFILE_probeStatusCtrl(void* handle, ProbeIndex probeIdx, bool enable)
{
  IAxisProfileProbe* probeInterface = static_cast<IAxisProfileProbe*>(handle);
  RETURN_HANDLE_INVALIDE(probeInterface, AxisProfileResult::ERR_EXEC_NULL, "enable/disable the probe");
  return probeInterface->probeStatusCtrl(probeIdx, enable);
}

extern "C" AxisProfileResult AXISPROFILE_getProbeActualValuesRT(void* handle, ProbeIndex probeIdx, ProbeActualValues* actualValueProbe)
{
  IAxisProfileProbe* probeInterface = static_cast<IAxisProfileProbe*>(handle);
  RETURN_HANDLE_INVALIDE(probeInterface, AxisProfileResult::ERR_EXEC_NULL, "get probe acutal values");
  return probeInterface->getProbeActualValuesRT(probeIdx, actualValueProbe);
}

extern "C" AxisProfileResult AXISPROFILE_onlineChangeParams(void* handle, ProbeIndex probeIdx, ProbeUpdateMeasurement updateValue)
{
  IAxisProfileProbe* probeInterface = static_cast<IAxisProfileProbe*>(handle);
  RETURN_HANDLE_INVALIDE(probeInterface, AxisProfileResult::ERR_EXEC_NULL, "online change parameters");
  return probeInterface->onlineChangeParams(probeIdx, updateValue);
}

extern "C" AxisProfileResult AXISPROFILE_getProbeValues(void* handle, ProbeIndex probeIdx, ProbeBaseConfig* probeConfig, 
                                                        ProbeExtConfig* probeExtendConfig)
{
  IAxisProfileProbe* probeInterface = static_cast<IAxisProfileProbe*>(handle);
  RETURN_HANDLE_INVALIDE(probeInterface, AxisProfileResult::ERR_EXEC_NULL, "get probe values");
  return probeInterface->getProbeValues(probeIdx, probeConfig, probeExtendConfig);
}

extern "C" AxisProfileResult AXISPROFILE_abortAndResetProbe(void* handle)
{
  IAxisProfileProbe* probeInterface = static_cast<IAxisProfileProbe*>(handle);
  RETURN_HANDLE_INVALIDE(probeInterface, AxisProfileResult::ERR_EXEC_NULL, "abort and reset probe");
  return probeInterface->abortAndResetProbe();
}

extern "C" AxisProfileResult AXISPROFILE_isProbeSupported(void* handle)
{
  IAxisProfileProbe* probeInterface = static_cast<IAxisProfileProbe*>(handle);
  RETURN_HANDLE_INVALIDE(probeInterface, AxisProfileResult::ERR_EXEC_NULL, "check if probe is supported");
  return probeInterface->isProbeSupported();
}
