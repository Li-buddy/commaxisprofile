#pragma once
// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/mock_profile_trace.h"
#include "comm/datalayer/datalayer_system.h"
#include "configuration/backup_restore_mgr.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    namespace provider
    {
      class NodeProfilesConfig;
    }
  }

  namespace datalayer
  {
    class IProvider;
  }
} 

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @class TestAxisProfileDataCompatibility
/// Test class for the backup/restore manager of the axis profile bundle.
class TestAxisProfileDataCompatibility : public xTST_Test2
{
public:

  TestAxisProfileDataCompatibility();
  virtual ~TestAxisProfileDataCompatibility() noexcept override;

  /// test data structure
  struct TestData
  {
    const wchar_t* m_testName{};                            ///< name of the test
    const char* configurationPath{};                        ///< configuration path
    const char* loadId{};                                   ///< load id
    comm::axisprofile::LoadPhases phase{};                  ///< phase 
    comm::axisprofile::AxisProfileResult expResult{};       ///< expected return result
  };

  /// interface functions
  TEST_DECLARE_NAME("Unittest.Configuration.BackupRestoreManager.Compatibility");
  TEST_DECLARE_DESCRIPTION("Unittest(Compatibility) for the backup/restore manager of the axis profile bundle.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods
  // ----------------------------------------- Tests ---------------------------------------------

  /// @brief   Test all transaction based loadSystemCfg()
  /// @return  S_OK, when tests was successful
  HRESULT testloadSystemCfg();

  // ------------------------------------- Helper Methods ----------------------------------------

  /// @brief   Setup the test environment for this unittest.
  /// @return  S_OK, when configuration was successful
  HRESULT configureTestEnvironment();

  /// @brief   Register the type of the profile configuration node at the datalayer.
  /// @return  S_OK, when registration was successful
  HRESULT registerProfileConfigurationType();

  /// @brief   Determine the base path for config test data
  /// @return  S_OK, when the test was successful
  HRESULT determinePathToTestConfigFolder();

  /// @brief   Determine the path to the profile configuration flatbuffer bfbs file.
  /// @return  S_OK, when the test was successful
  HRESULT determinePathToProfileConfigBfbsFile();

  /// @brief execute a testcase for the new transactional load mechanism
  /// @param[in]  testData    test data
  /// @return Return value indicating success or error.
  HRESULT executeLoadTestcase(const TestData& testData);

private:

  const std::string m_profilesConfigNodeTypeAddress = "types/axisprofile/profiles/cfg";    ///< Type address string for NodeProfilesConfig
  const std::string m_testProfileName = "testProfile";                                     ///< Profile name used in this test
  comm::axisprofile::IBackupRestoreMgr* m_backupMgr{};                                     ///< pointer to backup  and restore manager
  comm::datalayer::IProvider* m_datalayerProvider{};                                       ///< Pointer to the datalayer provider
  comm::axisprofile::provider::NodeProfilesConfig* m_profilesConfigNode{};                 ///< Pointer to the NodeProfilesConfig instance
  test::MockAxisProfileCmp m_mockAxisProfileCmp{};                                         ///< Mockup of the axis profile component
  comm::datalayer::DatalayerSystem m_testDatalayerSystem{};                                ///< DatalayerSystem used in this test
  comm::datalayer::PersistenceHelper* m_pPersistenceHelper{};                              ///< pointer to PersistenceHelper
  std::string m_configBfbsPath{};                                                          ///< Path to the bfbs file of the configuration flatbuffer
  std::string m_configFolderPath{};                                                        ///< Path to the test folder where all configuration Json files are stored
  const std::vector<TestData> m_testData{};                                                ///< Data of all single transaction based load
};
