
#include "functest_canopen_cmd.h"
#include "helper/result_helper.h"
#include "manager/axisprofile_manager.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_defs.h"
#include "profiles/i_axisprofile_cmd_internal.h"
#include "helper/device_id_helper.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(FuncTestCanopenCmd);
REGISTER_TEST_TO_GROUP(FuncTestCanopenCmd, 1_AutoTest);
REGISTER_TEST_TO_GROUP(FuncTestCanopenCmd, 3_FuncTest);
REGISTER_TEST_TO_GROUP(FuncTestCanopenCmd, Profiles_Canopen);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT FuncTestCanopenCmd::doInit()
{
  HRESULT res = S_OK;
  res = startTestEnvironment();
  RETURN_TEST_FAILED(res, L"\nStart axisprofile system failed.");

  log(L"\nTest commands of axisprofile...");

  res = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(res, L"\nSwitch system to config failed.");

  res = addProfileToSystem(AxisProfileType::CANOPEN_OVER_ETHERCAT, 1001, m_name.c_str(), &m_profile);
  RETURN_TEST_FAILED(res, L"\nCreate test axisprofile failed.");
  RETURN_TEST_RESULT_SET_PTR(m_profile, L"\nCreated axisprofile is empty.");

  log(L"\nConfigure new axisprofile...");
  m_axisProfileComponent->getAxisProfileManager()->setProfileInputBuffer(m_name.c_str(), MOCK_ECAT_INPUTBUFFER_STRING);
  m_axisProfileComponent->getAxisProfileManager()->setProfileOutputBuffer(m_name.c_str(), MOCK_ECAT_OUTPUTBUFFER_STRING);

  m_profile->getPropertyInterface()->setEtherCATName(MOCK_ECAT_MASTER_INSTANCE_NAME);
  m_profile->getPropertyInterface()->setDeviceName(MOCK_ECAT_SLAVE_DEVICE_NAME);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::STATUSWORD, MOCK_ECAT_INBUFFER_STATUSWORD);
  m_profile->getMappingHandler()->configureInputValue(comm::axisprofile::InputMappingValueID::POSITION, MOCK_ECAT_INBUFFER_POSITION);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::CONTROLWORD, MOCK_ECAT_OUTBUFFER_CMDWORD);
  m_profile->getMappingHandler()->configureOutputValue(comm::axisprofile::OutputMappingValueID::POSITION, MOCK_ECAT_OUTBUFFER_POSITION);

  m_profile->getPropertyInterface()->setDeviceIdentification(REXROTH_VENDOR_ID, static_cast<std::uint32_t>(CTRLX_COMPACT)<<16);

  if (!isValgrindEnvironment())
  {
    log(L"\nSet background cycle time to 10ms for timing easy handling...");
    m_axisProfileComponent->setCycleTime(0.01);
  }

  return res;
}

HRESULT FuncTestCanopenCmd::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the FuncTestCanopenCmd");
  log(L"\n----------------------------------------------------------------\n");

  res |= testCmdReset();
  res |= testCmdHoming();

  return res;
}


HRESULT FuncTestCanopenCmd::doEnd()
{
  return stopTestEnvironment();
}

HRESULT FuncTestCanopenCmd::testCmdReset()
{
  HRESULT res = S_OK;
  log(L"\n<b>Test resetAxis command...</b>");

  res = switchSystemStatemachine(StateIDType::BOOTING, StateIDType::RUNNING);
  RETURN_TEST_FAILED(res, L"\nSwitch system to running failed.");

  auto cmdIf = m_profile->getCmdInterface();

  log(L"\nState should be Idle at beginning...");
  CmdState cmdState = cmdIf->getCmdState(CmdType::CLEAR_DEV_ERR);
  RETURN_TEST_STATE(CmdState::STATE_IDLE, L"\nIncorrect state.", cmdState, cmdStateToString);

  log(L"\nSimulation of an error...");
  WordStruct tmpStatusWord;
  tmpStatusWord.m_bits.bit3 = 1; // has error
  m_mockEthercatMaster->setStatusWord(tmpStatusWord.m_word);
  res = executeSingleCycle();
  RETURN_TEST_FAILED(res, L"\nInitialize status and control word failed.");

  log(L"\nTriggers start ResetErr command...");
  CmdType cmdType {};
  AxisProfileResult aprRes = cmdIf->clearDeviceError(cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nClear device error failed.", AxisProfileResult::STS_OK);

  log(L"\nState could be Activating after triggered...");
  cmdState = cmdIf->getCmdState(cmdType);
  log(L"\nThe state is %ls...", UTF8TOWIDE(cmdStateToString(cmdState)));
  RETURN_TEST_RESULT_ANY(cmdState == CmdState::STATE_ACTIVATING, L"\nState should be Activating.");

  log(L"\nSimulation the error has been cleared...");
  tmpStatusWord.m_bits.bit3 = 0; // has no error
  m_mockEthercatMaster->setStatusWord(tmpStatusWord.m_word);
  res = executeSingleCycle();
  RETURN_TEST_FAILED(res, L"\nInitialize status and control word failed.");

  log(L"\nWait for a while, state should be reach Active...");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  cmdState = cmdIf->getCmdState(cmdType);
  RETURN_TEST_STATE(CmdState::STATE_ACTIVE, L"\nIncorrect state.", cmdState, cmdStateToString);

  log(L"\nTriggers abort ResetErr command...");
  aprRes = cmdIf->abortCmd(cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nClear device error failed.", AxisProfileResult::STS_OK);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  log(L"\nWait for a while, state should be reach Idle...");
  cmdState = cmdIf->getCmdState(cmdType);
  RETURN_TEST_STATE(CmdState::STATE_IDLE, L"\nIncorrect state.", cmdState, cmdStateToString);

  res = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(res, L"\nSwitch system to configure failed.");

  return res;
}

HRESULT FuncTestCanopenCmd::testCmdHoming()
{
  HRESULT res = S_OK;

  log(L"\n<b>Test referencingAxis command...</b>");

  res = switchSystemStatemachine(StateIDType::BOOTING, StateIDType::RUNNING);
  RETURN_TEST_FAILED(res, L"\nSwitch system to running failed.");

  auto cmdIf = m_profile->getCmdInterface();
  log(L"\nState should be Idle at beginning...");
  CmdState cmdState = cmdIf->getCmdState(CmdType::REFERENCE_POS);
  RETURN_TEST_STATE(CmdState::STATE_IDLE, L"\nIncorrect state.", cmdState, cmdStateToString);

  log(L"\nTriggers start referencingAxis command...");
  CmdType cmdType {};
  AxisProfileResult aprRes = cmdIf->referencingAxis(0.05, AxisProfileEncoder::ENCODER_1, cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nClear device error failed.", AxisProfileResult::STS_OK);

  log(L"\nState should be Activating after triggered...");
  cmdState = cmdIf->getCmdState(cmdType);
  log(L"\nThe state is %ls...", UTF8TOWIDE(cmdStateToString(cmdState)));
  RETURN_TEST_STATE(CmdState::STATE_ACTIVATING, L"\nIncorrect state.", cmdState, cmdStateToString);

  log(L"\nSimulate homing execution success...");
  auto& paramHelper = m_mockEthercatMaster->m_ecatSDORequestNodeCoE.m_fbsHelper;
  paramHelper.writeParam(0x6061,0, std::uint8_t(6));
  WordStruct tmpStatusWord;
  tmpStatusWord.m_bits.bit10 = 1; // target reached
  tmpStatusWord.m_bits.bit12 = 1; // homing active
  tmpStatusWord.m_bits.bit13 = 0; // homing error
  m_mockEthercatMaster->setStatusWord(tmpStatusWord.m_word);
  res = executeSingleCycle();
  RETURN_TEST_FAILED(res, L"\nInitialize status and control word failed.");

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  log(L"\nWait for a while, state should be reach Active...");
  cmdState = cmdIf->getCmdState(cmdType);
  RETURN_TEST_STATE(CmdState::STATE_ACTIVE, L"\nIncorrect state.", cmdState, cmdStateToString);

  log(L"\nTriggers abort referencingAxis command...");
  aprRes = cmdIf->abortCmd(cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nClear device error failed.", AxisProfileResult::STS_OK);

  log(L"\nSimulate homing aborted...");
  paramHelper = m_mockEthercatMaster->m_ecatSDORequestNodeCoE.m_fbsHelper;
  paramHelper.writeParam(0x6061, 0, std::uint8_t(8));
  tmpStatusWord.m_bits.bit12 = 0; // homing NOT active
  m_mockEthercatMaster->setStatusWord(tmpStatusWord.m_word);
  res = executeSingleCycle();
  RETURN_TEST_FAILED(res, L"\nInitialize status and control word failed.");

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  log(L"\nWait for a while, state should be reach Idle...");
  cmdState = cmdIf->getCmdState(cmdType);
  RETURN_TEST_STATE(CmdState::STATE_IDLE, L"\nIncorrect state.", cmdState, cmdStateToString);

  res = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(res, L"\nSwitch system to configure failed.");

  return res;
}
