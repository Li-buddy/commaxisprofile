#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and define the property handle.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"

/// @brief This method writes the given device address to the profile. Negative values and zero are not allowed!
/// @param[in]   handle        Handle of the interface
/// @param[in]   deviceAddress Address of the device.
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setDeviceAddressProperty(void* handle, std::uint32_t deviceAddress);

/// @brief This method returns the device address of the corresponding device.
/// @param[in]   handle        Handle of the interface
/// @return      Returns the device address.
extern "C" std::uint32_t AXISPROFILE_getDeviceAddressProperty(void* handle);

/// @brief This method returns the type of the profile.
/// @param[in]   handle        Handle of the interface
/// @return      Type of the profile
extern "C" comm::axisprofile::AxisProfileType AXISPROFILE_getProfileTypeProperty(void* handle);

/// @brief  This method gets the state of the device.
/// @param[in]   handle        Handle of the interface
/// @return      Return value indicating success or error.
extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getDeviceStateProperty(void* handle);

/// @brief This method gets the actual scaling type of the connected device for the given value type.
/// @param[in]   handle        Handle of the interface.
/// @param[in]   valueType     Defines the value type for the scaling type request.
/// @return      Returns the scaling type of the device.
extern "C" comm::axisprofile::AxisProfileDeviceScalingType AXISPROFILE_getDeviceScalingTypeProperty(void* handle, comm::axisprofile::AxisProfileDeviceValueType valueType);

/// @brief This method return category of current profile.
/// @param[in] handle  Handle of the interface.
/// @return The category of profile.
extern "C" comm::axisprofile::AxisProfileCategory AXISPROFILE_getProfileCategoryProperty(void* handle);

/// @brief This method return the fieldbus master name of current axisprofile.
/// @param[in] handle  Handle of the interface.
/// @return The name of fieldbus master instance configured for current axis profile.
extern "C" const char* AXISPROFILE_getCfgedFieldbusMasterName(void* handle);
