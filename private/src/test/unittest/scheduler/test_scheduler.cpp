#include "test_scheduler.h"
#include "axisprofile_internal_defs.h"
#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/external_bundles/mock_system_state.h"
#include "scheduler/axisprofile_callable_factory.h"
#include "scheduler/callable/axisprofile_adaption_callable.h"
#include "scheduler/callable/axisprofile_distribution_callable.h"

#include <string.h>
#include "helper/result_helper.h"
#include "common/scheduler/callable_configurations_generated.h"
#include "common/scheduler/task_generated.h"
#include "comm/datalayer/diagnosis_generated.h"
#include "common/systemhandler/component_generated.h"

using namespace comm::axisprofile;
using namespace common::scheduler;

REGISTER_TEST(TestAxisProfileScheduler);
REGISTER_TEST_TO_GROUP(TestAxisProfileScheduler, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileScheduler, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileScheduler, Scheduler);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestAxisProfileScheduler::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile scheduler functionality");
  log(L"\n----------------------------------------------------------------");

  log(L"\n");
  hResult |= testDevicprofileCallableFactory();
  hResult |= testAxisProfileCallable();
  hResult |= testCallableCfg();
  hResult |= testTimeoutValueSwitchingOP();
  hResult |= testChangeCycleTime();

  return hResult;
}

HRESULT TestAxisProfileScheduler::testDevicprofileCallableFactory()
{
  log(L"\n Test the axis profile callable factory.");
  HRESULT hresult = S_OK;

  test::MockAxisProfileCmp testProfileCmp;
  AxisprofileCallableFactory testfactory(&testProfileCmp);
  comm::datalayer::Variant testVariant;

  log(L"\n  test none string vector parameter ...");
  testVariant.setType(comm::datalayer::VariantType::UNKNOWN);
  std::shared_ptr<common::scheduler::ICallable> invalidCallable1 = testfactory.createCallable(testVariant);
  LOG_TEST_RESULT_ANY(invalidCallable1 == nullptr, L"Should be nullptr callable." );

  log(L"\n  test empty string parameter ...");
  testVariant.setType(comm::datalayer::VariantType::ARRAY_OF_STRING);
  std::vector<std::string> emptyStr({});
  testVariant.setValue(emptyStr);
  std::shared_ptr<common::scheduler::ICallable> invalidCallable2 = testfactory.createCallable(testVariant);
  LOG_TEST_RESULT_ANY(invalidCallable2 == nullptr, L"Should be nullptr callable.");

  log(L"\n  test invalid string parameter ...");
  testVariant.setType(comm::datalayer::VariantType::ARRAY_OF_STRING);
  std::vector<std::string> invalidStr({"invalid parameter"});
  testVariant.setValue(invalidStr);
  std::shared_ptr<common::scheduler::ICallable> invalidCallable3 = testfactory.createCallable(testVariant);
  LOG_TEST_RESULT_ANY(invalidCallable3 == nullptr, L"Should be nullptr callable.");

  log(L"\n  test adaption callable creation...");
  std::vector<std::string> callableAdpStr({"adaption"});
  testVariant.setValue(callableAdpStr);
  std::shared_ptr<common::scheduler::ICallable> testAdpCallable = testfactory.createCallable(testVariant);
  LOG_TEST_RESULT_ANY(testAdpCallable != nullptr, L"Adaption callable is not created.");

  log(L"\n  test adaption callable creation twice...");
  std::shared_ptr<common::scheduler::ICallable> invalidAdpCallable = testfactory.createCallable(testVariant);
  LOG_TEST_RESULT_ANY(invalidAdpCallable == nullptr, L"Should be nullptr callable.");

  log(L"\n  test distribution callable creation...");
  std::vector<std::string> callableDspStr({"distribution"});
  testVariant.setValue(callableDspStr);
  std::shared_ptr<common::scheduler::ICallable> testDspCallable = testfactory.createCallable(testVariant);
  LOG_TEST_RESULT_ANY(testDspCallable != nullptr, L"Distribution callable is not created.");

  log(L"\n  test distribution callable creation twice...");
  std::shared_ptr<common::scheduler::ICallable> invildDspCallable = testfactory.createCallable(testVariant);
  LOG_TEST_RESULT_ANY(invildDspCallable == nullptr, L"Distribution be nullptr callable.");

  log(L"\n  test getCallableArguments ...");
  std::vector<std::string> arguments;
  testfactory.getCallableArguments(arguments);
  if ( arguments.size() != 2 &&
       arguments[0].compare(callableTypeString(CallableType::ADAPTION_CALLABLE)) != 0 &&
       arguments[1].compare(callableTypeString(CallableType::ADAPTION_CALLABLE)) != 0)
  {
    error(L"Arguments of callable are not as expected.");
    hresult = E_FAIL;
  }

  log(L"\n  test destroy unspecified callable ...");
  common::scheduler::SchedStatus schedReturn = testfactory.destroyCallable(invalidCallable1);
  if (common::scheduler::SchedStatus::SCHED_E_INVALID_ARGUMENTS != schedReturn)
  {
    error(L"Deletion of callable in factory failed. Error: 0x%X", schedReturn);
    hresult = E_FAIL;
  }

  log(L"\n  test destroy adaption callable ...");
  schedReturn = testfactory.destroyCallable(testAdpCallable);
  if ( common::scheduler::SchedStatus::SCHED_S_OK != schedReturn )
  {
    error(L"Deletion of adaption callable in factory failed. Error: 0x%X", schedReturn);
    hresult = E_FAIL;
  }

  log(L"\n  test destroy distribution callable ...");
  schedReturn = testfactory.destroyCallable(testDspCallable);
  if (common::scheduler::SchedStatus::SCHED_S_OK != schedReturn)
  {
    error(L"Deletion of distribution callable in factory failed. Error: 0x%X", schedReturn);
    hresult = E_FAIL;
  }

  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}

HRESULT TestAxisProfileScheduler::testAxisProfileCallable()
{
  HRESULT hresult = S_OK;

  const TestCaseCallable adaptionCallableCases[] =
  {
    // m_name                                      m_mgr    oneShot,   m_initState                          m_eventType                                         m_eventPhase                                  m_statemachineState         m_switchResult                m_expectedResult                                   m_expectedState
    { L"tick any case",                            true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_TICK,                   SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"tickEvent, in ServiceState",               true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_TICK,                   SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"tickEvent, invalid cmp",                   false,   false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_TICK,                   SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SERVICE},
    { L"tickEvent, in Running",                    true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_TICK,                   SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_OPERATING},

    { L"switchToOp, begin, invalid cmp",           false,   false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SERVICE},
    { L"switchToOp, begin, from Running",          true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"switchToOp, begin, from Error",            true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::ERROR_STATE,   AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SERVICE},
    { L"switchToOp, execute, at Cfg",              true,    true,      SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS,   SchedState::SCHED_STATE_OPERATING},
    { L"switchToOp, execute, at Cfg, failed",      true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_OPERATING},
    { L"switchToOp, execute, at Cfg, failed",      true,    true,      SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::ERR_NOK,   SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_OPERATING},
    { L"switchToOp, execute, at Booting",          true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::BOOTING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS,   SchedState::SCHED_STATE_OPERATING},
    { L"switchToOp, execute, at Running",          true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_OPERATING},
    { L"switchToOp, execute, at Error",            true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::ERROR_STATE,   AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_OPERATING},
    { L"switchToOp, end",                          true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_END,       StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_OPERATING},
    { L"switchToOp, none",                         true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SERVICE},
                                                                       
    { L"switchToSet, begin, invalid cmp",          false,   false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SERVICE},
    { L"switchToSet, begin, from Cfg",             true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"switchToSet, execute, at Running",         true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS,   SchedState::SCHED_STATE_SETUP},
    { L"switchToSet, execute, at Running, failed", true,    true,      SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::RUNNING,       AxisProfileResult::ERR_NOK,   SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SETUP},
    { L"switchToSet, execute, at Cfg",             true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SETUP},
    { L"switchToSet, execute, at Error",           true,    true,      SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::ERROR_STATE,   AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SETUP},
    { L"switchToSet, end",                         true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_END,       StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SETUP},
    { L"switchToSet, none",                        true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SETUP},
                                                                       
    { L"switchToService, execute, at Cfg",         true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_SERVICE,      SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"switchToService, end",                     true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_SERVICE,      SchedEventPhase::SCHED_EVENT_PHASE_END,       StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
                                                                       
    { L"switchToExit, begin, invalid cmp",         false,   false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SERVICE},
    { L"switchToExit, begin, from Cfg",            true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"switchToExit, execute, at Run",            true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_IN_PROCESS,   SchedState::SCHED_STATE_EXIT},
    { L"switchToExit, execute, at Run err",        true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::RUNNING,       AxisProfileResult::ERR_NOK,   SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_EXIT},
    { L"switchToExit, execute, at Cfg",            true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_EXIT},
    { L"switchToExit, end",                        true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_END,       StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SETUP},
    { L"switchToExit, none",                       true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SETUP},
                                                                       
    { L"changeProperty, begin, invalid cmp",       false,   false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE, SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SERVICE},
    { L"changeProperty, begin, from Run",          true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE, SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_OPERATING},
    { L"changeProperty, begin, from Setup Err",    true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE, SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SETUP},
    { L"changeProperty, begin, from Setup",        true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE, SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SETUP},
    { L"changeProperty, execute, from Setup",      true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE, SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SETUP},
    { L"changeProperty, end",                      true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE, SchedEventPhase::SCHED_EVENT_PHASE_END,       StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SETUP},
    { L"changeProperty, none",                     true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE, SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SETUP},
   };

  const TestCaseCallable distributionCallableCases[] =
  {
    // m_name                                      m_mgr    oneShot,   m_initState                          m_eventType                                         m_eventPhase                                  m_statemachineState         m_switchResult                m_expectedResult                                   m_expectedState
    { L"tickEvent, in ServiceState",               true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_TICK,                   SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"tickEvent, invalid cmp",                   false,   false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_TICK,                   SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_ERROR,        SchedState::SCHED_STATE_SERVICE},
    { L"tickEvent, in Running",                    true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_TICK,                   SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_OPERATING},
                                                                      
    { L"switchToOp, any",                          true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_OPERATING},
    { L"switchToSetup, any",                       true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SETUP},
    { L"switchToService, any",                     true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_SERVICE,      SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"switchToExit, any",                        true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,   StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_EXIT},
    { L"changeProperty, any",                      true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE, SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::CONFIG,        AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
                                                                      
    { L"Call get current state",                   true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_GET_CURRENT_STATE,      SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SETUP}
   };

  const TestCaseCallable baseCallableCases[] =
  {
    // m_name                                      m_mgr    oneShot,   m_initState                          m_eventType                                         m_eventPhase                                  m_statemachineState         m_switchResult                m_expectedResult                                   m_expectedState
    { L"unknown event type ignore ",               false,   false,     SchedState::SCHED_STATE_SERVICE,     static_cast<SchedEventType>(-1),                    SchedEventPhase::SCHED_EVENT_PHASE_NONE,      StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"switchToOp, begin",                        true,    false,     SchedState::SCHED_STATE_SERVICE,     SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,     StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SERVICE},
    { L"switchToOp, end",                          true,    false,     SchedState::SCHED_STATE_OPERATING,   SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,    SchedEventPhase::SCHED_EVENT_PHASE_END,       StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_OPERATING},
    { L"switchToSet, end",                         true,    false,     SchedState::SCHED_STATE_SETUP,       SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP,        SchedEventPhase::SCHED_EVENT_PHASE_END,       StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_SETUP},
    { L"switchToExit, end",                        true,    false,     SchedState::SCHED_STATE_EXIT,        SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT,         SchedEventPhase::SCHED_EVENT_PHASE_END,       StateIDType::RUNNING,       AxisProfileResult::STS_OK,    SchedEventResponse::SCHED_EVENT_RESP_OKAY,         SchedState::SCHED_STATE_EXIT}
   };


  log(L"\n<b> Test the axis profile adaption callable.");
  for (const auto &elem : adaptionCallableCases )
  {
    hresult |= runAxisProfileCallableTestcase(comm::axisprofile::CallableType::ADAPTION_CALLABLE, &elem);
  }

  log(L"\n<b> Test the axis profile distribution callable.");
  for (const auto& elem : distributionCallableCases)
  {
    hresult |= runAxisProfileCallableTestcase(comm::axisprofile::CallableType::DISTRIBUTION_CALLABLE, &elem);
  }

  log(L"\n<b> Test the axis profile base callable.");
  for (const auto& elem : baseCallableCases)
  {
    hresult |= runAxisProfileCallableTestcase(comm::axisprofile::CallableType::DISTRIBUTION_CALLABLE, &elem);
  }

  return hresult;
}

HRESULT TestAxisProfileScheduler::runAxisProfileCallableTestcase(comm::axisprofile::CallableType type, const TestCaseCallable* testcase)
{
  log(L"\n  Run testcase: %ls", testcase->m_name);
  HRESULT hresult = S_OK;
  IAxisProfileComponent* usedAxisProfileCmp = nullptr;
  test::MockAxisProfileCmp validAxisProfileCmp;
  test::MockSystemState mockSystemState;
  comm::datalayer::Variant data;
  flatbuffers::FlatBufferBuilder builder;
  auto componentState = ::common::systemhandler::fbs::CreateState(builder);
  auto metadata = comm::datalayer::CreateMetadata(builder,
                                                  comm::datalayer::NodeClass_Variable,
                                                  comm::datalayer::CreateAllowedOperations(builder),
                                                  builder.CreateString("test component description"),
                                                  builder.CreateString("test component description url"),
                                                  builder.CreateString("test display name"));

  builder.Finish(::common::systemhandler::fbs::CreateComponentDirect(builder, "test component name", componentState, metadata));
  data.copyFlatbuffers(builder);

  validAxisProfileCmp.setSystemStateCmp(mockSystemState.registerComponent2(data));

  if ( testcase->m_addValidAxisProfileManager )
  {
    usedAxisProfileCmp = &validAxisProfileCmp;
  }

  std::shared_ptr<comm::axisprofile::AxisprofileCallable> testCallable;
  if (type == comm::axisprofile::CallableType::ADAPTION_CALLABLE)
  {
    testCallable = std::make_shared<AxisprofileAdaptionCallable>(usedAxisProfileCmp);
  }
  else
  {
    testCallable = std::make_shared<AxisprofileDistributionCallable>(usedAxisProfileCmp);
  }
  
  if (usedAxisProfileCmp)
  {
    testCallable.get()->m_state = testcase->m_initState;

    testCallable.get()->m_bootingOneShotFlag = testcase->m_oneShotFlag;
    testCallable.get()->m_configOneShotFlag  = testcase->m_oneShotFlag;

    auto mockStateMachine = static_cast<test::MockStateMachine*>(validAxisProfileCmp.getStateMachine());
    mockStateMachine->m_stateID = testcase->m_statemachineState;

    validAxisProfileCmp.setReturnValue(testcase->m_switchResult);
  }

  comm::datalayer::Variant testVariant;
  common::scheduler::SchedEventResponse schedResult = testCallable->execute(testcase->m_eventType, testcase->m_eventPhase, testVariant);
  if ( schedResult != testcase->m_expectedResult )
  {
    error(L"Execute did not return expected status. Expected: 0x%X, Actual: 0x%X", testcase->m_expectedResult, schedResult);
    hresult = E_FAIL;
  }

  // log reported diagnosis number and string
  if (schedResult == common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_ERROR)
  {
    auto dataType = testVariant.getType();
    if (dataType == ::comm::datalayer::VariantType::FLATBUFFERS)
    {
      flatbuffers::Verifier verifier(testVariant.getData(), testVariant.getSize());
      LOG_TEST_FAILED(::comm::datalayer::VerifyDiagnosisBuffer(verifier), L"Verify fbs failed on return variable!");

      auto variantData = ::comm::datalayer::GetDiagnosis(testVariant.getData());
      LOG_TEST_FAILED(flatbuffers::IsFieldPresent(variantData, comm::datalayer::Diagnosis::VT_MAINDIAGNOSISCODE), L"Maindiagnosis number missing");
      LOG_TEST_FAILED(flatbuffers::IsFieldPresent(variantData, comm::datalayer::Diagnosis::VT_DETAILEDDIAGNOSISCODE), L"Detialdiagnosis number missing");
      LOG_TEST_FAILED(flatbuffers::IsFieldPresent(variantData, comm::datalayer::Diagnosis::VT_DYNAMICDESCRIPTION), L"DynamicDescription missing");

      log(L"\n   maindiangnosis = %d", variantData->mainDiagnosisCode());
      log(L"\n   detialdiangnosis = %d", variantData->detailedDiagnosisCode());
      log(L"\n   dynamicDescription = %ls", UTF8TOWIDE(flatbuffers::GetString(variantData->dynamicDescription())));
    }
  }

  common::scheduler::SchedState schedState = testCallable->getState();
  if ( schedState != testcase->m_expectedState )
  {
    error(L"Execute did not return expected state. Expected: 0x%X, Actual: 0x%X", testcase->m_expectedState, schedState);
    hresult = E_FAIL;
  }

  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}

HRESULT TestAxisProfileScheduler::testCallableCfg()
{
  test::MockAxisProfileCmp testProfileCmp;
  AxisprofileCallableFactory testfactory(&testProfileCmp);

  log(L"\n<b> Test callable factory description...</b>");
  std::vector<std::string> tmpStr {};
  std::vector<std::string> expStr {{"@note This is an automatically generated callable. Please do not modify. Required exactly twice in a control system."}};
  testfactory.getFactoryDescription(tmpStr);
  LOG_TEST_RESULT_STRING(tmpStr.back(), L"Unexpected factory description!", expStr.back());

  log(L"\n<b> Test callable configuration...</b>");
  common::scheduler::fbs::CallableWdgConfig expAdpWdgConfig {::common::scheduler::fbs::CallableWdgConfig_WDG_HW_TASK};

  common::scheduler::fbs::CallableWdgConfig expDspWdgConfig {::common::scheduler::fbs::CallableWdgConfig_WDG_HW_TASK};

  comm::datalayer::Variant tmpCfg;
  testfactory.getCallableConfigurations(tmpCfg);

  LOG_TEST_FAILED((tmpCfg.getType() != comm::datalayer::VariantType::FLATBUFFERS), L"Callable is not flatbuffer!");

  flatbuffers::Verifier verifier(tmpCfg.getData(), tmpCfg.getSize());
  LOG_TEST_FAILED(::common::scheduler::fbs::VerifyCallableConfigurationsBuffer(verifier), L"Verify fbs failed on callable configuration!");

  auto callableCfgData = ::common::scheduler::fbs::GetCallableConfigurations(tmpCfg.getData());
  LOG_TEST_FAILED(flatbuffers::IsFieldPresent(callableCfgData, ::common::scheduler::fbs::CallableConfigurations::VT_CONFIGURATIONS), L"VT_CONFIGURATIONS is not present!");
  LOG_TEST_FAILED(callableCfgData->configurations()->size() != 2, L"Unexpected configuration table size!");

  // check adaption callable configuration
  {
    auto adpCallableCfg = callableCfgData->configurations()->begin();

    log(L"\n  check alias...");
    std::string expAdpAlias {"adaption"};
    LOG_TEST_RESULT_STRING(flatbuffers::GetString(adpCallableCfg->alias()), L"Unexpected alias!", expAdpAlias);

    log(L"\n  check syncPoints...");
    auto syncPoint = adpCallableCfg->sync();
    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(syncPoint, ::common::scheduler::fbs::SyncPoints::VT_BEFORE), L"VT_BEFORE is not present in syncPoint!");
    std::vector<std::string> expAdpBeforeList {"axisprofile-adaption-exit"};
    LOG_TEST_FAILED(syncPoint->before()->size() != expAdpBeforeList.size(), L"Unexpected syncPoint.before size!");
    for ( flatbuffers::uoffset_t i = 0; i < expAdpBeforeList.size(); i++)
    {
      LOG_TEST_RESULT_STRING(flatbuffers::GetString(syncPoint->before()->Get(i)), L"Unexpected syncPoint.before!", expAdpBeforeList.at(i));
    }

    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(syncPoint, ::common::scheduler::fbs::SyncPoints::VT_AFTER), L"VT_AFTER is not present in syncPoint!");
    LOG_TEST_FAILED(syncPoint->after()->size() != expAdpBeforeList.size(), L"Unexpected syncPoint.after size!");
    std::vector<std::string> expAdpAfterList {"axisprofile-adaption-entry", "fieldbuses-exit"};
    for ( flatbuffers::uoffset_t i = 0; i < expAdpBeforeList.size(); i++)
    {
      LOG_TEST_RESULT_STRING(flatbuffers::GetString(syncPoint->after()->Get(i)), L"Unexpected syncPoint.after!", expAdpAfterList.at(i));
    }

    log(L"\n  check arguments...");
    std::vector<std::string> expAdpArgument {"adaption"};
    auto arguments = adpCallableCfg->arguments();
    LOG_TEST_FAILED(arguments->size() != expAdpArgument.size(), L"Unexpected arguments size!");
    for ( flatbuffers::uoffset_t i = 0; i < expAdpArgument.size(); i++)
    {
      LOG_TEST_RESULT_STRING(flatbuffers::GetString(arguments->Get(i)), L"Unexpected arguments!", expAdpArgument.at(i));
    }

    log(L"\n  check watch dog type...");
    auto wdgConfig = adpCallableCfg->watchdog();
    LOG_TEST_RESULT(wdgConfig, L"Unexpected watch dog type!", expAdpWdgConfig);

    log(L"\n  check task space...");
    auto taskSpace = adpCallableCfg->task();
    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(taskSpace, ::common::scheduler::fbs::TaskSpecs::VT_NAME), L"VT_NAME is not present in taskSpace!");
    std::string expAdpName {"ctrlXAutomation"};
    LOG_TEST_RESULT_STRING(flatbuffers::GetString(taskSpace->name()), L"Unexpected name in taskSpace!", expAdpName);

    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(taskSpace, ::common::scheduler::fbs::TaskSpecs::VT_PRIORITY), L"VT_PRIORITY is not present in taskSpace!");
    std::string expAdpPriority {"high"};
    LOG_TEST_RESULT_STRING(flatbuffers::GetString(taskSpace->priority()), L"Unexpected priority in taskSpace!", expAdpPriority);

    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(taskSpace, ::common::scheduler::fbs::TaskSpecs::VT_TYPE), L"VT_TYPE is not present in taskSpace!");
    std::string expAdpType {"cyclic/ms/2"};
    LOG_TEST_RESULT_STRING(flatbuffers::GetString(taskSpace->type()), L"Unexpected type in taskSpace!", expAdpType);
  }

  // check distribution callable configuration
  {
    auto dspCallableCfg = callableCfgData->configurations()->Get(1);

    log(L"\n  check alias...");
    std::string expDspAlias {"distribution"};
    LOG_TEST_RESULT_STRING(flatbuffers::GetString(dspCallableCfg->alias()), L"Unexpected alias!", expDspAlias);

    log(L"\n  check syncPoints...");
    auto syncPoint = dspCallableCfg->sync();
    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(syncPoint, ::common::scheduler::fbs::SyncPoints::VT_BEFORE), L"VT_BEFORE is not present in syncPoint!");
    std::vector<std::string> expDspBeforeList {"axisprofile-distribution-exit"};
    LOG_TEST_FAILED(syncPoint->before()->size() != expDspBeforeList.size(), L"Unexpected syncPoint.before size!");
    for ( flatbuffers::uoffset_t i = 0; i < expDspBeforeList.size(); i++)
    {
      LOG_TEST_RESULT_STRING(flatbuffers::GetString(syncPoint->before()->Get(i)), L"Unexpected syncPoint.before!", expDspBeforeList.at(i));
    }

    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(syncPoint, ::common::scheduler::fbs::SyncPoints::VT_AFTER), L"VT_AFTER is not present in syncPoint!");
    LOG_TEST_FAILED(syncPoint->after()->size() != expDspBeforeList.size(), L"Unexpected syncPoint.after size!");
    std::vector<std::string> expDspAfterList {"axisprofile-distribution-entry", "motion-exit"};
    for ( flatbuffers::uoffset_t i = 0; i < expDspBeforeList.size(); i++)
    {
      LOG_TEST_RESULT_STRING(flatbuffers::GetString(syncPoint->after()->Get(i)), L"Unexpected syncPoint.after!", expDspAfterList.at(i));
    }

    log(L"\n  check arguments...");
    std::vector<std::string> expDspArgument {"distribution"};    
    auto arguments = dspCallableCfg->arguments();
    LOG_TEST_FAILED(arguments->size() != expDspArgument.size(), L"Unexpected arguments size!");
    for ( flatbuffers::uoffset_t i = 0; i < expDspArgument.size(); i++)
    {
      LOG_TEST_RESULT_STRING(flatbuffers::GetString(arguments->Get(i)), L"Unexpected arguments!", expDspArgument.at(i));
    }

    log(L"\n  check watch dog type...");
    auto wdgConfig = dspCallableCfg->watchdog();
    LOG_TEST_RESULT(wdgConfig, L"Unexpected watch dog type!", expDspWdgConfig);

    log(L"\n  check task space...");
    auto taskSpace = dspCallableCfg->task();
    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(taskSpace, ::common::scheduler::fbs::TaskSpecs::VT_NAME), L"VT_NAME is not present in taskSpace!");
    std::string expDspName {"ctrlXAutomation"};
    LOG_TEST_RESULT_STRING(flatbuffers::GetString(taskSpace->name()), L"Unexpected name in taskSpace!", expDspName);

    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(taskSpace, ::common::scheduler::fbs::TaskSpecs::VT_PRIORITY), L"VT_PRIORITY is not present in taskSpace!");
    std::string expDspPriority {"high"};
    LOG_TEST_RESULT_STRING(flatbuffers::GetString(taskSpace->priority()), L"Unexpected priority in taskSpace!", expDspPriority);

    LOG_TEST_FAILED(flatbuffers::IsFieldPresent(taskSpace, ::common::scheduler::fbs::TaskSpecs::VT_TYPE), L"VT_TYPE is not present in taskSpace!");
    std::string expDspType {"cyclic/ms/2"};
    LOG_TEST_RESULT_STRING(flatbuffers::GetString(taskSpace->type()), L"Unexpected type in taskSpace!", expDspType);
  }

  return S_OK;
}

::common::scheduler::SchedEventType TestAxisProfileScheduler::helperEventSelector(::common::scheduler::SchedState state)
{
  ::common::scheduler::SchedEventType expEventType = static_cast<::common::scheduler::SchedEventType>(-1);
  switch (state)
  {
    case common::scheduler::SchedState::SCHED_STATE_EXIT:
      expEventType = ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_EXIT;
      break;
    case common::scheduler::SchedState::SCHED_STATE_OPERATING:
      expEventType = ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING;
      break;
    case common::scheduler::SchedState::SCHED_STATE_SETUP:
      expEventType = ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SETUP;
      break;
    case common::scheduler::SchedState::SCHED_STATE_SERVICE:
      expEventType = ::common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_SERVICE;
      break;
    default:
      break;
  }

  return expEventType;
}

HRESULT TestAxisProfileScheduler::testTimeoutValueSwitchingOP()
{
  log(L"\n<b> Test the adaption callable returns Timeout value when switching to OP.");
  HRESULT hresult = S_OK;
  
  test::MockAxisProfileCmp axisProfileCmp;
  std::shared_ptr<comm::axisprofile::AxisprofileCallable> testCallable = std::make_shared<AxisprofileAdaptionCallable>(&axisProfileCmp);
  testCallable.get()->m_state = SchedState::SCHED_STATE_SETUP;
  auto mockStateMachine = static_cast<test::MockStateMachine*>(axisProfileCmp.getStateMachine());
  mockStateMachine->m_stateID = comm::axisprofile::StateIDType::CONFIG;

  std::uint32_t expTimeout = 200; // unit in seconds
  test::MockBootManager* bootMgr = static_cast<test::MockBootManager*>(axisProfileCmp.getBootManager());
  bootMgr->setMaxBootingTimeout(static_cast<double>(expTimeout));

  log(L"\n  scheduler try to switch to OP...");
  comm::datalayer::Variant testVariant;
  common::scheduler::SchedEventResponse schedResult = testCallable->execute(common::scheduler::SchedEventType::SCHED_EVENT_SWITCH_TO_OPERATING,
                                                                            common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_BEGIN,
                                                                            testVariant);
  RETURN_TEST_RESULT_S(schedResult, L"scheduler execute failed!", ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY);

  log(L"\n  check timeout return value...");
  RETURN_TEST_FAILED(testVariant.getType() == comm::datalayer::VariantType::ARRAY_OF_UINT32, L"return variant is not std::uint32_t");
  std::uint32_t timeout = *testVariant.getData();
  RETURN_TEST_RESULT(timeout, L"unexpected timeout value!", expTimeout+1);

  return hresult;
}

HRESULT TestAxisProfileScheduler::testChangeCycleTime()
{
  log(L"\n<b> Test the adaption callable change cycle time.");
  HRESULT hresult = S_OK;

  test::MockAxisProfileCmp axisProfileCmp;
  std::shared_ptr<comm::axisprofile::AxisprofileCallable> testCallable = std::make_shared<AxisprofileAdaptionCallable>(&axisProfileCmp);
  testCallable.get()->m_state = SchedState::SCHED_STATE_SETUP;
  auto mockStateMachine = static_cast<test::MockStateMachine*>(axisProfileCmp.getStateMachine());
  mockStateMachine->m_stateID = comm::axisprofile::StateIDType::CONFIG;

  log(L"\n  try to change cycle time...");
  std::uint32_t newCycleTime = 123654;
  double expNewCycleTime = MICROSECOND_2_SECOND(newCycleTime);
  comm::datalayer::Variant testVariant;
  flatbuffers::FlatBufferBuilder builder;
  auto nameFB = builder.CreateString("automation");
  common::scheduler::fbs::TaskBuilder fbBuilder(builder);
  fbBuilder.add_name(nameFB);
  fbBuilder.add_cycletime(newCycleTime);
  auto finalFB = fbBuilder.Finish();
  builder.Finish(finalFB);
  testVariant.copyFlatbuffers(builder);

  common::scheduler::SchedEventResponse schedResult = testCallable->execute(common::scheduler::SchedEventType::SCHED_EVENT_TASK_PROPERTIES_CHANGE,
                                                                            common::scheduler::SchedEventPhase::SCHED_EVENT_PHASE_EXECUTE,
                                                                            testVariant);
  RETURN_TEST_RESULT_S(schedResult, L"scheduler execute failed!", ::common::scheduler::SchedEventResponse::SCHED_EVENT_RESP_OKAY);

  log(L"\n  check new cycle time...");
  LOG_TEST_RESULT_FLT(axisProfileCmp.getCycleTime(), L"unexpected cycle time!", expNewCycleTime, comm::axisprofile::STD_EPS);

  return hresult;
}
