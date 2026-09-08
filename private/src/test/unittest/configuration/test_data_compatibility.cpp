#include "test_data_compatibility.h"
#include "provider/nodes/profiles/cfg/node_profiles_config.h"
#include "comm/datalayer/persistence_param_generated.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileDataCompatibility);
REGISTER_TEST_TO_GROUP(TestAxisProfileDataCompatibility, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileDataCompatibility, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileDataCompatibility, Configuration);

// Constructor/Destructor
TestAxisProfileDataCompatibility::TestAxisProfileDataCompatibility() : m_testData{    
    {L"Compatibility for motion 1.12", "/data_compatibility/motion1.12/correct", "load1.12", LoadPhases::LOAD, AxisProfileResult::STS_OK},
    {L"Compatibility for motion 1.14", "/data_compatibility/motion1.14/correct", "load1.14", LoadPhases::LOAD, AxisProfileResult::STS_OK},
    {L"Compatibility for motion 1.16", "/data_compatibility/motion1.16/correct", "load1.16", LoadPhases::LOAD, AxisProfileResult::STS_OK},
    {L"Compatibility for motion 1.18", "/data_compatibility/motion1.18/correct", "load1.18", LoadPhases::LOAD, AxisProfileResult::STS_OK},
    {L"Compatibility for motion 1.18", "/data_compatibility/motion1.18/incorrect", "load1.18error", LoadPhases::LOAD, AxisProfileResult::ERR_DATALAYER_WRONG_TYPE},
    {L"Compatibility for motion 5.2" , "/data_compatibility/motion5.2/correct", "load5.2", LoadPhases::LOAD, AxisProfileResult::STS_OK},
}
{

}

TestAxisProfileDataCompatibility::~TestAxisProfileDataCompatibility()
{
  if ( m_datalayerProvider )  { delete m_datalayerProvider; m_datalayerProvider = nullptr; }
  if ( m_profilesConfigNode ) { delete m_profilesConfigNode; m_profilesConfigNode = nullptr; }
  if ( m_pPersistenceHelper ) { delete m_pPersistenceHelper; m_pPersistenceHelper = nullptr; }
  if ( m_backupMgr )          { delete m_backupMgr; m_backupMgr = nullptr; }

  m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(m_testProfileName.c_str());
  m_testDatalayerSystem.stop();
}

HRESULT TestAxisProfileDataCompatibility::doInit()
{
  /// prepare for test interface
  log(L"\n");
  m_testDatalayerSystem.start();
  m_mockAxisProfileCmp.addDataLayer(m_testDatalayerSystem.factory());
  m_pPersistenceHelper = new comm::datalayer::PersistenceHelper(&m_testDatalayerSystem);
  m_pPersistenceHelper->start();
  m_mockAxisProfileCmp.setPersistenceHelper(m_pPersistenceHelper);
  m_backupMgr = new BackupRestoreMgr(m_mockAxisProfileCmp.getAxisProfileManager(), m_testDatalayerSystem.factory()->createClient(), m_mockAxisProfileCmp.getPersistenceHelper());

  return S_OK;
}

HRESULT TestAxisProfileDataCompatibility::doTest()
{
  log(L"\n----------------------------------------------------------------");
  log(L"\n Test the compatibility of the axis profile data");
  log(L"\n----------------------------------------------------------------\n");

  log(L"\nGet path to profile configuration flatbuffer Bfbs file.");
  HRESULT hresult = determinePathToProfileConfigBfbsFile();
  if ( FAILED(hresult) )
  {
    error(L"\nCould not get path to profile configuration flatbuffer Bfbs file. Aborting test...");
    return E_FAIL;
  }

  log(L"\nGet test configuration folder path (location of Json files).");
  hresult = determinePathToTestConfigFolder();
  if ( FAILED(hresult) )
  {
    error(L"\nCould not get test configuration files. Aborting test...");
    return E_FAIL;
  }

  // test task
  hresult |= testloadSystemCfg();

  return hresult;
}

HRESULT TestAxisProfileDataCompatibility::configureTestEnvironment()
{
  log(L"\nCreate the test environment.");

  if ( !m_datalayerProvider )
  {
    m_datalayerProvider = m_testDatalayerSystem.factory()->createProvider();
  }
  if ( !m_profilesConfigNode )
  {
    m_profilesConfigNode = new provider::NodeProfilesConfig(m_mockAxisProfileCmp.getProviderManager());
  }

  m_datalayerProvider->start();
  m_mockAxisProfileCmp.addDataLayer(m_testDatalayerSystem.factory());

  if ( comm::datalayer::STATUS_FAILED(m_datalayerProvider->registerNode(provider::NodeTree.APR_PROFILES_CFG.c_str(), m_profilesConfigNode)) )
  {
    error(L"\nCould not register profile configuration node.");
    return E_FAIL;
  }

  return S_OK;
}

HRESULT TestAxisProfileDataCompatibility::registerProfileConfigurationType()
{
  log(L"\nRegister profile configuration type at the datalayer");
  comm::datalayer::DlResult dlresult = m_datalayerProvider->registerType(m_profilesConfigNodeTypeAddress, m_configBfbsPath);
  if ( comm::datalayer::STATUS_FAILED(dlresult) )
  {
    error(L"\nCould not register type at datalayer!");
    return E_FAIL;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return S_OK;
}

HRESULT TestAxisProfileDataCompatibility::determinePathToTestConfigFolder()
{
  std::string path;
  m_mockAxisProfileCmp.getPersistenceHelperCppItf()->getRootPath(path);
  size_t findPos = path.find("generated");
  if ( findPos == std::string::npos )
  {
    return E_FAIL;
  }

  path.erase(( path.begin() + findPos ), path.end());
  path += "src/test/unittest/configuration/files";

  if ( !m_mockAxisProfileCmp.getPersistenceHelperCppItf()->directoryExists(path.c_str()) )
  {
    return E_FAIL;
  }

  m_configFolderPath = path;
  return S_OK;
}

HRESULT TestAxisProfileDataCompatibility::determinePathToProfileConfigBfbsFile()
{
  std::string path;
  m_mockAxisProfileCmp.getPersistenceHelperCppItf()->getRootPath(path);
  size_t findPos = path.find("private");
  if ( findPos == std::string::npos )
  {
    return E_FAIL;
  }

  path.erase(( path.begin() + findPos ), path.end());
  path += "public/bfbs/comm/axisprofile/profileConfig.bfbs";

  if ( !m_mockAxisProfileCmp.getPersistenceHelperCppItf()->fileExists(path.c_str()) )
  {
    return E_FAIL;
  }

  m_configBfbsPath = path;
  return S_OK;
}

HRESULT TestAxisProfileDataCompatibility::testloadSystemCfg()
{
  log(L"\n<b>----- Test transaction based load -----</b>");
  HRESULT result = configureTestEnvironment();
  if ( FAILED(result) )
  {
    return E_FAIL;
  }

  // register 
  result = registerProfileConfigurationType();
  if ( FAILED(result) )
  {
    return E_FAIL;
  }

  // test for compatibility
  for ( const auto& testData : m_testData )
  {
    result |= executeLoadTestcase(testData);
  }

  return result;
}

HRESULT TestAxisProfileDataCompatibility::executeLoadTestcase(const TestData& testData)
{
  // Generate placeholder variant for build  
  flatbuffers::FlatBufferBuilder builder;
  std::string folderPathLoad = m_configFolderPath + testData.configurationPath;
  auto offset = comm::datalayer::CreatePersistenceParamDirect(builder, folderPathLoad.c_str(), testData.loadId, comm::axisprofile::loadPhasetoString(testData.phase));
  builder.Finish(offset);

  comm::datalayer::Variant persistenceParamLoad;
  persistenceParamLoad.copyFlatbuffers(builder);

  AxisProfileResult apResult = m_backupMgr->loadSystemCfg(persistenceParamLoad);
  if ( apResult != testData.expResult )
  {
    error(L"\n %s test: transaction based load return unexpected result, expect: 0x%08X, actual: 0x%08X.", testData.m_testName, testData.expResult, apResult);
    error(L"\n %s test case not compatible", testData.m_testName);
    return E_FAIL;
  }

  log(L"\n%s test is <DARK_GREEN> okay <DARK_GREEN>", testData.m_testName);
  return S_OK;
}
