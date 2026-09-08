#include "test_provider_node_profiles.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/node_profiles.h"
#include "comm/axisprofile/datatypes/profile_generated.h"

using namespace comm::axisprofile;


REGISTER_TEST(TestProviderNodeProfiles);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfiles, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfiles, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfiles, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderNodeProfiles::TestProviderNodeProfiles()
{
}


TestProviderNodeProfiles::~TestProviderNodeProfiles(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderNodeProfiles::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the general datalayer node of all existing axis profiles. ");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testOnCreate();
  hResult |= testOnRemove();
  hResult |= testOnBrowse();
  hResult |= testOnMetaData();

  return hResult;
}


HRESULT TestProviderNodeProfiles::testOnCreate()
{
  log(L"\n<b>Test method onCreate().</b>");

  log(L"\nCreate AxisProfileComponent mockup.");
  test::MockAxisProfileCmp mockComponent;

  log(L"\nCreate node.");
  provider::NodeProfiles node(mockComponent.getProviderManager());

  log(L"\nCall onCreate() with wrong variant.");
  comm::datalayer::Variant data{};
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onCreate(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  // @brief test case of profile naming
  struct PrfileNameCaseT
  {
    std::wstring m_caseName;   // name of test case
    std::string m_profileName; // name of the profile to be tested
    comm::datalayer::DlResult m_expResult; // expected return value
  };
  std::vector<PrfileNameCaseT> profileNameCases {
    {L"empty name",             "",                                    comm::datalayer::DlResult::DL_INVALID_VALUE},
    {L"name is too long",       "a234567890123456789012345678901234567890123456789012345678901234",  comm::datalayer::DlResult::DL_INVALID_VALUE},   // 64 chars (but only 63 chars allowed)
    {L"invalid 1st character",  "$$",                                  comm::datalayer::DlResult::DL_INVALID_VALUE},
    {L"invalid character",      "a$$",                                 comm::datalayer::DlResult::DL_INVALID_VALUE},
  };

  for (const auto& it : profileNameCases)
  {
    log(L"\nTest profile naming of %s.", it.m_caseName.c_str());
    flatbuffers::FlatBufferBuilder builder;
    auto profileNameOffset = builder.CreateString(it.m_profileName);
    fbtypes::profileBuilder profileFbBuilder(builder);
    profileFbBuilder.add_name(profileNameOffset);
    profileFbBuilder.add_type(fbtypes::ProfileType::ProfileType_Servodrive_over_Ethercat);
    builder.Finish(profileFbBuilder.Finish());
    data.copyFlatbuffers(builder);
    PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onCreate(this, &node, m_nodeAddress, &data, false, it.m_expResult));
  }

  std::string profileName = "testProfile";
  log(L"\nCreate correct flatbuffer type with missing field.");
  flatbuffers::FlatBufferBuilder builder;
  auto profileNameOffset = builder.CreateString(profileName);
  fbtypes::profileBuilder profileFbBuilder(builder);
  profileFbBuilder.add_name(profileNameOffset);
  builder.Finish(profileFbBuilder.Finish());
  data.copyFlatbuffers(builder);

  log(L"\nCall onCreate() with variant with missing field.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onCreate(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCreate correct flatbuffer for onCreate() request.");
  log(L"\nProfile name: %ls, Profile type: %ls", UTF8TOWIDE(profileName), UTF8TOWIDE(fbtypes::EnumNameProfileType(fbtypes::ProfileType::ProfileType_Servodrive_over_Ethercat)));
  comm::datalayer::Variant data2{};
  flatbuffers::FlatBufferBuilder builder2;
  profileNameOffset = builder2.CreateString(profileName);
  fbtypes::profileBuilder profileFbBuilder2(builder2);
  profileFbBuilder2.add_name(profileNameOffset);
  profileFbBuilder2.add_type(fbtypes::ProfileType::ProfileType_Servodrive_over_Ethercat);
  builder2.Finish(profileFbBuilder2.Finish());
  data2.copyFlatbuffers(builder2);

  log(L"\nCreate node without provider manager.");
  provider::NodeProfiles nodeNoMgr(nullptr);

  log(L"\nCall onCreate() without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onCreate(this, &nodeNoMgr, m_nodeAddress, &data2, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCall onCreate() and simulate error in profile manager.");
  static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager())->m_returnValue = AxisProfileResult::ERR_NOK;
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onCreate(this, &node, m_nodeAddress, &data2, false, comm::datalayer::DlResult::DL_CREATION_FAILED));

  log(L"\nCall onCreate() when everything is OK.");
  static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager())->m_returnValue = AxisProfileResult::STS_OK;
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onCreate(this, &node, m_nodeAddress, &data2, true));

  log(L"\n");
  return S_OK;
}

HRESULT TestProviderNodeProfiles::testOnRemove()
{
  log(L"\n<b>Test method onRemove().</b>");

  log(L"\nCreate AxisProfileComponent mockup.");
  test::MockAxisProfileCmp mockComponent;

  log(L"\nCreate node without provider manager.");
  provider::NodeProfiles nodeNoMgr(nullptr);

  log(L"\nCall onRemove() without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRemove(this, &nodeNoMgr, m_nodeAddress, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node.");
  provider::NodeProfiles node(mockComponent.getProviderManager());

  log(L"\nCall onRemove() and simulate error in profile manager.");
  static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager())->m_returnValue = AxisProfileResult::ERR_NOK;
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRemove(this, &node, m_nodeAddress, false, comm::datalayer::DlResult::DL_FAILED));

  std::string profileName = "testProfile";
  log(L"\nAdd profile '%ls' for next test.", UTF8TOWIDE(profileName));
  static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager())->m_returnValue = AxisProfileResult::STS_OK;
  mockComponent.getAxisProfileManager()->createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName);

  log(L"\nCall onRemove() when everything is OK.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRemove(this, &node, (m_nodeAddress + '/' + profileName), true));

  log(L"\n");
  return S_OK;
}

HRESULT TestProviderNodeProfiles::testOnBrowse()
{
  log(L"\n<b>Test method onBrowse().</b>");

  log(L"\nCreate node without provider manager.");
  provider::NodeProfiles nodeNoMgr(nullptr);

  log(L"\nCall onBrowse() without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onBrowse(this, &nodeNoMgr, m_nodeAddress, false, nullptr, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate AxisProfileComponent mockup.");
  test::MockAxisProfileCmp mockComponent;

  log(L"\nDelete AxisProfileManager in the component mockup for next test.");
  mockComponent.setProfileManager(false);

  log(L"\nCreate node.");
  provider::NodeProfiles node(mockComponent.getProviderManager());

  log(L"\nCall onBrowse() when profile manager is a nullptr.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onBrowse(this, &node, m_nodeAddress, false, nullptr, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nRecreate AxisProfileManager in the component mockup for next test.");
  mockComponent.setProfileManager(true);

  std::string profileName1 = "testProfile1";
  std::string profileName2 = "testProfile2";
  static_cast<test::MockAxisProfileManager*>(mockComponent.getAxisProfileManager())->m_returnValue = AxisProfileResult::STS_OK;
  log(L"\nAdd profile '%ls' for next test.", UTF8TOWIDE(profileName1));
  mockComponent.getAxisProfileManager()->createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName1);
  log(L"\nAdd profile '%ls' for next test.", UTF8TOWIDE(profileName2));
  mockComponent.getAxisProfileManager()->createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, profileName2);

  log(L"\nCall onBrowse().");
  comm::datalayer::Variant data{};
  std::vector<std::string> expectedData = { profileName1, profileName2 };
  data.setValue(expectedData);
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onBrowse(this, &node, m_nodeAddress, true, &data));

  log(L"\n");
  return S_OK;
}

HRESULT TestProviderNodeProfiles::testOnMetaData()
{
  log(L"\n<b>Test method onMetadata().</b>");

  log(L"\nCreate AxisProfileComponent mockup.");
  test::MockAxisProfileCmp mockComponent;

  log(L"\nCreate node.");
  provider::NodeProfiles node(mockComponent.getProviderManager());

  log(L"\nCall onMetadata().");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true));

  log(L"\nCall getBfbsName().");
  std::string bfbsName = node.getBfbsName();
  if (bfbsName.empty())
  {
    error(L"\nDid not receive a bfbs name from node!");
    return E_FAIL;
  }

  log(L"\nCall getDescriptionUrl().");
  node.getDescriptionUrl();  

  log(L"\nCall getDescription().");
  std::string desc = node.getDescription();
  if (desc.empty())
  {
    error(L"\nDid not receive a description from node!");
    return E_FAIL;
  }

  log(L"\n");
  return S_OK;
}
