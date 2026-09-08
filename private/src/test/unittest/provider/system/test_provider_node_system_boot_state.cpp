#include "test_provider_node_system_boot_state.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/system/node_system_boot_state.h"

#include "comm/axisprofile/datatypes/aprBootState_generated.h"
using namespace comm::axisprofile;


REGISTER_TEST(TestProviderNodeSystemBootState);
REGISTER_TEST_TO_GROUP(TestProviderNodeSystemBootState, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeSystemBootState, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeSystemBootState, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderNodeSystemBootState::TestProviderNodeSystemBootState()
{
}


TestProviderNodeSystemBootState::~TestProviderNodeSystemBootState(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderNodeSystemBootState::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  return S_OK;
}


HRESULT TestProviderNodeSystemBootState::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the general datalayer node of all existing axis profiles. ");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testNode();
  hResult |= testErrors();

  return hResult;
}


HRESULT TestProviderNodeSystemBootState::testErrors()
{
  log(L"\n<b>----- Test all possible errors -----</b>");
  HRESULT hResult = S_OK;

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockComponent;
  provider::NodeSystemBootState node(mockComponent.getProviderManager());

  log(L"\nCreate profile for testing.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  log(L"\n<b>Test the onMetadata method");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, false, comm::datalayer::DlResult::DL_OK));

  log(L"\nCall onRead() when profile does not exist.");
  comm::datalayer::Variant data;
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &data, true, comm::datalayer::DlResult::DL_OK, &data, fbtypes::VerifyAPRBootStateBuffer));
  
  mockComponent.setBootManager(false, mockComponent.getBootManager());
  log(L"\nCall onRead() with invalid variant type.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_OPERATION_MODE));


  log(L"\n");
  return hResult;
}


HRESULT TestProviderNodeSystemBootState::testNode()
{
  log(L"\n<b>----- Test all class methods -----</b>");
  HRESULT hResult = S_OK;

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockComponent;
  provider::NodeSystemBootState node(mockComponent.getProviderManager());

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

  log(L"\n");
  return hResult;
}
