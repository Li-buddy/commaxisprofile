#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements
/// base class for cyclic data handling.
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
#include "comm/axisprofile/interfaces/i_axisprofile_cyclic_data_c.h"
#include "data_sharing/i_sharing_handler.h"
#include "i_axisprofile_cyclic_data_internal.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class ProfileTrace;
  class ICategoryAdapter;
  class IUserPDOMgr;

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

  /// @brief This is the base class for cyclic data handling.
  class AxisProfileCyclicData : public IAxisProfileCyclicDataInternal
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Pointer to the Profile Basic Information
    explicit AxisProfileCyclicData(const ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~AxisProfileCyclicData() noexcept override = default;

    const IAxisProfileCyclicDataC* getCInterface() const final;
    
    void resetFPUFlag() final;

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    bool checkActualValueAvailabilityNRT(AxisProfileValueTypeID valueTypeID, DataAccessErrorInfo& errorInfo) override;
    void setActSyncTime(double absSync0Time, double fieldbusCycleTime) final;
    double getEffectiveTimeOfActOutput() final;
    double getLatchTimeOfActInput() final;
    const StateMachineState*& getStatemachine() final;
    AxisProfileResult setStateMachineStatusIF(const StateMachineState* const stateMachineState) final;
    AxisProfileResult booting() override;
    AxisProfileResult setNewRefPosition(std::int32_t /*newRefPos*/) override;
    AxisProfileResult activeNewRefOperation(bool /*active*/) override;

  protected:

    /// @brief This method checks if the actual value is assigned in mapping.
    /// @param[in]  valueID     The actual value type(can be position velocity...)
    /// @param[out] errorInfo   Contain error information when the item not assigned.
    /// @return     Return boolean value indicates whether the item are assigned.
    bool checkIfTelegramItemAssigned(AxisProfileValueTypeID valueID, DataAccessErrorInfo& errorInfo);

    /// @brief This method checks the fieldbus status.
    /// @param[out] errorInfo   Contain error information when the fieldbus status meets error.
    /// @return     Return boolean value indicates whether the fieldbus status are ok.
    bool checkFieldbusStatus(DataAccessErrorInfo& errorInfo);

    /// @brief This method checks the slave status.
    /// @param[out] errorInfo   Contain error information when the slave status meets error.
    /// @return     Return boolean value indicates whether the slave status are ok.
    bool checkForSlaveStatus(DataAccessErrorInfo& errorInfo);

  protected: // data

    const StateMachineState* m_stateMachineState {};  ///< Pointer to state machine state
    IAxisProfileSharingHandler* m_sharingHandler {};  ///< Pointer to the sharing handler
    const ProfileBasicInfo& m_profileBasicInfo;       ///< Profile basic information
    IScaling* m_scaling {};                           ///< for batter performance, init a member during booting, instead of init when use.
    double m_actInputLatchTime {0.0};                 ///< stores latch time of actual input in second
    double m_actOutputEffectiveTime {0.0};            ///< stores effective time of actual output in second
    const double INPUT_SHIFT_CYCLE {2.0};             ///< default input shift cycles, input is latch happens 2 cycles ago
    const double OUTPUT_SHIFT_CYCLE {1.0};            ///< default output shift cycles, 1 cycle transmit, slave has no buffer
    std::vector<std::function<AxisProfileResult()>> m_initCommands;  ///< Vector contains all init methods

  private: // data

    IAxisProfileCyclicDataC  m_cInterface {};         ///< C-Interface for external usage
  };
}
