#include "axisprofile_internal_defs.h"
#include "fieldbuses/fieldbus_adapter_factory.h"
#include "fieldbuses/fieldbus_helper.h"
#include "helper/flatbuffers_helper_common.h"
#include "helper/result_helper.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_profile_trace.h"
#include "test_fieldbus_adapter.h"
#include "comm/ethercat/master/fbs/num_configured_slaves_generated.h"
#include "comm/ethercat/master/fbs/slave_address_mapping_generated.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestFieldbusAdapter);
REGISTER_TEST_TO_GROUP(TestFieldbusAdapter, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestFieldbusAdapter, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestFieldbusAdapter, Fieldbuses);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

TestFieldbusAdapter::TestFieldbusAdapter()
{
}

TestFieldbusAdapter::~TestFieldbusAdapter(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestFieldbusAdapter::doInit()
{
  return S_OK;
}

HRESULT TestFieldbusAdapter::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the fieldbus adapter base class.");
  log(L"\n----------------------------------------------------------------");
  
  hResult |= testReadWriteParamAsync();
  hResult |= testReadWriteParamSync();
  hResult |= testSlaveInfoFbs();
  hResult |= testSlaveAddressMappingFbs();
  hResult |= testMiscellaneous();
  hResult |= testMasterStatus();
  hResult |= testSlaveStatus();
  hResult |= testReadSlaveDcCfgInfo();
  hResult |= testParamListConversion();
  return hResult;
}

HRESULT TestFieldbusAdapter::doEnd()
{
  return S_OK;
}

HRESULT TestFieldbusAdapter::testReadWriteParamAsync()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of asynchronous parameter read/write methods</b>");

  test::MockDatalayerClient mockDatalayerClient{};
  test::MockAxisProfileTrace mockTrace{};
  FieldbusAdapterUnittest testClass{&mockTrace};
  testClass.setDatalayer(&mockDatalayerClient);

  log(L"\nCall readSingleParamAsync() and simulate error from datalayer.");
  ParameterResponseCallbackData responseCallbackData{};
  ParamReqDataECat dummyRequestDataRead{};
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  AxisProfileResult actResult = testClass.readSingleParamAsync(&responseCallbackData, dummyRequestDataRead);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER);

  log(L"\nCall writeSingleParamAsync() and simulate error from datalayer.");
  ParamReqDataECat dummyRequestDataWrite{};
  actResult = testClass.writeSingleParamAsync(&responseCallbackData, dummyRequestDataWrite);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER);

  log(L"\nCall readSingleParamAsync() when everything is OK.");
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  actResult = testClass.readSingleParamAsync(&responseCallbackData, dummyRequestDataRead);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::STS_OK);

  log(L"\nCall writeSingleParamAsync() when everything is OK.");
  actResult = testClass.writeSingleParamAsync(&responseCallbackData, dummyRequestDataWrite);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::STS_OK);

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapter::testReadWriteParamSync()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of synchronous parameter read/write methods</b>");

  std::size_t readLen = 0;
  test::MockDatalayerClient mockDatalayerClient{};
  test::MockAxisProfileTrace mockTrace{};
  FieldbusAdapterUnittest testClass{&mockTrace};
  testClass.setDatalayer(&mockDatalayerClient);

  log(L"\nCall readSingleParamSync() and simulate error from datalayer.");
  ParamReqDataECat dummyRequestDataRead{};
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  AxisProfileResult actResult = testClass.readSingleParamSync(dummyRequestDataRead, nullptr, 0, readLen);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER);

  log(L"\nCall writeSingleParamSync() and simulate error from datalayer.");
  ParamReqDataECat dummyRequestDataWrite{};
  actResult = testClass.writeSingleParamSync(dummyRequestDataWrite);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER);

  log(L"\nCall readSingleParamSync() when everything is OK.");
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  actResult = testClass.readSingleParamSync(dummyRequestDataRead, nullptr, 0, readLen);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::STS_OK);

  log(L"\nCall writeSingleParamSync() when everything is OK.");
  actResult = testClass.writeSingleParamSync(dummyRequestDataWrite);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::STS_OK);

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapter::testSlaveInfoFbs()
{
  HRESULT hResult = S_OK;
  AxisProfileResult actResult{ AxisProfileResult::STS_OK };
  log(L"\nCall extractSlaveCfgInfo() with wrong data type");
  comm::datalayer::Variant variantCfgedSlaveInfo{};
  std::string slaveName{};

  //test for wrong type
  log(L"\nCall extractSlaveCfgInfo() with wrong data type");

  DeviceHardwareInfo devInfo;
  actResult = DeviceFbsDataHelper::extractSlaveCfgInfo(variantCfgedSlaveInfo, slaveName, devInfo);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  //test for wrong verify
  log(L"\nCall extractSlaveCfgInfo() with wrong data verifier");
  variantCfgedSlaveInfo.setType(comm::datalayer::VariantType::FLATBUFFERS);
  actResult = DeviceFbsDataHelper::extractSlaveCfgInfo(variantCfgedSlaveInfo, slaveName, devInfo);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  //test without correct response
  log(L"\nCall extractSlaveCfgInfo() without correct response");
  DeviceFbsDataHelper::generateAddressRequest(1001, variantCfgedSlaveInfo);
  actResult = DeviceFbsDataHelper::extractSlaveCfgInfo(variantCfgedSlaveInfo, slaveName, devInfo);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  log(L"\nCall extractSlaveCfgInfo() with correct data");
  slaveName = { "testDevice" };
  std::uint32_t protocol = 26;// indicate support EoE/FoE/SoE
  generateCfgSlaveInfo(variantCfgedSlaveInfo, slaveName, protocol);

  actResult = DeviceFbsDataHelper::extractSlaveCfgInfo(variantCfgedSlaveInfo, slaveName, devInfo);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::STS_OK);
  return hResult;
}

void TestFieldbusAdapter::generateCfgSlaveInfo(comm::datalayer::Variant& variantCfgedSlaveInfo, std::string& slaveName, std::uint32_t protocol)
{
  flatbuffers::FlatBufferBuilder totalBuilder;
  auto slavenameOffset = totalBuilder.CreateString(slaveName);

  comm::ethercat::master::fbs::EthercatIdentityInfo identityInfo(36, 2369540, 3, 0);

  comm::ethercat::master::fbs::SlaveConfigInfoResponseBuilder slaveResponseBuilder(totalBuilder);
  slaveResponseBuilder.add_ethercatAddr(1001);
  slaveResponseBuilder.add_slaveName(slavenameOffset);
  slaveResponseBuilder.add_mbxProtocols(protocol);
  slaveResponseBuilder.add_slaveIdentity(&identityInfo);
  auto slaveResponseOffset = slaveResponseBuilder.Finish();

  comm::ethercat::master::fbs::SlaveConfigInfoBuilder infoBuilder(totalBuilder);
  infoBuilder.add_response(slaveResponseOffset);
  auto slaveInfoOffset = infoBuilder.Finish();
  totalBuilder.Finish(slaveInfoOffset);

  variantCfgedSlaveInfo.copyFlatbuffers(totalBuilder);
}

void TestFieldbusAdapter::generateCfgedSlaveNum(comm::datalayer::Variant& variantCfgedSlaveCount, std::uint32_t nCount, bool isValuable)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  comm::ethercat::master::fbs::NumConfiguredSlavesResponseBuilder responseBuilder(builder);
  responseBuilder.add_numSlaves(nCount);
  auto numOffsetResponse = responseBuilder.Finish();
  comm::ethercat::master::fbs::NumConfiguredSlavesBuilder numCfgBuilder(builder);
  if ( isValuable )
  {    
    numCfgBuilder.add_response(numOffsetResponse);   
  }
  auto numOffset = numCfgBuilder.Finish();
  builder.Finish(numOffset);
  variantCfgedSlaveCount.copyFlatbuffers(builder);
}

HRESULT TestFieldbusAdapter::testMiscellaneous()
{
  HRESULT hResult = S_OK;
  FieldbusAdapterUnittest testClass{nullptr};
  log(L"\n<b>Test of miscellaneous class methods</b>");

  log(L"\nCall setFieldbusMasterAddress().");
  testClass.setFieldbusMasterAddress("test");

  log(L"\nCall getFieldbusMasterAddress().");
  std::string address = testClass.getFieldbusMasterAddress();

  if (address.compare("test") != 0)
  {
    error(L"Fieldbus master address not as expected.");
  }

  ProfileBasicInfo info;
  info.m_axisProfileType = AxisProfileType::UNDEFINED;
  FieldbusAdapterFactory::createFieldbusAdapter(info, &info.m_fieldbusAdapter);

  log(L"\n<b>Test of rawDataToHexStream...</b>");
  std::uint16_t dataU16 {0x1234};
  std::int16_t dataI16 {0x1234};
  std::int16_t dataI16_ {-0x1234};
  std::uint32_t dataU32 {0x12345678};
  std::int32_t dataI32 {0x12345678};
  std::int32_t dataI32_ {-0x7654321};
  double dataFloat {123.456789};
  double dataFloat_ {-123.456789};

  log(L"\nU16 data %d, streamData: %s .", dataU16, UTF8TOWIDE(FieldbusHelper::rawDataToHexDump(reinterpret_cast<std::uint8_t*>(&dataU16), sizeof(dataU16))));
  log(L"\nI16 data %d, streamData: %s .", dataI16, UTF8TOWIDE(FieldbusHelper::rawDataToHexDump(reinterpret_cast<std::uint8_t*>(&dataI16), sizeof(dataI16))));
  log(L"\nI16 data %d, streamData: %s .", dataI16_, UTF8TOWIDE(FieldbusHelper::rawDataToHexDump(reinterpret_cast<std::uint8_t*>(&dataI16_), sizeof(dataI16_))));
  log(L"\nU32 data %d, streamData: %s .", dataU32, UTF8TOWIDE(FieldbusHelper::rawDataToHexDump(reinterpret_cast<std::uint8_t*>(&dataU32), sizeof(dataU32))));
  log(L"\nI32 data %d, streamData: %s .", dataI32, UTF8TOWIDE(FieldbusHelper::rawDataToHexDump(reinterpret_cast<std::uint8_t*>(&dataI32), sizeof(dataI32))));
  log(L"\nI32 data %d, streamData: %s .", dataI32_, UTF8TOWIDE(FieldbusHelper::rawDataToHexDump(reinterpret_cast<std::uint8_t*>(&dataI32_), sizeof(dataI32_))));
  log(L"\nFloat data %g:, streamData: %s .", dataFloat, UTF8TOWIDE(FieldbusHelper::rawDataToHexDump(reinterpret_cast<std::uint8_t*>(&dataFloat), sizeof(dataFloat))));
  log(L"\nFloat data %g:, streamData: %s .", dataFloat_, UTF8TOWIDE(FieldbusHelper::rawDataToHexDump(reinterpret_cast<std::uint8_t*>(&dataFloat_), sizeof(dataFloat_))));

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapter::testMasterStatus()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test of read master status</b>");
  test::FlatbuffersHelperCommon fbHelper;
  test::MockDatalayerClient mockDatalayerClient {};
  test::MockAxisProfileTrace mockTrace {};
  FieldbusAdapterUnittest testClass {&mockTrace};
  comm::axisprofile::AxisProfileResult retValue {comm::axisprofile::AxisProfileResult::STS_OK};
  comm::datalayer::Variant masterResp {};

  log(L"\n  Test error case of datalayer client empty...</b>");
  MasterStatus masterStatus;
  retValue = testClass.readFieldbusMasterStatusSync(masterStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n  Test error case datalayer read error...</b>");
  testClass.setDatalayer(&mockDatalayerClient);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  retValue = testClass.readFieldbusMasterStatusSync(masterStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_DATALAYER);

  log(L"\n  Test error case when response value not flatbuffer...</b>");
  masterResp.setType(comm::datalayer::VariantType::ARRAY_OF_BOOL8);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = masterResp;
  retValue = testClass.readFieldbusMasterStatusSync(masterStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_WRONGDATATYPE);

  log(L"\n  Test error case when response value not MasterStatus...</b>");
  masterResp.setType(comm::datalayer::VariantType::FLATBUFFERS);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = masterResp;
  retValue = testClass.readFieldbusMasterStatusSync(masterStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  log(L"\n  Test error case when response value not contain response...</b>");
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  comm::ethercat::master::fbs::MasterStatusBuilder statusBuilder(builder);
  auto slaveStateBuilderOffset = statusBuilder.Finish();
  builder.Finish(slaveStateBuilderOffset);
  masterResp.copyFlatbuffers(builder);

  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = masterResp;
  retValue = testClass.readFieldbusMasterStatusSync(masterStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_FIELDBUS);

  log(L"\n  Test normal case when everything is okay...</b>");
  fbHelper.m_masterStatus.m_status = 123456;
  fbHelper.generateMasterStatusResponseFlatbuffer(&masterResp);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = masterResp;

  retValue = testClass.readFieldbusMasterStatusSync(masterStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_S(masterStatus.m_status, L"unexpected master status!", fbHelper.m_masterStatus.m_status);

  return hResult;
}

HRESULT TestFieldbusAdapter::testSlaveStatus()
{
  HRESULT hResult = S_OK;

  log(L"\n<b>Test of read slave status</b>");
  test::FlatbuffersHelperCommon fbHelper;
  test::MockDatalayerClient mockDatalayerClient {};
  test::MockAxisProfileTrace mockTrace {};
  FieldbusAdapterUnittest testClass {&mockTrace};
  comm::axisprofile::AxisProfileResult retValue {comm::axisprofile::AxisProfileResult::STS_OK};
  comm::axisprofile::ReadSlaveStateData slaveData;
  comm::datalayer::Variant slaveVar {};
  
  log(L"\n  Test error case of datalayer client empty...");
  SlaveStatus slaveStatus;
  retValue = testClass.readSlaveStatusSync(slaveData, slaveStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n  Test error case datalayer read error...");
  testClass.setDatalayer(&mockDatalayerClient);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  retValue = testClass.readSlaveStatusSync(slaveData, slaveStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_DATALAYER);

  log(L"\n  Test error case when response value not flatbuffer...");
  slaveVar.setType(comm::datalayer::VariantType::ARRAY_OF_BOOL8);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = slaveVar;
  retValue = testClass.readSlaveStatusSync(slaveData, slaveStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_WRONGDATATYPE);

  log(L"\n  Test error case when response value not SlaveStatus...");
  slaveVar.setType(comm::datalayer::VariantType::FLATBUFFERS);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = slaveVar;
  retValue = testClass.readSlaveStatusSync(slaveData, slaveStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  log(L"\n  Test error case when response value not contain response...");
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  comm::ethercat::master::fbs::SlaveStatusBuilder statusBuilder(builder);
  auto slaveStateBuilderOffset = statusBuilder.Finish();
  builder.Finish(slaveStateBuilderOffset);
  slaveVar.copyFlatbuffers(builder);

  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = slaveVar;
  retValue = testClass.readSlaveStatusSync(slaveData, slaveStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_FIELDBUS);

  log(L"\n  Test normal case when everything is okay...");
  fbHelper.m_slaveStatus.m_status = 123456;
  fbHelper.generateSlaveStatusResponseFlatbuffer(&slaveVar);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = slaveVar;

  retValue = testClass.readSlaveStatusSync(slaveData, slaveStatus.m_status);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT_S(slaveStatus.m_status, L"unexpected slave status!", fbHelper.m_slaveStatus.m_status);

  return hResult;
}

HRESULT TestFieldbusAdapter::testReadSlaveDcCfgInfo()
{
  log(L"\n<b>Test of read slave DC config info</b>");
  test::FlatbuffersHelperCommon fbHelper;
  test::MockDatalayerClient mockDatalayerClient {};
  test::MockAxisProfileTrace mockTrace {};
  FieldbusAdapterUnittest testClass {&mockTrace};
  comm::axisprofile::AxisProfileResult retValue {comm::axisprofile::AxisProfileResult::STS_OK};
  comm::axisprofile::ReadSlaveStateData slaveData;
  comm::datalayer::Variant respVar {};

  log(L"\n  Test error case of datalayer client empty...</b>");
  DcConfigInfo dcCfgInfo {};
  retValue = testClass.readSlaveDcConfigInfoSync(0, dcCfgInfo);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n  Test error case datalayer read error...</b>");
  testClass.setDatalayer(&mockDatalayerClient);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_FAILED;
  retValue = testClass.readSlaveDcConfigInfoSync(0, dcCfgInfo);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_DATALAYER);

  log(L"\n  Test error case when response value not flatbuffer...</b>");
  respVar.setType(comm::datalayer::VariantType::ARRAY_OF_BOOL8);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = respVar;
  retValue = testClass.readSlaveDcConfigInfoSync(0, dcCfgInfo);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  log(L"\n  Test error case when response value not DcSlaveConfigInfo...</b>");
  respVar.setType(comm::datalayer::VariantType::FLATBUFFERS);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = respVar;
  retValue = testClass.readSlaveDcConfigInfoSync(0, dcCfgInfo);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  log(L"\n  Test error case when response value not contain response...</b>");
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  comm::ethercat::master::fbs::DcSlaveConfigInfoBuilder dcSlaveCfgBuilder(builder);
  auto dcSlaveCfgBuilderOffset = dcSlaveCfgBuilder.Finish();
  builder.Finish(dcSlaveCfgBuilderOffset);
  respVar.copyFlatbuffers(builder);

  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = respVar;
  retValue = testClass.readSlaveDcConfigInfoSync(0, dcCfgInfo);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  log(L"\n  Test normal case when everything is okay...</b>");
  fbHelper.m_dcSupported = true;
  fbHelper.m_shiftTimeNs = 200000;
  fbHelper.generateSlaveDcCfgInfoResponseFlatbuffer(&respVar);
  mockDatalayerClient.m_returnValue = comm::datalayer::DlResult::DL_OK;
  mockDatalayerClient.m_callbackVariant = respVar;

  retValue = testClass.readSlaveDcConfigInfoSync(0, dcCfgInfo);
  RETURN_TEST_RESULT(retValue, L"unexpected return value!", comm::axisprofile::AxisProfileResult::STS_OK);
  LOG_TEST_RESULT_ANY(dcCfgInfo.m_dcSupported == fbHelper.m_dcSupported, L"unexpected return value!");
  LOG_TEST_RESULT_ANY(dcCfgInfo.m_shiftTimeNs == fbHelper.m_shiftTimeNs, L"unexpected return value!");

  return S_OK;
}

HRESULT TestFieldbusAdapter::testParamListConversion()
{
  HRESULT hResult = S_OK;
  std::vector<std::uint8_t> dummyBuff {};
  dummyBuff.resize(LIST_BUFF_DEFAULT_SIZE);

  log(L"\n<b>Test of rawToParamList</b>");
  std::vector<std::uint16_t> outList {};
  log(L"\n  1. Error case of empty pointer...");
  auto res = FieldbusHelper::rawToParamList(nullptr, 0, outList);
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::ERR_EXEC_NULL);
  res = FieldbusHelper::rawToParamListSoE(nullptr, 0, outList);
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n  2. Error case of incorrect size...");
  res = FieldbusHelper::rawToParamList(dummyBuff.data(), 1, outList);
  RETURN_TEST_RESULT_ANY(outList.empty(), L"\nUnexpected return value!");
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::ERR_WRONGDATASIZE);
  res = FieldbusHelper::rawToParamListSoE(dummyBuff.data(), 3, outList);
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::ERR_WRONGDATASIZE);

  log(L"\n  3. Error case of incorrect length of SoE...");
  dummyBuff[0] = 255;
  dummyBuff[1] = 0;
  res = FieldbusHelper::rawToParamListSoE(dummyBuff.data(), dummyBuff.size(), outList);
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::ERR_WRONGDATASIZE);

  log(L"\n  4. Success case without convert function...");
  std::vector<std::uint16_t> expData {IDN_S(0, 21), IDN_S(0, 15), IDN_S(0, 21), IDN_P(0, 54), IDN_P(0, 71), IDN_P(0, 02)};
  log(L"\n   Create raw data by rawToParamListSoE...");
  res = FieldbusHelper::paramListToRawSoE(expData, dummyBuff);
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::STS_OK);
  res = FieldbusHelper::rawToParamListSoE(dummyBuff.data(), dummyBuff.size(), outList);
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::STS_OK);
  log(L"\n   check parse size...");
  RETURN_TEST_RESULT_ANY(outList.size() == expData.size(), L"\nUnexpected read size!");
  log(L"\n   check parse value...");
  LOG_TEST_RESULT_FLT_ARRY(outList.data(), L"\nUnexpected return value!", expData.data(), outList.size(), 1E-7);

  log(L"\n  5. Success case with convert function...");
  std::vector<ObjectDirEntry> expObjs {{0x2308, 4}, {456, 5}, {321, 4}, {654, 3}, {789, 2}, {987, 1}};
  std::vector<ObjectDirEntry> outObjs {};
  auto funcListToRaw = [] (const ObjectDirEntry& objEntry, std::uint8_t* rawBuf)
    {
      std::memcpy(rawBuf + 1, &objEntry.subIndex, sizeof(objEntry.subIndex));
      std::memcpy(rawBuf + 2, &objEntry.index, sizeof(objEntry.index));
    };
  auto funcRawToList = [] (const std::uint8_t* rawBuf, ObjectDirEntry& objEntry)
    {
      std::memcpy(&objEntry.subIndex, rawBuf + 1, sizeof(objEntry.subIndex));
      std::memcpy(&objEntry.index, rawBuf + 2, sizeof(objEntry.index));
    };

  log(L"\n   Create raw data by paramListToRaw...");
  res = FieldbusHelper::paramListToRaw<ObjectDirEntry>(expObjs, dummyBuff, funcListToRaw);
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::STS_OK);

  res = FieldbusHelper::rawToParamList<ObjectDirEntry>(dummyBuff.data(), dummyBuff.size(), outObjs, funcRawToList);
  LOG_TEST_RESULT(res, L"\nUnexpected return value!", AxisProfileResult::STS_OK);
  log(L"\n   check parse size...");
  RETURN_TEST_RESULT_ANY(outObjs.size() == expData.size(), L"\nUnexpected read size!");
  log(L"\n   check parse value...");
  for (size_t i = 0; i < outObjs.size(); i++)
  {
    if (outObjs[i].index != expObjs[i].index)
    {
      error(L"\n%ls:%d Value is not equal, outObjs[%u].index = %u, expObjs[%u].index = %u.", LITERALWIDECHAR(__FILE__), __LINE__, i, outObjs[i].index, i, expObjs[i].index);
    }
    if (outObjs[i].subIndex != expObjs[i].subIndex)
    {
      error(L"\n%ls:%d Value is not equal, outObjs[%u].subIndex = %u, expObjs[%u].subIndex = %u.", LITERALWIDECHAR(__FILE__), __LINE__, i, outObjs[i].subIndex, i, expObjs[i].subIndex);
    }
  }

  log(L"\n");
  return hResult;
}

HRESULT TestFieldbusAdapter::testSlaveAddressMappingFbs()
{
  HRESULT hResult = S_OK;
  AxisProfileResult actResult {AxisProfileResult::STS_OK};
  comm::datalayer::Variant variantMappingInfo {};
  std::vector<std::uint32_t> addressVec;

  //test for wrong type
  log(L"\nCall extractSlaveAddressByMapping() with wrong data type");
  actResult = DeviceFbsDataHelper::extractSlaveAddressByMapping(variantMappingInfo, addressVec);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  //test for wrong verify
  log(L"\nCall extractSlaveAddressByMapping() with wrong data verifier");
  variantMappingInfo.setType(comm::datalayer::VariantType::FLATBUFFERS);
  actResult = DeviceFbsDataHelper::extractSlaveAddressByMapping(variantMappingInfo, addressVec);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  //test without correct response
  log(L"\nCall extractSlaveAddressByMapping() without correct response");
  DeviceFbsDataHelper::generateSlaveAddressMappingRequestFlatbuffer(variantMappingInfo);
  actResult = DeviceFbsDataHelper::extractSlaveAddressByMapping(variantMappingInfo, addressVec);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::ERR_DATALAYER_WRONG_TYPE);

  log(L"\nCall extractSlaveAddressByMapping() with correct data");
  generateSlaveAddressMappingResponse(3, variantMappingInfo);
  actResult = DeviceFbsDataHelper::extractSlaveAddressByMapping(variantMappingInfo, addressVec);
  LOG_TEST_RESULT(actResult, L"\nUnexpected return value!", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT_ANY(addressVec.size() == 3, L"unexpected return value!");
  return hResult;
}

void TestFieldbusAdapter::generateSlaveAddressMappingResponse(std::uint16_t addressCnt, comm::datalayer::Variant& mappingResponse)
{
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<comm::ethercat::master::fbs::AddressMappingEntry>> entries;
  for (int i = 0; i < addressCnt; ++i)
  {
    auto entryAddr = comm::ethercat::master::fbs::CreateAddressedRequest(builder, comm::ethercat::master::fbs::Addresstype_fixedphysical, 1000 + i);
    auto entryName = builder.CreateString("SLAVE_" + std::to_string(i));

    auto entry = comm::ethercat::master::fbs::CreateAddressMappingEntry(builder, entryAddr, entryName);
    entries.push_back(entry);
  }

  auto entryList = builder.CreateVector(entries);
  auto response = comm::ethercat::master::fbs::CreateSlaveAddressMappingResponse(builder, entryList);

  comm::ethercat::master::fbs::SlaveAddressMappingBuilder mappingBuilder(builder);
  mappingBuilder.add_response(response);
  auto mappingRoot = mappingBuilder.Finish();

  builder.Finish(mappingRoot);
  mappingResponse.copyFlatbuffers(builder);
}
