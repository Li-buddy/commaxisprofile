#include "test_deviceinfo_manager.h"
#include "fieldbuses/fieldbus_adapter.h"

// Mockups
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/external_bundles/mock_datalayer_client_ex.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_axisprofile.h"
#include "comm/ethercat/master/fbs/slave_address_mapping_generated.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestDeviceInfoMgr);
REGISTER_TEST_TO_GROUP(TestDeviceInfoMgr, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestDeviceInfoMgr, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestDeviceInfoMgr, Manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestDeviceInfoMgr::doInit()
{
  return S_OK;
}

HRESULT TestDeviceInfoMgr::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the SoE fieldbus adapter class.");
  log(L"\n----------------------------------------------------------------");

  hResult |= TestSetDataLayer();
  hResult |= TestSetProfiles();
  hResult |= TestInitAvailableDeviceInfo();
  hResult |= TestUpdateProfileAddressSimpleCase();
  hResult |= TestUpdateProfileAddress();
  hResult |= TestCheckForDeviceAddressAndName();
  hResult |= TestValidateProtocolInfo();

  return hResult;
}

HRESULT TestDeviceInfoMgr::doEnd()
{
  return S_OK;
}

HRESULT TestDeviceInfoMgr::TestSetDataLayer()
{
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgr = getNewDeviceInforMgr();
  test::MockDatalayerClient testclient{};
  ptrDeviceMgr->setDataLayer(&testclient);
  return S_OK;
}

HRESULT TestDeviceInfoMgr::TestSetProfiles()
{
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgr = getNewDeviceInforMgr();
  std::map<std::string, ProfileInfo> m_mapAxisProfiles{};
  ProfileInfo testProfile1;
  ProfileInfo testProfile2;
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile1", testProfile1 });
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile2", testProfile2 });

  ptrDeviceMgr->setProfiles(&m_mapAxisProfiles);
  return S_OK;
}

HRESULT TestDeviceInfoMgr::TestInitAvailableDeviceInfo()
{
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgr = getNewDeviceInforMgr();
  log(L"\n  test for empty profile");

  std::map<std::string, ProfileInfo> m_mapAxisProfiles{};
  ptrDeviceMgr->setProfiles(&m_mapAxisProfiles);
  ptrDeviceMgr->initAvailableDeviceInfo();

  ProfileInfo testProfile1;
  ProfileInfo testProfile2;
  test::MockAxisProfile mockProfile1("testProfile1", AxisProfileType::SERVODRIVE_OVER_ETHERCAT);
  testProfile1.m_axisProfile = &mockProfile1;
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile1", testProfile1 });
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile2", testProfile2 });

  ptrDeviceMgr->setProfiles(&m_mapAxisProfiles);

  log(L"\n  test for empty client");
  ptrDeviceMgr->initAvailableDeviceInfo();

  test::MockDatalayerClient testclient{};
  ptrDeviceMgr->setDataLayer(&testclient);

  log(L"\n  test for empty address");
  ptrDeviceMgr->initAvailableDeviceInfo();
  ptrDeviceMgr->addFieldbusInstance(std::string("InstanceTest"), nullptr );

  log(L"\n  test for invalid address read result");
  testclient.m_returnValue = comm::datalayer::DlResult::DL_INVALID_ADDRESS;
  ptrDeviceMgr->initAvailableDeviceInfo();

  log(L"\n  test for error read result");
  testclient.m_returnValue = comm::datalayer::DlResult::DL_OUT_OF_MEMORY;
  ptrDeviceMgr->initAvailableDeviceInfo();

  log(L"\n  test for right read result extract");
  testclient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  ptrDeviceMgr->initAvailableDeviceInfo();

  ptrDeviceMgr->addFieldbusInstance(std::string("InstanceTest"), testProfile1.m_axisProfile);
  test::MockDatalayerClientEx testclientEx {};
  testclientEx.m_vecReturnValue.emplace_back(comm::datalayer::DlResult::DL_OK);
  testclientEx.m_vecReturnValue.emplace_back(comm::datalayer::DlResult::DL_TIMEOUT);
  comm::datalayer::Variant varMapping {};
  generateSlaveAddressMappingResponse(1, varMapping);
  testclientEx.m_vecCallBackVariant.emplace_back(varMapping);
  comm::datalayer::Variant varCfgedSlaveInfo {};
  std::string slaveName {"test"};
  generateCfgSlaveInfo(varCfgedSlaveInfo, slaveName);
  testclientEx.m_vecCallBackVariant.emplace_back(varCfgedSlaveInfo);

  ptrDeviceMgr->setDataLayer(&testclientEx);
  ptrDeviceMgr->initAvailableDeviceInfo();

  return S_OK;
}
HRESULT TestDeviceInfoMgr::TestUpdateProfileAddressSimpleCase()
{
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgr = getNewDeviceInforMgr();
  log(L"\nCall UpdateProfileAddress() with empty profile");
  ptrDeviceMgr->setProfiles(nullptr);
  ptrDeviceMgr->updateProfileAddress();

  std::map<std::string, ProfileInfo> m_mapAxisProfiles {};
  ProfileInfo testProfile1;
  ProfileInfo testProfile2;
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile1", testProfile1 });
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile2", testProfile2 });

  log(L"\nCall UpdateProfileAddress() with null client");
  ptrDeviceMgr->setProfiles(&m_mapAxisProfiles);
  ptrDeviceMgr->setDataLayer(nullptr);
  ptrDeviceMgr->updateProfileAddress();

  log(L"\nCall UpdateProfileAddress() with read result : invalid address");
  test::MockDatalayerClientEx testclient {};
  testclient.m_returnValue = comm::datalayer::DlResult::DL_INVALID_ADDRESS;
  ptrDeviceMgr->setDataLayer(&testclient);
  ptrDeviceMgr->addFieldbusInstance("testEthercat", nullptr);
  ptrDeviceMgr->updateProfileAddress();

  log(L"\nCall UpdateProfileAddress() with read result : DL_FAILED");
  testclient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  ptrDeviceMgr->updateProfileAddress();

  log(L"\nCall UpdateProfileAddress() with read result : OK");
  testclient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  ptrDeviceMgr->updateProfileAddress();

  log(L"\nCall UpdateProfileAddress() without device name");
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgrNoDeviceName = getNewDeviceInforMgr();
  ptrDeviceMgrNoDeviceName->setProfiles(&m_mapAxisProfiles);
  testclient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  ptrDeviceMgrNoDeviceName->setDataLayer(&testclient);
  std::uint32_t nCount {1};
  std::string slaveName {"test"};
  comm::datalayer::Variant varMapping {};
  comm::datalayer::Variant varCfgedSlaveInfo {};
  generateSlaveAddressMappingResponse(nCount, varMapping);
  testclient.m_vecCallBackVariant.emplace_back(varMapping);

  generateCfgSlaveInfo(varCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.emplace_back(varCfgedSlaveInfo);
  ptrDeviceMgrNoDeviceName->addFieldbusInstance("testEthercat", nullptr);
  ptrDeviceMgrNoDeviceName->updateProfileAddress();
  testclient.m_vecCallBackVariant.clear();
  testclient.m_nCalledTimes = 0;
  m_mapAxisProfiles.clear();
  return S_OK;
}

HRESULT TestDeviceInfoMgr::TestUpdateProfileAddress()
{
  std::map<std::string, ProfileInfo> m_mapAxisProfiles {};
  test::MockAxisProfile mockProfile1("testProfile1", AxisProfileType::SERVODRIVE_OVER_ETHERCAT);
  test::MockAxisProfile mockProfile2("testProfile2", AxisProfileType::CANOPEN_OVER_ETHERCAT);
  test::MockDatalayerClientEx testclient {};
  comm::datalayer::Variant varMapping {};
  comm::datalayer::Variant variantCfgedSlaveInfo {};

  // case 1, device name not found in profile
  log(L"\nCall UpdateProfileAddress() with device not found data");
  mockProfile1.getPropertyInterface()->setDeviceName("testDevice1");
  mockProfile1.getPropertyInterface()->setDeviceAddress(1001);
  mockProfile2.getPropertyInterface()->setDeviceName("not_found");
  mockProfile2.getPropertyInterface()->setDeviceAddress(1002);
  ProfileInfo testProfileOk;
  ProfileInfo testProfileWrong;
  testProfileOk.m_axisProfile = &mockProfile1;
  testProfileWrong.m_axisProfile = &mockProfile2;
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile1", testProfileOk });
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile2", testProfileWrong });

  //set profile
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgrNotFound = getNewDeviceInforMgr();
  ptrDeviceMgrNotFound->setProfiles(&m_mapAxisProfiles);
  testclient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  ptrDeviceMgrNotFound->setDataLayer(&testclient);
  ptrDeviceMgrNotFound->addFieldbusInstance("testEthercat", testProfileWrong.m_axisProfile);

  generateSlaveAddressMappingResponse(2, varMapping);
  testclient.m_vecCallBackVariant.push_back(varMapping);
  std::string slaveName = {"testDevice1"};
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);
  slaveName = {"testDevice2"};
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);
  AxisProfileResult expResult {AxisProfileResult::ERR_NOK};
  AxisProfileResult actResult = ptrDeviceMgrNotFound->updateProfileAddress();
  if (expResult != actResult)
  {
    error(L"\n UpdateProfileAddress did not return expResult. Result: %d, Exp: %d", actResult, expResult);
    return E_FAIL;
  }
  testclient.m_vecCallBackVariant.clear();
  testclient.m_nCalledTimes = 0;
  m_mapAxisProfiles.clear();

  // case 2, for conflict address
  log(L"\nCall UpdateProfileAddress() with duplicate address data, and device with address 1001 has empty device name, will update its device name");
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgrCorrect = getNewDeviceInforMgr();
  test::MockAxisProfile mockProfileSoE("testProfile1", AxisProfileType::SERVODRIVE_OVER_ETHERCAT);
  test::MockAxisProfile mockProfileCoE("testProfile2", AxisProfileType::CANOPEN_OVER_ETHERCAT);
  mockProfileSoE.getPropertyInterface()->setDeviceName("testDevice1");
  mockProfileSoE.getPropertyInterface()->setDeviceAddress(1001);
  mockProfileCoE.getPropertyInterface()->setDeviceName("");
  mockProfileCoE.getPropertyInterface()->setDeviceAddress(1001);

  ProfileInfo testProfileSoE;
  ProfileInfo testProfileCoE;
  testProfileSoE.m_axisProfile = &mockProfileSoE;
  testProfileCoE.m_axisProfile = &mockProfileCoE;
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile1", testProfileCoE });
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile2", testProfileSoE });

  //set profile
  ptrDeviceMgrCorrect->setProfiles(&m_mapAxisProfiles);
  ptrDeviceMgrCorrect->setDataLayer(&testclient);
  testclient.m_returnValue = comm::datalayer::DlResult::DL_OK;

  ptrDeviceMgrCorrect->addFieldbusInstance("testEthercat", testProfileSoE.m_axisProfile);

  //case for conflict address
  generateSlaveAddressMappingResponse(2, varMapping);
  testclient.m_vecCallBackVariant.push_back(varMapping);
  slaveName = { "testDevice1" };
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);
  slaveName = { "testDevice-add-1002" };
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);

  expResult = AxisProfileResult::ERR_PROFILE_CONFIG;
  actResult = ptrDeviceMgrCorrect->updateProfileAddress();
  if ( expResult != actResult )
  {
    error(L"\n UpdateProfileAddress did not return expResult. Result: %d, Exp: %d", actResult, expResult);
    return E_FAIL;
  }
  //m_mapAxisProfiles.clear();
  testclient.m_vecCallBackVariant.clear();
  testclient.m_nCalledTimes = 0;
  ptrDeviceMgrCorrect->clearDeviceInfoContainer();
  //rest property data;
  mockProfileCoE.getPropertyInterface()->setDeviceName("deviceName-1001");
  mockProfileCoE.getPropertyInterface()->setDeviceAddress(1001);

  // case 3, for conflict name
  log(L"\nCall UpdateProfileAddress() with duplicate address, and device with address 1001 will update address to 1002");
  generateSlaveAddressMappingResponse(2, varMapping);
  testclient.m_vecCallBackVariant.push_back(varMapping);
  slaveName = { "deviceName-1001" };//address:1001
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);
  slaveName = { "testDevice1" };//address:1002
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);
  actResult = ptrDeviceMgrCorrect->updateProfileAddress();
  if (AxisProfileResult::STS_OK != actResult )
  {
    error(L"\n UpdateProfileAddress did not return expResult. Result: %d, Exp: %d", actResult, expResult);
    return E_FAIL;
  }

  // case 4, normal case
  m_mapAxisProfiles.clear();
  testclient.m_vecCallBackVariant.clear();
  testclient.m_nCalledTimes = 0;
  ptrDeviceMgrNotFound->clearDeviceInfoContainer();
  log(L"\nCall UpdateProfileAddress() with correct data");
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgrDupAdd = getNewDeviceInforMgr();
  mockProfile1.getPropertyInterface()->setDeviceName("testDevice1");
  mockProfile1.getPropertyInterface()->setDeviceAddress(1002);
  mockProfile2.getPropertyInterface()->setDeviceName("testDevice2");
  mockProfile2.getPropertyInterface()->setDeviceAddress(1001);
  ProfileInfo testProfileDev1;
  ProfileInfo testProfileDev2;
  testProfileDev1.m_axisProfile = &mockProfile1;
  testProfileDev2.m_axisProfile = &mockProfile2;
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile1", testProfileDev1 });
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile2", testProfileDev2 });

  //set profile
  ptrDeviceMgrDupAdd->setProfiles(&m_mapAxisProfiles);
  testclient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  ptrDeviceMgrDupAdd->setDataLayer(&testclient);
  ptrDeviceMgrDupAdd->addFieldbusInstance("testEthercat", testProfileDev1.m_axisProfile);

  generateSlaveAddressMappingResponse(2, varMapping);
  testclient.m_vecCallBackVariant.push_back(varMapping);

  slaveName = {"testDevice1"};
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);
  slaveName = {"testDevice2"};
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclient.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);

  actResult = ptrDeviceMgrDupAdd->updateProfileAddress();
  if (AxisProfileResult::STS_OK != actResult)
  {
    error(L"\n UpdateProfileAddress did not return expResult. Result: %d, Exp: %d", actResult, AxisProfileResult::STS_OK);
    return E_FAIL;
  }

  testclient.m_vecCallBackVariant.clear();
  testclient.m_nCalledTimes = 0;
  m_mapAxisProfiles.clear();
  return S_OK;
}

HRESULT TestDeviceInfoMgr::TestCheckForDeviceAddressAndName()
{
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgrCorrect = getNewDeviceInforMgr();

  // test for check method
  std::vector<DeviceInfo> vecAllCfgedDeviceInfo {};
  vecAllCfgedDeviceInfo.push_back(DeviceInfo {"testProfile2", 1001, 0, AxisProfileCategory::CATEGORY_DRIVE});
  vecAllCfgedDeviceInfo.push_back(DeviceInfo {"testProfile2", 1001, 0, AxisProfileCategory::CATEGORY_DRIVE});
  AxisProfileResult actResult = ptrDeviceMgrCorrect->checkDuplicateDevices(vecAllCfgedDeviceInfo);
  AxisProfileResult expResult = AxisProfileResult::ERR_PROFILE_CONFIG;
  if (expResult != actResult)
  {
    error(L"\n UpdateProfileAddress did not return expResult. Result: %d, Exp: %d", actResult, expResult);
    return E_FAIL;
  }

  std::vector<DeviceInfo> vecAllCfgedDeviceInfo2 {};
  vecAllCfgedDeviceInfo2.push_back(DeviceInfo {"testProfile2", 1001, 0, AxisProfileCategory::CATEGORY_DRIVE});
  vecAllCfgedDeviceInfo2.push_back(DeviceInfo {"testProfile2", 1001, 0, AxisProfileCategory::CATEGORY_ENCODER});
  vecAllCfgedDeviceInfo2.push_back(DeviceInfo {"testProfile2", 1001, 1, AxisProfileCategory::CATEGORY_DRIVE});
  vecAllCfgedDeviceInfo2.push_back(DeviceInfo {"testProfile3", 1001, 1, AxisProfileCategory::CATEGORY_DRIVE});
  actResult = ptrDeviceMgrCorrect->checkDuplicateDevices(vecAllCfgedDeviceInfo2);
  expResult = AxisProfileResult::STS_OK;
  if (expResult != actResult)
  {
    error(L"\n UpdateProfileAddress did not return expResult. Result: %d, Exp: %d", actResult, expResult);
    return E_FAIL;
  }
  return S_OK;
}

HRESULT TestDeviceInfoMgr::TestValidateProtocolInfo()
{
  std::shared_ptr<DeviceInfoManager> ptrDeviceMgrCorrect = getNewDeviceInforMgr();
  test::MockAxisProfile mockProfileSoE("testProfile1", AxisProfileType::SERVODRIVE_OVER_ETHERCAT);
  test::MockAxisProfile mockProfileCoE("testProfile2", AxisProfileType::CANOPEN_OVER_ETHERCAT);
  test::MockAxisProfile mockProfileUndefined("testProfileUndefined", AxisProfileType::UNDEFINED);
  mockProfileSoE.getPropertyInterface()->setDeviceName("testDevice1");
  mockProfileSoE.getPropertyInterface()->setDeviceAddress(1001);
  mockProfileCoE.getPropertyInterface()->setDeviceName("testDevice1");
  mockProfileCoE.getPropertyInterface()->setDeviceAddress(1001);
  ProfileInfo testProfileSoE;
  ProfileInfo testProfileCoE;
  ProfileInfo testProfileUndefined;
  testProfileSoE.m_axisProfile = &mockProfileSoE;
  testProfileCoE.m_axisProfile = &mockProfileCoE;
  testProfileUndefined.m_axisProfile = &mockProfileUndefined;
  std::map<std::string, ProfileInfo> m_mapAxisProfiles {};
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile1", testProfileCoE });
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile2", testProfileSoE });

  //set profile
  ptrDeviceMgrCorrect->setProfiles(&m_mapAxisProfiles);
  test::MockDatalayerClientEx testclientCorrect {};
  ptrDeviceMgrCorrect->setDataLayer(&testclientCorrect);
  testclientCorrect.m_returnValue = comm::datalayer::DlResult::DL_OK;

  ptrDeviceMgrCorrect->addFieldbusInstance("testEthercat", testProfileSoE.m_axisProfile);
  std::string slaveName {"test"};
  comm::datalayer::Variant varCfgedSlaveCount {};
  comm::datalayer::Variant varCfgedSlaveInfo {};
  comm::datalayer::Variant varMapping {};
  comm::datalayer::Variant variantCfgedSlaveInfo {};
  generateSlaveAddressMappingResponse(2, varMapping);
  testclientCorrect.m_vecCallBackVariant.push_back(varMapping);
  slaveName = {"testDevice1"};
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclientCorrect.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);
  slaveName = {""};
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName);
  testclientCorrect.m_vecCallBackVariant.push_back(variantCfgedSlaveInfo);

  ptrDeviceMgrCorrect->updateProfileAddress();
  ptrDeviceMgrCorrect->validateProtocolInfo();

  m_mapAxisProfiles.clear();
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile2", testProfileSoE });
  ptrDeviceMgrCorrect->setProfiles(&m_mapAxisProfiles);
  ptrDeviceMgrCorrect->validateProtocolInfo();

  m_mapAxisProfiles.clear();
  m_mapAxisProfiles.emplace(std::pair<std::string, ProfileInfo>{ "testProfile3", testProfileUndefined });
  ptrDeviceMgrCorrect->setProfiles(&m_mapAxisProfiles);
  ptrDeviceMgrCorrect->validateProtocolInfo();

  return S_OK;
}

void TestDeviceInfoMgr::generateCfgSlaveInfo(comm::datalayer::Variant& variantCfgedSlaveInfo, std::string& slaveName)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  auto slaveNameOffset = builder.CreateString(slaveName);
  comm::ethercat::master::fbs::SlaveConfigInfoResponseBuilder responseBuilder(builder);
  responseBuilder.add_slaveName(slaveNameOffset);
  responseBuilder.add_mbxProtocols(0x1A);
  comm::ethercat::master::fbs::EthercatIdentityInfo identityInfo(36, 2369540, 3, 0);
  responseBuilder.add_slaveIdentity(&identityInfo);
  auto numOffsetResponse = responseBuilder.Finish();

  comm::ethercat::master::fbs::SlaveConfigInfoBuilder cfgSlaveName(builder);
  cfgSlaveName.add_response(numOffsetResponse);
  auto numOffset = cfgSlaveName.Finish();
  builder.Finish(numOffset);
  variantCfgedSlaveInfo.copyFlatbuffers(builder);
}

std::unique_ptr<comm::axisprofile::DeviceInfoManager> TestDeviceInfoMgr::getNewDeviceInforMgr()
{
  return std::make_unique<DeviceInfoManager>();
}

void TestDeviceInfoMgr::generateSlaveAddressMappingResponse(std::uint16_t addressCnt, comm::datalayer::Variant& mappingResponse)
{
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<comm::ethercat::master::fbs::AddressMappingEntry>> entries;
  for (int i = 0; i < addressCnt; ++i)
  {
    auto entryAddr = comm::ethercat::master::fbs::CreateAddressedRequest(builder, comm::ethercat::master::fbs::Addresstype_fixedphysical, 1001 + i);
    auto entryName = builder.CreateString("SLAVE_" + std::to_string(i));

    auto entry = comm::ethercat::master::fbs::CreateAddressMappingEntry(builder, entryAddr, entryName);
    entries.push_back(entry);
  }

  auto entryList = builder.CreateVector(entries);
  auto response = comm::ethercat::master::fbs::CreateSlaveAddressMappingResponse(builder, entryList);

  comm::ethercat::master::fbs::SlaveAddressMappingBuilder mappingBuilder(builder);
  mappingBuilder.add_response(response);
  auto mappingRoot = mappingBuilder.Finish();

  builder.Finish(mappingRoot);
  mappingResponse.copyFlatbuffers(builder);
}
