#include "test_provider_node_profiles_config.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_config.h"

using namespace comm::axisprofile;


REGISTER_TEST(TestProviderNodeProfilesConfig);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesConfig, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesConfig, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesConfig, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderNodeProfilesConfig::TestProviderNodeProfilesConfig()
{
}


TestProviderNodeProfilesConfig::~TestProviderNodeProfilesConfig(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderNodeProfilesConfig::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  return S_OK;
}


HRESULT TestProviderNodeProfilesConfig::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the general datalayer node of all existing axis profiles. ");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testNode();
  hResult |= testErrors();

  return hResult;
}


HRESULT TestProviderNodeProfilesConfig::testErrors()
{
  log(L"\n<b>----- Test all possible errors -----</b>");
  HRESULT hResult = S_OK;

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockComponent;
  provider::NodeProfilesConfig node(mockComponent.getProviderManager());

  log(L"\nCall onMetadata() when profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, false, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCall onWrite() when profile does not exist.");
  comm::datalayer::Variant data;
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_ADDRESS));

  log(L"\nCreate profile for further testing.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  log(L"\nCall onWrite() with invalid variant type.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  //log(L"\nCall onWrite() with invalid variant data (one field missing).");
  //data.setType(comm::datalayer::VariantType::FLATBUFFERS);
  //data = createProfileConfigFlatbuffer(true);
  //PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\n");
  return hResult;
}


HRESULT TestProviderNodeProfilesConfig::testNode()
{
  log(L"\n<b>----- Test all class methods -----</b>");
  HRESULT hResult = S_OK;

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockComponent;
  provider::NodeProfilesConfig node(mockComponent.getProviderManager());

  log(L"\nCall getBfbsName().");
  const char* bfbsName = node.getBfbsName();
  if ( !bfbsName )
  {
    hResult = E_FAIL;
    error(L"Could not read bfbs name!");
  }

  log(L"\nCall getDescriptionUrl().");
  const char* descrUrl = node.getDescriptionUrl();
  if ( !descrUrl )
  {
    hResult = E_FAIL;
    error(L"Could not read description URL!");
  }

  log(L"\nCall getDescription().");
  const char* descr = node.getDescription();
  if ( !descr )
  {
    hResult = E_FAIL;
    error(L"Could not read description!");
  }

  log(L"\nCreate profile for further testing.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT,m_profileName);

  log(L"\nCall onMetadata().");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress));

  //log(L"\nWrite configuration to testprofile.");
  //comm::datalayer::Variant writeData = createProfileConfigFlatbuffer(false);
  //PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &writeData));

  //log(L"\nRead configuration from testprofile.");
  //comm::datalayer::Variant readData;
  //PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &writeData, fbtypes::VerifyprofileConfigBuffer));
  //if(S_OK != compareFBs(&readData, &writeData))
  //{
  //  return E_FAIL;
  //}

  log(L"\n");
  return hResult;
}
