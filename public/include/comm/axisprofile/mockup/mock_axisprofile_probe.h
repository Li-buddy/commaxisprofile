#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axis profile probe.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_probe_c.h"
#include <map>

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

  /// @brief This class is a simple helper that can be used as handle in this mockup.
  class AxisProfileProbeMockHandle
  {
  public: // methods

    /// @brief Set the return value of class methods.
    /// Note: Do not set 'm_returnValue' directly in your test or CppCheck may generate errors because of the extern C implementation in some cases.
    void setReturnValue(AxisProfileResult value) { m_returnValue = value; }
      
    /// @brief Set AT data
    /// @param[in]   probeIdx        Indicates the probe number.
    /// @param[in]   actualValues    Acutal AT data to be set.
    void setProbeATData(ProbeIndex probeIdx, ProbeActualValues actualValues)
    {
      // save actual row data into member variable and for further processing
      m_actualValues = actualValues;

      // emplace probe into values map
      m_probeActualValues.emplace(std::make_pair(probeIdx, actualValues));
    }

    /// @brief Reset AT data
    /// @param[in]   probeIdx        Indicates the probe number.
    void resetProbeATData(ProbeIndex probeIdx)
    {
      auto probe = m_probeActualValues.find(probeIdx);
      if (probe != m_probeActualValues.end())
      {
        probe->second.m_isProbeActivated = false;
        probe->second.m_posEdgeLatchedTimes = 0;
        probe->second.m_negEdgeLatchedTimes = 0;
        probe->second.m_probeLatchedPosEdge = 0;
        probe->second.m_probeLatchedPosEdge = 0;
        probe->second.m_probeEdgeDiff = 0;
        probe->second.m_probeMarkeFailures = 0;
      }
    }

    /// @brief Set probe to default values
    /// @param[in]   probeIdx        Indicates the probe number.
    void setProbeDefault(ProbeIndex probeIdx)
    {
      // set probe control word 
      auto probeCtrlWord = m_ctrlWordConfig.find(probeIdx);
      if (probeCtrlWord != m_ctrlWordConfig.end())
      {
        probeCtrlWord->second.m_measureType = false;
        probeCtrlWord->second.m_evaluatePosEdge = false;
        probeCtrlWord->second.m_evaluateNegEdge = false;
      }

      // set probe extended control word
      auto probeExtendedCtrlWord = m_extendedCtrlWordConfig.find(probeIdx);
      if (probeExtendedCtrlWord != m_extendedCtrlWordConfig.end())
      {
        probeExtendedCtrlWord->second.m_isExpWindowActivated = false;
        probeExtendedCtrlWord->second.m_isMarkerFailureActivated = false;
        probeExtendedCtrlWord->second.m_isTimeMeasureActivated = false;
        probeExtendedCtrlWord->second.m_expectWindowPosStart = 0.0;
        probeExtendedCtrlWord->second.m_expectWindowPosEnd = 0.0;
        probeExtendedCtrlWord->second.m_timeCompensationPosEdge = 0.0;
        probeExtendedCtrlWord->second.m_timeCompensationNegEdge = 0.0;
        probeExtendedCtrlWord->second.m_measuredSignal = MeasuringSignalSel::SIGNAL_UNDEFINED;
      }
        
      // set probe extended control word
      auto probeEnable = m_probeStatusCtrl.find(probeIdx);
      if (probeEnable != m_probeStatusCtrl.end())
      {
        probeEnable->second = false;
      }

    }

    /// @brief Check control word configuration and adapt to AT data
    /// @param[in]   probeIdx        Indicates the probe number.
    void checkControlWordConfig(ProbeIndex probeIdx)
    {
      // check selected probe configuration and reset invalid parameters
      auto it = m_ctrlWordConfig.find(probeIdx);
      if (it != m_ctrlWordConfig.end())
      {
        if (!(it->second.m_evaluatePosEdge))
        {
          m_actualValues.m_posEdgeLatchedTimes = 0;
          m_actualValues.m_probeLatchedPosEdge = 0;
        }
        if (!(it->second.m_evaluateNegEdge))
        {
          m_actualValues.m_probeLatchedNegEdge = 0;
          m_actualValues.m_negEdgeLatchedTimes = 0;
        }
      }
    }

    /// @brief Check extended control word configuration and adapt to AT data
    /// @param[in]   probeIdx        Indicates the probe number.
    void checkExtendedCtrlWordConfig(ProbeIndex probeIdx)
    {
      auto it = m_extendedCtrlWordConfig.find(probeIdx);
      if (it != m_extendedCtrlWordConfig.end())
      {
        if (!(it->second.m_isMarkerFailureActivated))
        {
          m_actualValues.m_probeMarkeFailures = 0;
        }
      }
    }

    /// @brief Check if probe enabled and adapt to AT data
    /// @param[in]   probeIdx        Indicates the probe number.
    void checkIfProbeEnabled(ProbeIndex probeIdx)
    {
      auto it = m_probeStatusCtrl.find(probeIdx);
      if (it != m_probeStatusCtrl.end() && !(it->second))
      {
        resetProbeATData(probeIdx);
      }
    }

    /// @brief Adapt the probe configuration to get the AT data
    /// @param[in]   probeIdx        Indicates the probe number.
    void adapteProbeConfig(ProbeIndex probeIdx)
    {
      auto probe = m_probeActualValues.find(probeIdx);
      if (probe != m_probeActualValues.end())
      {
        probe->second = m_actualValues;
      }
    }
    
  public: // data
    std::map<ProbeIndex, ProbeBaseConfig> m_ctrlWordConfig{};         ///< Control word configuration mock data
    std::map<ProbeIndex, ProbeExtConfig> m_extendedCtrlWordConfig{};  ///< Extended control word configuration mock data
    std::map<ProbeIndex, bool> m_probeStatusCtrl{};                   ///< probe enabled mock data
    std::map<ProbeIndex, ProbeActualValues> m_probeActualValues{};    ///< Actual values mock data
    AxisProfileResult m_returnValue = AxisProfileResult::STS_OK;      ///< Return value of C-Methods
    ProbeActualValues m_actualValues{};                               ///< Actual value of probe
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // -------------------------------------- C-Interface Method-Mockups ---------------------------------------- //

  /// @brief Mockup of method AXISPROFILE_probeBaseConfig
  /// @param[in]   handle                 Handle of the interface (use class AxisProfileProbeMockHandle).   
  /// @param[in]   probeIdx               Indicates the probe number
  /// @param[in]   probeConfig            Probe control word configuration
  /// @param[in]   isAutoActivated        Is probe automatic activated
  /// @return      Member variable 'm_returnValue' of the handle is returned
  extern "C" AxisProfileResult AXISPROFILE_probeBaseConfigMock(void* handle, ProbeIndex probeIdx, ProbeBaseConfig probeConfig, bool isAutoActivated);

  /// @brief Mockup of method AXISPROFILE_probeExtConfig
  /// @param[in]   handle                 Handle of the interface (use class AxisProfileProbeMockHandle).
  /// @param[in]   probeIdx               Indicates the probe number
  /// @param[in]   probeExtendConfig      Probe extended control word configuration
  /// @param[in]   isStateMonitActivated  Is state monit activate
  /// @return      Member variable 'm_returnValue' of the handle is returned
  extern "C" AxisProfileResult AXISPROFILE_probeExtConfigMock(void* handle, ProbeIndex probeIdx, ProbeExtConfig probeExtendConfig, bool isStateMonitActivated);

  /// @brief Mockup of method AXISPROFILE_probeStatusCtrl.
  /// @param[in]   handle                 Handle of the interface (use class AxisProfileProbeMockHandle).
  /// @param[in]   probeIdx               Indicates the probe number.
  /// @param[in]   enable                 Is probe enabled
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_probeStatusCtrlMock(void* handle, ProbeIndex probeIdx, bool enable);
    
  /// @brief Mockup of method AXISPROFILE_getProbeActualValuesRT
  /// @param[in]   handle                 Handle of the interface (use class AxisProfileProbeMockHandle).
  /// @param[in]   probeIdx               Indicates the probe number.
  /// @param[out]  actualValueProbe       The actual measured values of probe
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getProbeActualValuesRTMock(void* handle, ProbeIndex probeIdx, ProbeActualValues* actualValueProbe);
    
  /// @brief Mockup of method AXISPROFILE_onlineChangeParams
  /// @param[in]   handle                 Handle of the interface (use class AxisProfileProbeMockHandle).
  /// @param[in]   probeIdx               Indicates the probe number.
  /// @param[in]   updateValue            Online update the measurement configuration
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_onlineChangeParamsMock(void* handle, ProbeIndex probeIdx, ProbeUpdateMeasurement updateValue);
   
  /// @brief Mockup of method AXISPROFILE_getProbeValues
  /// @param[in]   handle                 Handle of the interface (use class AxisProfileProbeMockHandle).
  /// @param[in]   probeIdx               Indicates the probe number.
  /// @param[out]  probeConfig            Probe control word configuration
  /// @param[out]  probeExtendConfig      Probe extended control word configuration
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getProbeValuesMock(void* handle, ProbeIndex probeIdx, ProbeBaseConfig* probeConfig, ProbeExtConfig* probeExtendConfig);
    
  /// @brief Mockup of method AXISPROFILE_abortAndResetProbe
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_abortAndResetProbeMock(void* handle);
    
  /// @brief Mockup of method AXISPROFILE_isProbeSupported
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_isProbeSupportedMock(void* handle);
}
