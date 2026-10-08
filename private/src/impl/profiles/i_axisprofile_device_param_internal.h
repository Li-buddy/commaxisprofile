#pragma once
/// @file
///
/// @brief This header file is part of the axisprofile library and define the
/// device parameter interface for all axisprofiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "bundle/i_axisprofile_device_param.h"
#include "profiles/i_axisprofile_sharing.h"
#include "profiles/profile_defs.h"
#include "profiles/i_axisprofile_state_switch.h"
#include "signal_func/signal_func_handler.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct IAxisProfileBootingConfigC;
struct IAxisProfileDeviceParamC;

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

  /// @brief This is the internal device parameter interface for all axisprofiles.
  class IAxisProfileDeviceParamInternal : public IAxisProfileDeviceParam, public IAxisProfileSharing, public IAxisProfileStateSwitch
  {
  public: // methods

    /// @brief destructor
    virtual ~IAxisProfileDeviceParamInternal() noexcept override = default;

    /// @brief Deprecated, This method gets the status/control C-Interface.
    /// @return      Return Pointer to the status/control C-Interface.
    virtual const IAxisProfileBootingConfigC* getDeprecatedCInterface() const = 0;

    /// @brief This method gets the status/control C-Interface.
    /// @return      Return Pointer to the status/control C-Interface.
    virtual const IAxisProfileDeviceParamC* getCInterface() const = 0;

    /// @brief This method gets operation mode available information
    /// @return     Return operation mode available information.
    virtual OpModeAvailInfo* getOpModeAvailInfo() = 0;

    /// @brief This method get device configuration data.
    /// @return Return the device configuration data.
    virtual DeviceCfgData& getDeviceCfgData() = 0;

    /// @brief This method get whether the device support probe latched times
    /// @return     Returns whether the device support this.
    virtual bool isLatchParamSupport() const = 0;
    
    /// @brief Read the device's checksum idn list. return empty
    /// @param[out]    params        The checksum idn list, assign empty if device not support checksum function.
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult getChecksumIdnList(std::vector<std::string>& idnStrListRet) const = 0;

    /// @brief Set the homing status of axis. The homing status is used to determine whether the axis is homed or not.
    ///        The homing status can be got from signal status word if the function is supported by device and realtime data mapped,
    ///        otherwise it can be got by reading specific parameter from device.
    ///        For some devices which cannot get homing status by above methods, the homing status can only be set by this method, and the value should be got from other ways by caller.
    /// 
    /// @param[in]  homed             The new homed state of the axis, true for homed, false for not homed.
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setAxisHomed(bool homed) = 0;

    /// @brief This method gets the signal function handler for the drive.
    /// @return return the pointer to the signal function handler for the drive.
    virtual SignalFuncHandler* getSignalFuncHandler() = 0;
  };
}
