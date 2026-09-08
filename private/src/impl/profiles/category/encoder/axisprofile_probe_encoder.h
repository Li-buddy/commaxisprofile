#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and declare the class AxisProfileProbeEncoder.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_probe.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class ProfileTrace;

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

  /// @brief This is the base class for probe handling.
  class AxisProfileProbeEncoder : public AxisProfileProbe
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo     Reference to the Profile Basic Information
    explicit AxisProfileProbeEncoder(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~AxisProfileProbeEncoder() noexcept override = default;
    void loadBootingTasks() final;

    AxisProfileResult probeBaseConfig(ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool isAutoActivated) final;

    AxisProfileResult probeExtConfig(ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig, bool isStateMonitActivated) final;

    AxisProfileResult probeStatusCtrl(ProbeIndex probeIdx, bool enable) final;

    AxisProfileResult getProbeActualValuesRT(ProbeIndex probeIdx, ProbeActualValues* actualValueProbe)final;

    AxisProfileResult onlineChangeParams(ProbeIndex probeIdx, ProbeUpdateMeasurement updateValue) final;

    AxisProfileResult getProbeValues(ProbeIndex probeIdx, ProbeBaseConfig* probeConfig, ProbeExtConfig* probeExtendConfig) final;

    AxisProfileResult abortAndResetProbe() final;

    AxisProfileResult isProbeSupported() final;

  private:
    /// @brief Method to send diagnostic for rejected commands
    /// @param[in]  cmdNameInfo   The additive information of the diagnostic
    void rejectCmdDiagnosis(const char* cmdNameInfo) const;
  };
}
