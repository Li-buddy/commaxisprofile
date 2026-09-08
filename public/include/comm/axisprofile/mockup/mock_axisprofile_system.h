#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axis profile system interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_system.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This class implements a mockup for the axis profile system interface.
  class MockAxisProfileSystem : public IAxisProfileSystem
  {
  public: // methods

    /// @brief Constructor
    MockAxisProfileSystem() = default;
    /// @brief Destructor
    virtual ~MockAxisProfileSystem() override = default;

    const char *getAxisProfileState() const override { return m_systemState.c_str(); }
    AxisProfileResult requestAxisProfileState(const char * /*stateName*/) override { return m_returnValue; }
    AxisProfileResult getAxisProfileStateNames(std::vector<std::string> &/*stateNames*/) const override { return m_returnValue; }
    double getMaxBootingTimeout() const override { return m_bootingTimeout; }

  public: // data

    AxisProfileResult m_returnValue = AxisProfileResult::STS_OK;   ///< Return value of member functions
    std::string m_systemState = "Running";                         ///< State that is returned by getAxisProfileState()
    double m_bootingTimeout = 23.0;                                ///< Return value of booting timeout
  };
}
