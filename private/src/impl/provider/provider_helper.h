#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements the a helper
/// class for datalayer provider functionalities. It contains the 'NodeTree'
/// of the axisprofile, which represents every string constant used in all
/// available axisprofile datalayer addresses.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "../src/diagnosis/axisprofile_detailed_diag.h"
#include "../src/diagnosis/axisprofile_main_diag.h"
#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/variant.h"
#include "nodes/i_node_base.h"
#include <string>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
{
struct MetaDataInfo;
}

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
{
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @struct AxisProfileNodeTree
  /// @brief  Holds the string references of all data layer addresses in the axisprofile data layer provider.
  ///
  /// The tree is grouped into different sections which represent the main nodes:
  ///
  /// |------ Section Name ------|------------------------------- Description -------------------------------|
  /// | axisprofile/             | Root node. Also general/common defines are placed here.                   |
  /// | axisprofile/admin        | Section for administrator related nodes                                    |
  /// | axisprofile/system       | Section for all system related nodes.                                     |
  /// | axisprofile/profiles     | Section for all profile related nodes.                                    |
  /// |------------------------------------------------------------------------------------------------------|
  const struct AxisProfileNodeTree
  {
    // --------------------------------------
    // ----------   axisprofile/  -----------
    // --------------------------------------
    const std::string WILDCARD           = "/*";                   ///< wildcard, e.g. for the profile name
    const std::string AXISPROFILE        = "axisprofile";          ///< base node
    const std::string SYSTEM             = "/system";              ///< sub node for system
    const std::string OPSTATE            = "/opstate";             ///< sub node for the operation state
    const std::string CMD                = "/cmd";                 ///< sub node for commands
    const std::string ACTUAL             = "/actual";              ///< sub node for actual values
    const std::string PROFILES           = "/profiles";            ///< sub node for profiles

    const std::string CONFIG             = "/config";              ///< sub node for configuration
    const std::string CFG                = "/cfg";                 ///< sub node for cfg
    const std::string INBUFFER           = "/input-buffer";        ///< sub node for the URI of the input buffer (telegram) of a profile
    const std::string OUTBUFFER          = "/output-buffer";       ///< sub node for the URI of the output buffer (telegram) of a profile
    const std::string INMAPPING          = "/input-mapping";       ///< sub node for the input value mapping
    const std::string OUTMAPPING         = "/output-mapping";      ///< sub node for the output value mapping
    const std::string STATUSWORD         = "/statusword";          ///< sub node for the statusword
    const std::string CONTROLWORD        = "/controlword";         ///< sub node for the controlword
    const std::string POSITION           = "/position";            ///< sub node for position values
    const std::string VELOCITY           = "/velocity";            ///< sub node for velocity values
    const std::string TORQUEFORCE        = "/torque-force";        ///< sub node for torque force values
    const std::string DIAGMSG            = "/diagcode";            ///< sub node for diagnostic message      
    const std::string SWITCHREF          = "/switch-ref";          ///< sub node for reference switch signal
    const std::string SWITCHPOSLIM       = "/switch-poslim";       ///< sub node for positive limit switch signal
    const std::string SWITCHNEGLIM       = "/switch-neglim";       ///< sub node for negative limit switch signal
    const std::string PROBE1POSEDG       = "/probe1-posedg";       ///< sub node for positive edge value of probe 1
    const std::string PROBE1NEGEDG       = "/probe1-negedg";       ///< sub node for negative edge value of probe 1
    const std::string PROBE1POSDIF       = "/probe1-posdif";       ///< sub node for position difference between edges of probe 1
    const std::string PROBE1ACTIVE       = "/probe1-active";       ///< sub node for if probe 1 activated
    const std::string PROBE1POSLAT       = "/probe1-poslat";       ///< sub node for positive edge latched times of probe 1
    const std::string PROBE1NEGLAT       = "/probe1-neglat";       ///< sub node for negative edge latched times of probe 1
    const std::string PROBE1MAKFAL       = "/probe1-makfal";       ///< sub node for marker failure counter of probe 1
    const std::string PROBE2POSEDG       = "/probe2-posedg";       ///< sub node for positive edge value of probe 2
    const std::string PROBE2NEGEDG       = "/probe2-negedg";       ///< sub node for negative edge value of probe 2
    const std::string PROBE2POSDIF       = "/probe2-posdif";       ///< sub node for position difference of edges of probe 2
    const std::string PROBE2ACTIVE       = "/probe2-active";       ///< sub node for if probe 2 activated
    const std::string PROBE2POSLAT       = "/probe2-poslat";       ///< sub node for positive edge latched times of probe 2
    const std::string PROBE2NEGLAT       = "/probe2-neglat";       ///< sub node for negative edge latched times of probe 2
    const std::string PROBE2MAKFAL       = "/probe2-makfal";       ///< sub node for marker failure counter of probe 2
    const std::string SAVE               = "/save";                ///< sub node for saving data
    const std::string LOAD               = "/load";                ///< sub node for loading data
    const std::string DEVICEADDRESS      = "/device-address";      ///< sub node for the device address
    const std::string DEVICENAME         = "/device-name";         ///< sub node for the device name
    const std::string BASICDATA          = "/profileBasicData";    ///< sub node for the profile basic data
    const std::string CATEGORY           = "/profile-category";    ///< sub node for the profile category
    const std::string SCALINGINFO        = "/scaling-information"; ///< sub node for profile scaling strategy cfg
    const std::string ScalingStrategy    = "/scaling-strategy";    ///< sub node for profile scaling strategy
    const std::string DEVICESCALINGTYPE  = "/device-scaling-type"; ///< sub node for device scaling type
    const std::string SCALINGNUMERATOR   = "/scaling-numerator";   ///< sub node for profile scaling numerator
    const std::string SCALINGDENOMINATOR = "/scaling-denominator"; ///< sub node for profile scaling denominator
    const std::string SCALINGRESOLUTION  = "/resolution";          ///< sub node for profile scaling resolution
    const std::string VELOCITYFACTOR     = "/velocity-factor";     ///< sub node for profile scaling resolution
    const std::string BOOTING            = "/boot-state";          ///< sub node for the boot state
    const std::string ADMIN              = "/admin";               ///< sub node for admin configuration items
    const std::string BOOTTIMEOUT        = "/boot-timeout";        ///< sub node for max booting timeout
    const std::string DRIVENUMBER        = "/drive-number";        ///< sub node for the drive number
    const std::string STATE              = "/state";               ///< sub node for state
    const std::string DEVICEOFFSET       = "/device-pos-offset";   ///< sub node for device offset
    const std::string MAKEPERSISTENT     = "/make-persistent";     ///< sub node for make persistent
    const std::string ACTIVATED          = "/activated";           ///< sub node for 
    const std::string DEVICE_APP_DATA    = "/device-app-data";     ///< sub node for device-app-data
    const std::string USE_CHECKSUM       = "/use-checksum";        ///< sub node for use-checksum
    const std::string CHECKSUM_VALUE     = "/checksum-value";      ///< sub node for checksum-value
    const std::string CHECKSUM_IDN_LIST  = "/checksum-idn-list";   ///< sub node for checksum-idn-list
    const std::string OUTOFRANGETIMES    = "/data-out-of-range-times";   ///< sub node for data-out-of-range_times
    const std::string MAXALLOWEDERROR    = "/max-consecutive-out-of-range-count";   ///< sub node for max-consecutive-out-of-range-count

    // -----------------------------------------
    // ---------   axisprofile/admin   ---------
    // -----------------------------------------
    const std::string AXISPROFILE_ADMIN                           = AXISPROFILE + ADMIN;                                    ///< axisprofile/admin
    const std::string AXISPROFILE_ADMIN_CFG_SAVE                  = AXISPROFILE_ADMIN + CFG + SAVE;                         ///< axisprofile/admin/cfg/save
    const std::string AXISPROFILE_ADMIN_CFG_LOAD                  = AXISPROFILE_ADMIN + CFG + LOAD;                         ///< axisprofile/admin/cfg/load
    const std::string AXISPROFILE_ADMIN_CFG_MAKEPERSISTENT        = AXISPROFILE_ADMIN + CFG + MAKEPERSISTENT;               ///< axisprofile/admin/cfg/make-persistent

    // --------------------------------------------
    // ----------   axisprofile/system  -----------
    // --------------------------------------------

    const std::string APR_SYSTEM                                  = AXISPROFILE + SYSTEM;                                   ///< axisprofile/system
    const std::string APR_SYSTEM_CONFIG                           = APR_SYSTEM + CONFIG;                                    ///< axisprofile/system/config
    const std::string APR_SYSTEM_CONFIG_SAVE                      = APR_SYSTEM_CONFIG + SAVE;                               ///< axisprofile/system/config/save
    const std::string APR_SYSTEM_CONFIG_LOAD                      = APR_SYSTEM_CONFIG + LOAD;                               ///< axisprofile/system/config/load
    const std::string APR_SYSTEM_OPSTATE                          = APR_SYSTEM + OPSTATE;                                   ///< axisprofile/system/opstate
    const std::string APR_SYSTEM_OPSTATE_CMD                      = APR_SYSTEM_OPSTATE + CMD;                               ///< axisprofile/system/opstate/cmd
    const std::string APR_SYSTEM_OPSTATE_ACTUAL                   = APR_SYSTEM_OPSTATE + ACTUAL;                            ///< axisprofile/system/opstate/actual
    const std::string APR_SYSTEM_BOOT_TIMEOUT                     = APR_SYSTEM + BOOTTIMEOUT;                               ///< axisprofile/system/boot-timeout
    const std::string APR_SYSTEM_BOOT_STATE                       = APR_SYSTEM + BOOTING;                                   ///< axisprofile/system/boot-state
    const std::string APR_SYSTEM_USE_CHECKSUM                     = APR_SYSTEM + USE_CHECKSUM;                              ///< axisprofile/system/use-checksum

    // ----------------------------------------------
    // ----------   axisprofile/profiles  -----------
    // ----------------------------------------------

    const std::string APR_PROFILES                                = AXISPROFILE + PROFILES;                                 ///< axisprofile/profiles
    const std::string APR_PROFILES_WC                             = APR_PROFILES + WILDCARD;                                ///< axisprofile/profiles/[name]
        
    //subnode under cfg node
    const std::string APR_PROFILES_CFG                            = APR_PROFILES_WC + CFG;                                  ///< axisprofile/profiles/[name]/cfg
    const std::string APR_PROFILES_CFG_MAX_CONSECUTIVE_OOR_CNT    = APR_PROFILES_CFG+ MAXALLOWEDERROR;                      ///< axisprofile/profiles/[name]/cfg/max-consecutive-out-of-range-count
    const std::string APR_PROFILES_CFG_INBUFFER                   = APR_PROFILES_CFG + INBUFFER;                            ///< axisprofile/profiles/[name]/cfg/in-buffer-uri
    const std::string APR_PROFILES_CFG_OUTBUFFER                  = APR_PROFILES_CFG + OUTBUFFER;                           ///< axisprofile/profiles/[name]/cfg/out-buffer-uri 

    const std::string APR_PROFILES_CFG_SAVE                       = APR_PROFILES_CFG + SAVE;                                ///< axisprofile/profiles/[name]/cfg/save
    const std::string APR_PROFILES_CFG_LOAD                       = APR_PROFILES_CFG + LOAD;                                ///< axisprofile/profiles/[name]/cfg/load 

    const std::string APR_PROFILES_CFG_INMAPPING                  = APR_PROFILES_CFG + INMAPPING;                           ///< axisprofile/profiles/[name]/cfg/input-mapping
    const std::string APR_PROFILES_CFG_INMAPPING_MODE             = APR_PROFILES_CFG_INMAPPING + WILDCARD;                  ///< axisprofile/profiles/[name]/cfg/input-mapping/*

    const std::string APR_PROFILES_CFG_OUTMAPPING                 = APR_PROFILES_CFG + OUTMAPPING;                          ///< axisprofile/profiles/[name]/cfg/output-mapping
    const std::string APR_PROFILES_CFG_OUTMAPPING_MODE            = APR_PROFILES_CFG_OUTMAPPING + WILDCARD;                 ///< axisprofile/profiles/[name]/cfg/output-mapping/*

    const std::string APR_PROFILES_CFG_DEVICEADDRESS              = APR_PROFILES_CFG + DEVICEADDRESS;                       ///< axisprofile/profiles/[name]/cfg/device-address
    const std::string APR_PROFILES_CFG_DEVICENAME                 = APR_PROFILES_CFG + DEVICENAME;                          ///< axisprofile/profiles/[name]/cfg/device-name
    const std::string APR_PROFILES_CFG_BASICDATA                  = APR_PROFILES_CFG + BASICDATA;                           ///< axisprofile/profiles/[name]/cfg/profile basic data
    const std::string APR_PROFILES_CFG_DRIVENUMBER                = APR_PROFILES_CFG + DRIVENUMBER;                         ///< axisprofile/profiles/[name]/cfg/drive-number
    const std::string APR_PROFILES_CFG_CATEGORY                   = APR_PROFILES_CFG + CATEGORY;                            ///< axisprofile/profiles/[name]/cfg/profile category
    const std::string APR_PROFILES_CFG_USE_CHECKSUM               = APR_PROFILES_CFG + DEVICE_APP_DATA + USE_CHECKSUM;      ///< axisprofile/profiles/[name]/cfg/device-app-data/use-checksum

    const std::string APR_PROFILES_CFG_SCALINGINFO                = APR_PROFILES_CFG + SCALINGINFO;                         ///< axisprofile/profiles/[name]/cfg/profile-scaling-strategy-configuration
    const std::string APR_PROFILES_CFG_SCALINGINFO_STRATEGY       = APR_PROFILES_CFG_SCALINGINFO + ScalingStrategy;         ///< axisprofile/profiles/[name]/cfg/profile-scaling-strategy
    const std::string APR_PROFILES_CFG_SCALINGINFO_TYPE           = APR_PROFILES_CFG_SCALINGINFO + DEVICESCALINGTYPE;       ///< axisprofile/profiles/[name]/cfg/device-scaling-type
    const std::string APR_PROFILES_CFG_SCALINGINFO_NUMERATOR      = APR_PROFILES_CFG_SCALINGINFO + SCALINGNUMERATOR;        ///< axisprofile/profiles/[name]/cfg/profile-scaling-numerator
    const std::string APR_PROFILES_CFG_SCALINGINFO_DENOMINATOR    = APR_PROFILES_CFG_SCALINGINFO + SCALINGDENOMINATOR;      ///< axisprofile/profiles/[name]/cfg/profile-scaling-denominator
    const std::string APR_PROFILES_CFG_SCALINGINFO_RESOLUTION     = APR_PROFILES_CFG_SCALINGINFO + SCALINGRESOLUTION;       ///< axisprofile/profiles/[name]/cfg/profile-encoder-resolution
    const std::string APR_PROFILES_CFG_SCALINGINFO_VELOCITYFACTOR = APR_PROFILES_CFG_SCALINGINFO + VELOCITYFACTOR;          ///< axisprofile/profiles/[name]/cfg/velocity-factor
    

    // -----------------------------------------------------
    // ----------   axisprofile/profiles/state/  -----------
    // -----------------------------------------------------
    const std::string APR_PROFILES_STATE                          = APR_PROFILES_WC + STATE;                                 ///< axisprofile/profiles/[name]/state  
    const std::string APR_PROFILES_DEVICEOFFSET                   = APR_PROFILES_STATE + DEVICEOFFSET;                       ///< axisprofile/profiles/[name]/state/device-pos-offset
    const std::string APR_PROFILES_ACTIVATED                      = APR_PROFILES_STATE + ACTIVATED;                          ///< axisprofile/profiles/[name]/state/activated
    const std::string APR_PROFILES_CHECKSUM_VALUE                 = APR_PROFILES_STATE + DEVICE_APP_DATA + CHECKSUM_VALUE;   ///< axisprofile/profiles/[name]/state/device-app-data/checksum-value
    const std::string APR_PROFILES_CHECKSUM_IDN_LIST              = APR_PROFILES_STATE + DEVICE_APP_DATA + CHECKSUM_IDN_LIST;///< axisprofile/profiles/[name]/state/device-app-data/checksum-idn-list
    const std::string APR_PROFILES_DATA_OUT_RANGE_TIMES           = APR_PROFILES_STATE + OUTOFRANGETIMES;                    ///< axisprofile/profiles/[name]/state/data-out-of-range-times
    const std::string APR_PROFILES_DATA_OUT_RANGE_TIMES_POS       = APR_PROFILES_DATA_OUT_RANGE_TIMES + POSITION;            ///< axisprofile/profiles/[name]/state/data-out-of-range-times/position
    const std::string APR_PROFILES_DATA_OUT_RANGE_TIMES_VEL       = APR_PROFILES_DATA_OUT_RANGE_TIMES + VELOCITY;            ///< axisprofile/profiles/[name]/state/data-out-of-range-times/velocity
    const std::string APR_PROFILES_DATA_OUT_RANGE_TIMES_TOR       = APR_PROFILES_DATA_OUT_RANGE_TIMES + TORQUEFORCE;         ///< axisprofile/profiles/[name]/state/data-out-of-range-times/torque-force
  } NodeTree; ///< the node tree that stores all strings as data layer addresses

  ///< @brief Actions, that are allowed in nodes
  namespace node_actions
  {
    const std::uint32_t NONE = 0;          ///< no action allowed
    const std::uint32_t READ = 1 << 0;     ///< you may read this node
    const std::uint32_t WRITE = 1 << 1;    ///< you may write on this node
    const std::uint32_t CREATE = 1 << 2;   ///< you may create this node
    const std::uint32_t REMOVE = 1 << 3;   ///< you may remove this node
  }

  /// @class ProviderHelper
  /// @brief This is the helper class for the datalayer provider.
  class ProviderHelper
  {
  public: // types
    /// @brief Actions, that are allowed in nodes
    enum class NodeActions : std::uint32_t
    {
      NONE = 0,         ///< no action allowed
      READ = 1 << 0,    ///< you may read this node
      WRITE = 1 << 1,   ///< you may write on this node
      CREATE = 1 << 2   ///< you may create this node
    };

  public: // methods
    /// Constructor
    ProviderHelper() = default;
    /// Destructor
    virtual ~ProviderHelper() noexcept = default;

    /// @brief Helper for creating meta data
    /// @param[in]  typeString      type of the node
    /// @param[in]  metaData        All the meta data information (description, class, operations, ...)
    /// @param[in]  callback        Callback to call when function is finished
    /// @param[in]  unit            unit of the node (leave empty, when unknown)
    static void createMetaData(const std::string& typeString,
                               const MetaDataInfo& metaData, const comm::datalayer::IClient::ResponseCallback& callback, const std::string& unit = "");

    /// @brief Helper for creating meta data
    /// @param[in]  address           the address string of the request
    /// @param[in]  typeString        type of the node
    /// @param[in]  metaData          All the meta data information (description, class, operations, ...)
    /// @param[in]  callback          Callback to call when function is finished
    /// @param[in]  unit              unit of the node (leave empty, when unknown)
    /// @param[in]  browseable        may this node be browsed? (not allowed for nodes of type Method)
    static void createMetaData3(const std::string& address, const std::string& typeString,
                                const MetaDataInfo& metaData, const comm::datalayer::IClient::ResponseCallback& callback, const std::string& unit = "", bool browseable = true);

    /// @brief Helper for creating meta data, with additional create type
    /// @param[in]  address           the address string of the request
    /// @param[in]  typeString        type of the node
    /// @param[in]  createTypeString  type of the node, when a create is used (can be set to the same value as typeString)
    /// @param[in]  metaData          All the meta data information (description, class, operations, ...)
    /// @param[in]  callback          Callback to call when function is finished
    /// @param[in]  unit              unit of the node (leave empty, when unknown)
    /// @param[in]  browseable        may this node be browsed? (not allowed for nodes of type Method)
    /// @param[in]  createOutTypeString  type of the data load, that is returned, when a create is used (used for commands to return the command ID)
    /// @param[in]  typeReadInString  input type of the node, when a read is used
    /// @param[in]  typeReadOutString output type of the node, when a read is used
    static void createMetaData3(const std::string& address, const std::string& typeString, const std::string& createTypeString,
                                const MetaDataInfo& metaData, const comm::datalayer::IClient::ResponseCallback& callback, const std::string& unit = "", bool browseable = true, const std::string& createOutTypeString = "",
                                const std::string& typeReadInString = "", const std::string& typeReadOutString = "");

    /// @brief get the profile name from the address string
    /// @param[in]   address       the address string of the request
    /// @param[out]  profileName   the axis name the is determined from the address string
    /// @return  true, if successful and the output value is valid
    static bool getProfileNameFromAddress(const std::string& address, std::string& profileName);

    /// @brief This simple method checks if the given variant has the expected type.
    /// @param[in]  data           The variant that should be checked.
    /// @param[in]  expectedType   The expected variant type.
    /// @param[in]  callback       The datalayer callback function which will be executed if an error occurs.
    /// @return  true if type is correct and false if not
    static bool checkVariantType(const comm::datalayer::Variant* data, comm::datalayer::VariantType expectedType, const comm::datalayer::IClient::ResponseCallback& callback);

    /// @brief This method converts a datalayer address with a wildcard into the associated type string.
    /// @param[in]   address      Datalayer address that contains a wildcard.
    /// @return  The type string if conversion succeeded and empty string if not.
    static std::string getTypeString(const std::string& address);

    /// @brief check the name of the axisprofile
    /// @param[in]  profileName   the profile name to be checked
    /// @param[out] dynErrTxt     dynamic error text for diagnosis
    /// @return  true, if object name is valid
    static bool checkProfileName(const std::string& profileName, std::string& dynErrTxt);

    /// @brief Extract the last subnode of an address string. Example: motion/kin/kin1/cfg/coord-systems/PCS/sets/set1 will return "set1".
    /// @param[in]      address   Address string to work with
    /// @param[out]     result    Extracted subnode
    /// @result         Status of function call
    static bool getLastSubNodeOfAddress(const std::string& address, std::string& result);

    /// @brief Extract content of the desired wildcard from the address string. The method searches for the given 'priorSearchNode' and extracts the node that comes after.
    ///        This works no matter how many wildcards are present in the address string.
    ///        Example: comm/axisprofile/profiles/myProfile/input-mapping, priorSerachNode: '/profiles/' --> Result: 'myProfile'
    /// @param[in]      address           Address string to work with
    /// @param[in]      priorSearchNode   Node which is in front of the desired wildcard content
    /// @param[out]     result            Extracted wildcard content
    /// @result         Status of function call
    static bool extractWildcardContentFromAddress(const std::string& address, const std::string& priorSearchNode, std::string& result);

    /// @brief fill a flatbuffer with error info for the response to the datalayer client
    /// @param[in]      callback          Callback to call when function is finished
    /// @param[in]      result            Datalayer result
    /// @param[in]      maindiag          Main diagnosis
    /// @param[in]      detaileddiag      Detailed diagnosis
    /// @param[in]      dynmicdescription Additional diagnosis
    /// @result         status of function call 
    static bool sendDlErrorResponse(const comm::datalayer::IClient::ResponseCallback& callback, comm::datalayer::DlResult result, const std::uint32_t maindiag, const std::uint32_t detaileddiag, const char* dynmicdescription, ...);

    /// @brief get the last node name from the address string
    /// @param[in]   address    the address string of the request
    /// @param[out]  nodeName   the node name the is determined from the address string
    /// @return  true, if successful and the output value is valid
    static bool getLastNodeNameFromAddress(const std::string& address,std::string& nodeName);

    /// @brief replace the first wide character in the address with the given string
    /// @param[in]  address      The address string to modify
    /// @param[in]  replaceStr   The string to replace the first wide character with
    /// @param[out] newAddress   The modified address string
    /// @return true if the replacement was successful, false otherwise
    static bool replaceFirstWideCharInAddress(const std::string& address, const std::string& replaceStr, std::string& newAddress);

  private: // methods

    /// @brief Helper for creating meta data, with additional create type
    /// @param[in]  address           the address string of the request
    /// @param[in]  typeString        type of the node
    /// @param[in]  createTypeString  type of the node, when a create is used (can be set to the same value as typeString)
    /// @param[in]  metaData          All the meta data information (description, class, operations, ...)
    /// @param[in]  callback          Callback to call when function is finished
    /// @param[in]  unit              unit of the node (leave empty, when unknown)
    /// @param[in]  browseable        may this node be browsed? (not allowed for nodes of type Method)
    /// @param[in]  createOutTypeString  type of the data load, that is returned, when a create is used (used for commands to return the command ID)
    /// @param[in]  typeReadInString  input type of the node, when a read is used
    /// @param[in]  typeReadOutString output type of the node, when a read is used
    /// @param[in]  extensions        list of extensions to be added to the metadata as key-value pairs (both are strings)
    static void createMetaDataInternal(const std::string& address, const std::string& typeString, const std::string& createTypeString,
                                      const MetaDataInfo& metaData, const comm::datalayer::IClient::ResponseCallback& callback, const std::string& unit, bool browseable, const std::string& createOutTypeString,
                                      const std::string& typeReadInString, const std::string& typeReadOutString, const std::vector<std::pair<std::string, std::string>>& extensions);
  };
}
