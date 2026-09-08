
#include "functest_sercos_cmd.h"
#include "helper/result_helper.h"
#include "manager/axisprofile_manager.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_defs.h"
#include "profiles/i_axisprofile_cmd_internal.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(FuncTestSercosCmd);
REGISTER_TEST_TO_GROUP(FuncTestSercosCmd, 1_AutoTest);
REGISTER_TEST_TO_GROUP(FuncTestSercosCmd, 3_FuncTest);
REGISTER_TEST_TO_GROUP(FuncTestSercosCmd, Profiles_Sercos);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

const std::vector<FuncTestSercosCmd::TestCase> FuncTestSercosCmd::m_testCases =
{
  {
    L"clearDeviceError",
    CmdType::CLEAR_DEV_ERR,
    [](FuncTestSercosCmd* test) {return test->setErrorState(true);},
    [](FuncTestSercosCmd* test) {return test->setErrorState(false);},
    [](FuncTestSercosCmd* test) {return test->runClearDevError();}
  },
  {
    L"referencingAxis",
    CmdType::REFERENCE_POS,
    [](FuncTestSercosCmd* test) {return test->prepareReferencingAxis();},
    [](FuncTestSercosCmd* test) {return test->statusChangeSoECmdCompleted();},
    [](FuncTestSercosCmd* test) {return test->runReferencingAxis();}
  },
  {
    L"runSetAbsPos",
    CmdType::SET_ABS_POS,
    [](FuncTestSercosCmd* test) {return test->prepareSetAbsPos();},
    [](FuncTestSercosCmd* test) {return test->statusChangeSoECmdCompleted();},
    [](FuncTestSercosCmd* test) {return test->runSetAbsPos();}
  },
  {
    L"runFixedStop",
    CmdType::FIXED_STOP,
    nullptr,
    [](FuncTestSercosCmd* test) {return test->statusChangeSoECmdCompleted();},
    [](FuncTestSercosCmd* test) {return test->runFixedStop();}
  },
};

HRESULT FuncTestSercosCmd::doInit()
{
  HRESULT res = S_OK;
  log(L"\n<b>Start test of SERCOS commands...</b>");

  res = startTestEnvironment();
  RETURN_TEST_FAILED(res, L"\nStart axisprofile system failed.");

  res = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(res, L"\nSwitch system to config failed.");

  res = addProfileToSystem(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, 1001, m_name.c_str(), &m_profile);
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

  if (!isValgrindEnvironment())
  {
    log(L"\nSet background cycle time to 10ms for timing easy handling...");
    m_axisProfileComponent->setBkThreadCycleTime(0.01);
  }

  return res;
}

HRESULT FuncTestSercosCmd::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the FuncTestSercosCmd");
  log(L"\n----------------------------------------------------------------\n");

  for (const auto& testCase : m_testCases)
  {
    res |= testSoECmd(testCase);
  }

  return res;
}

HRESULT FuncTestSercosCmd::doEnd()
{
  log(L"\n<b>Finished test of SERCOS commands...</b>");
  return stopTestEnvironment();
}

HRESULT FuncTestSercosCmd::testSoECmd(const TestCase& testCase)
{
  HRESULT res = S_OK;

  log(L"\n<b>Test %ls command...</b>", testCase.testName.c_str());

  if (testCase.prepareFunc != nullptr)
  {
    log(L"\nRun prepare function for testing...");
    res = testCase.prepareFunc(this);
    RETURN_TEST_FAILED(res, L"\nRun prepare function failed.");
  }

  res = switchSystemStatemachine(StateIDType::BOOTING, StateIDType::RUNNING);
  RETURN_TEST_FAILED(res, L"\nSwitch system to running failed.");

  log(L"\nCheck initial state of command, should be Idle...");
  auto cmdIf = m_profile->getCmdInterface();
  CmdState cmdState = cmdIf->getCmdState(testCase.cmdType);
  RETURN_TEST_STATE(CmdState::STATE_IDLE, L"\nIncorrect state.", cmdState, cmdStateToString);

  log(L"\nRun command test function of %s...", testCase.testName.c_str());
  res = testCase.testFunc(this);
  RETURN_TEST_FAILED(res, L"\nRun prepare function failed.");

  log(L"\nState should be Activating after triggered...");
  cmdState = cmdIf->getCmdState(testCase.cmdType);
  log(L"\nThe state is %ls...", UTF8TOWIDE(cmdStateToString(cmdState)));
  RETURN_TEST_STATE(CmdState::STATE_ACTIVATING, L"\nIncorrect state.", cmdState, cmdStateToString);

  log(L"\nWait for until state reach Active...");
  res = waitAndStateChecker<CmdState>([&] ()->CmdState{return cmdIf->getCmdState(testCase.cmdType);}, CmdState::STATE_ACTIVE, 200, 5000);
  LOG_TEST_RESULT(res, L"\nIncorrect state.", S_OK);

  cmdState = cmdIf->getCmdState(testCase.cmdType);
  log(L"\nFinal state is %ls...", UTF8TOWIDE(cmdStateToString(cmdState)));

  if (testCase.changeStatusFunc != nullptr)
  {
    log(L"\nRun change status function for testing...");
    res = testCase.changeStatusFunc(this);
    RETURN_TEST_FAILED(res, L"\nRun prepare function failed.");
  }

  log(L"\nWait until device completed the command...");
  res = waitAndStateChecker<CmdStatusInDevice>([&] ()->CmdStatusInDevice{return cmdIf->getCmdStatusInDevice(testCase.cmdType);}, CmdStatusInDevice::DEVICE_COMPLETED, 200, 5000);
  LOG_TEST_RESULT(res, L"\nIncorrect state.", S_OK);

  auto cmdStatusInDevice = cmdIf->getCmdStatusInDevice(testCase.cmdType);
  log(L"\nFinal status in device %s ...", UTF8TOWIDE(cmdStatusInDeviceToString(cmdStatusInDevice)));

  log(L"\nTriggers abort command %ls...", testCase.testName.c_str());
  auto aprRes = cmdIf->abortCmd(testCase.cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nClear device error failed.", AxisProfileResult::STS_OK);

  log(L"\nWait for until state reach Idle...");
  res = waitAndStateChecker<CmdState>([&] ()->CmdState{return cmdIf->getCmdState(testCase.cmdType);}, CmdState::STATE_IDLE, 200, 5000);
  LOG_TEST_RESULT(res, L"\nIncorrect state.", S_OK);

  cmdState = cmdIf->getCmdState(testCase.cmdType);
  log(L"\nFinal state is %ls...", UTF8TOWIDE(cmdStateToString(cmdState)));

  log(L"\nExecute single cycle to initialize the Status and Controlword!");
  m_mockEthercatMaster->setSoEStatuswordCmdChangeBit(false);
  res = executeSingleCycle();
  RETURN_TEST_FAILED(res, L"\nInitialize status and control word failed.");

  res = switchSystemStatemachine(StateIDType::CONFIG, StateIDType::CONFIG);
  RETURN_TEST_FAILED(res, L"\nSwitch system to configure failed.");

  log(L"\n");
  return res;
}

HRESULT FuncTestSercosCmd::runClearDevError()
{
  CmdType cmdType {};

  auto cmdIf = m_profile->getCmdInterface();
  AxisProfileResult aprRes = cmdIf->clearDeviceError(cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nClear device error failed.", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT FuncTestSercosCmd::prepareReferencingAxis()
{
  log(L"\nChange soft limitation...");
  auto& paramHelper = m_mockEthercatMaster->m_ecatParamRequestNodeSoE.m_fbsHelper;
  paramHelper.writeParam(IDN_S(0, 49), std::int32_t(9999999));    // upper limit 0.999999m
  paramHelper.writeParam(IDN_S(0, 50), std::int32_t(-9999999));   // lower limit -0.999999m
  paramHelper.writeParam(IDN_S(0, 277), std::uint16_t(0x00C0));   // absolute encoder support homing
  return S_OK;
}

HRESULT FuncTestSercosCmd::runReferencingAxis()
{
  CmdType cmdType {};

  auto cmdIf = m_profile->getCmdInterface();
  AxisProfileResult aprRes = cmdIf->referencingAxis(0.05, AxisProfileEncoder::ENCODER_1, cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nReference axis failed.", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT FuncTestSercosCmd::prepareSetAbsPos()
{
  log(L"\nChange to absolute measurement...");
  auto& paramHelper = m_mockEthercatMaster->m_ecatParamRequestNodeSoE.m_fbsHelper;
  paramHelper.writeParam(IDN_S(0, 277), std::uint16_t(0x0140));   // absolute encoder support homing
  return S_OK;
}

HRESULT FuncTestSercosCmd::runSetAbsPos()
{
  CmdType cmdType {};

  auto cmdIf = m_profile->getCmdInterface();
  AxisProfileResult aprRes = cmdIf->setAbsolutePosition(0.05, AxisProfileEncoder::ENCODER_1, cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nSet absolute position failed.", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT FuncTestSercosCmd::runFixedStop()
{
  CmdType cmdType {};

  auto cmdIf = m_profile->getCmdInterface();
  AxisProfileResult aprRes = cmdIf->fixedStop(20, cmdType);
  RETURN_TEST_RESULT(aprRes, L"\nFixed stop failed.", AxisProfileResult::STS_OK);
  return S_OK;
}

HRESULT FuncTestSercosCmd::statusChangeSoECmdCompleted()
{
  log(L"\nExecute single cycle to initialize the Status and Controlword!");
  m_mockEthercatMaster->setSoEStatuswordCmdChangeBit(true);
  auto res = executeSingleCycle();
  RETURN_TEST_FAILED(res, L"\nInitialize status and control word failed.");
  return S_OK;
}

HRESULT FuncTestSercosCmd::setErrorState(bool hasError)
{
  log(L"\nSet axis error state %Ls", hasError ? L"Error" : L"NoError");
  m_mockEthercatMaster->setSoEStatuswordErrorBit(hasError);
  return S_OK;
}
