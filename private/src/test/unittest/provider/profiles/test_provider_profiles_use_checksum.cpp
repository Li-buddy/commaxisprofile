#include "test_provider_profiles_use_checksum.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/mock_axisprofile_property.h"

// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_use_checksum.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderProfilesUseChecksum);
REGISTER_TEST_TO_GROUP(TestProviderProfilesUseChecksum, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesUseChecksum, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesUseChecksum, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderProfilesUseChecksum::doInit()
{
  // Reset to wildcard base path on every init to keep this setup idempotent.
  m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_USE_CHECKSUM;
  m_invalidNodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_USE_CHECKSUM;

  if (!test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName))
  {
    return E_FAIL;
  }

  if (!test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist"))
  {
    return E_FAIL;
  }
  return S_OK;
}


HRESULT TestProviderProfilesUseChecksum::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to set/get checksum switch.");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnWrite();
  hResult |= testOnMetadata();

  return hResult;
}

HRESULT TestProviderProfilesUseChecksum::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesUseChecksum nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesUseChecksum node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  IAxisProfile* profile = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str());
  if (!profile)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }
  
  IAxisProfileInternal* tempProfile = static_cast<IAxisProfileInternal*>(profile);
  tempProfile->getPropertyInterface()->enableChecksum(true);

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData{};
  expData.setValue(true);
  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\n");
  return hResult;
}


HRESULT TestProviderProfilesUseChecksum::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesUseChecksum node(mockAxisProfileCmp.getProviderManager());
  
  log(L"\nCall onWrite() when profile does not exist.");
  comm::datalayer::Variant data{};
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false));

  log(L"\nCreate Soe profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  log(L"\nCall onWrite() with invalid profile.");
  data.setType(comm::datalayer::VariantType::BOOL8);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_invalidNodeAddress, &data, false));

  log(L"\nCall onWrite() with invalid variant type.");
  data.setType(comm::datalayer::VariantType::INT32);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false));

  log(L"\nCall onWrite() when normal case.");
  data.setType(comm::datalayer::VariantType::BOOL8);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data)); 
  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesUseChecksum::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesUseChecksum node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));
  log(L"\n");
  return hResult;
}
