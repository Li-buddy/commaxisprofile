#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the Sercos homing job.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "sercos_job_base.h"
#include "profiles/signal_func/signal_func_cmd.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestSercosJobHoming; 

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
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

  /// @brief Struct homing related data that will be used in homing process
  struct HomingDevData
  {
    std::uint16_t idnRefDist;         ///< idn of reference position
    std::uint16_t idnRefOffset;       ///< idn of reference offset
    WordStructure ctrlWordS448 {};    ///< value of S-0-0448, control word of set absolute position
    WordStructure ctrlWordS147 {};    ///< value of S-0-0147, homing parameter set in the device
    std::int32_t cmdPosRaw {};        ///< reference position in raw that to be set when homed
    std::int32_t offsetPosRaw {};     ///< reference offset in raw that to be set
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is class of job object of Sercos homing.
  class SercosJobHoming : public SercosJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileInfo   Reference to the Profile Basic Information
    /// @param[in]  shareHandle   Pointer to the sharing handler
    explicit SercosJobHoming(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~SercosJobHoming() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() { return "SercosJobHoming"; }

    JobIDType getID() final { return jobName(); }

  protected: // methods

    AxisProfileResult onSetArgument(IContainer* arg) final;
    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;
    AxisProfileResult onJobAborting() final;

  private: // methods

    /// @brief This method read homing related parameters from device
    /// @return Return value indicating success or error.
    AxisProfileResult setupReadListFromDevice();

    /// @brief This method read homing related parameters from device
    /// @return Return value indicating success or error.
    AxisProfileResult setupWriteListToDevice();

    /// @brief This method validates homing is support
    /// @return Return value indicating success or error.
    AxisProfileResult validateHomingSupport();

    /// @brief This method clear reference point in device.
    /// @return Return value indicating success or error.
    AxisProfileResult clearReferencePoint();

    /// @brief This method write new reference position and offset to device.
    ///        The new position and offset has be writes to device before homing.
    /// @return Return value indicating success or error.
    AxisProfileResult exeParamListAccess();

    /// @brief This method activates homing by device (drive)
    /// @return Return value indicating success or error.
    AxisProfileResult activeHomingByDevice();

    /// @brief This method activates homing by profile (control)
    /// @return Return value indicating success or error.
    AxisProfileResult activeHomingByProfile();

    /// @brief This method aborts homing by device (drive)
    /// @return Return value indicating success or error.
    AxisProfileResult abortHomingByDevice();

  private: // data
    friend class::TestSercosJobHoming;

    HomingData m_cmdData {};      ///< command input data from motion
    HomingDevData m_devData {};   ///< device related data that used during homing process

    std::list<SercosParamStruct> m_paramList {}; ///< parameter list to be access asynchronously

    SignalFuncCmd m_signalFuncCmd {{FunctionSupport::HOMING_CMD_CYCLIC}};    ///< signal function command for homing
  };
}
