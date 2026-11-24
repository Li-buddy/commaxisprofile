#pragma once
/// @file
///
/// This header file is part of the axisprofile library and implements the
/// data sharing handler interface between every profile sub-module.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_internal.h"
#include "profiles/profile_defs.h"

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

  /// @brief This is the interface of the profile sharing handler
  class IAxisProfileSharingHandler
  {
  public: // methods

    /// @brief Destructor
    virtual ~IAxisProfileSharingHandler() noexcept = default;

    /// @brief This method register interface pointer to sharing handler object
    /// @note  The interface is overwritten if registers duplicated at same interface type.
    /// @param[in] itfType      detail type of interface
    /// @param[in] sharingITF   pointer to a interface
    /// @return    Return value indicating success or error.
    virtual AxisProfileResult registSharingITF(SharingInterfaceType itfType, IAxisProfileSharing* const sharingITF) = 0;

    /// @brief This method remove the specific interface from sharing handler
    /// @param[in] itfType      detail type of interface
    virtual void removeSharingITF(SharingInterfaceType itfType) = 0;

    /// @brief This method gets the interface of device parameter interface.
    /// @return  Pointer to the device parameter interface class.
    virtual IAxisProfileDeviceParamInternal* getDeviceParamIF() const = 0;

    /// @brief This method gets the interface of mapping handler.
    /// @return  Pointer to the mapping handler class.
    virtual IAxisProfileMappingHandler* getMappingHandlerIF() const = 0;

    /// @brief Deprecated. This method gets the interface of axisprofile command.
    /// @return  Pointer to the axisprofile command interface.
    virtual IAxisProfileCommandInternal* getCommandIF() const = 0;

    /// @brief This method gets the interface of axisprofile command.
    /// @return  Pointer to the axisprofile command interface.
    virtual IAxisProfileCmdInternal* getCmdIF() const = 0;

    /// @brief This method gets the interface of axisprofile cyclic data.
    /// @return  Pointer to the axisprofile cyclic data interface.
    virtual IAxisProfileCyclicDataInternal* getCyclicDataIF() const = 0;

    /// @brief This method gets the interface of status and controlword management.
    /// @return  Pointer to the status and controlword interface.
    virtual IAxisProfileStatusControlInternal* getStatusControlIF() const = 0;

    /// @brief This method gets the interface of async access handler.
    /// @return Pointer to the async access handler interface.
    virtual IAxisProfileParamAccess* getParamAccessIF()  const = 0;

    /// @brief This method gets the interface of aux job.
    /// @return  Pointer to the axisprofile command interface.
    virtual IAuxJobHandler* getAuxJobHandlerIF() const = 0;

    /// @brief This method gets the interface of job manager.
    /// @return  Pointer to the job manager
    virtual IJobMgr* getJobMgrIF() const = 0;

    /// @brief This method gets the interface of scaling.
    /// @return Pointer to the scaling interface
    virtual IScaling* getScalingIF() const = 0;

    virtual ICategoryAdapter* getCategoryIF() const = 0;

    /// @brief This method gets the interface of axisprofile property.
    /// @return  Pointer to the axisprofile property interface.
    virtual IAxisProfilePropertyInternal* getPropertyIF() const =0;
  };
}
