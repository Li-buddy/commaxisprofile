#pragma once

// ---------------------------------------------------------------------------
//
//! @file       profile_trace.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and implements a helper
//! class to create special profile related trace messages.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/debug_trace.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------


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

    //! @class ProfileTrace
    //! This is the helper class for profile trace messages.
    class ProfileTrace
    {
    public: // methods

      //! @brief Constructor
      ProfileTrace() = default;

      //! @brief Destructor.
      virtual ~ProfileTrace() noexcept = default;

      //! @brief This method returns the class name.
      //! @return   Return class name.
      static const char* getClassName(void) { return (CLASS_NAME(ProfileTrace)); }

      //! @brief This method sets the constant profile data for all following trace messages.
      //!        Note: Only call this method in system state 'Configuration'!
      //! @param[in]   profileName     Name of the profile
      //! @param[in]   type            Type of the profile
      //! @return Return value indicating success or error.
      virtual AxisProfileResult setConstantProfileData(const std::string& profileName, const AxisProfileType type);

      //! @brief This method sets the device address for all following trace messages.
      //!        Note: Only call this method in system state 'Configuration'!
      //! @param[in]   deviceAddress   Address of the device.
      //! @return Return value indicating success or error.
      virtual AxisProfileResult updateDeviceAddress(const std::uint32_t deviceAddress);

      //! @brief This method uses the given parameters to generate a formated (separated by semicolon) profile trace message dependent on the given error code.
      //!        When calling it without any parameters (just use default values) it will return a string which just contains the basic profile information (name, type, ...).
      //! @param[in]   errCode     Optional. The error code which is used to generate the specific error text by calling 'convertCmdResultToText()'. For success status codes, no text will be appended.
      //! @param[in]   addInfo1    Optional. Additional information which is used in 'convertCmdResultToText()' to generate the error text. See documentation of 'convertCmdResultToText()'.
      //! @param[in]   addInfo2    Optional. Additional information which is used in 'convertCmdResultToText()' to generate the error text. See documentation of 'convertCmdResultToText()'.
      //! @param[in]   addInfo3    Optional. Additional information which is used in 'convertCmdResultToText()' to generate the error text. See documentation of 'convertCmdResultToText()'.
      //! @return Return value is the finished trace message.
      virtual const char* createTraceMsg(const AxisProfileResult errCode = AxisProfileResult::STS_OK, const char* addInfo1 = nullptr, const char* addInfo2 = nullptr, const char* addInfo3 = nullptr);

      //! @brief This method generates a formated (separated by semicolon) trace message, containing the basic profile information and the given text.
      //! @param[in]   messageText    Message text which will be appended to the basic profile information.
      //! @return Return value is the finished trace message.
      virtual const char* createTraceMsg(const char* messageText);

      /// @brief This method get name of the profile
      /// @return Name of the profile
      virtual const char* getProfileName();

    private: // method

      //! @brief Add new information to the trace message buffer.
      //!
      //! This method adds new information to the 'm_traceMsgBuffer'. It appends the current content of the buffer by "; <infoName>:<newInfo>". If the second argument is not passed
      //! it will just add "; <newInfo>" to the message.
      //!
      //! @param[in]   newInfo    New information to be added.
      //! @param[in]   infoName   Identifier of the new information.
      //! @return      Return value indicating success or error.
      bool addNewInfoToTraceMsg(const char* newInfo, const char* infoName = nullptr);

      //! @brief This method converts the given CmdResult to text.
      //! @param[in]    statusCode   The CmdResult to convert
      //! @param[out]   dest         Pointer to a buffer where the resulting C-string is stored.
      //! @param[in]    size         Maximum number of bytes to be used in the buffer.
      //! @param[in]    addInfo1     Optional. Additional information (e.g. state name, variable name, ...). Usage depends on the given error code.
      //! @param[in]    addInfo2     Optional. Additional information (e.g. state name, variable name, ...). Usage depends on the given error code.
      //! @param[in]    addInfo3     Optional. Additional information (e.g. state name, variable name, ...). Usage depends on the given error code.
      //! @return      Return value indicating success or error.
      static std::int32_t convertAprResultToText(AxisProfileResult statusCode, char* dest, size_t size, const char* addInfo1 = nullptr, const char* addInfo2 = nullptr, const char* addInfo3 = nullptr);

    private: // data

      char m_traceMsgBuffer[common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS]{};  //!< Buffer used to create the trace message (will be returned by 'createTraceMsg()')
      char m_profileBasicInfo[128]{};                                                //!< Buffer used to store the profile basic information (will be filled by 'setConstantProfileData()')
      size_t m_profileBasicInfoSize{};                                               //!< Size (string length) of the content stored in m_profileBasicInfo[]
      const char* m_profileName {};                                                  //!< Name of the profile
    };
  }
}
