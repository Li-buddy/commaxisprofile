#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/datalayer_system.h"
#include "configuration/backup_restore_mgr.h"
#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/mock_profile_trace.h"
#include "stdafx.h"

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
  } // namespace axisprofile
  namespace datalayer
  {
    class IProvider;
  }
} // namespace comm

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestBackupRestoreManager
//! Test class for the backup/restore manager of the axis profile bundle.
class TestBackupRestoreManager : public xTST_Test2
{
public:
  //! Constructor
  TestBackupRestoreManager();
  //! Destructor
  virtual ~TestBackupRestoreManager() noexcept override;

  //! test data structure
  struct TestData
  {
    const wchar_t *m_testName{};                            //!< name of the test
    const char* configurationPath{};                        //!< configuration path
    const char* loadId{};                                   //!< load id
    comm::axisprofile::LoadPhases phase{};                  //!< phase 
    comm::axisprofile::AxisProfileResult expResult{};       //!< expected return result
  };
  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Configuration.BackupRestoreManager");
  TEST_DECLARE_DESCRIPTION("Unittest for the backup/restore manager of the axis profile bundle.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // methods
  // ----------------------------------------- Tests ---------------------------------------------

  //! @brief   This method is called by testErrors() and carries out all nullptr error test cases.
  //! @return  S_OK, when tests was successful
  HRESULT testErrorsNullptr();

  //! @brief   Test all load() error cases.
  //! @return  S_OK, when tests was successful
  HRESULT testLoadErrors();

  //! @brief   Test all save() error cases.
  //! @return  S_OK, when tests was successful
  HRESULT testSaveErrors();

  //! @brief   Test all loadSystemCfg() error cases.
  //! @return  S_OK, when tests was successful
  HRESULT testloadSystemCfgErrors();

  //! @brief   Test all saveSystemCfg() error cases.
  //! @return  S_OK, when tests was successful
  HRESULT testsaveSystemCfgErrors();

  //! @brief   Test all transaction based saveSystemCfg()
  //! @return  S_OK, when tests was successful
  HRESULT testsaveSystemCfg();

  //! @brief   Test all transaction based loadSystemCfg()
  //! @return  S_OK, when tests was successful
  HRESULT testloadSystemCfg();

  //! @brief   Test all methods of the backup/restore manager.
  //! @return  S_OK, when tests was successful
  HRESULT testClass();

  /// @brief   Test all transaction based makePersistent()
  /// @return  S_OK, when tests was successful
  HRESULT testMakePersistent();

  // ------------------------------------- Helper Methods ----------------------------------------

  //! @brief   Setup the test environment for this unittest.
  //! @return  S_OK, when configuration was successful
  HRESULT configureTestEnvironment();

  //! @brief   Register the type of the profile configuration node at the datalayer.
  //! @return  S_OK, when registration was successful
  HRESULT registerProfileConfigurationType();

  //! @brief   This method resets the test environment.
  //! @param[in]   client   Pointer to the datalayer client
  //! @return  S_OK, when reset was successful
  HRESULT resetTestEnvironment(comm::datalayer::IClient *client);

  //! @brief   Determine the base path for config test data
  //! @return  S_OK, when the test was successful
  HRESULT determinePathToTestConfigFolder();

  //! @brief   Determine the path to the profile configuration flatbuffer bfbs file.
  //! @return  S_OK, when the test was successful
  HRESULT determinePathToProfileConfigBfbsFile();

  //! @brief This method can be used to add some simple input/output mapping to get more information
  //! in the generated configuration file.
  //! @return S_OK when method finished successfully
  HRESULT addInputOutputMapping();

  //! @brief execute a testcase for the new transactionorented load mechanism
  //! @param[in]  testData    test data
  //! @return Return value indicating success or error.
  HRESULT executeLoadTestcase(const TestData &testData);

private:                                                                                   // data
  const std::string m_profilesConfigNodeTypeAddress = "types/axisprofile/profiles/cfg";    //!< Type address string for NodeProfilesConfig
  const std::string m_testProfileName = "testProfile";                                     //!< Profile name used in this test
  comm::axisprofile::IBackupRestoreMgr *m_backupMgr{};                                     //!< pointer to backup  and restore manager
  comm::datalayer::IProvider* m_datalayerProvider{};                                       //!< Pointer to the datalayer provider
  comm::axisprofile::provider::NodeProfilesConfig* m_profilesConfigNode{};                 //!< Pointer to the NodeProfilesConfig instance
  test::MockAxisProfileCmp m_mockAxisProfileCmp{};                                         //!< Mockup of the axis profile component
  comm::datalayer::DatalayerSystem m_testDatalayerSystem{};                                //!< DatalayerSystem used in this test
  std::string m_configBfbsPath{};                                                          //!< Path to the bfbs file of the configuration flatbuffer
  std::string m_configFolderPath{};                                                        //!< Path to the test folder where all configuration Json files are stored
  const std::vector<TestData> m_testData{};                                                //!< Data of all single transaction based load
  const std::vector<TestData> m_testData2{};                                               //!< Data of  miscellaneous

private: // types
  //! @class MockAxisProfileMappingHandlerSpecial
  //! Specialization of the axis profile maping handler mockup, needed for this test.
  class MockAxisProfileMappingHandlerSpecial : public test::MockAxisProfileMappingHandler
  {
  public: // methods
    //! @brief Constructor
    MockAxisProfileMappingHandlerSpecial() : test::MockAxisProfileMappingHandler() {}

    //! @brief Destructor
    virtual ~MockAxisProfileMappingHandlerSpecial() noexcept
    {
      m_inputMapping.clear();
      m_outputMapping.clear();
    }

    const std::vector<comm::axisprofile::InputMappingEntry> getConfiguredInputs() const final
    {
      std::vector<comm::axisprofile::InputMappingEntry> inputMapping{};
      for (auto const &mappingItem : m_inputMapping)
      {
        inputMapping.emplace_back(mappingItem.first, mappingItem.second, MappingGroup::BASE, false);
      }
      return inputMapping;
    }

    const std::vector<comm::axisprofile::OutputMappingEntry> getConfiguredOutputs() const final
    {
      std::vector<comm::axisprofile::OutputMappingEntry> outputMapping{};
      for (auto const &mappingItem : m_outputMapping)
      {
        outputMapping.emplace_back(mappingItem.first, mappingItem.second, MappingGroup::BASE, false);
      }
      return outputMapping;
    }

  public:                                                                             // data
    std::map<comm::axisprofile::InputMappingValueID, std::string> m_inputMapping{};   //!< Map that contains the general input mapping
    std::map<comm::axisprofile::OutputMappingValueID, std::string> m_outputMapping{}; //!< Map that contains the general output mapping
  };
};
