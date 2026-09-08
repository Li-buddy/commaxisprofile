/// @file
///
/// @brief This file is part of the axisprofile library and implements the base class of
/// diagnosis for axisprofile object 
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/log/diagnosis/i_registration4.h"
#include "diagnosis/dia.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------
std::shared_ptr<common::log::diagnosis::ILog4> comm::axisprofile::Dia::s_diagnosisLog{};

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

Dia::~Dia()
{
  s_diagnosisLog = nullptr;
}

void Dia::setDiagnosisHandler(common::log::diagnosis::IRegistration4* registration, const std::string& bundleRootPath)
{
  m_registrationFilePath = bundleRootPath + "/resources/diagnostics/" + LIBRARY_DIAGNOSTICS_FILE_NAME;
  auto diagResult = registration->registerDefaultDiagnoses(m_registrationFilePath, s_diagnosisLog);
  if (::common::log::diagnosis::statusFailed(diagResult))
  {
    // LCOV_EXCL_START
    APR_TRACE_ERROR("Calling 'registerDefaultDiagnoses' failed with error code 0x%08X", diagResult);
    // LCOV_EXCL_STOP
  }
}

void Dia::removeDiagnosisHandler(common::log::diagnosis::IRegistration4* registration)
{
  auto diagResult = registration->unregisterDefaultDiagnoses(m_registrationFilePath);
  if (::common::log::diagnosis::statusFailed(diagResult))
  {
    // LCOV_EXCL_START
    APR_TRACE_ERROR("Calling 'registerDefaultDiagnoses' failed with error code 0x%08X", diagResult);
    // LCOV_EXCL_STOP
  }

  s_diagnosisLog = nullptr;
}

std::shared_ptr<common::log::diagnosis::ILog4> Dia::getDiagnosisLog()
{
  return s_diagnosisLog;
}
