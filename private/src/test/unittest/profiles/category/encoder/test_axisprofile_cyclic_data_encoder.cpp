#include "test_axisprofile_cyclic_data_encoder.h"
#include "profiles/value_mapping/mapping_handler.h"
#include "helper/result_helper.h"
#include "mockup/mock_axisprofile_mapping_handler.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileCyclicDataEncoder);
REGISTER_TEST_TO_GROUP(TestAxisProfileCyclicDataEncoder, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileCyclicDataEncoder, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileCyclicDataEncoder, 4_EncoderTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestAxisProfileCyclicDataEncoder::doInit()
{
  log(L"\nInit test data member...");
  m_profileInfo = std::make_shared<ProfileBasicInfo>();
  m_telegramMDT = std::make_shared<MasterDataTelegram>();
  m_telegramAT = std::make_shared<AcknowledgeTelegram>();
  m_category = std::make_unique<EncoderCategoryAdapterCoE>(*m_profileInfo.get(), m_telegramAT.get(), m_telegramMDT.get());
  m_sharingHandler = std::make_unique<AxisProfileSharingHandler>(&m_profileInfo->m_profileTrace);
  m_category->registerSharingITFs(m_sharingHandler.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_category.get());
  m_mockAdapter = std::make_unique<test::MockFieldbusAdapterCoE>();
  return S_OK;
}

HRESULT TestAxisProfileCyclicDataEncoder::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test all the TestAxisProfileCyclicDataEncoder interface");
  log(L"\n----------------------------------------------------------------\n");

  log(L"\nTest setStateMachineStatusIF");
  auto result = m_category->getCyclicDataInternal()->setStateMachineStatusIF(nullptr);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n\t Test of checkActualValueAvailabilityNRT without mapping");
  AxisProfileValueTypeID id {};
  DataAccessErrorInfo errorInfo {};
  auto bRes = m_category->getCyclicDataInternal()->checkActualValueAvailabilityNRT(id, errorInfo);
  RETURN_TEST_RESULT(bRes, L"\nUnexpected return value", false);

  log(L"\n\t Test of checkActualValueAvailabilityNRT wrong fieldbus");
  test::MockAxisProfileMappingHandler mockMapping {};
  std::string uri {"mock"};
  mockMapping.configureInputValue(InputMappingValueID::POSITION, uri.c_str());
  mockMapping.getProfileAssignedInfo().m_input.m_position = true;
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mockMapping);
  m_profileInfo->m_fieldbusAdapter = m_mockAdapter.get();
  bRes = m_category->getCyclicDataInternal()->checkActualValueAvailabilityNRT(id, errorInfo);
  RETURN_TEST_RESULT(bRes, L"\nUnexpected return value", false);

  log(L"\n\t Test of checkActualValueAvailabilityNRT wrong device state");
  m_mockAdapter->setFieldbusMasterAddress("fieldbuses/ethercat/master/instances/ethercatmaster/device");
  m_mockAdapter->m_returnValue = AxisProfileResult::STS_OK;
  bRes = m_category->getCyclicDataInternal()->checkActualValueAvailabilityNRT(id, errorInfo);
  RETURN_TEST_RESULT(bRes, L"\nUnexpected return value", true);

  log(L"\n\t Test of checkSingleUserPDOActiveFromDevice...");
  UserPDOID pdoID {};
  bRes = m_category->getCyclicDataInternal()->checkSingleUserPDOActiveFromDevice(pdoID);
  RETURN_TEST_RESULT(bRes, L"\nUnexpected return value", false);

  log(L"\n\t Test of checkSingleUserPDOActiveToDevice...");
  bRes = m_category->getCyclicDataInternal()->checkSingleUserPDOActiveToDevice(pdoID);
  RETURN_TEST_RESULT(bRes, L"\nUnexpected return value", false);

  log(L"\nTest setStateMachineStatusIF");
  StateMachineState statemachine {};
  result = m_category->getCyclicDataInternal()->setStateMachineStatusIF(&statemachine);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of getActualValuesDevice...");
  m_category->getCyclicDataInternal()->booting();
  ActualValuesDevice actualvalues {};
  result = m_category->getCyclicDataInternal()->getActualValuesDevice(&actualvalues);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of getCInterface...");
  m_category->getCyclicDataInternal()->getCInterface();

  log(L"\n\t Test of getDigitalInputValueBit...");
  DigitalInputValueBitType bitType {};
  bool input {};
  result = m_category->getCyclicDataInternal()->getDigitalInputValueBit(bitType, input);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getEffectiveTimeOfActOutput...");
  m_category->getCyclicDataInternal()->getEffectiveTimeOfActOutput();

  log(L"\n\t Test of getLatchTimeOfActInput...");
  m_category->getCyclicDataInternal()->getLatchTimeOfActInput();

  log(L"\n\t Test of getSafetyOperationMode...");
  SafetyOperationMode mode {};
  result = m_category->getCyclicDataInternal()->getSafetyOperationMode(mode);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getSingleUserPDODataFromDevice...");
  UserPDODataInfo data {};
  result = m_category->getCyclicDataInternal()->getSingleUserPDODataFromDevice(pdoID, data);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getStatemachine...");
  m_category->getCyclicDataInternal()->getStatemachine();

  log(L"\n\t Test of resetFPUFlag...");
  m_category->getCyclicDataInternal()->resetFPUFlag();

  log(L"\n\t Test of setActSyncTime...");
  double absTime {}, syncTime {};
  m_category->getCyclicDataInternal()->setActSyncTime(absTime, syncTime);

  log(L"\n\t Test of setCommandValuesDevice...");
  CommandValuesDevice cmdValues {};
  result = m_category->getCyclicDataInternal()->setCommandValuesDevice(cmdValues);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setSingleUserPDODataToDevice...");
  result = m_category->getCyclicDataInternal()->setSingleUserPDODataToDevice(pdoID, data);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);
  // 
  log(L"\n\t Test of setNewRefPosition...");
  std::int32_t iNewRefPos {};
  m_category->getCyclicDataInternal()->setNewRefPosition(iNewRefPos);

  log(L"\n\t Test of activeNewRefOperation...");
  bool active = true;
  m_category->getCyclicDataInternal()->activeNewRefOperation(active);

  uint8_t offset {};
  result = m_category->getCyclicDataInternal()->activeSignalFunc(offset);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  result = m_category->getCyclicDataInternal()->resetSignalFuncBit(offset);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);
  bool state {};
  result = m_category->getCyclicDataInternal()->getSignalFuncState(offset, state);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  return res;
}

HRESULT TestAxisProfileCyclicDataEncoder::doEnd()
{
  return S_OK;
}
