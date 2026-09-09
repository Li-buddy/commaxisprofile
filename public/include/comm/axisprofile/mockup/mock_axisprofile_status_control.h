#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axis profile status/control interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_status_control_c.h"

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
  class AxisProfileStatusControlMockHandle
  {
  public: // methods

    /// @brief Set the return value of class methods.
    /// Note: Do not set 'm_returnValue' directly in your test or CppCheck may generate errors because of the extern C implementation in some cases.
    void setReturnValue(AxisProfileResult value) { m_returnValue = value; }

  public: // data

    bool m_deviceError = false;           ///< Flag that indicates, if a device error is currently simulated
    bool m_standStill  = false;           ///< Flag that indicates, if a device standstill is currently simulated
    bool m_opModeSupported = false;       ///< Flag that indicates, if a operation mode supported is currently simulated
    bool m_isDeviceFollowsControl = false;///< Flag that indicates, if a status of command value processing
    bool m_haltEnable = false;            ///< Flag that indicates, if set the drive to halt state
    bool m_deviceHasWarning = false;      ///< Flag that indicates, if a device has warning

    AxisProfilePowerMode m_powerMode = AxisProfilePowerMode::POWER_ON;                      ///< Current power mode
    AxisProfileOperationMode m_operationMode = AxisProfileOperationMode::POSITIONING;       ///< Current operation mode
    AxisProfileResult m_returnValue = AxisProfileResult::STS_OK;                            ///< Return value of C-Methods
    OpModeErrorInfo m_opModeErrorInfo{ OpModeSupportedErrorType::ERROR_NONE, "supported" }; ///< Return error information for operation mode supported checking
    ReadyToOperate m_readyToOperate { ReadyToOperate::DRIVE_NOT_READY };                    ///< Is device ready for operation
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // -------------------------------------- C-Interface Method-Mockups ---------------------------------------- //


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // --------------------------------------------- Statusword ----------------------------------------------- //

  /// @brief Mockup of method AXISPROFILE_getStatusWord().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @return The current statusword.
  extern "C" std::uint16_t AXISPROFILE_getStatusWordMock(void* handle);

  /// @brief Mockup of method AXISPROFILE_checkOperationMode().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[in]   opMode   Operation mode that will be compared with 'm_operationMode' of the handle
  /// @return  True if the given opMode matches 'm_operationMode' of the handle, false if not.
  extern "C" bool AXISPROFILE_checkOperationModeMock(void* handle, const AxisProfileOperationMode opMode);

  /// @brief Mockup of method AXISPROFILE_checkOperationModeSupported().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[in]   opMode   Operation mode that will be checked if it is supported
  /// @param[out]  errorInfo   Error information of operation mode
  /// @return  Member variable 'm_opModeSupported' of the handle is returned.
  extern "C" bool AXISPROFILE_checkOperationModeSupportedMock(void* handle, const AxisProfileOperationMode opMode, OpModeErrorInfo& errorInfo);

  /// @brief Mockup of method AXISPROFILE_getOperationMode().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @return  Member variable 'm_operationMode' of the handle is returned.
  extern "C" AxisProfileOperationMode AXISPROFILE_getOperationModeMock(void* handle);

  /// @brief Mockup of method AXISPROFILE_readOperationMode().
  /// @param[in]   handle          Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[out]  operationMode   Read operation mode from handle
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_readOperationModeMock(void* handle, AxisProfileOperationMode& operationMode);

  /// @brief Mockup of method AXISPROFILE_checkPowerMode().
  /// @param[in]   handle      Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[in]   powerMode   Power mode that will be compared with 'm_powerMode' of the handle
  /// @return  True if the given powerMode matches 'm_powerMode' of the handle, false if not.
  extern "C" bool AXISPROFILE_checkPowerModeMock(void* handle, const AxisProfilePowerMode powerMode);

  /// @brief Mockup of method AXISPROFILE_getPowerMode().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @return  Member variable 'm_powerMode' of the handle is returned.
  extern "C" AxisProfilePowerMode AXISPROFILE_getPowerModeMock(void* handle);

  /// @brief Mockup of method AXISPROFILE_checkError().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @return  Member variable 'm_deviceError' of the handle is returned.
  extern "C" bool AXISPROFILE_checkErrorMock(void* handle);

  /// @brief Mockup of method AXISPROFILE_cyclicStandStillCheck().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @return  Member variable 'm_standStill' of the handle is returned.
  extern "C" bool AXISPROFILE_cyclicStandStillCheckMock(void* handle);

  /// @brief Mockup of method AXISPROFILE_isDeviceFollowsControl().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @return  Member variable 'm_isDeviceFollowsControl' of the handle is returned.
  extern "C" bool AXISPROFILE_isDeviceFollowsControl(void* handle);


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // -------------------------------------------- Controlword ----------------------------------------------- //

  /// @brief Mockup of method AXISPROFILE_setControlWord().
  /// @param[in]   handle        Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[in]   controlWord   The controlword
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setControlWordMock(void* handle, std::uint16_t controlWord);

  /// @brief Mockup of method AXISPROFILE_setOperationMode().
  /// @param[in]   handle   Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[in]   opMode   The new operation mode
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setOperationModeMock(void* handle, const AxisProfileOperationMode opMode);

  /// @brief Mockup of method AXISPROFILE_setPowerMode().
  /// @param[in]   handle      Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[in]   powerMode   The new power mode
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setPowerModeMock(void* handle, const AxisProfilePowerMode powerMode);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // --------------------------------------------- Drive Halt ----------------------------------------------- //

  /// @brief Mockup of method AXISPROFILE_setDriveHaltModeMock().
  /// @param[in]   handle      Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[in]   haltEnable  Enable or disable drive halt
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setDriveHaltModeMock(void *handle, const bool haltEnable);

  /// @brief Mockup of method AXISPROFILE_getDriveHaltModeMock().
  /// @param[in]   handle         Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @param[out]  isHaltEnabled  Is drive halt enabled or not
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getDriveHaltModeMock(void *handle, bool& isHaltEnabled);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // --------------------------------------------- Statusword ----------------------------------------------- //

  /// @brief Mockup of method AXISPROFILE_deviceHasWarning().
  /// @param[in]  handle       Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @return     Member variable 'm_deviceHasWarning' of the handle is returned.
  extern "C" bool AXISPROFILE_deviceHasWarningMock(void *handle);

  /// @brief Mockup of method AXISPROFILE_checkReadyToOperate().
  /// @param[in]  handle       Interface handle (use class AxisProfileStatusControlMockHandle)
  /// @return     Member variable 'm_readyToOperate' of the handle is returned.
  extern "C" ReadyToOperate AXISPROFILE_checkReadyToOperateMock(void *handle);
}
