#include "test_provider_profiles_input_mapping.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_input_mapping.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderProfilesInputMapping);
REGISTER_TEST_TO_GROUP(TestProviderProfilesInputMapping, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesInputMapping, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesInputMapping, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderProfilesInputMapping::TestProviderProfilesInputMapping()
{
}


TestProviderProfilesInputMapping::~TestProviderProfilesInputMapping(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderProfilesInputMapping::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  return S_OK;
}

HRESULT TestProviderProfilesInputMapping::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the datalayer node, used to read/write the complete input mapping. ");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testErrors();
  hResult |= testNode();

  return hResult;
}

HRESULT TestProviderProfilesInputMapping::testErrors()
{
  log(L"\n<b>----- Test all possible errors -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesInputMapping nodeNoMgr(nullptr);

  log(L"\nCall onRead() without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeNoMgr, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCall onWrite() without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeNoMgr, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockComponent;
  provider::NodeProfilesInputMapping node(mockComponent.getProviderManager());

  log(L"\nCreate profile for further testing.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);
  IAxisProfileInternal* testProfile = static_cast<IAxisProfileInternal*>(mockComponent.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));

  log(L"\nCall onWrite() with wrong variant type.");
  comm::datalayer::Variant data {};
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  data.setType(comm::datalayer::VariantType::FLATBUFFERS);
  log(L"\nCall onWrite with correct variant type (flatbuffer) but wrong structure (verify fails).");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite with with flatbuffer that has a missing field (isFieldPresent fails).");
  data = createMappingFB(true);
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite() and simulate an error in the mapping handler.");
  data = createMappingFB();

  static_cast<test::MockAxisProfileMappingHandler*>(testProfile->getMappingHandler())->setReturnValue(AxisProfileResult::ERR_NOK);
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  return S_OK;
}

HRESULT TestProviderProfilesInputMapping::testNode()
{
  log(L"\n\n<b>----- Test all class methods -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockComponent;
  provider::NodeProfilesInputMapping node(mockComponent.getProviderManager());

  log(L"\nCall getBfbsName().");
  const char* bfbsName = node.getBfbsName();
  if (!bfbsName)
  {
    error(L"Could not read bfbs name!");
  }

  log(L"\nCall getDescriptionUrl().");
  const char* descrUrl = node.getDescriptionUrl();
  if (!descrUrl)
  {
    error(L"Could not read description URL!");
  }

  log(L"\nCall getDescription().");
  const char* descr = node.getDescription();
  if (!descr)
  {
    error(L"Could not read description!");
  }

  log(L"\nCreate profile for further testing.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);
  test::MockAxisProfile* testProfile = static_cast<test::MockAxisProfile*>(mockComponent.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));

  log(L"\nCreate simple input mapping for further testing.");
  MockAxisProfileMappingHandlerSpecial* testMappingHandler = new MockAxisProfileMappingHandlerSpecial(); // Will be deleted by the component
  testMappingHandler->m_inputMapping[InputMappingValueID::POSITION] = "PositionURI";
  testMappingHandler->m_inputMapping[InputMappingValueID::STATUSWORD] = "StatuswordURI";
  testMappingHandler->m_inputMappingCfgInfo[InputMappingValueID::POSITION] = {MappingGroup::BASE, false};
  testMappingHandler->m_inputMappingCfgInfo[InputMappingValueID::STATUSWORD] = {MappingGroup::BASE, true};
  testProfile->setMappingHandler(true, testMappingHandler);

  log(L"\nCall onMetadata().");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress));

  log(L"\nCall onWrite().");
  comm::datalayer::Variant writeData = createMappingFB(false);
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &writeData));

  log(L"\nCall onRead().");
  comm::datalayer::Variant readData;
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &writeData, fbtypes::VerifyvalueMappingBuffer));

  log(L"\n");
  return S_OK;
}

comm::datalayer::Variant TestProviderProfilesInputMapping::createMappingFB(bool leaveFieldEmpty)
{
  // Order of vector elements is important to be able to compare the flatbuffer!
  std::vector<comm::axisprofile::InputMappingEntry> inputValueMapping{};
  inputValueMapping.push_back({ InputMappingValueID::STATUSWORD, "StatuswordURI", MappingGroup::BASE, true });
  inputValueMapping.push_back({ InputMappingValueID::POSITION, "PositionURI", MappingGroup::BASE, false });

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto offset = createInputMappingFlatbuffer(builder, inputValueMapping, leaveFieldEmpty);
  builder.Finish(offset);
  comm::datalayer::Variant variantData;
  variantData.copyFlatbuffers(builder);
  return variantData;
}

flatbuffers::Offset<comm::axisprofile::fbtypes::valueMapping> TestProviderProfilesInputMapping::createInputMappingFlatbuffer(flatbuffers::FlatBufferBuilder& builder,
                                                                                                                             std::vector<comm::axisprofile::InputMappingEntry>& inputMapping, bool leaveFieldEmpty)
{
  std::vector<flatbuffers::Offset<fbtypes::mappingEntry>> inputMappingOffsets;
  for (auto& mappingEntry : inputMapping)
  {
    auto mappingStringId = builder.CreateString(inputMappingValueIdToString(mappingEntry.m_id));
    auto mappingStringUri = builder.CreateString(mappingEntry.m_uri);

    comm::axisprofile::fbtypes::mappingEntryBuilder entryBuilder(builder);
    entryBuilder.add_valueID(mappingStringId);
    entryBuilder.add_datalayerURI(mappingStringUri);
    entryBuilder.add_profileVar(static_cast<fbtypes::variableType>(mappingEntry.m_group));
    entryBuilder.add_required(mappingEntry.m_bRequired);
    auto entry = entryBuilder.Finish();
    inputMappingOffsets.push_back(entry);
  }

  auto sortedMappingIn = builder.CreateVectorOfSortedTables(&inputMappingOffsets);
  comm::axisprofile::fbtypes::valueMappingBuilder mappingBuilderInput(builder);
  if (!leaveFieldEmpty)
  {
    mappingBuilderInput.add_mapping(sortedMappingIn);
  }
  return mappingBuilderInput.Finish();
}
