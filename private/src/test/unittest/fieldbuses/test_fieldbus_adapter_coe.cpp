#include "test_fieldbus_adapter_coe.h"
#include "helper/flatbuffers_helper_coe.h"

// Mockups
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_profile_trace.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestFieldbusAdapterCoE);
REGISTER_TEST_TO_GROUP(TestFieldbusAdapterCoE, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestFieldbusAdapterCoE, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestFieldbusAdapterCoE, Fieldbuses);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

TestFieldbusAdapterCoE::TestFieldbusAdapterCoE()
{
}

TestFieldbusAdapterCoE::~TestFieldbusAdapterCoE(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestFieldbusAdapterCoE::doInit()
{
  return S_OK;
}

HRESULT TestFieldbusAdapterCoE::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the CoE fieldbus adapter class.");
  log(L"\n----------------------------------------------------------------");
  hResult |= testReadWriteParamAsync();
  hResult |= testReadWriteParamSync();
  hResult |= testGetParamResponseData();
  hResult |= testReadSlaveStateSync();
  hResult |= testReadMasterStateSync();
  hResult |= testMiscellaneous();
  return hResult;
}

HRESULT TestFieldbusAdapterCoE::doEnd()
{
  return S_OK;
}

HRESULT TestFieldbusAdapterCoE::testReadWriteParamAsync()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of asynchronous parameter read/write methods</b>");

  test::MockAxisProfileTrace mockTrace{};
  FieldbusAdapterCoE testClass{&mockTrace};

  log(L"\nCall readSingleParamAsync() when datalayer client is missing.");
  ParameterResponseCallbackData responseCallbackData{};
  ParamReqDataCoE dummyRequestDataRead{};
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult actResult = testClass.readSingleParamAsync(&responseCallbackData, dummyRequestDataRead);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall writeSingleParamAsync() when datalayer client is missing.");
  ParamReqDataCoE dummyRequestDataWrite{};
  actResult = testClass.writeSingleParamAsync(&responseCallbackData, dummyRequestDataWrite);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  test::MockDatalayerClient mockDatalayerClient{};
  testClass.setDatalayer(&mockDatalayerClient);

  log(L"\nCall readSingleParamAsync() when everything is OK.");
  expResult = AxisProfileResult::STS_OK;
  actResult = testClass.readSingleParamAsync(&responseCallbackData, dummyRequestDataRead);
  if (actResult != expResult)
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall writeSingleParamAsync() when everything is OK.");
  actResult = testClass.writeSingleParamAsync(&responseCallbackData, dummyRequestDataWrite);
  if (actResult != expResult)
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", actResult);
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapterCoE::testReadWriteParamSync()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of synchronous parameter read/write methods</b>");

  std::size_t readLen {0};
  test::MockAxisProfileTrace mockTrace{};
  FieldbusAdapterCoE testClass{&mockTrace};

  log(L"\nCall readSingleParamSync() when datalayer client is missing.");
  ParamReqDataCoE dummyRequestDataRead{};
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult actResult = testClass.readSingleParamSync(dummyRequestDataRead, nullptr, 0, readLen);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall writeSingleParamSync() when datalayer client is missing.");
  ParamReqDataCoE dummyRequestDataWrite{};
  actResult = testClass.writeSingleParamSync(dummyRequestDataWrite);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  test::MockDatalayerClient mockDatalayerClient{};
  testClass.setDatalayer(&mockDatalayerClient);

  log(L"\nCall readSingleParamSync() when everything is OK.");
  expResult = AxisProfileResult::STS_OK;
  dummyRequestDataRead.objEntry = {76, 0};
  test::FlatbuffersHelperCoE fbsHelper{};
  comm::datalayer::Variant responseFB{};
  fbsHelper.generateSDOResponseFlatbuffer(&responseFB, 76, 0);
  mockDatalayerClient.m_callbackVariant = responseFB;
  std::uint8_t outData[2]{};
  actResult = testClass.readSingleParamSync(dummyRequestDataRead, outData, sizeof(outData), readLen);
  if (actResult != expResult)
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall writeSingleParamSync() when everything is OK.");
  actResult = testClass.writeSingleParamSync(dummyRequestDataWrite);
  if (actResult != expResult)
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", actResult);
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapterCoE::testGetParamResponseData()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of method getParamResponseData()</b>");

  std::size_t readLen {0};
  test::MockAxisProfileTrace mockTrace{};
  FieldbusAdapterCoE testClass{&mockTrace};

  log(L"\nCall getSDOResponseData() with wrong variant type.");
  comm::datalayer::Variant testVariant{};
  AxisProfileResult expResult = AxisProfileResult::ERR_WRONGDATATYPE;
  AxisProfileResult actResult = testClass.getParamResponseData(&testVariant, nullptr, 0, readLen);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall getSDOResponseData() when flatbuffer verification fails.");
  testVariant.setType(comm::datalayer::VariantType::FLATBUFFERS);
  expResult = AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  actResult = testClass.getParamResponseData(&testVariant, nullptr, 0, readLen);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall getSDOesponseData() with wrong field 'VT_REQUEST'.");
  std::uint8_t dummyData[2] = {0, 0};
  m_fbsHelper.generateSDORequestFlatbuffer(&testVariant, 0, 0, dummyData, sizeof(dummyData));
  expResult = AxisProfileResult::ERR_FIELDBUS;
  actResult = testClass.getParamResponseData(&testVariant, nullptr, 0, readLen);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall getParamResponseData() with wrong data size.");
  m_fbsHelper.generateSDOResponseFlatbuffer(&testVariant, 0 , 0);
  actResult = testClass.getParamResponseData(&testVariant, dummyData, 1, readLen);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;  
  }

  log(L"\nCall getParamResponseData() when everything is OK.");
  expResult = AxisProfileResult::STS_OK;
  actResult = testClass.getParamResponseData(&testVariant, dummyData, sizeof(dummyData), readLen);
  if (actResult != expResult)
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", actResult);
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapterCoE::testReadSlaveStateSync()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of method readSlaveStateSync()</b>");

  test::MockAxisProfileTrace mockTrace{};
  FieldbusAdapterCoE testClass{&mockTrace};

  log(L"\nCall readSlaveStateSync() when datalayer client is missing.");
  std::string state{};
  ReadSlaveStateData dummyRequestDataRead{};
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult actResult = testClass.readSlaveStateSync(dummyRequestDataRead, &state);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  test::MockDatalayerClient mockDatalayerClient{};
  testClass.setDatalayer(&mockDatalayerClient);

  log(L"\nCall readSlaveStateSync() when response FB has wrong type.");
  expResult = AxisProfileResult::ERR_WRONGDATATYPE;
  comm::datalayer::Variant responseFB{};
  mockDatalayerClient.m_callbackVariant = responseFB;
  actResult = testClass.readSlaveStateSync(dummyRequestDataRead, &state);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall readSlaveStateSync() when response FB verification fails.");
  expResult = AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  responseFB.setType(comm::datalayer::VariantType::FLATBUFFERS);
  mockDatalayerClient.m_callbackVariant = responseFB;
  actResult = testClass.readSlaveStateSync(dummyRequestDataRead, &state);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall readSlaveStateSync() when everything is OK.");
  expResult = AxisProfileResult::STS_OK;
  test::FlatbuffersHelperCoE fbsHelper{};
  fbsHelper.generateSlaveStateResponseFlatbuffer(&responseFB);
  mockDatalayerClient.m_callbackVariant = responseFB;
  actResult = testClass.readSlaveStateSync(dummyRequestDataRead, &state);
  if (actResult != expResult)
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", actResult);
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapterCoE::testReadMasterStateSync()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of of method readFieldbusMasterStateSync()</b>");

  test::MockAxisProfileTrace mockTrace{};
  FieldbusAdapterCoE testClass{&mockTrace};

  log(L"\nCall readFieldbusMasterStateSync() when datalayer client is missing.");
  std::string state{};
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult actResult = testClass.readFieldbusMasterStateSync(&state);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  test::MockDatalayerClient mockDatalayerClient{};
  testClass.setDatalayer(&mockDatalayerClient);

  log(L"\nCall readFieldbusMasterStateSync() when response FB has wrong type.");
  expResult = AxisProfileResult::ERR_WRONGDATATYPE;
  comm::datalayer::Variant responseFB{};
  mockDatalayerClient.m_callbackVariant = responseFB;
  actResult = testClass.readFieldbusMasterStateSync(&state);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall readFieldbusMasterStateSync() when response FB verification fails.");
  expResult = AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  responseFB.setType(comm::datalayer::VariantType::FLATBUFFERS);
  mockDatalayerClient.m_callbackVariant = responseFB;
  actResult = testClass.readFieldbusMasterStateSync(&state);
  if (actResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, actResult);
    hResult = E_FAIL;
  }

  log(L"\nCall readFieldbusMasterStateSync() when everything is OK.");
  expResult = AxisProfileResult::STS_OK;
  test::FlatbuffersHelperCoE fbsHelper{};
  fbsHelper.generateMasterStateResponseFlatbuffer(&responseFB);
  mockDatalayerClient.m_callbackVariant = responseFB;
  actResult = testClass.readFieldbusMasterStateSync(&state);
  if (actResult != expResult)
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", actResult);
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapterCoE::testMiscellaneous()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of miscellaneous class methods</b>");

  test::MockAxisProfileTrace mockTrace{};
  FieldbusAdapterCoE testClass{&mockTrace};

  log(L"\nCall setFieldbusMasterAddress() when everything is OK.");
  testClass.setFieldbusMasterAddress(DL_ADDRESS_FIELDBUS_ECAT_MASTER_INSTANCES);

  log(L"\n");
  return hResult;
}
