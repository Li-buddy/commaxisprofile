#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_backup_restore_mgr.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and implements a mockup
//! of the backup/restore manager.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "configuration/i_backup_restore_mgr.h"


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockBackupRestoreManager
  //! @brief This class implements a mockup of the backup/restore manager.
  class MockBackupRestoreManager : public comm::axisprofile::IBackupRestoreMgr
  {
  public: // methods

    //! @brief Constructor
    MockBackupRestoreManager() = default;
    //! @brief Destructor
    virtual ~MockBackupRestoreManager() = default;

    comm::axisprofile::AxisProfileResult load(const std::string& /*profileName*/, const std::string& /*dlCfgUri*/, const std::string& /*cfgFilePath*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult save(const std::string& /*profileName*/, const std::string& /*dlCfgUri*/, const std::string& /*cfgFilePath*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult loadSystemCfg(const std::string& /*setup*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult saveSystemCfg(const std::string& /*setup*/) override { return m_returnValue; }

    comm::axisprofile::AxisProfileResult loadSystemCfg(const comm::datalayer::Variant& /* persistenceParam */) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult saveSystemCfg(const comm::datalayer::Variant& /* persistenceParam */) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult makePersistent(const comm::datalayer::Variant& /*persistenceParam*/, const std::string& /*axisprofileName*/)override { return m_returnValue; }

    // -------------------------------- Test methods ------------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

  protected: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   //!< Result of member functions
  };
}
