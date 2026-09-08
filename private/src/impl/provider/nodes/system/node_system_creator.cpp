// ---------------------------------------------------------------------------
//
//! @file       node_system_creator.cpp
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
//! class to create all system related nodes.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_system_act_statemachine.h"
#include "node_system_boot_state.h"
#include "node_system_boot_timeout.h"
#include "node_system_cmd_statemachine.h"
#include "node_system_creator.h"
#include "node_system_use_checksum.h"
#include "provider/nodes/node_backup_restore.h"
#include "provider/nodes/node_cfg_make_persistent.h"
#include "provider/nodes/node_transaction_based_loadsave.h"
#include "provider/provider_helper.h"
#include "provider/provider_mgr.h"

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

void NodeSystemCreator::addNodes(ProviderMgr *mgr)
{
  mgr->addNode(NodeTree.APR_SYSTEM_CONFIG_LOAD,                     new NodeBackupRestore(mgr, SaveLoadOperation::LOAD_ALL,                      //!< axisprofile/system/config/load
                                                                                         {"load all axisprofile", ""}));                         //!< axisprofile/system/config/load
  mgr->addNode(NodeTree.APR_SYSTEM_CONFIG_SAVE,                     new NodeBackupRestore(mgr, SaveLoadOperation::SAVE_ALL,                      //!< axisprofile/system/config/save
                                                                                         {"save all axisprofile", ""}));                         //!< axisprofile/system/config/save
  mgr->addNode(NodeTree.APR_SYSTEM_OPSTATE_CMD,                     new NodeSystemCmdStateMachine(mgr));                                         //!< axisprofile/system/opstate/cmd
  mgr->addNode(NodeTree.APR_SYSTEM_OPSTATE_CMD + NodeTree.WILDCARD, new NodeSystemCmdStateMachine(mgr));                                         //!< axisprofile/system/opstate/cmd/[name]
  mgr->addNode(NodeTree.APR_SYSTEM_OPSTATE_ACTUAL,                  new NodeSystemActStateMachine(mgr));                                         //!< axisprofile/system/opstate/actual
  mgr->addNode(NodeTree.APR_SYSTEM_BOOT_STATE,                      new NodeSystemBootState(mgr));                                               //!< axisprofile/system/boot-state
  mgr->addNode(NodeTree.APR_SYSTEM_BOOT_TIMEOUT,                    new NodeSystemBootTimeout(mgr));                                             //!< axisprofile/system/boot-timeout

  mgr->addNode(NodeTree.APR_SYSTEM_USE_CHECKSUM,                    new NodeSystemUseChecksum(mgr));                                             //!< axisprofile/system/use-checksum
  mgr->addNode(NodeTree.AXISPROFILE_ADMIN_CFG_SAVE,                 new NodeTransactionLoadSave(mgr, PersistanceOperation::SAVE_AXISPROFILE,     //!< axisprofile/admin/config/load
                                                                                               {"save data of comm.axisprofile", ""}));          //!< axisprofile/admin/config/load
  mgr->addNode(NodeTree.AXISPROFILE_ADMIN_CFG_LOAD,                 new NodeTransactionLoadSave(mgr, PersistanceOperation::LOAD_AXISPROFILE,     //!< axisprofile/admin/config/save
                                                                                               {"load data of comm.axisprofile", ""}));          //!< axisprofile/admin/config/load
  mgr->addNode(NodeTree.AXISPROFILE_ADMIN_CFG_MAKEPERSISTENT + NodeTree.WILDCARD,              new NodeCfgMakePersistent(mgr));                  ///< axisprofile/admin/config/make-persistent/[name]

}

