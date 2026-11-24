#include "test_provider_profiles_set_uri.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_set_uri.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderProfilesSetURI);
REGISTER_TEST_TO_GROUP(TestProviderProfilesSetURI, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesSetURI, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesSetURI, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderProfilesSetURI::TestProviderProfilesSetURI()
{
}


TestProviderProfilesSetURI::~TestProviderProfilesSetURI(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderProfilesSetURI::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddressIn, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_nodeAddressOut, m_profileName);
  return S_OK;
}


HRESULT TestProviderProfilesSetURI::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to set profile related URIs ");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testErrors();
  hResult |= testNode();

  return hResult;
}


HRESULT TestProviderProfilesSetURI::testErrors()
{
  log(L"\n<b>Test all error cases.</b>");
  comm::datalayer::Variant data;

  log(L"\nCreate node without manager.");
  provider::NodeProfilesSetURI nodeA(nullptr, provider::NodeOperation::INBUFFER);

  log(L"\nCall onWrite() without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeA, m_nodeAddressIn, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCall onRead() without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeA, m_nodeAddressIn, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate mockups.");
  test::MockAxisProfileCmp mockComponent;

  log(L"\nCreate node with manager.");
  provider::NodeProfilesSetURI nodeB(mockComponent.getProviderManager(), provider::NodeOperation::INBUFFER);

  log(L"\nCall onWrite() with wrong variant type.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeB, m_nodeAddressIn, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite() when requested profile does not exist.");
  data.setType(comm::datalayer::VariantType::STRING);
  static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager())->m_returnValue = AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeB, m_nodeAddressIn, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeA, m_nodeAddressIn, &data, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  return S_OK;
}


HRESULT TestProviderProfilesSetURI::testNode()
{
  log(L"\n\n<b>Test successful node requests.</b>");

  log(L"\nCreate mockups.");
  test::MockAxisProfileCmp mockComponent;
  MockAxisProfileManagerSpecial* mockProfileManager = new MockAxisProfileManagerSpecial(); // Will be deleted by the component
  mockComponent.setProfileManager(true, mockProfileManager);

  log(L"\nCreate profile for test.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "testProfile");

  log(L"\nCreate node with all possible node operations.");
  std::map< std::string, provider::NodeProfilesSetURI*> testNodes;
  provider::NodeProfilesSetURI nodeInBuffer(mockComponent.getProviderManager(), provider::NodeOperation::INBUFFER);
  testNodes["INBUFFER"] = &nodeInBuffer;
  provider::NodeProfilesSetURI nodeOutBuffer(mockComponent.getProviderManager(), provider::NodeOperation::OUTBUFFER); 
  testNodes["OUTBUFFER"] = &nodeOutBuffer;

  for (auto &elem : testNodes)
  {
    log(L"\n\nNow testing Node for operation %ls", UTF8TOWIDE(elem.first));

    log(L"\nCall onWrite() to set a new URI.");
    comm::datalayer::Variant writeData;
    std::string testURI(mockProfileManager->getProfileInputBuffer(nullptr));
    writeData.setType(comm::datalayer::VariantType::STRING);
    writeData.setValue(testURI);
    PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, elem.second, m_nodeAddressIn, &writeData));

    log(L"\nCall onRead() to check the written data.");
    comm::datalayer::Variant readData{};
    PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, elem.second, m_nodeAddressIn, &readData, true, comm::datalayer::DlResult::DL_OK, &writeData));

    log(L"\nCall onRead() to check the written data.");
    PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onMetadata(this, elem.second, m_nodeAddressIn));
  }

  log(L"\n");
  testNodes.clear();

  return S_OK;
}
