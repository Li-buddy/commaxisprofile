/// @file
///
/// @brief This file is part of the axisprofile library and implements the CanOpen job of set absolute position.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_setabspos.h"
#include "diagnosis/dia.h"
#include "helper/device_id_helper.h"
#include "profiles/profile_defs.h"

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

CanopenJobSetAbsPos::CanopenJobSetAbsPos(ProfileBasicInfo& profileInfo,
                                         IAxisProfileSharingHandler* shareHandle)
  : CanopenJobBase(profileInfo, shareHandle)
{
}

AxisProfileResult CanopenJobSetAbsPos::onSetArgument(IContainer* arg)
{
  auto setAbsPosArg = ArgSetAbsolutePosition::safeCast(arg);
  if (setAbsPosArg == nullptr)
  {
    APR_TRACE_ERROR("%s %s gets wrong type of argument",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_JOB_DATA_MISMATCH;
  }

  // load inputs
  m_cmdData = setAbsPosArg->getRef();
  APR_TRACE_MESSAGE("%s %s with: cmdPos=%g, encoderId=%d, allowPowerEnabled=%s",
                    m_profileInfo.m_profileTrace.createTraceMsg(), getID().c_str(),
                    m_cmdData.newPos, static_cast<std::int16_t>(m_cmdData.encoder),
                    m_cmdData.allowPowerEnabled ? "true" : "false");

  if (AxisProfileCategory::CATEGORY_ENCODER == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
    return AxisProfileResult::STS_OK;
  }

  auto propertyIf = m_shareHandle->getPropertyIF();
  if (!DeviceIdHelper::isRexrothCtrlXDrive(propertyIf->getDeviceSeries()))
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_NOT_SUPPORT,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Support on ctrlX drive series only");
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  // scaling and validation
  auto scaling = m_shareHandle->getScalingIF();
  auto deviceParamIf = m_shareHandle->getDeviceParamIF();

  m_newRefRaw = scaling->convertPositionScaledToRaw(m_cmdData.newPos);

  // new reference position should in range of travel limit of device
  const auto& deviceCfgData = deviceParamIf->getDeviceCfgData();
  if (deviceCfgData.m_softwareLimitActive && // check only when limitation is activated in device
      (m_newRefRaw > deviceCfgData.m_positivePosLimit || m_newRefRaw < deviceCfgData.m_negativePosLimit))
  {
    // CtrlX drive: 0x607D:1 and 0x607D:2 are positive and negative limit
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_OUT_OF_TRAVEL_LIMIT,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "%g out range of [%s, %s] of device", m_cmdData.newPos,
                       objDirToString(MIN_POS_RANGE_LIMIT).c_str(),
                       objDirToString(MAX_POS_RANGE_LIMIT).c_str());
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  WordStructure encoderProperty;
  ObjectDirEntry encoderPropertyObj;
  if (m_cmdData.encoder == AxisProfileEncoder::ENCODER_1)
  {
    encoderProperty.m_word = deviceCfgData.m_encoder1Property;
    encoderPropertyObj = ENCODER_1_PROPERTY;
    m_newRefObj = REF_DISTANCE;
  }
  else if (m_cmdData.encoder == AxisProfileEncoder::ENCODER_2)
  {
    encoderProperty.m_word = deviceCfgData.m_encoder2Property;
    encoderPropertyObj = ENCODER_2_PROPERTY;
    m_newRefObj = ENCODER_2_REF_DISTANCE;
  }
  else
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_NOT_SUPPORT,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "EncoderId %d not exist", static_cast<std::int16_t>(m_cmdData.encoder));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  // error case: absolute encoder works in relative mode
  if (encoderProperty.m_bits.bit6 && encoderProperty.m_bits.bit7)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_ENCODER_TYPE_ERROR,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Please set %s.bit7 to 0", objDirToString(encoderPropertyObj).c_str());
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  // error case: relative encoder works in relative mode
  if (encoderProperty.m_bits.bit6 == false && encoderProperty.m_bits.bit8 == false)
  {
    Dia::sendDiagnosis(MAIN_DIAG_COMMAND_ERR, DETAILED_DIAG_DEVICE_ENCODER_TYPE_ERROR,
                       m_profileInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Please set %s.bit8 to 1", objDirToString(encoderPropertyObj).c_str());
    return AxisProfileResult::ERR_WRONG_DEVICE_STATE;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobSetAbsPos::onJobEnterActivating()
{
  m_jobFuns.clear();
  m_jobStep = 0;

  if (AxisProfileCategory::CATEGORY_ENCODER == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
    m_jobFuns.emplace_back([&](){return judgmentHandleBased();});
  }
  else if (AxisProfileCategory::CATEGORY_DRIVE == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
    m_jobFuns.emplace_back([&]() {return setupReadListFromDevice();});
    m_jobFuns.emplace_back([&]() {return exeParamListAccess();}); // read parameters from device
    m_jobFuns.emplace_back([&]() {return writeToDeviceU16(CANCEL_REF_POINT, 0x00);}); // clear exist reference point
    m_jobFuns.emplace_back([&]() {return writeToDeviceU16(CANCEL_REF_POINT, 0x03);});
    m_jobFuns.emplace_back([&]() {return writeToDeviceU16(CANCEL_REF_POINT, 0x00);});
    m_jobFuns.emplace_back([&]() {return setupWriteListToDevice();});
    m_jobFuns.emplace_back([&]() {return exeParamListAccess();}); // write parameters to device
    m_jobFuns.emplace_back([&]() {return writeToDeviceU16(SET_ABSOLUTE_POSITION_CMD, 0x00);}); // reset before enable the function
    m_jobFuns.emplace_back([&]() {return writeToDeviceU16(SET_ABSOLUTE_POSITION_CMD, 0x03);});
    m_jobFuns.emplace_back([&]() {return setCmdStatusInDevice(CmdStatusInDevice::DEVICE_IN_PROCESS);});
    m_jobFuns.emplace_back([&]() {return initDelayCycles(0.5);}); // delay 0.1 second for synchronization
    m_jobFuns.emplace_back([&]() {return exeDelayCycles();});
    m_jobFuns.emplace_back([&]() {return setCmdStatusInDevice(CmdStatusInDevice::DEVICE_COMPLETED);});
  }
  else
  {
    APR_TRACE_ERROR("%s %s gets wrong type of profile category",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_NOK;
  }

  return CanopenJobBase::onJobEnterActivating();
}

AxisProfileResult CanopenJobSetAbsPos::onJobActivating()
{
  return runJobsInternal();
}

AxisProfileResult CanopenJobSetAbsPos::onJobEnterAborting()
{
  m_jobFuns.clear();
  m_jobStep = 0;

  if (AxisProfileCategory::CATEGORY_ENCODER == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
    m_jobFuns.emplace_back([&](){return resetIoEncoderData();});
  }
  else if (AxisProfileCategory::CATEGORY_DRIVE == m_shareHandle->getCategoryIF()->getProfileCategory())
  {
    m_jobFuns.emplace_back([&](){return writeToDeviceU16(SET_ABSOLUTE_POSITION_CMD, 0x00);});
    m_jobFuns.emplace_back([&](){m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET); return AxisProfileResult::STS_OK;});
  }
  else
  {
    APR_TRACE_ERROR("%s %s gets wrong type of profile category",
                    getID().c_str(), m_profileInfo.m_profileTrace.createTraceMsg());
    return AxisProfileResult::ERR_NOK;
  }

  return CanopenJobBase::onJobEnterAborting();
}

AxisProfileResult CanopenJobSetAbsPos::onJobAborting()
{
  return runJobsInternal();
}

////////////////////////////////////////// private methods //////////////////////////////////////////// 

AxisProfileResult CanopenJobSetAbsPos::setupReadListFromDevice()
{
  m_paramList.clear();
  m_paramList.emplace_back(ReadWriteType::READ, getObjDir(SET_ABSOLUTE_POSITION_CONTROL, m_profileInfo.m_driveNumber),
                            &m_ctrlWordS448.m_word, sizeof(m_ctrlWordS448.m_word));
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobSetAbsPos::setupWriteListToDevice()
{
  m_ctrlWordS448.m_bits.bit2 = true;
  if (m_cmdData.encoder == AxisProfileEncoder::ENCODER_1)
  {
    m_ctrlWordS448.m_bits.bit0 = true;
    m_ctrlWordS448.m_bits.bit1 = false;
  }
  else
  {
    m_ctrlWordS448.m_bits.bit0 = false;
    m_ctrlWordS448.m_bits.bit1 = true;
  }

  m_paramList.clear();
  m_paramList.emplace_back(ReadWriteType::WRITE, getObjDir(SET_ABSOLUTE_POSITION_CONTROL, m_profileInfo.m_driveNumber),
                           &m_ctrlWordS448.m_word, sizeof(m_ctrlWordS448.m_word));
  m_paramList.emplace_back(ReadWriteType::WRITE, getObjDir(m_newRefObj, m_profileInfo.m_driveNumber),
                           &m_newRefRaw, sizeof(m_newRefRaw));

  m_shareHandle->getScalingIF()->setHomeOffset(0.0); // clear homing offset of control
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobSetAbsPos::judgmentHandleBased()
{
  if (!m_shareHandle || !m_shareHandle->getMappingHandlerIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto mappingHandler = m_shareHandle->getMappingHandlerIF();
  const auto& assignedInfo = mappingHandler->getProfileAssignedInfo();
  if (!assignedInfo.m_input.m_isInReference ||
      !assignedInfo.m_output.m_setReference ||
      !assignedInfo.m_output.m_referenceOffset)
  {
    APR_TRACE_MESSAGE("set absolute position : handle as control based.");
    // handle as control based
    return AxisProfileResult::STS_OK;  //The return value needs to be changed.
  }

  // handle as device based
  // handle by IO device
  m_shareHandle->getCyclicDataIF()->setNewRefPosition(0);

  return setIoEncoderData();
}

AxisProfileResult CanopenJobSetAbsPos::setIoEncoderData()
{
  std::int32_t newRefPos = static_cast<std::uint32_t>(m_cmdData.newPos);
  m_shareHandle->getCyclicDataIF()->setNewRefPosition(newRefPos);
  m_shareHandle->getCyclicDataIF()->activeNewRefOperation(true);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobSetAbsPos::resetIoEncoderData()
{
  m_shareHandle->getCyclicDataIF()->activeNewRefOperation(false);
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenJobSetAbsPos::setCmdStatusInDevice(CmdStatusInDevice status)
{
  m_deviceStatus.getRef().store(status);
  return AxisProfileResult::STS_OK;
}
