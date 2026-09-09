#include "helper/result_helper.h"
#include "profiles/canopen/drive/canopen_scaling_strategy_drive.h"
#include "profiles/canopen/canopen_defs.h"
#include "test_canopen_scaling_strategy_drive.h"
#include "profiles/category/drive/drive_category_adapter_coe.h"

using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

REGISTER_TEST(TestCanopenScalingStrategyDrive);
REGISTER_TEST_TO_GROUP(TestCanopenScalingStrategyDrive, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenScalingStrategyDrive, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenScalingStrategyDrive, Profiles_Canopen);

HRESULT TestCanopenScalingStrategyDrive::doInit()
{
  HRESULT tResult = S_OK;

  //prepare for test object  
  log(L"\n<b>create test object.</b>");
  m_mockClient = std::make_shared< test::MockDatalayerClient>();
  m_mockFieldbusAdapter = std::make_shared<test::MockFieldbusAdapterCoE>();

  m_mockFieldbusAdapter->setDatalayer(static_cast<comm::datalayer::IClient*>(m_mockClient.get()));
  m_mockFieldbusAdapter->m_returnValue = AxisProfileResult::STS_OK;
  m_sdoRequestNodeMockCoE = std::make_shared<test::MockEthercatMasterSDORequestNodeCoE>();
  m_mockClient->m_testNode = m_sdoRequestNodeMockCoE.get();
  m_profileBasicInfo.m_fieldbusAdapter = m_mockFieldbusAdapter.get();
  m_profileBasicInfo.m_deviceAddress = m_deviceAddress;

  m_testObject = std::make_unique<ScalingStrategyCoEDrive>(ScalingStrategyCoEDrive {m_profileBasicInfo});
  m_bootParamSnapshot = std::make_unique<CanopenBootParamSnapshot>(m_profileBasicInfo);
  m_pMappingHandlerMockup = std::make_unique<test::MockAxisProfileMappingHandler>();
  m_deviceParamMockup = std::make_unique<test::MockScalingDriveDeviceParam>(m_profileBasicInfo);
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  m_mockCategory = std::make_unique<DriveCategoryAdapterCoE>(m_profileBasicInfo, &atData, &mdtData);

  m_sharingHandler = std::make_unique<AxisProfileSharingHandler>(AxisProfileSharingHandler {m_mockProfileTrace.get()});
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, m_testObject.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, m_pMappingHandlerMockup.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, m_deviceParamMockup.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_mockCategory.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_BOOTPARAMSNAPSHOT, m_bootParamSnapshot.get());
  auto& assign = m_pMappingHandlerMockup->getProfileAssignedInfo();
  assign.m_input.m_position = true;
  assign.m_input.m_velocity = true;
  assign.m_input.m_torqueForce = true;
  m_pMappingHandlerMockup->setReturnValue(assign);

  //create test data for position
  m_unitPosLinear.m_bytes.byte3 = CANOPEN_POSITION_UNIT_LINEAR;
  m_unitPosRotary.m_bytes.byte3 = CANOPEN_POSITION_UNIT_ROTARY;

  //create test data for velocity
  m_unitVelLinear.m_bytes.byte3 = CANOPEN_VELOCITY_UNIT_LINEAR;

  m_unitVelRotary.m_bytes.byte3 = CANOPEN_VELOCITY_UNIT_ROTARY;
  m_unitVelRotary.m_bytes.byte2 = CANOPEN_VELOCITY_TIME_UNIT_MINUTE;

  //create test data for acceleration
  m_unitAccLinear.m_bytes.byte3 = CANOPEN_ACCELERATION_UNIT_LINEAR;
  m_unitAccRotary.m_bytes.byte3 = CANOPEN_ACCELERATION_UNIT_ROTARY;

  //create test data for torque
  m_unitTorqueLinear.m_torqueScalingType = CANOPEN_TORQUE_TYPE_METHOD_LINEAR;
  m_unitTorqueRotary.m_torqueScalingType = CANOPEN_TORQUE_TYPE_METHOD_ROT;
  m_unitTorquePercent.m_torqueScalingType = CANOPEN_TORQUE_TYPE_METHOD_PCTG;

  m_vecTestDataValid.reserve(8);
  m_vecTestDataValid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcPosScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitPosRotary, AxisProfileResult::STS_OK, L"test For Rotary Position Type"}));
  m_vecTestDataValid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcPosScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitPosLinear,AxisProfileResult::STS_OK,L"test For Linear Position Type" }));
  m_vecTestDataValid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitVelLinear,AxisProfileResult::STS_OK,L"test For Linear Velocity Type" }));
  m_vecTestDataValid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitVelRotary,AxisProfileResult::STS_OK,L"test For Rotary Velocity Type" }));
  m_vecTestDataValid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcAccScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitAccLinear,AxisProfileResult::STS_OK,L"test For Linear Acceleration Type" }));
  m_vecTestDataValid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcAccScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitAccRotary,AxisProfileResult::STS_OK,L"test For Rotary Acceleration Type" }));
  m_vecTestDataValid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcTorqueScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitTorqueLinear, AxisProfileResult::STS_OK, L"test For Linear Torque Type"}));
  m_vecTestDataValid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcTorqueScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitTorquePercent, AxisProfileResult::STS_OK, L"test For Percent Torque Type"}));

  m_vecTestDataSpecialWrong.reserve(1);
  m_vecTestDataSpecialWrong.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcTorqueScalingLSBWeight, this, std::placeholders::_1),
                                                   &m_unitTorqueRotary, AxisProfileResult::ERR_INVALID_ARGUMENT, L"test For Rotary Torque Type"}));

  m_unitPosUndefine.m_bytes.byte3 = CANOPEN_POSITION_UNIT_ROTARY + 1;//create a wrong data
  m_unitVelUndefine.m_bytes.byte3 = CANOPEN_VELOCITY_UNIT_ROTARY + 1;//create a wrong data
  m_unitAccUndefine.m_bytes.byte3 = CANOPEN_ACCELERATION_UNIT_ROTARY + 1;//create a wrong data
  m_unitAccUndefine.m_bytes.byte4 = 1;
  m_unitTorqueUndefine.m_torqueScalingType = CANOPEN_TORQUE_TYPE_METHOD_ROT + 1;
  m_unitTorqueErrorMath.m_torqueScalingType = CANOPEN_TORQUE_TYPE_METHOD_LINEAR;
  m_unitTorqueErrorMath.m_torqueScalingFactor = 0;

  m_vecTestDataWrong.reserve(6);
  m_vecTestDataWrong.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcPosScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitPosUndefine, AxisProfileResult::ERR_INIT_TELEGRAM_OFFSETS, L"test For Linear Position Type"}));
  m_vecTestDataWrong.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitVelUndefine, AxisProfileResult::ERR_INIT_TELEGRAM_OFFSETS, L"test For Rotary Velocity Type"}));
  m_vecTestDataWrong.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcAccScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitAccUndefine, AxisProfileResult::ERR_INIT_TELEGRAM_OFFSETS, L"test For Rotary Acceleration Type"}));
  m_vecTestDataWrong.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcTorqueScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitTorqueUndefine, AxisProfileResult::ERR_INVALID_ARGUMENT, L"test For Rotary Torque Type"}));
  m_vecTestDataWrong.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcTorqueScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitTorqueRotary, AxisProfileResult::ERR_INVALID_ARGUMENT, L"test For Rotary Torque Type"}));
  m_vecTestDataWrong.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcTorqueScalingLSBWeight, this, std::placeholders::_1),
                                            &m_unitTorqueErrorMath, AxisProfileResult::ERR_MATH, L"test For Linear Torque Type"}));
  m_vecTestDataInvalid.reserve(3);
  m_vecTestDataInvalid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcPosScalingLSBWeight, this, std::placeholders::_1),
                                              &m_unitPosLinear, AxisProfileResult::ERR_EXEC_NULL, L"test For Linear Position Type"}));
  m_vecTestDataInvalid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcVelScalingLSBWeight, this, std::placeholders::_1),
                                              &m_unitVelRotary, AxisProfileResult::ERR_EXEC_NULL, L"test For Rotary Velocity Type"}));
  m_vecTestDataInvalid.emplace_back(TestData({std::bind(&TestCanopenScalingStrategyDrive::testCalcTorqueScalingLSBWeight, this, std::placeholders::_1),
                                              &m_unitTorqueRotary, AxisProfileResult::ERR_EXEC_NULL, L"test For Rotary Torque Type"}));
  return tResult;
}

HRESULT TestCanopenScalingStrategyDrive::doTest()
{
  log(L"\n<b>create test object with valid data and mapping.</b>");
  for (const auto& item : m_vecTestDataValid)
  {
    item.m_testFunctional(&item);
  }
  for (const auto& item : m_vecTestDataSpecialWrong)
  {
    item.m_testFunctional(&item);
  }
  testLoadLSBWeightsMethods();
  testReadScalingParamsSync();
  testCalcScalingLSBWeights();
  testReadScalingParamsSyncFailed();
  log(L"\n<b>create test object with wrong data.</b>");
  for (const auto& item : m_vecTestDataWrong)
  {
    item.m_testFunctional(&item);
  }

  log(L"\n<b>create test object with valid data but without mapping.</b>");
  auto& assign = m_pMappingHandlerMockup->getProfileAssignedInfo();
  assign.m_input.m_position = false;
  assign.m_input.m_velocity = false;
  assign.m_input.m_torqueForce = false;
  m_pMappingHandlerMockup->setReturnValue(assign);
  for (const auto& item : m_vecTestDataValid)
  {
    item.m_testFunctional(&item);
  }

  log(L"\n<b>create test object with invalid data.</b>");
  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER);
  for (const auto& item : m_vecTestDataInvalid)
  {
    item.m_testFunctional(&item);
  }

  log(L"\n<b>create test object without device param interface.</b>");
  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM);
  AxisProfileResult missingDeviceParamResult = m_testObject->calcPosScalingLSBWeight();
  RETURN_TEST_RESULT(missingDeviceParamResult, L"\nUnexpected return value without device param interface", AxisProfileResult::ERR_EXEC_NULL);
  return S_OK;
}

HRESULT TestCanopenScalingStrategyDrive::testReadScalingParamsSync()
{
  HRESULT tResult = S_OK;
  m_testObject->readPosScalingParamsSync();
  m_testObject->readVelScalingParamsSync();
  m_testObject->readAccScalingParamsSync();
  m_testObject->readTorqueScalingParamsSync();
  return tResult;
}

HRESULT TestCanopenScalingStrategyDrive::testReadScalingParamsSyncFailed()
{
  log(L"\n\t Test error case of reading torque scaling parameter...");
  m_sdoRequestNodeMockCoE->m_fbsHelper.resetParam();
  m_sdoRequestNodeMockCoE->m_fbsHelper.writeParam(0x6091, 1, std::uint32_t(0));
  m_sdoRequestNodeMockCoE->m_fbsHelper.writeParam(0x6091, 2, std::uint32_t(0));
  AxisProfileResult result = m_testObject->readTorqueScalingParamsSync();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\n\t Test error case of reading torque scaling parameter via calculating torque scaling LSBWeight...");
  result = m_testObject->calcTorqueScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_INVALID_ARGUMENT);
  return S_OK;
}

HRESULT TestCanopenScalingStrategyDrive::testCalcScalingLSBWeights()
{
  HRESULT tResult = S_OK;
  m_testObject->m_scalingLSBWeightsMethods.clear();
  m_testObject->calcScalingLSBWeights();
  return tResult;
}

HRESULT TestCanopenScalingStrategyDrive::testCalcPosScalingLSBWeight(const TestData* testData)
{
  HRESULT tResult {S_OK};
  m_sdoRequestNodeMockCoE->m_fbsHelper.resetParam();

  std::uint32_t newValue = static_cast<CanScalingData*>(testData->m_testScalingData)->m_bytes.byte3 << 16;
  tResult = m_sdoRequestNodeMockCoE->m_fbsHelper.overWriteParam(0x60A8, 0, std::uint32_t(0x00FF0000), newValue);
  RETURN_TEST_FAILED(tResult, L"\nOverwrite CoE Object 0x60A8::00 failed");

  log(L"\n\t Test of calcPosScalingLSBWeight...");
  AxisProfileResult result = m_testObject->calcPosScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", testData->m_expResult);

  return tResult;
}

HRESULT TestCanopenScalingStrategyDrive::testCalcVelScalingLSBWeight(const TestData* testData)
{
  HRESULT tResult {S_OK};
  m_sdoRequestNodeMockCoE->m_fbsHelper.resetParam();
  std::uint32_t newValue = static_cast<CanScalingData*>(testData->m_testScalingData)->m_bytes.byte3 << 16;
  tResult = m_sdoRequestNodeMockCoE->m_fbsHelper.overWriteParam(0x60A9, 0, std::uint32_t(0x00FF0000), newValue);
  RETURN_TEST_FAILED(tResult, L"\nOverwrite CoE Object 0x60A8::00 failed");

  log(L"\n\t Test of calcVelScalingLSBWeight...");
  AxisProfileResult result = m_testObject->calcVelScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", testData->m_expResult);

  return tResult;
}

HRESULT TestCanopenScalingStrategyDrive::testCalcAccScalingLSBWeight(const TestData* testData)
{
  HRESULT tResult {S_OK};
  m_sdoRequestNodeMockCoE->m_fbsHelper.resetParam();
  std::uint32_t newValue = static_cast<CanScalingData*>(testData->m_testScalingData)->m_bytes.byte4 << 24 |
                      static_cast<CanScalingData*>(testData->m_testScalingData)->m_bytes.byte3 << 16;
  tResult = m_sdoRequestNodeMockCoE->m_fbsHelper.overWriteParam(0x60AA, 0, std::uint32_t(0xFFFF0000), newValue);
  RETURN_TEST_FAILED(tResult, L"\nOverwrite CoE Object 0x60AA::00 failed");

  log(L"\n\t Test of calcAccScalingLSBWeight...");
  AxisProfileResult result = m_testObject->calcAccScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", testData->m_expResult);

  return tResult;
}

HRESULT TestCanopenScalingStrategyDrive::testLoadLSBWeightsMethods()
{
  HRESULT tResult = S_OK;  
  m_testObject->loadBootingTasks();
  return tResult;
}

HRESULT TestCanopenScalingStrategyDrive::testCalcTorqueScalingLSBWeight(const TestData* testData)
{
  HRESULT tResult {S_OK};
  m_sdoRequestNodeMockCoE->m_fbsHelper.resetParam();
  tResult = m_sdoRequestNodeMockCoE->m_fbsHelper.writeParam(0x2906, 1, static_cast<TorqueScalingData*>(testData->m_testScalingData)->m_torqueScalingType);
  RETURN_TEST_FAILED(tResult, L"\nOverwrite CoE Object 0x2906::01 failed");

  tResult = m_sdoRequestNodeMockCoE->m_fbsHelper.writeParam(0x2906, 2, static_cast<TorqueScalingData*>(testData->m_testScalingData)->m_torqueScalingFactor);
  RETURN_TEST_FAILED(tResult, L"\nOverwrite CoE Object 0x2906::02 failed");

  log(L"\n\t Test of calcTorqueScalingLSBWeight...");
  AxisProfileResult result = m_testObject->calcTorqueScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", testData->m_expResult);
  
  std::uint16_t torqueType1 = static_cast<TorqueScalingData*>(testData->m_testScalingData)->m_torqueScalingType | 1U << CANOPEN_TORQUE_TYPE_UNIT_OFFSET;
  tResult = m_sdoRequestNodeMockCoE->m_fbsHelper.writeParam(0x2906, 1, torqueType1);
  RETURN_TEST_FAILED(tResult, L"\nOverwrite CoE Object 0x2906::01 failed");

  log(L"\n\t Test of calcTorqueScalingLSBWeight...");
  result = m_testObject->calcTorqueScalingLSBWeight();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", testData->m_expResult);
  return tResult;
}
