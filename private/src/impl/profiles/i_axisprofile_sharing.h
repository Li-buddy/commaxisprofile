#pragma once
/// @file  i_axisprofile_sharing.h
///
/// This header file is part of the axisprofile library and define the
/// sharing handler extend operation. 
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class IAxisProfileSharingHandler;

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

  /// @brief This class interface used to extend the operation for all interface in axisprofile.
  class IAxisProfileSharing
  {
    friend class AxisProfileSharingHandler;
  public:
    /// @brief Destructor
    virtual ~IAxisProfileSharing() noexcept = default;

  protected:

    /// @brief This method set sharing handler to detail interface
    /// @param[in] handler The sharing handler need be set.
    virtual void setSharingHandler(IAxisProfileSharingHandler* handler) = 0;    
  };
}
