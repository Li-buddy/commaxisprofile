#pragma once
/// @file
///
/// This header file is part of the axisprofile library and define the
/// internal cyclic data interface for all axisprofiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "bundle/i_axisprofile_cyclic_data.h"
#include "profiles/i_axisprofile_sharing.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct IAxisProfileCyclicDataC;

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

  /// @brief This is the internal cyclic data interface for all axisprofiles.
  class IAxisProfileCyclicDataInternal : public IAxisProfileCyclicData, public IAxisProfileSharing, public IAxisProfileStateSwitch
  {
  public: // methods

    /// @brief destructor
    virtual ~IAxisProfileCyclicDataInternal() noexcept override = default;

    /// @brief This method gets the command C-Interface.
    /// @return Pointer to the command C-Interface.
    virtual const IAxisProfileCyclicDataC* getCInterface() const = 0;

    /// @brief This method set state machine status interface
    /// @param[in]   stateMachineState      Pointer stateMachineState.
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult setStateMachineStatusIF(const StateMachineState* const stateMachineState) = 0;

    /// @brief This method get state machine status interface.
    /// @return get the statemachine.
    virtual const StateMachineState*& getStatemachine() = 0;

    /// @brief This method set actual synchronized time.
    /// @param[in]    absSync0Time            Absolute Sync0 time of distributed clock
    /// @param[in]    fieldbusCycleTime       fieldbus cycle time
    virtual void setActSyncTime(double absSync0Time, double fieldbusCycleTime) = 0;

    /// @brief This method reset the FPU error member.
    virtual void resetFPUFlag() = 0;

    ///@brief The method set new reference position to device by cyclic data (if the related cycle data is mapped)
    /// @param[in]    newRefPos       New reference position
    virtual AxisProfileResult setNewRefPosition(std::int32_t newRefPos) = 0;

    ///@brief The method active the operation of setting new reference to device by cyclic data (if the related cycle data is mapped)
    /// @param[in]    active       Ative state
    virtual AxisProfileResult activeNewRefOperation(bool active) = 0;
  };
}
