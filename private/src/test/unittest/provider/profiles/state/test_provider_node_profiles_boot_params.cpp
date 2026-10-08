// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_provider_node_profiles_boot_params.h"
#include "helper/provider_helper_unittest.h"
#include "provider/nodes/profiles/state/node_profiles_boot_params.h"
#include "comm/axisprofile/datatypes/keyValueStringMap_generated.h"
#include "fieldbuses/fieldbus_helper.h"

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

comm::datalayer::Variant createBootParamKeyMapVariant(const BootParamSnapshot& snapshot)
{
  std::vector<flatbuffers::Offset<fbtypes::keyValueEntry>> params;
  params.reserve(snapshot.paramMap.size());

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  for (const auto& [name, value] : snapshot.paramMap)
  {
    auto nameOffset = builder.CreateString(name);
    auto valueOffset = builder.CreateString(provider::NodeProfilesBootParams::toReadableValue(value));
    params.push_back(fbtypes::CreatekeyValueEntry(builder, nameOffset, valueOffset));
  }

  auto mapOffset = builder.CreateVector(params);
  builder.Finish(fbtypes::CreatekeyValueStringMap(builder, mapOffset));

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
  log(L"\n Test the axisprofile datalayer node, used to get persisted boot parameters.");
  log(L"\n--------------------------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testToReadableValue();
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
  comm::datalayer::Variant expectedData = createBootParamKeyMapVariant(expectedSnapshot);
  comm::datalayer::Variant readData {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true,
                                                      comm::datalayer::DlResult::DL_OK, &expectedData, fbtypes::VerifykeyValueStringMapBuffer));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesBootParams::testToReadableValue()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method toReadableValue() -----</b>");

  BootParamItem u16Item {};
  u16Item.type = BootParamValueType::UNSIGNED16;
  u16Item.value.valueUint16 = 42U;
  if (provider::NodeProfilesBootParams::toReadableValue(u16Item) != "42")
  {
    error(L"\nUnexpected conversion result for UNSIGNED16.");
    hResult |= E_FAIL;
  }

  BootParamItem s16Item {};
  s16Item.type = BootParamValueType::SIGNED16;
  s16Item.value.valueInt16 = -12;
  if (provider::NodeProfilesBootParams::toReadableValue(s16Item) != "-12")
  {
    error(L"\nUnexpected conversion result for SIGNED16.");
    hResult |= E_FAIL;
  }

  BootParamItem u32Item {};
  u32Item.type = BootParamValueType::UNSIGNED32;
  u32Item.value.valueUint32 = 123456U;
  if (provider::NodeProfilesBootParams::toReadableValue(u32Item) != "123456")
  {
    error(L"\nUnexpected conversion result for UNSIGNED32.");
    hResult |= E_FAIL;
  }

  BootParamItem s32Item {};
  s32Item.type = BootParamValueType::SIGNED32;
  s32Item.value.valueInt32 = -7890;
  if (provider::NodeProfilesBootParams::toReadableValue(s32Item) != "-7890")
  {
    error(L"\nUnexpected conversion result for SIGNED32.");
    hResult |= E_FAIL;
  }

  BootParamItem byteArrayItem {};
  byteArrayItem.type = BootParamValueType::BYTEARRAY;
  byteArrayItem.value.valueByteArray = {0x11, 0x22, 0xAB};
  std::string expectedByteArray = FieldbusHelper::rawDataToHexDump(byteArrayItem.value.valueByteArray.data(), byteArrayItem.value.valueByteArray.size());
  if (provider::NodeProfilesBootParams::toReadableValue(byteArrayItem) != expectedByteArray)
  {
    error(L"\nUnexpected conversion result for BYTEARRAY.");
    hResult |= E_FAIL;
  }

  BootParamItem invalidTypeItem {};
  invalidTypeItem.type = static_cast<BootParamValueType>(0xFF);
  if (!provider::NodeProfilesBootParams::toReadableValue(invalidTypeItem).empty())
  {
    error(L"\nUnexpected conversion result for invalid value type.");
    hResult |= E_FAIL;
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
