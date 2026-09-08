#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements the
/// base class for probe handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_command_statemachine.h"
#include "axisprofile_internal_defs.h"
#include "comm/axisprofile/interfaces/i_axisprofile_probe_c.h"
#include "data_sharing/i_sharing_handler.h"
#include "i_axisprofile_probe_internal.h"
#include <vector>

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
  class AxisProfileProbe : public IAxisProfileProbeInternal
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo     Reference to the Profile Basic Information
    explicit AxisProfileProbe(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    virtual ~AxisProfileProbe() noexcept override = default;

    /// @brief This method gets the diagnostic message C-Interface.
    /// @return         Pointer to the diagnostic message C-Interface.
    const IAxisProfileProbeC* getCInterface() const final;

    AxisProfileResult booting() final;

  protected: // method
    
    void setSharingHandler(IAxisProfileSharingHandler* handler) final;
    
  protected: // data

    ProfileBasicInfo&           m_profileBasicInfo;                  ///< Profile basic information
    IAxisProfileSharingHandler* m_sharingHandler{};                  ///< Pointer to the sharing handler
    std::vector<std::function<AxisProfileResult()>> m_initCommands;  ///< Vector contains all init methods

  private: // data

    IAxisProfileProbeC m_cInterface{};                   ///< C-Interface for external usage
  };
}
