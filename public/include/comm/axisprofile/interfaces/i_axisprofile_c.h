#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface for motion bundles for all function of axisprofile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

/// @par List of available functions in this part of the axisprofile C interface.
/// <TABLE>
/// <TR><TH>           Function                         </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> getCommandInterface              </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getStatusControlInterface        </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getCyclicDataInterface           </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getBootingConfigInterface        </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getDiagMsgInterface              </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getProbeInterface                </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getCmdInterface                  </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getPropertyInterface             </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// </TABLE>

#include "i_axisprofile_command_c.h"
#include "i_axisprofile_status_control_c.h"
#include "i_axisprofile_diag_msg_c.h"
#include "i_axisprofile_cyclic_data_c.h"
#include "i_axisprofile_booting_config_c.h"
#include "i_axisprofile_probe_c.h"
#include "i_axisprofile_cmd_c.h"
#include "i_axisprofile_device_param_c.h"
#include "i_axisprofile_property_c.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
  /// @brief This is the C-Interface for the axisprofile main interface 'IAxisProfile'.
  typedef struct IAxisProfileC
  {
    /// @brief Pointer to the instantiated object.
    void* handle;

    /// @brief Deprecated, get the command interface
    /// @deprecated will be removed in future versions, using @getCmdInterface if possible
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Pointer to the command interface
    const IAxisProfileCommandC* (*getCommandInterface)(void* handle);

    /// @brief Get the status and control word interface
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Pointer to the status and control word interface
    const IAxisProfileStatusControlC* (*getStatusControlInterface)(void* handle);

    /// @brief Get the cyclic data interface
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Pointer to the cyclic data interface
    const IAxisProfileCyclicDataC* (*getCyclicDataInterface)(void* handle);

    /// @brief Deprecated interfaces
    void* removedSetDeviceAddress {};
    void* removedGetDeviceAddress {};
    void* removedGetProfileType {};
    void* removedGetDeviceState {};
    void* removedGetDeviceScalingType {};
    void* removed {};

    /// @brief Deprecated, get the booting configuration interface
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return The booting configuration interface
    const IAxisProfileBootingConfigC* (*getBootingConfigInterface)(void* handle);

    /// @brief Get the device diagnosis interface
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return The device diagnosis interface
    const IAxisProfileDiagMsgC* (*getDiagMsgInterface)(void* handle);

    /// @brief Get the probe interface
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return The probe interface
    const IAxisProfileProbeC* (*getProbeInterface)(void* handle);

    /// @brief Deprecated interfaces
    void* removedGetProfileCategory {};

    /// @brief Get the command interface
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Pointer to the command interface
    const IAxisProfileCmdC* (*getCmdInterface)(void* handle);

    /// @brief Get the device parameter interface
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Pointer to the device parameter interface
    const IAxisProfileDeviceParamC* (*getDeviceParamInterface)(void* handle);

    /// @brief Get the property interface
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Pointer to the property interface
    const IAxisProfilePropertyC* (*getPropertyInterface)(void* handle);

    void* reserved[100];  ///< reservation
  } IAxisProfileC;

#ifdef __cplusplus
}
#endif
