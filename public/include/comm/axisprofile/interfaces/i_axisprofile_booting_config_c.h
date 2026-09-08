#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface for motion bundles for booting parameterization.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 -2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

///  @par List of available functions in this part of the axisprofile booting configuration.
/// <TABLE>
/// <TR><TH>           Function                                 </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> checkBrakeDelaySupport                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getErrorBrakeDelayInfo                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getErrorBrakeDynLimit                    </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getPositionDataFormat                    </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkAndSetModulo                        </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkFunctionSupported                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// </TABLE>

#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
  /// @brief Deprecated by @IAxisProfileDeviceParamC. This is the C-Interface for the axisprofile booting config 'IAxisProfileDeviceParam'.
  typedef struct IAxisProfileBootingConfigC
  {
    /// @brief This is pointer to the instantiated object
    void *handle;

    /// @brief This function checks if the device supports error reaction of brake delay
    ///
    /// @param[in]  handle          Handle of the interface
    /// @return                     TRUE indicates that the device supports brake delay, FALSE indicates not support.
    bool(*checkBrakeDelaySupport)(void* handle);

    /// @brief This function gets brake delay information of raction of error 
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  errorType       Error type selection
    /// @param[out] isActivated     TRUE indicates that brake delay is activated, FALSE indicates not activated
    /// @param[out] delayTime       The brake delay of the selected error
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*getErrorBrakeDelayInfo)(void* handle, const ::comm::axisprofile::AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTime);

    /// @brief This function gets the dynamic limit of the reaction of specific error
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  errorType       Error type selection
    /// @param[out] deceleration    The deceleration of reaction of the selected error
    /// @param[out] jerk            The jerk of reaction of the selected error
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*getErrorBrakeDynLimit)(void* handle, const ::comm::axisprofile::AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk);

    /// @brief This function get data format of position
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[out] posDataFormat   Data format of position
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*getPositionDataFormat)(void* handle, ::comm::axisprofile::AxisProfilePosDataFormat& posDataFormat);

    /// @brief This function check and set modulo value of the device
    ///
    /// @note  When profile link to a device that support build-in modulo conversion, this function checks the consistency between device modulo value with input argument @ref modValue.
    ///        When profile link to a device that not support build-in modulo conversion, profile handles modulo conversion and argument @ref modValue effect immediately when function execute successfully.
    ///        This function can only be called during bootup process, the validation on input modValue range is in motion.core library,it should be always valid here.
    /// 
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  modValue        Modulo value that to be checked (when driver has build modulo conversion) or set (when axisprofile handles modulo conversion)
    /// @param[out] dynErrTxt       Dynamic error text when error returns, used for diagnosis of the caller
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*checkAndSetModulo)(void* handle, double modValue, std::string& dynErrTxt);

    /// @brief This function checks if the functionality is supported or not.
    /// @param[in]  handle     Handle of the interface.
    /// @param[in]  function   The function to be checked.
    /// @param[out] errorInfo  Dynamic error information when function is not supported, used for diagnosis of the caller.
    /// @return                 TRUE indicates that the function is supported, FALSE indicates not support.
    bool(*checkFunctionSupported)(void* handle, const ::comm::axisprofile::FunctionSupport function, ::comm::axisprofile::FunctionSupportErrorInfo& errorInfo);

    /// @brief This function getting distribution clock sync 0 shift time
    void* depricatedGetDCSync0ShiftTime;

    void *reserved[100];  /// reservation
  } IAxisProfileBootingConfigC;

#ifdef __cplusplus
}
#endif
