// ---------------------------------------------------------------------------
//
//! @file       cmd_trace.cpp
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
//! This source file is part of the axisprofile library and implements a helper
//! class to create special profile related trace messages.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profile_trace.h"
#include "common/string_helper.h"


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;


// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

AxisProfileResult ProfileTrace::setConstantProfileData(const std::string& profileName, const AxisProfileType type)
{
  std::memset(m_profileBasicInfo, 0, sizeof(m_profileBasicInfo));
  snprintf(m_profileBasicInfo, sizeof(m_profileBasicInfo), "Name: %s; Type: %s; Addr: %d", profileName.c_str(), axisProfileTypeToAbbreviationString(type), 0);
  m_profileBasicInfoSize = std::strlen(m_profileBasicInfo);
  m_profileName = profileName.c_str();
  return AxisProfileResult::STS_OK;
}

AxisProfileResult ProfileTrace::updateDeviceAddress(const std::uint32_t deviceAddress)
{
  std::string strBasicInfo(m_profileBasicInfo);
  size_t pos = strBasicInfo.rfind("Addr: ");
  if (pos == std::string::npos)
  {
    return AxisProfileResult::ERR_NOK;
  }
  strBasicInfo.replace(pos, (strBasicInfo.length() - pos), ("Addr: " + std::to_string(deviceAddress)));
  std::memset(m_profileBasicInfo, 0, sizeof(m_profileBasicInfo));
  stringCpyS(m_profileBasicInfo, sizeof(m_profileBasicInfo), strBasicInfo.c_str(), STATE_STRING_COPY_COUNT);

  return AxisProfileResult::STS_OK;
}

const char* ProfileTrace::createTraceMsg(const AxisProfileResult errCode, const char* addInfo1, const char* addInfo2, const char* addInfo3)
{
  // Clear buffer
  std::memset(m_traceMsgBuffer, 0, sizeof(m_traceMsgBuffer));

  // Add basic command information to trace message
  stringCatS(m_traceMsgBuffer, sizeof(m_traceMsgBuffer), m_profileBasicInfo, STATE_STRING_COPY_COUNT);

  // If value is passed to the function but its an empty string, set it to nullptr to get correct output format
  if (addInfo1 && (addInfo1[0] == '\0'))
  {
    addInfo1 = nullptr;
  }
  if (addInfo2 && (addInfo2[0] == '\0'))
  {
    addInfo2 = nullptr;
  }
  if (addInfo3 && (addInfo3[0] == '\0'))
  {
    addInfo3 = nullptr;
  }

  // If it is an error trace, generate error text
  size_t errTextLength = 0;
  if (aprStatusFailed(errCode))
  {
    char errText[common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS];
    std::memset(errText, 0, sizeof(errText));
    convertAprResultToText(errCode, errText, sizeof(errText), addInfo1, addInfo2, addInfo3);
    addNewInfoToTraceMsg(errText);
    errTextLength = std::strlen(errText);
  }

  // Check message length
  if ((m_profileBasicInfoSize + errTextLength) > common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS)
  {
    APR_TRACE_WARNING("Following trace message is too long (will be truncated after %d characters)", common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS)
  }

  return m_traceMsgBuffer;
}

const char* ProfileTrace::createTraceMsg(const char* messageText)
{
  // Clear buffer
  std::memset(m_traceMsgBuffer, 0, sizeof(m_traceMsgBuffer));

  // Add basic command information to trace message
  stringCatS(m_traceMsgBuffer, sizeof(m_traceMsgBuffer), m_profileBasicInfo, STATE_STRING_COPY_COUNT);

  // If value is passed to the function but its an empty string, set it to nullptr to get correct output format
  if (messageText && (messageText[0] == '\0'))
  {
    messageText = nullptr;
  }

  // Add message text to trace message
  addNewInfoToTraceMsg(messageText);

  // Check message length
  size_t messageTextSize = messageText ? std::strlen(messageText) : 0;
  if ((m_profileBasicInfoSize + messageTextSize) > common::log::trace::MAXIMUM_STRING_LENGTH_64_CHARS)
  {
    APR_TRACE_WARNING("Following trace message is too long (will be truncated after %d characters)", common::log::trace::MAXIMUM_STRING_LENGTH_64_CHARS)
  }

  return m_traceMsgBuffer;
}

const char* ProfileTrace::getProfileName()
{
  return m_profileName;
};

// -------------------------------------------------------------------------------
// ---------------------------------- PRIVATE ------------------------------------
// -------------------------------------------------------------------------------

bool ProfileTrace::addNewInfoToTraceMsg(const char* newInfo, const char* infoName)
{
  if (!newInfo)
  {
    return false;
  }

  if (strlen(m_traceMsgBuffer))
  {
    // If something was entered in the buffer before, add semicolon
    stringCatS(m_traceMsgBuffer, sizeof(m_traceMsgBuffer), "; ", STATE_STRING_COPY_COUNT);
  }
  if (infoName)
  {
    // Add identifier of the new information before (e.g. "Axs=")
    stringCatS(m_traceMsgBuffer, sizeof(m_traceMsgBuffer), infoName, STATE_STRING_COPY_COUNT);
  }

  // Add new information to trace buffer
  std::int32_t retVal = stringCatS(m_traceMsgBuffer, sizeof(m_traceMsgBuffer), newInfo, STATE_STRING_COPY_COUNT);

  return (retVal == 0) ? true : false;
}


std::int32_t ProfileTrace::convertAprResultToText(AxisProfileResult statusCode, char* dest, size_t size, const char* addInfo1, const char* /*addInfo2*/, const char* /*addInfo3*/)
{
  switch (statusCode)
  {
  case AxisProfileResult::ERR_COMMAND_IN_PROGRESS:                      return snprintf(dest, size, "Another command is already processed");
  case AxisProfileResult::ERR_WRONG_DEVICE_STATE:          return (addInfo1) ? snprintf(dest, size, "Wrong device state. Operation '%s' failed", addInfo1) :   // Optional : Use failed operation
                                                                               snprintf(dest, size, "Wrong device state");                                     
  case AxisProfileResult::ERR_EXEC_NULL:                   return (addInfo1) ? snprintf(dest, size, "Null-Pointer: %s", addInfo1) :                            // Optional: Describe what is null
                                                                               snprintf(dest, size, "Null-Pointer");                                           
  case AxisProfileResult::ERR_INVALID_ARGUMENT:            return (addInfo1) ? snprintf(dest, size, "Invalid argument: %s", addInfo1) :                        // Optional: Provide information of the invalid argument
                                                                               snprintf(dest, size, "Invalid argument");                                       
  case AxisProfileResult::ERR_DATALAYER:                   return (addInfo1) ? snprintf(dest, size, "Datalayer Error: %s", addInfo1) :                         // Optional: Use datalayer error code as string
                                                                               snprintf(dest, size, "Datalayer Error");
  case AxisProfileResult::ERR_DLRT_MEMMAP_VAR_NOT_FOUND:   return (addInfo1) ? snprintf(dest, size, "Datalayer RT MemMap Var '%s' not found", addInfo1) :      // Optional: Use variable name
                                                                               snprintf(dest, size, "Datalayer RT MemMap Var not found");
  case AxisProfileResult::ERR_FIELDBUS:                    return (addInfo1) ? snprintf(dest, size, "Fieldbus Error: %s", addInfo1) :                          // Optional: Use additional error text
                                                                               snprintf(dest, size, "Fieldbus Error");
  case AxisProfileResult::ERR_FIELDBUS_READ_VALUE:         return (addInfo1) ? snprintf(dest, size, "Could not read fieldbus value %s", addInfo1) :            // Optional: Use name of fieldbus value
                                                                               snprintf(dest, size, "Read fieldbus value failed");
  case AxisProfileResult::ERR_DEFAULT_ERROR_REACTION:                   return snprintf(dest, size, "Error occurred in the switch statemachine");
  
  case AxisProfileResult::ERR_COMMAND_INFO_NOT_FOUND:      return (addInfo1) ? snprintf(dest, size, "Command information not found on %s", addInfo1):
                                                                               snprintf(dest, size, "Command information not found");
  
  case AxisProfileResult::ERR_COMMAND_LOCKED:              return (addInfo1) ? snprintf(dest, size, "Command %s is locked", addInfo1) :
                                                                               snprintf(dest, size, "Command locked");
  case AxisProfileResult::ERR_NOK:                         
  default:                                                              return snprintf(dest, size, "Unspecified error");
  }
}
