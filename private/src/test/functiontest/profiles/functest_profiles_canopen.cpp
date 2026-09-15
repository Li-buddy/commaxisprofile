#include "functest_profiles_canopen.h"
#include "helper/result_helper.h"
#include "motion/test/test_helper.h"
#include "manager/axisprofile_manager.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"
#include "profiles/canopen/canopen_profile.h"
#include "profiles/value_mapping/mapping_handler.h"
#include <thread>

using namespace comm::axisprofile;

REGISTER_TEST(FuncTestProfilesCanopen);
REGISTER_TEST_TO_GROUP(FuncTestProfilesCanopen, 1_AutoTest);
REGISTER_TEST_TO_GROUP(FuncTestProfilesCanopen, 3_FuncTest);
REGISTER_TEST_TO_GROUP(FuncTestProfilesCanopen, Profiles_Canopen);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT FuncTestProfilesCanopen::doInit()
{
  return startTestEnvironment();
}

HRESULT FuncTestProfilesCanopen::doEnd()
{
  return stopTestEnvironment();
}

HRESULT FuncTestProfilesCanopen::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the axisprofile CiA402");
  log(L"\n----------------------------------------------------------------");

  hResult |= doDataTransmitReceiveTest();
  hResult |= doSingleMethodTest();

  return hResult;
}


HRESULT FuncTestProfilesCanopen::doDataTransmitReceiveTest()
{
  log(L"\n\n<b>Test 1: Check the communication to and from the datalayer.</b>");

  HRESULT result = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(result, L"\nSwitch System to CONFIG failed");

  IAxisProfileInternal* testProfile{};
  result = addProfileToSystem(AxisProfileType::CANOPEN_OVER_ETHERCAT, 1, m_profileName.c_str(), &testProfile);
  RETURN_TEST_FAILED(result, L"\nAdd profile to system failed");
  RETURN_TEST_RESULT_SET_PTR(testProfile, L"\nEmpty of axisprofile instance");

  log(L"\nConfigure new axisprofile.");
  m_axisProfileComponent->getAxisProfileManager()->setProfileInputBuffer(m_profileName.c_str(), MOCK_ECAT_INPUTBUFFER_STRING);
  m_axisProfileComponent->getAxisProfileManager()->setProfileOutputBuffer(m_profileName.c_str(), MOCK_ECAT_OUTPUTBUFFER_STRING);

  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::STATUSWORD, MOCK_ECAT_INBUFFER_STATUSWORD);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::POSITION, MOCK_ECAT_INBUFFER_POSITION);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::DIAGMSG, MOCK_ECAT_INBUFFER_DIAGMSG);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::VELOCITY, MOCK_ECAT_INBUFFER_VELOCITY);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SWITCH_REF, MOCK_ECAT_INBUFFER_SWITCH_REF);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SWITCH_POS_LIM, MOCK_ECAT_INBUFFER_SWITCH_POS_LIM);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SWITCH_NEG_LIM, MOCK_ECAT_INBUFFER_SWITCH_NEG_LIM);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::OP_MODE_DISPLAY, MOCK_ECAT_INBUFFER_OP_MODE_DISPLAY);

  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_POSEDG, MOCK_ECAT_INBUFFER_PROBE1_POS_EDG);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_NEGEDG, MOCK_ECAT_INBUFFER_PROBE1_NEG_EDG);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_POSLAT, MOCK_ECAT_INBUFFER_PROBE1_POS_LAT);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE1_NEGLAT, MOCK_ECAT_INBUFFER_PROBE1_NEG_LAT);

  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_POSEDG, MOCK_ECAT_INBUFFER_PROBE2_POS_EDG);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_NEGEDG, MOCK_ECAT_INBUFFER_PROBE2_NEG_EDG);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_POSLAT, MOCK_ECAT_INBUFFER_PROBE2_POS_LAT);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::PROBE2_NEGLAT, MOCK_ECAT_INBUFFER_PROBE2_NEG_LAT);

  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SMO_SAFETY_SELECTION, MOCK_ECAT_INBUFFER_SMO_SAFETY_SELECTION);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SMO_SAFETY_STATUS, MOCK_ECAT_INBUFFER_SMO_SAFETY_STATUS);

  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::TORQUE_FORCE, MOCK_ECAT_INBUFFER_TORQUE_FORCE);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::TORQUE_LIMIT_STATUS, MOCK_ECAT_INBUFFER_TORQUE_FORCE_LIMIT_STATUS);

  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_1, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_1);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_2, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_2);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_3, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_3);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_4, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_4);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_5, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_5);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::USER_PDO_INPUT_6, MOCK_ECAT_INBUFFER_USER_PDO_INPUT_6);
  testProfile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::SIGNAL_STATUS_WORD, MOCK_ECAT_INBUFFER_SIGNAL_STATUS_WORD);

  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::CONTROLWORD, MOCK_ECAT_OUTBUFFER_CMDWORD);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::POSITION, MOCK_ECAT_OUTBUFFER_POSITION);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::VELOCITY, MOCK_ECAT_OUTBUFFER_VELOCITY);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::OP_MODE, MOCK_ECAT_INBUFFER_OP_MODE);

  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_1, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_1);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_2, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_2);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_3, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_3);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_4, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_4);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_5, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_5);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::USER_PDO_OUTPUT_6, MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_6);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::TORQUE_FORCE, MOCK_ECAT_OUTBUFFER_TORQUE_FORCE);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH, MOCK_ECAT_OUTBUFFER_TORQUE_FORCE_LIMIT_HIGH);
  testProfile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::TORQUE_VEL_LIMIT_LOW, MOCK_ECAT_OUTBUFFER_TORQUE_FORCE_LIMIT_LOW);
  result = switchSystemStatemachine(StateIDType::BOOTING, StateIDType::RUNNING);
  RETURN_TEST_FAILED(result, L"\nSwitch System to RUNNING failed");

  log(L"\nExecute single cycle to initialize the Status- and Controlword!");
  m_mockEthercatMaster->setStatusWord(0x0023); // startup state is "Switched on"
  result = executeSingleCycle();
  RETURN_TEST_FAILED(result, L"\nExecute single cycle failed");
  log(L"\nSet power mode to 'ON' for profile '%ls'", UTF8TOWIDE(m_profileName));
  AxisProfileResult aprResult;
  do {
    aprResult = testProfile->getStatusControlInterface()->setPowerMode(AxisProfilePowerMode::POWER_ON);
    (void)executeSingleCycle();
    m_mockEthercatMaster->runCoEStatusControl();
    if (aprStatusFailed(aprResult))
    {
      break;
    }
  } while (AxisProfileResult::STS_PENDING == aprResult);
  RETURN_TEST_RESULT(aprResult, L"\nNot able to set power mode ON!", AxisProfileResult::STS_OK);

  log(L"\nSet operation mode to 'Positioning' for profile '%ls'", UTF8TOWIDE(m_profileName));
  do
  {
    aprResult = testProfile->getStatusControlInterface()->setOperationMode(AxisProfileOperationMode::POSITIONING);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if (aprStatusFailed(aprResult))
    {
      break;
    }
  } while (AxisProfileResult::STS_PENDING == aprResult);
  RETURN_TEST_RESULT(aprResult, L"\nNot able to set to position mode!", AxisProfileResult::STS_OK);

  log(L"\n<b>Start test cycle for commanding values.</b>");
  CommandValuesDevice mdtDataTestvalues {};
  mdtDataTestvalues.m_commandPosition = 0;
  ActualValuesDevice atDataTestvalues {};
  atDataTestvalues.m_actualPosition = 0;
  constexpr std::int32_t maxTestLoops = 1000;
  float actualPositionValues[maxTestLoops + 1]{};
  float commandedPositionValues[maxTestLoops + 1]{};

  for (std::int32_t testLoop = 0; testLoop <= maxTestLoops; testLoop++)
  {
    // Get actual data
    testProfile->getCyclicDataInterface()->getActualValuesDevice(&atDataTestvalues);
    actualPositionValues[testLoop] = static_cast<float>(atDataTestvalues.m_actualPosition);
    bool correctOperationMode = testProfile->getStatusControlInterface()->checkOperationMode(AxisProfileOperationMode::POSITIONING);
    bool correctPowerMode = testProfile->getStatusControlInterface()->checkPowerMode(AxisProfilePowerMode::POWER_ON);
    if (testLoop > 1)
    {
      if ((false == ((commandedPositionValues[testLoop - 2] - actualPositionValues[testLoop]) < 1E-6)) && (correctPowerMode) && (correctOperationMode))
      {
        error(L"\n| %f | %f | <DARK_RED>NOK</DARK_RED> |", actualPositionValues[testLoop], commandedPositionValues[testLoop]);
        return E_FAIL;
      }
    }

    // Write command data => expected value is m so 
    mdtDataTestvalues.m_commandPosition = static_cast<float>(testLoop) / 1000.0;
    testProfile->getCyclicDataInterface()->setCommandValuesDevice(mdtDataTestvalues);
    commandedPositionValues[testLoop] = static_cast<float>(mdtDataTestvalues.m_commandPosition);

    // Execute single cycle
    result = executeSingleCycle();
    RETURN_TEST_FAILED(result, L"\nCycle execute failed");
  }

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

HRESULT FuncTestProfilesCanopen::doSingleMethodTest()
{
  HRESULT result = S_OK;

  log(L"\n\n<b>Test 2: Check the correctness of the general functionality of the axisprofile.</b>");

  canopen::CanopenProfile* testProfileCanopen = new canopen::CanopenProfile(m_profileName, m_testNetworkTechnology);
  testProfileCanopen->setDeviceAddress(1);

  log(L"\n\t Check getDeviceState:");
  if (AxisProfileResult::ERR_NOTIMPLEMENTED != testProfileCanopen->getDeviceState())
  {
    error(L"The method seems to be implemented, change test implementation!");
  }
  else
  {
    log(L" OK");
  }

  delete testProfileCanopen;
  testProfileCanopen = nullptr;

  return result;
}
