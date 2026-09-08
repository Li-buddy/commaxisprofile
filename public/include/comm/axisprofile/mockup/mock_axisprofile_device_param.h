#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements a mockup for the axis profile device parameter.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_booting_config_c.h" // TODO deprecated
#include "comm/axisprofile/interfaces/i_axisprofile_device_param_c.h"
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
  class AxisProfileDeviceParamMockHandle
  {
  public: // methods

    /// @brief constructor
    AxisProfileDeviceParamMockHandle(): m_devRxnMockData({{AxisProfileDeviceErrorType::ERROR_NONE,        DevMockData {}}, // set default value
                                                          {AxisProfileDeviceErrorType::ERROR_LIGHT,       DevMockData {}},
                                                          {AxisProfileDeviceErrorType::ERROR_FIELDBUS,    DevMockData {}},
                                                          {AxisProfileDeviceErrorType::ERROR_CRITICAL,    DevMockData {}},
                                                          {AxisProfileDeviceErrorType::ERROR_FATAL,       DevMockData {}},
                                                          {AxisProfileDeviceErrorType::ERROR_NOT_DEFINED, DevMockData {}}}){};

    /// @brief Set the return value of class methods.
    /// Note: Do not set 'm_returnValue' directly in your test or CppCheck may generate errors because of the extern C implementation in some cases.
    void setReturnValue(AxisProfileResult value) { m_returnValue = value; }

    /// @brief Set reaction info to different device error type
    /// @param[in] devErrType      device error type
    /// @param[in] brkDlyActived   reaction delay is activated 
    /// @param[in] brkDlyTimeMs    reaction delay time in ms
    /// @param[in] dec             reaction delay brake deceleration
    /// @param[in] jrk             reaction delay brake jerk
    /// @param[in] retVal1         return value to getErrorBrakeDelayInfo
    /// @param[in] retVal2         return value to getErrorBrakeDynLimit
    void setOneRxnInfo(AxisProfileDeviceErrorType devErrType, bool brkDlyActived, double brkDlyTimeMs, double dec, double jrk, AxisProfileResult retVal1, AxisProfileResult retVal2)
    {
      auto it = m_devRxnMockData.find(devErrType);
      if (it != m_devRxnMockData.end())
      {
        it->second.m_brkDlyActivated = brkDlyActived;
        it->second.m_brkDlyTimeMs = brkDlyTimeMs;
        it->second.m_dec = dec;
        it->second.m_jrk = jrk;
        it->second.m_retToGetErrorBrakeDelayInfo = retVal1;
        it->second.m_retTogetErrorBrakeDynLimit  = retVal2;
      }
      else
      {
        DevMockData oneMockData {brkDlyActived, brkDlyTimeMs, dec, jrk};
        m_devRxnMockData.emplace(devErrType, oneMockData);
      }
    }

    /// @brief set device reaction delay support
    /// @param[in] rxnSupport  device reaction delay support
    void setRxnDlySupport(bool rxnSupport) { m_brakeDelaySupport = rxnSupport; }

    /// @brief This method can be used by tests to change the behavior of the supported functionality.
    /// @param[out] data  configuration data.
    void setFuncSupportResult(bool supported) { m_funcSupported = supported; }
    void setRateTorqueForce(double torqueForce) { m_rateTorqueForce = torqueForce; }

  public: // types

    /// @brief Mock structure of brake dynamic information
    struct DevMockData
    {
      bool m_brkDlyActivated { false }; ///< reaction delay is activated
      double m_brkDlyTimeMs { 0.0 };    ///< reaction delay time in ms
      double m_dec { 2.0 };             ///< brake deceleration
      double m_jrk { 0 } ;              ///< brake jerk
      AxisProfileResult m_retToGetErrorBrakeDelayInfo {AxisProfileResult::STS_OK};  ///< Return value for getErrorBrakeDelayInfo
      AxisProfileResult m_retTogetErrorBrakeDynLimit {AxisProfileResult::STS_OK};   ///< Return value for getErrorBrakeDynLimit
    };
      
  public: // data

    bool m_brakeDelaySupport {false};                                                     ///< Reaction delay time support
    bool m_isHomed{false};                                                                ///< Reaction if axis homed
    double m_modValue {360.0};                                                            ///< Modulo value of the device
    AxisProfileResult m_returnValue = AxisProfileResult::STS_OK;                          ///< Return value of C-Methods
    std::map<AxisProfileDeviceErrorType, DevMockData> m_devRxnMockData;                   ///< Reaction mock data
    AxisProfilePosDataFormat m_devicePosFormatMock = AxisProfilePosDataFormat::UNDEFINED; ///< position data format mock 
    bool m_funcSupported {false};                                                         ///< Functionality is supported or not.
    bool m_standStill {true};                                                             ///< Axis is standstill or not.
    double m_rateTorqueForce {1.0};                                                       ///< Rate torque/force of the load in IS unit
  };

  class AxisProfileBootingConfigMockHandle : public AxisProfileDeviceParamMockHandle {};

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // -------------------------------------- C-Interface Method-Mockups ---------------------------------------- //

  /// @brief Mockup of method AXISPROFILE_checkBrakeDelaySupport
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle).   
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" bool AXISPROFILE_checkBrakeDelaySupportMock(void* handle);

  /// @brief Mockup of method AXISPROFILE_getErrorBrakeDelayInfo
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle).
  /// @param[in]   errorType        device error type.
  /// @param[out]  isActivated      Indicates if the device brake delay isActivated.
  /// @param[out]  delayTimeMs      Reaction delay time to the type of error.
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getErrorBrakeDelayInfoMock(void* handle, const AxisProfileDeviceErrorType errorType, bool& isActivated, double& delayTimeMs);

  /// @brief Mockup of method AXISPROFILE_getErrorRactionDynLimit.
  /// @param[in]   handle           Handle of the interface (use class AxisProfileCommandMockHandle).
  /// @param[in]   errorType        Device error type.
  /// @param[out]  deceleration     Brake deceleration.
  /// @param[out]  jerk             Brake jerk.
  /// @return      Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getErrorBrakeDynLimitMock(void* handle, const AxisProfileDeviceErrorType errorType, double& deceleration, double& jerk);

  /// @brief Mockup of method AXISPROFILE_getPositionDataFormat.
  /// @param[in]  handle          Handle of the interface.
  /// @param[out] posDataFormat   The position data format(absolute or module)
  /// @return     Return value indicating success or error.
  extern "C" AxisProfileResult AXISPROFILE_getPositionDataFormatMock(void* handle, AxisProfilePosDataFormat & posDataFormat);

  /// @brief Mockup of method check and set modulo value of the device.
  /// @param[in]  handle          Handle of the interface
  /// @param[in]  modValue        Modulo value that to be checked (when driver has build modulo conversion) or set (when axisprofile handles modulo conversion)
  /// @param[out] dynErrTxt       Dynamic error text when error returns, used for diagnosis of the caller
  /// @return                     Status of the function call: STS_OK or an error code on failure.
  extern "C" AxisProfileResult AXISPROFILE_checkAndSetModuloMock(void* handle, double modValue, std::string & dynErrTxt);

  /// @brief Mockup of the function checking if the functionality is supported or not.
  /// @param[in]  handle     Handle of the interface.
  /// @param[in]  function   The function to be checked.
  /// @param[out] errorInfo  Dynamic error information when function is not supported, used for diagnosis of the caller.
  /// @return                 TRUE indicates that the function is supported, FALSE indicates not support.
  extern "C" bool AXISPROFILE_checkFunctionSupportedMock(void* handle, const FunctionSupport function, FunctionSupportErrorInfo & errorInfo);

  /// @brief Mockup of the function checks if the axis is standstill
  /// @param[in]  handle     Handle of the interface.
  /// @param[out] standStill TRUE indicates that the axis is standstill, FALSE indicates not standstill.
  /// @return                 Status of the function call: STS_OK or an error code on
  extern "C" AxisProfileResult AXISPROFILE_checkStandstillMock(void* handle, bool& standStill);

  /// @brief Mockup of getting rate torque force of the load in IS unit
  /// @param[in]  handle                Handle of the interface
  /// @param[out] rateTorqueForce       Rate torque for rotary axis or rate force for linear axis in IS unit
  /// @param[out] dynErrTxt             Dynamic error text when error returns, used for diagnosis of the caller
  /// @return                 Status of the function call: STS_OK or an error code on
  extern "C" AxisProfileResult AXISPROFILE_getRateTorqueForceMock(void* handle, double& rateTorqueForce, std::string& dynErrTxt);

  /// @brief This function checks axis has homed or not.
  /// @param[in]  handle                Handle of the interface.
  /// @param[out] homed                 True indicates that the axis has homed, false indicates that the axis has not homed.
  /// @return                           Status of the function call: STS_OK or an error code
  extern "C" AxisProfileResult AXISPROFILE_checkAxisHomedMock(void* handle, bool& homed);
}
