#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the main
///        interface for init, enable, execute, abort and reset probe.
///        This is a sub-interface of 'IAxisProfilePropertyInternal'.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------

#include "bundle/i_axisprofile_property.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "i_axisprofile_sharing.h"

struct IAxisProfilePropertyC;

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
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for all stages of property.
  class IAxisProfilePropertyInternal : public IAxisProfileProperty, public IAxisProfileSharing
  {
  public: // methods

    virtual ~IAxisProfilePropertyInternal() noexcept override = default;

    /// @brief This method gets the property C-Interface.
    /// @return Pointer to the property C-Interface.
    virtual const IAxisProfilePropertyC* getCInterface() const = 0;

    /// @brief This method sets the flag indicate whether enable checksum function.
    /// @param[in]  enable  whether enable checksum function.
    virtual void enableChecksum(bool enable) = 0;

    /// @brief This method get the flag indicate whether enable checksum function
    /// @return     Return whether enable checksum.
    virtual bool isChecksumEnabled() const = 0;

    /// @brief This method sets the EtherCAT master name for curret axisprofile.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult setEtherCATName(const char* name) = 0;
  };
}
