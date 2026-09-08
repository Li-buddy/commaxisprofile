/// @file
///
/// @brief This file is part of the axisprofile library and implements
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
#include "axisprofile_c.h"
#include "axisprofile_if_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Function implementation (Wrapper for C-Interface)
// ---------------------------------------------------------------------------

extern "C" const IAxisProfileCommandC* AXISPROFILE_getCommandInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "command C-Interface");
  return axisprofileInterface->getCommandInterfaceC();
}

extern "C" const IAxisProfileStatusControlC* AXISPROFILE_getStatusControlInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "status/control C-Interface");
  return axisprofileInterface->getStatusControlInterfaceC();
}

extern "C" const IAxisProfileDiagMsgC* AXISPROFILE_getDiagMsgInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "diagnostic message C-Interface");
  return axisprofileInterface->getDiagMsgInterfaceC();
}

extern "C" const IAxisProfileCyclicDataC* AXISPROFILE_getCyclicDataInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "cyclic data C-Interface");
  return axisprofileInterface->getCyclicDataInterfaceC();
}

extern "C" const IAxisProfileBootingConfigC* AXISPROFILE_getBootingConfigInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "booting config C-Interface");
  return axisprofileInterface->getBootingConfigInterfaceC();
}

extern "C" const IAxisProfileDeviceParamC* AXISPROFILE_getDeviceParamInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "device parameter C-Interface");
  return axisprofileInterface->getDeviceParamInterfaceC();
}

extern "C" const IAxisProfileProbeC* AXISPROFILE_getProbeInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "probe C-Interface");
  return axisprofileInterface->getProbeInterfaceC();
}

extern "C" const IAxisProfileCmdC* AXISPROFILE_getCmdInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "command C-Interface");
  return axisprofileInterface->getCmdInterfaceC();
}

extern "C" const IAxisProfilePropertyC* AXISPROFILE_getPropertyInterfaceC(void* handle)
{
  const IAxisProfile* axisprofileInterface = static_cast<IAxisProfile*>(handle);
  RETURN_HANDLE_INVALIDE(axisprofileInterface, nullptr, "property C-Interface");
  return axisprofileInterface->getPropertyInterfaceC();
}
