#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface for motion bundles for property function.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

///  @par List of available functions in this part of the axisprofile property C interface.
/// <TABLE>
/// <TR><TH>           Function                         </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> setDeviceAddress                 </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getDeviceAddress                 </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getProfileType                   </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getDeviceState                   </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getDeviceScalingType             </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getProfileCategory               </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// </TABLE>
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"



#ifdef __cplusplus
extern "C" {
#endif
  /// @brief This is the C-Interface for the axisprofile property interface 'IaxisprofileProperty'.
  typedef struct IAxisProfilePropertyC
  {
    /// @brief Pointer to the instantiated object.
    void* handle;

    /// @brief Set device address
    /// @param[in]  handle        Pointer to the instantiated object
    /// @param[in]  deviceAddress Device address
    /// @return Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*setDeviceAddress)(void* handle, std::uint32_t deviceAddress);

    /// @brief Get device address
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Device address
    std::uint32_t(*getDeviceAddress)(void* handle);

    /// @brief Get profile type
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Profile type
    ::comm::axisprofile::AxisProfileType(*getProfileType)(void* handle);

    /// @brief Get device state
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return Status of the function call: STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*getDeviceState)(void* handle);

    /// @brief This method gets the actual scaling type of the connected device for the given value type
    /// @param[in]  handle        Handle of the interface
    /// @param[in]  valueType     Defines the value type for the scaling type request
    /// @return The scaling type of the device
    ::comm::axisprofile::AxisProfileDeviceScalingType(*getDeviceScalingType)(void* handle, ::comm::axisprofile::AxisProfileDeviceValueType valueType);

    /// @brief Get the profile category
    /// @param[in]  handle        Pointer to the instantiated object
    /// @return The profile category
    ::comm::axisprofile::AxisProfileCategory(*getProfileCategory)(void* handle);

    void* reserved[100];  /// reservation
  } IAxisProfilePropertyC;

#ifdef __cplusplus
}
#endif
