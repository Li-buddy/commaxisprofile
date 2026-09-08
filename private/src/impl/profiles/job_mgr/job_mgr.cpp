/// @file
///
/// @brief This file is part of the axisprofile library and implements the job manager of axisprofile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "job_mgr.h"
#include "job_object/job_obj.h"

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
/// @brief Check and return if job not exist
/// @param[in]  id        Job Id
/// @param[in]  retValue  Return value
#define RETURN_JOB_NOT_EXIST(id, retValue) \
  if (m_mapJobObj.find(id) == m_mapJobObj.end()) \
  { \
    APR_TRACE_ERROR("%s; %s not exist", \
                    m_profileInfo.m_profileTrace.createTraceMsg(), id.c_str()); \
    return retValue; \
  }

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

void JobMgr::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_shareHandle = handler;
}

AxisProfileResult JobMgr::registerJob(std::unique_ptr<IJobObj>& obj)
{
  if (obj == nullptr)
  {
    APR_TRACE_ERROR("%s; job object is nullptr",
                    m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // initial with nullptr and can be overwritten
  const auto& id = obj->getID();
  if (id.empty())
  {
    APR_TRACE_ERROR("%s; job object is empty",
                    m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_JOB_EMPTY_NAME;
  }

  if (m_mapJobObj.find(id) != m_mapJobObj.end())
  {
    APR_TRACE_ERROR("%s; %s already exit",
                    m_profileInfo.m_profileTrace.createTraceMsg(), id.c_str());
    return AxisProfileResult::ERR_JOB_EXIST;
  }

  m_mapJobObj[id].swap(obj);
  APR_TRACE_MESSAGE("%s; %s registered",
                    m_profileInfo.m_profileTrace.createTraceMsg(), id.c_str());
  return AxisProfileResult::STS_OK;
}

void JobMgr::unregisterAllJobs()
{
  APR_TRACE_MESSAGE("%s; unregister all jobs",
                    m_profileInfo.m_profileTrace.createTraceMsg());
  m_mapJobObj.clear();
}

IJobObj* JobMgr::getJobObj(JobIDType id)
{
  RETURN_JOB_NOT_EXIST(id, nullptr);
  return m_mapJobObj[id].get();
}

AxisProfileResult JobMgr::sendJobCmd(JobIDType id, CmdReq newReq, IContainer* arg)
{
  RETURN_JOB_NOT_EXIST(id, AxisProfileResult::ERR_JOB_NOT_EXIST);

  std::string infoTxt;
  auto result = m_mapJobObj[id]->setNewState(newReq, infoTxt, arg);
  if (aprStatusFailed(result))
  {
    APR_TRACE_ERROR("%s; %s failed; %s",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    id.c_str(), infoTxt.c_str());
    return result;
  }

  return AxisProfileResult::STS_OK;
}

CmdState JobMgr::getJobState(JobIDType id)
{
  RETURN_JOB_NOT_EXIST(id, CmdState::STATE_UNKNOWN);
  return m_mapJobObj[id]->getState();
}

void JobMgr::runAllJobs()
{
  for (const auto& it : m_mapJobObj)
  {
    if (it.second)
    {
      auto result = it.second->mainCycle();
      if (aprStatusFailed(result))
      {
        // error only returns when switching to Error State from other state, so it one shot
        APR_TRACE_ERROR("%s; error in job of %s, ErrCode 0x%X",
                        m_profileInfo.m_profileTrace.createTraceMsg(),
                        it.first.c_str(), result);
      }
    }
  }
}
