/// @file
///
/// @brief This file is part of the axisprofile library and implements the CanOpen get operation mode job.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_getopmode.h"
#include <iostream>
// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

CanopenJobGetOpMode::CanopenJobGetOpMode(ProfileBasicInfo& profileInfo,
                                         IAxisProfileSharingHandler* shareHandle)
  : CanopenJobBase(profileInfo, shareHandle)
{
}

const IContainer* CanopenJobGetOpMode::getModeData()
{
  return &m_opModeData;
}

AxisProfileResult CanopenJobGetOpMode::onJobEnterActivating()
{
  std::cout << "CanopenJobGetOpMode::onJobEnterActivating()" << std::endl;
  m_opModeData.getRef() = AxisProfileOperationMode::OPMODE_PENDING;
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_IN_PROCESS);

  //
  const ProfileAssignedInfo& profileAssignedInfo = m_shareHandle->getMappingHandlerIF()->getProfileAssignedInfo();
  if (profileAssignedInfo.m_input.m_opModeDisplay)
  {
    m_isCyclicData = true;
  }
  return CanopenJobBase::onJobEnterActivating();
}

AxisProfileResult CanopenJobGetOpMode::onJobActivating()
{
  if (m_isCyclicData)
  {
    std::cout << "CanopenJobGetOpMode::onJobActivating()-> handle as cyclic" << std::endl;
    auto mode = m_statusControlIf->getOperationMode();
    m_opModeData.getRef() = mode;
    std::cout << "mode : "<< axisProfileOpModeToString(mode) << std::endl;
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
    return AxisProfileResult::STS_OK;
  }

  std::cout << "CanopenJobGetOpMode::onJobActivating()" << std::endl;

  AxisProfileResult result = m_paramAccessIf->readAndCheckParamAsync(
    getObjIndex(MODES_OF_OPERATION_DISPLAY.index, m_profileInfo.m_driveNumber),
    MODES_OF_OPERATION_DISPLAY.subIndex,
    &m_rawMode,
    sizeof(m_rawMode),
    m_smStatus);
  std::cout << "reading result : " << (int)result << std::endl;
  if (result == AxisProfileResult::STS_PENDING)
  {
    m_opModeData.getRef() = AxisProfileOperationMode::OPMODE_PENDING;
    return result;
  }

  if (result == AxisProfileResult::STS_OK)
  {
    m_opModeData.getRef() = convertFromCanopenOpMode(m_rawMode);
    m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_COMPLETED);
    return result;
  }

  m_opModeData.getRef() = AxisProfileOperationMode::UNDEFINED;
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_ERROR);
  return result;
}

AxisProfileResult CanopenJobGetOpMode::onJobEnterAborting()
{
  std::cout << "CanopenJobGetOpMode::onJobEnterAborting()" << std::endl;
  m_deviceStatus.getRef().store(CmdStatusInDevice::DEVICE_NOT_SET);
  return AxisProfileResult::STS_OK;
}
