/// @file
///
/// @brief This file is part of the axisprofile library and implements the CanOpen homing job.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_homing.h"
#include "diagnosis/dia.h"
#include "helper/device_id_helper.h"
#include "profiles/scaling/i_scaling.h"

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

CanopenJobHoming::CanopenJobHoming(ProfileBasicInfo& profileInfo,
                                   IAxisProfileSharingHandler* shareHandle)
  : CanopenJobBase(profileInfo, shareHandle)
{
}

AxisProfileResult CanopenJobHoming::onSetArgument(IContainer* arg)
{
  auto homingArg = ArgHoming::safeCast(arg);
  if (homingArg == nullptr)
  {
    APR_TRACE_ERROR("%s %s gets wrong type of argument",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_JOB_DATA_MISMATCH;
  }

  // load inputs
  m_cmdData = homingArg->getRef();
  APR_TRACE_MESSAGE("%s %s with: cmdPos=%g, encoderId=%d, refMode=%s, withoutMove=%s",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    m_cmdData.newPos, static_cast<std::int16_t>(m_cmdData.encoder),
                    axisprofileRefModeToString(m_cmdData.mode),
                    m_cmdData.withoutMove ? "true" : "false");

  if (m_cmdData.mode != RefMode::SET_POS_DEVICE)
  {
    // check only when homing by device
    return AxisProfileResult::STS_OK;
  }

  // scaling and validation
  auto scaling = m_shareHandle->getScalingIF();
  auto deviceParamIf = m_shareHandle->getDeviceParamIF();

  m_cmdPosRaw = scaling->convertPositionScaledToRaw(m_cmdData.newPos);
  m_posOffsetRaw = 0;

  // new reference position should in range of travel limit of device
  const auto& deviceCfgData = deviceParamIf->getDeviceCfgData();
  if (deviceCfgData.m_softwareLimitActive && // check only when limitation is activated in device
      (m_cmdPosRaw > deviceCfgData.m_positivePosLimit || m_cmdPosRaw < deviceCfgData.m_negativePosLimit))
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_OUT_OF_TRAVEL_LIMIT,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "%g out range of [0x607D:1, 0x607D:2] of device", m_cmdData.newPos);
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  if (m_cmdData.encoder != AxisProfileEncoder::ENCODER_1)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_ENCODER_NOT_EXIST,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Encoder id = %d", static_cast<std::int16_t>(m_cmdData.encoder));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobHoming::onJobEnterActivating()
{
  // decide command execution way
  SignalFuncHandler* signalFuncHandler {};
  getSignalFuncHandler(signalFuncHandler);

  m_signalFuncCmd.initSignalFuncHandler(signalFuncHandler);
  FunctionSupportErrorInfo errorInfo {};
  m_isSignalFuncCmd = m_signalFuncCmd.isSignalFuncSupported(errorInfo);
  APR_TRACE_MESSAGE(m_isSignalFuncCmd ? "homing command : handled as signal function." : "homing command : handled as normal command.");

  m_jobFuns.clear();
  m_jobStep = 0; // jobs that need to switch homing to active
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);

  switch (m_cmdData.mode)
  {
    case RefMode::SET_POS_DEVICE:
      setupHomingByDeviceJobs();
      break;
    case RefMode::SET_POS_PROFILE:
      m_jobFuns.emplace_back([&]() {return activeHomingByProfile();});
      m_jobFuns.emplace_back([&]() {return setHomingState(true);});
      break;
    default:
      break;
  }
  
  return CanopenJobBase::onJobEnterActivating();
}

AxisProfileResult CanopenJobHoming::onJobActivating()
{
  return runJobsInternal();
}

AxisProfileResult CanopenJobHoming::onJobEnterAborting()
{
  m_jobFuns.clear();
  m_jobStep = 0; // jobs that need to switch homing to idle
  switch (m_cmdData.mode)
  {
    case RefMode::SET_POS_DEVICE:
      if(!m_isSignalFuncCmd)
      {
        m_jobFuns.emplace_back([&]() {return abortHomingProcedure();});
        m_jobFuns.emplace_back([&]() {return initDelayCycles(0.05);}); // delay 0.05 second for synchronization
        m_jobFuns.emplace_back([&]() {return exeDelayCycles();});
        m_jobFuns.emplace_back([&]() {return switchAndCheckOpMode(AxisProfileOperationMode::POSITIONING);});
      }
      else
      {
        m_jobFuns.emplace_back([&]() {return m_signalFuncCmd.onJobAborting();});
        m_jobFuns.emplace_back([&]() { return setCmdStatusInDevice(CmdStatusInDevice::DEVICE_NOT_SET); });
      }
      break;
    default:
      break;
  }
  return CanopenJobBase::onJobEnterAborting();
}

AxisProfileResult CanopenJobHoming::onJobAborting()
{
  return runJobsInternal();
}

AxisProfileResult CanopenJobHoming::onUpdateStatusInDevice()
{
  return checkHomingProcedure();
}

////////////////////////////////////////// private methods ////////////////////////////////////////////

AxisProfileResult CanopenJobHoming::setupReadListFromDevice()
{
  m_paramList.clear();
  auto propertyIf = m_shareHandle->getPropertyIF();
  if (DeviceIdHelper::isRexrothCtrlXDrive(propertyIf->getDeviceSeries()))
  {
    m_paramList.emplace_back(ReadWriteType::READ, getObjDir(SET_ABSOLUTE_POSITION_CONTROL, m_profileInfo.m_driveNumber),
                             &m_ctrlWordS448.m_word, sizeof(m_ctrlWordS448.m_word));
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobHoming::setupWriteListToDevice()
{
  auto propertyIf = m_shareHandle->getPropertyIF();

  // validation is okay, parameter can be written
  m_paramList.clear();
  if (propertyIf->getDeviceSeries() == DeviceSeries::INDRADRIVE)
  {
    m_paramList.emplace_back(ReadWriteType::WRITE, getObjDir(REF_DISTANCE_INDRA_DRIVE, m_profileInfo.m_driveNumber),
                             &m_cmdPosRaw, sizeof(m_cmdPosRaw));

    // It is a little bit confusion here, because for Indra drive, the REF_DISTANCE is used for set offset to 0,
    // but for other drive the REF_DISTANCE is used for set the homing position.
    // The reason is, the REF_DISTANCE for indra drive is used for set the homing offset, the homing position must use
    // REF_DISTANCE_INDRADRIVE to set
    m_paramList.emplace_back(ReadWriteType::WRITE, getObjDir(REF_DISTANCE, m_profileInfo.m_driveNumber),
                             &m_posOffsetRaw, sizeof(m_posOffsetRaw));
  }
  else
  {
    m_paramList.emplace_back(ReadWriteType::WRITE, getObjDir(REF_DISTANCE, m_profileInfo.m_driveNumber),
                             &m_cmdPosRaw, sizeof(m_cmdPosRaw));
    if(DeviceIdHelper::isRexrothCtrlXDrive(propertyIf->getDeviceSeries()))
    {
      // May cause misunderstand see comments above.
      m_paramList.emplace_back(ReadWriteType::WRITE, getObjDir(HOMING_OFFSET_CTRLX_DRIVE, m_profileInfo.m_driveNumber),
                               &m_posOffsetRaw, sizeof(m_posOffsetRaw));

      // enable homing on absolute drives
      m_ctrlWordS448.m_bits.bit7 = true;
      m_paramList.emplace_back(ReadWriteType::WRITE, getObjDir(SET_ABSOLUTE_POSITION_CONTROL, m_profileInfo.m_driveNumber),
                               &m_ctrlWordS448.m_word, sizeof(m_ctrlWordS448.m_word));
    }
  }

  // set new reference position only without movement
  if (m_cmdData.withoutMove)
  {
    m_paramList.emplace_back(ReadWriteType::WRITE, getObjDir(HOMING_METHOD, m_profileInfo.m_driveNumber),
                             &m_homingAtCurrentPos, sizeof(m_homingAtCurrentPos));
  }

  return AxisProfileResult::STS_OK;
}

void CanopenJobHoming::setupHomingByDeviceJobs()
{
  m_jobFuns.emplace_back([&]() {return setupReadListFromDevice();});
  m_jobFuns.emplace_back([&]() {return exeParamListAccess();}); // read parameters from device
  setupClearReferencePointJobs();
  m_jobFuns.emplace_back([&]() {return setupWriteListToDevice();});
  m_jobFuns.emplace_back([&]() {return exeParamListAccess();}); // write parameters from device
  setupActiveHomingByDeviceJobs();
}

void CanopenJobHoming::setupClearReferencePointJobs()
{
  auto propertyIf = m_shareHandle->getPropertyIF();
  if (DeviceIdHelper::isRexrothCtrlXDrive(propertyIf->getDeviceSeries()))
  {
    // for ctrlX drive, we clear the old reference point in the drive
    m_jobFuns.emplace_back([&]() {return writeToDeviceU16(CANCEL_REF_POINT, 0x00);});
    m_jobFuns.emplace_back([&]() {return writeToDeviceU16(CANCEL_REF_POINT, 0x03);});
    m_jobFuns.emplace_back([&]() {return writeToDeviceU16(CANCEL_REF_POINT, 0x00);});
  }
}

void CanopenJobHoming::setupActiveHomingByDeviceJobs()
{  
  m_jobFuns.emplace_back([&]() {return setHomingState(false);});
  if(!m_isSignalFuncCmd)
  {
    m_jobFuns.emplace_back([&]() {return switchAndCheckOpMode(AxisProfileOperationMode::HOMING);});
    m_jobFuns.emplace_back([&]() {return startHomingProcedure();});      
    m_jobFuns.emplace_back([&]() {return initDelayCycles(0.05);}); // delay 0.05 second for synchronization
    m_jobFuns.emplace_back([&]() {return exeDelayCycles();});
  }
  else
  {
    m_jobFuns.emplace_back([&]() {return m_signalFuncCmd.onJobActivating(m_asyncSm);});
  }
}

AxisProfileResult CanopenJobHoming::switchAndCheckOpMode(AxisProfileOperationMode mode)
{
  auto actualMode = m_statusControlIf->getOperationMode();
  if (actualMode == mode)
  {
    APR_TRACE_MESSAGE("%s %s set operation mode to %s",
                      m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                      axisProfileOpModeToString(mode));
    return AxisProfileResult::STS_OK;
  }
  m_statusControlIf->setOperationMode(mode);
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  return AxisProfileResult::STS_PENDING;
}

AxisProfileResult CanopenJobHoming::startHomingProcedure()
{
  WordStruct tmpControlWord;

  tmpControlWord.m_word = 0;
  tmpControlWord.m_bits.bit0 = 1; // switch on
  tmpControlWord.m_bits.bit1 = 1; // enable voltage
  tmpControlWord.m_bits.bit2 = 1; // no quick stop
  tmpControlWord.m_bits.bit3 = 1; // enable operation
  tmpControlWord.m_bits.bit4 = 1; // start homing
  m_statusControlIf->setControlWord(tmpControlWord.m_word);
  m_shareHandle->getScalingIF()->setHomeOffset(0.0); // clear homing offset of control

  APR_TRACE_MESSAGE("%s %s activated...",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str());
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobHoming::checkHomingProcedure()
{
  if (m_cmdData.mode != RefMode::SET_POS_DEVICE)
  {
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
    return AxisProfileResult::STS_OK;
  }

  WordStruct tmpStatusWord;
  tmpStatusWord.m_word = m_statusControlIf->getStatusWord();

  // TODO: watching bit12 (homing attained) changing from 0 to 1 is required.
  // For example, when previous homing command is done, user requests homing command again
  // see bug: https://rb-msvs-alm.de.bosch.com/tfs/Col-DCIATE3_EDC/PD_EDC/_workitems/edit/1010211
  bool homingError = tmpStatusWord.m_bits.bit13;

  if (homingError)
  {
    // TODO: there need a basic function to read acutal error code/text from the drive and report into diagnostic
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_ERROR);
    APR_TRACE_ERROR("Homing error occurred during drive processing of %s.", m_profileInfo.m_profileName.c_str());
    return AxisProfileResult::ERR_JOB_EXECUTION;
  }
  else
  {
    bool targetReached = tmpStatusWord.m_bits.bit10;
    bool homingActive = tmpStatusWord.m_bits.bit12;
    if (targetReached && homingActive)
    {
      // make sure trace only once
      if (m_deviceStatus.get().load() != CmdStatusInDevice::DEVICE_COMPLETED)
      {
        setHomingState(true);
        APR_TRACE_MESSAGE("%s %s completed in device",
                          m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str());
        m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
      }
    }
    else
    {
      if (m_isSignalFuncCmd)
      {
        auto deviceParam = m_shareHandle->getDeviceParamIF();
        bool homed {false};
        auto res = deviceParam->checkAxisHomed(homed);
        if(res!=AxisProfileResult::STS_OK)
        {
          m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);
        }
        if (homed)
        {
          setHomingState(true);
          APR_TRACE_MESSAGE("%s %s completed in device",
                            m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str());
          m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
        }
      }
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobHoming::abortHomingProcedure()
{
  WordStruct tmpControlWord;

  tmpControlWord.m_word = 0;
  tmpControlWord.m_bits.bit0 = 1; // switch on
  tmpControlWord.m_bits.bit1 = 1; // enable voltage
  tmpControlWord.m_bits.bit2 = 1; // no quick stop
  tmpControlWord.m_bits.bit3 = 1; // enable operation
  m_statusControlIf->setControlWord(tmpControlWord.m_word);

  // TODO: for some 3rd party drive, bit12 is always true if has homed before
  //       so a datalayer node will be implemented to enable bit12 checking (similar to soft-motion)
  //WordStruct tmpStatusWord;
  //tmpStatusWord.m_word = m_statusControlIf->getStatusWord();
  //bool homingActive = tmpStatusWord.m_bits.bit12;
  //if (!homingActive)
  //{
  //  APR_TRACE_MESSAGE("%s %s aborted successfully",
  //                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str());
  //  return AxisProfileResult::STS_OK;
  //}
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobHoming::activeHomingByProfile()
{
  auto scaling = m_shareHandle->getScalingIF();
  auto cycleData = m_shareHandle->getCyclicDataIF();

  ActualValuesDevice actValue {};
  auto res = cycleData->getActualValuesDevice(&actValue);
  if (aprStatusFailed(res))
  {
    APR_TRACE_ERROR("%s %s get actual device value failed, ErrCode=0x%X.", m_profileInfo.m_profileName.c_str(),
                    getID().c_str(), static_cast<std::uint16_t>(res));
    return res;
  }

  if (!actValue.m_dataValid)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_PROFILE_FEEDBACK_VALUE_INVALID,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  double actualPosWithoutOffset = actValue.m_actualPosition - scaling->getHomeOffset();
  double newOffset = m_cmdData.newPos - actualPosWithoutOffset;
  scaling->setHomeOffset(newOffset);
  APR_TRACE_MESSAGE("%s %s update offset to %g",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(), newOffset);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobHoming::setHomingState(bool state)
{
  // for Rexroth drive, it is okay to overwrite again after homing completed
  // for 3rd party drive, we have to set it to true to indicate homing is done
  auto deviceParamIf = m_shareHandle->getDeviceParamIF();
  deviceParamIf->setAxisHomed(state);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobHoming::setCmdStatusInDevice(CmdStatusInDevice status)
{
  m_deviceStatus.getRef().store(status);
  return AxisProfileResult::STS_OK;
}
