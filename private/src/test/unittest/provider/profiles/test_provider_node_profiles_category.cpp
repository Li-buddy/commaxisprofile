/// @file
///
/// This source file is part of the axisprofile library and implement
/// the node for profile category.
///
/// @copyright (C) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_provider_node_profiles_category.h"
#include "helper/provider_helper_unittest.h"

/// Mockups
#include "mockup/mock_axisprofile_cmp.h"

/// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_category.h"

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

REGISTER_TEST(TestProviderNodeProfilesCategory);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesCategory, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesCategory, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesCategory, Provider);

HRESULT TestProviderNodeProfilesCategory::doInit()
{
  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");
  return S_OK;
}

HRESULT TestProviderNodeProfilesCategory::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to set/get scaling strategy.");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnMetadata();
  hResult |= testOnWrite();

  return hResult;
}

HRESULT TestProviderNodeProfilesCategory::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesCategory nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp {};
  provider::NodeProfilesCategory node(mockAxisProfileCmp.getProviderManager());

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

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData {};
  comm::axisprofile::AxisProfileCategory profileCategory = profile->getProfileCategory();

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto offsetType = node.generateFlatbufferData(builder, profileCategory);

  builder.Finish(offsetType);
  expData.copyFlatbuffers(builder);

  comm::datalayer::Variant readData {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true,
                                                      comm::datalayer::DlResult::DL_OK, &expData, fbtypes::VerifyProfileCategoryBuffer));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeProfilesCategory::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp {};
  provider::NodeProfilesCategory node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeProfilesCategory::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");
  test::MockAxisProfileCmp mockAxisProfileCmp {};
  provider::NodeProfilesCategory node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onWrite() when profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile for further testing.");
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);
  log(L"\nCall onWrite() with invalid variant type.");
  comm::datalayer::Variant data {};
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite with with flatbuffer that has a missing field (isFieldPresent fails).");
  data = createFBData(true);
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite() when everything is OK.");
  data = createFBData();
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\n");
  return hResult;
}

comm::datalayer::Variant TestProviderNodeProfilesCategory::createFBData(bool bLeaveFieldEmpty)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  //create profile data
  fbtypes::ProfileCategoryBuilder categoryBuilder(builder);
  if (!bLeaveFieldEmpty)
  {
    categoryBuilder.add_category(fbtypes::CategoryOfProfile::CategoryOfProfile_CATEGORY_DRIVE);
  }

  builder.Finish(categoryBuilder.Finish());
  //get variant data
  comm::datalayer::Variant variantData;
  variantData.copyFlatbuffers(builder);
  return variantData;
}
