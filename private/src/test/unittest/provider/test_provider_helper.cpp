#include "test_provider_helper.h"
#include "comm/datalayer/datalayer_system.h"
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
  providerHelper.createMetaData(typeString, metaDataInfo, callback);
  if ( comm::datalayer::DlResult::DL_OK != returnStatus )
  {
    error(L"createMetaData did fail with returnStatus: 0x%X", returnStatus);
    hResult = E_FAIL;
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

  log(L"\n");
  return hResult;
}
