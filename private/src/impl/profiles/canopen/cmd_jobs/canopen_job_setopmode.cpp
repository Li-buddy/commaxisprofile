/// @file
///
/// @brief This file is part of the axisprofile library and implements the CanOpen set operation mode job.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_setopmode.h"
#include "profiles/profile_defs.h"
#include "profiles/axisprofile_cmd.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

CanopenJobSetOpMode::CanopenJobSetOpMode(ProfileBasicInfo& profileInfo,
                                         IAxisProfileSharingHandler* shareHandle)
  : CanopenJobBase(profileInfo, shareHandle)
{
}

AxisProfileResult CanopenJobSetOpMode::onSetArgument(IContainer* arg)
{
  auto setOpModeArg = ::comm::axisprofile::ArgSetOperationMode::safeCast(arg);
  if (!setOpModeArg)
  {
    APR_TRACE_ERROR("%s %s gets wrong type of argument",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_JOB_DATA_MISMATCH;
  }

  m_requestedOpMode = setOpModeArg->get();
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobSetOpMode::onJobEnterActivating()
{
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  return CanopenJobBase::onJobEnterActivating();
}

AxisProfileResult CanopenJobSetOpMode::onJobActivating()
{
  AxisProfileResult result = m_statusControlIf->setOperationMode(m_requestedOpMode);
  if (result == AxisProfileResult::STS_PENDING)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
    return result;
  }

  if (result == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
    return result;
  }

  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_ERROR);
  return result;
}

AxisProfileResult CanopenJobSetOpMode::onJobEnterAborting()
{
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_INTERRUPTED);
  return CanopenJobBase::onJobEnterAborting();
}