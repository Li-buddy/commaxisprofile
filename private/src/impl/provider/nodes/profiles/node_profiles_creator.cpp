// ---------------------------------------------------------------------------
//
//! @file       node_profiles_creator.cpp
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This source file is part of the axisprofile library and provides a helper
//! class to create all profile related nodes.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_creator.h"
#include "node_profiles.h"
#include "provider/provider_helper.h"
#include "provider/provider_mgr.h"
#include "provider/nodes/node_backup_restore.h"
#include "state/node_profiles_device_offset.h"
#include "state/node_profiles_checksum_value.h"
#include "state/node_profiles_checksum_list.h"
#include "state/node_profiles_boot_params.h"
#include "cfg/node_profiles_config.h"
#include "cfg/node_profiles_set_uri.h"
#include "cfg/node_profiles_basicData.h"
#include "cfg/node_profiles_input_mapping.h"
#include "cfg/node_profiles_output_mapping.h"
#include "cfg/node_profiles_map_single_input.h"
#include "cfg/node_profiles_device_name.h"
#include "cfg/node_profiles_map_single_output.h"
#include "cfg/node_profiles_scalingStrategyCfg.h"
#include "cfg/node_profiles_scalingStrategy.h"
#include "cfg/node_profiles_deviceScalingType.h"
#include "cfg/node_profiles_category.h"
#include "cfg/node_profiles_scalingFactor.h"
#include "cfg/node_profiles_use_checksum.h"
#include "cfg/node_profiles_ethercatmaster.h"
#include "templates/node_state_simple.h"
#include "templates/node_cfg_simple.h"
#include "cfg/node_profiles_scalingUint.h"
// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::provider;


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

void NodeProfilesCreator::addNodes(ProviderMgr* mgr)
{
  //original parallel compatible node
  mgr->addNode(NodeTree.APR_PROFILES_WC,                             new NodeProfiles(mgr));                                  ///< axisprofile/profiles/[name]

  //subnode under cfg node
  mgr->addNode(NodeTree.APR_PROFILES_WC,                             new NodeProfiles(mgr));                                  ///< axisprofile/profiles/[name]
  mgr->addNode(NodeTree.APR_PROFILES_CFG,                            new NodeProfilesConfig(mgr));                            ///< axisprofile/profiles/[name]/config

  // ----------------------------------------
  // ----------   backup restore  -----------
  // ----------------------------------------
  mgr->addNode(NodeTree.APR_PROFILES_CFG_LOAD,                       new NodeBackupRestore(mgr, SaveLoadOperation::LOAD_SINGLE_PROFILE,
                                                                     {"load single axisprofile", ""}));                       ///< axisprofile/profiles/[name]/config/load
  mgr->addNode(NodeTree.APR_PROFILES_CFG_SAVE,                       new NodeBackupRestore(mgr, SaveLoadOperation::SAVE_SINGLE_PROFILE,
                                                                     {"save single axisprofile", ""}));                       ///< axisprofile/profiles/[name]/config/save
  // -------------------------------------------
  // ----------   in-/output buffer  -----------
  // -------------------------------------------
  mgr->addNode(NodeTree.APR_PROFILES_CFG_INBUFFER,                   new NodeProfilesSetURI(mgr, BufferOrient::INPUT_BUFFER));   ///< axisprofile/profiles/[name]/cfg/input-buffer
  mgr->addNode(NodeTree.APR_PROFILES_CFG_OUTBUFFER,                  new NodeProfilesSetURI(mgr, BufferOrient::OUTPUT_BUFFER));  ///< axisprofile/profiles/[name]/cfg/output-buffer

  // ---------------------------------------
  // ----------   input mapping  -----------
  // ---------------------------------------
  mgr->addNode(NodeTree.APR_PROFILES_CFG_INMAPPING,                  new NodeProfilesInputMapping(mgr));                      ///< axisprofile/profiles/[name]/cfg/input-mapping
  mgr->addNode(NodeTree.APR_PROFILES_CFG_INMAPPING_MODE,             new NodeProfilesMapSingleInput(mgr));                    ///< axisprofile/profiles/[name]/cfg/input-mapping/*

  // ----------------------------------------
  // ----------   output mapping  -----------
  // ----------------------------------------
  mgr->addNode(NodeTree.APR_PROFILES_CFG_OUTMAPPING,                 new NodeProfilesOutputMapping(mgr));                     ///< axisprofile/profiles/[name]/cfg/output-mapping
  mgr->addNode(NodeTree.APR_PROFILES_CFG_OUTMAPPING_MODE,            new NodeProfilesMapSingleOutput(mgr));                   ///< axisprofile/profiles/[name]/cfg/output-mapping/*

  mgr->addNode(NodeTree.APR_PROFILES_CFG_DEVICEADDRESS,              new NodeCfgSimple<std::uint32_t, IAxisProfileInternal, comm::axisprofile::AxisProfileResult>(mgr, &IAxisProfileInternal::getDeviceAddress, &IAxisProfileInternal::setDeviceAddress,
                                                                                                                                       {"device address", ""}));                ///< axisprofile/profiles/[name]/cfg/device-address
  mgr->addNode(NodeTree.APR_PROFILES_CFG_DEVICENAME,                 new NodeProfilesDeviceName(mgr));                        ///< axisprofile/profiles/[name]/cfg/device-name
  mgr->addNode(NodeTree.APR_PROFILES_CFG_BASICDATA,                  new NodeProfilesBasicData(mgr));                         ///< axisprofile/profiles/[name]/cfg/profileBasicData
  mgr->addNode(NodeTree.APR_PROFILES_CFG_DRIVENUMBER,                new NodeCfgSimple<std::uint8_t, IAxisProfileInternal, comm::axisprofile::AxisProfileResult>    ///< axisprofile/profiles/[name]/cfg/drive-number
               (mgr, &IAxisProfileInternal::getDriveNumber, &IAxisProfileInternal::setDriveNumber,
                {"Used for situations where multiple axes correspond to a single address. If there is only one address corresponding to one axis, the default value is 0. Otherwise, add 1 in sequence", ""}));
  mgr->addNode(NodeTree.APR_PROFILES_CFG_CATEGORY,                   new NodeProfilesCategory(mgr));                          ///< axisprofile/profiles/[name]/cfg/profile-category
  mgr->addNode(NodeTree.APR_PROFILES_CFG_USE_CHECKSUM,               new NodeProfilesUseChecksum(mgr));                       ///< axisprofile/profiles/[name]/cfg/device-app-data/use-checksum
  mgr->addNode(NodeTree.APR_PROFILES_CFG_ETHERCAT,                   new NodeProfilesEtherCATMasterName(mgr));                ///< axisprofile/profiles/[name]/cfg/ethercat-master-name
  mgr->addNode(NodeTree.APR_PROFILES_CFG_BOOTPARAMS,                 new NodeProfilesBootParams(mgr));                        ///< axisprofile/profiles/[name]/cfg/boot-params

  // ---------------------------------
  // ----------   scaling  -----------
  // ---------------------------------
  mgr->addNode(NodeTree.APR_PROFILES_CFG_SCALINGINFO,                new NodeProfilesScalingStrategyCfg(mgr));                ///< axisprofile/profiles/[name]/cfg/profile-scaling-information
  mgr->addNode(NodeTree.APR_PROFILES_CFG_SCALINGINFO_STRATEGY,       new NodeProfilesScalingStrategy(mgr));                   ///< axisprofile/profiles/[name]/cfg/profile-scaling-strategy
  mgr->addNode(NodeTree.APR_PROFILES_CFG_SCALINGINFO_TYPE,           new NodeProfilesDeviceScalingType(mgr));                 ///< axisprofile/profiles/[name]/cfg/profile-scaling-scaling-type
  mgr->addNode(NodeTree.APR_PROFILES_CFG_SCALINGINFO_NUMERATOR,      new NodeProfilesScalingUint(mgr, &AxisProfileScalingStrategyCfg::m_numerator, {"Scaling Factor Numerator(valid only for COE_CALC_CONTROL strategy)", ""}));                  ///< axisprofile/profiles/[name]/cfg/profile-scaling-numerator
  mgr->addNode(NodeTree.APR_PROFILES_CFG_SCALINGINFO_DENOMINATOR,    new NodeProfilesScalingUint(mgr, &AxisProfileScalingStrategyCfg::m_denominator, {"Scaling Factor Denominator(only valid for COE_CALC_CONTROL strategy)", ""}));                 ///< axisprofile/profiles/[name]/cfg/profile-scaling-denominator
  mgr->addNode(NodeTree.APR_PROFILES_CFG_SCALINGINFO_RESOLUTION,     new NodeProfilesScalingUint(mgr, &AxisProfileScalingStrategyCfg::m_resolution, {"Encoder resolution(valid only for COE_CALC_CONTROL strategy)", ""}));                  ///< axisprofile/profiles/[name]/cfg/profile-scaling-resolution
  mgr->addNode(NodeTree.APR_PROFILES_CFG_SCALINGINFO_VELOCITYFACTOR, new NodeProfilesScalingFactorCfg(mgr));                  ///< axisprofile/profiles/[name]/cfg/

  // -------------------------------
  // ----------   state  -----------
  // -------------------------------
  mgr->addNode(NodeTree.APR_PROFILES_DEVICEOFFSET,                   new NodeProfilesDeviceOffset(mgr));                      ///< axisprofile/profiles/[name]/state/device-pos-offset
  mgr->addNode(NodeTree.APR_PROFILES_ACTIVATED,                      new NodeStateSimple<bool, IAxisProfileInternal>
               (&IAxisProfileInternal::checkProfileActivated, mgr,
                {"the profile is activated or deactivated", ""}));                                                            ///< axisprofile/profiles/[name]/state/activated
  mgr->addNode(NodeTree.APR_PROFILES_CHECKSUM_VALUE,                 new NodeProfilesChecksumValue(mgr));                     ///< axisprofile/profiles/[name]/state/device-app-data/checksum-value
  mgr->addNode(NodeTree.APR_PROFILES_CHECKSUM_IDN_LIST,              new NodeProfilesChecksumList(mgr));                      ///< axisprofile/profiles/[name]/state/device-app-data/checksum-idn-list

}
