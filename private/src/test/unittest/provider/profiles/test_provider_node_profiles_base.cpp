#include "test_provider_node_profiles_base.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/node_profiles_base.h"



using namespace comm::axisprofile;

REGISTER_TEST(TestProviderNodeProfilesBase);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesBase, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesBase, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesBase, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderNodeProfilesBase::TestProviderNodeProfilesBase()
{
}


TestProviderNodeProfilesBase::~TestProviderNodeProfilesBase(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderNodeProfilesBase::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  return S_OK;
}


HRESULT TestProviderNodeProfilesBase::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the base class for all profile datalayer nodes.");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testErrors();
  hResult |= testNode();

  return hResult;
}


HRESULT TestProviderNodeProfilesBase::testErrors()
{
  log(L"\n<b>Test all error cases.</b>");
  comm::datalayer::Variant data;

  log(L"\nCreate node without provider manager.");
  provider::NodeProfilesBase nodeNoMgr(nullptr);

  log(L"\nCall getProfileByAddress() without provider manager.");
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::DL_OK;
  nodeNoMgr.getProfileByAddress("dummyString", test::ProviderHelperUnittest::m_callback);
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_INVALID_HANDLE)
  {
    error(L"\nExpected return Code 'Invalid Handle' but received %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  log(L"\nCall getProfileManager() without provider manager.");
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::DL_OK;
  nodeNoMgr.getProfileManager(test::ProviderHelperUnittest::m_callback);
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_INVALID_HANDLE)
  {
    error(L"\nExpected return Code 'Invalid Handle' but received %l", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  log(L"\nCall onMetadata() without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onMetadata(this, &nodeNoMgr, m_nodeAddress, false, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with provider manager.");
  test::MockAxisProfileCmp mockComponent;
  provider::NodeProfilesBase node(mockComponent.getProviderManager());

  log(L"\nCall getProfileByAddress() when requested profile does not exist.");
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::DL_OK;
  node.getProfileByAddress("dummyString", test::ProviderHelperUnittest::m_callback);
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_INVALID_HANDLE)
  {
    error(L"\nExpected return Code 'Invalid Handle' but received %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  log(L"\nCall onMetadataBasicType() when requested profile does not exist.");
  node.onMetadataBasicType("dummyString", test::ProviderHelperUnittest::m_callback, comm::datalayer::VariantType::ARRAY_OF_INT8);
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_INVALID_HANDLE)
  {
    error(L"\nExpected return Code 'ERR_PROFILE_NOT_FOUND' but received %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  return S_OK;
}


HRESULT TestProviderNodeProfilesBase::testNode()
{
  log(L"\n\n<b>Test successful node request.</b>");

  log(L"\nCreate mockups.");
  test::MockAxisProfileCmp mockComponent;

  log(L"\nCreate node.");
  provider::NodeProfilesBase node(mockComponent.getProviderManager());

  log(L"\nCreate profile for test.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "testProfile");

  log(L"\nCall getProfileByAddress().");
  const IAxisProfile* returnedProfile = node.getProfileByAddress(m_nodeAddress, test::ProviderHelperUnittest::m_callback);
  if (!returnedProfile)
  {
    error(L"\nExpected success but received null-pointer!");
    return E_FAIL;
  }

  log(L"\nCall getProfileManager().");
  const IAxisProfileManagerInternal* returnedMgr = node.getProfileManager(test::ProviderHelperUnittest::m_callback);
  if (!returnedMgr)
  {
    error(L"\nExpected success but received null-pointer!");
    return E_FAIL;
  }

  log(L"\nAdd profile '%ls' for next test.", UTF8TOWIDE(m_profileName));
  mockComponent.getAxisProfileManager()->createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  log(L"\nCall onMetadata() when everything is OK.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, false));

  log(L"\nCall onMetadataBasicType() when type is error.");
  node.onMetadataBasicType(m_nodeAddress, test::ProviderHelperUnittest::m_callback, comm::datalayer::VariantType::FLATBUFFERS);
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_TYPE_MISMATCH)
  {
    error(L"\nExpected return Code 'ERR_PROFILE_NOT_FOUND' but received %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  log(L"\n");
  return S_OK;
}
