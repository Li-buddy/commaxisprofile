#pragma once
/// @file
///
/// This header file is part of the axisprofile library and implements
/// base class for status and controlword handling.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_status_control_c.h"
#include "data_sharing/i_sharing_handler.h"
#include "i_axisprofile_status_control_internal.h"

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

  /// @brief This is the base class for status- and controlword handling.
  class AxisProfileStatusControl : public IAxisProfileStatusControlInternal
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Reference to the Profile Basic Information
    explicit AxisProfileStatusControl(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    virtual ~AxisProfileStatusControl() noexcept override = default;

    const IAxisProfileStatusControlC* getCInterface() const final;
    AxisProfileResult booting() final;    
    void confirmOperationModeFromDevice(AxisProfileOperationMode operationMode) override;

  protected: // methods
      
    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

  protected: // data

    std::vector<std::function<AxisProfileResult()>> m_initStatusControl;      ///< Vector contains all init methods
    ProfileBasicInfo& m_profileBasicInfo;                                     ///< Profile basic information
    IAxisProfileSharingHandler* m_sharingHandler {};                          ///< Pointer to the sharing handler

  private: // data

    IAxisProfileStatusControlC m_cInterface{};                                ///< C-Interface for external usage
  };
}
