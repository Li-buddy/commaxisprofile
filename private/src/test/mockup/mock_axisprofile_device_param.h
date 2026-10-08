#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements a mockup for
/// the axis profile booting config interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_device_param_internal.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
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
  /// @brief This class implements a mockup for the axis profile booting config interface.
  class MockAxisProfileDeviceParam : public comm::axisprofile::IAxisProfileDeviceParamInternal
  {
  public: // methods
    /// @brief Constructor
    MockAxisProfileDeviceParam() = default;
    /// @brief Destructor
    virtual ~MockAxisProfileDeviceParam() noexcept override = default;

    const IAxisProfileBootingConfigC* getDeprecatedCInterface() const override { return nullptr; }
    const IAxisProfileDeviceParamC* getCInterface() const override { return nullptr; }
    bool checkBrakeDelaySupport() const override { return false; }
    comm::axisprofile::AxisProfileResult getErrorBrakeDelayInfo(const comm::axisprofile::AxisProfileDeviceErrorType /*errorType*/, bool& /*isActivated*/, double& /*delayTime*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult getErrorBrakeDynLimit(const comm::axisprofile::AxisProfileDeviceErrorType /*errorType*/, double& /*deceleration*/, double& /*jerk*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult booting() override { return m_returnValue; };
    comm::axisprofile::OpModeAvailInfo* getOpModeAvailInfo() override { return m_opModeAvailInfo; };
    comm::axisprofile::AxisProfileResult getPositionDataFormat(comm::axisprofile::AxisProfilePosDataFormat& /*posDataFormat*/) override { return m_returnValue; };
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/) override {};
    comm::axisprofile::DeviceCfgData& getDeviceCfgData() override {  return m_deviceCfgData; }
    bool isLatchParamSupport() const override { return false; };
    comm::axisprofile::AxisProfileResult checkAndSetModulo(double /*modValue*/, std::string& /*dynErrTxt*/) override { return m_returnValue; };
    bool checkFunctionSupported(const comm::axisprofile::FunctionSupport /*function*/, comm::axisprofile::FunctionSupportErrorInfo& /*errorInfo*/) override { return m_funcSupported; };
    comm::axisprofile::AxisProfileResult getDCSync0ShiftTime(double& sync0) override
    {
      sync0 = m_dcSync0ShiftTime;
      return m_returnValue;
    };
    comm::axisprofile::AxisProfileResult getOutputEffectiveDelay(double& delay) override
    {
      delay = m_outputEffectiveDelay;
      return m_returnValue;
    }
    void loadBootingTasks() override {}

    comm::axisprofile::AxisProfileResult checkStandstill(bool& standStill) override
    {
      standStill = m_standstill;
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult getRateTorqueForce(double& rateTorqueForce, std::string& /*dynErrTxt*/) override
    {
      rateTorqueForce = m_rateTorqueForce;
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult getChecksumIdnList(std::vector<std::string>& idnStrListRet) const override final
    {
      idnStrListRet = m_checksumIdnList;
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult checkAxisHomed(bool& homed)
    {
      homed = m_homed;
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult setAxisHomed(bool homed) override
    {
      m_homed = homed;
      return m_returnValue;
    };

    comm::axisprofile::SignalFuncHandler* getSignalFuncHandler() override
    {
      return nullptr;
    }

    // -------------------------------- Test methods ------------------------------------

    /// @brief This method can be used by tests to change the behavior of the member functions
    ///        by changing the returned value.
    ///
    /// @param[in]    result   New return code
    virtual void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

    /// @brief This method can be used by tests to change the behavior of the member functions
    ///        by changing the returned value.
    ///
    /// @param[in]    result   Operation mode available information.
    virtual void setReturnValue(comm::axisprofile::OpModeAvailInfo* result) { m_opModeAvailInfo = result; }

    /// @brief This method set configuration data.
    /// @param[out] data  configuration data.
    virtual void setReturnCfgData(comm::axisprofile::DeviceCfgData& data) { m_deviceCfgData = data; }

    /// @brief This method can be used by tests to change the behavior of the supported functionality.
    /// @param[out] data  configuration data.
    virtual void setFuncSupportResult(bool supported) { m_funcSupported = supported; }
    
    /// @brief This method can be used by tests to change the checksum idn list.
    /// @param[out] data  configuration data.
    virtual void setChecksumIdnList(std::vector<std::string>& list) { m_checksumIdnList = list; }

  public: // data

    comm::axisprofile::AxisProfileResult m_returnValue {comm::axisprofile::AxisProfileResult::STS_OK};  ///< the expected re turn value
    comm::axisprofile::OpModeAvailInfo* m_opModeAvailInfo {nullptr};                                    ///< Operation mode information
    comm::axisprofile::DeviceCfgData m_deviceCfgData {};                                                ///< Device series
    bool m_funcSupported {false};                                                                       ///< Functionality is supported or not.
    double m_dcSync0ShiftTime {600 * 1E-6};                                                             ///< DC Sync 0 shift time
    double m_outputEffectiveDelay {6 * 1E-3};                                                           ///< Output effective delay time
    bool m_standstill {true};                                                                           ///< Indicates axis is standstill or not
    double m_rateTorqueForce {1.0};                                                                     ///< Rate torque/force value
    std::vector<std::string> m_checksumIdnList;                                                         ///< Parameter checksum idn list
    bool m_homed {false};                                                                               ///< Indicates axis has homed or not    
  };
}
