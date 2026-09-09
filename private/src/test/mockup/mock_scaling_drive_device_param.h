#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements a mockup class for drive scaling tests.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/category/drive/axisprofile_device_param_drive.h"

#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
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

  /// @brief Mock device parameter implementation shared by drive scaling tests.
  class MockScalingDriveDeviceParam : public ::comm::axisprofile::AxisProfileDeviceParamDrive
  {
  public: // methods
    /// @brief Constructor.
    /// @param[in] profileBasicInfo Profile context used by the drive base class.
    explicit MockScalingDriveDeviceParam(::comm::axisprofile::ProfileBasicInfo& profileBasicInfo)
      : AxisProfileDeviceParamDrive(profileBasicInfo, nullptr, nullptr)
    {
    }

    /// @brief Destructor.
    ~MockScalingDriveDeviceParam() noexcept override = default;

    ::comm::axisprofile::OpModeAvailInfo* getOpModeAvailInfo() override { return nullptr; }
    bool checkFunctionSupported(const ::comm::axisprofile::FunctionSupport, ::comm::axisprofile::FunctionSupportErrorInfo&) override { return false; }
    ::comm::axisprofile::AxisProfileResult checkStandstill(bool& standStill) override { standStill = true; return ::comm::axisprofile::AxisProfileResult::STS_OK; }
    ::comm::axisprofile::AxisProfileResult getChecksumIdnList(std::vector<std::string>&) const override { return ::comm::axisprofile::AxisProfileResult::STS_OK; }
    void loadBootingTasks() override {}
    ::comm::axisprofile::AxisProfileResult getDeviceInformation() override { return ::comm::axisprofile::AxisProfileResult::STS_OK; }
    bool checkBootIdnListModified() override { return false; }
    ::comm::axisprofile::AxisProfileResult readChecksumValueFromDevice(std::uint32_t& checkSum) override { checkSum = 0; return ::comm::axisprofile::AxisProfileResult::STS_OK; }
  };
}
