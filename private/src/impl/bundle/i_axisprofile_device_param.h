#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the main
///        interface for read the parameters access
///        This is a sub-interface of 'IAxisProfile'.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
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
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for device parameter access.
  class IAxisProfileDeviceParam
  {
    public: // methods

    /// @brief Destructor
    virtual ~IAxisProfileDeviceParam() noexcept = default;

    /// @brief This method get whether the device support brake delay function
    /// @return     Returns whether the device support this function.
    virtual bool checkBrakeDelaySupport() const = 0;

    /// @brief This method get the delay information of multi brake.
    /// @param[in]  errorType    Define the error type to get the reference delay information.
    /// @param[out] isActivated  Get if the delay activated.
    /// @param[out] delayTime    Get the delay time.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult getErrorBrakeDelayInfo(const AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTime) = 0;

    /// @brief This method get the deceleration information of the multi brake.
    /// @param[in]  errorType    Define the error type to get the reference brake information.
    /// @param[out] deceleration Deceleration of the brake.
    /// @param[out] jerk         Jerk of the brake.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult getErrorBrakeDynLimit(const AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk) = 0;

    /// @brief This method used to get position data format from drive side.
    /// @param[out] posDataFormat The current format of position(absolute or modulo).
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult getPositionDataFormat(AxisProfilePosDataFormat& posDataFormat) = 0;

    /// @brief This method check and set modulo value of the device
    /// @param[in]  modValue        Modulo value that to be checked (when driver has build modulo conversion) or set (when axisprofile handles modulo conversion)
    /// @param[out] dynErrTxt       Dynamic error text when error returns, used for diagnosis of the caller
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult checkAndSetModulo(double modValue, std::string& dynErrTxt) = 0;

    /// @brief This function checks if the functionality is supported or not.
    /// @param[in]  function   The function to be checked.
    /// @param[out] errorInfo  Dynamic error information when function is not supported, used for diagnosis of the caller.
    /// @return                 TRUE indicates that the function is supported, FALSE indicates not support.
    virtual bool checkFunctionSupported(const FunctionSupport function, FunctionSupportErrorInfo& errorInfo) = 0;

    /// @brief This function getting distribution clock's sync 0 shift time
    /// @param[in]  handle    Handle of the interface.
    /// @param[out] sync0     Sync0 shift time of distribution clock in second
    /// @return Distribution clock's sync0 shift time in unit of second.
    virtual AxisProfileResult getDCSync0ShiftTime(double& sync0) = 0;

    /// @brief This function getting delay time between slave latch the output and slave use it in loop control.
    ///
    /// @note In ctrlx drive, the output (command) is push into buffer when it is latched from fieldbus stack of slave.
    ///       A cubic approximation/interpolation is taken on the values in buffer and position loop using its result.
    ///       A linear interpolation may also be taken depending on the configuration (P-0-0187) of slave.
    /// 
    /// @param[in]  handle    Handle of the interface.
    /// @param[out] delay     Delay time in second
    /// @return Delay time between slave latch the output and slave use it in loop control
    virtual AxisProfileResult getOutputEffectiveDelay(double& delay) = 0;

    /// @brief This function checks if the axis is standstill
    /// @param[out] standStill TRUE indicates that the axis is standstill, FALSE indicates not standstill.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult checkStandstill(bool& standStill) = 0;

    /// @brief This method gets rate torque/force of the load in IS unit
    /// @param[out] rateTorqueForce       Rate torque for rotary axis or rate force for linear axis in IS unit
    /// @param[out] dynErrTxt             Dynamic error text when error returns, used for diagnosis of the caller
    /// @return                 Status of the function call: STS_OK or an error code on
    virtual AxisProfileResult getRateTorqueForce(double& rateTorqueForce, std::string& dynErrTxt) = 0;

    /// @brief This function checks axis has homed or not.
    /// @param[out] homed                 True indicates that the axis has homed, false indicates that the axis has not homed.
    /// @return                           Status of the function call: STS_OK or an error code
    virtual AxisProfileResult checkAxisHomed(bool& homed) = 0;
  };
}
