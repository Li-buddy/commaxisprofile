#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface for motion bundles for fieldbus cycle data handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

///  @par List of available functions in this part of the axisprofile cycle data handling.
/// <TABLE>
/// <TR><TH>           Function                                 </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> getActualValuesDevice                    </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> setCommandValuesDevice                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getDigitalInputValueBit                  </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkActualValueAvailabilityNRT          </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getSafetyOperationMode                   </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getSingleUserPDODataFromDevice           </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> setSingleUserPDODataToDevice             </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkSingleUserPDOActiveFromDevice       </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> checkSingleUserPDOActiveToDevice         </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getEffectiveTimeOfActOutput              </TD><TD> 1.1.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getLatchTimeOfActInput                   </TD><TD> 1.1.0             </TD><TD>       </TD></TR>
/// </TABLE>

#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

  /// @brief This is the C-Interface for the axisprofile cyclic data interface 'IAxisProfileCyclicData'.
  typedef struct IAxisProfileCyclicDataC
  {
    /// @brief This is pointer to the instantiated object
    void *handle;

    /// @brief This function gets actual value of the device
    /// 
    /// @param[in]  handle          Handle of the interface
    /// @param[out] actualValues    Actual values read from the device
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*getActualValuesDevice)(void *handle, ::comm::axisprofile::ActualValuesDevice* actualValues);

    /// @brief This function set command value to device
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[out] commandValues   Command values to be set to the device
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*setCommandValuesDevice)(void *handle, ::comm::axisprofile::CommandValuesDevice commandValues);

    /// @brief This function get digital input value by bit selection
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  valueType       Bit type selection
    /// @param[out] inputValue      Value of the digital input
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*getDigitalInputValueBit)(void *handle, ::comm::axisprofile::DigitalInputValueBitType valueType, bool& inputValue);

    /// @brief This function checks if the selected value is available in acutal value structure, @ref getActualValuesDevice
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  valueType       Value ID selection
    /// @param[out] errorInfo       Not available value information (reason)
    /// @return                     TRUE when the value is available, FALSE when the value is not available.
    bool(*checkActualValueAvailabilityNRT)(void* handle, ::comm::axisprofile::AxisProfileValueTypeID valueType, ::comm::axisprofile::DataAccessErrorInfo& errorInfo);

    /// @brief This function gets output synchronized time of fieldbus's last frame cycle. The time start from 1970.1.1.0.0.0 in unit of second.
    void* deprecatedGetLastOutputSyncTime;

    /// @brief This function gets input synchronized time of fieldbus's last frame cycle. The time start from 1970.1.1.0.0.0 in unit of second
    void* deprecatedGetLastInputSyncTime;

    /// @brief This function gets mode of safety operation
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[out] mode            Safety operation mode
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    /// @retval STS_OK                   the getting of safety operation mode is success.
    /// @retval ERR_ERR_DATA_NOT_MAPPED  indicates safety-selection or safety-status was not mapped;
    /// @retval ERR_WRONG_DEVICE_STATE   the state machine of axis profile is not in running state.
    /// @retval ERR_EXEC_NULL            null pointer occurred.
    ::comm::axisprofile::AxisProfileResult(*getSafetyOperationMode)(void* handle, ::comm::axisprofile::SafetyOperationMode& mode);

    /// @brief This function gets user defined cycle data (POD of CoE, AT of SoE) for the device
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  id              User defined cycle data ID
    /// @param[out] actualPDOInfo   User defined cycle data
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*getSingleUserPDODataFromDevice)(void* handle, ::comm::axisprofile::UserPDOID id, ::comm::axisprofile::UserPDODataInfo& actualPDOInfo);

    /// @brief This function sets user defined cycle data (POD of CoE, MDT of SoE) to the device
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  id              User defined cycle data ID
    /// @param[out] newPDOInfo      New data
    /// @return                     Status of the function call: STS_OK or an error code on failure.
    ::comm::axisprofile::AxisProfileResult(*setSingleUserPDODataToDevice)(void* handle, ::comm::axisprofile::UserPDOID id, ::comm::axisprofile::UserPDODataInfo newPDOInfo);

    /// @brief This function check whether single user input cycle data (POD of CoE, AT of SoE) is active
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  id              User defined cycle data ID
    /// @return                     TRUE indicate whether activated.
    bool(*checkSingleUserPDOActiveFromDevice)(void* handle, ::comm::axisprofile::UserPDOID id);

    /// @brief This function check whether single user output cycle data (POD of CoE, AT of SoE) is active
    ///
    /// @param[in]  handle          Handle of the interface
    /// @param[in]  id              User defined cycle data ID
    /// @return                     TRUE indicate whether activated.
    bool(*checkSingleUserPDOActiveToDevice)(void* handle, ::comm::axisprofile::UserPDOID id);

    /// @brief This function gets time that actual output is effected in the drive. The time start from 1970.1.1.0.0.0 in unit of second.
    ///
    /// @note  The acutal output is not effective immediately when motion write it to axisprofile. The new output takes 1 fieldbus cycle to be written to drive.
    ///        The drive may take fine interpolation buffer.
    ///        In rexroth drive buffer is 2~3 depth depend on interpolation type (P-0-0187 of ctrlX drive).
    ///        In third party driver, we assume the output is effective immediately.
    /// 
    ///        So the output synchronized time is 1~4 cycles time after the actual output is written to axisprofile.
    /// 
    /// @param[in]  handle          Handle of the interface
    /// @return                     Effective time of acutal output, in unit of second.
    double(*getEffectiveTimeOfActOutput)(void* handle);

    /// @brief This function gets time that actual input is latched. The time start from 1970.1.1.0.0.0 in unit of second.
    ///
    /// @note  The actual input is latched 2 cycles ago of sync0's input latch point of fieldbus in drive.
    /// 
    /// @param[in]  handle          Handle of the interface
    /// @return                     Latch time of actual input, in unit of second.
    double(*getLatchTimeOfActInput)(void* handle);

    void *reserved[100];  ///< reservation
  } IAxisProfileCyclicDataC;

#ifdef __cplusplus
}
#endif
