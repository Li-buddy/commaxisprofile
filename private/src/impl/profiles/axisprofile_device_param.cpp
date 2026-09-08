/// @file
///
/// This source file is part of the axisprofile library and implements
/// base class for device parameter handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_device_param.h"
#include "bundle/axisprofile_device_param_c.h"
#include "diagnosis/dia.h"
#include "scaling/i_scaling.h"

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

AxisProfileDeviceParam::AxisProfileDeviceParam(ProfileBasicInfo& profileBasicInfo)
  : m_profileBasicInfo(profileBasicInfo), m_sharingHandler(nullptr)
{
  // *** Initialize deprecated C-Interface *** //
  m_cDeprecatedInterface.handle = this;
  m_cDeprecatedInterface.checkBrakeDelaySupport = AXISPROFILE_checkBrakeDelaySupport;
  m_cDeprecatedInterface.getErrorBrakeDelayInfo = AXISPROFILE_getErrorBrakeDelayInfo;
  m_cDeprecatedInterface.getErrorBrakeDynLimit = AXISPROFILE_getErrorBrakeDynLimit;
  m_cDeprecatedInterface.getPositionDataFormat = AXISPROFILE_getPositionDataFormat;
  m_cDeprecatedInterface.checkAndSetModulo = AXISPROFILE_checkAndSetModulo;
  m_cDeprecatedInterface.checkFunctionSupported = AXISPROFILE_checkFunctionSupported;
  m_cDeprecatedInterface.depricatedGetDCSync0ShiftTime = nullptr;

  // *** Initialize C-Interface *** //
  m_cInterface.handle = this;
  m_cInterface.checkBrakeDelaySupport = AXISPROFILE_checkBrakeDelaySupport;
  m_cInterface.getErrorBrakeDelayInfo = AXISPROFILE_getErrorBrakeDelayInfo;
  m_cInterface.getErrorBrakeDynLimit = AXISPROFILE_getErrorBrakeDynLimit;
  m_cInterface.getPositionDataFormat = AXISPROFILE_getPositionDataFormat;
  m_cInterface.checkAndSetModulo = AXISPROFILE_checkAndSetModulo;
  m_cInterface.checkFunctionSupported = AXISPROFILE_checkFunctionSupported;
  m_cInterface.checkStandstill = AXISPROFILE_checkStandstill;
  m_cInterface.getRateTorqueForce = AXISPROFILE_getRateTorqueForce;
  m_cInterface.checkAxisHomed = AXISPROFILE_checkAxisHomed;
}

const IAxisProfileBootingConfigC* AxisProfileDeviceParam::getDeprecatedCInterface() const
{
  return &m_cDeprecatedInterface;
}

const IAxisProfileDeviceParamC* AxisProfileDeviceParam::getCInterface() const
{
  return &m_cInterface;
}

void AxisProfileDeviceParam::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

AxisProfileResult AxisProfileDeviceParam::getDCSync0ShiftTime(double& sync0)
{
  if (m_dcSupport)
  {
    sync0 = m_dcSync0ShiftTime;
    return AxisProfileResult::STS_OK;
  }
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileDeviceParam::readDcConfigInfo()
{
  DcConfigInfo dcCfg {};
  auto result = m_profileBasicInfo.m_fieldbusAdapter->readSlaveDcConfigInfoSync(m_profileBasicInfo.m_deviceAddress, dcCfg);
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Read distribution clock failed, error:0x%x", result);
    return AxisProfileResult::STS_OK; // only send warning diagnosis, so return STS_OK.
  }

  m_dcSupport = dcCfg.m_dcSupported;
  m_dcSync0ShiftTime = NANOSECOND_2_SECOND(dcCfg.m_shiftTimeNs);
  m_dcSync0CycleTime = NANOSECOND_2_SECOND(dcCfg.m_cycleTimeNs);

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileDeviceParam::getOutputEffectiveDelay(double& delay)
{
  if (m_hasOutputEffectiveDelay)
  {
    delay = m_outputEffectiveDelay;
    return AxisProfileResult::STS_OK;
  }
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult AxisProfileDeviceParam::booting()
{
  getDeviceInformation(); // calculate software version before using checksum parameter
  
  bool isNodeChecksumEnable = m_sharingHandler->getPropertyIF()->isChecksumEnabled();
  m_needReadBootParams = isNodeChecksumEnable ? (checkNeedReadBootParams() || m_firstBootup) : true;
  APR_TRACE_MESSAGE("%s, m_firstBootup: %d, m_needReadBootParams: %d, node checksumEnable:%d",
                    m_profileBasicInfo.m_profileTrace.createTraceMsg(), m_firstBootup, m_needReadBootParams, isNodeChecksumEnable);

  m_firstBootup = false;

  loadBootingTasks();
  AxisProfileResult result {AxisProfileResult::STS_OK};
  for (auto const& method : m_initDeviceParam)
  {
    result = method();
    if (aprStatusNotOkay(result))
    {
      return result;
    }
  }
  return result;
}

AxisProfileResult AxisProfileDeviceParam::getRateTorqueForce(double& rateTorqueForce, std::string& dynErrTxt)
{
  if (!m_sharingHandler)
  {
    dynErrTxt = "invalid sharing handler";
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  // TODO: more check to be added
  const IScaling* scaling = m_sharingHandler->getScalingIF();
  rateTorqueForce = scaling->getRateTorqueForce();
  return AxisProfileResult::STS_OK;
}

bool AxisProfileDeviceParam::isNeedReadBootParams() const
{
  return m_needReadBootParams;
}

std::uint32_t AxisProfileDeviceParam::getCheckSumValue() const
{
  return m_checksum;
}

AxisProfileResult AxisProfileDeviceParam::checkAxisHomed(bool& homed)
{
  homed = m_homed.load();
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileResult AxisProfileDeviceParam::setAxisHomed(bool homed)
{
  m_homed.store(homed);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileDeviceParam::initHomeState()
{
  m_homed.store(false);
  return AxisProfileResult::STS_OK;
}
