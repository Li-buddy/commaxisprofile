#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines
///        C/C++ method mapping for the base class for probe handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_axisprofile_probe.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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
// Declaration of classes and structures
// ---------------------------------------------------------------------------

/// @brief This method set the probe control word for initial configuration.
/// @param[in]   handle                 Handle of the interface.   
/// @param[in]   probeIdx               Indicates the probe number.
/// @param[in]   probeConfig            Probe control word configuration.
/// @param[in]   isAutoActivated        Is probe automatic activated.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_probeBaseConfig(void* handle, comm::axisprofile::ProbeIndex probeIdx, comm::axisprofile::ProbeBaseConfig probeConfig, bool isAutoActivated);

/// @brief This method set the probe extended control word for initial configuration.
/// @param[in]   handle                 Handle of the interface.
/// @param[in]   probeIdx               Indicates the probe number.
/// @param[in]   probeExtendConfig      Probe extended control word configuration.
/// @param[in]   isStateMonitActivated  Is state monit activate.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_probeExtConfig(void* handle, comm::axisprofile::ProbeIndex probeIdx, comm::axisprofile::ProbeExtConfig probeExtendConfig, bool isStateMonitActivated);

/// @brief This method enable/disable the probe measurement.
/// @param[in]   handle                 Handle of the interface.
/// @param[in]   probeIdx               Indicates the probe number.
/// @param[in]   enable                 Is probe enabled.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_probeStatusCtrl(void* handle, comm::axisprofile::ProbeIndex probeIdx, bool enable);

/// @brief This method get the probe actual measured values.
/// @param[in]   handle                 Handle of the interface.
/// @param[in]   probeIdx               Indicates the probe number.
/// @param[out]  actualValueProbe       The actual measured values of probe.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getProbeActualValuesRT(void* handle, comm::axisprofile::ProbeIndex probeIdx, comm::axisprofile::ProbeActualValues* actualValueProbe);

/// @brief This method online update and changes the measurement. 
/// @param[in]   handle                 Handle of the interface.
/// @param[in]   probeIdx               Indicates the probe number.
/// @param[in]   updateValue            Online update the measurement configuration.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_onlineChangeParams(void* handle, comm::axisprofile::ProbeIndex probeIdx, comm::axisprofile::ProbeUpdateMeasurement updateValue);

/// @brief This method get probe acutal configuration.
/// @param[in]   handle                 Handle of the interface.
/// @param[in]   probeIdx               Indicates the probe number.
/// @param[out]  probeConfig            Probe control word configuration.
/// @param[out]  probeExtendConfig      Probe extended control word configuration.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getProbeValues(void* handle, comm::axisprofile::ProbeIndex probeIdx, comm::axisprofile::ProbeBaseConfig* probeConfig, comm::axisprofile::ProbeExtConfig* probeExtendConfig);

/// @brief This method abort and reset probe.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_abortAndResetProbe(void* handle);
    
/// @brief This method check if probe is supported.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_isProbeSupported(void* handle);
