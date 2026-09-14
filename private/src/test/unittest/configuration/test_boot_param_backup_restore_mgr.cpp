#include <thread>
#include "test_boot_param_backup_restore_mgr.h"

#include "configuration/boot_param_backup_restore_mgr.h"

#include "mockup/mock_axisprofile.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestBootParamBackupRestoreManager);
REGISTER_TEST_TO_GROUP(TestBootParamBackupRestoreManager, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestBootParamBackupRestoreManager, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestBootParamBackupRestoreManager, Configuration);

HRESULT TestBootParamBackupRestoreManager::doInit()
{
  m_testDatalayerSystem.start();
  m_mockAxisProfileCmp.addDataLayer(m_testDatalayerSystem.factory());
  comm::datalayer::PersistenceHelper* persistenceHelper = new comm::datalayer::PersistenceHelper(&m_testDatalayerSystem);
  persistenceHelper->start();
  m_mockAxisProfileCmp.setPersistenceHelper(persistenceHelper);

  log(L"\nGet path to boot parameter flatbuffer Bfbs file.");
  HRESULT hresult = determinePathToBootParamBfbsFile();
  if (FAILED(hresult))
  {
    error(L"\nCould not get path to boot parameter flatbuffer Bfbs file. Aborting test...");
    return E_FAIL;
  }

  log(L"\nGet test configuration folder path (location of Json files).");
  hresult = determinePathToTestConfigFolder();
  if (FAILED(hresult))
  {
    error(L"\nCould not get test configuration files. Aborting test...");
    return E_FAIL;
  }

  return S_OK;
}

HRESULT TestBootParamBackupRestoreManager::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n Test boot parameter backup/restore manager of the axis profile bundle");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = testBootParamsPersitence();
  hresult |= testBootParamsPersistenceErrors();
  hresult |= testBootParamsFlatbufferCodec();

  return hresult;
}

HRESULT TestBootParamBackupRestoreManager::doEnd()
{
  m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(m_testProfileName.c_str());
  if (m_datalayerProvider)
  {
    delete m_datalayerProvider;
    m_datalayerProvider = nullptr;
  }

  comm::datalayer::PersistenceHelper* persistenceHelper = m_mockAxisProfileCmp.getPersistenceHelperCppItf();
  if (persistenceHelper)
  {
    delete persistenceHelper;
    persistenceHelper = nullptr;
  }
  m_testDatalayerSystem.stop();

  return S_OK;
}

HRESULT TestBootParamBackupRestoreManager::testBootParamsPersitence()
{
  log(L"\n<b>----- Test boot params persistence function -----</b>");

  if (FAILED(configureTestEnvironment()) || FAILED(registerBootParamType()))
  {
    return E_FAIL;
  }

  log(L"\nCreate boot parameter backup/restore manager.");
  BootParamBackupRestoreMgr testCase(m_mockAxisProfileCmp.getAxisProfileManager(), m_mockAxisProfileCmp.getPersistenceHelper());

  log(L"\nCreate profile with name '%ls' for further testing.", test::StringHelper::utf8ToWide(m_testProfileName).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_testProfileName);

  auto* profile = static_cast<test::MockAxisProfile*>(m_mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_testProfileName.c_str()));
  if (profile == nullptr)
  {
    error(L"\nCould not get mock profile instance for boot param persistence test.");
    return E_FAIL;
  }

  BootParamSnapshot expectedSnapshot {};
  expectedSnapshot.profileName = m_testProfileName;
  expectedSnapshot.protocol = BootSnapshotProtocol::CANOPEN;
  expectedSnapshot.paramMap.emplace("0x605A::00", BootParamItem {BootParamValueType::SIGNED16, BootParamValue {.valueInt16 = 2}});
  profile->getDeviceBootParmSnapshotInterface()->setBootParamSnapshot(expectedSnapshot);

  log(L"\nCall saveBootParam() to save boot params.");
  std::string savePath = m_configFolderPath + "/generated/bootParams_profile.json";
  AxisProfileResult retVal = testCase.saveBootParam(m_testProfileName.c_str(), savePath.c_str());
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nMethod should succeed but received error code: 0x%08X", retVal);
    return E_FAIL;
  }

  profile->getDeviceBootParmSnapshotInterface()->setBootParamSnapshot(BootParamSnapshot {});

  log(L"\nCall loadBootParams() to load boot params.");
  retVal = testCase.loadBootParams(m_testProfileName.c_str(), savePath.c_str());
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nMethod should succeed but received error code: 0x%08X", retVal);
    return E_FAIL;
  }

  const auto& restoredSnapshot = profile->getDeviceBootParmSnapshotInterface()->getBootParamSnapshot();
  auto restoredIt = restoredSnapshot.paramMap.find("0x605A::00");
  if (restoredIt == restoredSnapshot.paramMap.end())
  {
    error(L"\nSigned16 boot param should exist after load.");
    return E_FAIL;
  }

  if (restoredIt->second.type != BootParamValueType::SIGNED16)
  {
    error(L"\nUnexpected boot param type after load: 0x%04X", static_cast<std::uint16_t>(restoredIt->second.type));
    return E_FAIL;
  }

  if (restoredIt->second.value.valueInt16 != 2)
  {
    error(L"\nUnexpected Signed16 boot param value after load: %d", restoredIt->second.value.valueInt16);
    return E_FAIL;
  }

  if (FAILED(resetTestEnvironment()))
  {
    return E_FAIL;
  }

  log(L"\n");
  return S_OK;
}

HRESULT TestBootParamBackupRestoreManager::testBootParamsPersistenceErrors()
{
  log(L"\n<b>----- Test boot params persistence error branches -----</b>");

  if (FAILED(configureTestEnvironment()) || FAILED(registerBootParamType()))
  {
    return E_FAIL;
  }

  BootParamBackupRestoreMgr testCase(m_mockAxisProfileCmp.getAxisProfileManager(), m_mockAxisProfileCmp.getPersistenceHelper());

  log(L"\nCreate profile with name '%ls' for error-path tests.", test::StringHelper::utf8ToWide(m_testProfileName).c_str());
  m_mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_testProfileName);

  auto* profile = static_cast<test::MockAxisProfile*>(m_mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_testProfileName.c_str()));
  if (profile == nullptr)
  {
    error(L"\nCould not get mock profile instance for boot param error test.");
    return E_FAIL;
  }

  BootParamSnapshot expectedSnapshot {};
  expectedSnapshot.profileName = m_testProfileName;
  expectedSnapshot.protocol = BootSnapshotProtocol::CANOPEN;
  expectedSnapshot.paramMap.emplace("0x605A::00", BootParamItem {BootParamValueType::SIGNED16, BootParamValue {.valueInt16 = 2}});

  std::string savePath = m_configFolderPath + "/generated/bootParams_profile.json";
  profile->getDeviceBootParmSnapshotInterface()->setBootParamSnapshot(expectedSnapshot);
  if (testCase.saveBootParam(m_testProfileName.c_str(), savePath.c_str()) != AxisProfileResult::STS_OK)
  {
    error(L"\nCould not prepare boot params file for error-path tests.");
    return E_FAIL;
  }

  log(L"\nCall loadBootParams() with empty input arguments.");
  AxisProfileResult retVal = testCase.loadBootParams("", "");
  if (retVal != AxisProfileResult::ERR_INVALID_ARGUMENT)
  {
    error(L"\nWrong return code! Expected: 'ERR_INVALID_ARGUMENT' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall loadBootParams() for a profile that does not exist.");
  retVal = testCase.loadBootParams("missingProfile", savePath.c_str());
  if (retVal != AxisProfileResult::ERR_PROFILE_NOT_FOUND)
  {
    error(L"\nWrong return code! Expected: 'ERR_PROFILE_NOT_FOUND' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall loadBootParams() and simulate persistence read error for non-existing file.");
  retVal = testCase.loadBootParams(m_testProfileName.c_str(), m_configFolderPath + "/generated/not_existing_bootparams.json");
  if (retVal != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_PERSISTENCE' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall saveBootParam() for a profile that does not exist.");
  retVal = testCase.saveBootParam("missingProfile", savePath.c_str());
  if (retVal != AxisProfileResult::ERR_PROFILE_NOT_FOUND)
  {
    error(L"\nWrong return code! Expected: 'ERR_PROFILE_NOT_FOUND' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall saveBootParam() and simulate persistence write error.");
  retVal = testCase.saveBootParam(m_testProfileName.c_str(), "/this/path/generates/an/error");
  if (retVal != AxisProfileResult::ERR_DATALAYER_PERSISTENCE)
  {
    error(L"\nWrong return code! Expected: 'ERR_DATALAYER_PERSISTENCE' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall loadBootParams() with null persistence helper.");
  BootParamBackupRestoreMgr testCaseNullPersistence(m_mockAxisProfileCmp.getAxisProfileManager(), nullptr);
  retVal = testCaseNullPersistence.loadBootParams(m_testProfileName.c_str(), savePath.c_str());
  if (retVal != AxisProfileResult::ERR_EXEC_NULL)
  {
    error(L"\nWrong return code! Expected: 'ERR_EXEC_NULL' Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  if (FAILED(resetTestEnvironment()))
  {
    return E_FAIL;
  }

  log(L"\n");
  return S_OK;
}

HRESULT TestBootParamBackupRestoreManager::configureTestEnvironment()
{
  log(L"\nCreate the test environment.");

  if (!m_datalayerProvider)
  {
    m_datalayerProvider = m_testDatalayerSystem.factory()->createProvider();
  }

  m_datalayerProvider->start();
  m_mockAxisProfileCmp.addDataLayer(m_testDatalayerSystem.factory());

  return S_OK;
}

HRESULT TestBootParamBackupRestoreManager::registerBootParamType()
{
  log(L"\nRegister boot parameter type at the datalayer");
  comm::datalayer::DlResult bootParamResult = m_datalayerProvider->registerType(m_profilesBootParamTypeAddress, m_bootParamBfbsPath);

  if (comm::datalayer::STATUS_FAILED(bootParamResult))
  {
    error(L"\nCould not register boot parameter type at datalayer!");
    return E_FAIL;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  return S_OK;
}

HRESULT TestBootParamBackupRestoreManager::resetTestEnvironment()
{
  log(L"\nReset the test environment.");
  const IAxisProfile* profile = m_mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_testProfileName.c_str());
  if (profile)
  {
    log(L"\nDelete axis profile '%ls'", test::StringHelper::utf8ToWide(m_testProfileName).c_str());
    m_mockAxisProfileCmp.getAxisProfileManager()->deleteAxisProfile(m_testProfileName.c_str());
  }

  m_datalayerProvider->stop();
  return S_OK;
}

HRESULT TestBootParamBackupRestoreManager::determinePathToTestConfigFolder()
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

HRESULT TestBootParamBackupRestoreManager::determinePathToBootParamBfbsFile()
{
  std::string path;
  m_mockAxisProfileCmp.getPersistenceHelperCppItf()->getRootPath(path);
  size_t findPos = path.find("private");
  if (findPos == std::string::npos)
  {
    return E_FAIL;
  }

  path.erase((path.begin() + findPos), path.end());
  std::string bootParamPath = path + "public/bfbs/comm/axisprofile/deviceParamSnapshot.bfbs";

  if (!m_mockAxisProfileCmp.getPersistenceHelperCppItf()->fileExists(bootParamPath))
  {
    return E_FAIL;
  }

  m_bootParamBfbsPath = bootParamPath;
  return S_OK;
}

HRESULT TestBootParamBackupRestoreManager::testBootParamsFlatbufferCodec()
{
  log(L"\n<b>----- Test static boot params flatbuffer codec helpers -----</b>");

  log(L"\nCreate expected boot parameter snapshot with all supported value types.");
  BootParamSnapshot expectedSnapshot {};
  expectedSnapshot.schemaVersion = 1;
  expectedSnapshot.profileName = m_testProfileName;
  expectedSnapshot.protocol = BootSnapshotProtocol::SERCOS;
  expectedSnapshot.checksumValue = 0x12345678;
  expectedSnapshot.paramMap.emplace("0x1000::00", BootParamItem {BootParamValueType::UNSIGNED16, BootParamValue {.valueUint16 = 21}});
  expectedSnapshot.paramMap.emplace("0x1001::00", BootParamItem {BootParamValueType::SIGNED16, BootParamValue {.valueInt16 = -7}});
  expectedSnapshot.paramMap.emplace("0x1002::00", BootParamItem {BootParamValueType::UNSIGNED32, BootParamValue {.valueUint32 = 123456U}});
  expectedSnapshot.paramMap.emplace("0x1003::00", BootParamItem {BootParamValueType::SIGNED32, BootParamValue {.valueInt32 = -456789}});
  expectedSnapshot.paramMap.emplace("0x1004::00", BootParamItem {BootParamValueType::BYTEARRAY, BootParamValue {.valueByteArray = {0x11, 0x22, 0x33}}});

  log(L"\nEncode snapshot with generateFlatbufferData().");
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  builder.Finish(BootParamBackupRestoreMgr::generateFlatbufferData(builder, expectedSnapshot));

  comm::datalayer::Variant data;
  data.copyFlatbuffers(builder);

  log(L"\nDecode snapshot with extractFlatbufferData() variant overload.");
  BootParamSnapshot restoredSnapshot {};
  AxisProfileResult retVal = BootParamBackupRestoreMgr::extractFlatbufferData(&data, restoredSnapshot);
  if (retVal != AxisProfileResult::STS_OK)
  {
    error(L"\nextractFlatbufferData() should succeed but returned: 0x%08X", retVal);
    return E_FAIL;
  }

  if (restoredSnapshot.profileName != expectedSnapshot.profileName ||
      restoredSnapshot.protocol != expectedSnapshot.protocol ||
      restoredSnapshot.checksumValue != expectedSnapshot.checksumValue ||
      restoredSnapshot.paramMap.size() != expectedSnapshot.paramMap.size())
  {
    error(L"\nRestored snapshot metadata does not match expected values.");
    return E_FAIL;
  }

  const auto findAndCheckValue = [&restoredSnapshot](const std::string& key, BootParamValueType type) -> const BootParamItem*
  {
    auto itemIt = restoredSnapshot.paramMap.find(key);
    if (itemIt == restoredSnapshot.paramMap.end() || itemIt->second.type != type)
    {
      return nullptr;
    }
    return &itemIt->second;
  };

  const BootParamItem* u16Item = findAndCheckValue("0x1000::00", BootParamValueType::UNSIGNED16);
  const BootParamItem* s16Item = findAndCheckValue("0x1001::00", BootParamValueType::SIGNED16);
  const BootParamItem* u32Item = findAndCheckValue("0x1002::00", BootParamValueType::UNSIGNED32);
  const BootParamItem* s32Item = findAndCheckValue("0x1003::00", BootParamValueType::SIGNED32);
  const BootParamItem* bytesItem = findAndCheckValue("0x1004::00", BootParamValueType::BYTEARRAY);
  if (!u16Item || !s16Item || !u32Item || !s32Item || !bytesItem)
  {
    error(L"\nAt least one restored boot parameter item is missing or has wrong type.");
    return E_FAIL;
  }

  if (u16Item->value.valueUint16 != 21 ||
      s16Item->value.valueInt16 != -7 ||
      u32Item->value.valueUint32 != 123456U ||
      s32Item->value.valueInt32 != -456789 ||
      bytesItem->value.valueByteArray != std::vector<std::uint8_t> {0x11, 0x22, 0x33})
  {
    error(L"\nAt least one restored boot parameter value is unexpected.");
    return E_FAIL;
  }

  log(L"\nCall extractFlatbufferData() with wrong variant type.");
  comm::datalayer::Variant wrongTypeData {};
  wrongTypeData.setType(comm::datalayer::VariantType::UNKNOWN);
  retVal = BootParamBackupRestoreMgr::extractFlatbufferData(&wrongTypeData, restoredSnapshot);
  if (retVal != AxisProfileResult::ERR_DATALAYER_WRONG_TYPE)
  {
    error(L"\nWrong return code for wrong variant type. Expected: ERR_DATALAYER_WRONG_TYPE Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall extractFlatbufferData() with incomplete flatbuffer (missing parameter list).");
  flatbuffers::FlatBufferBuilder invalidBuilder;
  invalidBuilder.ForceDefaults(true);
  auto profileNameOffset = invalidBuilder.CreateString(m_testProfileName);
  invalidBuilder.Finish(fbtypes::CreatebootParamSnapshot(invalidBuilder,
                                                         1,
                                                         profileNameOffset,
                                                         fbtypes::bootSnapshotProtocol_CANOPEN,
                                                         55,
                                                         0));

  comm::datalayer::Variant incompleteData {};
  incompleteData.copyFlatbuffers(invalidBuilder);
  retVal = BootParamBackupRestoreMgr::extractFlatbufferData(&incompleteData, restoredSnapshot);
  if (retVal != AxisProfileResult::ERR_DATALAYER_WRONG_TYPE)
  {
    error(L"\nWrong return code for incomplete flatbuffer. Expected: ERR_DATALAYER_WRONG_TYPE Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall extractFlatbufferData() fb-view overload with invalid protocol value.");
  flatbuffers::FlatBufferBuilder wrongProtocolBuilder;
  wrongProtocolBuilder.ForceDefaults(true);
  std::vector<flatbuffers::Offset<fbtypes::bootParam>> emptyParamList {};
  auto emptyParamListOffset = wrongProtocolBuilder.CreateVector(emptyParamList);
  auto wrongProtocolNameOffset = wrongProtocolBuilder.CreateString(m_testProfileName);
  wrongProtocolBuilder.Finish(fbtypes::CreatebootParamSnapshot(wrongProtocolBuilder,
                                                               1,
                                                               wrongProtocolNameOffset,
                                                               fbtypes::bootSnapshotProtocol_UNKNOWN,
                                                               66,
                                                               emptyParamListOffset));

  const fbtypes::bootParamSnapshot* wrongProtocolData = fbtypes::GetbootParamSnapshot(wrongProtocolBuilder.GetBufferPointer());
  retVal = BootParamBackupRestoreMgr::extractFlatbufferData(wrongProtocolData, restoredSnapshot);
  if (retVal != AxisProfileResult::ERR_DATALAYER_WRONG_TYPE)
  {
    error(L"\nWrong return code for invalid protocol. Expected: ERR_DATALAYER_WRONG_TYPE Actual: 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\n");
  return S_OK;
}
