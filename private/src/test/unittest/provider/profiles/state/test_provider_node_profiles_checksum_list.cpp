// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_provider_node_profiles_checksum_list.h"
#include "helper/provider_helper_unittest.h"
#include "provider/nodes/profiles/state/node_profiles_checksum_list.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/mock_axisprofile_device_param.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderProfilesChecksumList);
REGISTER_TEST_TO_GROUP(TestProviderProfilesChecksumList, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesChecksumList, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesChecksumList, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderProfilesChecksumList::doInit()
{
  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");
  return S_OK;
}

HRESULT TestProviderProfilesChecksumList::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n--------------------------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to get checksum idn list.");
  log(L"\n--------------------------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnMetadata();

  return hResult;
}

HRESULT TestProviderProfilesChecksumList::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesChecksumList nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesChecksumList node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  log(L"\nSet checksum ind list.");
  IAxisProfile* tempProfile = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str());
  if ( !tempProfile )
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }
  auto profile = static_cast<IAxisProfileInternal*>(tempProfile);
  test::MockAxisProfileDeviceParam* pDevParm = static_cast<test::MockAxisProfileDeviceParam*>(profile->getDeviceParamInterface());
  std::vector<std::string> idns = {"IDN_S(1, 123)", "IDN_P(0, 120)"};
  pDevParm->setChecksumIdnList(idns);

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData{};
  expData.setValue(idns);
  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesChecksumList::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onMetadata() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesChecksumList node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
