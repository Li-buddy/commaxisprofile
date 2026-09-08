#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface for motion bundles for status word and control word.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

/// @par List of available functions in this part of the axisprofile status control word.
/// <TABLE>
/// <TR><TH>           Function                             </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> getStatusWord                        </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkOperationMode                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getOperationMode                     </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkPowerMode                       </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getPowerMode                         </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkError                           </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> setControlWord                       </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> setOperationMode                     </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> cyclicStandStillCheck                </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkOperationModeSupported          </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> isDeviceFollowsControl               </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> setDriveHaltMode                     </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getDriveHaltMode                     </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> deviceHasWarning                     </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkReadyToOperate                  </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// </TABLE>

#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
  /// @brief This is the C-Interface of the axisprofile status- and controlword interface 'IAxisProfileStatusControl'.
  typedef struct IAxisProfileStatusControlC
  {
    /// @brief This is pointer to the instantiated object
    void *handle;

    /// @brief This function returns the statusword
    std::uint16_t(*getStatusWord)(void *handle);

    /// @brief This function checks if the given operation mode equals the operation mode of the device.
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  opMode          The operation mode to check
    /// @return true if the operation mode is equal to given value, false otherwise
    bool(*checkOperationMode)(void* handle, const ::comm::axisprofile::AxisProfileOperationMode opMode);

    /// @brief This function get acutal operation mode
    ///
    /// @param[in]  handle          Handle of the interface
    /// @return The actual operation mode
    ::comm::axisprofile::AxisProfileOperationMode(*getOperationMode)(void* handle);

    /// @brief This function checks if the given power mode equals the power mode of the device
    /// 
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  powerMode       The power mode to be checked
    /// @return true if the power mode is active, false otherwise
    bool(*checkPowerMode)(void *handle, const ::comm::axisprofile::AxisProfilePowerMode powerMode);

    /// @brief This function gets the actual power mode of the device
    ///
    /// @param[in]  handle          Handle of the interface
    /// @return The actual power mode
    ::comm::axisprofile::AxisProfilePowerMode(*getPowerMode)(void *handle);

    /// @brief This function checks if the device is in error state.
    ///
    /// @param[in]  handle          Handle of the interface
    /// @return true if the device is in error state, false otherwise
    bool(*checkError)(void*handle);

    /// @brief This function sets the control word of the device
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  controlWord     The control word to be set
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*setControlWord)(void *handle, std::uint16_t controlWord);

    /// @brief This function sets the given operation mode to the device
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  opMode          The operation mode to be set
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*setOperationMode)(void *handle, const ::comm::axisprofile::AxisProfileOperationMode opMode);

    /// @brief This function sets the given power mode to the device
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  powerMode       The power mode to be set
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*setPowerMode)(void *handle, const ::comm::axisprofile::AxisProfilePowerMode powerMode);

    /// @brief This function checks if the device is in standstill state
    ///
    /// @param[in]  handle          Handle of the interface
    /// @return true if the device is in standstill state, false otherwise
    bool(*cyclicStandStillCheck)(void *handle);

    /// @brief This function checks if the given operation mode is supported by the device
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  opMode          The operation mode to be checked
    /// @param[out] errorInfo       The error information in case the operation mode is not supported
    /// @return true if the operation mode is supported, false otherwise
    bool(*checkOperationModeSupported)(void* handle, const ::comm::axisprofile::AxisProfileOperationMode opMode, ::comm::axisprofile::OpModeErrorInfo& errorInfo);

    /// @brief This function checks if the device follows the control
    ///
    /// @param[in]  handle          Handle of the interface
    /// @return true if the device follows the control, false otherwise
    bool(*isDeviceFollowsControl)(void* handle);

    /// @brief This function sets the given drive halt mode to device true for enable false for disable
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  haltEnable      Enable or disable drive halt
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*setDriveHaltMode)(void *handle, const bool haltEnable);

    /// @brief This function checks if the drive halt in device is enabled
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[out] isHaltEnabled   isHaltEnabled Is drive halt enabled or not
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*getDriveHaltMode)(void *handle, bool& isHaltEnabled);

    /// @brief This function checks if the device has warning
    ///
    /// @param[in]  handle          Handle of the interface
    /// @return true if the device has warning, false otherwise
    bool(*deviceHasWarning)(void *handle);

    /// @brief This function checks if the device is ready to operate
    ///
    /// @param[in]  handle          Handle of the interface
    /// @return The acutal 'ready to operation'
    ::comm::axisprofile::ReadyToOperate(*checkReadyToOperate)(void *handle);

    void *reserved[100];  ///< reservation
  } IAxisProfileStatusControlC;

#ifdef __cplusplus
}
#endif
