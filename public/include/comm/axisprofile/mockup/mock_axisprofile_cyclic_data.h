#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axis profile cyclic data interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_cyclic_data_c.h"
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
  class AxisProfileCyclicDataMockHandle
  {
  public: // methods

    /// @brief Constructor
    AxisProfileCyclicDataMockHandle()
    {
      for (std::uint32_t i = static_cast<std::uint32_t>(DigitalInputValueBitType::MIN_VALUE); i < static_cast<std::uint32_t>(DigitalInputValueBitType::MAX_VALUE); i++)
      {
        m_digitalInputValueBit.emplace(static_cast<DigitalInputValueBitType>(i), false);
      }
    };

    /// @brief Destructor
    virtual ~AxisProfileCyclicDataMockHandle() noexcept { m_digitalInputValueBit.clear(); };

    /// @brief Set the return value of class methods.
    /// Note: Do not set 'm_returnValue' directly in your test or CppCheck may generate errors because of the extern C implementation in some cases.
    void setReturnValue(AxisProfileResult value) { m_returnValue = value; }

    /// @brief This method set the digital input value for a single bit of the axisprofile.
    /// @param[in]   valueType    The type of digital input value bit
    /// @param[in]   value        Value of the specific bit
    /// @return      Return value indicating success or error.
    AxisProfileResult setDigitalInputValueBitMock(DigitalInputValueBitType valueType, bool value)
    {
      auto it = m_digitalInputValueBit.find(valueType);
      if (it == m_digitalInputValueBit.end())
      {
        return AxisProfileResult::ERR_INVALID_ARGUMENT;
      }

      it->second = value;
      return AxisProfileResult::STS_OK;
    }

  public: // data

    bool m_copyCmdToActualValues {true};                                 ///< Flag to control whether command values are copied to actual values in the mock
    ActualValuesDevice m_actualValues{};                                 ///< Actual device values
    std::map<DigitalInputValueBitType, bool>  m_digitalInputValueBit{};  ///< Digital input value for a single bit
    AxisProfileResult m_returnValue = AxisProfileResult::STS_OK;         ///< Return value of C-Methods
    bool m_bMockAvailableNRT {true};                                     ///< Mock check result for check actual value
    comm::axisprofile::DataAccessErrorInfo m_mockDataAccessErrorInfo {}; ///< Mock error data for access error state.
    SafetyOperationMode m_curSafetyOperationMode{ SafetyOperationMode::SOM_NORMAL_MODE };  ///< Mock the current safety operation mode.
    UserPDODataInfo m_mockUserPDOData {};                                ///< Mock user PDO data
    bool m_bMockSingleUserPDOActiveFromDevice {true};                    ///< Mock single user pdo active from device
    bool m_bMockSingleUserPDOActiveToDevice {true};                      ///< Mock single user pdo active to device
    double m_mockEffectiveTimeOfActOutput {0.0};                         ///< Mock the effective time of actual output
    double m_mockLatchTimeOfActInput {0.0};                              ///< Mock the latch time of actual input
  };


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // -------------------------------------- C-Interface Method-Mockups ---------------------------------------- //

  /// @brief Mockup of method AXISPROFILE_getActualValuesDevice().
  /// @param[in]    handle         Interface handle (use class AxisProfileCyclicDataMockHandle)
  /// @param[out]   actualValues   Variable where the actual values will be written to
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getActualValuesDeviceMock(void* handle, ActualValuesDevice* actualValues);

  /// @brief Mockup of method AXISPROFILE_setCommandValuesDevice().
  /// @param[in]   handle          Interface handle (use class AxisProfileCyclicDataMockHandle)
  /// @param[in]   commandValues   The command values
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_setCommandValuesDeviceMock(void* handle, CommandValuesDevice commandValues);

  /// @brief Mockup of method AXISPROFILE_getActualValueDigitalIOMock().
  /// @param[in]    handle         Interface handle (use class AxisProfileCyclicDataMockHandle)
  /// @param[in]    valueType      The digital input value type for a single bit
  /// @param[out]   inputValue     Variable where the digital input value will be written to
  /// @return  Member variable 'm_returnValue' of the handle is returned.
  extern "C" AxisProfileResult AXISPROFILE_getDigitalInputValueBitMock(void* handle, DigitalInputValueBitType valueType, bool& inputValue);

  /// @brief Mockup of method AXISPROFILE_checkActualValueAvailabilityNRTMock.
  /// @param[in]  handle      Handle of the interface
  /// @param[in]  valueType   Enum to define the target value type of user.
  /// @param[out] errorInfo   Once the data unavailable, return the error information.
  /// @return     Member variable 'm_mockAvailableNRT' of the handle is returned.
  extern "C" bool AXISPROFILE_checkActualValueAvailabilityNRTMock(void* handle, AxisProfileValueTypeID valueType, DataAccessErrorInfo & errorInfo);

  /// @brief Mockup to get the current safety operation mode of drive.
  /// @param[in]  handle  handle of the interface.
  /// @param[out]   mode  the current mode in the member variable 'm_curSafetyOperationMode' will be output.
  /// @return             return value indicates success or error.
  extern "C" AxisProfileResult AXISPROFILE_getSafetyOperationModeMock(void* handle, SafetyOperationMode& mode);

  /// @brief Mockup of method gets the single user PDO data from device.
  /// @param[in]  handle          Handle of the interface
  /// @param[in]  id              The user PDO id to inquired.
  /// @param[out] actualPDOInfo   Reference to actual user PDO information.
  /// @return     Return value indicating success or error.
  extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_getSingleUserPDODataFromDeviceMock(void* handle, comm::axisprofile::UserPDOID id, comm::axisprofile::UserPDODataInfo & actualPDOInfo);

  /// @brief Mockup of method set the single user PDO data to device.
  /// @param[in]  handle        Handle of the interface
  /// @param[in]  id            The user PDO id to inquired.
  /// @param[out] newPDOInfo    New user PDO information.
  /// @return     Return value indicating success or error.
  extern "C" comm::axisprofile::AxisProfileResult AXISPROFILE_setSingleUserPDODataToDeviceMock(void* handle, comm::axisprofile::UserPDOID id, comm::axisprofile::UserPDODataInfo newPDOInfo);

  /// @brief Mockup of method check whether single user PDO (AT) is active.
  /// @param[in]  handle   Handle of the interface
  /// @param[in]  id       The user PDO id to check.
  /// @return     Indicate whether activated.
  extern "C" bool AXISPROFILE_checkSingleUserPDOActiveFromDeviceMock(void* handle, comm::axisprofile::UserPDOID id);

  /// @brief Mockup of method check whether single user PDO (MDT) is active.
  /// @param[in]   handle   Handle of the interface
  /// @param[in]   id       The user PDO id to check.
  /// @return      Indicate whether activated.
  extern "C" bool AXISPROFILE_checkSingleUserPDOActiveToDeviceMock(void* handle, comm::axisprofile::UserPDOID id);

  /// @brief Mockup of method get the effective time of actual output.
  /// @param[in]   handle    Handle of the interface
  /// @return      Effective time of act output.
  extern "C" double AXISPROFILE_getEffectiveTimeOfActOutputMock(void* handle);

  /// @brief Mockup of method get the latch time of actual input.
  /// @param[in]   handle    Handle of the interface
  /// @return      Latch time of act input.
  extern "C" double AXISPROFILE_getLatchTimeOfActInputMock(void* handle);
}
