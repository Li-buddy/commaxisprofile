#include <thread>
#include <limits>
#include "functest_provider_configuration.h"
#include "provider/provider_helper.h"
#include "profiles/scaling/i_scaling.h"
#include "helper/provider_helper_functest.h"
#include "comm/axisprofile/datatypes/valueMapping_generated.h"


using namespace comm::axisprofile;

REGISTER_TEST(FuncTestProviderConfiguration);
REGISTER_TEST_TO_GROUP(FuncTestProviderConfiguration, 1_AutoTest);
REGISTER_TEST_TO_GROUP(FuncTestProviderConfiguration, 3_FuncTest);
REGISTER_TEST_TO_GROUP(FuncTestProviderConfiguration, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

FuncTestProviderConfiguration::FuncTestProviderConfiguration()
{
}


FuncTestProviderConfiguration::~FuncTestProviderConfiguration(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT FuncTestProviderConfiguration::doInit()
{
  return startTestEnvironment();
}

HRESULT FuncTestProviderConfiguration::doEnd()
{
  return stopTestEnvironment();
}

HRESULT FuncTestProviderConfiguration::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Functiontest for profile configuration via datalayer.");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= testCreateOfProfiles();

  return hResult;
}

HRESULT FuncTestProviderConfiguration::testCreateOfProfiles()
{
  HRESULT hresult = S_OK;

  log(L"\n<b>Create and configure profiles via associated datalayer nodes.</b>");

  HRESULT result = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  if (FAILED(result))
  {
    return result;
  }

  log(L"\nGet all profiles from system via datalayer (should be empty).");
  std::string urlBrowse = comm::axisprofile::provider::NodeTree.APR_PROFILES;
  comm::datalayer::Variant browseData;
  comm::datalayer::Variant expDataBrowse;
  test::ProviderHelperFunctest::browseNodeSync(this, m_datalayerTestClient, urlBrowse.c_str(), &browseData, true, &expDataBrowse);

  const std::string profileName = "testProfile";
  IAxisProfileInternal* testProfile1{};
  result = addProfileToSystem(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, 1, profileName.c_str(), &testProfile1);
  if (FAILED(result))
  {
    return result;
  }

  const std::string profileName2 = "testProfile2";
  IAxisProfileInternal* testProfile2{};
  result = addProfileToSystem(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, 1, profileName2.c_str(), &testProfile2);
  if (FAILED(result))
  {
    return result;
  }

  log(L"\nGet all profiles from system via datalayer again.");
  std::vector<std::string> expProfileNames{};
  expProfileNames.push_back(profileName);
  expProfileNames.push_back(profileName2);
  expDataBrowse.setValue(expProfileNames);
  test::ProviderHelperFunctest::browseNodeSync(this, m_datalayerTestClient, urlBrowse.c_str(), &browseData, true, &expDataBrowse);

  log(L"\nGet all profile subnodes from system via datalayer.");
  std::string urlBrowseProfile = comm::axisprofile::provider::NodeTree.APR_PROFILES_WC;
  test::ProviderHelperCommon::insertProfileName(urlBrowseProfile, profileName);

  std::vector<std::string> expNodes = { provider::NodeTree.CFG, provider::NodeTree.STATE};
  for (auto& nodeAddress : expNodes)
  {
    test::ProviderHelperCommon::removeLeadingSlash(&nodeAddress);
  }
  expDataBrowse.setValue(expNodes);
  test::ProviderHelperFunctest::browseNodeSync(this, m_datalayerTestClient, urlBrowseProfile.c_str(), &browseData, true, &expDataBrowse);

  log(L"\nConfigure the input buffer URI of the test profile via datalayer.");
  std::string urlInputBuffer = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_INBUFFER;
  test::ProviderHelperCommon::insertProfileName(urlInputBuffer, profileName);
  std::string inputAdress = "test/at";
  comm::datalayer::Variant writeData;
  writeData.setValue(inputAdress);
  test::ProviderHelperFunctest::writeNodeSync(this, m_datalayerTestClient, urlInputBuffer.c_str(), &writeData, true);

  log(L"\nConfigure the output buffer URI of the test profile via datalayer.");
  std::string urlOutputBuffer = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_OUTBUFFER;
  test::ProviderHelperCommon::insertProfileName(urlOutputBuffer, profileName);
  std::string outputAdress = "test/mdt";
  writeData.setValue(outputAdress);
  test::ProviderHelperFunctest::writeNodeSync(this, m_datalayerTestClient, urlOutputBuffer.c_str(), &writeData, true);

  log(L"\nCheck the input configuration via datalayer.");
  comm::datalayer::Variant actData;
  comm::datalayer::Variant expData;
  expData.setValue(inputAdress);
  test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, urlInputBuffer.c_str(), &actData, true, &expData);

  log(L"\nCheck the output configuration via datalayer.");
  expData.setValue(outputAdress);
  test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, urlOutputBuffer.c_str(), &actData, true, &expData);

  log(L"\nConfigure the input mapping of the test profile via datalayer.");
  flatbuffers::FlatBufferBuilder builderInput;
  auto inputValueId = builderInput.CreateString("position");
  auto inputMappingUri = builderInput.CreateString("axis1/position");
  std::vector<flatbuffers::Offset<comm::axisprofile::fbtypes::mappingEntry>> inputMapping;

  comm::axisprofile::fbtypes::mappingEntryBuilder entryBuilderInput(builderInput);
  entryBuilderInput.add_datalayerURI(inputMappingUri);
  entryBuilderInput.add_valueID(inputValueId);
  auto inputMappingEntry = entryBuilderInput.Finish();
  inputMapping.push_back(inputMappingEntry);

  auto sortedMappingIn = builderInput.CreateVectorOfSortedTables(&inputMapping);
  comm::axisprofile::fbtypes::valueMappingBuilder mappingBuilderInput(builderInput);
  mappingBuilderInput.add_mapping(sortedMappingIn);
  builderInput.Finish(mappingBuilderInput.Finish());

  comm::datalayer::Variant inputMappingExp;
  inputMappingExp.copyFlatbuffers(builderInput);
  std::string urlMappingInput = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_INMAPPING;
  test::ProviderHelperCommon::insertProfileName(urlMappingInput, profileName);
  test::ProviderHelperFunctest::writeNodeSync(this, m_datalayerTestClient, urlMappingInput.c_str(), &inputMappingExp, true);

  log(L"\nConfigure the output mapping of the test profile via datalayer.");
  flatbuffers::FlatBufferBuilder builderOutput;
  auto outputValueId = builderOutput.CreateString("position");
  auto outputMappingUri = builderOutput.CreateString("axis1/position");
  std::vector<flatbuffers::Offset<comm::axisprofile::fbtypes::mappingEntry>> outputMapping;

  comm::axisprofile::fbtypes::mappingEntryBuilder entryBuilderOutput(builderOutput);
  entryBuilderOutput.add_valueID(outputValueId);
  entryBuilderOutput.add_datalayerURI(outputMappingUri);
  auto outputMappingEntry = entryBuilderOutput.Finish();
  outputMapping.push_back(outputMappingEntry);

  auto sortedMappingOut = builderOutput.CreateVectorOfSortedTables(&outputMapping);
  comm::axisprofile::fbtypes::valueMappingBuilder mappingBuilderOutput(builderOutput);
  mappingBuilderOutput.add_mapping(sortedMappingOut);
  builderOutput.Finish(mappingBuilderOutput.Finish());

  comm::datalayer::Variant outputMappingExp;
  outputMappingExp.copyFlatbuffers(builderOutput);
  std::string urlMappingOutput = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_OUTMAPPING;
  test::ProviderHelperCommon::insertProfileName(urlMappingOutput, profileName);
  test::ProviderHelperFunctest::writeNodeSync(this, m_datalayerTestClient, urlMappingOutput.c_str(), &outputMappingExp, true);

  log(L"\nRead the input mapping of the test profile via datalayer.");
  comm::datalayer::DlResult dlResult;
  log(L"\nReadSync: %ls", test::StringHelper::utf8ToWide(urlMappingInput).c_str());
  dlResult = m_datalayerTestClient->readSync(urlMappingInput.c_str(), &actData);
  if ( comm::datalayer::STATUS_FAILED(dlResult) )
  {
    error(L"Reading of uri %ls failed.", test::StringHelper::utf8ToWide(urlMappingInput).c_str());
  }

  log(L"\nCheck the received data.");
  flatbuffers::Verifier verifierIn(actData.getData(), actData.getSize());
  if ( !comm::axisprofile::fbtypes::VerifyvalueMappingBuffer(verifierIn) )
  {
    error(L"Verify of uri %ls failed", test::StringHelper::utf8ToWide(urlMappingInput).c_str());
  }
  auto variantDataIn = comm::axisprofile::fbtypes::GetvalueMapping(actData.getData());

  auto vecDataIn = variantDataIn->mapping();
  for ( auto elem = vecDataIn->begin(); elem != vecDataIn->end(); ++elem )
  {
    std::string valueIdStr = flatbuffers::GetString(elem->valueID()).c_str();
    std::string valueUriStr = flatbuffers::GetString(elem->datalayerURI()).c_str();
    if ( valueIdStr.compare("position") || valueUriStr.compare("axis1/position") )
    {
      error(L"\nID: %ls, Uri: %ls", test::StringHelper::utf8ToWide(flatbuffers::GetString(elem->valueID())).c_str(), test::StringHelper::utf8ToWide(flatbuffers::GetString(elem->datalayerURI())).c_str());
    }
  }

  log(L"\nRead the output mapping of the test profile via datalayer.");
  log(L"\nReadSync: %ls", test::StringHelper::utf8ToWide(urlMappingOutput).c_str());
  dlResult = m_datalayerTestClient->readSync(urlMappingOutput.c_str(), &actData);
  if ( comm::datalayer::STATUS_FAILED(dlResult) )
  {
    error(L"Reading of uri %ls failed.", test::StringHelper::utf8ToWide(urlMappingOutput).c_str());
  }

  log(L"\nCheck the received data.");
  flatbuffers::Verifier verifierOut(actData.getData(), actData.getSize());
  if ( !comm::axisprofile::fbtypes::VerifyvalueMappingBuffer(verifierOut) )
  {
    error(L"Verify of uri %ls failed", test::StringHelper::utf8ToWide(urlMappingOutput).c_str());
  }
  auto variantDataOut = comm::axisprofile::fbtypes::GetvalueMapping(actData.getData());

  auto vecDataOut = variantDataOut->mapping();
  for ( auto elem = vecDataOut->begin(); elem != vecDataOut->end(); ++elem )
  {
    std::string valueIdStr = flatbuffers::GetString(elem->valueID()).c_str();
    std::string valueUriStr = flatbuffers::GetString(elem->datalayerURI()).c_str();
    if ( valueIdStr.compare("position") || valueUriStr.compare("axis1/position") )
    {
      error(L"\nID: %ls, Uri: %ls", test::StringHelper::utf8ToWide(flatbuffers::GetString(elem->valueID())).c_str(), test::StringHelper::utf8ToWide(flatbuffers::GetString(elem->datalayerURI())).c_str());
    }
  }

  log(L"\nCheck the out-of-range limit and per-category counters via datalayer.");
  auto* scaling = testProfile1->getScaling();
  RETURN_TEST_RESULT_SET_PTR(scaling, L"Profile has no scaling interface.");

  const auto readUInt32 = [&](std::string address, std::uint32_t expected)
  {
    test::ProviderHelperCommon::insertProfileName(address, profileName);
    expData.setValue(expected);
    return test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, address.c_str(), &actData, true, &expData);
  };
  const std::uint32_t limit = 10U;
  std::string limitAddress = provider::NodeTree.APR_PROFILES_CFG_SCALINGINFO_OUTOFRANGE_THRESHOLD;
  test::ProviderHelperCommon::insertProfileName(limitAddress, profileName);
  writeData.setValue(limit);
  result = test::ProviderHelperFunctest::writeNodeSync(this, m_datalayerTestClient, limitAddress.c_str(), &writeData, true);
  if (FAILED(result))
  {
    return result;
  }
  hresult |= readUInt32(provider::NodeTree.APR_PROFILES_CFG_SCALINGINFO_OUTOFRANGE_THRESHOLD, limit);

  const double outOfRange = std::numeric_limits<double>::infinity();
  scaling->convertPositionScaledToRaw(outOfRange);
  for (unsigned sample = 0; sample < 2; ++sample)
  {
    scaling->convertVelocityScaledToRaw(outOfRange);
  }
  for (unsigned sample = 0; sample < 3; ++sample)
  {
    scaling->convertTorqueScaledToRaw(outOfRange);
  }
  hresult |= readUInt32(provider::NodeTree.APR_PROFILES_DATA_OUT_RANGE_TIMES_POS, 1U);
  hresult |= readUInt32(provider::NodeTree.APR_PROFILES_DATA_OUT_RANGE_TIMES_VEL, 2U);
  hresult |= readUInt32(provider::NodeTree.APR_PROFILES_DATA_OUT_RANGE_TIMES_TOR, 3U);

  return hresult;
}
