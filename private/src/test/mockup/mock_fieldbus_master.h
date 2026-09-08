#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implement mock class for IFieldbusMaster.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "fieldbuses/i_fieldbus_master.h"

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

  /// @brief This is mockup class of fieldbus master object.
  class MockFieldbusMaster
  {
  public: // methods

    /// @brief constructor
    MockFieldbusMaster() = default;
    /// @brief destructor
    virtual ~MockFieldbusMaster() noexcept override = default;

    const char* getMasterName() override { return m_masterName.c_str(); };
    const std::set<comm::axisprofile::IAxisProfileInternal*>& getLinkedProfiles() override { return m_profiles; };
    void addLinkedProfile(comm::axisprofile::IAxisProfileInternal* profile) override { m_profiles.insert(profile); };
    comm::axisprofile::AxisProfileResult initRtData(bool abort) override { return m_retValue; };
    comm::axisprofile::AxisProfileResult ProcessRtData() override { return m_retValue; };
    double getActDcBaseTime() override { m_actDcBaseTime; };
    double getFieldbusCycleTime() override { m_fieldbusCycleTime; };

  public: // data

    std::string m_masterName {"mockMaster"};  ///< Name of fieldbus master
    double m_actDcBaseTime {0.0};             ///< Actual distributed clock base time
    double m_fieldbusCycleTime {0.0};         ///< Fieldbus cycle time
    std::set<comm::axisprofile::IAxisProfileInternal*> m_profiles;  ///< Profiles lined to this master
    comm::axisprofile::AxisProfileResult m_retValue {comm::axisprofile::AxisProfileResult::STS_OK}; ///< return value
  };
}
