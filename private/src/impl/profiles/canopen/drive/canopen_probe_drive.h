#pragma once

/// @file
///
/// This header file is part of the axisprofile library and declare canopen probe parameters.
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
#include "axisprofile_internal_defs.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/canopen/canopen_param_access.h"
#include "profiles/category/drive/axisprofile_probe_drive.h"
#include <list>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenProbe;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  /// @brief This struct indicates the probe related parameters.
  struct ProbeParameters
  {
    ProbeBaseConfig m_probeConfig {}; ///< Probe control word.
    bool m_selectSignal {false};      ///< Measured signal select (0: probe, 1: Z plus) 

    bool m_evaluatePosEdge {false};   ///< Is positive edge evaluated?
    bool m_evaluateNegEdge {false};   ///< Is negative edge evaluated?
    bool m_measureType {false};       ///< Signal or continuous

    std::uint16_t m_probeStatusCtrl = 0;   ///< Is probe enabled?
    std::uint16_t m_cmdEnableValue = 0;    ///< Is probe activated?
    std::uint16_t m_probe1Enable = 0;      ///< Is probe 1 enabled?
    std::uint16_t m_probe2Enable = 0;      ///< Is probe 2 enabled?
  };

  /// @brief This struct indicates the probe commands
  struct ExecuteProbe
  {
    std::uint16_t m_probeStatusCtrl = 0;   ///< Enable/Disable probe
  };

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class CanopenAsyncAccessHandler;

  /// @brief This class implements the probe handling for canopen drive.
  class CanopenProbeDrive : public AxisProfileProbeDrive
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Pointer to the Profile Basic Information  
    /// @param[in]   at                 Pointer to the Acknowledge Telegram (AT)    
    CanopenProbeDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at);

    /// @brief Destructor
    ~CanopenProbeDrive() noexcept override = default;

    /// @brief This method returns the class name.
    /// @return   Return class name.
    static const char* getClassName(void) { return (CLASS_NAME(CanopenProbeDrive)); }

    AxisProfileResult probeBaseConfig(ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool isAutoActivated) final;
        
    AxisProfileResult probeExtConfig(ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig, bool isStateMonitActivated) final;

    AxisProfileResult probeStatusCtrl(ProbeIndex probeIdx, bool enable) final;   
        
    AxisProfileResult getProbeActualValuesRT(ProbeIndex probeIdx, ProbeActualValues* actualValueProbe)final;
        
    AxisProfileResult onlineChangeParams(ProbeIndex probeIdx, ProbeUpdateMeasurement updateValue) final;
        
    AxisProfileResult getProbeValues(ProbeIndex probeIdx, ProbeBaseConfig* probeConfig, ProbeExtConfig* probeExtendConfig) final;

    AxisProfileResult abortAndResetProbe() final;

    AxisProfileResult isProbeSupported() final;

  private: // methods

    void loadBootingTasks() final;

    /// @brief This methods set and write the probe control word into drive
    /// @param[in]      probeIdx     Indicates which probe should be set.
    /// @param[in]      probeConfig  Probe bits configuration of 0x60B8
    /// @param[in,out]  asyncState   Asycn state machine
    /// @return         Return value indicating success or error.
    AxisProfileResult executeCtrlWordConfig(ProbeIndex probeIdx, ProbeBaseConfig probeConfig, AsyncCommandStatemachine& asyncState);

    /// @brief This methods set and write the probe status control into drive
    /// @param[in]      probeIdx    Indicates which probe should be set
    /// @param[in]      enable      Is probe enabled?
    /// @param[in,out]  asyncState  Async state machine 
    /// @return         Return value indicating succeed or error
    AxisProfileResult executeProbeStatusCtrl(ProbeIndex probeIdx, bool enable, AsyncCommandStatemachine& asyncState);

    /// @brief This methods processing the drive configuration of probe
    /// @param[in]      probeIdx    Indicates which probe should be set
    /// @param[in,out]  asyncState  Async state machine 
    /// @return         Return value indicating success or error
    AxisProfileResult probeValueProcess(ProbeIndex probeIdx, AsyncCommandStatemachine& asyncState);

    /// @brief This methods set probe and related commands into the commands map
    void setProbeCommands();

    /// @brief This method initialize async access handler
    /// @return         Return indicating success or error.
    AxisProfileResult initAsyncAccessHandler();
        
  private: // data

    friend class ::TestCanopenProbe;            ///< For unit test of the CanopenProbeDrive
    const AcknowledgeTelegram* m_telegramAT{};  ///< Pointer to the Acknowledge Telegram (AT)
    CanopenParamAccess* m_paramAccess {};       ///< Pointer to the canopen async access handler

    std::list<CanopenParamStruct> m_probeActualValues {};        ///< Parameter list to be access asynchronously for get probe values
    std::list<CanopenParamStruct> m_probeEable {};               ///< Parameter list to be access asynchronously for enable probe

    ProbeParameters m_probeParam {};                             ///< Probe parameters
    std::map<ProbeIndex, ExecuteProbe> m_probeCmds {};           ///< Map of probe number related commands
  };
}
