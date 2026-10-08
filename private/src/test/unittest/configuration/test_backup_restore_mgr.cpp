#include <thread>
#include <filesystem>
#include <fstream>
#include "test_backup_restore_mgr.h"

#include "provider/provider_helper.h"
#include "provider/nodes/profiles/cfg/node_profiles_config.h"
#include "provider/nodes/profiles/state/node_profiles_boot_params.h"
#include "comm/datalayer/persistence_param_generated.h"
#include "comm/axisprofile/datatypes/dummyFbs_generated.h"

#include "mockup/mock_axisprofile.h"

using namespace comm::axisprofile;

namespace
{
  std::uint32_t browseDirectoryRegexAlwaysFail(const void* /*handle*/, const std::string& /*directory*/, bool /*includeDirectories*/,
                                               const std::string& /*regularExpression*/, std::vector<std::string>& /*files*/)
  {
    return static_cast<std::uint32_t>(comm::datalayer::DlResult::DL_FAILED);
  }
}

REGISTER_TEST(TestBackupRestoreManager);
REGISTER_TEST_TO_GROUP(TestBackupRestoreManager, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestBackupRestoreManager, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestBackupRestoreManager, Configuration);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestBackupRestoreManager::TestBackupRestoreManager() : m_testData
{
  {L"Prepare",                      "/incorrect/name",             "prepare",     LoadPhases::PREPARE,            AxisProfileResult::STS_OK},
  {L"Validate",                     "/incorrect/name",             "validate",    LoadPhases::VALIDATE,           AxisProfileResult::STS_OK},
  {L"Activate",                     "/incorrect/name",             "activate",    LoadPhases::ACTIVATE,           AxisProfileResult::STS_OK},
  {L"Abort",                        "/incorrect/name",             "abort",       LoadPhases::ABORT,              AxisProfileResult::STS_OK},
  {L"Incorretct profile name",      "/incorrect/name",             "query1",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct address",           "/incorrect/address",          "query2",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct inputBuffer",       "/incorrect/inputbuffer",      "query3",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct outputBuffer",      "/incorrect/outputbuffer",     "query4",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct inputMappingId1",   "/incorrect/inputMappingId1",  "query5",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct inputMappingId2",   "/incorrect/inputMappingId2",  "query6",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct outputMappingId1",  "/incorrect/outputMappingId1", "query7",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},  
  {L"Incorretct outputMappingId1",  "/incorrect/outputMappingId1", "query8",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct outputMappingId2",  "/incorrect/outputMappingId2", "query9",      LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE}, 
  {L"Incorretct inputMappingUrl1",  "/incorrect/inputMappingUrl1", "query10",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct inputMappingUrl2",  "/incorrect/inputMappingUrl2", "query11",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE}, 
  {L"Incorretct outputMappingUrl1", "/incorrect/outputMappingUrl1","query12",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},   
  {L"Incorretct outputMappingUrl2", "/incorrect/outputMappingUrl2","query13",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE}, 
  {L"Incorretct scalingDenominator","/incorrect/scalingDenominator","query14",    LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct scalingNumerator",  "/incorrect/scalingNumerator", "query15",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct scalingResolution", "/incorrect/scalingResolution","query16",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct ScalingStrategy",   "/incorrect/scalingStrategy",  "query17",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct scalingDeviceType", "/incorrect/scalingDeviceType","query18",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Incorretct deviceName",        "/incorrect/deviceName",       "query19",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"No file",                      "/empty/",                     "query19",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"functionality",                "",                            "query20",     LoadPhases::QUERY,              AxisProfileResult::STS_OK},
  {L"Incorretct type",              "/incorrect/type",             "query21",     LoadPhases::QUERY,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
  {L"Load",                         "/incorrect/name",             "load",        LoadPhases::LOAD,               AxisProfileResult::ERR_DATALAYER_WRITE_NODE},
  {L"Undefined load phase",         "/incorrect/name",             "count",       LoadPhases::COUNT,              AxisProfileResult::ERR_NOT_IN_CONFIG_STATE},
 
},
m_testData2
{
{L"Null persistenceHelper pLoad",       "/incorrect/name",           "load",    LoadPhases::LOAD,               AxisProfileResult::ERR_EXEC_NULL},    
}
{}



TestBackupRestoreManager::~TestBackupRestoreManager(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestBackupRestoreManager::doInit()
{
  m_testDatalayerSystem.start();
  m_mockAxisProfileCmp.addDataLayer(m_testDatalayerSystem.factory());
  comm::datalayer::PersistenceHelper* persistenceHelper = new comm::datalayer::PersistenceHelper(&m_testDatalayerSystem);
  persistenceHelper->start();
  m_mockAxisProfileCmp.setPersistenceHelper(persistenceHelper);

  log(L"\nGet path to profile configuration flatbuffer Bfbs file.");
  HRESULT hresult = determinePathToProfileConfigBfbsFile();
  if (FAILED(hresult))
  {
    error(L"\nCould not get path to profile configuration flatbuffer Bfbs file. Aborting test...");  
    return E_FAIL;
  }

  m_backupMgr = new BackupRestoreMgr(m_mockAxisProfileCmp.getAxisProfileManager(), m_testDatalayerSystem.factory()->createClient(), m_mockAxisProfileCmp.getPersistenceHelper());

  log(L"\nGet test configuration folder path (location of Json files).");
  hresult = determinePathToTestConfigFolder();
  if (FAILED(hresult))
  {
    error(L"\nCould not get test configuration files. Aborting test..."); 
    return E_FAIL;
  }
  return S_OK;
}

HRESULT TestBackupRestoreManager::doEnd()
{
  m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(m_testProfileName.c_str());
  if (m_datalayerProvider)  { delete m_datalayerProvider;  m_datalayerProvider  = nullptr; }
  if (m_profilesConfigNode) { delete m_profilesConfigNode; m_profilesConfigNode = nullptr; }
  if (m_bootParamNode) { delete m_bootParamNode; m_bootParamNode = nullptr; }

  comm::datalayer::PersistenceHelper* persistenceHelper = m_mockAxisProfileCmp.getPersistenceHelperCppItf();
  if (persistenceHelper) { delete persistenceHelper; persistenceHelper = nullptr; }
  if (m_backupMgr) {delete m_backupMgr; m_backupMgr = nullptr;}
  m_testDatalayerSystem.stop();
  return S_OK;
}

HRESULT TestBackupRestoreManager::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n Test the backup/restore manager of the axis profile bundle");
  log(L"\n----------------------------------------------------------------\n");

  log(L"\n");

  HRESULT hresult = testErrorsNullptr();
  hresult |= testLoadErrors();
  hresult |= testSaveErrors();
  hresult |= testloadSystemCfgErrors();
  hresult |= testsaveSystemCfgErrors();
  hresult |= testClass();
  hresult |= testsaveSystemCfg();
  hresult |= testloadSystemCfg();
  hresult |= testMakePersistent();

  return hresult;
}

HRESULT TestBackupRestoreManager::testLoadErrors()
{
  log(L"\n<b>----- Test all possible errors of method loadCfg() -----</b>");

  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }
  
  log(L"\nCreate backup/restore manager.");
  comm::datalayer::IClient* datalayerClient = m_testDatalayerSystem.factory()->createClient();
  BackupRestoreMgr testClassLoadErrors(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClient, m_mockAxisProfileCmp.getPersistenceHelper());

  log(L"\nCall loadCfg() and simulate error when reading metadata(not register metadata type).");
  AxisProfileResult aprResult = testClassLoadErrors.loadCfg(m_testProfileName.c_str(), "dummyJsonFilePath");
  if (aprResult != AxisProfileResult::ERR_DATALAYER_READ_METADATA)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_READ_METADATA' Actual: 0x%08X", aprResult);
    return E_FAIL;
  }

  result = registerProfileConfigurationType();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCall loadCfg() and simulate error that JSon file does not exist.");
  aprResult = testClassLoadErrors.loadCfg(m_testProfileName.c_str(), "dummyJsonFilePath");
  if (aprResult != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_PERSISTENCE' Actual: 0x%08X", aprResult);
    return E_FAIL;
  }

  log(L"\nCall loadCfg() and simulate syntax error in Json configuration file.");
  std::string datalayerAddress = provider::NodeTree.APR_PROFILES_CFG;
  datalayerAddress.replace(datalayerAddress.find("*"), 1, m_testProfileName.c_str());
  std::string pathJsonFileSyntaxError = m_configFolderPath + "/incorrect/config_syntax_error.json";
  aprResult = testClassLoadErrors.loadCfg(m_testProfileName, pathJsonFileSyntaxError);
  if (aprResult != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_PERSISTENCE' Actual: 0x%08X", aprResult);
    return E_FAIL;
  }

  log(L"\nCall loadCfg() and simulate error because of Json configuration file with missing data.");
  std::string pathJsonFileMissingData = m_configFolderPath + "/incorrect/config_wrong_data.json";
  aprResult = testClassLoadErrors.loadCfg(m_testProfileName, pathJsonFileMissingData);
  if (aprResult != AxisProfileResult::ERR_DATALAYER_WRONG_TYPE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_WRONG_TYPE' Actual: 0x%08X", aprResult);
    return E_FAIL;
  }

  log(L"\nCall loadCfg() and simulate error when trying to create a new profile.");
  static_cast<test::MockAxisProfileManager*>(m_mockAxisProfileCmp.getAxisProfileManager())->m_returnValue = AxisProfileResult::ERR_NOK;
  std::string validConfigFilePath = m_configFolderPath + "/correct/testProfile.json";
  aprResult = testClassLoadErrors.loadCfg(m_testProfileName, validConfigFilePath);
  const IAxisProfile* testProfile = m_mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_testProfileName.c_str());
  if (testProfile)
  {
    error(L"\nCall of method loadCfg() created the profile but was expected to fail!!");
    return E_FAIL;
  }
  static_cast<test::MockAxisProfileManager*>(m_mockAxisProfileCmp.getAxisProfileManager())->m_returnValue = AxisProfileResult::STS_OK;

  result = resetTestEnvironment(datalayerClient);
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\n");

  return result;
}

HRESULT TestBackupRestoreManager::testSaveErrors()
{
  log(L"\n<b>----- Test all possible errors of method save() -----</b>");

  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCreate backup/restore manager.");
  comm::datalayer::IClient* datalayerClient = m_testDatalayerSystem.factory()->createClient();
  BackupRestoreMgr testClassSaveErrors(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClient, m_mockAxisProfileCmp.getPersistenceHelper());

  log(L"\nCall save() for a profile that does not exist.");
  AxisProfileResult aprResult = testClassSaveErrors.save(m_testProfileName.c_str(), {});
  if (aprResult != AxisProfileResult::ERR_PROFILE_NOT_FOUND)
  {
    error(L"\nWrong return code! Expected: 'ERR_PROFILE_NOT_FOUND' Actual: 0x%08X", aprResult);
    return E_FAIL;
  }

  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(m_testProfileName).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_testProfileName);

  log(L"\nCall save() and simulate error from persistence helper.");
  aprResult = testClassSaveErrors.save(m_testProfileName.c_str(), {});
  if (aprResult != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_PERSISTENCE' Actual: 0x%08X", aprResult);
    return E_FAIL;
  }

  result = registerProfileConfigurationType();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCall save() and simulate error that target file path is inaccessible.");
  aprResult = testClassSaveErrors.save(m_testProfileName.c_str(), "/this/path/generates/an/error");
  if (aprResult != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_PERSISTENCE' Actual: 0x%08X", aprResult);
    return E_FAIL;
  }

  result = resetTestEnvironment(datalayerClient);
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\n");

  return result;
}

HRESULT TestBackupRestoreManager::testloadSystemCfgErrors()
{
  log(L"\n<b>----- Test all possible errors of method loadSystemCfg() -----</b>");

  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCreate backup/restore manager.");
  comm::datalayer::IClient* datalayerClient = m_testDatalayerSystem.factory()->createClient();
  BackupRestoreMgr testClassCfgErr(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClient, m_mockAxisProfileCmp.getPersistenceHelper());

  log(L"\nCall loadSystemCfg() with folder that contains an incorrect Json file (syntax error).");
  std::string pathBuffer = m_configFolderPath + "/incorrect";
  AxisProfileResult retVal = testClassCfgErr.loadSystemCfg(pathBuffer.c_str());
  if (retVal != AxisProfileResult::ERR_DATALAYER_READ_METADATA)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_READ_METADATA ' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall loadSystemCfg() with persistenceParam with wrong variant type");
  comm::datalayer::Variant persistenceParamLoad;
  retVal = testClassCfgErr.loadSystemCfg(persistenceParamLoad);
  if (retVal != AxisProfileResult::ERR_INVALID_ARGUMENT)
  {
    error(L"\nWrong return code! Expected: 'ERR_INVALID_ARGUMENT ' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  result = resetTestEnvironment(datalayerClient);
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\n");
  return result;
}

HRESULT TestBackupRestoreManager::testsaveSystemCfgErrors()
{
  log(L"\n<b>----- Test all possible errors of method saveSystemCfg() -----</b>");

  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCreate backup/restore manager.");
  comm::datalayer::IClient* datalayerClient = m_testDatalayerSystem.factory()->createClient();
  BackupRestoreMgr testClassCfgError(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClient, m_mockAxisProfileCmp.getPersistenceHelper());

  const std::string testProfile1Name = "testProfile1";
  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(testProfile1Name).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfile1Name);

  const std::string testProfile2Name = "testProfile2";
  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(testProfile2Name).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfile2Name);

  log(L"\nCall saveSystemCfg() and simulate error when saving a single profile.");
  AxisProfileResult retVal = testClassCfgError.saveSystemCfg("/this/will/fail");
  if (retVal != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_PERSISTENCE' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nDelete profile with name '%ls' again.", test::StringHelper::utf8ToWide(testProfile1Name).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(testProfile1Name.c_str());

  log(L"\nDelete profile with name '%ls' again.", test::StringHelper::utf8ToWide(testProfile2Name).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(testProfile2Name.c_str());

  result = resetTestEnvironment(datalayerClient);
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\n");
  return result;
}

HRESULT TestBackupRestoreManager::testErrorsNullptr()
{
  log(L"\n<b>----- Test all possible null-pointer errors -----</b>");

  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  for (std::uint8_t index = 0; index < 4; index++)
  {
    bool testCases[4]{}; // [0] = loadCfg(), [1] = save(), [2] = loadSystemCfg(), [3] = saveSystemCfg()
    AxisProfileResult expectedResult;
    comm::datalayer::IClient* dlClient = nullptr;
    const PersistenceHelperItf* persistenceHelper = nullptr;
    IAxisProfileManagerInternal* profileManager = nullptr;
    std::string missingClassName{};

    switch (index)
    {
    case 0:
      testCases[0] = true;  // Call loadCfg()
      testCases[1] = true;  // Call save()
      testCases[3] = true;  // Call saveSystemCfg()
      expectedResult = AxisProfileResult::ERR_EXEC_NULL;
      missingClassName = "AxisProfileManager";
      log(L"\nCreate BackupRestoreMgr without AxisProfileManager.");
      dlClient = m_testDatalayerSystem.factory()->createClient();
      persistenceHelper = m_mockAxisProfileCmp.getPersistenceHelper();
      break;
    case 1:
      testCases[0] = true;  // Call loadCfg()
      testCases[1] = true;  // Call save()
      expectedResult = AxisProfileResult::ERR_EXEC_NULL;
      missingClassName = "DatalayerClient";
      log(L"\nCreate BackupRestoreMgr without DatalayerClient.");
      profileManager = m_mockAxisProfileCmp.getAxisProfileManager();
      persistenceHelper = m_mockAxisProfileCmp.getPersistenceHelper();
      break;
    case 2:
      testCases[0] = true; // Call loadCfg()
      testCases[1] = true; // Call save()
      testCases[2] = true; // Call loadSystemCfg()
      testCases[3] = true; // Call saveSystemCfg()
      expectedResult = AxisProfileResult::ERR_EXEC_NULL;
      missingClassName = "PersistenceHelper";
      log(L"\nCreate BackupRestoreMgr without PersistenceHelper.");
      profileManager = m_mockAxisProfileCmp.getAxisProfileManager();
      dlClient = m_testDatalayerSystem.factory()->createClient();
      break;
    case 3:
      testCases[0] = true; // Call loadCfg()
      testCases[1] = true; // Call save()
      expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
      missingClassName = "argument";
      log(L"\nCreate BackupRestoreMgr with all dependencies.");
      profileManager = m_mockAxisProfileCmp.getAxisProfileManager();
      dlClient = m_testDatalayerSystem.factory()->createClient();
      persistenceHelper = m_mockAxisProfileCmp.getPersistenceHelper();
      break;
    }

    BackupRestoreMgr testClassNullptr(profileManager, dlClient, persistenceHelper);

    if (testCases[0])
    {
      log(L"\nCall loadCfg() with missing %ls.", test::StringHelper::utf8ToWide(missingClassName).c_str());
      AxisProfileResult retVal = testClassNullptr.loadCfg({}, {});
      if (retVal != expectedResult)
      {
        error(L"\nWrong return code! Expected: 0x%08X Actual: 0x%08X", expectedResult, retVal);
        result = E_FAIL;
      }
    }
    if (testCases[1])
    {
      log(L"\nCall save() with missing %ls.", test::StringHelper::utf8ToWide(missingClassName).c_str());
      AxisProfileResult retVal = testClassNullptr.save({}, {});
      if (retVal != expectedResult)
      {
        error(L"\nWrong return code! Expected: 0x%08X Actual: 0x%08X", expectedResult, retVal);
        result = E_FAIL;
      }
    }
    if (testCases[2])
    {
      log(L"\nCall loadSystemCfg() with missing %ls.", test::StringHelper::utf8ToWide(missingClassName).c_str());
      AxisProfileResult retVal = testClassNullptr.loadSystemCfg("");
      if (retVal != expectedResult)
      {
        error(L"\nWrong return code! Expected: 0x%08X Actual: 0x%08X", expectedResult, retVal);
        result = E_FAIL;
      }
    }
    if (testCases[3])
    {
      log(L"\nCall saveSystemCfg() with missing %ls.", test::StringHelper::utf8ToWide(missingClassName).c_str());
      AxisProfileResult retVal = testClassNullptr.saveSystemCfg("");
      if (retVal != expectedResult)
      {
        error(L"\nWrong return code! Expected: 0x%08X Actual: 0x%08X", expectedResult, retVal);
        result = E_FAIL;
      }
    }

    if (index == 3)
    {
      result = resetTestEnvironment(dlClient);
      if (FAILED(result))
      {
        return E_FAIL;
      }
    }
  }

  log(L"\n");
  return result;
}

HRESULT TestBackupRestoreManager::testClass()
{
  log(L"\n<b>----- Test all class methods -----</b>");

  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCreate backup/restore manager.");
  comm::datalayer::IClient* datalayerClient = m_testDatalayerSystem.factory()->createClient();
  BackupRestoreMgr testClass(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClient, m_mockAxisProfileCmp.getPersistenceHelper());

  std::string emptyPath{ m_configFolderPath + "/empty" };  

  log(L"\nCall loadSystemCfg() when no configuration files can be found (no error).");
  AxisProfileResult retVal = testClass.loadSystemCfg( emptyPath );
  if (retVal != AxisProfileResult::STS_NO_CONFIG_FILES_FOUND)
  {
    error(L"\nWrong return code! Expected: 'STS_NO_CONFIG_FILES_FOUND' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall saveSystemCfg() when no profiles exist in the system (no error).");
  retVal = testClass.saveSystemCfg("");
  if (retVal != AxisProfileResult::STS_NO_PROFILES_EXIST)
  {
    error(L"\nWrong return code! Expected: 'STS_NO_PROFILES_EXIST' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall executeCfgLoad() and force browseDirectoryRegex() failure.");
  PersistenceHelperItf persistenceBrowseFail = *m_mockAxisProfileCmp.getPersistenceHelper();
  persistenceBrowseFail.browseDirectoryRegex = browseDirectoryRegexAlwaysFail;
  comm::datalayer::IClient* datalayerClientBrowseFail = m_testDatalayerSystem.factory()->createClient();
  BackupRestoreMgr testClassBrowseFail(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClientBrowseFail, &persistenceBrowseFail);
  retVal = testClassBrowseFail.executeCfgLoad("/forced/browse/failure", "load-id-persistence-error");
  if (retVal != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_PERSISTENCE' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall executeCfgLoad() and simulate deleteAxisProfile() failure.");
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "deleteFailProfile");
  static_cast<test::MockAxisProfileManager*>(m_mockAxisProfileCmp.getAxisProfileManager())->m_returnValue = AxisProfileResult::ERR_NOK;
  retVal = testClass.executeCfgLoad(emptyPath, "load-id-delete-fail");
  static_cast<test::MockAxisProfileManager*>(m_mockAxisProfileCmp.getAxisProfileManager())->m_returnValue = AxisProfileResult::STS_OK;
  if (retVal != AxisProfileResult::ERR_NOK)
  {
    error(L"\nWrong return code! Expected: 'ERR_NOK' Actual: 0x%08X", retVal);
    return E_FAIL;
  }
  m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile("deleteFailProfile");

  log(L"\nCall executeCfgLoad() with file '.json' to cover empty profile name branch.");
  const std::string emptyNameFolderPath = m_configFolderPath + "/generated/emptyProfileName";
  std::error_code fileEc;
  std::filesystem::create_directories(emptyNameFolderPath, fileEc);
  if (fileEc)
  {
    error(L"\nCould not create folder for empty profile-name test case.");
    return E_FAIL;
  }

  {
    std::ofstream emptyNameConfigFile(emptyNameFolderPath + "/.json", std::ios::out | std::ios::trunc);
    emptyNameConfigFile << "{}";
  }

  retVal = testClass.executeCfgLoad(emptyNameFolderPath, "load-id-empty-profile-name");
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nWrong return code! Expected: 'STS_OK' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  std::filesystem::remove_all(emptyNameFolderPath, fileEc);

  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(m_testProfileName).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_testProfileName);

  result = registerProfileConfigurationType();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  result = addInputOutputMapping();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCall save() when everything is OK.");
  std::string generatedFilePath = m_configFolderPath + "/generated/myConfigFile.json";
  retVal = testClass.save(m_testProfileName.c_str(), generatedFilePath.c_str());
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nMethod should succeed but received error code: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall load() return OK, although the invalid boot params data.");
  retVal = testClass.load(m_testProfileName.c_str(), generatedFilePath.c_str());
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nMethod should succeed but received error code: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nDelete profile with name '%ls' again.", test::StringHelper::utf8ToWide(m_testProfileName).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(m_testProfileName.c_str());

  log(L"\nCall loadCfg() when everything is OK (profile should be created!).");
  std::string validConfigFilePath = m_configFolderPath + "/correct/testProfile.json";
  retVal = testClass.load(m_testProfileName.c_str(), validConfigFilePath.c_str());
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nMethod should succeed but received error code: 0x%08X", retVal);
    return E_FAIL;
  }

  const IAxisProfile* testProfile = m_mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_testProfileName.c_str());
  if (!testProfile)
  {
    error(L"\nThe method loadCfg() did not create the requested profile!");
    return E_FAIL;
  }

  log(L"\nCall loadSystemCfg() when everything is OK.");
  std::string folderPathLoad = m_configFolderPath + "/correct";
  retVal = testClass.loadSystemCfg(folderPathLoad.c_str());
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nWrong return code! Expected: 'STS_OK' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  const std::string newProfileName = "needlessProfile";
  log(L"\nCreate new profile: %s", test::StringHelper::utf8ToWide(newProfileName).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "needlessProfile");

  log ( L"\nCall loadSystemCfg() when everything is OK." );
  retVal = testClass.loadSystemCfg( folderPathLoad.c_str() );
  if ( retVal != AxisProfileResult::STS_OK )
  {
    error( L"\nWrong return code! Expected: 'STS_OK' Actual: 0x%08X", retVal );
    return E_FAIL;
  }

  log(L"\nCheck if the profile '%s' has been deleted.", test::StringHelper::utf8ToWide(newProfileName).c_str());
  const IAxisProfile* checkProfile = m_mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(newProfileName.c_str());
  if (checkProfile)
  {
    error(L"\nThe profile '%s' still exists!", test::StringHelper::utf8ToWide(newProfileName).c_str());
    return E_FAIL;
  }

  log(L"\nCall saveSystemCfg() when everything is OK.");
  std::string folderPathSave = m_configFolderPath + "/generated";
  retVal = testClass.saveSystemCfg(folderPathSave.c_str());
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nWrong return code! Expected: 'STS_OK' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  result = resetTestEnvironment(datalayerClient);
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\n");
  return result;
}

HRESULT TestBackupRestoreManager::configureTestEnvironment()
{
  log(L"\nCreate the test environment.");

  if (!m_datalayerProvider)
  {
    m_datalayerProvider = m_testDatalayerSystem.factory()->createProvider();
  }
  if (!m_profilesConfigNode)
  {
    m_profilesConfigNode = new provider::NodeProfilesConfig(m_mockAxisProfileCmp.getProviderManager());
  }
  if (!m_bootParamNode)
  {
    m_bootParamNode = new provider::NodeProfilesBootParams(m_mockAxisProfileCmp.getProviderManager());
  }

  m_datalayerProvider->start();
  m_mockAxisProfileCmp.addDataLayer(m_testDatalayerSystem.factory());

  if (comm::datalayer::STATUS_FAILED(m_datalayerProvider->registerNode(provider::NodeTree.APR_PROFILES_CFG.c_str(), m_profilesConfigNode))/* ||
      comm::datalayer::STATUS_FAILED(m_datalayerProvider->registerNode(provider::NodeTree.APR_PROFILES_CFG_BOOTPARAMS.c_str(), m_bootParamNode))*/)
  {
    error(L"\nCould not register profile configuration node.");
    return E_FAIL;
  }

  return S_OK;
}

HRESULT TestBackupRestoreManager::registerProfileConfigurationType()
{
  log(L"\nRegister profile configuration type at the datalayer");
  comm::datalayer::DlResult configResult = m_datalayerProvider->registerType(m_profilesConfigNodeTypeAddress, m_configBfbsPath);
  comm::datalayer::DlResult bootParamResult = m_datalayerProvider->registerType(m_profilesBootParamTypeAddress, m_bootParamBfbsPath);
  
  if (comm::datalayer::STATUS_FAILED(configResult) || comm::datalayer::STATUS_FAILED(bootParamResult))
  {
    error(L"\nCould not register type at datalayer!");
    return E_FAIL;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return S_OK;
}

HRESULT TestBackupRestoreManager::resetTestEnvironment(comm::datalayer::IClient* client)
{
  log(L"\nReset the test environment.");

  comm::datalayer::Variant nodeList;
  if (comm::datalayer::STATUS_FAILED(client->readSync("datalayer/nodes", &nodeList)) || (nodeList.getType() != comm::datalayer::VariantType::ARRAY_OF_STRING))
  {
    error(L"\nCould not check registered datalayer nodes.");
    return E_FAIL;
  }

  const char** convActData = nodeList;
  std::string configNodeAddress = provider::NodeTree.APR_PROFILES_CFG;
  for (std::uint8_t idx = 0; idx < nodeList.getCount(); idx++)
  {
    if (m_profilesConfigNodeTypeAddress.compare(convActData[idx]) == 0 ||
        m_profilesBootParamTypeAddress.compare(convActData[idx]) == 0)
    {
      if (comm::datalayer::STATUS_FAILED(m_datalayerProvider->unregisterType(convActData[idx])))
      {
        error(L"\nCould not unregister profile configuration type.");
        return E_FAIL;
      }
    }
    if (configNodeAddress.compare(convActData[idx]) == 0 ||
        provider::NodeTree.APR_PROFILES_CFG_BOOTPARAMS.compare(convActData[idx]) == 0)
    {
      if (comm::datalayer::STATUS_FAILED(m_datalayerProvider->unregisterNode(convActData[idx])))
      {
        error(L"\nCould not unregister profile configuration node.");
        return E_FAIL;
      }
    }
  }

  const IAxisProfile* profile = m_mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_testProfileName.c_str());
  if (profile)
  {
    log(L"\nDelete axis profile '%ls'", test::StringHelper::utf8ToWide(m_testProfileName).c_str());
    m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(m_testProfileName.c_str());
  }

  m_datalayerProvider->stop();

  return S_OK;
}

HRESULT TestBackupRestoreManager::determinePathToTestConfigFolder()
{
  std::string path;
  m_mockAxisProfileCmp.getPersistenceHelperCppItf()->getRootPath(path);
  size_t findPos = path.find("generated");
  if (findPos == std::string::npos)
  {
    return E_FAIL;
  }

  path.erase((path.begin() + findPos), path.end());
  path += "src/test/unittest/configuration/files";

  if (!m_mockAxisProfileCmp.getPersistenceHelperCppItf()->directoryExists(path.c_str()))
  {
    return E_FAIL;
  }

  m_configFolderPath = path;
  return S_OK;
}

HRESULT TestBackupRestoreManager::determinePathToProfileConfigBfbsFile()
{
  std::string path;
  m_mockAxisProfileCmp.getPersistenceHelperCppItf()->getRootPath(path);
  size_t findPos = path.find("private");
  if (findPos == std::string::npos)
  {
    return E_FAIL;
  }

  path.erase((path.begin() + findPos), path.end());
  std::string configPath = path + "public/bfbs/comm/axisprofile/profileConfig.bfbs";
  std::string bootParamPath = path + "public/bfbs/comm/axisprofile/deviceParamSnapshot.bfbs";

  if (!m_mockAxisProfileCmp.getPersistenceHelperCppItf()->fileExists(configPath) ||
      !m_mockAxisProfileCmp.getPersistenceHelperCppItf()->fileExists(bootParamPath))
  {
    return E_FAIL;
  }

  m_configBfbsPath = configPath;
  m_bootParamBfbsPath = bootParamPath;
  return S_OK;
}

HRESULT TestBackupRestoreManager::addInputOutputMapping()
{
  log(L"\nConfigure input/output mapping for further testing.");

  IAxisProfile* profile = m_mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_testProfileName.c_str());
  if (!profile)
  {
    error(L"\nCould not add input/output configuration because %ls does not exist.", test::StringHelper::utf8ToWide(m_testProfileName).c_str());
    return E_FAIL;
  }

  MockAxisProfileMappingHandlerSpecial* mockMappingHandler = new MockAxisProfileMappingHandlerSpecial(); // Will be deleted by the axis profile
  static_cast<test::MockAxisProfile*>(profile)->setMappingHandler(true, mockMappingHandler);

  mockMappingHandler->m_inputMapping[InputMappingValueID::STATUSWORD] = "datalayer/myBundle/stsWord";
  mockMappingHandler->m_inputMapping[InputMappingValueID::POSITION]   = "datalayer/myBundle/position";
  mockMappingHandler->m_outputMapping[OutputMappingValueID::CONTROLWORD] = "datalayer/myBundle/ctrlWord";
  mockMappingHandler->m_outputMapping[OutputMappingValueID::POSITION]    = "datalayer/myBundle/position";

  return S_OK;
}

HRESULT TestBackupRestoreManager::testsaveSystemCfg()
{
  log(L"\n<b>----- Test transaction based saveSystemCfg() -----</b>");
  
  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCreate backup/restore manager.");
  comm::datalayer::IClient* datalayerClient = m_testDatalayerSystem.factory()->createClient();
  BackupRestoreMgr testClassSaveSystemCfg(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClient, m_mockAxisProfileCmp.getPersistenceHelper());

  const std::string testProfile1Name = "testProfile1";
  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(testProfile1Name).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfile1Name);

  const std::string testProfile2Name = "testProfile2";
  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(testProfile2Name).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfile2Name);

  flatbuffers::FlatBufferBuilder builder;
  comm::datalayer::Variant var;
  builder.ForceDefaults(true);
  auto fbPath = builder.CreateString("..... / 123 / ....");
  auto fbId = builder.CreateString("id123");
  auto fbPhase = builder.CreateString("");
  auto fb = comm::datalayer::CreatePersistenceParam(builder, fbPath, fbId, fbPhase);
  comm::datalayer::FinishPersistenceParamBuffer(builder, fb);
  var.copyFlatbuffers(builder);

  log(L"\nCall saveSystemCfg() and simulate error when saving a single profile (expect failure)...");
  AxisProfileResult retVal = testClassSaveSystemCfg.saveSystemCfg(var);
  if(retVal != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\ntranscation based save return unexpected result.");
    return E_FAIL;
  }

  comm::datalayer::Variant var2;
  retVal = testClassSaveSystemCfg.saveSystemCfg(var2);
  if(retVal != AxisProfileResult::ERR_INVALID_ARGUMENT)
  {
    error(L"\ntranscation based save return unexpected result.");
    return E_FAIL;
  }

  BackupRestoreMgr testClassSaveSystemCfg2(m_mockAxisProfileCmp.getAxisProfileManager(), nullptr, nullptr);
  retVal = testClassSaveSystemCfg2.saveSystemCfg(var2);
  if (retVal != AxisProfileResult::ERR_EXEC_NULL)
  {
    error(L"\ntranscation based save return unexpected result.");
    return E_FAIL;
  }

  result = resetTestEnvironment(datalayerClient);
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\n");

  return result;
}

HRESULT TestBackupRestoreManager::testloadSystemCfg()
{
  log(L"\n<b>----- Test transaction based load -----</b>");

  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }
  test::MockStateMachine stateMachine{};
  m_mockAxisProfileCmp.getAxisProfileManager()->getStateMachineStatusIF() = stateMachine.getStateMachineStatus();
  std::vector<std::string> profiles;
  (void)m_mockAxisProfileCmp.getAxisProfileManager()->getProfileNames(profiles);
  for(const auto &profile : profiles)
  {
    (void)m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(profile);
  }
  result = registerProfileConfigurationType();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  comm::datalayer::Variant persistenceParamLoad;
  flatbuffers::FlatBufferBuilder builder;
  std::string folderPathLoad = m_configFolderPath;
  auto offset = comm::datalayer::CreatePersistenceParamDirect(builder, folderPathLoad.c_str(), "load", comm::axisprofile::loadPhasetoString(LoadPhases::LOAD));
  builder.Finish(offset);
  (void)persistenceParamLoad.copyFlatbuffers(builder);
  (void)m_backupMgr->loadSystemCfg(persistenceParamLoad);

  result = addInputOutputMapping();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  for(const auto & testData : m_testData)
  {
    result |= executeLoadTestcase(testData);
  }

  delete m_backupMgr; //test miscellaneous

  comm::datalayer::IClient* datalayerClient = m_testDatalayerSystem.factory()->createClient();
  m_backupMgr = new BackupRestoreMgr(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClient, nullptr);

  for(const auto & testData : m_testData2)
  {
    result |= executeLoadTestcase(testData);
  }

  result |= resetTestEnvironment(datalayerClient);
  return result;
}

HRESULT TestBackupRestoreManager::executeLoadTestcase(const TestData &testData)
{
  // Generate placeholder variant for build
  HRESULT result = S_OK;
  comm::datalayer::Variant persistenceParamLoad;
  flatbuffers::FlatBufferBuilder builder;
  std::string folderPathLoad = m_configFolderPath + testData.configurationPath;
  auto offset = comm::datalayer::CreatePersistenceParamDirect(builder, folderPathLoad.c_str(), testData.loadId, comm::axisprofile::loadPhasetoString(testData.phase));
  builder.Finish(offset);
  persistenceParamLoad.copyFlatbuffers(builder);

  AxisProfileResult apResult  = m_backupMgr->loadSystemCfg(persistenceParamLoad);
  if(apResult != testData.expResult)
  {
    result = E_FAIL;
    error(L"\n %s test: transcation based load return unexpected result, expect: 0x%08X, actual: 0x%08X.", testData.m_testName, testData.expResult, apResult);
  }
  else
  {
    log(L"\n%s test is <DARK_GREEN> okay <DARK_GREEN>", testData.m_testName);
  }
  return result;
}

HRESULT TestBackupRestoreManager::testMakePersistent()
{
  log(L"\n<b>----- Test makePersistent() -----</b>");

  HRESULT result = configureTestEnvironment();
  if (FAILED(result))
  {
    return E_FAIL;
  }

  log(L"\nCreate backup/restore manager.");
  comm::datalayer::IClient* datalayerClient = m_testDatalayerSystem.factory()->createClient();
  BackupRestoreMgr testClassMakePersistent(m_mockAxisProfileCmp.getAxisProfileManager(), datalayerClient, m_mockAxisProfileCmp.getPersistenceHelper());

  const std::string testProfile1Name = "testProfile1";
  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(testProfile1Name).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfile1Name);

  const std::string testProfile2Name = "testProfile2";
  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(testProfile2Name).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfile2Name);

  flatbuffers::FlatBufferBuilder builder;
  comm::datalayer::Variant var;
  builder.ForceDefaults(true);
  auto fbPath = builder.CreateString("..... / 123 / ....");
  auto fbId = builder.CreateString("id123");
  auto fbPhase = builder.CreateString("");
  auto fb = comm::datalayer::CreatePersistenceParam(builder, fbPath, fbId, fbPhase);
  comm::datalayer::FinishPersistenceParamBuffer(builder, fb);
  var.copyFlatbuffers(builder);

  log(L"\nCall makePersistent() and simulate error when saving a single profile Phase error(expect failure)...");
  AxisProfileResult retVal = testClassMakePersistent.makePersistent(var, testProfile1Name);
  if (retVal != AxisProfileResult::ERR_INVALID_ARGUMENT)
  {
    error(L"\ntranscation based save return unexpected result.");
    return E_FAIL;
  }

  comm::datalayer::Variant var2;
  builder.Clear();
  fbPath = builder.CreateString("..... / 123 / ....");
  fbId = builder.CreateString("id123");
  fbPhase = builder.CreateString("SAVE");
  fb = comm::datalayer::CreatePersistenceParam(builder, fbPath, fbId, fbPhase);
  comm::datalayer::FinishPersistenceParamBuffer(builder, fb);
  var2.copyFlatbuffers(builder);

  log(L"\nCall makePersistent() and simulate error when saving a single profile (expect failure)...");
  retVal = testClassMakePersistent.makePersistent(var2, testProfile1Name);
  if (retVal != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\ntranscation based save return unexpected result.");
    return E_FAIL;
  }

  log(L"\nCall makePersistent() and simulate axisprofile name is absent...");
  retVal = testClassMakePersistent.makePersistent(var2, "");
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\ntranscation based save return unexpected result.");
    return E_FAIL;
  }


  log(L"\nCall makePersistent() with persistenceParam with wrong variant type");
  comm::datalayer::Variant var3;
  retVal = testClassMakePersistent.makePersistent(var3, "");
  if (retVal != AxisProfileResult::ERR_INVALID_ARGUMENT)
  {
    error(L"\nWrong return code! Expected: 'ERR_INVALID_ARGUMENT ' Actual: 0x%08X", retVal);
    return E_FAIL;
  }


  log(L"\nCall makePersistent() and simulate persistent is nullptr...");
  BackupRestoreMgr testClassMakePersistent2(m_mockAxisProfileCmp.getAxisProfileManager(), nullptr, nullptr);
  retVal = testClassMakePersistent2.makePersistent(var2, "");
  if (retVal != AxisProfileResult::ERR_EXEC_NULL)
  {
    error(L"\ntranscation based save return unexpected result.");
    return E_FAIL;
  }

  if (FAILED(resetTestEnvironment(datalayerClient)))
  {
    return E_FAIL;
  }

  log(L"\n");

  return result;
}
