/// @file
///
/// @brief This file is part of the axisprofile library and implements the base job of sercos.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_job_base.h"

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

AxisprofileJobBase::AxisprofileJobBase(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle)
  : m_profileInfo(profileInfo), m_shareHandle(shareHandle)
{
}

void AxisprofileJobBase::onReset()
{
  APR_TRACE_MESSAGE("%s %s is reset", m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str());
  m_asyncSm.resetCommandState();
  m_jobStep = 0;
  JobObj::onReset();
}

void AxisprofileJobBase::onJobStateChanged(JobState state, JobState oldState)
{
  APR_TRACE_MESSAGE("%s %s enter state %s from %s",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    jobStateToStr(state), jobStateToStr(oldState));
}

AxisProfileResult AxisprofileJobBase::initDelayCycles(double time)
{
  double cnt = time / m_shareHandle->getScalingIF()->getCycleTime();
  m_waitCycleCnt = static_cast<std::uint32_t>(std::round(cnt));
  APR_TRACE_MESSAGE("%s wait %d cycles until position loop become stable",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    m_waitCycleCnt);

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisprofileJobBase::exeDelayCycles()
{
  if (m_waitCycleCnt > 0)
  {
    --m_waitCycleCnt;
    return AxisProfileResult::STS_PENDING;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisprofileJobBase::runJobsInternal()
{
  // there is noting to do
  if (m_jobFuns.size() == 0)
  {
    return AxisProfileResult::STS_OK;
  }

  auto res = m_jobFuns[m_jobStep]();
  if (aprStatusFailed(res))
  {
    // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
    APR_TRACE_ERROR("%s %s failed, ErrCode=0x%X.", m_profileInfo.m_profileName.c_str(),
                    getID().c_str(), static_cast<std::uint16_t>(res));
    return res;
    // LCOV_EXCL_STOP - exclude code coverage section end
  }
  else if (res == AxisProfileResult::STS_OK)
  {
    if (++m_jobStep >= m_jobFuns.size())
    {
      return AxisProfileResult::STS_OK;
    }
  }
  return AxisProfileResult::STS_PENDING;
}

const IContainer* AxisprofileJobBase::getJobData()
{
  return &m_deviceStatus;
}

IContainer* AxisprofileJobBase::getJobDataRef()
{
  return &m_deviceStatus;
}

AxisProfileResult AxisprofileJobBase::onJobInActived()
{
  return onUpdateStatusInDevice();
}

AxisProfileResult AxisprofileJobBase::onUpdateStatusInDevice()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisprofileJobBase::getSignalFuncHandler(SignalFuncHandler*& handler)
{
  if (!m_shareHandle->getDeviceParamIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto deviceParam = m_shareHandle->getDeviceParamIF();
  handler = deviceParam->getSignalFuncHandler();
  return AxisProfileResult::STS_OK;
}
