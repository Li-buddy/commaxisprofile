#pragma once
/// @file  deviceinfo_manager.h
///
/// This header file is part of the axisprofile library and define device information manager.
///
/// @copyright (C) 2018-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "helper/device_fbs_data_helper.h"
#include "comm/datalayer/variant.h"
#include "profiles/i_axisprofile_internal.h"
#include "manager/i_axisprofile_manager_internal.h"

#include <string>
#include <map>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestDeviceInfoMgr;

namespace comm
{
  namespace datalayer
  {
    class IClient;
  }
}

namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------
    
    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------
    /// @brief The mail box protocol information can be found in this link: 
    /// https://inside-docupedia.bosch.com/confluence/pages/viewpage.action?pageId=1529033893
    constexpr uint32_t PROTOCOL_MASK_COE = 2;    ///< the mail box protocol CoE flag in bit2 
    constexpr uint32_t PROTOCOL_MASK_SOE = 4;    ///< the mail box protocol SoE flag in bit4

    // ---------------------------------------------------------------------------
    // Declaration of macros
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------
    /// @brief Type of profile information
    struct ProfileInfo
    {
      IAxisProfileInternal* m_axisProfile{};                      ///< Profile pointer for internal usage
      std::shared_ptr<const IAxisProfileC> m_axisProfileShared;   ///< Shared pointer for external usage
      std::string m_inputBufferUri{};                             ///< uri of the input buffer for this profile
      std::string m_outputBufferUri{};                            ///< uri of the output buffer for this profile
    };

    /// @brief Type of device information related to the axis profile
    struct DeviceInfo
    {
      std::string         m_name;     //device name
      std::uint32_t       m_address{};  //device address
      std::uint8_t        m_driveNum{}; //drive number of the device
      AxisProfileCategory m_category{}; //drive category of the device
    };

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    /// @brief This class used to extend the operation for slave device in axisprofile.
    class DeviceInfoManager
    {
    public: //method

      DeviceInfoManager() = default;
      virtual ~DeviceInfoManager() noexcept;

      /// @brief This method returns the class name.
      /// @return   Return class name.
      static const char* getClassName() { return ( CLASS_NAME(DeviceInfoManager) ); }

      /// @brief This method clear fieldbus profiles information map.
      /// @return   Return class name.
      void clearFieldbusContainer();

      /// @brief This method clear device information map.
      /// @return   Return class name.
      void clearDeviceInfoContainer() { m_mapDeviceTopoInfo.clear(); }

      /// @brief  This method set datalayer client for object.
      /// @param[in]    client      The datalayer for current object
      void setDataLayer(comm::datalayer::IClient* client);

      /// @brief  This method set datalayer client for object.
      /// @param[in]    datalayer   The datalayer for current object
      void setDataLayerFactory(comm::datalayer::IDataLayerFactory* datalayer);

      /// @brief  This method set list of profile information, key is profile name
      /// @param[in]    mapProfiles   The map contain profiles.
      void setProfiles(std::map<std::string, ProfileInfo>* mapProfiles);
      
      /// @brief  This method will update device address according to device name, when device name not empty.
      /// @return Return value indicating success or error.
      AxisProfileResult updateProfileAddress();

      /// @brief This method add a linking pair of master and profile, master object is created if not exist
      /// @param[in] instName    Instance name of the fieldbus master
      /// @param[in] profile     Pointer to profile instance link against to the master instance
      /// @return true for adding success
      bool linkProfileToFieldbusInstance(const std::string& instName, IAxisProfileInternal* profile);

      /// @brief This method get the map of fieldbus master name against fieldbus master object
      /// @return Return map of fieldbus master name and object
      const ContainerFieldbusInst& getFieldbusMasters() { return m_mapFieldbusInstance;}

      /// @brief This function will validate protocol information between device and profile
      /// @return Return value indicating success (>=0) or error (<0).
      AxisProfileResult validateProtocolInfo();
      
      /// @brief This method will collect necessary device information for EtherCAT data layer node.
      /// @param[in] abortInitialization Whether abort the booting method.
      /// @return    Return value indicating success (>=0) or error (<0).
      AxisProfileResult collectAllDeviceInfo(const bool& abortInitialization);

      /// @brief Validate if the EtherCAT name of profile consistent with online master name.
      /// @param[in] vecProfiles  The profiles need to validated.
      /// @return Return value indicating success or error.
      AxisProfileResult checkEtherCATConsistency(const std::vector<ProfileInfo>& vecProfilesInfo);

    private: //method

      /// @brief This method will fill single device information to container.
      /// @param[in] urlSlaveCfgInfo    The url of salve config information.
      /// @param[in] nAddress           The device address.  
      /// @return     Return value indicating success or error.
      AxisProfileResult fillSingleDeviceInfo(const std::string& urlSlaveCfgInfo, const std::uint32_t nAddress);      

      /// @brief This method will fill the map container with device information.
      /// @param[in]  masterName              The fieldbus instance name for corresponding device.
      /// @return     Return value indicating success or error.
      AxisProfileResult fillDeviceInfo(std::string const& masterName);

      /// @brief This method create a fieldbus master instance name and its linked profile.
      /// @param[in] masterName  Instance name of the fieldbus master
      /// @return true for adding success
      IFieldbusMaster* findAndCreateFieldbusMaster(const std::string& masterName);

      /// @brief  This method check whether has the duplicated device in list.
      /// @param[in]  vecAllCfgedDeviceInfo   device information.
      /// @return     Return value indicating success or error.
      static AxisProfileResult checkDuplicateDevices(const std::vector<DeviceInfo>& vecAllCfgedDeviceInfo);

      /// @brief  This method get the salve address by the salve address mapping of DL.
      /// @param[in]   strFieldbusInstance     The fieldbus instance name for corresponding device.
      /// @param[out]  addressArray   the address of slaves.
      /// @return Return value indicating success or error.
      AxisProfileResult getSlaveAddress(std::string const& strFieldbusInstance, std::vector<std::uint32_t>& addressArray);

      /// @brief  This method process the version compatibility with empty device name.
      /// @param[in]  propertyInterface     interface of property.
      /// @param[in]  hardwareInfoMap       map of device name and device hardware info
      /// @param[in]  profileName           the profile's name
      static void handleLegacyDeviceName(IAxisProfileProperty* propertyInterface, const std::map<std::string, DeviceHardwareInfo>& hardwareInfoMap, const std::string& profileName);

    private: //data
      friend class ::TestDeviceInfoMgr;                                     ///< for unit test

      ContainerFieldbusInst               m_mapFieldbusInstance {};       ///< map of fieldbus masters
      std::map<std::string, DeviceHardwareInfo> m_mapDeviceTopoInfo {};   ///< map of device name and address
      std::map<std::string, ProfileInfo>* m_mapAxisProfiles = nullptr;    ///< map of axisprofile name and information
      comm::datalayer::IClient*           m_datalayerClient = nullptr;    ///< Datalayer client
      comm::datalayer::IDataLayerFactory* m_datalayer = nullptr;          ///< Pointer to datalayer factory
    };
  }
}
