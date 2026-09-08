#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines
///        C/C++ method mapping for the base class of all axis profiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_axisprofile.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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

/// @brief This method gets the interface for axisprofile commands.
/// @param[in]   handle        Handle of the interface
/// @return      Pointer to the command interface.
extern "C" const IAxisProfileCommandC* AXISPROFILE_getCommandInterfaceC(void* handle);

/// @brief This method gets the interface for status- and controlword management.
/// @param[in]   handle        Handle of the interface
/// @return      Pointer to the status- and controlword interface.
extern "C" const IAxisProfileStatusControlC* AXISPROFILE_getStatusControlInterfaceC(void* handle);

/// @brief This method gets the interface for diagnostic message management.
/// @param[in]   handle        Handle of the interface
/// @return      Pointer to the diagnostic message interface.
extern "C" const IAxisProfileDiagMsgC* AXISPROFILE_getDiagMsgInterfaceC(void* handle);

/// @brief This method gets the interface for cyclic data management.
/// @param[in]   handle        Handle of the interface
/// @return      Pointer to the cyclic data interface.
extern "C" const IAxisProfileCyclicDataC* AXISPROFILE_getCyclicDataInterfaceC(void* handle);

/// @brief Deprecated. This method gets the interface for booting configuration management.
/// @param[in]   handle        Handle of the interface
/// @return      Pointer to the booting config interface.
extern "C" const IAxisProfileBootingConfigC* AXISPROFILE_getBootingConfigInterfaceC(void* handle);

/// @brief This method gets the interface for device parameter access.
/// @param[in]   handle        Handle of the interface
/// @return      Pointer to the device parameter access interface.
extern "C" const IAxisProfileDeviceParamC* AXISPROFILE_getDeviceParamInterfaceC(void* handle);

/// @brief This method gets the interface for probe management.
/// @param[in]   handle         Handle of the interface
/// @return      Pointer to the probe interface.
extern "C" const IAxisProfileProbeC* AXISPROFILE_getProbeInterfaceC(void* handle);

/// @brief This method gets the interface for axisprofile command.
/// @param[in]   handle        Handle of the interface
/// @return      Pointer to the command interface.
extern "C" const IAxisProfileCmdC* AXISPROFILE_getCmdInterfaceC(void* handle);

/// @brief This method returns the property of the profile.
/// @param[in]   handle        Handle of the interface
/// @return      Property of the profile
extern "C" const IAxisProfilePropertyC* AXISPROFILE_getPropertyInterfaceC(void* handle);
