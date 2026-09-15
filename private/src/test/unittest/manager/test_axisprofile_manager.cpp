#include "test_axisprofile_manager.h"
#include <string.h>
#include "bundle/i_axisprofile.h"
#include "profiles/i_axisprofile_internal.h"
#include "helper/datalayer_memory_map_helper.h"
#include "common/datalayer/client_internal.h"

// Mockups
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/external_bundles/mock_datalayer_factory.h"
#include "mockup/external_bundles/mock_datalayer_memory_user.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"

// Test Class
#include "manager/axisprofile_manager.h"
#include "helper/result_helper.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileManager);
REGISTER_TEST_TO_GROUP(TestAxisProfileManager, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileManager, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileManager, Manager);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

TestAxisProfileManager::TestAxisProfileManager()
{
}

TestAxisProfileManager::~TestAxisProfileManager(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestAxisProfileManager::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile manager");
  log(L"\n----------------------------------------------------------------");

  HRESULT hresult = S_OK;

  hresult |= testManagerInit();
  hresult |= testMemoryInit();
  hresult |= testMappingInit();
  hresult |= testFieldMgrInit();
  hresult |= testFieldbusMasterInstanceInit();
  hresult |= testAxisProfiles();
  hresult |= testProcessDataAt();
  hresult |= testProcessDataMdt();
  hresult |= testMethodErrorCases();
  hresult |= testProcessAuxJobs();
  hresult |= testValidateMappingCfg();
  hresult |= testExecExternalChecking();

  return hresult;
}

HRESULT TestAxisProfileManager::testManagerFieldbusMgrInit()
{
  log(L"\n<b>Test the initialization of the fieldbus manager.</b>");
  HRESULT hresult = S_OK;
  AxisProfileManager testManager{};
  
  AxisProfileResult result = testManager.initDeviceInfoMgr();
  AxisProfileResult expectedResult = AxisProfileResult::ERR_EXEC_NULL;
  if ( result != expectedResult )
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }
  return hresult;
}

HRESULT TestAxisProfileManager::testManagerInit()
{
  log(L"\n<b>Test the initialization of the axisprofile manager");
  HRESULT hresult = S_OK;

  // order of object creation must be as followed, because the manager needs the factory in the destructor
  // 1. MockMemoryUser as shared pointer
  // 2. MockDatalayerFactory
  // 3. TestDeviceManager
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDL;
  testFactoryDL.setMemoryUser(testUserObject);
  AxisProfileManager testManager{};

  log(L"\nCall initManager() without a datalayer.");
  AxisProfileResult result = testManager.initManager(m_abortBoot);
  AxisProfileResult expectedResult = AxisProfileResult::ERR_EXEC_NULL;
  if ( result != expectedResult )
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nSet the datalayer (with createClient() error simulation).");
  testFactoryDL.simulateCreateClientError(true);
  testManager.setDataLayer(&testFactoryDL, nullptr);

  log(L"\nCall initManager() with datalayer but simulate out of memory (client creation fails).");
  result = testManager.initManager(m_abortBoot);
  expectedResult = AxisProfileResult::ERR_EXEC_NULL;
  if (result != expectedResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nSet the datalayer.");
  testFactoryDL.simulateCreateClientError(false);
  testManager.setDataLayer(&testFactoryDL, nullptr);
  test::MockEthercatMasterInstancesNode mockEcatNode{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;

  log(L"\nCall initManager() without any memory configuration.");
  result = testManager.initManager(m_abortBoot);
  expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"Method should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nAdd an axis profile for further testing.");
  std::string testProfileName = "testProfile";
  testManager.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfileName);

  testManager.setProfileInputBuffer(testProfileName.c_str(), MOCK_ECAT_INPUTBUFFER_STRING);
  testManager.setProfileOutputBuffer(testProfileName.c_str(), MOCK_ECAT_OUTPUTBUFFER_STRING);

  log(L"\nCall initManager() and simulate an error when opening datalayer RT memory.");
  testFactoryDL.setReturnValue(comm::datalayer::DlResult::DL_FAILED);
  testManager.setDataLayer(&testFactoryDL, nullptr);
  result = testManager.initManager(m_abortBoot);
  expectedResult = AxisProfileResult::ERR_DATALAYER;
  if ( result != expectedResult )
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nCall initManager() and simulate a wrong memory map (verifyFlatbuffers() fail).");
  testFactoryDL.setReturnValue(comm::datalayer::DlResult::DL_OK);
  result = testManager.initManager(m_abortBoot);
  expectedResult = AxisProfileResult::ERR_DLRT;
  if ( result != expectedResult )
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  if(testManager.getStateMachineStatusIF())
  {
    error(L"Return should not be null pointer.");
  }

  printTestCaseResult(hresult);
  return hresult;
}

HRESULT TestAxisProfileManager::testMemoryInit()
{
  log(L"\n<b>Test the initialization of the axisprofile memory");
  HRESULT hresult = S_OK;

  // order of object creation must be as followed, because the manager needs the factory in the destructor
  // 1. MockMemoryUser as shared pointer
  // 2. MockDatalayerFactory
  // 3. TestDeviceManager
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDL;
  testFactoryDL.setMemoryUser(testUserObject);
  AxisProfileManager testManager{};
  testManager.setDataLayer(&testFactoryDL, nullptr);

  log(L"\nAdd two axis profiles for further testing.");
  HRESULT resulAddProfile = addProfileToManager(&testManager, 1, "testProfile1", MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  resulAddProfile |= addProfileToManager(&testManager, 2, "testProfile2", MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  if ( S_OK != resulAddProfile )
  {
    error(L"Add profiles to manager failed. End testcase");
    return E_FAIL;
  }

  log(L"\nCall initMemory() with existing profiles.");
  AxisProfileResult result = testManager.initMemory();
  AxisProfileResult expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"Method should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  printTestCaseResult(hresult);

  log(L"\n<b>Test the initialization of the axisprofile memory with an empty buffer");
  hresult = S_OK;

  // order of object creation must be as followed, because the manager needs the factory in the destructor
  // 1. MockMemoryUser as shared pointer
  // 2. MockDatalayerFactory
  // 3. TestDeviceManager
  std::shared_ptr<test::MockMemoryUser> testUserObjectEmpty = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDLEmpty;
  testFactoryDLEmpty.setMemoryUser(testUserObjectEmpty);
  AxisProfileManager testManagerEmpty{};
  testManagerEmpty.setDataLayer(&testFactoryDL, nullptr);

  std::string invaliProfile = "invalidProfile";

  log(L"\nAdd a profile with empty buffer address.");
  AxisProfileResult resultCreate = testManagerEmpty.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, invaliProfile);
  if (aprStatusFailed(resultCreate))
  {
    error(L"Create profiles to manager failed. End testcase");
    return E_FAIL;
  }

  log(L"\nCall initMemory() with invalid profiles.");
  result = testManagerEmpty.initMemory();
  expectedResult = AxisProfileResult::ERR_PROFILE_CONFIG;
  if (result != expectedResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  testManagerEmpty.setProfileInputBuffer(invaliProfile.c_str(), MOCK_ECAT_INPUTBUFFER_STRING);
  result = testManagerEmpty.initMemory();
  expectedResult = AxisProfileResult::ERR_PROFILE_CONFIG;
  if (result != expectedResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  testFactoryDL.setReturnValue(comm::datalayer::DlResult::DL_FAILED);
  log(L"\nCall initMemory().");
  result = testManagerEmpty.initMemory();
  expectedResult = AxisProfileResult::ERR_DATALAYER;
  if ( result != expectedResult )
  {
    error(L"Method should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  printTestCaseResult(hresult);  

  return hresult;
}

HRESULT TestAxisProfileManager::testMappingInit()
{
  log(L"\n<b>Test the initialization of the axisprofile mapping");
  HRESULT hresult = S_OK;

  // order of object creation must be as followed, because the manager needs the factory in the destructor
  // 1. MockMemoryUser as shared pointer
  // 2. MockDatalayerFactory
  // 3. TestDeviceManager
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDL;
  testFactoryDL.setMemoryUser(testUserObject);

  log(L"\nCreate memory map for further testing.");
  test::MemoryMapHelper memMapHelper;
  auto& memMap = memMapHelper.createMemoryMap(100);
  testUserObject->setMemoryMap(&memMap);

  AxisProfileManager testManager{};
  testManager.setDataLayer(&testFactoryDL, nullptr);

  log(L"\nAdd an axis profile for further testing.");
  HRESULT resulAddProfile = addProfileToManager(&testManager, 1, "testProfile1", MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  if ( S_OK != resulAddProfile )
  {
    error(L"Add profiles to manager failed.");
    return E_FAIL;
  }

  log(L"\nFirst of all call initMemory() for initialization.");
  if ( AxisProfileResult::STS_OK != testManager.initMemory() )
  {
    error(L"Initialization of Memory for initMapping failed.");
    return  E_FAIL;
  }

  log(L"\nCall initMapping() and simulate an abort of the waiting loop.");
  m_abortBoot = true;
  testUserObject.get()->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::GET_MEMMAP, comm::datalayer::DlResult::Value::DL_RT_INVALIDMEMORYMAP);
  AxisProfileResult result = testManager.initMapping(m_abortBoot);
  AxisProfileResult expectedResult = AxisProfileResult::STS_BOOTING_ABORT;
  if (result != expectedResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nCall initMapping() and simulate an error in the memory user (getMemoryMap() fails).");
  m_abortBoot = false;
  testUserObject.get()->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::GET_MEMMAP, comm::datalayer::DlResult::Value::DL_FAILED);
  result = testManager.initMapping(m_abortBoot);
  expectedResult = AxisProfileResult::ERR_DLRT;
  if (result != expectedResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nReset memory user error.");
  testUserObject.get()->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::GET_MEMMAP, comm::datalayer::DlResult::Value::DL_OK);

  log(L"\nCall initMapping().");
  result = testManager.initMapping(m_abortBoot);
  expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"Method should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  printTestCaseResult(hresult);

  return hresult;
}

HRESULT TestAxisProfileManager::testFieldMgrInit()
{
  log(L"\n<b>Test the initialization of the fieldbus manager.</b>");
  HRESULT hresult = S_OK;

  AxisProfileManager testManager{};
  testManager.initDeviceInfoMgr();
  return hresult;
}

HRESULT TestAxisProfileManager::testFieldbusMasterInstanceInit()
{
  log(L"\n<b>Test the initialization of the fieldbus master instance data.</b>");
  HRESULT hresult = S_OK;

  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDL;
  testFactoryDL.setMemoryUser(testUserObject);
  AxisProfileManager testManager{};
  testManager.setDataLayer(&testFactoryDL, nullptr);

  std::string errorProfileName = "errorProfile";
  log(L"\nAdd profile '%ls' with wrong config to manager.", UTF8TOWIDE(errorProfileName));
  HRESULT resulAddProfile = addProfileToManager(&testManager, 1, errorProfileName.c_str(), MOCK_ECAT_INPUTBUFFER_STRING, "error/out/buffer");
  if (S_OK != resulAddProfile)
  {
    error(L"Add profiles to manager failed. End testcase");
    return E_FAIL;
  }

  //init fieldbus manager
  testManager.initDeviceInfoMgr();

  log(L"\nCall browseFieldbusMasterInstance() and simulate wrong profile config error.");
  bool abortInitialization{};
  test::MockEthercatMasterInstancesNode mockEcatNode{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;
  AxisProfileResult result = testManager.browseFieldbusMasterInstance(abortInitialization);
  AxisProfileResult expectedResult = AxisProfileResult::ERR_PROFILE_CONFIG;
  if (result != expectedResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nDelete profile '%ls' with wrong configuration.", UTF8TOWIDE(errorProfileName));
  delete testManager.m_mapAxisProfiles[errorProfileName].m_axisProfile;
  testManager.m_mapAxisProfiles.erase(errorProfileName);

  log(L"\nAdd correct profile '%ls' to manager.", UTF8TOWIDE(m_profileName));
  resulAddProfile = addProfileToManager(&testManager, 1, m_profileName.c_str(), MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  if (S_OK != resulAddProfile)
  {
    error(L"Add profiles to manager failed. End testcase");
    return E_FAIL;
  }

  log(L"\nCall browseFieldbusMasterInstance() and simulate datalayer browse error.");
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = nullptr;
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  result = testManager.browseFieldbusMasterInstance(abortInitialization);
  expectedResult = AxisProfileResult::ERR_DATALAYER_BROWSE_NODE;
  if (result != expectedResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nCall browseFieldbusMasterInstance() when everything is OK.");
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_returnValue = comm::datalayer::DlResult::DL_OK;
  result = testManager.browseFieldbusMasterInstance(abortInitialization);
  expectedResult = AxisProfileResult::STS_OK;
  if (result != expectedResult)
  {
    error(L"Method should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  return hresult;
}

HRESULT TestAxisProfileManager::testProcessDataAt()
{
  log(L"\n<b>Test the processing of the actual values.");
  HRESULT hresult = S_OK;
  
  // order of object creation must be as followed, because the manager needs the factory in the destructor
  // 1. MockMemoryUser as shared pointer
  // 2. MockDatalayerFactory
  // 3. TestDeviceManager
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDL;
  testFactoryDL.setMemoryUser(testUserObject);
  AxisProfileManager testManager{};
  testManager.setDataLayer(&testFactoryDL, nullptr);

  // process data without registered profiles (nothing to do)
  AxisProfileResult result = testManager.processDataAt();
  AxisProfileResult expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"processDataAt did fail but should succeed without any work todo. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // create profile so processing has something to do
  HRESULT resulAddProfile = addProfileToManager(&testManager, 1, "testProfile1", MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  if ( S_OK != resulAddProfile )
  {
    error(L"Add profiles to manager failed. End testcase");
    return E_FAIL;
  }

  //init fieldbus manager
  testManager.initDeviceInfoMgr();

  log(L"\nCall browseFieldbusMasterInstance() when everything is OK.");
  bool abortInitialization{};
  test::MockEthercatMasterInstancesNode mockEcatNode{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_returnValue = comm::datalayer::DlResult::DL_OK;
  result = testManager.browseFieldbusMasterInstance(abortInitialization);
  expectedResult = AxisProfileResult::STS_OK;
  if (result != expectedResult)
  {
    error(L"Method should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  log(L"\nCreate memory map for further testing.");
  test::MemoryMapHelper memMapHelper;
  auto& memMap = memMapHelper.createMemoryMap(100);
  testUserObject->setMemoryMap(&memMap);

  // initialize axisProfile manger
  test::MockEthercatMasterParamRequestNodeSoE paramRequestNodeMockSoE{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &paramRequestNodeMockSoE;
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_returnValue = comm::datalayer::DlResult::DL_OK;
  if (aprStatusFailed(testManager.initManager(abortInitialization)))
  {
    error(L"Initialization of device manager failed.");
    return E_FAIL;
  }

  // process data with error in end access to memory
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::END_ACCESS, comm::datalayer::DlResult::DL_FAILED);
  result = testManager.processDataAt();
  expectedResult = AxisProfileResult::ERR_DATALAYER;
  if ( result != expectedResult )
  {
    error(L"processDataAt did not fail as expected in end access. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // process data successful to reset internal variable
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::END_ACCESS, comm::datalayer::DlResult::DL_OK);
  result = testManager.processDataAt();
  expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"processDataAt did fail but should work. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // process data with error in begin access to memory
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::BEGIN_ACCESS, comm::datalayer::DlResult::DL_FAILED);
  result = testManager.processDataAt();
  expectedResult = AxisProfileResult::ERR_DATALAYER;
  if ( result != expectedResult )
  {
    error(L"processDataAt did not fail as expected in begin access. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}

HRESULT TestAxisProfileManager::testProcessDataMdt()
{
  log(L"\n<b>Test the processing of the commanded values.");
  HRESULT hresult = S_OK;
  // order of object creation must be as followed, because the manager needs the factory in the destructor
  // 1. MockMemoryUser as shared pointer
  // 2. MockDatalayerFactory
  // 3. TestDeviceManager
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDL;
  testFactoryDL.setMemoryUser(testUserObject);
  AxisProfileManager testManager{};
  testManager.setDataLayer(&testFactoryDL, nullptr);

  // process data successful without registered profiles (nothing to do)
  AxisProfileResult result = testManager.processDataMdt();
  AxisProfileResult expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"processDataMdt did fail but should work. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // create profile so processing has something to do
  HRESULT resulAddProfile = addProfileToManager(&testManager, 1, "testProfile1", MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  if ( S_OK != resulAddProfile )
  {
    error(L"Add profiles to manager failed. End testcase");
    return E_FAIL;
  }

  // just init the memory here not the mapping, because mapping is not needed for the unittest and in fact part of the profile not the manager
  if ( aprStatusFailed(testManager.initMemory()) )
  {
    error(L"Initialization of device manager failed.");
    return E_FAIL;
  }

  // process data with error in end access to memory
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::END_ACCESS, comm::datalayer::DlResult::DL_FAILED);
  result = testManager.processDataMdt();
  expectedResult = AxisProfileResult::ERR_DATALAYER;
  if ( result != expectedResult )
  {
    error(L"processDataMdt did not fail as expected in end access. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // process data successful to reset internal variable.
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::END_ACCESS, comm::datalayer::DlResult::DL_OK);
  result = testManager.processDataMdt();
  expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"processDataMdt did fail but should work. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // process data with error in begin access to memory
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::BEGIN_ACCESS, comm::datalayer::DlResult::DL_FAILED);
  result = testManager.processDataMdt();
  expectedResult = AxisProfileResult::ERR_DATALAYER;
  if ( result != expectedResult )
  {
    error(L"processDataMdt did not fail as expected in begin access. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}

HRESULT TestAxisProfileManager::testAxisProfiles()
{
  log(L"\n<b>Test the creation and deletion of axis profiles.");
  HRESULT hresult = S_OK;
  // order of object creation must be as followed, because the manager needs the factory in the destructor
  // 1. MockMemoryUser as shared pointer
  // 2. MockDatalayerFactory
  // 3. TestDeviceManager
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDL;
  testFactoryDL.setMemoryUser(testUserObject);
  AxisProfileManager testManager{};
  testManager.setDataLayer(&testFactoryDL, nullptr);
  test::MockEthercatMasterInstancesNode mockEcatNode{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;

  if ( aprStatusFailed(testManager.initManager(m_abortBoot)) )
  {
    error(L"Initialization of device manager failed.");
    return E_FAIL;
  }

  // create profile with an empty string
  std::string emptyProfileName = "";
  AxisProfileResult result = testManager.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, emptyProfileName);
  AxisProfileResult expectedResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  if (result != expectedResult)
  {
    error(L"createAxisProfile did not return invalid argument. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // create profile of a non existing type
  std::string testProfileName = "testProfile";
  result = testManager.createAxisProfile(comm::axisprofile::AxisProfileType::UNDEFINED, testProfileName);
  expectedResult = AxisProfileResult::ERR_NOTIMPLEMENTED;
  if ( result != expectedResult )
  {
    error(L"createAxisProfile did not return expected status. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // create profile successful
  result = testManager.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfileName);
  expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"createAxisProfile did fail but should succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  // Try to get a non existing profile
  const IAxisProfile* profileRequest = testManager.getProfileByName("NotValid");
  if ( profileRequest )
  {
    error(L"getProfileName returns a profile, that not exists.");
    hresult = E_FAIL;
  }

  // Try to get the testprofile
  profileRequest = testManager.getProfileByName(testProfileName.c_str());
  if (!profileRequest)
  {
    error(L"getProfileName did not return the expected profile.");
    hresult = E_FAIL;
  }

  // delete profile successful
  result = testManager.deleteAxisProfile("testProfile");
  expectedResult = AxisProfileResult::STS_OK;
  if ( result != expectedResult )
  {
    error(L"deleteAxisProfile did fail but should succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}

HRESULT TestAxisProfileManager::testMethodErrorCases()
{  
  HRESULT hresult = S_OK;

  // Initialize Mockups
  const size_t sizeOfMemory = 20;
  std::shared_ptr<test::MockMemoryUser> testUserObject = std::unique_ptr<test::MockMemoryUser>(new test::MockMemoryUser(sizeOfMemory));
  test::MockDatalayerFactory testFactoryDL;
  testFactoryDL.setMemoryUser(testUserObject);
  AxisProfileManager testManager{};
  testManager.setDataLayer(&testFactoryDL, nullptr);

  log(L"\n<b>Test error cases for setProfileInputBuffer/setProfileOutputBuffer.");
  AxisProfileResult result    = testManager.setProfileInputBuffer(nullptr, nullptr);
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  if ( expResult != result )
  {
    error(L"setProfileInputBuffer did not return 'ERR_EXEC_NULL' for nullptr profileName. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  result = testManager.setProfileInputBuffer("doesNotExist", "doesAlsoNotExist");
  expResult = AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  if ( expResult != result )
  {
    error(L"setProfileInputBuffer did not return 'ERR_PROFILE_NOT_FOUND' for nullptr profileName. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  result = testManager.setProfileOutputBuffer(nullptr, nullptr);
  expResult = AxisProfileResult::ERR_EXEC_NULL;
  if (expResult != result)
  {
    error(L"setProfileOutputBuffer did not return 'ERR_EXEC_NULL' for nullptr profileName. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  result = testManager.setProfileOutputBuffer("doesNotExist", "doesAlsoNotExist");
  expResult = AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  if (expResult != result)
  {
    error(L"setProfileOutputBuffer did not return 'ERR_PROFILE_NOT_FOUND' for nullptr profileName. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  std::string testProfileName = "testProfile";
  testManager.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfileName);

  result = testManager.setProfileInputBuffer(testProfileName.c_str(), nullptr);
  expResult = AxisProfileResult::ERR_EXEC_NULL;
  if ( expResult != result )
  {
    error(L"setProfileInputBuffer did not return invalid argument for nullptr buffer string. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  result = testManager.setProfileOutputBuffer(testProfileName.c_str(), nullptr);
  expResult = AxisProfileResult::ERR_EXEC_NULL;
  if ( expResult != result )
  {
    error(L"setProfileOutputBuffer did not return invalid argument for nullptr buffer string. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  char test[] = "";
  result = testManager.setProfileInputBuffer(testProfileName.c_str(), test);
  expResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  if ( expResult != result )
  {
    error(L"setProfileInputBuffer did not return invalid argument for empty buffer string. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  result = testManager.setProfileOutputBuffer(testProfileName.c_str(), test);
  expResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  if ( expResult != result )
  {
    error(L"setProfileOutputBuffer did not return invalid argument for empty buffer string. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  log(L"\n<b>Test errorcases for getProfileInputBuffer/getProfileOutputBuffer.");
  const char* testBuffer = testManager.getProfileInputBuffer(nullptr);
  if ( testBuffer != nullptr )
  {
    error(L"getProfileInputBuffer did not return a nullptr for invalid input.");
    hresult = E_FAIL;
  }

  testBuffer = testManager.getProfileOutputBuffer(nullptr);
  if ( testBuffer != nullptr )
  {
    error(L"getProfileOutputBuffer did not return a nullptr for invalid input.");
    hresult = E_FAIL;
  }

  log(L"\n<b>Test errorcases for deleteProfile.");
  // delete profile successful
  result = testManager.deleteAxisProfile({});
  expResult = AxisProfileResult::ERR_INVALID_ARGUMENT;
  if ( result != expResult )
  {
    error(L"deleteAxisProfile did not return invalid argument for nullptr. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  }

  result = testManager.deleteAxisProfile("noProfile");
  expResult = AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  if ( result != expResult )
  {
    error(L"deleteAxisProfile did not return profile not found. Expected: 0x%X, Actual: 0x%X", expResult, result);
    hresult = E_FAIL;
  } 

  return hresult; 
}

HRESULT TestAxisProfileManager::addProfileToManager(comm::axisprofile::IAxisProfileManagerInternal* manager, std::uint32_t deviceAddress, const char* profileName, const char* inputAddress, const char* outputAdress)
{
  StateMachineState state {StateMachineState::CONFIG};
  manager->getStateMachineStatusIF() = &state;
  if ( aprStatusFailed(manager->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName)))
  {
    return E_FAIL;
  }
  if (aprStatusFailed(manager->setProfileInputBuffer(profileName, inputAddress)))
  {
    return E_FAIL;
  }
  if (aprStatusFailed(manager->setProfileOutputBuffer(profileName, outputAdress)))
  {
    return E_FAIL;
  }
  std::shared_ptr<const IAxisProfileC> newProfile{};
  if (aprStatusFailed(manager->connectAxisProfile(profileName, newProfile)))
  {
    return E_FAIL;
  }
  auto protertyIf = newProfile.get()->getPropertyInterface(newProfile.get()->handle);
  if (aprStatusFailed(protertyIf->setDeviceAddress(protertyIf->handle, deviceAddress)))
  {
    return E_FAIL;
  }

  return S_OK;
}

inline void TestAxisProfileManager::printTestCaseResult(HRESULT result)
{
  if (result == S_OK)
  {
    log(L"\nTest Case <DARK_GREEN>SUCCEEDED</DARK_GREEN>");
  }
  else
  {
    log(L"\nTest Case <DARK_RED>FAILED</DARK_RED>");
  }
}

HRESULT TestAxisProfileManager::testProcessAuxJobs()
{
  log(L"\n<b>Test the processAuxJobs.");
  HRESULT hresult = S_OK;
  AxisProfileManager testManager {};

  log(L"\nCall processAuxJobs() when everything is OK.");
  AxisProfileResult result = testManager.processAuxJobs();
  AxisProfileResult expectedResult = AxisProfileResult::STS_OK;
  if (result != expectedResult)
  {
    error(L"Method should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    hresult = E_FAIL;
  }

  return hresult;
}

HRESULT TestAxisProfileManager::testValidateMappingCfg()
{
  log(L"\n<b>Test the ValidateMappingCfg.</b>");
  HRESULT hresult = S_OK;
  AxisProfileManager testManager{};

  //test for correct case
  log(L"\n<BLUE>Test the correct case.</BLUE>");
  std::string testProfileName = "testProfile";
  testManager.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfileName);
  IAxisProfileInternal* pProfile = static_cast<IAxisProfileInternal*>( testManager.getProfileByName(testProfileName.c_str()));
  pProfile->getPropertyInterface()->setDeviceName("testDevice");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::POSITION, "testDevice/input/position");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice/input/statusword");
  pProfile->getMappingHandler()->configureOutputValue(OutputMappingValueID::POSITION, "testDevice/output/position");
  pProfile->getMappingHandler()->configureOutputValue(OutputMappingValueID::CONTROLWORD, "testDevice/output/controlword");

  AxisProfileResult expectedResult = AxisProfileResult::STS_OK;
  AxisProfileResult result = testManager.validateMappingCfg();
  LOG_TEST_RESULT(expectedResult, L"Method should have succeeded.", result);
  
  //test for incosistence input mapping device name
  log(L"\n<BLUE>Test the input mapping incosistence case.</BLUE>");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice1/input/statusword");
  result = testManager.validateMappingCfg();
  expectedResult = AxisProfileResult::ERR_PROFILE_CONFIG;
  LOG_TEST_RESULT(expectedResult, L"Method should have succeeded.", result);
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice/input/statusword");

  //test for incosistence output mapping device name
  log(L"\n<BLUE>Test the output mapping incosistence case.</BLUE>");
  pProfile->getMappingHandler()->configureOutputValue(OutputMappingValueID::CONTROLWORD, "testDevice1/output/controlword");
  result = testManager.validateMappingCfg();
  expectedResult = AxisProfileResult::ERR_PROFILE_CONFIG;
  LOG_TEST_RESULT(expectedResult, L"Method should have succeeded.", result);
  pProfile->getMappingHandler()->configureOutputValue(OutputMappingValueID::CONTROLWORD, "testDevice/output/controlword");

  //test for duplicate mapping items
  log(L"\n<BLUE>Test the duplicate mapping items case.</BLUE>");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::POSITION,   "testDevice/input/statusword");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice/input/statusword");
  result = testManager.validateMappingCfg();
  expectedResult = AxisProfileResult::ERR_PROFILE_CONFIG;
  LOG_TEST_RESULT(expectedResult, L"Method should have succeeded.", result);

  //test for empty items
  log(L"\n<BLUE>Test the empty mapping items case.</BLUE>");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::POSITION, "");
  result = testManager.validateMappingCfg();
  expectedResult = AxisProfileResult::STS_OK;
  LOG_TEST_RESULT(expectedResult, L"Method should have succeeded.", result);

  //test for IO mapping items
  log(L"\n<BLUE>Test the duplicate mapping items case.</BLUE>");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::DIAGMSG, "testDevice/input/DIAGMSG");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice/input/statusword");
  result = testManager.validateMappingCfg();
  expectedResult = AxisProfileResult::STS_OK;
  LOG_TEST_RESULT(expectedResult, L"Method should have succeeded.", result);

  return hresult;
}

HRESULT TestAxisProfileManager::testExecExternalChecking()
{
  log(L"\n<b>Test case of executing external checking for profiles.</b>");
  HRESULT result = S_OK;
  AxisProfileManager testManager{};

  log(L"\n<BLUE>Test of external checking.</BLUE>");
  std::string testProfile = "testProfile";
  testManager.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfile);
  testManager.createAxisProfile(comm::axisprofile::AxisProfileType::CANOPEN_OVER_ETHERCAT, "testDisabledCase");

  bool profileChecked = false;
  auto checkProfile = [&](const std::string& profileName)
  {
    profileChecked = true;
    return profileName == testProfile? CheckResponse::OK_USE_PROFILE : CheckResponse::OK_DONT_USE_PROFILE;
  };
  auto res = testManager.registerCheckCallback(checkProfile, "testBundle");
  LOG_TEST_RESULT(AxisProfileResult::STS_OK, L"Register external checking function of profile", res);
  res = testManager.execExternalChecking();
  LOG_TEST_RESULT_ANY(res == AxisProfileResult::STS_OK && profileChecked == true, L"Execute external checking function");
  std::vector<IAxisProfile*> profiles;
  testManager.getActivatedProfiles(profiles);
  LOG_TEST_RESULT_ANY(!profiles.empty(), L"Result of external checking is enabled.");
  res = testManager.unregisterAllCheckCallbacks("testBundle");
  LOG_TEST_RESULT(AxisProfileResult::STS_OK, L"Unregister external checking function of profile", res);

  return result;
}
