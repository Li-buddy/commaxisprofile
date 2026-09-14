// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_provider_node_profiles_boot_params.h"
#include "helper/provider_helper_unittest.h"
#include "configuration/boot_param_backup_restore_mgr.h"
#include "provider/nodes/profiles/state/node_profiles_boot_params.h"

// Mockups
#include "mockup/mock_axisprofile.h"
#include "mockup/mock_axisprofile_cmp.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderProfilesBootParams);
REGISTER_TEST_TO_GROUP(TestProviderProfilesBootParams, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesBootParams, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesBootParams, Provider);

namespace
{
comm::datalayer::Variant createBootParamVariant(const BootParamSnapshot& snapshot)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  builder.Finish(BootParamBackupRestoreMgr::generateFlatbufferData(builder, snapshot));

  comm::datalayer::Variant data;
  data.copyFlatbuffers(builder);
  return data;
}
}

HRESULT TestProviderProfilesBootParams::doInit()
{
  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");
  return S_OK;
}

HRESULT TestProviderProfilesBootParams::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n--------------------------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to get/set persisted boot parameters.");
  log(L"\n--------------------------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnWrite();
  hResult |= testOnMetadata();

  return hResult;
}

HRESULT TestProviderProfilesBootParams::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesBootParams nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesBootParams node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  auto* profile = static_cast<test::MockAxisProfile*>(mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));
  if (!profile)
  {
    error(L"\nCould not get profile instance.");
    return E_FAIL;
  }

  log(L"\nCall onRead() when boot parameter snapshot interface is missing.");
  profile->setBootParamSnapshotInterfaceMissing(true);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));
  profile->setBootParamSnapshotInterfaceMissing(false);

  BootParamSnapshot expectedSnapshot {};
  expectedSnapshot.profileName = m_profileName;
  expectedSnapshot.protocol = BootSnapshotProtocol::CANOPEN;
  expectedSnapshot.checksumValue = 55;
  expectedSnapshot.paramMap.emplace("0x605A::00", BootParamItem {BootParamValueType::SIGNED16, BootParamValue {.valueInt16 = 2}});
  profile->getDeviceBootParmSnapshotInterface()->setBootParamSnapshot(expectedSnapshot);

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expectedData = createBootParamVariant(expectedSnapshot);
  comm::datalayer::Variant readData {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true,
                                                      comm::datalayer::DlResult::DL_OK, &expectedData, fbtypes::VerifybootParamSnapshotBuffer));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesBootParams::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesBootParams node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onWrite() when requested profile does not exist.");
  comm::datalayer::Variant data {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_invalidNodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  auto* profile = static_cast<test::MockAxisProfile*>(mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));
  if (!profile)
  {
    error(L"\nCould not get profile instance.");
    return E_FAIL;
  }

  log(L"\nCall onWrite() when boot parameter snapshot interface is missing.");
  profile->setBootParamSnapshotInterfaceMissing(true);
  comm::datalayer::Variant missingIfData {};
  missingIfData.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &missingIfData, false,
                                                       comm::datalayer::DlResult::DL_INVALID_HANDLE));
  profile->setBootParamSnapshotInterfaceMissing(false);

  log(L"\nCall onWrite() with wrong variant type.");
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onWrite() with flatbuffer that has missing mandatory fields.");
  flatbuffers::FlatBufferBuilder invalidBuilder;
  invalidBuilder.ForceDefaults(true);
  auto profileNameOffset = invalidBuilder.CreateString(m_profileName);
  invalidBuilder.Finish(fbtypes::CreatebootParamSnapshot(invalidBuilder,
                                                         1,
                                                         profileNameOffset,
                                                         fbtypes::bootSnapshotProtocol_CANOPEN,
                                                         10,
                                                         0));
  comm::datalayer::Variant invalidData {};
  invalidData.copyFlatbuffers(invalidBuilder);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &invalidData, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onWrite() when everything is OK.");
  BootParamSnapshot writtenSnapshot {};
  writtenSnapshot.profileName = m_profileName;
  writtenSnapshot.protocol = BootSnapshotProtocol::SERCOS;
  writtenSnapshot.checksumValue = 1234;
  writtenSnapshot.paramMap.emplace("IDN_S(1,123)", BootParamItem {BootParamValueType::UNSIGNED16, BootParamValue {.valueUint16 = 42}});
  writtenSnapshot.paramMap.emplace("IDN_P(0,120)", BootParamItem {BootParamValueType::BYTEARRAY, BootParamValue {.valueByteArray = {0x11, 0x22}}});

  data = createBootParamVariant(writtenSnapshot);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, true, comm::datalayer::DlResult::DL_OK));

  const BootParamSnapshot& restoredSnapshot = profile->getDeviceBootParmSnapshotInterface()->getBootParamSnapshot();
  if (restoredSnapshot.profileName != writtenSnapshot.profileName ||
      restoredSnapshot.protocol != writtenSnapshot.protocol ||
      restoredSnapshot.checksumValue != writtenSnapshot.checksumValue ||
      restoredSnapshot.paramMap.size() != writtenSnapshot.paramMap.size())
  {
    error(L"\nUnexpected boot parameter snapshot metadata after successful onWrite().");
    return E_FAIL;
  }

  auto uint16Item = restoredSnapshot.paramMap.find("IDN_S(1,123)");
  auto bytesItem = restoredSnapshot.paramMap.find("IDN_P(0,120)");
  if (uint16Item == restoredSnapshot.paramMap.end() || bytesItem == restoredSnapshot.paramMap.end())
  {
    error(L"\nExpected boot parameters are missing after successful onWrite().");
    return E_FAIL;
  }

  if (uint16Item->second.type != BootParamValueType::UNSIGNED16 || uint16Item->second.value.valueUint16 != 42)
  {
    error(L"\nUnsigned16 boot parameter value mismatch after successful onWrite().");
    return E_FAIL;
  }

  if (bytesItem->second.type != BootParamValueType::BYTEARRAY ||
      bytesItem->second.value.valueByteArray != std::vector<std::uint8_t> {0x11, 0x22})
  {
    error(L"\nByteArray boot parameter value mismatch after successful onWrite().");
    return E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesBootParams::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onMetadata() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesBootParams node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
