#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "common/scheduler/i_scheduler.h"
#include "component/statemachine/i_state_machine.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestAxisProfileScheduler
//! Test class for plc open state machine.
class TestAxisProfileScheduler : public xTST_Test2
{
public:
  //! constructor
  TestAxisProfileScheduler() = default;

  //! Destructor
  virtual ~TestAxisProfileScheduler() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Scheduler");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile scheduler.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_DS);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();


private: //data


  //! @struct TestCaseCallable
  //! @brief This struct contains the test data of the different test cases.
  struct TestCaseCallable
  {
    const wchar_t *m_name{};                                        //!< name of testcase
    bool m_addValidAxisProfileManager{};                            //!< indicates if a real axisprofile manager is given in the constructor
    const bool m_oneShotFlag{};                                    //!< one shot flag
    const common::scheduler::SchedState m_initState;                //!< initialized state before execute call
    const common::scheduler::SchedEventType m_eventType;            //!< event type for execute call
    const common::scheduler::SchedEventPhase m_eventPhase;          //!< event phase for execute call
    const comm::axisprofile::StateIDType m_statemachineState;       //!< active statemachine state for testcase
    const comm::axisprofile::AxisProfileResult m_switchResult;      //!< result of axisprofile switchingToState
    const common::scheduler::SchedEventResponse m_expectedResult;   //!< expected result of execute call
    const common::scheduler::SchedState m_expectedState;            //!< expected state after execute call
  };

private: // methods

  //! @brief   This test validates, that the callable factory of the axisprofile works
  //! @return   S_OK, if test was successful 
  HRESULT testDevicprofileCallableFactory();

  //! @brief   This test validates the callables of the axisprofile
  //! @return   S_OK, if test was successful 
  HRESULT testAxisProfileCallable();

  //! @brief   This method runs the given testcase for the axisprofile callable.
  //! @param[in]    type       callable type
  //! @param[in]    testcase   test case to execute
  //! @return   S_OK, if test was successful 
  HRESULT runAxisProfileCallableTestcase(comm::axisprofile::CallableType type, const TestCaseCallable* testcase);

  //! @brief   This method test getFactoryDescription and getCallableConfigurations for scheduler V2.
  //! @return   S_OK, if test was successful 
  HRESULT testCallableCfg();

  //! @brief   This method test return timeout information to scheduler
  //! @return   S_OK, if test was successful 
  HRESULT testTimeoutValueSwitchingOP();

  //! @brief   This method test change cycle time
  //! @return   S_OK, if test was successful 
  HRESULT testChangeCycleTime();

  //! @brief   This method selects event type by SchedState
  //! @param[in]    state   expected scheduler state
  //! @return   scheduler event
  ::common::scheduler::SchedEventType helperEventSelector(::common::scheduler::SchedState state);
};
