#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axis profile property.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This class is mockup of the axis profile property.
  class AxisProfilePropertyMockHandle
  {
  public: // methods
    AxisProfilePropertyMockHandle() = default;
    ~AxisProfilePropertyMockHandle() = default;

  public: // data
    std::uint32_t m_deviceAddress {};
    AxisProfileResult m_retValue = AxisProfileResult::STS_OK;                             ///< Return value of C-Methods
    AxisProfileType m_profileType = AxisProfileType::UNDEFINED;                           ///< Type of the profile
    AxisProfileDeviceScalingType m_deviceType = AxisProfileDeviceScalingType::UNDEFINED;  ///< Scaling type of device
    AxisProfileCategory m_profileCateogry = AxisProfileCategory::CATEGORY_DRIVE;          ///< axis profile category
  };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // -------------------------------------- C-Interface Method-Mockups ---------------------------------------- //

  /// @brief This method writes the given device address to the profile. Negative values and zero are not allowed!
  /// @param[in]   handle        Handle of the interface
  /// @param[in]   deviceAddress Address of the device.
  /// @return      Return value indicating success or error.
  extern "C" AxisProfileResult AXISPROFILE_setDeviceAddressPropertyMock(void* handle, std::uint32_t deviceAddress);

  /// @brief This method returns the device address of the corresponding device.
  /// @param[in]   handle        Handle of the interface
  /// @return      Returns the device address.
  extern "C" std::uint32_t AXISPROFILE_getDeviceAddressPropertyMock(void* handle);

  /// @brief This method returns the type of the profile.
  /// @param[in]   handle        Handle of the interface
  /// @return      Type of the profile
  extern "C" AxisProfileType AXISPROFILE_getProfileTypePropertyMock(void* handle);

  /// @brief  This method gets the state of the device.
  /// @param[in]   handle        Handle of the interface
  /// @return      Return value indicating success or error.
  extern "C" AxisProfileResult AXISPROFILE_getDeviceStatePropertyMock(void* handle);

  /// @brief This method gets the actual scaling type of the connected device for the given value type.
  /// @param[in]   handle        Handle of the interface.
  /// @param[in]   valueType     Defines the value type for the scaling type request.
  /// @return      Returns the scaling type of the device.
  extern "C" AxisProfileDeviceScalingType AXISPROFILE_getDeviceScalingTypePropertyMock(void* handle, comm::axisprofile::AxisProfileDeviceValueType valueType);

  /// @brief This method returns category of current profile.
  /// @param[in] handle  Handle of the interface.
  /// @return The category of profile.
  extern "C" AxisProfileCategory AXISPROFILE_getProfileCategoryPropertyMock(void* handle);
}
