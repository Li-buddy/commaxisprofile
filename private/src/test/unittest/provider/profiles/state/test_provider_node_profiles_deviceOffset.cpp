// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_provider_node_profiles_deviceOffset.h"
#include "helper/provider_helper_unittest.h"
#include "provider/nodes/profiles/state/node_profiles_device_offset.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

REGISTER_TEST(TestProviderProfilesDeviceOffset);
REGISTER_TEST_TO_GROUP(TestProviderProfilesDeviceOffset, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesDeviceOffset, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesDeviceOffset, Provider);

HRESULT TestProviderProfilesDeviceOffset::doInit()
{
  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");
  return S_OK;
}

HRESULT TestProviderProfilesDeviceOffset::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n--------------------------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to get the offset between control and device.");
  log(L"\n--------------------------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnMetadata();

  return hResult;
}

HRESULT TestProviderProfilesDeviceOffset::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesDeviceOffset nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesDeviceOffset node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  log(L"\nSet device offset.");
  IAxisProfile* tempProfile = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str());
  if ( !tempProfile )
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }  
  auto profile = static_cast<IAxisProfileInternal*>(tempProfile);
  
  profile->getScaling()->setHomeOffset(1);
  double offset = profile->getScaling()->convertPositionRawToScaled(0);
  
  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData{};
  expData.setValue(offset);
  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesDeviceOffset::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onMetadata() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesDeviceOffset node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
