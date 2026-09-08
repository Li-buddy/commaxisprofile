#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface for motion bundles for diagnosis message.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

///  @par List of available functions in this part of the axisprofile device diagnosis handling.
/// <TABLE>
/// <TR><TH>           Function                       </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> getErrorType                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getErrorCode                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// </TABLE>

#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
  /// @brief This is the C-Interface for the axisprofile cyclic data interface 'IAxisProfileDiagMsg'.
  typedef struct IAxisProfileDiagMsgC
  {
    /// @brief pointer to the instantiated object
    void* handle;

    /// @brief Get acutal the error type of the drive
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @return the error type
    ::comm::axisprofile::AxisProfileDeviceErrorType(*getErrorType)(void* handle);

    /// @brief Get acutal the error code of the drive
    ///
    /// @param[in]  handle        pointer to the instantiated object
    /// @return the error code
    std::uint32_t(*getErrorCode)(void* handle);

    void* reserved[100];  ///< reservation
  } IAxisProfileDiagMsgC;

#ifdef __cplusplus
}
#endif
