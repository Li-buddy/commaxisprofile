#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the main
///        interface for init, enable, execute, abort and reset probe.
///        This is a sub-interface of 'IAxisProfile'.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022 - 2025 Bosch Rexroth AG
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

  /// @brief This is the interface for all stages of probe.
  class IAxisProfileProbe
  {
  public: // methods

    virtual ~IAxisProfileProbe() noexcept = default;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ------------------------------------------------ init -------------------------------------------------- //
      
    /// @brief This method set the probe control word for initial configuration.
    /// @param[in]  probeIdx               Indicates the probe number.
    /// @param[in]  probeConfig            Probe control word configuration.
    /// @param[in]  isAutoActivated        Indicates if probe automatic activated after configuration.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult probeBaseConfig(ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool isAutoActivated) = 0;
      
    /// @brief This method set the probe extended control word for initial configuration.
    /// @param[in]  probeIdx               Indicates the probe number.
    /// @param[in]  probeExtendConfig      Probe extended control word configuration.
    /// @param[in]  isStateMonitActivated  Indicates if state monit activate.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult probeExtConfig(ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig, bool isStateMonitActivated) = 0;
      
    /// @brief This method controls the enable/disable status of probe measurement
    /// @param[in]  probeIdx               Indicates the probe number.
    /// @param[in]  enable                 Is probe enabled.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult probeStatusCtrl(ProbeIndex probeIdx, bool enable) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // --------------------------------------------- Execute ------------------------------------------------- //
    // ------------------------------------------------ AT --------------------------------------------------- //

    /// @brief This method get the probe actual measured values.
    /// @param[in]  probeIdx               Indicates the probe number.
    /// @param[out] actualValueProbe       The actual measured values of probe.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult getProbeActualValuesRT(ProbeIndex probeIdx, ProbeActualValues* actualValueProbe) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ---------------------------------------- Get/Update Values -------------------------------------------- //

    /// @brief This method online update and changes the measurement.
    /// @param[in]  probeIdx               Indicates the probe number.
    /// @param[in]  updateValue            Online update the measurement configuration.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult onlineChangeParams(ProbeIndex probeIdx, ProbeUpdateMeasurement updateValue) = 0;
      
    /// @brief This method get probe acutal configuration.
    /// @param[in]  probeIdx               Indicates the probe number.
    /// @param[out] probeConfig            Probe control word configuration.
    /// @param[out] probeExtendConfig      Probe extended control word configuration.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult getProbeValues(ProbeIndex probeIdx, ProbeBaseConfig* probeConfig, ProbeExtConfig* probeExtendConfig) = 0;

    /// @brief This method check if probe supports on current device.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult isProbeSupported() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // --------------------------------------------- Abort/Reset -------------------------------------------- //
      
    /// @brief This method abort and reset probe.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult abortAndResetProbe() = 0;
  };
}
