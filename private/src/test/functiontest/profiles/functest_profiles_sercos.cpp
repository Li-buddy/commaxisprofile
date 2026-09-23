#include "comm/axisprofile/interfaces/i_axisprofile_device_param_c.h"
#include "functest_profiles_sercos.h"
#include "helper/result_helper.h"
#include "motion/test/test_helper.h"
#include "manager/axisprofile_manager.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "profiles/sercos/sercos_profile.h"
#include "profiles/value_mapping/mapping_handler.h"

// Mockup
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"

using namespace comm::axisprofile;

REGISTER_TEST(FuncTestProfilesSercos);
REGISTER_TEST_TO_GROUP(FuncTestProfilesSercos, 1_AutoTest);
REGISTER_TEST_TO_GROUP(FuncTestProfilesSercos, 3_FuncTest);
REGISTER_TEST_TO_GROUP(FuncTestProfilesSercos, Profiles_Sercos);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT FuncTestProfilesSercos::doInit()
{
  HRESULT res = S_OK;
  log(L"\n<b>Create function test system...</b>");

  res = startTestEnvironment();
  RETURN_TEST_FAILED(res, L"\nStart axisprofile system failed.");

  res = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(res, L"\nSwitch system to config failed.");

  res = addProfileToSystem(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, 1, m_profileName.c_str(), &m_profile);
  RETURN_TEST_FAILED(res, L"\nCreate test axisprofile failed.");
  RETURN_TEST_RESULT_SET_PTR(m_profile, L"\nCreated axisprofile is empty.");

  log(L"\nConfigure new axisprofile.");
  m_axisProfileComponent->getAxisProfileManager()->setProfileInputBuffer(m_profileName.c_str(), MOCK_ECAT_INPUTBUFFER_STRING);
  m_axisProfileComponent->getAxisProfileManager()->setProfileOutputBuffer(m_profileName.c_str(), MOCK_ECAT_OUTPUTBUFFER_STRING);

  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::STATUSWORD, MOCK_ECAT_INBUFFER_STATUSWORD);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::POSITION, MOCK_ECAT_INBUFFER_POSITION);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::DIAGMSG, MOCK_ECAT_INBUFFER_DIAGMSG);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::VELOCITY, MOCK_ECAT_INBUFFER_VELOCITY);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::TORQUE_FORCE, MOCK_ECAT_INBUFFER_TORQUE_FORCE);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::TORQUE_LIMIT_STATUS, MOCK_ECAT_INBUFFER_TORQUE_FORCE_LIMIT_STATUS);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SWITCH_REF, MOCK_ECAT_INBUFFER_SWITCH_REF);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SWITCH_POS_LIM, MOCK_ECAT_INBUFFER_SWITCH_POS_LIM);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SWITCH_NEG_LIM, MOCK_ECAT_INBUFFER_SWITCH_NEG_LIM);

  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_POSEDG, MOCK_ECAT_INBUFFER_PROBE1_POS_EDG);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_NEGEDG, MOCK_ECAT_INBUFFER_PROBE1_NEG_EDG);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_ACTIVE, MOCK_ECAT_INBUFFER_PROBE1_ACTIVE);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_POSLAT, MOCK_ECAT_INBUFFER_PROBE1_POS_LAT);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_NEGLAT, MOCK_ECAT_INBUFFER_PROBE1_NEG_LAT);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_POSDIF, MOCK_ECAT_INBUFFER_PROBE1_POS_DIF);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_MAKFAL, MOCK_ECAT_INBUFFER_PROBE1_MAK_FAL);

  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_POSEDG, MOCK_ECAT_INBUFFER_PROBE2_POS_EDG);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_NEGEDG, MOCK_ECAT_INBUFFER_PROBE2_NEG_EDG);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_ACTIVE, MOCK_ECAT_INBUFFER_PROBE2_ACTIVE);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_POSLAT, MOCK_ECAT_INBUFFER_PROBE2_POS_LAT);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_NEGLAT, MOCK_ECAT_INBUFFER_PROBE2_NEG_LAT);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_POSDIF, MOCK_ECAT_INBUFFER_PROBE2_POS_DIF);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_MAKFAL, MOCK_ECAT_INBUFFER_PROBE2_MAK_FAL);

  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SMO_SAFETY_SELECTION, MOCK_ECAT_INBUFFER_SMO_SAFETY_SELECTION);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SMO_SAFETY_STATUS, MOCK_ECAT_INBUFFER_SMO_SAFETY_STATUS);

  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_1, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_1);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_2, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_2);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_3, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_3);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_4, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_4);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_5, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_5);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_6, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_6);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SIGNAL_STATUS_WORD, MOCK_ECAT_INBUFFER_SIGNAL_STATUS_WORD);

  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::CONTROLWORD, MOCK_ECAT_OUTBUFFER_CMDWORD);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::POSITION, MOCK_ECAT_OUTBUFFER_POSITION);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::VELOCITY, MOCK_ECAT_OUTBUFFER_VELOCITY);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::TORQUE_FORCE, MOCK_ECAT_OUTBUFFER_TORQUE_FORCE);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH, MOCK_ECAT_OUTBUFFER_TORQUE_FORCE_LIMIT_HIGH);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::TORQUE_VEL_LIMIT_LOW, MOCK_ECAT_OUTBUFFER_TORQUE_FORCE_LIMIT_LOW);

  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_1, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_1);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_2, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_2);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_3, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_3);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_4, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_4);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_5, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_5);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_6, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_6);

  return res;
}

HRESULT FuncTestProfilesSercos::doEnd()
{
  return stopTestEnvironment();
}

HRESULT FuncTestProfilesSercos::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the axisprofile SERCOS");
  log(L"\n----------------------------------------------------------------");

  hResult |= doDataTransmitReceiveTest();
  hResult |= doSingleMethodTest();
  hResult |= doDeviceParam();

  return hResult;
}

HRESULT FuncTestProfilesSercos::doDataTransmitReceiveTest()
{
  HRESULT result = switchSystemStatemachine(StateIDType::BOOTING, StateIDType::RUNNING);
  RETURN_TEST_FAILED(result, L"\nSwitch system to config failed.");

  log(L"\nExecute single cycle to initialize the Status- and Controlword!");
  result = executeSingleCycle();
  RETURN_TEST_FAILED(result, L"\nRun single cycle failed.");

  log(L"\nSet power mode to 'ON' for profile '%ls'", UTF8TOWIDE(m_profileName));
  AxisProfileResult aprResult = m_profile->getStatusControlInterface()->setPowerMode(AxisProfilePowerMode::POWER_ON);
  RETURN_TEST_RESULT(aprResult, L"\nNot able to set power mode ON!", AxisProfileResult::STS_OK);

  log(L"\nSet operation mode to 'Positioning' for profile '%ls'", UTF8TOWIDE(m_profileName));
  aprResult = m_profile->getStatusControlInterface()->setOperationMode(AxisProfileOperationMode::POSITIONING);
  RETURN_TEST_RESULT(aprResult, L"\nNot able to set operation mode positioning!", AxisProfileResult::STS_OK);

  log(L"\n<b>Start test cycle for commanding values.</b>");
  CommandValuesDevice mdtDataTestvalues {};
  ActualValuesDevice atDataTestvalues {};
  constexpr std::int32_t maxTestLoops = 1000;
  float actualPositionValues[maxTestLoops + 1]{};
  float commandedPositionValues[maxTestLoops + 1]{};

  for (std::int32_t testLoop = 0; testLoop <= maxTestLoops; testLoop++)
  {
    // Get actual data
    m_profile->getCyclicDataInterface()->getActualValuesDevice(&atDataTestvalues);
    actualPositionValues[testLoop] = static_cast<float>(atDataTestvalues.m_actualPosition);
    bool correctOperationMode = m_profile->getStatusControlInterface()->checkOperationMode(AxisProfileOperationMode::POSITIONING);
    bool correctPowerMode = m_profile->getStatusControlInterface()->checkPowerMode(AxisProfilePowerMode::POWER_ON);
    if (testLoop > 1)
    {
      if ((false == (( commandedPositionValues[testLoop - 2] - actualPositionValues[testLoop]) < 1E-6)) && (correctPowerMode) && (correctOperationMode))
      {
        error(L"\n| %f | %f | <DARK_RED>NOK</DARK_RED> |", actualPositionValues[testLoop], commandedPositionValues[testLoop]);
        return E_FAIL;
      }
    }

    // Write command data => expected value is m so 
    mdtDataTestvalues.m_commandPosition = static_cast<float>(testLoop)/1000.0;
    m_profile->getCyclicDataInterface()->setCommandValuesDevice(mdtDataTestvalues);
    commandedPositionValues[testLoop] = static_cast<float>( mdtDataTestvalues.m_commandPosition);

   // Execute single cycle
    result = executeSingleCycle();
    RETURN_TEST_FAILED(result, L"\nCycle execute failed");
  }

  log(L"\nTest check and set modulo value...");
  auto bootIfT = m_profile->getDeviceParamInterfaceC();
  double correctModValue = 0.3600;
  double InCorrectModValue = 0.360001;
  std::string errTxt {};

  log(L"\n\tCase of correct check modulo value...");
  aprResult = bootIfT->checkAndSetModulo(bootIfT->handle, correctModValue, errTxt);
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  log(L"\n\tReturned ErrTxt: %ls", UTF8TOWIDE(errTxt));

  log(L"\n\tCase of incorrect check modulo value...");
  aprResult = bootIfT->checkAndSetModulo(bootIfT->handle, InCorrectModValue, errTxt);
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::ERR_INCONSISTENCY);
  log(L"\n\tReturned ErrTxt: %ls", UTF8TOWIDE(errTxt));

  log(L"\nSwitch system back to CONFIG!");
  result = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(result, L"\nSwitch CONFIG state failed");

  log(L"\nTest cycle finished!");

#ifdef _MSC_VER
  // plot
  // if you use test helper as local variable on stack, the stack size is to big  => moved to heap
  test::TestHelper* testHelper = new test::TestHelper(this);
  testHelper->plotTime(actualPositionValues, const_cast<wchar_t*>(L"Position-Time-Diagram"), maxTestLoops, 0.1);
  testHelper->plotTime(commandedPositionValues, const_cast<wchar_t*>(L"Velocity-Time-Diagram"), maxTestLoops, 0.1);
  delete testHelper;
  testHelper = nullptr;
#endif

  return result;
}

HRESULT FuncTestProfilesSercos::doSingleMethodTest()
{
  log(L"\n<b>Test 2: Check the correctness of the general functionality of the axisprofile.</b>");

  log(L"\nGet profile instance %ls...", UTF8TOWIDE(m_profileName));
  IAxisProfileInternal* testProfile = static_cast<IAxisProfileInternal*>(m_axisProfileComponent->getAxisProfileManager()->getProfileByName(m_profileName.c_str()));
  RETURN_TEST_RESULT_SET_PTR(testProfile, L"\nProfile is not exist in the system");

  log(L"\nTest set device address ...");
  auto result = testProfile->setDeviceAddress(1);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nTest get device state...");
  result = testProfile->getDeviceState();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::ERR_NOTIMPLEMENTED);

  return S_OK;
}

HRESULT FuncTestProfilesSercos::doDeviceParam()
{
  HRESULT res = S_OK;
  log(L"\n<b>Start test of device parameter access...</b>");

  res = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(res, L"\nSwitch system to config failed.");

  res = switchSystemStatemachine(StateIDType::BOOTING, StateIDType::RUNNING);
  RETURN_TEST_FAILED(res, L"\nSwitch system to running failed.");

  auto paramIf = m_profile->getDeviceParamInterface();

  std::uint16_t expS331 = 1;
  log(L"\nSet S-0-0331: %d...", expS331);
  auto& paramHelper = m_mockEthercatMaster->m_ecatParamRequestNodeSoE.m_fbsHelper;
  paramHelper.writeParam(IDN_S(0, 331), expS331);

  log(L"\nCheck standstill, should true...");
  bool isStandstill{};
  res = waitAndChecker([&] ()->AxisProfileResult{return paramIf->checkStandstill(isStandstill);}, AxisProfileResult::STS_OK, 100, 5000);
  RETURN_TEST_FAILED(res, L"\nCheck standstill failed.");
  // cppcheck-suppress knownConditionTrueFalse ; False positive: the actStep is depend on a pre-condition
  LOG_TEST_RESULT_ANY(isStandstill == true, L"\nUnexpected standstill state.");

  expS331 = 0;
  log(L"\nSet S-0-0331: %d...", expS331);
  paramHelper.writeParam(IDN_S(0, 331), expS331);

  log(L"\nCheck standstill, should false...");
  res = waitAndChecker([&] ()->AxisProfileResult{return paramIf->checkStandstill(isStandstill);}, AxisProfileResult::STS_OK, 100, 5000);
  RETURN_TEST_FAILED(res, L"\nCheck standstill failed.");
  // cppcheck-suppress knownConditionTrueFalse ; False positive: the actStep is depend on a pre-condition
  LOG_TEST_RESULT_ANY(isStandstill == false, L"\nUnexpected standstill state.");

  log(L"\nSwitch system back to CONFIG!");
  res = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(res, L"\nSwitch CONFIG state failed");

  return res;
}
