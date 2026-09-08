#pragma once

/// @file
///
/// This header file is part of the axisprofile library and implements
/// factory to create every supported axisprofile.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "axisprofile_internal_defs.h"
#include "profiles/value_mapping/mapping_handler_defs.h"
#include <map>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestAxisProfileFactory;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class IAxisProfileInternal;


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

  /// @brief axis profile factory
  class AxisProfileFactory
  {
  public: // methods
    /// @brief destructor
    virtual ~AxisProfileFactory() noexcept = default;

    /// @brief This is the central method to create new profiles.
    /// @param[in]    profileName          Name of the profile
    /// @param[in]    axisProfileType      The axis profile type
    /// @param[out]   newProfile          Pointer to the new profile
    /// @return      Return value indicating success or error.
    static AxisProfileResult createProfile(const std::string& profileName, const AxisProfileType axisProfileType, IAxisProfileInternal** newProfile);

    /// @brief This is the method to get input mapping value ID and mapping type list.
    /// @param[in]    axisProfileType      The axis profile type
    /// @param[in]    profileCategory      Category of current profile.
    /// @param[out]   InputMappingValueID  get all input mapping value ID and mapping type
    /// @return      Return value indicating success or error.
    static AxisProfileResult getInputMappingValueIDList(std::map<InputMappingValueID, MappingCfgInfo>& InputMappingValueID, const AxisProfileCategory profileCategory, const AxisProfileType axisProfileType);

    /// @brief This is the method to get output mapping value ID list and mapping type.
    /// @param[in]   axisProfileType       The axis profile type
    /// @param[in]   profileCategory       Category of current profile.
    /// @param[out]  OutputMappingValueID  get all output mapping value ID and mapping type
    /// @return      Return value indicating success or error.
    static AxisProfileResult getOutputMappingValueIDList(std:: map<OutputMappingValueID, MappingCfgInfo>& OutputMappingValueID, const AxisProfileCategory profileCategory, const AxisProfileType axisProfileType);

  private: // methods

    /// @brief This method converts the external axis profile type to the underlying IEC 61800-7 drive profile type and the used network technology.
    /// @param[in]   type                The axis profile type
    /// @param[out]  driveProfileType    The resulting drive profile type
    /// @param[out]  networkTechnology   The resulting network technology
    /// @return      Return value indicating success or error.
    static AxisProfileResult convertAxisProfileType(const AxisProfileType type, DriveProfileType& driveProfileType, NetworkTechnology& networkTechnology);

    /// @brief This method checks if the requested profile is valid.
    /// @param[in]   networkTechnology   Network technology that should be used by the profile
    /// @param[in]   driveProfileType    IEC 61800-7 drive profile type
    /// @param[in]   networkTechnology   Network technology that should be used by the profile
    /// @return      Return value indicating success or error.
    static AxisProfileResult checkRequestedProfile(const AxisProfileType axisProfileType, const DriveProfileType driveProfileType, const NetworkTechnology networkTechnology);

  private:

    friend class ::TestAxisProfileFactory;            ///< Unittest of the AxisProfileFactory
  };
}
