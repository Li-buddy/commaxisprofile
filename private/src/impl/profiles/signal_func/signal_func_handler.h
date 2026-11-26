#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and declare the signal function handler class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "profiles/profile_defs.h"

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
  // Forward function references
  // ---------------------------------------------------------------------------
  class IAxisProfileSharingHandler;
  class IAxisProfileCyclicData;

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This class implements the signal function handler.
  class SignalFuncHandler
  {
  public:
    explicit SignalFuncHandler(ProfileBasicInfo& profileBasicInfo);

    virtual ~SignalFuncHandler() noexcept = default;

    /// @brief Init the requested inject pointer and other preparation for signal function handler.
    /// @param[in] sharing The pointer to the sharing handler.
    /// @return    Return value indicating success or error.
    AxisProfileResult init(IAxisProfileSharingHandler* sharing);

    /// @brief Is the function supported by signal status word and signal control word.
    /// @param[in,out] func the function to be checked, and the function details will be filled.
    /// @param[out] errorInfo the dynamic error information when function is not supported, used for diagnosis of the caller.
    /// @return result indicating if the function is supported or not.
    bool isSignalFuncSupported(SignalFunction& func, FunctionSupportErrorInfo& errorInfo);

    /// @brief Is the function defined in signal status word.
    /// @param[in,out]  func       The function to be checked, and the function details will be filled.
    /// @param[out]     errorInfo  Dynamic error information when function is not supported, used for diagnosis of the caller.
    /// @return  TRUE indicates that the function is supported, FALSE indicates not support.
    bool isFuncInSignalStatusWord(SignalFunction& func, FunctionSupportErrorInfo& errorInfo);

    /// @brief Is the function defined in signal control word.
    /// @param[in,out]  func       The function to be checked, and the function details will be filled.
    /// @param[out]     errorInfo  Dynamic error information when function is not supported, used for diagnosis of the caller.
    /// @return  TRUE indicates that the function is supported, FALSE indicates not support.
    bool isFuncInSignalControlWord(SignalFunction& func, FunctionSupportErrorInfo& errorInfo);

    /// @brief This method resets the signal function bit in the signal control word to ensure a rising edge for the next step.
    /// @param[in] func the signal function to be reset, which will be used to determine the corresponding bit in the signal control word.
    /// @return returns AxisProfileResult indicating the result of the reset operation, success or error.
    AxisProfileResult resetSignalFuncBit(SignalFunction& func);

    /// @brief This method activates the signal function by setting the corresponding bit in the signal control word.
    /// @param[in] func the signal function to be activated, which will be used to determine the corresponding bit in the signal control word.
    /// @return returns AxisProfileResult indicating the result of the activation operation, success or error.
    AxisProfileResult activeSignalFunc(SignalFunction& func);

    /// @brief This method gets the current state of the signal function from the signal status word.
    /// @param[in] func the signal function to be checked, which will be used to determine the corresponding bit in the signal status word.
    /// @param[out] val the current state of the signal function, true if the function is active, false otherwise.
    /// @return returns AxisProfileResult indicating the result of the state retrieval operation, success or error.
    AxisProfileResult getSignalFuncState(SignalFunction& func, bool& val);
        
  protected:

    /// @brief Read the configuration of signal status word from drive.
    /// @param[out]    params        The configured parameters of signal status word, same size with assignBits.
    /// @param[out]    assignBits    The assign bit list of the signal status word, same size with params.
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult readDriveSignalStatusWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits) = 0;

    /// @brief read the configuration of signal control word from drive.
    /// @param[out]    params        The configured parameters of signal control word, same size with assignBits.
    /// @param[out]    assignBits    The assign bit list of the signal control word, same size with params.
    /// @return      Return value indicating success or error.
    virtual AxisProfileResult readDriveSignalControlWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits) = 0;

    /// @brief Register the signal word functions for the drive.
    virtual void registDriveSignalWordFuncs() = 0;

    /// @brief Register the signal word functions for the encoder.
    virtual void registEncoderSignalWordFuncs() = 0;

    /// @brief Prepare the required signal word information based on the profile category.
    ///        this design is to avoid increase new class for each profile category,
    ///        and the signal word information is prepared at booting.
    /// @param[in] category The profile category, which is used to determine the required signal word information.
    void prepareSignalWordInfo(const AxisProfileCategory category);

    /// @brief Check and validate the signal status word configuration.
    /// @return      Return value indicating success or error.
    AxisProfileResult validateSignalStatusWordCfg();

    /// @brief Check and validate the signal control word configuration.
    /// @return      Return value indicating success or error.
    AxisProfileResult validateSignalControlWordCfg();

    /// @brief Register the predefined functions supported by signal status word.
    /// @param[in] function  The predefined function.
    /// @param[in] validator The validator, which will be called at booting.
    void registerSignalStatusWordFuncs(const FunctionSupport& function, const SignalWordValidator& validator);

    /// @brief Register the predefined functions supported by signal control word.
    /// @param[in] function The predefined function.
    /// @param[in] validator The validator, which will be called at booting.
    void registerSignalControlWordFuncs(const FunctionSupport& function, const SignalWordValidator& validator);

  protected:
    ProfileBasicInfo&           m_profileBasicInfo;                             ///< The profile basic information.
    IAxisProfileSharingHandler* m_sharingHandler {};                            ///< mediator for sharing data
    IAxisProfileCyclicData*     m_cyclicData {};                                ///< the pointer to the cyclic data, for signal function state check
    std::vector<SignalFunction> m_signalStatusWordFuncs {};                     ///< The predefined functions supported by signal status word.
    std::vector<SignalFunction> m_signalControlWordFuncs {};                    ///< The predefined functions supported by signal control word.
  };
}
