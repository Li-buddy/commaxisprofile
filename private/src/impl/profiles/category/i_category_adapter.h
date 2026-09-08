#pragma once

/// @file
///
/// @brief This source file is part of the axisprofile library and implements the ICategoryAdapter.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "profiles/i_axisprofile_state_switch.h"
#include "profiles/i_axisprofile_sharing.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include <vector>
#include <functional>

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
  class IAxisProfileCommandInternal;
  class IAxisProfileStatusControlInternal;
  class IAxisProfileCyclicDataInternal;
  class IAxisProfileDeviceParamInternal;
  class IAxisProfileProbeInternal;
  class IScaling;
  class IAxisProfileSharingHandler;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This class responsible for profile category common behavior.
  class ICategoryAdapter: public IAxisProfileSharing
  {
  public:
    virtual ~ICategoryAdapter() noexcept override = default;
    
    /// @brief This method gets the usage mode of current profile.
    /// @return Current category enum value.
    virtual AxisProfileCategory getProfileCategory() const = 0;

    /// @brief Get the command interface for current category object.
    /// @return The command interface.
    virtual IAxisProfileCommandInternal* getCommandInternal() = 0;

    /// @brief Get the status control interface for current category object.
    /// @return The statuc control interface.
    virtual IAxisProfileStatusControlInternal* getStatusControlInternal() = 0;

    /// @brief Get the cyclic data interface for current category object.
    /// @return The cyclic data interface.
    virtual IAxisProfileCyclicDataInternal* getCyclicDataInternal() = 0;

    /// @brief Get the device parameter interface for current category object.
    /// @return The device parameter interface.
    virtual IAxisProfileDeviceParamInternal* getDeviceParamInternal() = 0;

    /// @brief Get the probe interface for current category object.
    /// @return The probe interface.
    virtual IAxisProfileProbeInternal* getProbeInternal() = 0;

    /// @brief Get the scaling pointer for current category object.
    /// @return The scaling pointer.
    virtual IScaling* getScalingInternal() = 0;

    /// @brief This method will register all the component to sharing handler.
    /// @param[in] handler The pointer to sharing handler.
    virtual void registerSharingITFs(IAxisProfileSharingHandler* handler) = 0;

    /// @brief This method will remove all the component pointers for sharing handler.
    virtual void removeSharingITFs() = 0;

    /// @brief This method will update the scaling strategy of current object.
    /// Note: the scaling strategy switch only allowed for CoE protocol.
    /// @param[in] strategy the target scaling strategy
    /// @return result indicate the update is success or error happens.
    virtual AxisProfileResult updateScalingStrategy(const AxisProfileScalingStrategy& strategy) = 0;

    /// @brief This method load the init method for the profile.
    /// @param[in,out] methods The container for init methods storage.
    virtual void loadInitMethod(std::vector<std::function<AxisProfileResult()>>& methods) = 0;

    /// @brief Set the statemachine to category.
    /// @param[in] statemachine The new statemachine.
    virtual AxisProfileResult setStatemachine(const StateMachineState* const statemachine) = 0;

    /// @brief Get the statemachine
    /// @return the statemachine
    virtual const StateMachineState*& getStatemachine() = 0;
  };
}
