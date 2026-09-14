#include "helper/result_helper.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_property.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_scaling_drive_device_param.h"
#include "profiles/category/drive/drive_category_adapter_soe.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/scaling/unit_defs.h"
#include "profiles/sercos/sercos_boot_param_snapshot.h"
#include "profiles/sercos/sercos_defs.h"
#include "test_sercos_scaling_strategy_drive.h"

using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

REGISTER_TEST(TestScalingStrategySoEDrive);
REGISTER_TEST_TO_GROUP(TestScalingStrategySoEDrive, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestScalingStrategySoEDrive, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestScalingStrategySoEDrive, Profiles_Sercos);

// Test

HRESULT TestScalingStrategySoEDrive::doInit()
{
  return S_OK;
}

HRESULT TestScalingStrategySoEDrive::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Sercos profile scaling class");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testCalcPosScalingLSBWeight();
  hResult |= testCalcVelScalingLSBWeight();
  hResult |= testCalcAccScalingLSBWeight();
  hResult |= testCalcTorqueScalingLSBWeight();
  hResult |= testGetterAndSetter();
  hResult |= testModulo();

  return hResult;
}

HRESULT TestScalingStrategySoEDrive::doEnd()
{
  return S_OK;
}

HRESULT TestScalingStrategySoEDrive::testCalcPosScalingLSBWeight()
{
  const std::vector<TestPosData> testDataPosScaling
  {
    // ScaType,                                       ScaFactor, ScaExpo, RotRes,  ExpVal,          ExpErrorCode,                             Name
    {sercos::SERCOS_SCA_POS_TYPE_METHOD_LINEAR |
    (1U << sercos::SERCOS_SCA_POS_TYPE_UNIT_OFFSET),  1,         -6,      3600000, 1E-6 * 0.0254,   AxisProfileResult::STS_OK,                L"Case: scaling method 'Linear' in inches"},
    {sercos::SERCOS_SCA_POS_TYPE_METHOD_LINEAR,       1,         -7,      3600000, 1E-7,            AxisProfileResult::STS_OK,                L"Case: scaling method 'Linear' in meters"},
    {sercos::SERCOS_SCA_POS_TYPE_METHOD_ROT,          1,         -7,      3600000, 0.0001,          AxisProfileResult::STS_OK,                L"Case: default values, scaling method 'Rotational'"},
    {sercos::SERCOS_SCA_POS_TYPE_METHOD_NONE,         1,         -7,      3600000, 1.0,             AxisProfileResult::ERR_INVALID_ARGUMENT,  L"Case: scaling method 'None'"},
    {sercos::SERCOS_SCA_POS_TYPE_METHOD_LINEAR |
    (1U << sercos::SERCOS_SCA_POS_TYPE_UNIT_OFFSET),  3,         -7,      3600000, 3*(1E-7)*0.0254, AxisProfileResult::STS_OK,                L"Case: scaling method 'Linear' in inches"},
    {sercos::SERCOS_SCA_POS_TYPE_METHOD_LINEAR,       5,         -7,      3600000, 0.0000005,       AxisProfileResult::STS_OK,                L"Case: scaling method 'Linear' in meters"},
    {sercos::SERCOS_SCA_POS_TYPE_METHOD_ROT,          1,         -7,      0,       1.0,             AxisProfileResult::ERR_MATH,              L"Error-Case: scaling method 'Rotational' with invalid config."},
    {0x0003,                                          1,         -7,      360,     1.0,             AxisProfileResult::ERR_INVALID_ARGUMENT,  L"Error-Case: scaling undefined method"},
  }; 
  HRESULT hResult = S_OK;
  log(L"\n<b>Test of LSB weight for position scaling data.</b>");

  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  test::MockEthercatMasterParamRequestNodeSoE paramRequestNodeMockSoE{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  mockClient.m_testNode = &paramRequestNodeMockSoE;
  std::uint32_t deviceAddress = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_deviceAddress = deviceAddress;

  sercos::ScalingStrategySoEDrive testClass{profileBasicInfo};
  AxisProfileSharingHandler sHandler(&mockProfileTrace);
  test::MockScalingDriveDeviceParam testDeviceParam {profileBasicInfo};
  test::MockAxisProfileProperty property {};

  sHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &testClass);
  sHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
  sHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);

  log(L"\n\n<b>Call calcScalingLSBWeights() and simulate error caused by nullptr.</b>");
  AxisProfileResult actResult = testClass.booting();
  LOG_TEST_RESULT(actResult, L"Method did not fail with expected error code!", AxisProfileResult::ERR_EXEC_NULL);  

  // add assign item
  test::MockAxisProfileTrace profileTraceMockup {};
  comm::axisprofile::sercos::SercosBootParamSnapshot bootParamSnapshot {profileBasicInfo};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  //mock for position
  auto& assign = mappingHandlerMockup.getProfileAssignedInfo();
  assign.m_input.m_position = true;
  mappingHandlerMockup.setReturnValue(assign);
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  std::unique_ptr<comm::axisprofile::DriveCategoryAdapterSoE> mockCategory = std::make_unique<DriveCategoryAdapterSoE>(profileBasicInfo, &atData, &mdtData);
  AxisProfileSharingHandler sharingHandler(&profileTraceMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &testClass);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, mockCategory.get());
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_BOOTPARAMSNAPSHOT, &bootParamSnapshot);

  log(L"\n\n<b>Call calcScalingLSBWeights() and simulate error in the fieldbus adapter.</b>");
  mockFieldbusAdapter.m_returnValue = AxisProfileResult::ERR_FIELDBUS;
  actResult = testClass.booting();
  LOG_TEST_RESULT(actResult, L"Method did not fail with expected error code!", AxisProfileResult::ERR_FIELDBUS);

  mockFieldbusAdapter.m_returnValue = AxisProfileResult::STS_OK;

  for (const auto& testCase : testDataPosScaling)
  {
    hResult |= runTestCasePosScaling(paramRequestNodeMockSoE, testClass, testCase);
  }

  return hResult;
}

HRESULT TestScalingStrategySoEDrive::runTestCasePosScaling(test::MockEthercatMasterParamRequestNodeSoE& paramRequestNode, sercos::ScalingStrategySoEDrive& scaling, const TestPosData& testCaseData)
{
  HRESULT hResult = S_OK;

  log(L"\n<b>%ls</b>", testCaseData.m_testName);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0076_SCALING_POS_TYPE, testCaseData.m_posScalingType);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0077_SCALING_POS_FACTOR, testCaseData.m_posScalingFactor);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0078_SCALING_POS_EXPONENT, testCaseData.m_posScalingExponent);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0079_SCALING_POS_ROT_RES, testCaseData.m_posRotationalResolution);
  printScalingParams(testCaseData);

  AxisProfileResult actResult = scaling.calcPosScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"\nMethod did not fail with expected error code!", testCaseData.m_expResult);

  const double scalingLSBWeight = scaling.getPosScalingLSBWeight();
  LOG_TEST_RESULT_FLT(scalingLSBWeight, L"\nCalculated LSBWeight is not correct!", testCaseData.m_expLSBWeight, 1e-10);

  const double scalingLSBWeight_inv = scaling.getPosScalingLSBWeight_inv();
  LOG_TEST_RESULT_FLT(scalingLSBWeight_inv, L"\nscalingLSBWeight_inv LSBWeight is not correct!", 1/testCaseData.m_expLSBWeight, 1e-4);

  return hResult;
}

inline void TestScalingStrategySoEDrive::printScalingParams(const TestPosData& testCaseData)
{
  log(L"\n\tS-0-0076 = 0x%04X", testCaseData.m_posScalingType);
  log(L"\n\tS-0-0077 = %d", testCaseData.m_posScalingFactor);
  log(L"\n\tS-0-0078 = %d", testCaseData.m_posScalingExponent);
  log(L"\n\tS-0-0079 = %d", testCaseData.m_posRotationalResolution);
}

HRESULT TestScalingStrategySoEDrive::testCalcVelScalingLSBWeight()
{
  const std::vector<TestVelData> testDataVelScaling
  {
  // ScaType,                                 ScaFactor, ScaExpo,  ExpVal,  ExpTimeUnit,      ExpVelScalingType,         ExpErrorCode,                          Name
    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_ROT,             1, -4,  0.0006,    TimeUnit::MINUTE, VelScalingType::ROTARY,    AxisProfileResult::STS_OK,             L"Case: default values, scaling method 'Rotational', time unit: minute"},
    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_ROT |
    (1U << sercos::SERCOS_SCA_VEL_TYPE_UNIT_OFFSET) |
    (1U << sercos::SERCOS_SCA_VEL_TYPE_TIME_UNIT_OFFSET), 1, -3, 1E-3,      TimeUnit::SECOND, VelScalingType::ROTARY,    AxisProfileResult::STS_OK,             L"Case: scaling method 'ROTARY' in deg/s"},
    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_ROT |
    (1U << sercos::SERCOS_SCA_VEL_TYPE_TIME_UNIT_OFFSET),1, -6,  0.00036,   TimeUnit::SECOND, VelScalingType::ROTARY,    AxisProfileResult::STS_OK,             L"Case: scaling method 'Rotational', time unit: second"},
    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_NONE,            1, -6,  1,         TimeUnit::MINUTE, VelScalingType::UNDEFINED, AxisProfileResult::STS_OK,             L"Case: scaling method 'None'"},
    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_LINEAR |
    (1U << sercos::SERCOS_SCA_VEL_TYPE_UNIT_OFFSET),     3, -3,  1.27E-6,   TimeUnit::MINUTE, VelScalingType::LINEAR,    AxisProfileResult::STS_OK,             L"Case: scaling method 'Linear' in inch/min"},
    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_LINEAR |
    (1U << sercos::SERCOS_SCA_VEL_TYPE_UNIT_OFFSET) |
    (1U << sercos::SERCOS_SCA_VEL_TYPE_TIME_UNIT_OFFSET),3, -3,  7.62E-5,   TimeUnit::SECOND, VelScalingType::LINEAR,    AxisProfileResult::STS_OK,             L"Case: scaling method 'Linear' in inch/s"},

    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_LINEAR,  1, -3,  1.666666666667E-5, TimeUnit::MINUTE, VelScalingType::LINEAR,    AxisProfileResult::STS_OK,             L"Case: scaling method 'Linear' in meters/min"},
    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_LINEAR |
    (1U << sercos::SERCOS_SCA_VEL_TYPE_TIME_UNIT_OFFSET),1, -5,  1E-5,      TimeUnit::SECOND, VelScalingType::LINEAR,    AxisProfileResult::STS_OK,             L"Case: scaling method 'Linear' in meters/s"},
    {sercos::SERCOS_SCA_VEL_TYPE_METHOD_LINEAR,          0, -4,  1,         TimeUnit::MINUTE, VelScalingType::LINEAR,    AxisProfileResult::ERR_MATH,           L"Case: zero scaling factor"},
    {0x0003,                                             1, -4,  1,         TimeUnit::MINUTE, VelScalingType::UNDEFINED, AxisProfileResult::ERR_NOTIMPLEMENTED, L"Case: wrong scaling type"},
  }; 
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of LSB weight for velocity scaling data.</b>");

  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  test::MockEthercatMasterParamRequestNodeSoE paramRequestNodeMockSoE{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  mockClient.m_testNode = &paramRequestNodeMockSoE;
  std::uint32_t deviceAddress = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  profileBasicInfo.m_deviceAddress = deviceAddress;

  sercos::ScalingStrategySoEDrive testClass{profileBasicInfo};

  log(L"\n\n<b>Call calcScalingLSBWeights() and simulate error caused by nullptr.</b>");
  AxisProfileResult actResult = testClass.calcVelScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"Method did not fail with expected error code!", AxisProfileResult::ERR_EXEC_NULL);

  // add assign item
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockScalingDriveDeviceParam testDeviceParam {profileBasicInfo};
  //mock for position
  auto& assign = mappingHandlerMockup.getProfileAssignedInfo();
  assign.m_input.m_velocity = true;
  assign.m_output.m_velocity = true;
  mappingHandlerMockup.setReturnValue(assign);

  comm::axisprofile::sercos::SercosBootParamSnapshot bootParamSnapshot {profileBasicInfo};
  AxisProfileSharingHandler sharingHandler(&profileTraceMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &testClass);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_BOOTPARAMSNAPSHOT, &bootParamSnapshot);

  log(L"\n\n<b>Call calcScalingLSBWeights() and simulate error in the fieldbus adapter.</b>");
  mockFieldbusAdapter.m_returnValue = AxisProfileResult::ERR_FIELDBUS;
  actResult = testClass.calcVelScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"\nMethod did not fail with expected error code!", AxisProfileResult::ERR_FIELDBUS);
  mockFieldbusAdapter.m_returnValue = AxisProfileResult::STS_OK;

  for (const auto& testCase : testDataVelScaling)
  {
    hResult |= runTestCaseVelScaling(paramRequestNodeMockSoE, testClass, testCase);
  }

  return hResult;
}

HRESULT TestScalingStrategySoEDrive::runTestCaseVelScaling(test::MockEthercatMasterParamRequestNodeSoE& paramRequestNode, sercos::ScalingStrategySoEDrive& scaling, const TestVelData& testCaseData)
{
  HRESULT hResult = S_OK;

  log(L"\n<b>%ls</b>", testCaseData.m_testName);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0044_SCALING_VEL_TYPE, testCaseData.m_velScalingType);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0045_SCALING_VEL_FACTOR, testCaseData.m_velScalingFactor);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0046_SCALING_VEL_EXPONENT, testCaseData.m_velScalingExponent);
  printScalingParams(testCaseData);

  AxisProfileResult actResult = scaling.calcVelScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"\nMethod did not fail with expected error code!", testCaseData.m_expResult);

  LOG_TEST_RESULT(scaling.m_velocityType, L"\nVelocity scaling Type is not correct!", testCaseData.m_expVelType);

  LOG_TEST_RESULT(scaling.m_timeUnit, L"\nVelocity scaling unit is not correct!", testCaseData.m_expTimeUnit);

  const double scalingLSBWeight = scaling.getVelScalingLSBWeight();
  LOG_TEST_RESULT_FLT(scalingLSBWeight, L"\nCalculated LSBWeight is not correct!", testCaseData.m_expLSBWeight, 1e-10);

  const double scalingLSBWeight_inv = scaling.getVelScalingLSBWeight_inv();
  LOG_TEST_RESULT_FLT(scalingLSBWeight_inv, L"\nCalculated LSBWeight_inv is not correct!", 1.0/testCaseData.m_expLSBWeight, 1e-4);

  return hResult;
}

inline void TestScalingStrategySoEDrive::printScalingParams(const TestVelData& testCaseData)
{
  log(L"\n\tS-0-0044 = 0x%04X", testCaseData.m_velScalingType);
  log(L"\n\tS-0-0045 = %d", testCaseData.m_velScalingFactor);
  log(L"\n\tS-0-0046 = %d", testCaseData.m_velScalingExponent);
}

HRESULT TestScalingStrategySoEDrive::testCalcTorqueScalingLSBWeight()
{
  const std::vector<TestTorqueData> testDataTorqueScaling 
  {
    // translPosFactor, translPosExponent,                                   ScaType, ScaFactor, ScaExpo, Input, Output,    Feed,  Nominal,       ExpVal, ExpMetrication,            ExpScalingType,                                 ExpErrorCode,                          Name
    {                1,                -7, sercos::SERCOS_SCA_TORQUE_TYPE_METHOD_ROT,         1,      -2,     1,      1,      10,        1,         0.01,           true, TorqueScalingType::ROTARY,      AxisProfileResult::ERR_INVALID_ARGUMENT, L"Case: default values, scaling method 'Rotational' in N.m"},
    {                1,                -7, sercos::SERCOS_SCA_TORQUE_TYPE_METHOD_ROT |
                                          (1U << sercos::SERCOS_SCA_TORQUE_TYPE_UNIT_OFFSET), 2,      -1,     1,      1,      10,        1, 0.2*0.112985,          false, TorqueScalingType::ROTARY,      AxisProfileResult::ERR_INVALID_ARGUMENT, L"Case: default values, scaling method 'Rotational' in inIbf"},
    {                1,                -7, sercos::SERCOS_SCA_TORQUE_TYPE_METHOD_LINEAR |
                                          (1U << sercos::SERCOS_SCA_TORQUE_TYPE_UNIT_OFFSET), 2,      -1,     1,      1, 100000u,     1000,  0.2*4.44822,          false, TorqueScalingType::LINEAR,      AxisProfileResult::STS_OK,               L"Case: default values, scaling method 'linear' in Ibf"},
    {                1,                -7, sercos::SERCOS_SCA_TORQUE_TYPE_METHOD_PCTG,        1,      -1,      1,     1,  10000u,     1000,       1*2*PI,           true, TorqueScalingType::PERCENTAGE,  AxisProfileResult::STS_OK,               L"Case: scaling method 'percentage"},
    {                1,                -7, sercos::SERCOS_SCA_TORQUE_TYPE_METHOD_PCTG,        1,      -1,      1,     1, 100000u,     1000,     0.1*2*PI,           true, TorqueScalingType::PERCENTAGE,  AxisProfileResult::STS_OK,               L"Case: scaling method 'percentage"},
    {                1,                -7, sercos::SERCOS_SCA_TORQUE_TYPE_METHOD_LINEAR,      1,       0,      1,     1, 100000u,     1000,            1,           true, TorqueScalingType::LINEAR,      AxisProfileResult::STS_OK,               L"Case: scaling method 'Linear' in N"},
    {                1,                -7, sercos::SERCOS_SCA_TORQUE_TYPE_METHOD_LINEAR,      0,      -4,      1,     1, 100000u,     1000,            1,           true, TorqueScalingType::LINEAR,      AxisProfileResult::ERR_MATH,             L"Case: zero scaling factor"},
    {                1,                -7, 0x0003,                                            1,      -4,      1,     1,      10,        1,            1,           true, TorqueScalingType::UNDEFINED,   AxisProfileResult::ERR_INVALID_ARGUMENT, L"Case: wrong scaling type"},
  };
  HRESULT hResult = S_OK;
  log(L"\n<b>Test of LSB weight for torque scaling data.</b>");

  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  test::MockEthercatMasterParamRequestNodeSoE paramRequestNodeMockSoE{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  mockClient.m_testNode = &paramRequestNodeMockSoE;
  std::uint32_t deviceAddress = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_deviceAddress = deviceAddress;
  sercos::ScalingStrategySoEDrive testClass{profileBasicInfo};

  log(L"\n\n<b>Call calcScalingLSBWeights() and simulate error caused by nullptr.</b>");
  AxisProfileResult actResult = testClass.calcTorqueScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"Method did not fail with expected error code!", AxisProfileResult::ERR_EXEC_NULL);

  // add assign item
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  test::MockScalingDriveDeviceParam testDeviceParam {profileBasicInfo};
  //mock for position
  auto& assign = mappingHandlerMockup.getProfileAssignedInfo();
  assign.m_input.m_torqueForce = true;
  assign.m_input.m_position = true;
  mappingHandlerMockup.setReturnValue(assign);

  comm::axisprofile::sercos::SercosBootParamSnapshot bootParamSnapshot {profileBasicInfo};
  AxisProfileSharingHandler sharingHandler(&profileTraceMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &testClass);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_BOOTPARAMSNAPSHOT, &bootParamSnapshot);

  log(L"\n\n<b>Call calcScalingLSBWeights() and simulate error in the fieldbus adapter.</b>");
  mockFieldbusAdapter.m_returnValue = AxisProfileResult::ERR_FIELDBUS;
  actResult = testClass.calcTorqueScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"Method did not fail with expected error code!", AxisProfileResult::STS_BOOTING_CONTINUE);

  mockFieldbusAdapter.m_returnValue = AxisProfileResult::STS_OK;

  for (const auto& testCase : testDataTorqueScaling)
  {
    hResult |= runTestCaseTorqueScaling(paramRequestNodeMockSoE, testClass, testCase);
  }

  return hResult;
}

HRESULT TestScalingStrategySoEDrive::runTestCaseTorqueScaling(test::MockEthercatMasterParamRequestNodeSoE& paramRequestNode, sercos::ScalingStrategySoEDrive& scaling, const TestTorqueData& testCaseData)
{
  HRESULT hResult = S_OK;

  log(L"\n<b>%ls</b>", testCaseData.m_testName);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0077_SCALING_POS_FACTOR, testCaseData.m_translPosScalingFactor);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0078_SCALING_POS_EXPONENT, testCaseData.m_translPosScalingExponent);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0086_SCALING_TORQUE_TYPE, testCaseData.m_torqueScalingType);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0093_SCALING_TORQUE_FACTOR, testCaseData.m_torqueScalingFactor);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0094_SCALING_TORQUE_EXPONENT, testCaseData.m_torqueScalingExponent);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0533_NOMINAL_TORQUE, testCaseData.m_nominalTorque);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0121_GEAR_INPUT_REVOLUTION, testCaseData.m_gearInputRevolution);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0122_GEAR_OUTPUT_REVOLUTION, testCaseData.m_gearOutputRevolution);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0123_FEED_CONSTANT, testCaseData.m_feedConstant);
  printScalingParams(testCaseData);

  //prepare related position data before torque scaling
  scaling.calcPosScalingLSBWeight();
  AxisProfileResult actResult = scaling.calcTorqueScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"\nMethod did not fail with expected error code!", testCaseData.m_expResult);

  LOG_TEST_RESULT(scaling.m_torqueType, L"\nTorque scaling type is not correct!", testCaseData.m_expTorqueType);

  LOG_TEST_RESULT(scaling.m_torqueMetrication, L"\nMetric system is not correct!", testCaseData.m_expMetrication);

  const double scalingLSBWeight = scaling.getTorqueScalingLSBWeight();
  LOG_TEST_RESULT_FLT(scalingLSBWeight, L"\nCalculated LSBWeight is not correct!", testCaseData.m_expLSBWeight, 1e-4);

  const double scalingLSBWeight_inv = scaling.getTorqueScalingLSBWeight_inv();
  LOG_TEST_RESULT_FLT(scalingLSBWeight_inv, L"\nCalculated LSBWeight inv is not correct!", 1/testCaseData.m_expLSBWeight, 1e-4);

  return hResult;
}

inline void TestScalingStrategySoEDrive::printScalingParams(const TestTorqueData& testCaseData)
{
  log(L"\n\tS-0-0077 = 0x%04X", testCaseData.m_translPosScalingFactor);
  log(L"\n\tS-0-0078 = %d", testCaseData.m_translPosScalingExponent);
  log(L"\n\tS-0-0086 = 0x%04X", testCaseData.m_torqueScalingType);
  log(L"\n\tS-0-0093 = %d", testCaseData.m_torqueScalingFactor);
  log(L"\n\tS-0-0094 = %d", testCaseData.m_torqueScalingExponent);
}

HRESULT TestScalingStrategySoEDrive::testCalcAccScalingLSBWeight()
{
  const std::vector<TestAccData> testDataAccScaling
  {
    // ScaType,                                     ScaFactor, ScaExpo,  ExpVal,     ExpErrorCode,                          Name
    {sercos::SERCOS_SCA_ACC_TYPE_METHOD_LINEAR |
    (1U << sercos::SERCOS_SCA_ACC_TYPE_UNIT_OFFSET),  1, -6,  1E-6 * 0.0254,         AxisProfileResult::STS_OK,             L"Case: scaling method 'Linear' in inches/s^2"},
    {sercos::SERCOS_SCA_ACC_TYPE_METHOD_LINEAR,       1, -7,  1E-7,                  AxisProfileResult::STS_OK,             L"Case: scaling method 'Linear' in meters/s^2"},
    {sercos::SERCOS_SCA_ACC_TYPE_METHOD_ROT,          1, -4,  0.0001*57.29577951471995,  AxisProfileResult::STS_OK,         L"Case: default values, scaling method 'Rotational'"},
    {sercos::SERCOS_SCA_ACC_TYPE_METHOD_NONE,         1, -7,  1.0,                   AxisProfileResult::STS_OK,             L"Case: scaling method 'None'"},
    {sercos::SERCOS_SCA_ACC_TYPE_METHOD_LINEAR |
    (1U << sercos::SERCOS_SCA_ACC_TYPE_UNIT_OFFSET),  3, -7,  3*(1E-7)*0.0254,       AxisProfileResult::STS_OK,             L"Case: scaling method 'Linear' in inches/s^2"},
    {sercos::SERCOS_SCA_ACC_TYPE_METHOD_LINEAR,       5, -7,  0.0000005,             AxisProfileResult::STS_OK,             L"Case: scaling method 'Linear' in meters/s^2"},
    {sercos::SERCOS_SCA_ACC_TYPE_METHOD_LINEAR,       0, -7,  1.0,                   AxisProfileResult::ERR_MATH,           L"Error-Case: scaling method 'Rotational' with invalid config."},
    {0x0003,                                          1, -7,  1.0,                   AxisProfileResult::ERR_NOTIMPLEMENTED, L"Error-Case: scaling undefined method"},
  };
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of LSB weight for acceleration scaling data.</b>");

  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  test::MockEthercatMasterParamRequestNodeSoE paramRequestNodeMockSoE{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  mockClient.m_testNode = &paramRequestNodeMockSoE;
  std::uint32_t deviceAddress = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;
  profileBasicInfo.m_deviceAddress = deviceAddress;


  sercos::ScalingStrategySoEDrive testClass{profileBasicInfo};
  comm::axisprofile::sercos::SercosBootParamSnapshot bootParamSnapshot {profileBasicInfo};
  test::MockScalingDriveDeviceParam testDeviceParam {profileBasicInfo};
  AxisProfileSharingHandler sharingHandler(&mockProfileTrace);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &testClass);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testDeviceParam);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_BOOTPARAMSNAPSHOT, &bootParamSnapshot);

  log(L"\n\n<b>Call calcScalingLSBWeights() and simulate error in the fieldbus adapter.</b>");
  mockFieldbusAdapter.m_returnValue = AxisProfileResult::ERR_FIELDBUS;
  AxisProfileResult actResult = testClass.calcAccScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"\nMethod did not fail with expected error code!", AxisProfileResult::ERR_FIELDBUS);

  mockFieldbusAdapter.m_returnValue = AxisProfileResult::STS_OK;

  for (const auto& testCase : testDataAccScaling)
  {
    hResult |= runTestCaseAccScaling(paramRequestNodeMockSoE, testClass, testCase);
  }

  return hResult;
}

HRESULT TestScalingStrategySoEDrive::runTestCaseAccScaling(test::MockEthercatMasterParamRequestNodeSoE& paramRequestNode, sercos::ScalingStrategySoEDrive& scaling, const TestAccData& testCaseData)
{
  HRESULT hResult = S_OK;

  log(L"\n<b>%ls</b>", testCaseData.m_testName);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0160_SCALING_ACC_TYPE, testCaseData.m_accScalingType);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0161_SCALING_ACC_FACTOR, testCaseData.m_accScalingFactor);
  paramRequestNode.m_fbsHelper.writeParam(S_0_0162_SCALING_ACC_EXPONENT, testCaseData.m_accScalingExponent);
  printScalingParams(testCaseData);

  AxisProfileResult actResult = scaling.calcAccScalingLSBWeight();
  LOG_TEST_RESULT(actResult, L"\nMethod did not fail with expected error code!", testCaseData.m_expResult);

  const double scalingLSBWeight = scaling.getAccScalingLSBWeight();
  LOG_TEST_RESULT_FLT(scalingLSBWeight, L"\nCalculated LSBWeight is not correct!", testCaseData.m_expLSBWeight, 1e-7);

  const double scalingLSBWeight_inv = scaling.getAccScalingLSBWeight_inv();  
  LOG_TEST_RESULT_FLT(scalingLSBWeight_inv, L"\nCalculated LSBWeight_inv is not correct!", 1/testCaseData.m_expLSBWeight, 1e-4);

  return hResult;
}

inline void TestScalingStrategySoEDrive::printScalingParams(const TestAccData& testCaseData)
{
  log(L"\n\tS-0-0160 = 0x%04X", testCaseData.m_accScalingType);
  log(L"\n\tS-0-0161 = %d", testCaseData.m_accScalingFactor);
  log(L"\n\tS-0-0162 = %d", testCaseData.m_accScalingExponent);
}

HRESULT TestScalingStrategySoEDrive::testGetterAndSetter()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test of data access interface of scaling data.</b>");

  test::MockAxisProfileTrace mockProfileTrace{};
  test::MockFieldbusAdapterSoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  test::MockEthercatMasterParamRequestNodeSoE paramRequestNodeMockSoE{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  mockClient.m_testNode = &paramRequestNodeMockSoE;
  std::uint32_t deviceAddress = 1;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;
  profileBasicInfo.m_profileName = m_profileName;

  profileBasicInfo.m_deviceAddress = deviceAddress;

  sercos::ScalingStrategySoEDrive testClass{ profileBasicInfo };
  
  //test for setter and getter
  std::uint32_t expectValue = 1;
  std::uint32_t actualValue = testClass.getEncoderResolution();
  LOG_TEST_RESULT(actualValue, L"\n The actual encoder resolution is not equal to expected one!", expectValue);

  actualValue = testClass.getScalingDenominator();
  LOG_TEST_RESULT(actualValue, L"\n The actual denominator is not equal to expected one!", expectValue);

  actualValue = testClass.getScalingNumerator();
  LOG_TEST_RESULT(actualValue, L"\n The actual numerator is not equal to expected one!", expectValue);

  log(L"\n<b>Some dummy test only for code coverage.</b>");
  testClass.setDeviceScalingType(AxisProfileDeviceScalingType::LINEAR);
  testClass.setEncoderResolution(8388608);
  testClass.setScalingDenominator(100000);
  testClass.setScalingNumerator(8388608);

  return hResult;
}

HRESULT TestScalingStrategySoEDrive::testModulo() const
{
  ProfileBasicInfo profileBasicInfo;
  sercos::ScalingStrategySoEDrive testClass {profileBasicInfo};
  testClass.m_moduloValue = 100;
  test::MockScalingDriveDeviceParam testDeviceParam {profileBasicInfo};

  log(L"\n\tget modulo value.");
  auto modValue = testClass.getModValue();
  LOG_TEST_RESULT_FLT(modValue, L"Incorrect modulo value!", testClass.m_moduloValue, STD_EPS);

  log(L"\n\tcheckAndSetModulo when inconsistency value.");
  std::string errTxt {};
  auto result = testClass.checkAndSetModulo(0.0, errTxt);
  LOG_TEST_RESULT(result, L"\nMethod did not fail with expected error code!", AxisProfileResult::ERR_INCONSISTENCY);
  log(L"\n\tErrTxt: %ls", UTF8TOWIDE(errTxt));

  log(L"\n\tcheckAndSetModulo when consistency value.");
  result = testClass.checkAndSetModulo(testClass.m_moduloValue, errTxt);
  LOG_TEST_RESULT(result, L"\nMethod did not fail with expected error code!", AxisProfileResult::STS_OK);
  log(L"\n\tErrTxt: %ls", UTF8TOWIDE(errTxt));

  return S_OK;
}
