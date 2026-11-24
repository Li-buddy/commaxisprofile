#include "test_provider_helper.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/datalayer/datalayer_system.h"
#include "comm/datalayer/diagnosis_generated.h"
#include "provider/provider_helper.h"
#include "comm/datalayer/metadata_generated.h"

// mockups

using namespace comm::axisprofile;

REGISTER_TEST(TestProviderHelper);
REGISTER_TEST_TO_GROUP(TestProviderHelper, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderHelper, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderHelper, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderHelper::TestProviderHelper()
{
}


TestProviderHelper::~TestProviderHelper(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderHelper::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile provider helper class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= testMetadata();
  hResult |= testMiscellaneous();

  return hResult;
}

HRESULT TestProviderHelper::testMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test to create metadata.");

  provider::ProviderHelper providerHelper;

  std::string typeString = "type/empty";
  comm::datalayer::Variant returnVariant;
  comm::datalayer::DlResult returnStatus;
  comm::datalayer::IClient::ResponseCallback callback = [&](comm::datalayer::DlResult status, const comm::datalayer::Variant *source)
  {
    if ( source )
    {
      returnVariant = *source;
    }
    returnStatus = status;
  };

  log(L"\nCall createMetaData() with invalid input arguments.");
  provider::MetaDataInfo metaDataInfo {nullptr, nullptr, "", comm::datalayer::NodeClass::NodeClass_Program, provider::node_actions::CREATE};
  providerHelper.createMetaData(typeString, metaDataInfo, callback);
  if ( comm::datalayer::DlResult::DL_INVALID_HANDLE != returnStatus )
  {
    error(L"createMetaData did not fail as expected.");
    hResult = E_FAIL;
  }

  log(L"\nCall createMetaData() with valid input arguments.");
  metaDataInfo.m_descriptionUrl = "";
  metaDataInfo.m_description = "description";
  const auto readMetadata = [&]() -> const comm::datalayer::Metadata*
  {
    if (returnStatus != comm::datalayer::DlResult::DL_OK ||
        returnVariant.verifyFlatbuffers(comm::datalayer::VerifyMetadataBuffer) != comm::datalayer::DlResult::DL_OK)
    {
      return nullptr;
    }
    return comm::datalayer::GetMetadata(returnVariant.getData());
  };
  providerHelper.createMetaData(typeString, metaDataInfo, callback, "mm");
  const auto* metadata = readMetadata();
  if (!metadata || flatbuffers::GetString(metadata->unit()) != "mm")
  {
    error(L"createMetaData did not return valid metadata with the requested unit.");
    return E_FAIL;
  }

  metaDataInfo.nodeActions = provider::node_actions::READ | provider::node_actions::WRITE |
                             provider::node_actions::CREATE | provider::node_actions::REMOVE;
  providerHelper.createMetaData3("node", typeString, "types/create", metaDataInfo, callback, "mm/s", false,
                                 "types/create-out", "types/read-in", "types/read-out");
  metadata = readMetadata();
  const auto* operations = metadata ? metadata->operations() : nullptr;
  const auto* references = metadata ? metadata->references() : nullptr;
  if (!operations || !operations->read() || !operations->write() || !operations->create() || !operations->delete_() ||
      operations->browse() || flatbuffers::GetString(metadata->unit()) != "mm/s" || !references || references->size() != 6)
  {
    error(L"Incorrect operations, unit, or reference count for createMetaData3.");
    return E_FAIL;
  }
  const auto hasTarget = [&](const char* key, const std::string& expected)
  {
    const auto* reference = references->LookupByKey(key);
    return reference && flatbuffers::GetString(reference->targetAddress()) == expected;
  };
  if (!hasTarget("readType", typeString) || !hasTarget("writeType", typeString) || !hasTarget("createType", "types/create") ||
      !hasTarget("createOutType", "types/create-out") || !hasTarget("readInType", "types/read-in") || !hasTarget("readOutType", "types/read-out"))
  {
    error(L"createMetaData3 returned an incorrect type reference.");
    return E_FAIL;
  }

  metaDataInfo.nodeActions = provider::node_actions::NONE;
  providerHelper.createMetaData3("node", typeString, metaDataInfo, callback);
  metadata = readMetadata();
  operations = metadata ? metadata->operations() : nullptr;
  if (!operations || !operations->browse() || operations->read() || operations->write() || operations->create() || operations->delete_() ||
      metadata->unit() || (metadata->references() && metadata->references()->size() != 0))
  {
    error(L"Metadata without actions must be browsable and omit the unit and type references.");
    return E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestProviderHelper::testMiscellaneous()
{
  HRESULT hResult = S_OK;
  provider::ProviderHelper providerHelper;
  log(L"\n<b>Test miscellaneous methods of the provider helper.");

  log(L"\nCall getLastSubNodeOfAddress() with invalid input string.");
  std::string outputString{};
  bool result = providerHelper.getLastSubNodeOfAddress("invalidAddress", outputString);
  if (result)
  {
    error(L"Method should have failed. Received string: %ls", UTF8TOWIDE(outputString));
    hResult = E_FAIL;
  }

  log(L"\nCall getLastSubNodeOfAddress() with valid input string.");
  result = providerHelper.getLastSubNodeOfAddress("extract/the/last/node", outputString);
  if (!result || (outputString != "node"))
  {
    error(L"Method should have succeeded. Expected output: 'node' Actual output: '%ls'", UTF8TOWIDE(outputString));
    hResult = E_FAIL;
  }

  log(L"\nCall extractWildcardContentFromAddress() with invalid arguments.");
  result = providerHelper.extractWildcardContentFromAddress("extract/the/wildcard/content", "invalidString", outputString);
  if (result)
  {
    error(L"Method should have failed. Received string: %ls", UTF8TOWIDE(outputString));
    hResult = E_FAIL;
  }

  log(L"\nCall extractWildcardContentFromAddress() with valid input string (extract node in between).");
  result = providerHelper.extractWildcardContentFromAddress("extract/the/wildcard/content", "the", outputString);
  if (!result || (outputString != "wildcard"))
  {
    error(L"Method should have succeeded. Expected output: 'node' Actual output: '%ls'", UTF8TOWIDE(outputString));
    hResult = E_FAIL;
  }

  log(L"\nCall extractWildcardContentFromAddress() with valid input string (extract node at the end).");
  result = providerHelper.extractWildcardContentFromAddress("extract/the/wildcard/content", "wildcard", outputString);
  if (!result || (outputString != "content"))
  {
    error(L"Method should have succeeded. Expected output: 'node' Actual output: '%ls'", UTF8TOWIDE(outputString));
    hResult = E_FAIL;
  }

  for (const auto* address : {"axisprofile/profiles/axis1", "axisprofile/profiles/axis1/cfg", "axisprofile/system/axis1", "axisprofile/system/axis1/state"})
  {
    outputString.clear();
    if (!providerHelper.getProfileNameFromAddress(address, outputString) || outputString != "axis1")
    {
      error(L"Could not extract the profile name from %ls.", UTF8TOWIDE(address));
      return E_FAIL;
    }
  }
  if (!providerHelper.replaceFirstWideCharInAddress("profiles/*/cfg/*", "axis1", outputString) || outputString != "profiles/axis1/cfg/*" ||
      providerHelper.replaceFirstWideCharInAddress("profiles", "axis1", outputString) || outputString != "profiles" ||
      providerHelper.getTypeString("profiles/*/cfg") != "types/profiles/cfg" || !providerHelper.getTypeString("profiles").empty())
  {
    error(L"Unexpected wildcard replacement or type address.");
    return E_FAIL;
  }

  std::string reason;
  for (const auto& name : {std::string{}, std::string(MAX_PROFILE_NAME_LENGTH, 'a'), std::string("1axis"), std::string("axis-name")})
  {
    reason.clear();
    if (providerHelper.checkProfileName(name, reason) || reason.empty())
    {
      error(L"Expected an explanation for the invalid profile name '%ls'.", UTF8TOWIDE(name));
      return E_FAIL;
    }
  }
  if (!providerHelper.checkProfileName(std::string(MAX_PROFILE_NAME_LENGTH - 1, 'a'), reason) || !providerHelper.checkProfileName("axis_1", reason))
  {
    error(L"A valid profile name was rejected.");
    return E_FAIL;
  }

  using comm::datalayer::DlResult;
  using comm::datalayer::Variant;
  using comm::datalayer::VariantType;
  DlResult returnStatus = DlResult::DL_OK;
  Variant returnData;
  unsigned callbacks = 0;
  const auto callback = [&](DlResult status, const Variant* data)
  {
    ++callbacks;
    returnStatus = status;
    if (data)
    {
      returnData = *data;
    }
  };
  Variant input;
  input.setValue(true);
  const Variant* invalidInputs[] = {nullptr, &input};
  for (const auto* invalid : invalidInputs)
  {
    const auto previous = callbacks;
    if (providerHelper.checkVariantType(invalid, VariantType::UINT32, callback) || callbacks != previous + 1 ||
        returnStatus != DlResult::DL_TYPE_MISMATCH || returnData.getType() != VariantType::UINT32 ||
        static_cast<std::uint32_t>(returnData) != static_cast<std::uint32_t>(AxisProfileResult::ERR_DATALAYER_WRONG_TYPE))
    {
      error(L"An invalid variant must return the wrong-type error through the callback.");
      return E_FAIL;
    }
  }
  input.setValue(std::uint32_t{7});
  const auto unexpectedCallback = [this, &hResult](DlResult, const Variant*)
  {
    error(L"A matching variant must not invoke the callback.");
    hResult = E_FAIL;
  };
  if (!providerHelper.checkVariantType(&input, VariantType::UINT32, unexpectedCallback))
  {
    error(L"A matching variant must succeed.");
    return E_FAIL;
  }

  const DlResult expectedStatus = DlResult::DL_SUBMODULE_FAILURE;
  const auto beforeErrorResponse = callbacks;
  if (!providerHelper.sendDlErrorResponse(callback, expectedStatus, 11U, 22U, "axis %u", 7U) ||
      callbacks != beforeErrorResponse + 1 || returnStatus != expectedStatus ||
      returnData.verifyFlatbuffers(comm::datalayer::VerifyDiagnosisBuffer) != DlResult::DL_OK)
  {
    error(L"Expected a valid diagnosis buffer and the requested callback status.");
    return E_FAIL;
  }
  const auto* diagnosis = comm::datalayer::GetDiagnosis(returnData.getData());
  const std::string description = std::string("axis 7 ,DLResult: ") + expectedStatus.toString();
  if (diagnosis->mainDiagnosisCode() != 11U || diagnosis->detailedDiagnosisCode() != 22U ||
      flatbuffers::GetString(diagnosis->dynamicDescription()) != description)
  {
    error(L"The diagnosis lost its codes or formatted description.");
    return E_FAIL;
  }

  log(L"\n");
  return hResult;
}
