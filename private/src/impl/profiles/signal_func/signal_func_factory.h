#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and creation of signal function handler
/// for different profiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "signal_func_handler.h"
#include "profiles/profile_defs.h"
#include "profiles/sercos/drive/sercos_signal_func_handler.h"
#include "profiles/canopen/drive/canopen_signal_func_handler.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  class SignalFuncHandlerFactory
  {
  public:
    virtual ~SignalFuncHandlerFactory() noexcept = default;

    /// @brief create signal function handler for different profiles.
    /// @param[in] profileBasicInfo the profile basic information for the signal function handler creation.
    /// @return pointer to the created SignalFuncHandler, or nullptr if the profile type is not supported.
    static SignalFuncHandler* createSignalFuncHandler(ProfileBasicInfo& profileBasicInfo)
    {
      SignalFuncHandler* pHandler {};
      switch (profileBasicInfo.m_axisProfileType)
      {
        case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
        {
          pHandler = new sercos::SercosSignalFuncHandler(profileBasicInfo);
        }
        break;
        case AxisProfileType::CANOPEN_OVER_ETHERCAT:
        {
          pHandler = new canopen::CanopenSignalFuncHandler(profileBasicInfo);
        }
        break;
        default:
          break;
      }
      return pHandler;
    }
  };
}
