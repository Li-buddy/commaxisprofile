/// @file
///
/// @brief This source file is part of the axisprofile library and implements
/// base class for status and controlword handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_status_control_encoder.h"
#include "diagnosis/dia.h"

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

AxisProfileStatusControlEncoder::AxisProfileStatusControlEncoder(ProfileBasicInfo& profileBasicInfo)
  : AxisProfileStatusControl(profileBasicInfo)
{

}

AxisProfilePowerMode AxisProfileStatusControlEncoder::getPowerMode() const
{
  // not supported
  return m_actualPowerMode;
}

bool AxisProfileStatusControlEncoder::checkOperationMode(const AxisProfileOperationMode /*opMode*/) const
{
  // not supported
  rejectCmdDiagnosis("checkOperationMode");
  return false;
}

bool AxisProfileStatusControlEncoder::checkPowerMode(const AxisProfilePowerMode /*powerMode*/) const
{
  // not supported
  rejectCmdDiagnosis("checkPowerMode");
  return false;
}

AxisProfileResult AxisProfileStatusControlEncoder::analyzeStatusWord()
{
  return AxisProfileResult::STS_OK;
}

std::uint16_t AxisProfileStatusControlEncoder::getStatusWord() const
{
  rejectCmdDiagnosis("getStatusWord");
  return 0;
}

bool AxisProfileStatusControlEncoder::checkError() const
{
  rejectCmdDiagnosis("checkError");
  return false;
}

bool AxisProfileStatusControlEncoder::cyclicStandStillCheck() const
{
  rejectCmdDiagnosis("cyclicStandStillCheck");
  return false;
}

bool AxisProfileStatusControlEncoder::checkOperationModeSupported(const AxisProfileOperationMode /*opMode*/, OpModeErrorInfo& /*errorInfo*/)
{
  rejectCmdDiagnosis("checkOperationModeSupported");
  return false;
}

AxisProfileOperationMode AxisProfileStatusControlEncoder::getOperationMode() const
{
  rejectCmdDiagnosis("getOperationMode");
  return AxisProfileOperationMode::UNDEFINED;
}

bool AxisProfileStatusControlEncoder::isDeviceFollowsControl() const
{
  rejectCmdDiagnosis("isDeviceFollowsControl");
  return false;
}

bool AxisProfileStatusControlEncoder::deviceHasWarning() const
{
  rejectCmdDiagnosis("deviceHasWarning");
  return false;
}

ReadyToOperate AxisProfileStatusControlEncoder::checkReadyToOperate()
{
  return ReadyToOperate::UNDEF_OPERATION;
}

// -------------------------------------- Controlword ---------------------------------------
std::uint16_t AxisProfileStatusControlEncoder::getControlWord() const
{
  rejectCmdDiagnosis("getControlWord");
  return 0;
}

AxisProfileResult AxisProfileStatusControlEncoder::setControlWord(std::uint16_t /*controlWord*/)
{
  rejectCmdDiagnosis("setControlWord");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileStatusControlEncoder::setOperationMode(const AxisProfileOperationMode /*opMode*/)
{
  rejectCmdDiagnosis("setOperationMode");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileStatusControlEncoder::setPowerMode(const AxisProfilePowerMode /*powerMode*/)
{
  rejectCmdDiagnosis("setPowerMode");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

// -------------------------------------- Drive Halt ---------------------------------------
AxisProfileResult AxisProfileStatusControlEncoder::setDriveHaltMode(const bool /*haltEnable*/)
{
  rejectCmdDiagnosis("setDriveHaltMode");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileStatusControlEncoder::getDriveHaltMode(bool& /*isHaltEnabled*/) const
{
  rejectCmdDiagnosis("getDriveHaltMode");
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

void AxisProfileStatusControlEncoder::loadBootingTasks()
{
  m_initStatusControl.clear();
}

void AxisProfileStatusControlEncoder::rejectCmdDiagnosis(const char* cmdNameInfo) const
{
  auto category = m_sharingHandler->getCategoryIF()->getProfileCategory();
  Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_FUNCTION_NOT_SUPPORTED, __FILE__, __FUNCTION__, __LINE__,
                     "Command %s not allowed for %s on profile %s.", cmdNameInfo, axisProfileCategoryTypeToString(category),
                     m_profileBasicInfo.m_profileName.c_str());
}
