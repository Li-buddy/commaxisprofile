/// @file      aux_job_handler.cpp
///
/// This source file is part of the axisprofile library and implements the
/// auxiliary job
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "aux_job_handler.h"
#include "profiles/profile_defs.h"
#include <functional>

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

AuxJobHandler::~AuxJobHandler()
{
  clearAllJobs();
}

AxisProfileResult AuxJobHandler::addAuxJob(auxJobs job)
{
  m_auxJobs.emplace_back(job);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AuxJobHandler::runAllAuxJobs()
{
  if (m_auxJobs.size() > 0)
  {
    for (const auto& job : m_auxJobs)
    {
      job.m_jobCyclic(); // result handling should inside the job.
    }
  }
  return  AxisProfileResult::STS_OK;
}

void AuxJobHandler::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

void AuxJobHandler::clearAllJobs()
{
  m_auxJobs.clear();
}
