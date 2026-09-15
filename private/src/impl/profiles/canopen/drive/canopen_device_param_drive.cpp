/// @file
///
/// This header file is part of the axisprofile library and implement CanopenDeviceParamDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_device_param_drive.h"
#include "diagnosis/dia.h"
#include "fieldbuses/fieldbus_helper.h"
#include "helper/checksum_idn_list_helper.h"
#include "helper/device_id_helper.h"
#include "profiles/canopen/canopen_boot_params.h"
#include <algorithm>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;


// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------
constexpr std::size_t LIST_BUFF_CHECKSUM_PARAM_LIST_SIZE = 2048;       ///< Max buffer size for IDN list of checksum parameters

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

CanopenDeviceParamDrive::CanopenDeviceParamDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
  :AxisProfileDeviceParamDrive(profileBasicInfo, at, mdt), m_fieldbusAdapter(profileBasicInfo.m_fieldbusAdapter),
  m_driveNumber(profileBasicInfo.m_driveNumber)
{
  // Register function of homing status checking with cyclic data.
  registerSignalStatusWordFuncs(FunctionSupport::HOMING_STATUS_CYCLIC, [](const std::uint32_t& param, const std::uint16_t& bit) -> bool
    {
      // Homing status is supported when this object is configured in the signal status word.
      return static_cast<std::uint16_t>(param >> 16) == HOMING_FEEDBACK_VALUE_STATUS.index
      && static_cast<std::uint8_t>(param>>8) == HOMING_FEEDBACK_VALUE_STATUS.subIndex && bit == 0;
    });
}

void CanopenDeviceParamDrive::loadBootingTasks()
{
  m_initDeviceParam.clear();
  m_initDeviceParam.reserve(8);
  m_initDeviceParam.emplace_back([this]{return getDeviceParamJobObj();});
  m_initDeviceParam.emplace_back([this]{return determineOpModeAvailable();});
  m_initDeviceParam.emplace_back([this]{return readProbeLatchedParam();});
  m_initDeviceParam.emplace_back([this]{return initHomeState();});

  if (!m_needReadBootParams)
  {
    APR_TRACE_MESSAGE("%s, Not need to read CoE drive parameters again.", m_profileBasicInfo.m_profileTrace.createTraceMsg());
    return ;
  }
  m_initDeviceParam.emplace_back([this]{return checkDeviceQuickStopCfg();});
  m_initDeviceParam.emplace_back([this]{return readDcConfigInfo();});
  m_initDeviceParam.emplace_back([this]{return checkSignalStatusWord();});
  m_initDeviceParam.emplace_back([this]{return readEncoderTypes();});
}

AxisProfileResult CanopenDeviceParamDrive::readDelayBrakeParam()
{
  m_brakeDelySupport = false;
  return  AxisProfileResult::STS_OK;   
}

AxisProfileResult CanopenDeviceParamDrive::getDelayBrakeInfo()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenDeviceParamDrive::getBrakeDynLimit()
{
  m_decelerationLimitSupport = false;
  return AxisProfileResult::STS_OK;
}

OpModeAvailInfo* CanopenDeviceParamDrive::getOpModeAvailInfo()
{
  return &m_opModeAvailInfoCanopen;
}

bool CanopenDeviceParamDrive::checkFunctionSupported(const FunctionSupport function, FunctionSupportErrorInfo& errorInfo)
{
  bool result = true;
  switch (function)
  {
    case FunctionSupport::OPMODE_CYCLIC:
    {
      const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
      if (!profileAssignedInfo.m_output.m_opMode || !profileAssignedInfo.m_input.m_opModeDisplay)
      {
        result = false;
        errorInfo = {FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, "operation mode cyclic data was not mapped"};
      }
    }
    break;
    case FunctionSupport::HOMING_STATUS_CYCLIC:
    {
      SignalStatusWordFunction func;
      func.function = function;
      result = isFuncInSignalStatusWord(func, errorInfo);
    }
    break;
    case FunctionSupport::ERROR_CODE_CYCLIC:
    {
      const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
      if (!profileAssignedInfo.m_input.m_diagcode)
      {
        result = false;
        auto deviceSeries = m_sharingHandler->getPropertyIF()->getDeviceSeries();
        std::string addInfo = DeviceIdHelper::isRexrothDrive(deviceSeries) ? "0x1002 or 0x603F" : "corresponding diagnostic message";
        errorInfo = {FuncSupportErrorType::ERROR_CYCLIC_DATA_CONFIG_NOT_SUPPORT, addInfo};
      }
    }
    break;
    default:
    {
      result = false;
      errorInfo = {FuncSupportErrorType::ERROR_NOT_DEFINED, "invalid input parameter"};
    }
    break;
  }
  return result;
}

AxisProfileResult CanopenDeviceParamDrive::readSignalStatusWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits)
{
  auto syncReader = [this](const ObjectDirEntry& obj, void* data, std::size_t size)
  {
    std::memset(data, 0, size);
    // prepare request parameters
    ParamReqDataCoE requestData {m_profileBasicInfo.m_deviceAddress, obj};

    // NOTE: different from SoE response, no actual data size indicated from response data package.
    AxisProfileResult res = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(data), size);
    if (aprStatusFailed(res))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "Signal status, CoE: %s, error: 0x%08X", objDirToString(requestData.objEntry).c_str(), res);
    }
    return res;
  };

  // Signal status word: Configuration list (0x2C82:02 / S-0-0026), Data length 4 bytes var
  // Maximum of 16 parameters can be configured.
  // The lists in 0x2C82:02 and 0x2C82:03 have the same length
  std::uint8_t maxCfgParams = 16;
  std::unique_ptr<std::uint32_t[]> paramsBuf = std::make_unique<std::uint32_t[]>(maxCfgParams);
  auto res = syncReader(getObjDir(SIGNAL_STATUS_WORD_CONFIG_LIST, m_profileBasicInfo.m_driveNumber), paramsBuf.get(), maxCfgParams * sizeof(std::uint32_t));
  if (aprStatusSucceeded(res))
  {
    std::uint32_t* firstParam = paramsBuf.get();
    for (auto i = 0; i < maxCfgParams; ++i)
    {
      // The read result (4bytes per element) contains index and sub-index of CoE object.
      params.emplace_back(*(firstParam + i));
    }

    // Signal status word: Assign list signal (0x2C82:03 / S-0-0328), Data length 2 bytes var.
    std::unique_ptr<std::uint16_t[]> bitsBuf = std::make_unique<std::uint16_t[]>(maxCfgParams);
    res = syncReader(getObjDir(SIGNAL_STATUS_WORD_ASSIGN_LIST, m_profileBasicInfo.m_driveNumber), bitsBuf.get(), maxCfgParams * sizeof(std::uint16_t));
    if (aprStatusSucceeded(res))
    {
      std::uint16_t* firstBit = bitsBuf.get();
      for (auto i = 0; i < maxCfgParams; ++i)
      {
        assignBits.emplace_back(*(firstBit + i));
      }
    }
  }

  return res;
}

AxisProfileResult CanopenDeviceParamDrive::determineOpModeAvailable()
{
  std::uint32_t driveModes = 0;
  std::uint32_t bufsize = sizeof(driveModes);
  void* buf = &driveModes;
  ParamReqDataCoE requestData {m_profileBasicInfo.m_deviceAddress, getObjDir(SUPPORTED_DRIVE_MODE, m_driveNumber)};
  AxisProfileResult result = m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(buf), bufsize);
  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Supported OP modes, PDO %s, slaveId %d", objDirToString(requestData.objEntry).c_str(), requestData.address);
    return result;
  }
  m_opModeAvailInfoCanopen.clearOpModeAvailInfoCanopen();

  // parse flag which contains the mode supported information.
  // flag of the csp (cyclic synchronous position mode) is bit7
  m_opModeAvailInfoCanopen.positionAvail.available = (driveModes & MASK1(32, 1, 7)) != 0;
  // flag of the csv (cyclic synchronous velocity mode) is bit8
  m_opModeAvailInfoCanopen.velocityAvail.available = (driveModes & MASK1(32, 1, 8)) != 0;
  // flag of the torque force available information is not supported at current stage.
  m_opModeAvailInfoCanopen.torqueForceAvail.available = (driveModes & MASK1(32, 1, 9)) != 0;

  if (m_opModeAvailInfoCanopen.torqueForceAvail.available)
  {
    std::uint8_t torqueOpt = 0;
    ParamReqDataCoE reqTorqueData {m_profileBasicInfo.m_deviceAddress, getObjDir(TORQUE_FORCE_OPTIONS_CODE, m_driveNumber)};
    result = m_fieldbusAdapter->readSingleParamSync(reqTorqueData, &torqueOpt, sizeof(torqueOpt));
    if (aprStatusFailed(result))
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                         "Torque/Force options code, PDO %s, slaveId %d", objDirToString(requestData.objEntry).c_str(), requestData.address);
      // By pass for older driver and 3rd party drive
    }
    // The torque mode: Torque/force control with velocity limit will be chosen
    // when the CoE object torque/Force options(0x60CA) being assigned value 1 or 4
    if (torqueOpt != 1 && torqueOpt != 4)
    {
      m_opModeAvailInfoCanopen.torqueForceAvail.available = false;
      m_opModeAvailInfoCanopen.torqueForceAvail.message = "Please set PDO 0x60CA to 1 or 4";
    }
  }
  const ProfileAssignedInfo& profileAssignedInfo = m_sharingHandler->getMappingHandlerIF()->getProfileAssignedInfo();
  if (profileAssignedInfo.m_output.m_opMode)
  {
    // Set default operation mode to avoid invalid operation mode causes power on failure.
    m_telegramMDT->m_mdtData.m_opMode = CANOPEN_POSITION_MODE;
    if (!m_opModeAvailInfoCanopen.positionAvail.available && m_opModeAvailInfoCanopen.velocityAvail.available)
    {
      m_telegramMDT->m_mdtData.m_opMode = CANOPEN_VELOCITY_MODE;
    }
    if (!m_opModeAvailInfoCanopen.positionAvail.available && !m_opModeAvailInfoCanopen.velocityAvail.available && m_opModeAvailInfoCanopen.torqueForceAvail.available)
    {
      m_telegramMDT->m_mdtData.m_opMode = CANOPEN_FORCE_VEL_LIMIT;
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenDeviceParamDrive::checkDeviceQuickStopCfg()
{
  DeviceSeries series = m_sharingHandler->getPropertyIF()->getDeviceSeries();
  if (!DeviceIdHelper::isRexrothDrive(series))
  {
    return AxisProfileResult::STS_OK;
  }

  std::int16_t driveQuickStopOption = 0;
  std::uint32_t bufsize = sizeof(driveQuickStopOption);
  void* buf = &driveQuickStopOption;

  ParamReqDataCoE requestData {m_profileBasicInfo.m_deviceAddress, getObjDir(QUICK_STOP_OPTION, m_driveNumber)};
  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(buf), bufsize);
  if (aprStatusFailed(result))
  {
    // This should return error, but due to the ctrlX Drive CoE is still in develop,
    // some objects may not been supported in every firmware version.
    // The return value should be changed to error when the ctrlX Drive CoE in is in stable version 
    APR_TRACE_MESSAGE("%s can't be written successfully, check if the device %d support the object",
                      objDirToString(requestData.objEntry).c_str(), requestData.address);
  }

  // Until RM2503, this method have only one judgment: quick stop option code 0x605A
  const std::int16_t expQuickStopValue = 6;
  if (driveQuickStopOption != expQuickStopValue)
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_DEVICE_CONFIG_ERROR,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Quick stop option code %s should be 6", objDirToString(requestData.objEntry).c_str());
    //TODO: add return error after test OK in MOT0206 RC1 app.
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenDeviceParamDrive::getDeviceInformation()
{
  std::array<std::uint8_t, LIST_BUFF_DEFAULT_SIZE> rawBuf {}; // buffer for raw data reading

  CanopenParamStruct readResponseParam(SOFTWARE_VERSION.index, SOFTWARE_VERSION.subIndex, rawBuf.data(), rawBuf.size());
  ParamReqDataCoE requestData {m_profileBasicInfo.m_deviceAddress, getObjDir(readResponseParam.m_objDirEntry, m_driveNumber)};
  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(readResponseParam.m_ptrData), readResponseParam.dataSize);

  if (aprStatusFailed(result))
  {
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED,
                       m_profileBasicInfo.m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__,
                       "Object %s, error: 0x%08X", objDirToString(requestData.objEntry).c_str(), result);
  }

  std::string versionStr;
  versionStr.assign(reinterpret_cast<const char*>(rawBuf.data()), rawBuf.size());
  APR_TRACE_MESSAGE("Device address: %u, firmware version: %s", m_profileBasicInfo.m_deviceAddress, versionStr.c_str());

  result = m_sharingHandler->getPropertyIF()->setDeviceManufacturerVersion(versionStr); // set manufacturer version,then can get software version
  if (aprStatusFailed(result))
  {
    APR_TRACE_WARNING("Software version parse failed. IDN %s value: %s, error: 0x%X", objDirToString(readResponseParam.m_objDirEntry).c_str(), versionStr.c_str(), result);
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_PARSE_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Software version parse failed. IDN %s value: %s, error: 0x%X",
                       objDirToString(readResponseParam.m_objDirEntry).c_str(), versionStr.c_str(), result);

  }

  return AxisProfileResult::STS_OK;                                                                                                                   
}

AxisProfileResult CanopenDeviceParamDrive::readProbeLatchedParam()
{
  ParamReqDataCoE requestData {};
  requestData.address = m_profileBasicInfo.m_deviceAddress;

  std::vector<CanopenParamStruct> readStructSet;
  readStructSet.reserve(4);
  readStructSet.emplace_back(PROBE1_POS_LATCHED_TIMES.index, PROBE1_POS_LATCHED_TIMES.subIndex, &m_bootingInfoPrb.m_probe1PosLatchedTimes, sizeof(m_bootingInfoPrb.m_probe1PosLatchedTimes));
  readStructSet.emplace_back(PROBE1_NEG_LATCHED_TIMES.index, PROBE1_NEG_LATCHED_TIMES.subIndex, &m_bootingInfoPrb.m_probe1NegLatchedTimes, sizeof(m_bootingInfoPrb.m_probe1NegLatchedTimes));
  readStructSet.emplace_back(PROBE2_POS_LATCHED_TIMES.index, PROBE2_POS_LATCHED_TIMES.subIndex, &m_bootingInfoPrb.m_probe2PosLatchedTimes, sizeof(m_bootingInfoPrb.m_probe2PosLatchedTimes));
  readStructSet.emplace_back(PROBE2_NEG_LATCHED_TIMES.index, PROBE2_NEG_LATCHED_TIMES.subIndex, &m_bootingInfoPrb.m_probe2NegLatchedTimes, sizeof(m_bootingInfoPrb.m_probe2NegLatchedTimes));

  for (const auto& readParam : readStructSet)
  {
    requestData.objEntry = readParam.m_objDirEntry;
    AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(readParam.m_ptrData), readParam.dataSize);
    if (aprStatusFailed(result))
    {
      // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
      APR_TRACE_MESSAGE("%s; read object %s failed at slaveId %d, check if the device supports it",
                        m_profileBasicInfo.m_profileTrace.createTraceMsg(result),
                        objDirToString(requestData.objEntry).c_str(),
                        requestData.address);
      m_prbLatTimesSupport = false;
      return AxisProfileResult::STS_OK;
      // LCOV_EXCL_STOP - exclude code coverage section end
    }
  }
  m_prbLatTimesSupport = true;
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenDeviceParamDrive::readEncoderTypes()
{
  auto* propertyIf = m_sharingHandler->getPropertyIF();
  auto deviceSeries = propertyIf->getDeviceSeries();
  if (!DeviceIdHelper::isRexrothCtrlXDrive(deviceSeries))
  {
    APR_TRACE_MESSAGE("%s; device type is %s, encoder property not exist.",
                      m_profileBasicInfo.m_profileTrace.createTraceMsg(),
                      deviceSeriesToString(deviceSeries));
    return AxisProfileResult::STS_OK;
  }

  ParamReqDataCoE requestData {};
  requestData.address = m_profileBasicInfo.m_deviceAddress;
  std::vector<CanopenParamStruct> readStructSet {};
  readStructSet.reserve(2);
  readStructSet.emplace_back(ENCODER_1_PROPERTY, &m_deviceCfgData.m_encoder1Property, sizeof(m_deviceCfgData.m_encoder1Property));
  readStructSet.emplace_back(ENCODER_2_PROPERTY, &m_deviceCfgData.m_encoder2Property, sizeof(m_deviceCfgData.m_encoder2Property));
  for (const auto& readParam : readStructSet)
  {
    requestData.objEntry= readParam.m_objDirEntry;
    auto result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, static_cast<std::uint8_t*>(readParam.m_ptrData), readParam.dataSize);
    if (aprStatusFailed(result))
    {
      // LCOV_EXCL_START - exclude code coverage section begin because this is only an fallback. Case could never happen.
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, m_profileBasicInfo.m_profileName.c_str(),
                         __FILE__, __FUNCTION__, __LINE__, "Encoder type, ObjDir: %s, error: 0x%X",
                         objDirToString(requestData.objEntry).c_str(), result);
      // LCOV_EXCL_STOP - exclude code coverage section end
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenDeviceParamDrive::getDeviceParamJobObj()
{
  if (!m_sharingHandler || !m_sharingHandler->getJobMgrIF())
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_JobObj = m_sharingHandler->getJobMgrIF()->getJobObj(CanopenJobDeviceParam::jobName());
  if (m_JobObj == nullptr)
  {
    APR_TRACE_MESSAGE("%s; %s not exit", m_profileBasicInfo.m_profileTrace.createTraceMsg(), CanopenJobDeviceParam::jobName());
    return AxisProfileResult::ERR_EXEC_NULL;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CanopenDeviceParamDrive::checkStandstill(bool& standStill)
{
  auto* propertyIf = m_sharingHandler->getPropertyIF();
  auto deviceSeries = propertyIf->getDeviceSeries();
  if (DeviceIdHelper::isRexrothDrive(deviceSeries))
  {
    std::uint16_t value {};
    auto res = deviceParamCoE(m_profileBasicInfo, m_JobObj, ReadWriteType::READ,
                              STANDSTILL_STATUS, value, sizeof(value));
    if (res == AxisProfileResult::STS_OK)
    {
      // bit0 of 0x2212:04/S-0-0331 indicates the standstill status of bit 0
      standStill = (value & MASK1(16, 1, 0)) != 0;
    }
    return res;
  }
  return AxisProfileResult::ERR_NOT_SUPPORTED;
}

AxisProfileResult CanopenDeviceParamDrive::readChecksumValueFromDevice(std::uint32_t& checkSum)
{
  ParamReqDataCoE requestData {m_profileBasicInfo.m_deviceAddress, getObjDir(CHECKSUM_VALUE, m_driveNumber)};
  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, reinterpret_cast<std::uint8_t*>(&checkSum), sizeof(checkSum));
  if (aprStatusFailed(result))
  {
    APR_TRACE_WARNING("%s, get checksum error", m_profileBasicInfo.m_profileTrace.createTraceMsg(result));
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                        __FILE__, __FUNCTION__, __LINE__, "Checksum, CoE DIN: %s, error: 0x%X", objDirToString(requestData.objEntry).c_str(), result);
  }
  return result;
}

AxisProfileResult CanopenDeviceParamDrive::writeChecksumIdnListToDevice(const std::vector<ObjectDirEntry>& objList)
{ 
  std::vector<std::uint8_t> tmpBuff {};
  auto funcListToRaw = [] (const ObjectDirEntry& objEntry, std::uint8_t* rawBuf)
    {
      std::memcpy(rawBuf + 1, &objEntry.subIndex, sizeof(objEntry.subIndex));
      std::memcpy(rawBuf + 2, &objEntry.index, sizeof(objEntry.index));
    };
  AxisProfileResult res = FieldbusHelper::paramListToRaw<ObjectDirEntry>(objList, tmpBuff, funcListToRaw);
  if (aprStatusFailed(res))
  {
    APR_TRACE_WARNING("%s, checksum IDN list parse error", m_profileBasicInfo.m_profileTrace.createTraceMsg(res));
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_PARSE_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Checksum IDN list parse, CoE IDN: %s, error: 0x%X",
                       objDirToString(CHECKSUM_PARAMETERS).c_str(), res);
    return res;
  }

  ParamReqDataCoE requestDataWrite {m_profileBasicInfo.m_deviceAddress, getObjDir(CHECKSUM_PARAMETERS, m_driveNumber),
    tmpBuff.data(), static_cast<std::uint32_t>(tmpBuff.size()), LIST_BUFF_CHECKSUM_PARAM_LIST_SIZE};
  res = m_profileBasicInfo.m_fieldbusAdapter->writeSingleParamSync(requestDataWrite);
  if (aprStatusFailed(res))
  {
    APR_TRACE_WARNING("%s, checksum IDN list set error", m_profileBasicInfo.m_profileTrace.createTraceMsg(res));
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_WRITE_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Checksum IDN list set, CoE IDN: %s, error: 0x%X",
                       objDirToString(requestDataWrite.objEntry).c_str(), res);
  }
  return res;
}

AxisProfileResult CanopenDeviceParamDrive::readChecksumIdnListFromDevice(std::vector<ObjectDirEntry>& objList)
{
  std::size_t readLen {};
  std::array<std::uint8_t, LIST_BUFF_CHECKSUM_PARAM_LIST_SIZE> rawBuf {}; // buffer for raw data reading

  ParamReqDataCoE requestData {m_profileBasicInfo.m_deviceAddress, getObjDir(CHECKSUM_PARAMETERS, m_driveNumber)};
  requestData.maxLen = LIST_BUFF_CHECKSUM_PARAM_LIST_SIZE;
  AxisProfileResult result = m_profileBasicInfo.m_fieldbusAdapter->readSingleParamSync(requestData, rawBuf.data(), rawBuf.size(), readLen);

  if (aprStatusFailed(result))
  {
    APR_TRACE_WARNING("%s, checksum IDN list read error", m_profileBasicInfo.m_profileTrace.createTraceMsg(result));
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_READ_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Checksum parameters list read, CoE IDN: %s, error: 0x%X",
                       objDirToString(requestData.objEntry).c_str(), result);
    return result;
  }

  objList.clear();
  auto funcRawToList = [] (const std::uint8_t* rawBuf, ObjectDirEntry& objEntry) {
      std::memcpy(&objEntry.subIndex, rawBuf + 1, sizeof(objEntry.subIndex));
      std::memcpy(&objEntry.index, rawBuf + 2, sizeof(objEntry.index));
    };

  result = FieldbusHelper::rawToParamList<ObjectDirEntry>(rawBuf.data(), readLen, objList, funcRawToList);
  if (aprStatusFailed(result))
  {
    APR_TRACE_WARNING("%s, checksum IDN list parse error", m_profileBasicInfo.m_profileTrace.createTraceMsg(result));
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_PARSE_PARAM_FAILED, m_profileBasicInfo.m_profileName.c_str(),
                       __FILE__, __FUNCTION__, __LINE__, "Checksum parameters list parse, CoE IDN: %s, error: 0x%X",
                       objDirToString(requestData.objEntry).c_str(), result);
  }
  return result;
}

bool CanopenDeviceParamDrive::checkBootIdnListModified()
{
  return ChecksumIdnListHelper::checkBootIdnListModifiedImpl<std::vector<ObjectDirEntry> >(m_checksumIdnList, COE_BOOT_PARAM_LIST,
    [this] (std::vector<ObjectDirEntry>& list) {
      return this->readChecksumIdnListFromDevice(list);
    },
    [this] (const std::vector<ObjectDirEntry>& list){
      return this->writeChecksumIdnListToDevice(list);
    },
    m_profileBasicInfo.m_deviceAddress
  );  
}

AxisProfileResult CanopenDeviceParamDrive::getChecksumIdnList(std::vector<std::string>& idnStrListRet) const
{
  return ChecksumIdnListHelper::getChecksumIdnListStrImpl<ObjectDirEntry>(m_checksumIdnList, idnStrListRet, [] (const ObjectDirEntry& objDirEntry)
                                                                          {
                                                                            return objDirToString(objDirEntry);
                                                                          });
}

AxisProfileResult CanopenDeviceParamDrive::checkAxisHomed(bool& homed)
{
  AxisProfileResult res {AxisProfileResult::STS_OK};
  if (isHomedInCyclicData())
  {
    homed = m_homed.load();
    return res;
  }

  auto* propertyIf = m_sharingHandler->getPropertyIF();
  auto deviceSeries = propertyIf->getDeviceSeries();
  if (DeviceIdHelper::isRexrothCtrlXDrive(deviceSeries))
  {
    std::uint16_t value {};
    res = deviceParamCoE(m_profileBasicInfo, m_JobObj, ReadWriteType::READ,
                         HOMING_FEEDBACK_VALUE_STATUS, value, sizeof(value));
    if (res == AxisProfileResult::STS_OK)
    {
      // S-0-0403.0 / 0x2508:01 Homed status of one of the encoder (encoder 1 or 2)
      m_homed.store((value & MASK1(16, 1, 0)) != 0);
    }
  }

  // m_homed will be updated when homing command executed successfully.
  homed = m_homed.load();
  return res;
}
