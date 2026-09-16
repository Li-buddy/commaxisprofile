#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface of command interface.
///
/// @details
/// The command execution takes several cycles which depends on the command type, e.g. referencingAxis (homing) can only be 
/// complete when homing sensor detected and drive reacted on it.
/// 
/// All methods in this interface are designed to be realtime capable, which means they can be cycle invoked in realtime tasks.
/// @getCmdState is to read command state which indicates control has signaled to device start/abort the execution of command.
/// @getCmdState is to read command status in device which indicates command execution state in device.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

///  @par List of available functions in this part of the axisprofile command.
/// <TABLE>
/// <TR><TH>              Function                    </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e">    getCmdState                 </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e">    resetCmd                    </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e">    abortCmd                    </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e">    getCmdStatusInDevice        </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// 
/// <TR><TD id="st_e">    clearDeviceError            </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e">    clearDeviceError2           </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e">    setAbsolutePosition         </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e">    referencingAxis             </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e">    fixedStop                   </TD><TD> 1.2.0             </TD><TD>       </TD></TR>
/// </TABLE>

#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
  /// @brief This is the interface of axisprofile command.
  typedef struct IAxisProfileCmdC
  {
    /// @brief pointer to the instantiated object
    void* handle;

    /// @brief Get acutal state of axisprofile command
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[in]  cmdType       command type
    /// @return state of command
    ::comm::axisprofile::CmdState(*getCmdState)(void* handle, ::comm::axisprofile::CmdType cmdType);

    /// @brief Reset a axisprofile command
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[in]  cmdType       command type
    /// @return Status of the function call, STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*resetCmd)(void* handle, ::comm::axisprofile::CmdType cmdType);

    /// @brief Abort a axisprofile command
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[in]  cmdType       command type
    /// @return Status of the function call, STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*abortCmd)(void* handle, ::comm::axisprofile::CmdType cmdType);

    /// @brief Get command status in device
    ///
    /// @note The function is realtime capable, command status is only updated when the command is in working,
    /// which means the command is in ACTIVATING, ACTIVE or ABORTING state.
    /// 
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[in]  cmdType       command type
    /// @return Status of command in the device
    ::comm::axisprofile::CmdStatusInDevice(*getCmdStatusInDevice)(void* handle, ::comm::axisprofile::CmdType cmdType);

    /// @brief Placeholders reserved for command operation extension
    void* reservedOperation[20];

    /// @brief Command of clearing device error
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*clearDeviceError)(void* handle, ::comm::axisprofile::CmdType& cmdType);

    /// @brief Command of clearing device error wrapper that can be cycle invoked.
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @return Status of the function call, STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*clearDeviceError2)(void* handle);

    /// @brief Command of setting new absolute position to device, only ctrlX drive specific function.
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[in]  newPos        new absolute position
    /// @param[in]  encoder       index of encoder to be set
    /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*setAbsolutePosition)(void* handle, double newPos, ::comm::axisprofile::AxisProfileEncoder encoder, ::comm::axisprofile::CmdType& cmdType);

    /// @brief Command of triggering searching new reference of device
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[in]  newPos        new position to be set when referencing has done
    /// @param[in]  encoder       index of encoder to be set
    /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*referencingAxis)(void* handle, double newPos, ::comm::axisprofile::AxisProfileEncoder encoder, ::comm::axisprofile::CmdType& cmdType);

    /// @brief Command of triggering fixed stop
    ///
    /// @param[in]  handle          pointer to the instantiated object
    /// @param[in]  clampingTorque  Clamping torque of fixedstop
    /// @param[out] cmdType         type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*fixedStop)(void* handle, double clampingTorque, ::comm::axisprofile::CmdType& cmdType);

    /// @brief Command of triggering setting new reference without movement.
    ///
    /// @note This command is designed for the application which only want to set new reference without movement after homing.
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[in]  newPos        new position to be set when referencing has done
    /// @param[in]  encoder       index of encoder to be set
    /// @param[in]  refMode       reference mode, could be set new reference to device or control or do nothing after homing.
    /// @param[out] cmdType       type of the command that used to maintain (@ref getCmdState, @ref resetCmd, @ref abortCmd)
    /// @return Status of the function call, STS_OK or an error code on failure
    ::comm::axisprofile::AxisProfileResult(*setNewReferenceWithoutMovement)(void* handle, double newPos, ::comm::axisprofile::AxisProfileEncoder encoder, ::comm::axisprofile::RefMode refMode, ::comm::axisprofile::CmdType& cmdType);

    /// @brief This function sets the given operation mode to the device, and then check if the operation mode is set to the target value.
    ///        It combines the set operation mode command and the check of the operation mode in one function, which is useful for cyclic calls.
    /// note:  This function is intended for use in real-time control loops where non-blocking behavior is required.
    ///        why need cyclic:
    ///        In CoE protocol, the operation mode is set by writing to the operation mode object in the device.
    ///        This progress can be done by SDO or PDO, so this interface is provided to support both methods.
    ///        The SDO progress need more than one cycle to finish, before finish, will return STS_PENDING.
    ///        If the operation mode data is offered by PDO, this function will return STS_OK with much less cycles.
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @param[in]  opMode The target operation mode to be set on the device. This should be a valid AxisProfileOperationMode value.
    /// @return STS_OK will be returned if the operation mode is set to the desired value, and also checked.
    ///         STS_OK will be returned if the operation mode already in target mode before command execution.
    ::comm::axisprofile::AxisProfileResult(*switchOperationModeWithCheck)(void* handle, const ::comm::axisprofile::AxisProfileOperationMode opMode);

    void* reserved[99];  ///< reservation
  } IAxisProfileCmdC;

#ifdef __cplusplus
}
#endif
