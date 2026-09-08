/// @file
///
/// @brief This file is part of the axisprofile library and implements the job of fixed stop command.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "diagnosis/dia.h"
#include "profiles/i_axisprofile_device_param_internal.h"
#include "profiles/scaling/i_scaling.h"
#include "sercos_job_fixed_stop.h"

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

SercosJobFixedStop::SercosJobFixedStop(ProfileBasicInfo& profileInfo, IAxisProfileSharingHandler* shareHandle)
  : SercosJobBase(ProcedureCommands::S_0_0149_FIXED_STOP_COMMAND, profileInfo, shareHandle)
{}

AxisProfileResult SercosJobFixedStop::onSetArgument(IContainer* arg)
{
  auto fixedStopArg = ArgFixedStop::safeCast(arg);
  if (fixedStopArg == nullptr)
  {
    APR_TRACE_ERROR("%s %s gets wrong type of argument",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str());
    return AxisProfileResult::ERR_JOB_DATA_MISMATCH;
  }

  m_clampingTorque = fixedStopArg->get();
  APR_TRACE_MESSAGE("%s %s with: clampingTorque=%g",
                    m_profileInfo.m_profileTrace.createTraceMsg(),
                    getID().c_str(), m_clampingTorque);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult SercosJobFixedStop::onJobEnterActivating()
{
  m_jobFuns.clear();
  m_jobStep = 0;
  m_jobFuns.emplace_back([&]() {return validateConfig();});
  m_jobFuns.emplace_back([&]() {return m_paramAccess->writeAndCheckParamAsync(S_0_0530_CLAMPING_TORQUE, &m_torqueRawS530, sizeof(m_torqueRawS530), m_asyncSm);});
  m_jobFuns.emplace_back([&]() {return m_paramAccess->setSercosProcedureCommandAsync(m_command, m_asyncSm);});

  return SercosJobBase::onJobEnterActivating();
}

AxisProfileResult SercosJobFixedStop::onJobActivating()
{
  auto res = runJobsInternal();
  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
  }
  return res;
}

AxisProfileResult SercosJobFixedStop::onJobAborting()
{
  auto res = m_paramAccess->clearSercosProcedureCommandAsync(m_command, m_asyncSm);
  if (res == AxisProfileResult::STS_OK)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  }
  return res;
}

////////////////////////////////////////// private methods //////////////////////////////////////////// 

AxisProfileResult SercosJobFixedStop::validateConfig()
{
  if (!m_shareHandle || !m_shareHandle->getDeviceParamIF() || !m_shareHandle->getScalingIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_torqueRawS530 = m_shareHandle->getScalingIF()->convertTorqueScaledToRaw(m_clampingTorque);
  const auto& deviceCfg = m_shareHandle->getDeviceParamIF()->getDeviceCfgData();
  if (m_torqueRawS530 > deviceCfg.m_trqLimitMax)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_TORQUE_LIMIT,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Torque/Force target %d high than max value IDN %s",
                       m_torqueRawS530, idnToString(S_0_0092_BIPOLARE_TORQUE_LIMIT).c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}
