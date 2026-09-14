#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the general axis profile interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_c.h"
#include "mock_axisprofile_cmd.h"
#include "mock_axisprofile_command.h"
#include "mock_axisprofile_cyclic_data.h"
#include "mock_axisprofile_device_param.h"
#include "mock_axisprofile_diag_msg.h"
#include "mock_axisprofile_probe.h"
#include "mock_axisprofile_property.h"
#include "mock_axisprofile_status_control.h"
#include <cstdint>

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
  class AxisProfileMockHandle
  {
  public: // methods

    /// @brief Constructor
    AxisProfileMockHandle()
    {
      // Profile command mockup
      m_profileCommandMock.handle = &m_commandMockHandle;
      m_profileCommandMock.clearDeviceError = AXISPROFILE_clearDeviceErrorMock;
      m_profileCommandMock.setAndCheckPositiveStop = AXISPROFILE_setAndCheckPositiveStopMock;
      m_profileCommandMock.setAndCheckClampingTorque = AXISPROFILE_setAndCheckClampingTorqueDeprecatedMock;
      m_profileCommandMock.getClampingTorque = AXISPROFILE_getClampingTorqueDeprecatedMock;
      m_profileCommandMock.checkStandStill = AXISPROFILE_checkStandStillMock;
      m_profileCommandMock.setAndCheckStandStillWindow = AXISPROFILE_setAndCheckstandStillWindowMock;
      m_profileCommandMock.getStandStillWindow = AXISPROFILE_getStandStillWindowMock;
      m_profileCommandMock.activateCyclicStandStillCheck = AXISPROFILE_activateCyclicStandStillCheckMock;
      m_profileCommandMock.deactivateCyclicStandStillCheck = AXISPROFILE_deactivateCyclicStandStillCheckMock;
      m_profileCommandMock.setAbsolutePosition = AXISPROFILE_setAbsolutePositionMock;
      m_profileCommandMock.resetAbsolutePosition = AXISPROFILE_resetAbsolutePositionMock;
      m_profileCommandMock.referencingAxis = AXISPROFILE_referencingAxisMock;
      m_profileCommandMock.resetReferencingAxis = AXISPROFILE_resetReferencingAxisMock;
      m_profileCommandMock.checkAxisHomed = AXISPROFILE_checkAxisHomedMockDeprecated;
      m_profileCommandMock.setReferencingPosition = AXISPROFILE_setReferencingPositionMock;

      m_profileCommandMock.resetAxisProfileCommandState = AXISPROFILE_resetAxisprofileCommandStateMock;
      m_profileCommandMock.abortAxisProfileCommand = AXISPROFILE_abortAxisprofileCommandMock;
      m_profileCommandMock.getAxisProfileCommandState = AXISPROFILE_getCommandStateMock;
      m_profileCommandMock.getCommandsAtDeterminedState = AXISPROFILE_getCommandsAtDeterminedStateMock;

      // Profile cyclic data mockup
      m_profileCyclicDataMock.handle = &m_cyclicDataMockHandle;
      m_profileCyclicDataMock.getActualValuesDevice = AXISPROFILE_getActualValuesDeviceMock;
      m_profileCyclicDataMock.setCommandValuesDevice = AXISPROFILE_setCommandValuesDeviceMock;
      m_profileCyclicDataMock.getDigitalInputValueBit = AXISPROFILE_getDigitalInputValueBitMock;
      m_profileCyclicDataMock.checkActualValueAvailabilityNRT = AXISPROFILE_checkActualValueAvailabilityNRTMock;
      m_profileCyclicDataMock.getSafetyOperationMode = AXISPROFILE_getSafetyOperationModeMock;
      m_profileCyclicDataMock.getSingleUserPDODataFromDevice = AXISPROFILE_getSingleUserPDODataFromDeviceMock;
      m_profileCyclicDataMock.setSingleUserPDODataToDevice = AXISPROFILE_setSingleUserPDODataToDeviceMock;
      m_profileCyclicDataMock.checkSingleUserPDOActiveFromDevice = AXISPROFILE_checkSingleUserPDOActiveFromDeviceMock;
      m_profileCyclicDataMock.checkSingleUserPDOActiveToDevice = AXISPROFILE_checkSingleUserPDOActiveToDeviceMock;
      m_profileCyclicDataMock.getEffectiveTimeOfActOutput = AXISPROFILE_getEffectiveTimeOfActOutputMock;
      m_profileCyclicDataMock.getLatchTimeOfActInput = AXISPROFILE_getLatchTimeOfActInputMock;

      // Profile status control mockup
      m_profileStatusControlMock.handle = &m_statusControlMockHandle;

      m_profileStatusControlMock.getStatusWord = AXISPROFILE_getStatusWordMock;
      m_profileStatusControlMock.checkOperationMode = AXISPROFILE_checkOperationModeMock;
      m_profileStatusControlMock.checkOperationModeSupported = AXISPROFILE_checkOperationModeSupportedMock;
      m_profileStatusControlMock.getOperationMode = AXISPROFILE_getOperationModeMock;
      m_profileStatusControlMock.checkPowerMode = AXISPROFILE_checkPowerModeMock;
      m_profileStatusControlMock.getPowerMode = AXISPROFILE_getPowerModeMock;
      m_profileStatusControlMock.checkError = AXISPROFILE_checkErrorMock;
      m_profileStatusControlMock.cyclicStandStillCheck = AXISPROFILE_cyclicStandStillCheckMock;
      m_profileStatusControlMock.isDeviceFollowsControl = AXISPROFILE_isDeviceFollowsControl;

      m_profileStatusControlMock.setControlWord = AXISPROFILE_setControlWordMock;
      m_profileStatusControlMock.setOperationMode = AXISPROFILE_setOperationModeMock;
      m_profileStatusControlMock.setPowerMode = AXISPROFILE_setPowerModeMock;
      m_profileStatusControlMock.setDriveHaltMode = AXISPROFILE_setDriveHaltModeMock;
      m_profileStatusControlMock.getDriveHaltMode = AXISPROFILE_getDriveHaltModeMock;
      m_profileStatusControlMock.deviceHasWarning = AXISPROFILE_deviceHasWarningMock;
      m_profileStatusControlMock.checkReadyToOperate = AXISPROFILE_checkReadyToOperateMock;

      // Profile diagnostic message mockup
      m_profileDiagMsgMock.handle = &m_diagMsgMockHandle;
      m_profileDiagMsgMock.getErrorType = AXISPROFILE_getErrorTypeMock;
      m_profileDiagMsgMock.getErrorCode = AXISPROFILE_getErrorCodeMock;

      // Deprecated, Profile booting config mockup
      m_profileBootingConfigMock.handle = &m_deviceParamMockHandle;
      m_profileBootingConfigMock.checkBrakeDelaySupport = AXISPROFILE_checkBrakeDelaySupportMock;
      m_profileBootingConfigMock.getErrorBrakeDelayInfo = AXISPROFILE_getErrorBrakeDelayInfoMock;
      m_profileBootingConfigMock.getErrorBrakeDynLimit = AXISPROFILE_getErrorBrakeDynLimitMock;
      m_profileBootingConfigMock.getPositionDataFormat = AXISPROFILE_getPositionDataFormatMock;
      m_profileBootingConfigMock.checkAndSetModulo = AXISPROFILE_checkAndSetModuloMock;
      m_profileBootingConfigMock.checkFunctionSupported = AXISPROFILE_checkFunctionSupportedMock;
      m_profileBootingConfigMock.depricatedGetDCSync0ShiftTime = nullptr;

      // Profile device parameter mockup
      m_profileDeviceParamMock.handle = &m_deviceParamMockHandle;
      m_profileDeviceParamMock.checkBrakeDelaySupport = AXISPROFILE_checkBrakeDelaySupportMock;
      m_profileDeviceParamMock.getErrorBrakeDelayInfo = AXISPROFILE_getErrorBrakeDelayInfoMock;
      m_profileDeviceParamMock.getErrorBrakeDynLimit = AXISPROFILE_getErrorBrakeDynLimitMock;
      m_profileDeviceParamMock.getPositionDataFormat = AXISPROFILE_getPositionDataFormatMock;
      m_profileDeviceParamMock.checkAndSetModulo = AXISPROFILE_checkAndSetModuloMock;
      m_profileDeviceParamMock.checkFunctionSupported = AXISPROFILE_checkFunctionSupportedMock;
      m_profileDeviceParamMock.checkStandstill = AXISPROFILE_checkStandstillMock;
      m_profileDeviceParamMock.getRateTorqueForce = AXISPROFILE_getRateTorqueForceMock;
      m_profileDeviceParamMock.checkAxisHomed = AXISPROFILE_checkAxisHomedMock;

      // Profile probe mockup
      m_profileProbeMock.handle = &m_probeMockHandle;
      m_profileProbeMock.probeBaseConfig = AXISPROFILE_probeBaseConfigMock;
      m_profileProbeMock.probeExtConfig = AXISPROFILE_probeExtConfigMock;
      m_profileProbeMock.probeStatusCtrl = AXISPROFILE_probeStatusCtrlMock;
      m_profileProbeMock.getProbeActualValuesRT = AXISPROFILE_getProbeActualValuesRTMock;
      m_profileProbeMock.onlineChangeParams = AXISPROFILE_onlineChangeParamsMock;
      m_profileProbeMock.getProbeValues = AXISPROFILE_getProbeValuesMock;
      m_profileProbeMock.abortAndResetProbe = AXISPROFILE_abortAndResetProbeMock;
      m_profileProbeMock.isProbeSupported = AXISPROFILE_isProbeSupportedMock;

      // profile cmd mockup
      m_profileCmdMock.handle = &m_cmdMockHandle;
      m_profileCmdMock.getCmdState = AXISPROFILE_getCmdStateMock;
      m_profileCmdMock.resetCmd = AXISPROFILE_resetCmdMock;
      m_profileCmdMock.abortCmd = AXISPROFILE_abortCmdMock;
      m_profileCmdMock.getCmdStatusInDevice = AXISPROFILE_getCmdStatusInDeviceMock;
      m_profileCmdMock.clearDeviceError = AXISPROFILE_clearDeviceErrorMockCmd;
      m_profileCmdMock.clearDeviceError2 = AXISPROFILE_clearDeviceError2MockCmd;
      m_profileCmdMock.setAbsolutePosition = AXISPROFILE_setAbsolutePositionMockCmd;
      m_profileCmdMock.referencingAxis = AXISPROFILE_referencingAxisMockCmd;
      m_profileCmdMock.fixedStop = AXISPROFILE_fixedStopMockCmd;
      m_profileCmdMock.setNewReferenceWithoutMovement = AXISPROFILE_setNewReferenceWithoutMovementMockCmd;

      // Profile property mockup
      m_profilePropertyMock.handle = &m_propertyMockHandle;
      m_profilePropertyMock.setDeviceAddress = AXISPROFILE_setDeviceAddressPropertyMock;
      m_profilePropertyMock.getDeviceAddress = AXISPROFILE_getDeviceAddressPropertyMock;
      m_profilePropertyMock.getProfileType = AXISPROFILE_getProfileTypePropertyMock;
      m_profilePropertyMock.getDeviceState = AXISPROFILE_getDeviceStatePropertyMock;
      m_profilePropertyMock.getDeviceScalingType = AXISPROFILE_getDeviceScalingTypePropertyMock;
      m_profilePropertyMock.getProfileCategory = AXISPROFILE_getProfileCategoryPropertyMock;
      m_profilePropertyMock.getCfgedFieldbusMasterName = AXISPROFILE_getCfgedFieldbusMasterNameMock;
    }

    /// @brief Set the return value of class methods.
    /// Note: Do not set 'm_returnValue' directly in your test or CppCheck may generate errors because of the extern C implementation in some cases.
    void setReturnValue(AxisProfileResult value) { m_returnValue = value; }

  public: // data

    AxisProfileResult m_returnValue = AxisProfileResult::STS_OK;      ///< Result of member functions

    // Handle Classes
    AxisProfileCommandMockHandle m_commandMockHandle{};               ///< Command mock handle
    AxisProfileCyclicDataMockHandle m_cyclicDataMockHandle{};         ///< Cyclic data mock handle
    AxisProfileStatusControlMockHandle m_statusControlMockHandle{};   ///< Status control mock handle
    AxisProfileDiagMsgMockHandle m_diagMsgMockHandle{};               ///< Diagnostic message mock handle
    AxisProfileDeviceParamMockHandle m_deviceParamMockHandle{};       ///< Device parameter mock handle
    AxisProfileProbeMockHandle m_probeMockHandle{};                   ///< Probe mock handle
    AxisProfileCmdMockHandle m_cmdMockHandle {};                      ///< Cmd mock handle
    AxisProfilePropertyMockHandle m_propertyMockHandle{};             ///< Property mock handle

    IAxisProfileCommandC m_profileCommandMock{};                ///< Mockup of the command interface
    IAxisProfileCyclicDataC m_profileCyclicDataMock{};          ///< Mockup of the cyclic data interface
    IAxisProfileStatusControlC m_profileStatusControlMock{};    ///< Mockup of the status control interface
    IAxisProfileDiagMsgC m_profileDiagMsgMock{};                ///< Mockup of the diagnostic message interface
    IAxisProfileBootingConfigC m_profileBootingConfigMock {};   ///< Deprecated. Mockup of the booting config interface
    IAxisProfileDeviceParamC m_profileDeviceParamMock{};        ///< Mockup of the device parameter interface
    IAxisProfileProbeC m_profileProbeMock{};                    ///< Mockup of the probe interface
    IAxisProfileCmdC m_profileCmdMock {};                       ///< Mockup of the cmd interface  
    IAxisProfilePropertyC m_profilePropertyMock{};              ///< Mockup of the property interface
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // -------------------------------------- C-Interface Method-Mockups ---------------------------------------- //

  /// @brief Mockup of method C_getCommandInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profileCommandMock' of the handle is returned.
  extern "C" const IAxisProfileCommandC* AXISPROFILE_getCommandInterfaceCMock(void* handle);

  /// @brief Mockup of method C_getStatusControlInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profileStatusControlMock' of the handle is returned.
  extern "C" const IAxisProfileStatusControlC* AXISPROFILE_getStatusControlInterfaceCMock(void* handle);

  /// @brief Mockup of method C_getDiagMsgInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profileDiagMsgMock' of the handle is returned.
  extern "C" const IAxisProfileDiagMsgC * AXISPROFILE_getDiagMsgInterfaceCMock(void* handle);

  /// @brief Mockup of method C_getCyclicDataInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profileCyclicDataMock' of the handle is returned.
  extern "C" const IAxisProfileCyclicDataC* AXISPROFILE_getCyclicDataInterfaceCMock(void* handle);

  /// @brief Deprecated, Mockup of method C_getBootingConfigInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profileBootingConfigMock' of the handle is returned.
  extern "C" const IAxisProfileBootingConfigC* AXISPROFILE_getBootingConfigInterfaceCMock(void* handle);

  /// @brief Mockup of method C_getDeviceParamInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profileDeviceParamMock' of the handle is returned.
  extern "C" const IAxisProfileDeviceParamC* AXISPROFILE_getDeviceParamInterfaceCMock(void* handle);

  /// @brief Mockup of method C_getProbeInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profileProbeMock' of the handle is returned.
  extern "C" const IAxisProfileProbeC* AXISPROFILE_getProbeInterfaceCMock(void* handle);

  /// @brief Mockup of method C_getCmdInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profileCmdMock' of the handle is returned.
  extern "C" const IAxisProfileCmdC* AXISPROFILE_getCmdInterfaceCMock(void* handle);

  /// @brief Mockup of method C_getPropertyInterface().
  /// @param[in]   handle           Interface handle (use class AxisProfileMockHandle)
  /// @return                       Member variable 'm_profilePropertyMock' of the handle is returned.
  extern "C" const IAxisProfilePropertyC* AXISPROFILE_getPropertyInterfaceCMock(void* handle);
}
