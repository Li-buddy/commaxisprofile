#include "helper/result_helper.h"
#include "profiles/canopen/drive/canopen_scaling_strategy_drive.h"
#include "test_scaling.h"
#include "profiles/data_sharing/sharing_handler.h"
#include <limits>

// Mockups
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_axisprofile_mapping_handler.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestScaling);
REGISTER_TEST_TO_GROUP(TestScaling, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestScaling, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestScaling, Profiles_Scaling);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestScaling::TestScaling() : m_testDataSet
{
  {PosScalingType::ROTARY, VelScalingType::ROTARY, AccScalingType::ROTARY, true,  true, true,   AxisProfileResult::STS_OK,   L"ALL rotary"},
  {PosScalingType::LINEAR, VelScalingType::LINEAR, AccScalingType::LINEAR, false, false, false, AxisProfileResult::STS_OK,   L"Linear, imperial unit"},
  {PosScalingType::ROTARY, VelScalingType::LINEAR, AccScalingType::ROTARY, true,  true,  true,   AxisProfileResult::ERR_NOK, L"Conflict Scaling"},
  {PosScalingType::LINEAR, VelScalingType::LINEAR, AccScalingType::LINEAR, false, true,  false,  AxisProfileResult::ERR_NOK, L"Conflict metric"},
}
{}


TestScaling::~TestScaling(void)
{}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestScaling::doInit()
{
  return S_OK;
}

HRESULT TestScaling::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Scaling class");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testcheckScalingUniform();
  hResult |= testGetDeviceType();
  hResult |= testModuloMethods();

  ProfileBasicInfo profileBasicInfo;
  canopen::ScalingStrategyCoEDrive scaling{profileBasicInfo};
  scaling.setOutOfRangeErrLimit(2U);

  const auto checkClamp = [&](auto toRaw, ScalingDataCategory category, const ClampState& state) -> HRESULT
  {
    const auto convert = [&](double value) { return (scaling.*toRaw)(value); };
    using Raw = decltype(convert(0.0));
    constexpr Raw minimum = std::numeric_limits<Raw>::min();
    constexpr Raw maximum = std::numeric_limits<Raw>::max();
    const auto count = [&]() { return scaling.getOutOfRangeErrCnt(category); };

    if (convert(minimum) != minimum || count() != 0U || convert(maximum) != maximum || count() != 0U)
    {
      error(L"Exact limits must not count as a clamp (category %u).", static_cast<unsigned>(category));
      return E_FAIL;
    }
    for (unsigned burst = 0; burst < 2; ++burst)
    {
      if (convert(static_cast<double>(minimum) - 1.0) != minimum || count() != 1U || state.reported ||
          convert(static_cast<double>(maximum) + 1.0) != maximum || count() != 0U || !state.reported)
      {
        error(L"Unexpected saturation or threshold state (category %u).", static_cast<unsigned>(category));
        return E_FAIL;
      }
      if (convert(std::numeric_limits<double>::quiet_NaN()) != 0 || count() != 1U ||
          convert(std::numeric_limits<double>::infinity()) != maximum || count() != 0U || !state.reported ||
          convert(-std::numeric_limits<double>::infinity()) != minimum || count() != 1U)
      {
        error(L"Unexpected conversion or counter for non-finite input (category %u).", static_cast<unsigned>(category));
        return E_FAIL;
      }
      if (convert(-3.75) != -3 || count() != 0U || state.reported)
      {
        error(L"An in-range sample must truncate and reset the clamp state (category %u).", static_cast<unsigned>(category));
        return E_FAIL;
      }
    }
    return S_OK;
  };
  hResult |= checkClamp(&Scaling::convertPositionScaledToRaw, ScalingDataCategory::SCALINGDATA_POSITION, scaling.m_positionClamp);
  hResult |= checkClamp(&Scaling::convertVelocityScaledToRaw, ScalingDataCategory::SCALINGDATA_VELOCITY, scaling.m_velocityClamp);
  hResult |= checkClamp(&Scaling::convertTorqueScaledToRaw, ScalingDataCategory::SCALINGDATA_TORQUE, scaling.m_torqueClamp);

  scaling.m_posScalingLSBWeight = 2.0;
  scaling.m_posScalingLSBWeight_inv = 0.5;
  scaling.m_velScalingLSBWeight = 4.0;
  scaling.m_velScalingLSBWeight_inv = 0.25;
  scaling.m_torqueScalingLSBWeight = 0.5;
  scaling.m_torqueScalingLSBWeight_inv = 2.0;
  if (scaling.convertPositionRawToScaled(3) != 6.0 || scaling.convertPositionScaledToRaw(6.0) != 3 ||
      scaling.convertVelocityRawToScaled(3) != 12.0 || scaling.convertVelocityScaledToRaw(12.0) != 3 ||
      scaling.convertTorqueRawToScaled(3) != 1.5 || scaling.convertTorqueScaledToRaw(1.5) != 3)
  {
    error(L"Conversion did not use the configured weight for its category.");
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}

HRESULT TestScaling::doEnd()
{
  return S_OK;
}

HRESULT TestScaling::testcheckScalingUniform()
{
  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of check scaling uniformity.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;

  // add assign item
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  //mock for position
  auto& assign = mappingHandlerMockup.getProfileAssignedInfo();
  assign.m_input.m_position = true;
  assign.m_input.m_velocity = true;
  assign.m_input.m_torqueForce = true;
  mappingHandlerMockup.setReturnValue(assign);

  canopen::ScalingStrategyCoEDrive testingScaling {profileBasicInfo};
  AxisProfileSharingHandler sharingHandler(&profileTraceMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, &testingScaling);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
    
  testingScaling.calcScalingLSBWeights();
  for(const auto& testCase : m_testDataSet)
  {
    hResult |= executeCheckScalingUniform(&testingScaling, testCase);
  }
  return hResult;
}

HRESULT TestScaling::executeCheckScalingUniform(comm::axisprofile::Scaling* testingScaling, const TestData& testCase)
{
  HRESULT hResult = S_OK;;
  testingScaling->m_positionType     = testCase.m_positionType;
  testingScaling->m_velocityType     = testCase.m_velocityType;
  testingScaling->m_accelerationType = testCase.m_accelerationType;
  testingScaling->m_posMetrication   = testCase.m_posMetrication;
  testingScaling->m_velMetrication   = testCase.m_velMetrication;
  testingScaling->m_accMetrication   = testCase.m_accMetrication;

  AxisProfileResult result = testingScaling->checkScalingUniform();
  if(result != testCase.m_expResult)
  {
    error(L"\n\t %ls test fails! Actual: %d, Expected: %d", testCase.m_testName, result, testCase.m_expResult);
    hResult = E_FAIL;
  }
  else
  {
    log(L"\n\t %ls test... <DARK_GREEN>OK</DARK_GREEN>", testCase.m_testName );
  }
  return hResult;
}

HRESULT TestScaling::testGetDeviceType()
{
  const std::vector<TestDataDeviceType> testDataSet =
  {
   // PositionScalingType         VelocityScalingType        TorqueScalingType              AccelerationScalingType    expected position scaling type              expected velocity scaling type            expected torque scaling type              expected acceleration scaling type        test name
    { PosScalingType::ROTARY,     VelScalingType::ROTARY,    TorqueScalingType::ROTARY,     AccScalingType::ROTARY,    AxisProfileDeviceScalingType::ROTATIONAL,   AxisProfileDeviceScalingType::ROTATIONAL, AxisProfileDeviceScalingType::ROTATIONAL, AxisProfileDeviceScalingType::ROTATIONAL, L"Rotational Scaling"},
    { PosScalingType::LINEAR,     VelScalingType::LINEAR,    TorqueScalingType::LINEAR,     AccScalingType::LINEAR,    AxisProfileDeviceScalingType::LINEAR,       AxisProfileDeviceScalingType::LINEAR,     AxisProfileDeviceScalingType::LINEAR,     AxisProfileDeviceScalingType::LINEAR,     L"Linear Scaling"},
    { PosScalingType::LINEAR,     VelScalingType::LINEAR,    TorqueScalingType::PERCENTAGE, AccScalingType::LINEAR,    AxisProfileDeviceScalingType::LINEAR,       AxisProfileDeviceScalingType::LINEAR,     AxisProfileDeviceScalingType::PERCENTAGE, AxisProfileDeviceScalingType::LINEAR,     L"torque percentage Scaling"},
    { PosScalingType::UNDEFINED,  VelScalingType::UNDEFINED, TorqueScalingType::UNDEFINED,  AccScalingType::UNDEFINED, AxisProfileDeviceScalingType::UNDEFINED,    AxisProfileDeviceScalingType::UNDEFINED,  AxisProfileDeviceScalingType::UNDEFINED,  AxisProfileDeviceScalingType::UNDEFINED,  L"Undefined Scaling"},
  };


  HRESULT hResult = S_OK;;
  log(L"\n<b>Test of device scaling type.</b>");
  test::MockFieldbusAdapterCoE mockFieldbusAdapter{};
  test::MockDatalayerClient mockClient{};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_fieldbusAdapter = &mockFieldbusAdapter;

  canopen::ScalingStrategyCoEDrive testingScaling{profileBasicInfo};
  for (const auto& testCase : testDataSet)
  {
    hResult |= executeGetDeviceType(&testingScaling, testCase);
  }

  // check torque and undefined
  if (testingScaling.getDeviceScalingType(AxisProfileDeviceValueType::UNDEFINED) != AxisProfileDeviceScalingType::UNDEFINED)
  {
    error(L"Scaling type of undefined is not undefined like expected");
  }

  testingScaling.getTorqueType(); // only for code coverage
  return hResult;
}

HRESULT TestScaling::executeGetDeviceType(comm::axisprofile::Scaling* testingScaling, const TestDataDeviceType& testCase)
{
  HRESULT hResult = S_OK;

  log(L"\n\t Running testcase %ls", testCase.m_testName);
  testingScaling->m_positionType     = testCase.m_positionType;
  testingScaling->m_velocityType     = testCase.m_velocityType;
  testingScaling->m_torqueType       = testCase.m_torqueType;
  testingScaling->m_accelerationType = testCase.m_accelerationType;
  // check torque and undefined
  if (testingScaling->getDeviceScalingType(AxisProfileDeviceValueType::POSITION) != testCase.m_expDeviceTypePos)
  {
    error(L"Scaling type of position is not as expected.");
  }

  // check velocity and undefined
  if (testingScaling->getDeviceScalingType(AxisProfileDeviceValueType::VELOCITY) != testCase.m_expDeviceTypeVel)
  {
    error(L"Scaling type of velocity is not as expected.");
  }

  // check torque and undefined
  if (testingScaling->getDeviceScalingType(AxisProfileDeviceValueType::TORQUE_FORCE) != testCase.m_expDeviceTypeTorque)
  {
    error(L"Scaling type of torque is not as expected.");
  }

  // check acceleration and undefined
  if (testingScaling->getDeviceScalingType(AxisProfileDeviceValueType::ACCELERATION) != testCase.m_expDeviceTypeAcc)
  {
    error(L"Scaling type of acceleration is not as expected.");
  }  

  return hResult;
}

HRESULT TestScaling::testModuloMethods() const
{
  log(L"\n\n<b>Test modulo related functions.</b>");
  double moduloVlue {123.321};
  std::string errTxt {};

  ProfileBasicInfo profileBasicInfo;
  canopen::ScalingStrategyCoEDrive testingScaling {profileBasicInfo};
  auto scaling = static_cast<Scaling*>(&testingScaling);  //for code coverage

  log(L"\n\tSet and check to new modulo value...");
  auto retValue = scaling->checkAndSetModulo(moduloVlue, errTxt);
  LOG_TEST_RESULT(retValue, L"\nUnexpected return value!", AxisProfileResult::STS_OK);
  log(L"\n\tErrTxt: %ls", UTF8TOWIDE(errTxt));

  log(L"\n\tGet and check to modulo value...");
  auto actModValue = scaling->getModValue();
  LOG_TEST_RESULT_FLT(actModValue, L"Incorrect modulo value!", moduloVlue, STD_EPS);

  return S_OK;
}
