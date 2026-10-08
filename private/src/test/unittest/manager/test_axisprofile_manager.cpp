#include "test_axisprofile_manager.h"
#include "bundle/i_axisprofile.h"
#include "profiles/i_axisprofile_internal.h"
#include "helper/datalayer_memory_map_helper.h"
#include "common/datalayer/client_internal.h"
#include "configuration/i_boot_param_backup_restore_mgr.h"
#include "profiles/category/drive/axisprofile_device_param_drive.h"
#include "profiles/data_sharing/sharing_handler.h"

// Mockups
#include "mockup/external_bundles/mock_datalayer_factory.h"
#include "mockup/external_bundles/mock_datalayer_memory_user.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"
#include "../../mockup/mock_axisprofile.h"
#include "../../mockup/mock_axisprofile_property.h"
#include "../../mockup/mock_device_boot_parm_snapshot.h"

// Test Class
#include "manager/axisprofile_manager.h"
#include "helper/result_helper.h"


using namespace comm::axisprofile;

namespace
{
class MockBootParamBackupRestoreMgr final : public IBootParamBackupRestoreMgr
{
public:
  AxisProfileResult loadBootParams(const std::string& /*profileName*/) const override
  {
    return m_loadResult;
  }

  AxisProfileResult saveBootParam(const std::string& profileName) const override
  {
    ++m_totalSaveCalls;
    ++m_saveCallsByProfile[profileName];
    return m_saveResult;
  }

  std::uint32_t getTotalSaveCalls() const
  {
    return m_totalSaveCalls;
  }

  std::uint32_t getSaveCallsByProfile(const std::string& profileName) const
  {
    auto it = m_saveCallsByProfile.find(profileName);
    return it == m_saveCallsByProfile.end() ? 0U : it->second;
  }

  AxisProfileResult m_loadResult {AxisProfileResult::STS_OK};
  AxisProfileResult m_saveResult {AxisProfileResult::STS_OK};

private:
  mutable std::uint32_t m_totalSaveCalls {};
  mutable std::map<std::string, std::uint32_t> m_saveCallsByProfile {};
};

class MockAxisProfileDeviceParamDrive final : public AxisProfileDeviceParamDrive
{
public:
  explicit MockAxisProfileDeviceParamDrive(ProfileBasicInfo& profileBasicInfo, IAxisProfilePropertyInternal* propertyIf)
    : AxisProfileDeviceParamDrive(profileBasicInfo, nullptr, nullptr)
    , m_sharingHandler(&m_profileBasicInfo.m_profileTrace)
  {
    m_sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, propertyIf);
    AxisProfileDeviceParamDrive::setSharingHandler(&m_sharingHandler);
  }

  void setNeedReadBootParams(bool needReadBootParams)
  {
    m_needReadBootParams = needReadBootParams;
  }

  void loadBootingTasks() override
  {
    m_initDeviceParam.clear();
  }

  OpModeAvailInfo* getOpModeAvailInfo() override
  {
    return &m_opModeAvailInfo;
  }

  AxisProfileResult getChecksumIdnList(std::vector<std::string>& idnStrListRet) const override
  {
    idnStrListRet.clear();
    return AxisProfileResult::STS_OK;
  }

  bool checkFunctionSupported(const FunctionSupport /*function*/, FunctionSupportErrorInfo& /*errorInfo*/) override
  {
    return true;
  }

  AxisProfileResult checkStandstill(bool& standStill) override
  {
    standStill = true;
    return AxisProfileResult::STS_OK;
  }

private:
  AxisProfileResult getDeviceInformation() override
  {
    return AxisProfileResult::STS_OK;
  }

  bool checkBootIdnListModified() override
  {
    return false;
  }

  AxisProfileResult readChecksumValueFromDevice(std::uint32_t& checkSum) override
  {
    checkSum = 0;
    return AxisProfileResult::STS_OK;
  }

  OpModeAvailInfo m_opModeAvailInfo {true, true, true};
  AxisProfileSharingHandler m_sharingHandler;
};

class MockAxisProfileForBootParamSave final : public test::MockAxisProfile
{
public:
  explicit MockAxisProfileForBootParamSave(const std::string& name)
    : test::MockAxisProfile(name, AxisProfileType::SERVODRIVE_OVER_ETHERCAT)
    , m_driveParam(m_profileBasicInfo, &m_property)
    , m_snapshot(m_profileBasicInfo)
  {
    // Ensure checksum capability path is reachable in manager auto-save tests.
    m_property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
    m_property.setSoftwareVersion(255U, 0U);
  }

  IAxisProfileDeviceParamInternal* getDeviceParamInterface() override
  {
    return &m_driveParam;
  }

  IAxisProfilePropertyInternal* getPropertyInterface() override
  {
    return &m_property;
  }

  IDeviceBootParamSnapshot* getDeviceBootParmSnapshotInterface() override
  {
    return &m_snapshot;
  }

  MockAxisProfileDeviceParamDrive& getDriveParamMock()
  {
    return m_driveParam;
  }

  test::MockAxisProfileProperty& getPropertyMock()
  {
    return m_property;
  }

  test::MockDeviceBootParmSnapshot& getSnapshotMock()
  {
    return m_snapshot;
  }

private:
  test::MockAxisProfileProperty m_property;
  MockAxisProfileDeviceParamDrive m_driveParam;
  test::MockDeviceBootParmSnapshot m_snapshot;
};
}

REGISTER_TEST(TestAxisProfileManager);
REGISTER_TEST_TO_GROUP(TestAxisProfileManager, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileManager, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileManager, Manager);


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
  hresult |= testForCollectAllDeviceInfo();
  hresult |= testAxisProfiles();
  hresult |= testProcessDataAt();
  hresult |= testProcessDataMdt();
  hresult |= testMethodErrorCases();
  hresult |= testProcessAuxJobs();
  hresult |= testValidateMappingCfg();
  hresult |= testSaveBootParamsIfNeed();
  hresult |= testExecExternalChecking();

  return hresult;
}

HRESULT TestAxisProfileManager::testManagerFieldbusMgrInit()
{
  log(L"\n<b>Test the initialization of the fieldbus manager.</b>");
  HRESULT hresult = S_OK;
  AxisProfileManager testManager{};  
  LOG_TEST_RESULT(testManager.initDeviceInfoMgr(), L"Method should have failed.", AxisProfileResult::ERR_EXEC_NULL);
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
  LOG_TEST_RESULT(testManager.initManager(m_abortBoot), L"Method should have failed.", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nSet the datalayer (with createClient() error simulation).");
  testFactoryDL.simulateCreateClientError(true);
  testManager.setDataLayer(&testFactoryDL, nullptr);

  log(L"\nCall initManager() with datalayer but simulate out of memory (client creation fails).");
  LOG_TEST_RESULT(testManager.initManager(m_abortBoot), L"Method should have failed.", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nSet the datalayer.");
  testFactoryDL.simulateCreateClientError(false);
  testManager.setDataLayer(&testFactoryDL, nullptr);
  test::MockEthercatMasterInstancesNode mockEcatNode{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;

  log(L"\nCall initManager() without any memory configuration.");
  LOG_TEST_RESULT(testManager.initManager(m_abortBoot), L"Method should have failed.", AxisProfileResult::STS_OK);

  log(L"\nAdd an axis profile for further testing.");
  std::string testProfileName = "testProfile";
  testManager.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfileName);

  testManager.setProfileInputBuffer(testProfileName.c_str(), MOCK_ECAT_INPUTBUFFER_STRING);
  testManager.setProfileOutputBuffer(testProfileName.c_str(), MOCK_ECAT_OUTPUTBUFFER_STRING);

  log(L"\nCall initManager() and simulate an error when opening datalayer RT memory.");
  testFactoryDL.setReturnValue(comm::datalayer::DlResult::DL_FAILED);
  testManager.setDataLayer(&testFactoryDL, nullptr);
  LOG_TEST_RESULT(testManager.initManager(m_abortBoot), L"Method should have failed.", AxisProfileResult::ERR_DATALAYER);

  log(L"\nCall initManager() and simulate a wrong memory map (verifyFlatbuffers() fail).");
  testFactoryDL.setReturnValue(comm::datalayer::DlResult::DL_OK);
  LOG_TEST_RESULT(testManager.initManager(m_abortBoot), L"Method should have failed.", AxisProfileResult::ERR_DLRT);

  LOG_TEST_RESULT_PTR(testManager.getStateMachineStatusIF(), L"Return should not be null pointer.", nullptr);

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
  RETURN_TEST_RESULT(resulAddProfile, L"Add profiles to manager failed. End testcase", S_OK);

  log(L"\nCall initMemory() with existing profiles.");
  LOG_TEST_RESULT(testManager.initMemory(), L"Method should have succeeded.", AxisProfileResult::STS_OK);

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
  RETURN_TEST_RESULT(resultCreate, L"Create profiles to manager failed. End testcase", S_OK);

  log(L"\nCall initMemory() with invalid profiles.");
  LOG_TEST_RESULT(testManagerEmpty.initMemory(), L"Method should have failed.", AxisProfileResult::ERR_PROFILE_CONFIG);

  testManagerEmpty.setProfileInputBuffer(invaliProfile.c_str(), MOCK_ECAT_INPUTBUFFER_STRING);
  LOG_TEST_RESULT(testManagerEmpty.initMemory(), L"Method should have failed.", AxisProfileResult::ERR_PROFILE_CONFIG);

  testFactoryDL.setReturnValue(comm::datalayer::DlResult::DL_FAILED);
  log(L"\nCall initMemory().");
  LOG_TEST_RESULT(testManagerEmpty.initMemory(), L"Method should have succeeded.", AxisProfileResult::ERR_DATALAYER);

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
  RETURN_TEST_RESULT(resulAddProfile, L"Add profiles to manager failed.", S_OK);

  log(L"\nFirst of all call initMemory() for initialization.");
  RETURN_TEST_RESULT(testManager.initMemory(), L"Initialization of Memory for initMapping failed.", AxisProfileResult::STS_OK);

  log(L"\nCall initMapping() and simulate an abort of the waiting loop.");
  m_abortBoot = true;
  testUserObject.get()->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::GET_MEMMAP, comm::datalayer::DlResult::Value::DL_RT_INVALIDMEMORYMAP);
  RETURN_TEST_RESULT(testManager.initMapping(m_abortBoot), L"Method should have failed.", AxisProfileResult::STS_BOOTING_ABORT);

  log(L"\nCall initMapping() and simulate an error in the memory user (getMemoryMap() fails).");
  m_abortBoot = false;
  testUserObject.get()->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::GET_MEMMAP, comm::datalayer::DlResult::Value::DL_FAILED);
  RETURN_TEST_RESULT(testManager.initMapping(m_abortBoot), L"Method should have failed.", AxisProfileResult::ERR_DLRT);

  log(L"\nReset memory user error.");
  testUserObject.get()->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::GET_MEMMAP, comm::datalayer::DlResult::Value::DL_OK);

  log(L"\nCall initMapping().");
  RETURN_TEST_RESULT(testManager.initMapping(m_abortBoot), L"Method should have succeed.", AxisProfileResult::STS_OK);

  printTestCaseResult(hresult);

  return hresult;
}

HRESULT TestAxisProfileManager::testFieldMgrInit()
{
  log(L"\n<b>Test the initialization of the fieldbus manager.</b>");
  HRESULT hresult = S_OK;
  AxisProfileManager testManager{};
  LOG_TEST_RESULT(testManager.initDeviceInfoMgr(), L"Failed to initialize the fieldbus manager", AxisProfileResult::STS_OK);
  return hresult;
}

HRESULT TestAxisProfileManager::testForCollectAllDeviceInfo()
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
  RETURN_TEST_RESULT(resulAddProfile, L"Add profiles to manager failed. End testcase", S_OK);

  //init fieldbus manager
  testManager.initDeviceInfoMgr();

  log(L"\nCall validateFieldbusMasterInfo() and simulate wrong profile config error.");
  test::MockEthercatMasterInstancesNode mockEcatNode{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;
  testManager.collectAllDeviceInfo(false);
  AxisProfileResult result = testManager.validateFieldbusMasterInfo();
  LOG_TEST_RESULT(result, L"Method should have failed.", AxisProfileResult::ERR_PROFILE_CONFIG);

  log(L"\nDelete profile '%ls' with wrong configuration.", UTF8TOWIDE(errorProfileName));
  delete testManager.m_mapAxisProfiles[errorProfileName].m_axisProfile;
  testManager.m_mapAxisProfiles.erase(errorProfileName);

  log(L"\nAdd correct profile '%ls' to manager.", UTF8TOWIDE(m_profileName));
  resulAddProfile = addProfileToManager(&testManager, 1, m_profileName.c_str(), MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  RETURN_TEST_RESULT(resulAddProfile, L"Add profiles to manager failed. End testcase", S_OK);

  log(L"\nCall validateFieldbusMasterInfo() and simulate datalayer browse error.");
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = nullptr;
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  result = testManager.collectAllDeviceInfo(false);
  LOG_TEST_RESULT(result, L"Method should have failed.", AxisProfileResult::ERR_DATALAYER_BROWSE_NODE);

  log(L"\nCall validateFieldbusMasterInfo() when everything is OK.");
  auto profile = testManager.getProfileByName(m_profileName.c_str());
  auto internalProfile = static_cast<IAxisProfileInternal*>(profile);
  internalProfile->getPropertyInterface()->setEtherCATName(MOCK_ECAT_MASTER_INSTANCE_NAME);
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_returnValue = comm::datalayer::DlResult::DL_OK;
  testManager.collectAllDeviceInfo(false);
  result = testManager.validateFieldbusMasterInfo();
  LOG_TEST_RESULT(result, L"Method should have failed.", AxisProfileResult::STS_OK);

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
  LOG_TEST_RESULT(result, L"processDataAt did fail but should succeed without any work todo.", AxisProfileResult::STS_OK);

  // create profile so processing has something to do
  HRESULT resulAddProfile = addProfileToManager(&testManager, 1, "testProfile1", MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  RETURN_TEST_RESULT(resulAddProfile, L"Add profiles to manager failed. End testcase", S_OK);

  //init fieldbus manager
  testManager.initDeviceInfoMgr();

  log(L"\nCall validateFieldbusMasterInfo() when everything is OK.");
  bool abortInitialization{};
  test::MockEthercatMasterInstancesNode mockEcatNode{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &mockEcatNode;
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_returnValue = comm::datalayer::DlResult::DL_OK;
  auto profile = testManager.getProfileByName("testProfile1");
  auto internalProfile = static_cast<IAxisProfileInternal*>(profile);
  internalProfile->getPropertyInterface()->setEtherCATName(MOCK_ECAT_MASTER_INSTANCE_NAME);
  testManager.collectAllDeviceInfo(false);
  result = testManager.validateFieldbusMasterInfo();
  LOG_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nCreate memory map for further testing.");
  test::MemoryMapHelper memMapHelper;
  auto& memMap = memMapHelper.createMemoryMap(100);
  testUserObject->setMemoryMap(&memMap);

  // initialize axisProfile manger
  test::MockEthercatMasterParamRequestNodeSoE paramRequestNodeMockSoE{};
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_testNode = &paramRequestNodeMockSoE;
  static_cast<test::MockDatalayerClient*>(testManager.m_datalayerClient->getBase())->m_returnValue = comm::datalayer::DlResult::DL_OK;
  RETURN_TEST_RESULT(testManager.initManager(abortInitialization), L"Add profiles to manager failed. End testcase", S_OK);

  // process data with error in end access to memory
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::END_ACCESS, comm::datalayer::DlResult::DL_FAILED);
  LOG_TEST_RESULT(testManager.processDataAt(), L"\nprocessDataAt did not fail as expected in end access.", AxisProfileResult::ERR_DATALAYER);

  // process data successful to reset internal variable
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::END_ACCESS, comm::datalayer::DlResult::DL_OK);
  LOG_TEST_RESULT(testManager.processDataAt(), L"\nprocessDataAt did fail but should work.", AxisProfileResult::STS_OK);

  // process data with error in begin access to memory
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::BEGIN_ACCESS, comm::datalayer::DlResult::DL_FAILED);
  LOG_TEST_RESULT(testManager.processDataAt(), L"\nprocessDataAt did not fail as expected in begin access.", AxisProfileResult::ERR_DATALAYER);

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
  LOG_TEST_RESULT(result, L"processDataMdt did fail but should work.", AxisProfileResult::STS_OK);

  // create profile so processing has something to do
  HRESULT resulAddProfile = addProfileToManager(&testManager, 1, "testProfile1", MOCK_ECAT_INPUTBUFFER_STRING, MOCK_ECAT_OUTPUTBUFFER_STRING);
  RETURN_TEST_RESULT(resulAddProfile, L"Add profiles to manager failed. End testcase", S_OK);

  // just init the memory here not the mapping, because mapping is not needed for the unittest and in fact part of the profile not the manager
  RETURN_TEST_RESULT(testManager.initMemory(), L"Failed to init manager. End testcase", AxisProfileResult::STS_OK);

  // process data with error in end access to memory
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::END_ACCESS, comm::datalayer::DlResult::DL_FAILED);
  LOG_TEST_RESULT(testManager.processDataMdt(), L"processDataMdt did not fail as expected in end access", AxisProfileResult::ERR_DATALAYER);

  // process data successful to reset internal variable.
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::END_ACCESS, comm::datalayer::DlResult::DL_OK);
  LOG_TEST_RESULT(testManager.processDataMdt(), L"processDataMdt did fail but should work", AxisProfileResult::STS_OK);

  // process data with error in begin access to memory
  testUserObject->setReturnValue(test::MockMemoryUser::ReturnValueIdentifier::BEGIN_ACCESS, comm::datalayer::DlResult::DL_FAILED);
  LOG_TEST_RESULT(testManager.processDataMdt(), L"processDataMdt did not fail as expected in begin access", AxisProfileResult::ERR_DATALAYER);

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
  RETURN_TEST_RESULT(testManager.initManager(m_abortBoot), L"Failed to init manager. End testcase", AxisProfileResult::STS_OK);

  // create profile with an empty string
  std::string emptyProfileName = "";
  AxisProfileResult result = testManager.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, emptyProfileName);
  LOG_TEST_RESULT(result, L"createAxisProfile did not return invalid argument", AxisProfileResult::ERR_INVALID_ARGUMENT);

  // create profile of a non existing type
  std::string testProfileName = "testProfile";
  result = testManager.createAxisProfile(comm::axisprofile::AxisProfileType::UNDEFINED, testProfileName);
  LOG_TEST_RESULT(result, L"createAxisProfile did not return expected status.", AxisProfileResult::ERR_NOTIMPLEMENTED);

  // create profile successful
  result = testManager.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfileName);
  LOG_TEST_RESULT(result, L"createAxisProfile did fail but should succeeded.", AxisProfileResult::STS_OK);

  // Try to get a non existing profile
  const IAxisProfile* profileRequest = testManager.getProfileByName("NotValid");
  LOG_TEST_RESULT_PTR(profileRequest, L"getProfileName returns a profile, that not exists.", nullptr);

  // Try to get the testprofile
  profileRequest = testManager.getProfileByName(testProfileName.c_str());
  LOG_TEST_RESULT_SET_PTR(profileRequest, L"getProfileName did not return the expected profile.");

  // delete profile successful
  result = testManager.deleteAxisProfile("testProfile");
  LOG_TEST_RESULT(result, L"deleteAxisProfile did fail but should succeeded.", AxisProfileResult::STS_OK);

  printTestCaseResult(hresult);

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
  AxisProfileResult result = testManager.setProfileInputBuffer(nullptr, nullptr);
  LOG_TEST_RESULT(result, L"setProfileInputBuffer did not return 'ERR_EXEC_NULL' for nullptr profileName.", AxisProfileResult::ERR_EXEC_NULL);

  result = testManager.setProfileInputBuffer("doesNotExist", "doesAlsoNotExist");
  LOG_TEST_RESULT(result, L"setProfileInputBuffer did not return 'ERR_PROFILE_NOT_FOUND' for nullptr profileName.", AxisProfileResult::ERR_PROFILE_NOT_FOUND);
  
  result = testManager.setProfileOutputBuffer(nullptr, nullptr);
  LOG_TEST_RESULT(result, L"setProfileOutputBuffer did not return 'ERR_EXEC_NULL' for nullptr profileName.", AxisProfileResult::ERR_EXEC_NULL);

  result = testManager.setProfileOutputBuffer("doesNotExist", "doesAlsoNotExist");
  LOG_TEST_RESULT(result, L"setProfileOutputBuffer did not return 'ERR_PROFILE_NOT_FOUND' for nullptr profileName.", AxisProfileResult::ERR_PROFILE_NOT_FOUND);

  std::string testProfileName = "testProfile";
  testManager.createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, testProfileName);

  char test[] = "";
  result = testManager.setProfileInputBuffer(testProfileName.c_str(), test);
  LOG_TEST_RESULT(result, L"setProfileInputBuffer did not return 'invalid argument' for empty buffer string.", AxisProfileResult::ERR_INVALID_ARGUMENT);

  result = testManager.setProfileOutputBuffer(testProfileName.c_str(), test);
  LOG_TEST_RESULT(result, L"setProfileOutputBuffer did not return 'invalid argument' for empty buffer string.", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\n<b>Test errorcases for getProfileInputBuffer/getProfileOutputBuffer.");
  const char* testBuffer = testManager.getProfileInputBuffer(nullptr);
  LOG_TEST_RESULT_PTR(testBuffer, L"getProfileInputBuffer did not return a nullptr for invalid input.", nullptr);

  testBuffer = testManager.getProfileOutputBuffer(nullptr);
  LOG_TEST_RESULT_PTR(testBuffer, L"getProfileOutputBuffer did not return a nullptr for invalid input.", nullptr);

  log(L"\n<b>Test errorcases for deleteProfile.");
  // delete profile successful
  result = testManager.deleteAxisProfile({});
  LOG_TEST_RESULT(result, L"deleteAxisProfile did not return invalid argument for nullptr.", AxisProfileResult::ERR_INVALID_ARGUMENT);

  result = testManager.deleteAxisProfile("noProfile");
  LOG_TEST_RESULT(result, L"deleteAxisProfile did not return profile not found.", AxisProfileResult::ERR_PROFILE_NOT_FOUND);

  printTestCaseResult(hresult);

  return hresult; 
}

HRESULT TestAxisProfileManager::testSaveBootParamsIfNeed()
{
  log(L"\n<b>Test auto-save behavior of saveBootParamsIfNeed().");
  HRESULT hresult = S_OK;

  AxisProfileManager testManager{};
  MockBootParamBackupRestoreMgr mockBackupRestoreMgr {};
  testManager.setBootParamBackupRestoreMgr(&mockBackupRestoreMgr);

  std::unique_ptr<MockAxisProfileForBootParamSave> profile(new MockAxisProfileForBootParamSave("saveProfile"));

  // Case 1: checksum changed -> force read -> dirty snapshot, should save.
  profile->getPropertyMock().enableChecksum(true);
  profile->getDriveParamMock().setNeedReadBootParams(true);
  profile->getSnapshotMock().setSnapshotDirty(true);
  testManager.saveBootParamsIfNeed(profile.get(), "saveProfile");
  if (mockBackupRestoreMgr.getSaveCallsByProfile("saveProfile") != 1U)
  {
    error(L"\nExpected one save call for checksum-changed case.");
    hresult = E_FAIL;
  }

  // Case 2: checksum unchanged -> no read required, should not save.
  profile->getDriveParamMock().setNeedReadBootParams(false);
  profile->getSnapshotMock().setSnapshotDirty(true);
  testManager.saveBootParamsIfNeed(profile.get(), "saveProfile");
  if (mockBackupRestoreMgr.getSaveCallsByProfile("saveProfile") != 1U)
  {
    error(L"\nSave should not be triggered when no re-read is needed.");
    hresult = E_FAIL;
  }

  // Case 3: read failed -> snapshot not dirty, should not save.
  profile->getDriveParamMock().setNeedReadBootParams(true);
  profile->getSnapshotMock().setSnapshotDirty(false);
  testManager.saveBootParamsIfNeed(profile.get(), "saveProfile");
  if (mockBackupRestoreMgr.getSaveCallsByProfile("saveProfile") != 1U)
  {
    error(L"\nSave should not be triggered when snapshot is not dirty.");
    hresult = E_FAIL;
  }

  // Case 4: save failed -> dirty state must be kept for retry.
  mockBackupRestoreMgr.m_saveResult = AxisProfileResult::ERR_DATALAYER_PERSISTENCE;
  profile->getSnapshotMock().setSnapshotDirty(true);
  testManager.saveBootParamsIfNeed(profile.get(), "saveProfile");
  if (mockBackupRestoreMgr.getSaveCallsByProfile("saveProfile") != 2U)
  {
    error(L"\nExpected save attempt count to increase in save-failed case.");
    hresult = E_FAIL;
  }
  if (!profile->getSnapshotMock().isSnapshotDirty())
  {
    error(L"\nSnapshot dirty flag should stay true after save failure.");
    hresult = E_FAIL;
  }

  // Case 5: multi-profile trigger should be independent per profile.
  mockBackupRestoreMgr.m_saveResult = AxisProfileResult::STS_OK;
  std::unique_ptr<MockAxisProfileForBootParamSave> profileA(new MockAxisProfileForBootParamSave("profileA"));
  std::unique_ptr<MockAxisProfileForBootParamSave> profileB(new MockAxisProfileForBootParamSave("profileB"));

  profileA->getPropertyMock().enableChecksum(true);
  profileA->getDriveParamMock().setNeedReadBootParams(true);
  profileA->getSnapshotMock().setSnapshotDirty(true);

  profileB->getPropertyMock().enableChecksum(true);
  profileB->getDriveParamMock().setNeedReadBootParams(false);
  profileB->getSnapshotMock().setSnapshotDirty(true);

  testManager.saveBootParamsIfNeed(profileA.get(), "profileA");
  testManager.saveBootParamsIfNeed(profileB.get(), "profileB");

  if (mockBackupRestoreMgr.getSaveCallsByProfile("profileA") != 1U)
  {
    error(L"\nProfileA should trigger one save call.");
    hresult = E_FAIL;
  }
  if (mockBackupRestoreMgr.getSaveCallsByProfile("profileB") != 0U)
  {
    error(L"\nProfileB should not trigger save.");
    hresult = E_FAIL;
  }

  return hresult;
}

HRESULT TestAxisProfileManager::addProfileToManager(comm::axisprofile::IAxisProfileManagerInternal* manager, std::uint32_t deviceAddress, const char* profileName, const char* inputAddress, const char* outputAdress)
{
  StateMachineState state {StateMachineState::CONFIG};
  manager->getStateMachineStatusIF() = &state;
  auto result = manager->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName);
  RETURN_TEST_FAILED(result, L"\nCreate test axisprofile failed.");

  result = manager->setProfileInputBuffer(profileName, inputAddress);
  RETURN_TEST_FAILED(result, L"\nFailed to set input buffer.");

  result = manager->setProfileOutputBuffer(profileName, outputAdress);
  RETURN_TEST_FAILED(result, L"\nFailed to set Output buffer.");

  std::shared_ptr<const IAxisProfileC> newProfile{};
  result = manager->connectAxisProfile(profileName, newProfile);
  RETURN_TEST_FAILED(result, L"\nFailed to connect AxisProfile.");

  auto protertyIf = newProfile.get()->getPropertyInterface(newProfile.get()->handle);
  RETURN_TEST_FAILED(protertyIf->setDeviceAddress(protertyIf->handle, deviceAddress), L"\nFailed to set device address.");

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
  LOG_TEST_RESULT(testManager.processAuxJobs(), L"Method should have succeeded.", AxisProfileResult::STS_OK);

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

  LOG_TEST_RESULT(testManager.validatePdoCfg(), L"Method should have succeeded.", AxisProfileResult::STS_OK);
  
  //test for incosistence input mapping device name
  log(L"\n<BLUE>Test the input mapping incosistence case.</BLUE>");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice1/input/statusword");
  LOG_TEST_RESULT(testManager.validatePdoCfg(), L"Method should have succeeded.", AxisProfileResult::ERR_PROFILE_CONFIG);

  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice/input/statusword");
  //test for incosistence output mapping device name
  log(L"\n<BLUE>Test the output mapping incosistence case.</BLUE>");
  pProfile->getMappingHandler()->configureOutputValue(OutputMappingValueID::CONTROLWORD, "testDevice1/output/controlword");
  LOG_TEST_RESULT(testManager.validatePdoCfg(), L"Method should have succeeded.", AxisProfileResult::ERR_PROFILE_CONFIG);

  pProfile->getMappingHandler()->configureOutputValue(OutputMappingValueID::CONTROLWORD, "testDevice/output/controlword");
  //test for duplicate mapping items
  log(L"\n<BLUE>Test the duplicate mapping items case.</BLUE>");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::POSITION,   "testDevice/input/statusword");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice/input/statusword");
  LOG_TEST_RESULT(testManager.validatePdoCfg(), L"Method should have succeeded.", AxisProfileResult::ERR_PROFILE_CONFIG);

  //test for empty items
  log(L"\n<BLUE>Test the empty mapping items case.</BLUE>");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::POSITION, "");
  LOG_TEST_RESULT(testManager.validatePdoCfg(), L"Method should have succeeded.", AxisProfileResult::STS_OK);

  //test for IO mapping items
  log(L"\n<BLUE>Test the duplicate mapping items case.</BLUE>");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::DIAGMSG, "testDevice/input/DIAGMSG");
  pProfile->getMappingHandler()->configureInputValue(InputMappingValueID::STATUSWORD, "testDevice/input/statusword");
  LOG_TEST_RESULT(testManager.validatePdoCfg(), L"Method should have succeeded.", AxisProfileResult::STS_OK);

  printTestCaseResult(hresult);

  return hresult;
}

HRESULT TestAxisProfileManager::testExecExternalChecking()
{
  log(L"\n<b>Test case of executing external checking for profiles.</b>");
  HRESULT hresult = S_OK;
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

  printTestCaseResult(hresult);

  return hresult;
}
