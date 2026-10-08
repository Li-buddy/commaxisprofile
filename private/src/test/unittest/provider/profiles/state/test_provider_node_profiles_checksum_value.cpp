// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_provider_node_profiles_checksum_value.h"
#include "helper/provider_helper_unittest.h"
#include "provider/nodes/profiles/state/node_profiles_checksum_value.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderProfilesChecksumValue);
REGISTER_TEST_TO_GROUP(TestProviderProfilesChecksumValue, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesChecksumValue, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesChecksumValue, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderProfilesChecksumValue::doInit()
{
  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");
  return S_OK;
}

HRESULT TestProviderProfilesChecksumValue::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n--------------------------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to get checksum value.");
  log(L"\n--------------------------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnMetadata();

  return hResult;
}

HRESULT TestProviderProfilesChecksumValue::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesChecksumValue nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesChecksumValue node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  auto* profile = static_cast<IAxisProfileInternal*>(mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));
  if (!profile)
  {
    error(L"\nCould not get profile instance.");
    return E_FAIL;
  }

  std::uint32_t checksum = 123;
  BootParamSnapshot snapshot {};
  snapshot.checksumValue = checksum;
  profile->getDeviceBootParmSnapshotInterface()->setBootParamSnapshot(snapshot);

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expectedData {};
  expectedData.setValue(checksum);
  comm::datalayer::Variant readData {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expectedData));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesChecksumValue::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onMetadata() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesChecksumValue node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
