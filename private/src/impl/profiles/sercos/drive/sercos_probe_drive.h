#pragma once

/// @file
///
/// This header file is part of the axisprofile library and declare sercos probe parameters.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 - 2025 Bosch Rexroth AG
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
#include "profiles/category/drive/axisprofile_probe_drive.h"
#include "profiles/sercos/sercos_defs.h"
#include <list>
#include "profiles/signal_func/signal_func_cmd.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestSercosProbe;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr double PROBE_COMPENSATION_LIM = 50000000; ///< only used for dead time compensation limit of probe, it is a constant value in ctrlX/Indra Drive.

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This struct indicates the probe related parameters.
  struct ProbeParameters
  {
    ProbeBaseConfig m_probeConfig {};         ///< Probe control word.
    ProbeExtConfig m_probeExtendConfig {};    ///< Probe extended control word.
    std::uint16_t m_selectedSignal = 0;       ///< Measured signal.
    bool m_isAutoActivated = false;           ///< Is probe automatic activated?
    bool m_isStateMonitActivated = false;     ///< Is state monitor activated?

    std::int32_t m_expPosStart = 0;           ///< Expected window start position.
    std::int32_t m_expPosEnd = 0;             ///< Expected window end position.
    std::int32_t m_posEdgeCompensation = 0;   ///< Positive edge dead time compensation.
    std::int32_t m_negEdgeCompensation = 0;   ///< Negative edge dead time compensation.

    std::int32_t m_updateExpPosStart = 0;     ///< Expected window start position update.           
    std::int32_t m_updateExpPosEnd = 0;       ///< Expected window end position update.
    bool m_evaluatePosEdge = false;           ///< Is positive edge evaluated?
    bool m_evaluateNegEdge = false;           ///< Is negative edge evaluated?
    bool m_isMarkerFailureActivated = false;  ///< Is marker failure activated?
    bool m_isExpWindowActivated = false;      ///< Is expected window activated?
    bool m_measureType = false;

    std::uint16_t m_probeStatusCtrl = 0;      ///< Is probe enabled?
    std::uint16_t m_cmdEnableValue = 0;       ///< is probe activated?
    std::uint16_t m_probe1Enable = 0;         ///< is probe 1 enabled?
    std::uint16_t m_probe2Enable = 0;         ///< is probe 2 enabled?
  };

  /// @brief This struct indicates the probe related commands IDN.
  struct ExecuteProbe
  {
    std::uint16_t m_expWindowStart = 0;        ///< Expect window start position of probe.                            
    std::uint16_t m_expWindowEnd = 0;          ///< Expect window end position of probe.           
    std::uint16_t m_signalSelection = 0;       ///< Measurement signal selection.            
    std::uint16_t m_posEdgeCompensation = 0;   ///< Dead time compensation of positive edge. 
    std::uint16_t m_negEdgeCompensation = 0;   ///< Dead time compensation of negative edge. 
    std::uint16_t m_probeStatusCtrl = 0;       ///< Enable/Disable probe.
  };

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  /// @brief This class implements the probe handling for sercos.
  class SercosProbeDrive : public AxisProfileProbeDrive
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Reference to the Profile Basic Information
    /// @param[in]   at                 Pointer to the Acknowledge Telegram (AT)
    SercosProbeDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at);

    /// @brief Destructor
    ~SercosProbeDrive() noexcept override = default;

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

    /// @brief This methods set and write the probe control word into drive.
    /// @param[in]      probeIdx              Indicates which probe should be set.
    /// @param[in]      probeConfig           Probe bits configuration of S-0-0169.
    /// @param[in]      isAutoActivated       If cycle probe automatic activated.
    /// @param[in,out]  asyncState            Async state machine.
    /// @return         Return value indicating success or error.
    AxisProfileResult executeCtrlWordConfig(ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool isAutoActivated,
                                            AsyncCommandStatemachine& asyncState);

    /// @brief This methods set and write the probe extended control word into drive.
    /// @param[in]      probeIdx              Indicates which probe should be set.
    /// @param[in]      probeExtendConfig     Probe extended configuration. 
    /// @param[in]      isStateMonitActivated If state monitor activated.
    /// @param[in,out]  asyncState            Async state machine.
    /// @return         Return value indicating success or error.
    AxisProfileResult executeExtendedCtrlWordConfig(ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig,
                                                    bool isStateMonitActivated, AsyncCommandStatemachine& asyncState);

    /// @brief This methods set and write the probe status control into drive.
    /// @param[in]      probeIdx              Indicates which probe should be set.
    /// @param[in]      enable                Is probe enabled?
    /// @param[in,out]  asyncState            Async state machine.
    /// @return         Return value indicating success or error.
    AxisProfileResult executeProbeStatusCtrl(ProbeIndex probeIdx, bool enable, AsyncCommandStatemachine& asyncState);

    /// @brief Executes a probe status control signal function for the specified probe.
    /// @param[in]      probeIdx         The index of the probe to control.
    /// @param[in]      enable           True to enable the probe status control signal, false to disable it.
    /// @param[in,out]  asyncState       Reference to the asynchronous command state machine for managing the operation's state.
    /// @return         The result of the axis profile operation, indicating success or failure status.
    AxisProfileResult executeProbeStatusCtrlSignalFunc(ProbeIndex probeIdx, bool enable, AsyncCommandStatemachine& asyncState);

    /// @brief This methods processing the drive configuration of probe.
    /// @param[in]      probeIdx              Indicates which probe should be set.
    /// @param[in,out]  asyncState            Async state machine.
    /// @return         Return value indicating success or error.                                                
    AxisProfileResult probeValueProcess(ProbeIndex probeIdx, AsyncCommandStatemachine& asyncState);

    /// @brief This methods processing the data for online update
    /// @param[in]      probeIdx              Indicates which probe should be set.
    /// @param[in,out]  asyncState            Async state machine.
    /// @param[in]      updateValue           The data which should be updated
    /// @return         Return value indicating success or error.  
    AxisProfileResult updateMeasureData(ProbeIndex probeIdx, AsyncCommandStatemachine& asyncState, ProbeUpdateMeasurement updateValue);

    /// @brief This methods set probe and related commands into the commands map
    void setProbeCommands();

    /// @brief This methods switch the signal IDN to signal.
    /// @return         Return value indicating the signal.    
    MeasuringSignalSel measuredSignalConvert();

    // @brief This methods convert the time unit from second to nanosecond.
    /// @param[in]      rawTime               input time as second
    /// @return         Return nanosecond.    
    std::int32_t timeUnitAdapteDevice(double rawTime);

    /// @brief This methods convert the time unit from nanosecond to second.
    /// @param[in]      convertTime           inverse type
    /// @param[in]      rawTime               input time as nanosecond
    /// @param[in]      dataSize              data size
    /// @return         Return second.  
    double timeUintAdapteInv(std::uint32_t convertTime, std::int32_t rawTime, const size_t dataSize);

    /// @brief This methods check if probe needs activate or deactivate
    /// @param[in]      probeIdx              Indicates which probe should be set.
    /// @param[in]      enable                Is probe enabled?
    /// @return         Is probe need activate or deactivate?  
    bool checkProbeEnabledStatus(ProbeIndex probeIdx, bool enable) const;

    /// @brief This method initialize async access handler
    /// @return         Return indicating success or error.
    AxisProfileResult initAsyncAccessHandler();

    /// @brief This method checks if the signal function is supported for the specified probe index.
    /// @param[in] probeIdx Indicates which probe should be checked.
    /// @param[out] retFlag Returns true if the signal function is supported, false otherwise.
    /// @return Return value indicating success or error.
    comm::axisprofile::AxisProfileResult checkIsSignalSupported(ProbeIndex probeIdx, bool& retFlag);

  private: // data

    friend class ::TestSercosProbe;              // For unit test of the SercosProbeDrive

    const AcknowledgeTelegram* m_telegramAT {};  ///< Pointer to the Acknowledge Telegram (AT)
    SercosParamAccess* m_paramAccess {};         ///< Pointer to the Sercos async access handler

    std::list<SercosParamStruct> m_paramListExtendedCtrlWord {};   ///< Parameter list to be access asynchronously for probe control word
    std::list<SercosParamStruct> m_probeActualValues {};           ///< Parameter list to be access asynchronously for get probe values
    std::list<SercosParamStruct> m_probeEable {};                  ///< Parameter list to be access asynchronously for enable probe
    std::list<SercosParamStruct> m_onlineUpdate {};                ///< Parameter list to be access asynchronously for online update values

    ProbeParameters m_probeParam {};                               ///< Probe parameters
    std::map<ProbeIndex, ExecuteProbe> m_probeCmds {};             ///< Map of probe number related commands 
    bool m_timeMeasure = false;                                    ///< Is time measurement activated    
    std::uint16_t m_prb1MeasureSignal {};                          ///< Probe1 measure signal
    std::uint16_t m_prb2MeasureSignal {};                          ///< Probe2 measure signal

    SignalFuncCmd m_signalFuncProbe {{FunctionSupport::PROBE1_ENABLE_CYCLIC}};   ///< Signal function command default for probe1 parameter
    bool m_isSignalFuncCmd {false};                                              ///< flag to indicate whether the command is a signal function command
  };
}
