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
#include "helper/device_id_helper.h"

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

AxisProfileDeviceParam::AxisProfileDeviceParam(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  : m_profileBasicInfo(profileBasicInfo), m_telegramAT(at), m_telegramMDT(mdt), m_sharingHandler(nullptr)
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

AxisProfileDeviceParam::~AxisProfileDeviceParam()
{
  delete m_signalFuncHandler; m_signalFuncHandler = nullptr;
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

AxisProfileResult AxisProfileDeviceParam::checkAxisHomed(bool& homed)
{
  homed = m_homed.load();
  return AxisProfileResult::STS_OK;
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

SignalFuncHandler* AxisProfileDeviceParam::getSignalFuncHandler()
{
  return m_signalFuncHandler;
}

AxisProfileResult AxisProfileDeviceParam::prepareSignalFuncHandler()
{
  if (!m_signalFuncHandler || !m_sharingHandler)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  m_signalFuncHandler->init(m_sharingHandler);
  return AxisProfileResult::STS_OK;
}

bool AxisProfileDeviceParam::updateHomedByCyclicData()
{
  SignalFunction func;
  func.function = FunctionSupport::HOMING_STATUS_CYCLIC;
  FunctionSupportErrorInfo errorInfo;
  if (m_signalFuncHandler->isFuncInSignalStatusWord(func, errorInfo))
  {
    // for ctrlX drive, the homed status is read from signal status word, bit 16 of signal status word
    // S-0-0403.0 / 0x2508:01 Status of position feedback value of reference encoder (encoder 1 or 2)
    // 0: Relative 1: Homed
    // for ctrlX encoder, the homed status is read from signal status word, bit 16 of signal status word
    // P-0-0331.0 / 0x24CE:02 Measuring encoder status 
    bool homed = (m_telegramAT->m_signalStatusword & MASK1(16, 1, func.statusBitOffset)) != 0;
    m_homed.store(homed);
    return true;
  }

  if (!m_sharingHandler || !m_sharingHandler->getPropertyIF() || !m_sharingHandler->getMappingHandlerIF())
  {
    return false;
  }

  auto* propertyIf = m_sharingHandler->getPropertyIF();
  auto deviceSeries = propertyIf->getDeviceSeries();
  // 3rd-party with "homed" cyclic data
  auto mapping = m_sharingHandler->getMappingHandlerIF();
  const auto& assigned = mapping->getProfileAssignedInfo();
  // For 3rd-party drives, the homed status is read from the assigned mapping item,
  // the "homed" state is from a user defined bit data
  // which is updated by the homing command.
  if (!DeviceIdHelper::isRexrothDrive(deviceSeries) && assigned.m_input.m_homed)
  {
    m_homed.store(m_telegramAT->m_bitStatusHomed.m_bit);
    return true;
  }
  return false;
}
