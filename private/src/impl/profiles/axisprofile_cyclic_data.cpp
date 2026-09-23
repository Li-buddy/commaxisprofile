/// @file
///
/// This source file is part of the axisprofile library and implements
/// base class for cyclic data handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_cyclic_data.h"
#include "bundle/axisprofile_cyclic_data_c.h"
#include "common/fpu_helper.h"
#include "diagnosis/dia.h"
#include "fieldbuses/fieldbus_defs.h"
#include "common/string_helper.h"

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

AxisProfileCyclicData::AxisProfileCyclicData(const ProfileBasicInfo& profileBasicInfo)
  : m_profileBasicInfo(profileBasicInfo)
{
  // *** Initialize C-Interface *** //
  m_cInterface.handle = this;
  m_cInterface.getActualValuesDevice = AXISPROFILE_getActualValuesDevice;
  m_cInterface.setCommandValuesDevice = AXISPROFILE_setCommandValuesDevice;
  m_cInterface.getDigitalInputValueBit = AXISPROFILE_getDigitalInputValueBit;
  m_cInterface.checkActualValueAvailabilityNRT = AXISPROFILE_checkActualValueAvailabilityNRT;
  m_cInterface.deprecatedGetLastInputSyncTime = nullptr;
  m_cInterface.deprecatedGetLastOutputSyncTime = nullptr;
  m_cInterface.getSafetyOperationMode = AXISPROFILE_getSafetyOperationMode;
  m_cInterface.getSingleUserPDODataFromDevice = AXISPROFILE_getSingleUserPDODataFromDevice;
  m_cInterface.setSingleUserPDODataToDevice = AXISPROFILE_setSingleUserPDODataToDevice;
  m_cInterface.checkSingleUserPDOActiveFromDevice = AXISPROFILE_checkSingleUserPDOActiveFromDevice;
  m_cInterface.checkSingleUserPDOActiveToDevice = AXISPROFILE_checkSingleUserPDOActiveToDevice;
  m_cInterface.getEffectiveTimeOfActOutput = AXISPROFILE_getEffectiveTimeOfActOutput;
  m_cInterface.getLatchTimeOfActInput = AXISPROFILE_getLatchTimeOfActInput;
}

const IAxisProfileCyclicDataC* AxisProfileCyclicData::getCInterface() const
{
  return &m_cInterface;
}

void AxisProfileCyclicData::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

void AxisProfileCyclicData::resetFPUFlag()
{
  fpu::g_FPUError = false;
  // Clear all FPU exceptions
  fpu::clearFPUFlags();
}

bool AxisProfileCyclicData::checkIfTelegramItemAssigned(AxisProfileValueTypeID valueID, DataAccessErrorInfo& msg)
{
  if (!m_sharingHandler || !m_sharingHandler->getMappingHandlerIF())
  {
    return false;
  }
  const ProfileAssignedInfo& ProfileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  switch (valueID)
  {
    case AxisProfileValueTypeID::ACTUAL_POSITION:
      if (!ProfileAssignedInfo.m_input.m_position)
      {
        msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR;
        msg.m_additionInfo = "POSITION : " + std::string(dataAccessCommonErrorTypeToString(msg.m_accessErrorType));
        return false;
      }
      break;
    case AxisProfileValueTypeID::ACTUAL_VELOCITY:
      if (!ProfileAssignedInfo.m_input.m_velocity)
      {
        msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR;
        msg.m_additionInfo = "VELOCITY : " + std::string(dataAccessCommonErrorTypeToString(msg.m_accessErrorType));
        return false;
      }
      break;
    case AxisProfileValueTypeID::ACTUAL_TORQUE:  // now skip for check
      if (!ProfileAssignedInfo.m_input.m_torqueForce)
      {
        msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR;
        msg.m_additionInfo = "TORQUE : " + std::string(dataAccessCommonErrorTypeToString(msg.m_accessErrorType));
        return false;
      }
      break;
    case AxisProfileValueTypeID::ACTUAL_ACCELERATION:  // now skip for check    
    case AxisProfileValueTypeID::LAST_ID:
    default:
      msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_NOT_PROVIDED;
      msg.m_additionInfo = dataAccessCommonErrorTypeToString(msg.m_accessErrorType);
      return false;
  }
  return true;
}

bool AxisProfileCyclicData::checkFieldbusStatus(DataAccessErrorInfo& errInfo)
{
  std::string masterName {};
  std::string wholeAddress = m_profileBasicInfo.m_fieldbusAdapter->getFieldbusMasterAddress();
  // get instance name
  // fieldbuses/ethercat/master/instances/ethercatmaster
  std::string ins {"instances"};
  size_t insPos = wholeAddress.find(ins);
  if (std::string::npos == insPos || wholeAddress.length() <= DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES.length())
  {
    errInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_MASTER_STATUS;
    errInfo.m_additionInfo = std::string(dataAccessCommonErrorTypeToString(errInfo.m_accessErrorType)) + " : no master instance exist";
    Dia::sendDiagnosis(MAIN_DIAG_FIELDBUS_ERR, DETAILED_DIAG_MASTER_STATUS_ERROR, __FILE__, __FUNCTION__, __LINE__, "no master instance exist.");
    return false;
  }
  size_t length = wholeAddress.length() - insPos - 1 - ins.length();
  masterName = wholeAddress.substr(insPos + ins.length() + 1, length);

  MasterStatus status;
  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readFieldbusMasterStatusSync(status.m_status);
  if (aprStatusFailed(result))
  {
    APR_TRACE_ERROR("Read master status failed on %s.", masterName.c_str());
    errInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_MASTER_STATUS;
    errInfo.m_additionInfo = std::string(dataAccessCommonErrorTypeToString(errInfo.m_accessErrorType)) + " : failed to read master status";
    Dia::sendDiagnosis(MAIN_DIAG_FIELDBUS_ERR, DETAILED_DIAG_MASTER_STATUS_ERROR, __FILE__, __FUNCTION__, __LINE__, "Read master status failed on %s.", masterName.c_str());
    return false;
  }

  // Not in op state
  if (status.m_statusBits.NotInStateOp)
  {
    APR_TRACE_ERROR("master %s status is %d, not in op state.", masterName.c_str(), status.m_status);
    errInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_MASTER_STATUS;
    errInfo.m_additionInfo = " master name: " + masterName + " " + std::string(dataAccessCommonErrorTypeToString(errInfo.m_accessErrorType)) +
      " master status: " + std::to_string(status.m_status);
    Dia::sendDiagnosis(MAIN_DIAG_FIELDBUS_ERR, DETAILED_DIAG_MASTER_STATUS_ERROR, __FILE__, __FUNCTION__, __LINE__, "master %s status is %d, not in op state.", masterName.c_str(), status.m_status);
    return false;
  }

  // status.m_statusBits.TopologyNotOK || bugfix563709, acceptable case: the slave is not used by motion
  // status.m_statusBits.SlavesError || bugfix563709, acceptable case: the slave is not used by motion
  if (status.m_statusBits.NoEthernetLink ||
      status.m_statusBits.SettingsInconsistent ||
      status.m_statusBits.PortError ||
      status.m_statusBits.LicenseError)
  {
    MasterStatus tmpStatus = status;
    tmpStatus.m_statusBits.TopologyNotOK = false; // bugfix563709
    tmpStatus.m_statusBits.SlavesError = false;
    APR_TRACE_ERROR("master %s status is %s, please check setting of the master.", masterName.c_str(), getMasterHighestErrorInfoString(tmpStatus));
    errInfo.m_accessErrorType = DataAccessCommonErrorType::ERROR_MASTER_STATUS;
    errInfo.m_additionInfo = " master name: " + masterName + " " + std::string(dataAccessCommonErrorTypeToString(errInfo.m_accessErrorType)) +
      " master status: " + getMasterHighestErrorInfoString(tmpStatus);
    Dia::sendDiagnosis(MAIN_DIAG_FIELDBUS_ERR, DETAILED_DIAG_MASTER_STATUS_ERROR, __FILE__, __FUNCTION__, __LINE__, "master %s status is %s, please check setting of the master.", masterName.c_str(), getMasterHighestErrorInfoString(tmpStatus));
    return false;
  }

  return true;
}

bool AxisProfileCyclicData::checkForSlaveStatus(DataAccessErrorInfo& msg)
{
  SlaveStatus status;
  ReadSlaveStateData readSlaveStateData {};
  readSlaveStateData.m_deviceAddress = m_profileBasicInfo.m_deviceAddress;
  AxisProfileResult aprResult = m_profileBasicInfo.m_fieldbusAdapter->readSlaveStatusSync(readSlaveStateData, status.m_status);
  if (aprStatusFailed(aprResult))
  {
    APR_TRACE_ERROR("Read slave status failed on profile %s.", m_profileBasicInfo.m_profileName.c_str());
    msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_MASTER_STATUS;
    msg.m_additionInfo = std::string(dataAccessCommonErrorTypeToString(msg.m_accessErrorType)) + " : read slave status failed";
    Dia::sendDiagnosis(MAIN_DIAG_FIELDBUS_ERR, DETAILED_DIAG_SLAVE_STATUS_ERROR, __FILE__, __FUNCTION__, __LINE__, "Read slave status failed on profile %s.", m_profileBasicInfo.m_profileName.c_str());
    return false;
  }
  // error / not on line
  if (status.m_statusBits.Error || status.m_statusBits.NotOnline)
  {
    APR_TRACE_ERROR("%s status is %s, check setting and connection of the slave.", m_profileBasicInfo.m_profileName.c_str(), getSlaveHighestErrorInfoString(status));
    msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_SLAVE_STATUS;
    msg.m_additionInfo = std::string(dataAccessCommonErrorTypeToString(msg.m_accessErrorType)) + " : slave not online";
    Dia::sendDiagnosis(MAIN_DIAG_FIELDBUS_ERR, DETAILED_DIAG_SLAVE_STATUS_ERROR, __FILE__, __FUNCTION__, __LINE__, "%s status is %s, check setting and connection of the slave.", m_profileBasicInfo.m_profileName.c_str(), getSlaveHighestErrorInfoString(status));
    return false;
  }
  // not in op
  if (status.m_statusBits.NotInStateOp)
  {
    APR_TRACE_ERROR("%s status is %s, slave not in op.", m_profileBasicInfo.m_profileName.c_str(), getSlaveHighestErrorInfoString(status));
    msg.m_accessErrorType = DataAccessCommonErrorType::ERROR_SLAVE_STATUS;
    msg.m_additionInfo = std::string(dataAccessCommonErrorTypeToString(msg.m_accessErrorType)) + " : not in op state";
    Dia::sendDiagnosis(MAIN_DIAG_FIELDBUS_ERR, DETAILED_DIAG_SLAVE_STATUS_ERROR, __FILE__, __FUNCTION__, __LINE__, "%s status is %s, slave not in op.", m_profileBasicInfo.m_profileName.c_str(), getSlaveHighestErrorInfoString(status));
    return false;
  }
  return true;
}


bool AxisProfileCyclicData::checkActualValueAvailabilityNRT(AxisProfileValueTypeID valueID, DataAccessErrorInfo& errInfo)
{
  // step 0 check if mapped
  bool result = checkIfTelegramItemAssigned(valueID, errInfo);
  if (!result)
  {
    return result;
  }
  // step 1 check field bus
  // step 2 check master status
  result = checkFieldbusStatus(errInfo);
  if (!result)
  {
    return result;
  }

  // step 3 check slave status
  return checkForSlaveStatus(errInfo);
}

void AxisProfileCyclicData::setActSyncTime(double absSync0Time, double fieldbusCycleTime)
{
  // actual input is latch 2 cycle ago in drive
  m_actInputLatchTime = absSync0Time - fieldbusCycleTime * INPUT_SHIFT_CYCLE;

  // default delay 1 cycle: fieldbus transmit need 1 cycle, slave use it immediately
  // assume 3rd party driver has no buffer, if it has buffer, user has to set it in probe/cfg/deadtime
  m_actOutputEffectiveTime = absSync0Time + fieldbusCycleTime * OUTPUT_SHIFT_CYCLE;

  // actual output will be effective sever cycles later, depend on driver interpolation type
  double outputEffectiveDelay {};
  if (m_sharingHandler->getDeviceParamIF()->getOutputEffectiveDelay(outputEffectiveDelay) == AxisProfileResult::STS_OK)
  {
    m_actOutputEffectiveTime += outputEffectiveDelay;
  }
}

double AxisProfileCyclicData::getEffectiveTimeOfActOutput()
{
  return m_actOutputEffectiveTime;
};

double AxisProfileCyclicData::getLatchTimeOfActInput()
{
  return m_actInputLatchTime;
};

const StateMachineState*& AxisProfileCyclicData::getStatemachine()
{
  return m_stateMachineState;
}

AxisProfileResult AxisProfileCyclicData::setStateMachineStatusIF(const StateMachineState* const stateMachineState)
{
  if (!stateMachineState)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  m_stateMachineState = stateMachineState;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileCyclicData::booting()
{
  loadBootingTasks();
  AxisProfileResult result {AxisProfileResult::STS_OK};
  for (const auto& method : m_initCommands)
  {
    result = method();
    if (aprStatusNotOkay(result))
    {
      return result;
    }
  }
  return result;
}

AxisProfileResult AxisProfileCyclicData::setNewRefPosition(std::int32_t /*newRefPos*/)
{
  return AxisProfileResult::STS_OK;
};

AxisProfileResult AxisProfileCyclicData::activeNewRefOperation(bool /*active*/)
{
  return AxisProfileResult::STS_OK;
};
