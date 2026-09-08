#include "test_provider_system_use_checksum.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/mock_axisprofile_property.h"

// Test Class
#include "provider/nodes/system/node_system_use_checksum.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderSystemUseChecksum);
REGISTER_TEST_TO_GROUP(TestProviderSystemUseChecksum, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderSystemUseChecksum, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderSystemUseChecksum, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderSystemUseChecksum::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to set/get checksum switch of all profiles.");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnWrite();
  hResult |= testOnMetadata();

  return hResult;
}


HRESULT TestProviderSystemUseChecksum::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeSystemUseChecksum nodeNoManager(nullptr);

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeSystemUseChecksum node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when no profiles exist.");
  comm::datalayer::Variant expData {};
  expData.setValue(false);
  comm::datalayer::Variant readData {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName1));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName1);
  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName2));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName2);

  IAxisProfile* profile1 = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName1.c_str());
  IAxisProfile* profile2 = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName2.c_str());
  IAxisProfileInternal* tempProfile1 = static_cast<IAxisProfileInternal*>(profile1);
  IAxisProfileInternal* tempProfile2 = static_cast<IAxisProfileInternal*>(profile2);
  if (nullptr == tempProfile1 || nullptr == tempProfile2)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }

  log(L"\nCall onRead() when all profiles' use-checksum is enable.");
  tempProfile1->getPropertyInterface()->enableChecksum(true);
  tempProfile2->getPropertyInterface()->enableChecksum(true);
  expData.setValue(true);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\nCall onRead() when one of the profiles' use-checksum is disable.");
  tempProfile1->getPropertyInterface()->enableChecksum(false);
  tempProfile2->getPropertyInterface()->enableChecksum(true);
  expData.setValue(false);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\n");
  return hResult;
}


HRESULT TestProviderSystemUseChecksum::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeSystemUseChecksum node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onWrite() when no profiles exist.");;
  comm::datalayer::Variant data {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName1));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName1);
  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName2));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName2);

  log(L"\nCall onWrite() with invalid variant type.");
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false));

  data.setType(comm::datalayer::VariantType::INT32);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false));
 
  log(L"\nCall onWrite() when normal case.");
  data.setType(comm::datalayer::VariantType::BOOL8);
  data.setValue(false);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data)); 
  log(L"\n");
  return hResult;
}

HRESULT TestProviderSystemUseChecksum::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method OnMetadata() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeSystemUseChecksum node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName1));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName1);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
