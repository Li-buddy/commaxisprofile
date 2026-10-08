/// @file
///
/// This source file is part of the axisprofile library and implement device information manager.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "common/debug_trace.h"
#include "deviceinfo_manager.h"
#include "diagnosis/dia.h"
#include "fieldbuses/fieldbus_adapter.h"
#include "fieldbuses/fieldbus_defs.h"
#include "fieldbuses/fieldbus_master.h"
#include "flatbuffers/flatbuffers.h"
#include "fieldbuses/fieldbus_helper.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Implementation of classes and structures
// ---------------------------------------------------------------------------

DeviceInfoManager::~DeviceInfoManager()
{
  clearFieldbusContainer();
}

void DeviceInfoManager::clearFieldbusContainer()
{
  for (const auto& it : m_mapFieldbusInstance)
  {
    delete it.second;
  }
  m_mapFieldbusInstance.clear();
}

void DeviceInfoManager::setDataLayer(comm::datalayer::IClient* client)
{
  m_datalayerClient = client;
}

void DeviceInfoManager::setDataLayerFactory(comm::datalayer::IDataLayerFactory* datalayer)
{
  m_datalayer = datalayer;
}

void DeviceInfoManager::setProfiles(std::map<std::string, ProfileInfo>* mapProfiles)
{
  m_mapAxisProfiles = mapProfiles;
}

AxisProfileResult DeviceInfoManager::fillSingleDeviceInfo(const std::string& masterName, const std::uint32_t nAddress)
{
  // inquiry slave information
  std::string urlSlaveCfgInfo = DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + '/' + masterName + DL_SUB_ADDRESS_SLAVE_CFG_INFO;

  comm::datalayer::Variant slaveInfoData {};
  DeviceFbsDataHelper::generateAddressRequest(nAddress, slaveInfoData);

  comm::datalayer::DlResult dlResult = m_datalayerClient->readSync(urlSlaveCfgInfo, &slaveInfoData);
  if (STATUS_FAILED(dlResult))
  {
    // LCOV_EXCL_START
    APR_TRACE_ERROR("Not able to read slaves information with address: %s, error: %s", urlSlaveCfgInfo.c_str(), dlResult.toString());
    return AxisProfileResult::ERR_DATALAYER_READ_NODE;
    // LCOV_EXCL_STOP
  }
  DeviceHardwareInfo devInfo {nAddress};
  devInfo.m_masterName = masterName;
  std::string deviceName {};  
  comm::axisprofile::AxisProfileResult aspResult = DeviceFbsDataHelper::extractSlaveCfgInfo(slaveInfoData, deviceName, devInfo);
    if (AxisProfileResult::STS_OK != aspResult)
  {
    APR_TRACE_ERROR("Failed to extract configured slave name.");
    return aspResult;
  }
  m_mapDeviceTopoInfo.insert({deviceName, devInfo});

  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceInfoManager::fillDeviceInfo(std::string const& masterName)
{
  std::vector<std::uint32_t> addressArray;
  AxisProfileResult axpResult = getSlaveAddress(masterName, addressArray);
  if (aprStatusNotOkay(axpResult))
  {
    return AxisProfileResult::ERR_DATALAYER_READ_NODE;
  }
  uint32_t mainCode {}, detailCode {};
  for (std::uint32_t slaveAddr : addressArray)
  {
    // inquiry single available device info
    axpResult = fillSingleDeviceInfo(masterName, slaveAddr);
    if (aprStatusFailed(axpResult))
    {
      std::string urlSlaveCfgInfo = DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + '/' + masterName + DL_SUB_ADDRESS_SLAVE_CFG_INFO;
      switch (axpResult)
      {
        case AxisProfileResult::ERR_DATALAYER_READ_NODE:
          mainCode = MAIN_DIAG_BOOTING_ERR;
          detailCode = DETAILED_DIAG_READ_DEVICE_CFG_VIA_MASTER_FAILED;
          break;
        case AxisProfileResult::ERR_DATALAYER_WRONG_TYPE:
        default:
          mainCode = MAIN_DIAG_DATALAYER_ERR;
          detailCode = DETAILED_DIAG_INVALID_FLATBUFFER;
          break;
      }
      Dia::sendDiagnosis(mainCode, detailCode,
                         __FILE__, __FUNCTION__, __LINE__,
                         "The URI is %s, device address is %d", urlSlaveCfgInfo.c_str(), slaveAddr);
      APR_TRACE_ERROR("Failed to inquiry SingleDeviceInfo of field Instance %s at address: %u, error: 0x%x", masterName.c_str(), slaveAddr, axpResult);
      return axpResult;
    }
  }
  return axpResult;
}

AxisProfileResult DeviceInfoManager::collectAllDeviceInfo(const bool& abortInitialization)
{
  if (!m_datalayerClient)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  comm::datalayer::Variant data {};
  comm::datalayer::DlResult dlResult;
  std::uint16_t timeout = 10;      // 10 seconds timeout
  std::uint16_t pollingMs = 100;   // 100 MS every polling
  std::uint16_t nTimeout = timeout * 1000 / 100;

  // try to browse master instance, when no usable data(not timeout)
  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_WAIT_FOR_FIELDBUS_INSTANCES, __FILE__, __FUNCTION__, __LINE__, "");

  while (true)
  {
    if (abortInitialization)
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_BOOT_ABORTED,
                         __FILE__, __FUNCTION__, __LINE__, "By Web or System");
      return AxisProfileResult::STS_BOOTING_ABORT;
    }

    // Browse should return DL_INVALID_ADDRESS as long as EthercatMaster boot has not been finished.
    // Afterwards it could return with success but without any instances (empty list). We can not continue here without a valid instance.
    dlResult = m_datalayerClient->browseSync(DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES, &data);
    if ((comm::datalayer::STATUS_FAILED(dlResult)) && (dlResult != comm::datalayer::DlResult::DL_INVALID_ADDRESS))
    {
      APR_TRACE_ERROR("Could not browse %s! DlResult: %s", DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES.c_str(), dlResult.toString());
      return AxisProfileResult::ERR_DATALAYER_BROWSE_NODE;
    }
    nTimeout--;
    if ((dlResult == comm::datalayer::DlResult::DL_OK) || (nTimeout == 0 && data.getCount() == 0))
    {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(pollingMs));
  }
  if (data.getCount() == 0)
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_MASTER_NOT_EXIT, __FILE__, __FUNCTION__, __LINE__,
                       "Master URL: %s, DL_Result: %s", DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES.c_str(), dlResult.toString());
    return AxisProfileResult::ERR_FIELDBUS_NO_MASTER;
  }

  Dia::sendDiagnosis(MAIN_DIAG_BOOTING_MSG, DETAILED_DIAG_BOOT_FIELDBUS_INSTANCES_READY, __FILE__, __FUNCTION__, __LINE__, "");

  // collect all slave device information
  const comm::datalayer::Variant* convData = &data;
  const char** onlineInstances = *convData;
  for (std::uint8_t i = 0; i < data.getCount(); i++)
  {
    linkProfileToFieldbusInstance(onlineInstances[i], nullptr);
    auto res = fillDeviceInfo(onlineInstances[i]);
    if (aprStatusFailed(res))
    {
      return res;
    }
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceInfoManager::checkEtherCATConsistency(const std::vector<ProfileInfo>& vecProfilesInfo)
{  
  if (m_mapFieldbusInstance.empty())
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_MASTER_NOT_EXIT, __FILE__, __FUNCTION__, __LINE__,
                       "Master URL: %s", DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES.c_str());
    return AxisProfileResult::ERR_FIELDBUS_NO_MASTER;
  }
  std::string addressBase = DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + '/';
  std::string cfgedEtherCATName {};

  // compare with each profile
  for (auto const& profile : vecProfilesInfo)
  {
    IAxisProfileInternal* internalProfile = static_cast<IAxisProfileInternal*>(profile.m_axisProfile);
    for (auto it = m_mapFieldbusInstance.begin(); it != m_mapFieldbusInstance.end(); ++it)
    {
      const auto& instance = *it;
      std::string address = addressBase + instance.first;
      std::string addressSlash = address + "/";
      // using addressSlash avoid the mismatch of the string contained case
      // for example: ethercatmaster and ethercatmaster_1
      if (profile.m_outputBufferUri.find(addressSlash)!= std::string::npos)
      {
        // the linkProfileToFieldbusInstance method will guarantee no duplicated instance
        cfgedEtherCATName = FieldbusHelper::getEtherCATNameOfURI(profile.m_outputBufferUri.c_str());
        linkProfileToFieldbusInstance(cfgedEtherCATName, internalProfile);
        // assign address to fieldbus adapter
        internalProfile->getFieldbusAdapter()->setFieldbusMasterAddress(address);
        APR_TRACE_MESSAGE("Configure fieldbus instance '%s' for profile '%s'", address.c_str(), internalProfile->getProfileName());
        // once match, finish current profile handle for better performance
        break;
      }
      else
      {
        // if a profile hold a EtherCAT name that not match with all instance -> configuration error.
        if (std::next(it) == m_mapFieldbusInstance.end())
        {
          // if profile's master name not included in the masters container, then error.
          Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_MASTER_NOT_EXIT, __FILE__, __FUNCTION__, __LINE__,
                             "%s: please check if the EtherCAT instance : %s exist",
                             internalProfile->getProfileName(),
                             cfgedEtherCATName.c_str());
          return AxisProfileResult::ERR_PROFILE_CONFIG;
        }
      }
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceInfoManager::updateProfileAddress()
{
  if (!m_mapAxisProfiles || m_mapAxisProfiles->empty())
  {
    return AxisProfileResult::STS_OK;
  }

  // if no physic device existed, no need to check 
  if (m_mapDeviceTopoInfo.empty())
  {
    return AxisProfileResult::STS_OK;
  }

  for (const auto& it : m_mapDeviceTopoInfo)
  {
    APR_TRACE_MESSAGE("Available device info: device name: %s, address: %u", it.first.c_str(), it.second.m_address);
  }

  // check if the device exist, update the address
  std::vector<DeviceInfo> vecAllCfgedDeviceInfo {};
  for (const auto& profile : *m_mapAxisProfiles)
  {
    // escape inactive profile
    IAxisProfileInternal* pAxisprofile = profile.second.m_axisProfile;
    if (!pAxisprofile || !pAxisprofile->checkProfileActivated())
    {
      continue;
    }

    IAxisProfileProperty* propertyInterface = pAxisprofile->getPropertyInterface();
 
    // if the device name is empty, the solution is earlier version before 22.11
    handleLegacyDeviceName(propertyInterface, m_mapDeviceTopoInfo, pAxisprofile->getProfileName());

    auto it = m_mapDeviceTopoInfo.find(propertyInterface->getDeviceName());
    if (it == m_mapDeviceTopoInfo.end())
    {
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_DEVICENAME,
                         __FILE__, __FUNCTION__, __LINE__, "%s",
                         propertyInterface->getDeviceName());
      APR_TRACE_ERROR("The device with name %s does not exist, please check device configuration", propertyInterface->getDeviceName());
      return AxisProfileResult::ERR_NOK;
    }

    const auto& deviceInfo = it->second;
    propertyInterface->setDeviceIdentification(deviceInfo.m_vendorId, deviceInfo.m_productCode);
    if (propertyInterface->getDeviceSeries() == DeviceSeries::NOTINDICATES)
    {
      Dia::sendDiagnosis(MAIN_DIAG_BOOTING_WRN, DETAILED_DIAG_PARSE_VENDOR_ID_FAILED,
                         __FILE__, __FUNCTION__, __LINE__, "%s vendorId parse failed. VendorId: 0x%X, product code: 0x%X",
                         propertyInterface->getDeviceName(), deviceInfo.m_vendorId, deviceInfo.m_productCode);
      APR_TRACE_WARNING("%s vendorId or product code parse failed. VendorId: 0x%X, product code: 0x%X",
                        propertyInterface->getDeviceName(), deviceInfo.m_vendorId, deviceInfo.m_productCode);
    }

    // if device name not empty use the device name information
    // update address via name
    if (propertyInterface->getDeviceAddress() != it->second.m_address)
    {
      propertyInterface->setDeviceAddress(it->second.m_address);
      APR_TRACE_MESSAGE("Profile %s, device address of %s update to %d successfully!", pAxisprofile->getProfileName(), it->first.c_str(), it->second.m_address);
    }
    vecAllCfgedDeviceInfo.emplace_back(
      DeviceInfo {propertyInterface->getDeviceName(), propertyInterface->getDeviceAddress(), pAxisprofile->getDriveNumber(), propertyInterface->getProfileCategory()});   
  }

  return checkDuplicateDevices(vecAllCfgedDeviceInfo);
}

AxisProfileResult DeviceInfoManager::checkDuplicateDevices(const std::vector<DeviceInfo>& vecAllCfgedDeviceInfo)
{
  // Follow special cases are accepted:
  // 1. one drive links to 1~4 drive-profiles(CoE standard), with different drive number
  // 2. one drive links to both drive profile and encoder profile same time, with different category
  for (const auto& current : vecAllCfgedDeviceInfo)
  {
    auto cnt = std::count_if(vecAllCfgedDeviceInfo.cbegin(), vecAllCfgedDeviceInfo.cend(), [&current] (auto item) {
                                return (current.m_name == item.m_name &&
                                        current.m_address == item.m_address &&
                                        current.m_driveNum == item.m_driveNum &&
                                        current.m_category == item.m_category);
                             });

    if (cnt > 1)
    {
      APR_TRACE_ERROR("Duplicate detected for device: %s", current.m_name.c_str());
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_INVALID_DEVICE_ADR, __FILE__, __FUNCTION__, __LINE__,
                         "Conflict name %s and address %d", current.m_name.c_str(), current.m_address);
      return AxisProfileResult::ERR_PROFILE_CONFIG;
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult DeviceInfoManager::validateProtocolInfo()
{
  if (m_mapDeviceTopoInfo.empty() || m_mapAxisProfiles->empty())
  {
    return AxisProfileResult::STS_OK;
  }

  // traverse all profile, check protocol type  
  IAxisProfileInternal* pAxisprofile {};
  AxisProfileType profileType {};
  DriveProfileType driveProtocolType {};
  for (const auto& profile : *m_mapAxisProfiles)
  {
    //if the device name is empty it's earlier version before 22.11
    //TODO: the key of m_mapDeviceTopoInfo could not be the deviceName, because the deviceName was added after RM22.11
    //When restoring json configuration which is older than RM22.11, the deviceName field is empty.
    pAxisprofile = profile.second.m_axisProfile;
    if (!pAxisprofile || !pAxisprofile->checkProfileActivated() || strlen(pAxisprofile->getPropertyInterface()->getDeviceName()) == 0)
    {
      continue;
    }
    auto deviceName = pAxisprofile->getPropertyInterface()->getDeviceName();
    auto& deviceProtocol = m_mapDeviceTopoInfo[deviceName].m_supportedProtocol;
    bool isMatched {false};
    profileType = pAxisprofile->getPropertyInterface()->getProfileType();
    switch (profileType)
    {
      case comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
        isMatched = deviceProtocol & (1 << PROTOCOL_MASK_SOE);
        // for trace
        driveProtocolType = isMatched ? DriveProfileType::SERCOS : DriveProfileType::CANOPEN;
        break;
      case comm::axisprofile::AxisProfileType::CANOPEN_OVER_ETHERCAT:
        isMatched = deviceProtocol & (1 << PROTOCOL_MASK_COE);
        // for trace
        driveProtocolType = isMatched ? DriveProfileType::CANOPEN : DriveProfileType::SERCOS;
        break;
      default:
        // LCOV_EXCL_START can't happen
        isMatched = false;
        driveProtocolType = DriveProfileType::UNDEFINED;
        // LCOV_EXCL_STOP
        break;
    }

    // send diagnosis, abort booting when first error was found
    if (!isMatched)
    {
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_INCONSISTENT_PROTOCOL, __FILE__, __FUNCTION__, __LINE__,
                         "profile '%s' is '%s', device '%s' is '%s'.",
                         pAxisprofile->getProfileName(), axisProfileTypeToString(profileType),
                         pAxisprofile->getPropertyInterface()->getDeviceName(), driveProfileTypeToString(driveProtocolType));
      APR_TRACE_ERROR("The protocol type in profile '%s' is configured as '%s', but the corresponding device '%s' is in '%s' protocol.",
                      pAxisprofile->getProfileName(), axisProfileTypeToString(profileType),
                      pAxisprofile->getPropertyInterface()->getDeviceName(), driveProfileTypeToString(driveProtocolType));
      return AxisProfileResult::ERR_NOK;
    }
  }

  return AxisProfileResult::STS_OK;
}

bool DeviceInfoManager::linkProfileToFieldbusInstance(const std::string& instName, IAxisProfileInternal* profile)
{
  if (!instName.empty())
  {
    auto master = findAndCreateFieldbusMaster(instName);
    if (master && profile != nullptr)
    {
      master->addLinkedProfile(profile);
      return true;
    }
  }
  return false;
}

IFieldbusMaster* DeviceInfoManager::findAndCreateFieldbusMaster(const std::string& instName)
{
  if (instName.empty())
  {
    return nullptr;
  }

  if (m_mapFieldbusInstance.find(instName) == m_mapFieldbusInstance.end())
  {
    m_mapFieldbusInstance[instName] = new FieldbusMaster(instName.c_str(), m_datalayer);
  }
  return m_mapFieldbusInstance[instName];
}

AxisProfileResult DeviceInfoManager::getSlaveAddress(std::string const& strFieldbusInstance, std::vector<std::uint32_t>& addressArray)
{
  AxisProfileResult aprResult {AxisProfileResult::STS_OK};
  std::string addressString = DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES + '/' + strFieldbusInstance + DL_SUB_ADDRESS_ADDRESS_MAP;
  comm::datalayer::Variant variantMapping;
  DeviceFbsDataHelper::generateSlaveAddressMappingRequestFlatbuffer(variantMapping);
  
  comm::datalayer::DlResult dlResult = m_datalayerClient->readSync(addressString.c_str(), &variantMapping);
  if (comm::datalayer::STATUS_FAILED(dlResult))
  {
    APR_TRACE_ERROR("Not able to read slaves address mapping with address: %s, error: %s", addressString.c_str(), dlResult.toString());
    Dia::sendDiagnosis(MAIN_DIAG_BOOTING_ERR, DETAILED_DIAG_READ_DEVICE_CFG_VIA_MASTER_FAILED,
                       __FILE__, __FUNCTION__, __LINE__,
                       "The URI is %s, datalayer error: %s", addressString.c_str(), dlResult.toString());
    return AxisProfileResult::ERR_DATALAYER_READ_NODE;
  }

  aprResult = DeviceFbsDataHelper::extractSlaveAddressByMapping(variantMapping, addressArray);
  if (aprStatusFailed(aprResult))
  {
    APR_TRACE_ERROR("Failed to extract configured slave address mapping of field Instance %s .", strFieldbusInstance.c_str());
  }
  APR_TRACE_MESSAGE("configured slave number of fieldbus instance %s is %d.", strFieldbusInstance.c_str(), addressArray.size());
  return aprResult;
}

void DeviceInfoManager::handleLegacyDeviceName(IAxisProfileProperty* propertyInterface, const std::map<std::string, DeviceHardwareInfo>& hardwareInfoMap, const std::string& profileName)
{
  // if the device name is empty, the solution is earlier version before 22.11
  if (strlen(propertyInterface->getDeviceName()) == 0)
  {
    std::uint32_t address = propertyInterface->getDeviceAddress();
    auto itor = std::find_if(hardwareInfoMap.begin(), hardwareInfoMap.end(), [address] (const std::pair<std::string, DeviceHardwareInfo>& pair) {
                    return pair.second.m_address == address;
                  });
    if (itor != hardwareInfoMap.end())
    {
      APR_TRACE_MESSAGE("profile %s with address %d, update the device name to %s", profileName.c_str(), address, itor->first.c_str());
      propertyInterface->setDeviceName(itor->first.c_str());
    }
  }
}
