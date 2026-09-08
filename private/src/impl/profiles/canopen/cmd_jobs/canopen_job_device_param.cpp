/// @file
///
/// @brief This file is part of the axisprofile library and implements the job of device parameter read/write.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_device_param.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

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

CanopenJobDeviceParam::CanopenJobDeviceParam(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle)
  : AxisprofileJobBase(profileInfo, shareHandle)
{
  m_paramAccessIf = static_cast<CanopenParamAccess*>(m_shareHandle->getParamAccessIF());
  m_data.getRef().m_ptrData = m_buf.data();
}

const IContainer* CanopenJobDeviceParam::getJobData()
{
  return &m_data;
}

IContainer* CanopenJobDeviceParam::getJobDataRef()
{
  return &m_data;
}

AxisProfileResult CanopenJobDeviceParam::onJobEnterActivating()
{
  APR_TRACE_MESSAGE("%s Job '%s': %s, IDN=%s, started",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    m_data.get().m_readWriteType == ReadWriteType::READ ? "READ" : "WRITE",
                    objDirToString(m_data.get().m_objDirEntry).c_str());

  m_smStatus.resetRequestState();
  return AxisprofileJobBase::onJobEnterActivating();
}

AxisProfileResult CanopenJobDeviceParam::onJobActivating()
{
  AxisProfileResult res;
  if (m_data.get().m_readWriteType == ReadWriteType::READ)
  {
    res = m_paramAccessIf->readAndCheckParamAsync(m_data.get().m_objDirEntry.index, m_data.get().m_objDirEntry.subIndex,
                                                  m_buf.data(), m_data.get().dataSize, m_smStatus);
  }
  else
  {
    res = m_paramAccessIf->writeAndCheckParamAsync(m_data.get().m_objDirEntry.index, m_data.get().m_objDirEntry.subIndex,
                                                   m_buf.data(), m_data.get().dataSize, m_smStatus);
  }

  if (res == AxisProfileResult::STS_OK)
  {
    APR_TRACE_MESSAGE("%s Job '%s': %s, IDN=%s, completed",
                      m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                      m_data.get().m_readWriteType == ReadWriteType::READ ? "READ" : "WRITE",
                      objDirToString(m_data.get().m_objDirEntry).c_str());
  }
  return res;
}

AxisProfileResult CanopenJobDeviceParam::onJobEnterAborting()
{
  m_smStatus.resetRequestState();
  APR_TRACE_MESSAGE("%s Job '%s': %s, IDN=%s, cleared",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    m_data.get().m_readWriteType == ReadWriteType::READ ? "READ" : "WRITE",
                    objDirToString(m_data.get().m_objDirEntry).c_str());
  return AxisprofileJobBase::onJobEnterAborting();
}
