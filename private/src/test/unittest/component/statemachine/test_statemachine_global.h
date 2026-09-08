#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "component/statemachine/i_state_machine.h"
#include "component/statemachine/statemachine_defs.h"
#include "component/statemachine/statemachine_statusdefs.h"
#include "motion/test/test_helper.h"
#include <vector>

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
//! @class TestStatemachineGlobal
//! Test class for the global state machine.
class TestStatemachineGlobal : public xTST_Test2
{
public:
  //! Constructor
  TestStatemachineGlobal();
  //! Destructor
  virtual ~TestStatemachineGlobal() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Component.Statemachine");
  TEST_DECLARE_DESCRIPTION("Test of the global state machine of the axisprofile component.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief Init necessary motion system objects on test call.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit() override;

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest() override;

  //! @brief Free memory of allocated objects in doInit() test function.
  //! @return S_OK, when deletion successful.
  HRESULT doEnd() override;

private:
  test::TestHelper m_helper;                                                      //!< test helper
  comm::axisprofile::IStateMachine *m_stateMachine = nullptr;                     //!< test object
  static const std::vector<comm::axisprofile::StateIDType> s_goToStateSequence;   //!< sequence of states for goToState()

  bool m_resultCallback = false;      //!< for test: the result of the callback
  std::uint32_t m_exitCallbackCnt = 0;     //!< for test: counter how often the exit callback was called
  std::uint32_t m_enterCallbackCnt = 0;    //!< for test: counter how often the enter callback was called

  //! @brief save the result of an recursive request
  enum RecRequestResult
  {
    UNSET,      //!< variable has not been set by now
    GOT_TRUE,   //!< variable was set to true
    GOT_FALSE   //!< variable was set to false
  };
  RecRequestResult m_requestResult = UNSET;       //!< for test: save the result of an recursive request

  //! @brief struct for test data
  struct DefinedEdge
  {
    comm::axisprofile::StateIDType m_source;         //!< source state of an edge
    comm::axisprofile::StateIDType m_target;         //!< target state of an edge
    comm::axisprofile::GlobalStateResult m_result;   //!< expected result
  };
  
  //! @brief go to a defined state
  //! @param[in]  stateID  the requested state
  //! @return  S_OK, when tests were successful
  HRESULT goToState(comm::axisprofile::StateIDType stateID );

  //! @brief test defines
  //! @return  S_OK, when tests were successful
  HRESULT testDefines() const;

  //! @brief test names
  //! @return  S_OK, when tests were successful
  HRESULT testNames();

  //! @brief test all transitions
  //! @return  S_OK, when tests were successful
  HRESULT testTransitions();

  //! @brief test callbacks on transitions
  //! @return  S_OK, when tests were successful
  HRESULT testCallbacks();

  //! @brief test recursive transition request
  //! @param[in]  targID      the requested state in the recursive call
  //! @param[in]  expName     name of the expected state after the recursive call
  //! @param[in]  expID       ID of the expected state after the recursive call
  //! @param[in]  expResult   expected result of the recursive call
  //! @return  S_OK, when tests were successful
  HRESULT testRecursiveTransition(comm::axisprofile::StateIDType targID, const wchar_t *expName,
                                  comm::axisprofile::StateIDType expID, RecRequestResult expResult );

  //! @brief test emergency transition
  //! @return  S_OK, when tests were successful
  HRESULT testEmergency();

  //! @brief thread function for emergency transition test (testEmergency())
  void thread1Fct();

  //! @brief little helper to init a test
  //! @param[in]  startState  the requested start state
  //! @return  S_OK, when tests were successful
  HRESULT initTest(comm::axisprofile::StateIDType startState );
};

