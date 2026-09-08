#include "test_provider_node_base.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_provider_mgr.h"

// Test Class
#include "provider/nodes/node_base.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderNodeBase);
REGISTER_TEST_TO_GROUP(TestProviderNodeBase, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeBase, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeBase, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderNodeBase::TestProviderNodeBase()
{
}


TestProviderNodeBase::~TestProviderNodeBase(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderNodeBase::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the base class for all axisprofile datalayer nodes. ");
  log(L"\n----------------------------------------------------------------------\n");
  
  hResult |= testClass();
  hResult |= testCreateBasicTypeString();

  return hResult;
}


HRESULT TestProviderNodeBase::testClass()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test all interface methods of the node base class.</b>");

  log(L"\nCreate node base class.");
  test::MockProviderMgr mockProviderMgr;
  provider::NodeBase testClass(&mockProviderMgr);
  comm::datalayer::Variant myVariant;

  log(L"\nCall onCreate().");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onCreate(this, &testClass, "-NoAddressGiven-", nullptr, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onRemove().");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRemove(this, &testClass, "-NoAddressGiven-", false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onBrowse().");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onBrowse(this, &testClass, "-NoAddressGiven-", false, nullptr, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onRead().");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &testClass, "-NoAddressGiven-", nullptr, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onWrite().");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &testClass, "<-NoAddressGiven-", nullptr, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onMetadata().");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &testClass, "-NoAddressGiven-", false, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall getBfbsName().");
  const char* retVal = testClass.getBfbsName();
  if (retVal)
  {
    error(L"\nMethod has not returned a nullptr!");
    hResult = E_FAIL;
  }

  log(L"\nCall getDescription().");
 retVal = testClass.getDescription();
  if (retVal)
  {
    error(L"\nMethod has not returned a nullptr!");
    hResult = E_FAIL;
  }

  log(L"\nCall getDescriptionUrl().");
  retVal = testClass.getDescriptionUrl();
  if (retVal)
  {
    error(L"\nMethod has not returned a nullptr!");
    hResult = E_FAIL;
  }

  log(L"\nCall createTypeString() with valid datalayer address.");
  std::string typeString;
  bool retValBool = testClass.createTypeString("axisprofile/profiles/testProfile/test-uri", "testProfile",typeString, test::ProviderHelperUnittest::m_callback);
  if (!retValBool)
  {
    error(L"\nMethod has not returned true (success)!");
    hResult = E_FAIL;
  }

  log(L"\nCall createTypeString() with valid datalayer address (with '/' at the end).");
  retValBool = testClass.createTypeString("axisprofile/profiles/testProfile/test-uri/", "testProfile", typeString, test::ProviderHelperUnittest::m_callback);
  if (!retValBool)
  {
    error(L"\nMethod has not returned true (success)!");
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeBase::testCreateBasicTypeString()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test all interface methods of the node base class.</b>");

  log(L"\nCreate node base class.");
  test::MockProviderMgr mockProviderMgr;
  provider::NodeBase testBasicType(&mockProviderMgr);
  comm::datalayer::Variant myVariant;

  log(L"\n\n<BLUE><b>Test creation of basic type strings for different data types</b></BLUE>");
  std::string validAddr = "AXISPROFILE";
  std::string typeStr;
  comm::datalayer::IProviderNode::ResponseCallback responseCallback;

  log(L"\nTest VariantType::BOOL8");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::BOOL8, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_BOOL8)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::INT8");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::INT8, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_INT8)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::UINT8");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::UINT8, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_UINT8)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::INT16");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::INT16, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_INT16)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::INT32");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::INT32, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_INT32)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::UINT32");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::UINT32, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_UINT32)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::INT64");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::INT64, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_INT64)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::UINT64");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::UINT64, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_UINT64)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::FLOAT32");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::FLOAT32, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_FLOAT32)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::FLOAT64");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::FLOAT64, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_FLOAT64)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::STRING");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::STRING, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_STRING)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_BOOL8");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_BOOL8, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_BOOL8)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_INT8");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_INT8, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_INT8)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_UINT8");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_UINT8, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_UINT8)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_INT16");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_INT16, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_INT16)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_UINT16");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_UINT16, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_UINT16)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_INT32");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_INT32, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_INT32)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_UINT32");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_UINT32, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_UINT32)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_INT64");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_INT64, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_INT64)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_UINT64");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_UINT64, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_UINT64)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_FLOAT32");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_FLOAT32, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_FLOAT32)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_FLOAT64");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_FLOAT64, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_FLOAT64)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::ARRAY_OF_STRING");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::ARRAY_OF_STRING, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_ARRAY_OF_STRING)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  log(L"\nTest VariantType::RAW");
  PROVIDER_CHECK(testBasicType.createBasicTypeString(validAddr.c_str(), comm::datalayer::VariantType::RAW, typeStr, responseCallback));
  if (typeStr != comm::datalayer::TYPE_DL_RAW)
  {
    error(L"\nCreated typestring doesn't match!");
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n<DARK_GREEN>CreateBasicTypeString expected success!</DARK_GREEN>");
  }

  return hResult;
}
