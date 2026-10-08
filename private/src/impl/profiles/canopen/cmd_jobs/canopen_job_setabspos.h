#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines CanOpen job of set absolute position.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
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
class TestCanopenJobSetAbsPos;
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

  /// @brief This is class of job object of CANOpen set absolute position.
  class CanopenJobSetAbsPos : public CanopenJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileInfo    Reference to the Profile Basic Information
    /// @param[in] shareHandle    Pointer to the sharing handler
    explicit CanopenJobSetAbsPos(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~CanopenJobSetAbsPos() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() {return "CanopenJobSetAbsPos";}

    JobIDType getID() final {return jobName();}

  protected: // methods

    AxisProfileResult onSetArgument(IContainer* arg) final;
    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;
    AxisProfileResult onJobAborting() final;

  private: // methods

    /// @brief This method setup list that will read from device
    /// @return Return value indicating success or error.
    AxisProfileResult setupReadListFromDevice();

    /// @brief This method setup list that will write to device
    /// @return Return value indicating success or error.
    AxisProfileResult setupWriteListToDevice();

    /// @brief This method setup jobs that prepare set absolute position by device.
    void setupPrepareSetAbsPosJobs();

    /// @brief This method setup jobs that write configuration for set absolute position to device.
    void setupWriteConfigJobs();

    /// @brief This method setup jobs that activate set absolute position by device.
    void setupActiveSetAbsPosJobs();

    /// @brief This method is used to determine handle-based
    /// @return Return value indicating success or error.
    AxisProfileResult judgmentHandleBased();

    /// @brief This method is used to configure IO encoder data.
    /// @return Return value indicating success or error.
    AxisProfileResult setIoEncoderData();

    /// @brief This method is used to reset IO encoder data.
    /// @return Return value indicating success or error.
    AxisProfileResult resetIoEncoderData();

    /// @brief This method is used to set new status in device.
    /// @param[in]  status  The new status in device
    /// @return Return value indicating success or error.
    AxisProfileResult setCmdStatusInDevice(CmdStatusInDevice status);

  private: // data
    friend class::TestCanopenJobSetAbsPos;
    SetAbsPosData m_cmdData {};           ///< command date of set absolute position

    ObjectDirEntry m_newRefObj {};        ///< object of the new reference to be set to device
    std::int32_t m_newRefRaw {};          ///< raw value of the new reference to be set to device
    std::uint16_t m_ctrlWord {};          ///< value of 0x2510:02, control word of set absolute position
    WordStructure m_ctrlWordS448 {};      ///< value of S-0-0448 (0x2510:2), ctrlX drive only
    SignalFuncCmd m_signalFuncCmd {{FunctionSupport::SET_ABS_POS_CMD_CYCLIC}};    ///< signal function command for set absolute position
  };
}
