#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines CanOpen homing job.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_base.h"
#include "profiles/signal_func/signal_func_cmd.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenJobHoming;
// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
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

  /// @brief This is class of job object of CANOpen homing.
  class CanopenJobHoming : public CanopenJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileInfo   Pointer to the Profile Basic Information
    /// @param[in]  shareHandle   Pointer to the sharing handler
    explicit CanopenJobHoming(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle);

    /// @brief Destructor
    virtual ~CanopenJobHoming() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() { return "CanopenJobHoming"; }

    JobIDType getID() override { return jobName(); }

  protected: // methods

    AxisProfileResult onSetArgument(IContainer* arg) final;
    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;
    AxisProfileResult onJobAborting() final;
    AxisProfileResult onUpdateStatusInDevice() final;

  private: // methods

    /// @brief This method setup list that will read from device
    /// @return Return value indicating success or error.
    AxisProfileResult setupReadListFromDevice();

    /// @brief This method setup list that will write to device
    /// @return Return value indicating success or error.
    AxisProfileResult setupWriteListToDevice();

    /// @brief This method setup jobs that activate homing by device.
    void setupHomingByDeviceJobs();

    /// @brief This method setup jobs that clear reference point in device.
    void setupClearReferencePointJobs();

    /// @brief This method setup jobs that start homing by device.
    void setupActiveHomingByDeviceJobs();

    /// @brief This method switch and check operation mode
    /// @param[in]  toHome    True if switch to home mode, false if switch to position mode
    /// @return Return value indicating success or error.
    AxisProfileResult switchAndCheckOpMode(AxisProfileOperationMode mode);

    /// @brief This method start homing procedure
    /// @return Return value indicating success or error.
    AxisProfileResult startHomingProcedure();

    /// @brief This method check homing
    /// @return Return value indicating success or error.
    AxisProfileResult checkHomingProcedure();

    /// @brief This method abort homing procedure
    /// @return Return value indicating success or error.
    AxisProfileResult abortHomingProcedure();

    /// @brief This method activates homing by profile (control)
    /// @return Return value indicating success or error.
    AxisProfileResult activeHomingByProfile();

    /// @brief This method set homing state of the axisprofile.
    /// @return Return value indicating success or error.
    AxisProfileResult setHomingState(bool state);

    /// @brief This method is used to set new status in device.
    /// @param[in]  status  The new status in device
    /// @return Return value indicating success or error.
    AxisProfileResult setCmdStatusInDevice(CmdStatusInDevice status);

  private: // data
    friend class::TestCanopenJobHoming;
    HomingData m_cmdData {};                ///< command input data from motion
    std::int32_t m_cmdPosRaw {};            ///< reference position in raw that to be set when homed
    std::int32_t m_posOffsetRaw {};         ///< reference offset in raw that to be set
    std::int8_t m_homingAtCurrentPos {37};  ///< method of homing on current position (without movement)
    WordStructure m_ctrlWordS448 {};        ///< value of S-0-0448 (0x2510:2), ctrlX drive only

    SignalFuncCmd m_signalFuncCmd {{FunctionSupport::HOMING_CMD_CYCLIC}};    ///< signal function command for homing
  };
}
