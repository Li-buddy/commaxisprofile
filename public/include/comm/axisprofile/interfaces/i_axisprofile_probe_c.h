#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface for motion bundles for probe function.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

/// @par List of available functions in this part of the axisprofile probe function.
/// <TABLE>
/// <TR><TH>           Function                         </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> probeBaseConfig                  </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> probeExtConfig                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> probeStatusCtrl                  </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getProbeActualValuesRT           </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> onlineChangeParams               </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getProbeValues                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> isProbeSupported                 </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> abortAndResetProbe               </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// </TABLE>

#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
  /// @brief This is the C-Interface for the axisprofile probe 'IAxisProfileProbe'.
  typedef struct IAxisProfileProbeC
  {
    /// @brief Pointer to the instantiated object.
    void *handle;

    /// @brief This function set the probe control word for initial configuration
    ///
    /// @param[in]  handle                  Handle of the interface
    /// @param[in]  probeIdx                Indicates the probe number
    /// @param[in]  probeConfig             Probe control word configuration
    /// @param[in]  isAutoActivated         Indicates if probe automatic activated after configuration
    /// @return                             Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*probeBaseConfig)(void* handle, ::comm::axisprofile::ProbeIndex probeIdx, ::comm::axisprofile::ProbeBaseConfig probeConfig, bool isAutoActivated);

    /// @brief This function set the probe extended control word for initial configuration
    ///
    /// @param[in]  handle                  Handle of the interface
    /// @param[in]  probeIdx                Indicates the probe number
    /// @param[in]  probeExtendConfig       Probe extended control word configuration
    /// @param[in]  isStateMonitActivated   Indicates if state monit activate
    /// @return                             Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*probeExtConfig)(void* handle, ::comm::axisprofile::ProbeIndex probeIdx, ::comm::axisprofile::ProbeExtConfig probeExtendConfig, bool isStateMonitActivated);

    /// @brief This function controls the enable/disable status of probe measurement
    ///
    /// @param[in]  handle                  Handle of the interface
    /// @param[in]  probeIdx                Indicates the probe number
    /// @param[in]  enable                  Is probe enabled
    /// @return                             Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*probeStatusCtrl)(void* handle, ::comm::axisprofile::ProbeIndex probeIdx, bool enable);

    /// @brief This function get the probe actual measured values
    ///
    /// @param[in]  handle                  Handle of the interface
    /// @param[in]  probeIdx                Indicates the probe number
    /// @param[in]  actualValueProbe        The actual measured values of probe
    /// @return                             Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*getProbeActualValuesRT)(void* handle, ::comm::axisprofile::ProbeIndex probeIdx, ::comm::axisprofile::ProbeActualValues* actualValueProbe);

    /// @brief This function online update and changes the measurement
    ///
    /// @param[in]  handle                  Handle of the interface
    /// @param[in]  probeIdx                Indicates the probe number
    /// @param[in]  updateValue             Online update the measurement configuration
    /// @return                             Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*onlineChangeParams)(void* handle, ::comm::axisprofile::ProbeIndex probeIdx, ::comm::axisprofile::ProbeUpdateMeasurement updateValue);

    /// @brief This function get probe acutal configuration.
    ///
    /// @param[in]  handle                  Handle of the interface
    /// @param[in]  probeIdx                Indicates the probe number.
    /// @param[out] probeConfig             Probe control word configuration.
    /// @param[out] probeExtendConfig       Probe extended control word configuration.
    /// @return                             Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*getProbeValues)(void* handle, ::comm::axisprofile::ProbeIndex probeIdx, ::comm::axisprofile::ProbeBaseConfig* probeConfig, ::comm::axisprofile::ProbeExtConfig* probeExtendConfig);

    /// @brief This method check if probe supports on current device
    ///
    /// @param[in]  handle                  Handle of the interface
    /// @return                             Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*isProbeSupported)(void* handle);

    /// @brief This method abort and reset probe
    ///
    /// @param[in]  handle                  Handle of the interface
    /// @return                             Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*abortAndResetProbe)(void* handle);
    
    void *reserved[100];  /// reservation
  } IAxisProfileProbeC;

#ifdef __cplusplus
}
#endif
