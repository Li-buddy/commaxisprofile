
#include "test_provider_node_profiles_drive_number.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_drive_number.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderProfilesDriveNumber);
REGISTER_TEST_TO_GROUP(TestProviderProfilesDriveNumber, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesDriveNumber, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesDriveNumber, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderProfilesDriveNumber::doInit()
{
  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");
  return S_OK;
}


HRESULT TestProviderProfilesDriveNumber::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to set/get the drive number.");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnWrite();
  hResult |= testOnMetadata();

  return hResult;
}


HRESULT TestProviderProfilesDriveNumber::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesDriveNumber nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesDriveNumber node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  std::uint8_t driveNumber = 10;
  log(L"\nSet drive number to %d.", driveNumber);
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));

  if (!profile)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }
  profile->setDriveNumber(driveNumber);

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData{};
  expData.setValue(driveNumber);
  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\n");
  return hResult;
}


HRESULT TestProviderProfilesDriveNumber::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesDriveNumber node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onWrite() with invalid variant type.");
  comm::datalayer::Variant data{};
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite() when profile does not exist.");
  data.setType(comm::datalayer::VariantType::UINT8);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_invalidNodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  log(L"\nCall onWrite() when everything is OK.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderProfilesDriveNumber::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeProfilesDriveNumber node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
