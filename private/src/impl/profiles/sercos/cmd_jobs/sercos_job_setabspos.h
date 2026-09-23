#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines Sercos job of set absolute position.
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

  /// @brief This is class of job object of Sercos set absolute position.
  class SercosJobSetAbsPos : public SercosJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileInfo    Reference to the Profile Basic Information
    /// @param[in] shareHandle    Pointer to the sharing handler
    explicit SercosJobSetAbsPos(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~SercosJobSetAbsPos() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() { return "SercosJobSetAbsPos"; }

    JobIDType getID() final { return jobName(); }

  protected: // methods

    AxisProfileResult onSetArgument(IContainer* arg) final;
    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;
    AxisProfileResult onJobAborting() final;

  private: // methods

    /// @brief This method validates configuration before setting absolute position.
    /// @return Return value indicating success or error.
    AxisProfileResult validateConfig();

    /// @brief This method writes configuration to device.
    /// @return Return value indicating success or error.
    AxisProfileResult writeConfigToDevice();

    /// @brief This method active set absolute position by device
    /// @return Return value indicating success or error.
    AxisProfileResult activeSetAbsPosByDevice();

    /// @brief This method abort set absolute position by device
    /// @return Return value indicating success or error.
    AxisProfileResult abortSetAbsPosByDevice();

    /// @brief This method is used to determine handle-based
    /// @return Return value indicating success or error.
    AxisProfileResult judgmentHandleBased();

    /// @brief This method is used to configure IO encoder data.
    /// @return Return value indicating success or error.
    AxisProfileResult setIoEncoderData();

    /// @brief This method is used to reset IO encoder data.
    /// @return Return value indicating success or error.
    AxisProfileResult resetIoEncoderData();

  private: // data

    double m_cmdPos {};                   ///< position in IS that to be set to device
    AxisProfileEncoder m_encoder {};      ///< encoder selector
    bool m_allowPowerEnabled {};          ///< allow power on after set absolute position
    telegramMDT tMDT {};                  ///< telegram MDT

    std::uint16_t m_newRefIdn {};         ///< IDN of the new reference to be set to device
    std::int32_t m_newRefRaw {};          ///< raw value of the new reference to be set to device
    std::uint16_t m_ctrlWordS448 {};      ///< value of S-0-0448, control word of set absolute position

    std::list<SercosParamStruct> m_paramList {}; ///< parameter list to be access asynchronously
  };
}
