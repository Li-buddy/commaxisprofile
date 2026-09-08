/// @file
///
/// @brief This file is part of the axisprofile library and implements the job of device parameter read/write.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "sercos_job_device_param.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

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

SercosJobDeviceParam::SercosJobDeviceParam(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle)
  : AxisprofileJobBase(profileInfo, shareHandle)
{
  m_paramAccess = static_cast<SercosParamAccess*>(m_shareHandle->getParamAccessIF());
  m_data.getRef().m_ptrData = m_buf.data();
}

const IContainer* SercosJobDeviceParam::getJobData()
{
  return &m_data;
}

IContainer* SercosJobDeviceParam::getJobDataRef()
{
  return &m_data;
}

AxisProfileResult SercosJobDeviceParam::onJobEnterActivating()
{
  APR_TRACE_MESSAGE("%s Job '%s': %s, IDN=%s, started",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    m_data.get().m_readWriteType == ReadWriteType::READ ? "READ" : "WRITE",
                    idnToString(m_data.get().m_requestIDN).c_str());

  m_asyncSm.resetCommandState();
  m_asyncSm.switchToCmdStartStep();
  return JobObj::onJobEnterActivating();
}

AxisProfileResult SercosJobDeviceParam::onJobActivating()
{
  AxisProfileResult res;
  if (m_data.get().m_readWriteType == ReadWriteType::READ)
  {
    res = m_paramAccess->readAndCheckParamDataAsync(m_data.get().m_requestIDN, m_buf.data(), m_data.get().dataSize, m_asyncSm.getRequestState());
  }
  else
  {
    res = m_paramAccess->writeAndCheckParamAsync(m_data.get().m_requestIDN, m_buf.data(), m_data.get().dataSize, m_asyncSm.getRequestState());
  }

  if (res == AxisProfileResult::STS_OK)
  {
    APR_TRACE_MESSAGE("%s Job '%s': %s, IDN=%s, completed",
                      m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                      m_data.get().m_readWriteType == ReadWriteType::READ ? "READ" : "WRITE",
                      idnToString(m_data.get().m_requestIDN).c_str());
  }
  return res;
}

AxisProfileResult SercosJobDeviceParam::onJobEnterAborting()
{
  m_asyncSm.resetCommandState();
  m_asyncSm.switchToCmdStartStep();
  APR_TRACE_MESSAGE("%s Job '%s': %s, IDN=%s, cleared",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    m_data.get().m_readWriteType == ReadWriteType::READ ? "READ" : "WRITE",
                    idnToString(m_data.get().m_requestIDN).c_str());
  return JobObj::onJobEnterAborting();
}
