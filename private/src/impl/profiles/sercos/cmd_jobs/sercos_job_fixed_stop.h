#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the job of fixed stop command.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "sercos_job_base.h"
#include "profiles/sercos/sercos_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestSercosJobFixedStop;
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

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is class of job object of fixed stop.
  class SercosJobFixedStop : public SercosJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileInfo   Reference to the Profile Basic Information
    /// @param[in]  shareHandle   Pointer to the sharing handler
    explicit SercosJobFixedStop(ProfileBasicInfo& profileInfo,
                                IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~SercosJobFixedStop() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() {return "SercosJobFixedStop";}

    JobIDType getID() final {return jobName();}

  protected: // methods

    AxisProfileResult onSetArgument(IContainer* arg) final;
    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobAborting() final;

  private: // methods

    /// @brief This method validates configuration before setting absolute position.
    /// @return Return value indicating success or error.
    AxisProfileResult validateConfig();

  private: // data
    friend class::TestSercosJobFixedStop; 
    double m_clampingTorque {0.0};        ///< clamping torque for fixed stop
    std::uint16_t m_torqueRawS530 {0};    ///< clamping torque converted to raw value, S530
  };
}
