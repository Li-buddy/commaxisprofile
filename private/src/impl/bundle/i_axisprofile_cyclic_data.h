#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and implements the main
///        interface for getting and setting cyclic data (e.g. position, velocity, ...).
///        This is a sub-interface of 'IAxisProfile'.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"

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
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for getting and setting cyclic data.
  class IAxisProfileCyclicData
  {
  public: // methods

    virtual ~IAxisProfileCyclicData() noexcept = default;

    /// @brief This method gets the at data container of the axisprofile.
    /// @param[out]  actualValues   Pointer to actual values container the axisprofile data should be stored to.
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult getActualValuesDevice(ActualValuesDevice* const actualValues) = 0;

    /// @brief This method copies the given command values container to the data container in the axisprofile.
    /// @param[in]  commandValues   Commanded values container that should be copied.
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult setCommandValuesDevice(CommandValuesDevice commandValues) = 0;

    /// @brief This method gets the digital input value for a single bit of the axisprofile.
    /// @param[in]   valueType   The type of digital input value bit
    /// @param[out]  inputValue   Actual value of the specific bit
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult getDigitalInputValueBit(DigitalInputValueBitType valueType, bool& inputValue) = 0;

    /// @brief This method check data for NRT usage. 
    /// @param[in] valueTypeID  Enum to define the target value type of user.
    /// @param[out] errorInfo   Once the data unavailable, return the error information.
    /// @return     Indicate whether the value return correctly.
    virtual bool checkActualValueAvailabilityNRT(AxisProfileValueTypeID valueTypeID, DataAccessErrorInfo& errorInfo) = 0;

    /// @brief Get the current safety operation mode.
    /// @param[out]  mode  indicates the current safety operation mode.
    /// @return            result value indicating success or error.
    virtual AxisProfileResult getSafetyOperationMode(SafetyOperationMode& mode) = 0;

    /// @brief This method gets the single user PDO data from device.
    /// @param[in]   id              The user PDO id to inquired.
    /// @param[out]  actualPDOInfo   Reference to actual user PDO information.
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult getSingleUserPDODataFromDevice(UserPDOID id, UserPDODataInfo& actualPDOInfo) = 0;

    /// @brief This method set the single user PDO data to device.
    /// @param[in]  id            The user PDO id to inquired.
    /// @param[out] newPDOInfo    New user PDO information.
    /// @return     Return value indicating success or error.
    virtual AxisProfileResult setSingleUserPDODataToDevice(UserPDOID id, UserPDODataInfo newPDOInfo) = 0;

    /// @brief This method check whether single user PDO (AT) is active.
    /// @param[in] id The user PDO id to check.
    /// @return   Indicate whether activated.
    virtual bool checkSingleUserPDOActiveFromDevice(UserPDOID id) = 0;

    /// @brief This method check whether single user PDO (MDT) is active.
    /// @param[in] id The user PDO id to check.
    /// @return   Indicate whether activated.
    virtual bool checkSingleUserPDOActiveToDevice(UserPDOID id) = 0;

    /// @brief This method get effective time of actual output in unit of second.
    /// @return Effective time of acutal output
    virtual double getEffectiveTimeOfActOutput() = 0;

    /// @brief This method get latch time of acutal input in unit of second.
    /// @return Latch time of actual input
    virtual double getLatchTimeOfActInput() = 0;
  };
}
