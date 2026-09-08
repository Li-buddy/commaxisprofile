#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the main
///        interface concerning everything that refers to the statusword or controlword
///        of the axisprofile. This is a sub-interface of 'IAxisProfile'.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
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

  /// @brief This is the interface for status- and controlword management of the axisprofile.
  class IAxisProfileStatusControl
  {
  public: // methods

    virtual ~IAxisProfileStatusControl() noexcept = default;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // --------------------------------------------- Statusword ----------------------------------------------- //

    /// @brief  This method gets the status word of the device.
    ///
    /// @return      Return value indicating success or error.
    virtual std::uint16_t getStatusWord() const = 0;

    /// @brief  This method checks if the given operation mode equals the operation mode of the device.
    /// @param[in]      opMode       Operation mode which will be checked
    ///
    /// @return         Return value indicating if given operation mode is set in the device (true) or not (false).
    virtual bool checkOperationMode(const AxisProfileOperationMode opMode) const = 0;

    /// @brief This method checks if the given operation mode is supported in the operation mode of the device.
    /// @param[in]      opMode       Operation mode which will be checked
    /// @param[in]      errorInfo    Error information while return false
    ///
    /// @return         Return value indicating if checked operation mode is supported in the device (true) or not (false).
    virtual bool checkOperationModeSupported(const AxisProfileOperationMode opMode, OpModeErrorInfo& errorInfo) = 0;

    /// @brief  This method gets the actual operation mode of the device.
    ///
    /// @return         Return value is the actual operation mode.   
    virtual AxisProfileOperationMode getOperationMode() const = 0;

    /// @brief  This method checks if the given power mode equals the power mode of the device.
    /// @param[in]      powerMode    Power mode which will be checked
    ///
    /// @return         Return value indicating if given power mode is set in the device (true) or not (false).
    virtual bool checkPowerMode(const AxisProfilePowerMode powerMode) const = 0;

    /// @brief  This method gets the actual power mode of the device.
    ///
    /// @return         Return value is the actual power mode. 
    virtual AxisProfilePowerMode getPowerMode() const = 0;

    /// @brief This methods checks if the device is in error state.
    ///
    /// @return     Return value indicating whether the device is in error (true) or not (false).
    virtual bool checkError() const = 0;

    /// @brief This methods checks if the device is in standstill state. 
    /// This method is correctly functional, ONLY IF activateCyclicStandStillCheck() declared in i_axisprofile_command.h is called earlier. 
    /// @return     Return value indicating whether the device is in standstill (true) or not (false).
    virtual bool cyclicStandStillCheck() const = 0;

    /// @brief This method check if the device follows control.
    /// @return     Return whether the device follows the control to set point.
    virtual bool isDeviceFollowsControl() const = 0;

    /// @brief This method check if the device has warning.
    /// @return     Return whether the device currently has warning or not.
    virtual bool deviceHasWarning() const = 0;

    /// @brief This method check if the device is ready to operate.
    /// @return     Return if the device is ready to operate.
    virtual ReadyToOperate checkReadyToOperate() = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // -------------------------------------------- Controlword ----------------------------------------------- //

    /// @brief  This method sets the control word of the device.
    ///
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setControlWord(std::uint16_t controlWord) = 0;

    /// @brief  This method sets the given operation mode to the device.
    /// @param[in]      opMode       Operation mode which will be set
    ///
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setOperationMode(const AxisProfileOperationMode opMode) = 0;

    /// @brief  This method sets the given power mode to the device.
    /// @param[in]      powerMode    Power mode which will be set
    ///
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setPowerMode(const AxisProfilePowerMode powerMode) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // --------------------------------------------- Drive Halt ----------------------------------------------- //
      
    /// @brief  This method sets the given drive halt mode to device true for enable false for disable.
    /// @param[in]      haltEnable   Enable or disable drive halt
    ///
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setDriveHaltMode(const bool haltEnable) = 0;

    /// @brief  This method checks if the drive halt in device is enabled. 
    /// @param[out]     isHaltEnabled Is drive halt enabled or not
    ///
    /// @return      Return whether the device is in drive halt state.
    virtual AxisProfileResult getDriveHaltMode(bool& isHaltEnabled) const = 0;

    /// @brief  This method gets the status word of the device.
    /// @return      Return value indicating success or error.
    virtual std::uint16_t getControlWord() const = 0;
  };
}
