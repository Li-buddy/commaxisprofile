#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the main
///        interface of all axis profiles. Due to the large number of methods, the
///        interface is split. This means it has sub-interfaces for different use cases,
///        which are available by calling the appropriate get-method.
///        New axisprofiles can be created and existing ones deleted by using the
///        central interface of this bundle 'IAxisProfileManager'.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/interfaces/i_axisprofile_command_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_status_control_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_diag_msg_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_cyclic_data_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_booting_config_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_device_param_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_probe_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_cmd_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_property_c.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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

  /// @brief This is the interface for the axisprofile.
  class IAxisProfile
  {
  public: // methods

    virtual ~IAxisProfile() noexcept = default;

    /// @brief This method gets the C-Interface for axisprofile commands.
    /// @return     Pointer to the command C-Interface.
    virtual const IAxisProfileCommandC* getCommandInterfaceC() const = 0;

    /// @brief This method gets the C-Interface for status- and controlword management.
    /// @return     Pointer to the status- and controlword C-Interface.
    virtual const IAxisProfileStatusControlC* getStatusControlInterfaceC() const = 0;

    /// @brief This method gets the C-Interface for diagnostic message management.
    /// @return     Pointer to the diagnostic message C-Interface.
    virtual const IAxisProfileDiagMsgC* getDiagMsgInterfaceC() const = 0;

    /// @brief This method gets the C-Interface for cyclic data management.
    /// @return     Pointer to the cyclic data C-Interface.
    virtual const IAxisProfileCyclicDataC* getCyclicDataInterfaceC() const = 0;

    /// @brief Deprecated. This method gets the C-Interface for  booting configuration interface
    /// @return     Pointer to the booting configuration C-Interface.
    virtual const IAxisProfileBootingConfigC* getBootingConfigInterfaceC() const = 0;

    /// @brief This method gets the C-Interface for device parameter access.
    /// @return     Pointer to the booting configuration C-Interface.
    virtual const IAxisProfileDeviceParamC* getDeviceParamInterfaceC() const = 0;

    /// @brief This method gets the C-Interface for command management.
    /// @return       Pointer to the command C-Interface.
    virtual const IAxisProfileCmdC* getCmdInterfaceC() const = 0;

    /// @brief This method writes the given device address to the profile. Negative values and zero are not allowed!
    /// @param[in]  deviceAddress  Address of the device.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult setDeviceAddress(std::uint32_t deviceAddress) = 0;

    /// @brief This method returns the device address of the corresponding device.
    /// @return     Returns the device address.
    virtual std::uint32_t getDeviceAddress() const = 0;

    /// @brief This method returns the type of the profile.
    /// @return     Type of the profile.
    virtual AxisProfileType getProfileType() const = 0;

    /// @brief  This method gets the state of the device.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult getDeviceState() = 0;

    /// @brief This method gets the actual scaling type of the connected device for the given value type
    /// @param[in]  valueType     Defines the value type for the scaling type request.
    /// @return     Returns the scaling type of the device
    virtual AxisProfileDeviceScalingType getDeviceScalingType( AxisProfileDeviceValueType valueType) const = 0;
          
    /// @brief This method writes the scaling configuration to the profile. 
    /// @param[in]    strategy  strategy of scaling
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult setProfileScalingStrategyCfg(const AxisProfileScalingStrategyCfg& strategy) = 0;

    /// @brief This method return the scaling configuration to the profile. 
    /// @return       Return profile scaling strategy configuration.
    virtual AxisProfileScalingStrategyCfg getProfileScalingStrategyCfg() const = 0;

    /// @brief This method gets the C-Interface for probe management.
    /// @return       Pointer to the probe C-Interface.
    virtual const IAxisProfileProbeC* getProbeInterfaceC() const = 0;
      
    /// @brief This method write the device name to the profile.
    /// @param[in]    deviceName  name of device
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult setDeviceName(const char* deviceName) = 0;

    /// @brief This method return the device name. 
    /// @return       Return device name.
    virtual const char* getDeviceName() const = 0;

    /// @brief This method return the profile category.
    /// @return       Return category of current profile. 
    virtual AxisProfileCategory getProfileCategory() = 0;

    /// @brief This method returns the property of the profile.
    /// @return     Property of the profile.
    virtual const IAxisProfilePropertyC* getPropertyInterfaceC() const = 0;
  };
}
