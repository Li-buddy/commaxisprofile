#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/datalayer_system.h"
#include "mockup/mock_axisprofile_cmp.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    namespace provider
    {
      class NodeProfilesBootParams;
    }
  }
  namespace datalayer
  {
    class IProvider;
    class IClient;
  }
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @class TestBootParamBackupRestoreManager
/// Test class for boot parameter backup/restore manager.
class TestBootParamBackupRestoreManager : public xTST_Test2
{
public:
  /// Constructor
  TestBootParamBackupRestoreManager() = default;
  /// Destructor
  virtual ~TestBootParamBackupRestoreManager() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Configuration.BootParamBackupRestoreMgr");
  TEST_DECLARE_DESCRIPTION("Unittest for boot parameter backup/restore manager of the axis profile bundle.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_WSY);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private:
  /// @brief Test load/save boot parameters with valid data.
  /// @return S_OK, when tests were successful.
  HRESULT testBootParamsPersitence();

  /// @brief Test error branches of loadBootParams()/saveBootParam().
  /// @return S_OK, when tests were successful.
  HRESULT testBootParamsPersistenceErrors();

  /// @brief Test static boot parameter flatbuffer pack/unpack helper methods.
  /// @return S_OK, when tests were successful.
  HRESULT testBootParamsFlatbufferCodec();

  /// @brief Setup the test environment for this unittest.
  /// @return S_OK, when setup was successful.
  HRESULT configureTestEnvironment();

  /// @brief Register all required types at the datalayer.
  /// @return S_OK, when registration was successful.
  HRESULT registerBootParamType();

  /// @brief Reset the test environment.
  /// @return S_OK, when reset was successful.
  HRESULT resetTestEnvironment();

  /// @brief Determine the base path for config test data.
  /// @return S_OK, when path was found.
  HRESULT determinePathToTestConfigFolder();

  /// @brief Determine the path to needed bfbs files.
  /// @return S_OK, when path was found.
  HRESULT determinePathToBootParamBfbsFile();

private:
  const std::string m_profilesBootParamTypeAddress = "types/axisprofile/profiles/state/boot-params"; ///< Type address string for boot params
  const std::string m_testProfileName = "testProfile";                                               ///< profile name used in this test
  comm::datalayer::IProvider *m_datalayerProvider{};                                                 ///< Pointer to the datalayer provider
  test::MockAxisProfileCmp m_mockAxisProfileCmp{};                                                   ///< Mockup of the axis profile component
  comm::datalayer::DatalayerSystem m_testDatalayerSystem{};                                          ///< DatalayerSystem used in this test
  std::string m_bootParamBfbsPath{};                                                                 ///< Path to the bfbs file of the boot params flatbuffer
  std::string m_configFolderPath{};                                                                  ///< Path to the test folder where all configuration Json files are stored
};
