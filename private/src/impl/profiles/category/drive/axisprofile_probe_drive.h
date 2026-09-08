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
#include "profiles/axisprofile_probe.h"

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
  const std::string CHEKC_ITEM_EVL_POS_EDGE = "evaluates positive edge";          ///< Name of check item for positive edge evaluation
  const std::string CHEKC_ITEM_EVL_NEG_EDGE = "evaluates negative edge";          ///< Name of check item for negative edge evaluation
  const std::string CHEKC_ITEM_MARKER_FAILURE = "marker failure active";          ///< Name of check item for marker failure active

  const std::string PRECONDITION_LATCHED_CNT_MAPPED = "latched counter mapping";  ///< Name of precondition for latched counter mapped
  const std::string PRECONDITION_EDGE_VALUE_MAPPED = "edge value mapping";        ///< Name of precondition for edge value mapped
  const std::string PRECONDITION_MARKER_FAILURE = "marker failure mapping";       ///< Name of precondition for marker failure mapped

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief This struct check items of probe sub-functions
  struct EnableCheckItem
  {
    std::string m_itemName {};      ///< Name of probe function item, e.g. 'evaluate positive edge'
    bool m_itemEnabled {};          ///< Is item enabled
    std::string m_precondition {};  ///< Name of prerequisite of item, e.g. 'cycle data of positive edge latched mapped'
    bool m_preconditionFullfill {}; ///< Is prerequisite full fill
    std::string m_addTxt;           ///< Additional text relates to parameter ID (e.g. IDN)
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This struct contains the probe related parameters with bit processing
  struct ProbeBitProcess
  {
    WordStructure m_ctrlWord {};         // Probe control word process via bit      
    WordStructure m_extendedCtrlWord {}; // Probe extended control word process via bit
  };
  /// @brief This is the base class for probe handling.
  class AxisProfileProbeDrive : public AxisProfileProbe
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo     Reference to the Profile Basic Information
    explicit AxisProfileProbeDrive(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~AxisProfileProbeDrive() noexcept override;

    // ------------------------------ Internal methods (not in interface!) --------------------------------
    /// @brief This method raise a command and get its async statemachine
    /// @param[in]      probeIdx     Indicates which probe should be set.
    /// @param[out]     asyncState   Pointer to async statemachine
    /// @return         Return command async statemachine
    AxisProfileResult raiseAndGetStatemachine(ProbeIndex probeIdx, AsyncCommandStatemachine*& asyncState);

    /// @brief This method handle the result of commands execution
    /// @param[in,out]  asyncState    Async statemachine
    /// @param[in]      result        Executing result of this cycle
    /// @return         Return handle result
    AxisProfileResult resultHandlingCycle(AsyncCommandStatemachine& asyncState, AxisProfileResult& result);

    /// @brief Reset probe commands state
    /// @return         Return handle result
    AxisProfileResult resetProbeCommand();

    /// @brief This method check if the item list is full filled
    /// @param[in] probeIdx  probe index
    /// @param[in] checkList Item list to be checked
    /// @return TRUE indicated all item full filled
    bool itemListChecker(ProbeIndex probeIdx, std::vector<EnableCheckItem>& checkList) const;

  protected: // data

    ProbeBitProcess             m_probeBitProcess {};                ///< Probe async command per bit execution
    std::vector<EnableCheckItem> m_probe1CheckList {};               ///< Check if probe1 values assigned to profile
    std::vector<EnableCheckItem> m_probe2CheckList {};               ///< Check if probe2 values assigned to profile

  private: // data

    AsyncCommandStatemachine* m_asyncStates {};           ///< Probe async state machine

    /// @brief probe async interface may invoked by different threads and different probes of same thread.
    ///        1. for multi-thread safety, probe async interfaces (of each axisprofile instance) share one async state machine.
    ///        2. for multi-probe safety,  @ref m_workingProbe is a flag to indicate the actual working probe index.
    /// @note Probe async interface can not be used by its 2 probes same time!
    ///       e.g. probe2's async method is only start until probe1 has's finished its methods.
    ProbeIndex m_workingProbe {ProbeIndex::PROBE_UNDEFINED}; ///< actual working probe
  };
}
