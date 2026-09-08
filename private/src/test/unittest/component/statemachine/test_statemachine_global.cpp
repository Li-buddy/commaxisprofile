#include "component/statemachine/state_machine.h"
#include "test_statemachine_global.h"
#include <thread>

using namespace comm::axisprofile;

REGISTER_TEST(TestStatemachineGlobal);
REGISTER_TEST_TO_GROUP(TestStatemachineGlobal, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestStatemachineGlobal, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestStatemachineGlobal, Component);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

TestStatemachineGlobal::TestStatemachineGlobal() : m_helper(this)
{
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestStatemachineGlobal::doInit()
{
  delete m_stateMachine;
  m_stateMachine = new StateMachine();

  return S_OK;
}

HRESULT TestStatemachineGlobal::doEnd()
{
  delete m_stateMachine;
  m_stateMachine = nullptr;

  return S_OK;
}


HRESULT TestStatemachineGlobal::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test for the global state machine statemachine");
  log(L"\n----------------------------------------------------------------");

  char *buffer = new char[1024 * 10];
  if ( !static_cast<StateMachine*>(m_stateMachine)->getDot( buffer, 1024 * 10 ) )
  {
    error( L"\nCan't create dot!" );
  }
  else
  {
    log(L"\n");
    m_helper.plotDot( buffer, L"global state machine" );
  }
  delete[] buffer;

  HRESULT res = S_OK;

  res |= testDefines();
  res |= testNames();
  res |= testTransitions();
  res |= testCallbacks();
  // test a recursive transition (request transition from a callback) to a 'normal' state --> expecting error
  res |= testRecursiveTransition( StateIDType::BOOTING, L"CONFIG", StateIDType::CONFIG, RecRequestResult::GOT_FALSE );
  // test a recursive transition (request transition from a callback) to an 'emergency' state --> expecting success
  res |= testRecursiveTransition( StateIDType::ERROR_STATE, L"ERROR", StateIDType::ERROR_STATE, RecRequestResult::GOT_TRUE );

  res |= testEmergency();

  return res;
}

HRESULT TestStatemachineGlobal::testDefines() const
{
  HRESULT res = S_OK;
  log(L"\n<BLUE><b>Test Statemachine Defs...</b></BLUE>");

  log(L"\nConvert states to string.");
  std::string convString = stateTypeToString(StateIDType::ERROR_STATE);
  if (convString.empty()) { error(L"\nCould not convert StateIDType 'Error' to string!"); res = E_FAIL; }
  convString = stateTypeToString(StateIDType::BOOTING);
  if (convString.empty()) { error(L"\nCould not convert StateIDType 'Booting' to string!"); res = E_FAIL; }
  convString = stateTypeToString(StateIDType::RUNNING);
  if (convString.empty()) { error(L"\nCould not convert StateIDType 'Running' to string!"); res = E_FAIL; }
  convString = stateTypeToString(StateIDType::CONFIG);
  if (convString.empty()) { error(L"\nCould not convert StateIDType 'Config' to string!"); res = E_FAIL; }
  convString = stateTypeToString(static_cast<StateIDType>(99));
  if (!convString.empty()) { error(L"\nCould convert undefined StateIDType to string!"); res = E_FAIL; }

  return res;
}

HRESULT TestStatemachineGlobal::testNames()
{
  log( L"\n<BLUE><b>Test state names...</b></BLUE>" );

  auto &sNames = m_stateMachine->getStateNames();
  log( L"\nStates:" );
  for ( auto& name : sNames )
  {
    log( L"\n\t%ls", test::StringHelper::utf8ToWide( name.c_str() ).c_str() );
  }
  log( L"\n  Expect 4 states..." );
  if ( sNames.size() != 4 )
  {
    error(L"Wrong number of states.");
  }
  else
  {
    log(L"<DARK_GREEN>OK");
  }

  // now test name->ID->name
  for ( auto& name : sNames )
  {
    StateIDType id;
    if ( !m_stateMachine->getStateID(name.c_str(), id) )
    {
      error(L"\nCan't get ID of state by name!");
    }
    const char *stateName = m_stateMachine->getStateName( id );
    log( L"\n  %ls -> %u -> %ls ...", test::StringHelper::utf8ToWide( name.c_str() ).c_str(), static_cast<std::uint32_t>( id ), test::StringHelper::utf8ToWide( name.c_str() ).c_str() );
    if ( !stateName || ( name != stateName ) )
    {
      error( L"\nUnexpected state name!" );
    }
    else
    {
      log( L"<DARK_GREEN><b>success.</b></DARK_GREEN>" );
    }
  }

  log( L"\n  run getStateID() with a nullptr (expecting error)..." );
  StateIDType nullid;
  if ( m_stateMachine->getStateID(nullptr, nullid) )
  {
    error(L"Got success from getStateID() with a nullptr!");
  }

  log( L"\n  try to get state name with invalid ID (expecting error)..." );
  if ( m_stateMachine->getStateName(static_cast<StateIDType>( 12345 )) )
  {
    error(L"Got success from getStateName() with invalid ID!");
  }

  return S_OK;
}


const std::vector<comm::axisprofile::StateIDType> TestStatemachineGlobal::s_goToStateSequence = {
  StateIDType::ERROR_STATE,      // we can go to ERROR from every state
  StateIDType::CONFIG,
  StateIDType::BOOTING,
  StateIDType::RUNNING
};

HRESULT TestStatemachineGlobal::goToState( StateIDType stateID )
{
  if ( m_stateMachine->getState() == stateID )
  {
    return S_OK;
  }

  for ( auto id : s_goToStateSequence )
  {
    if ( m_stateMachine->getState() != id )
    {
      if ( m_stateMachine->requestSwitchToState(id) != GlobalStateResult::STS_OK )
      {
        error(L"\nCan't switch to new state!");
      }
    }
    if ( stateID == id )
    {
      return S_OK;
    }
  }

  return E_FAIL;
}

HRESULT TestStatemachineGlobal::testTransitions()
{
  const std::vector<DefinedEdge> testEdges = {      //!< Test data which transitions should be tested
  { StateIDType::ERROR_STATE, StateIDType::ERROR_STATE, GlobalStateResult::ERR_NO_TRANSITION },
  { StateIDType::ERROR_STATE, StateIDType::BOOTING, GlobalStateResult::ERR_NO_TRANSITION },
  { StateIDType::ERROR_STATE, StateIDType::CONFIG, GlobalStateResult::STS_OK },
  { StateIDType::ERROR_STATE, StateIDType::RUNNING, GlobalStateResult::ERR_NO_TRANSITION },

  { StateIDType::BOOTING, StateIDType::ERROR_STATE, GlobalStateResult::STS_OK },
  { StateIDType::BOOTING, StateIDType::BOOTING, GlobalStateResult::ERR_NO_TRANSITION },
  { StateIDType::BOOTING, StateIDType::CONFIG, GlobalStateResult::STS_OK },
  { StateIDType::BOOTING, StateIDType::RUNNING, GlobalStateResult::STS_OK },

  { StateIDType::CONFIG, StateIDType::ERROR_STATE, GlobalStateResult::STS_OK },
  { StateIDType::CONFIG, StateIDType::BOOTING, GlobalStateResult::STS_OK },
  { StateIDType::CONFIG, StateIDType::CONFIG, GlobalStateResult::ERR_NO_TRANSITION },
  { StateIDType::CONFIG, StateIDType::RUNNING, GlobalStateResult::ERR_NO_TRANSITION },

  { StateIDType::RUNNING, StateIDType::ERROR_STATE, GlobalStateResult::STS_OK },
  { StateIDType::RUNNING, StateIDType::BOOTING, GlobalStateResult::ERR_NO_TRANSITION },
  { StateIDType::RUNNING, StateIDType::CONFIG, GlobalStateResult::STS_OK },
  { StateIDType::RUNNING, StateIDType::RUNNING, GlobalStateResult::ERR_NO_TRANSITION }
  };        

  log( L"\n<BLUE><b>Test all transitions...</b></BLUE>" );
  for ( auto& test : testEdges )
  {
    // go to the start state
    if ( FAILED(goToState(test.m_source)) )
    {
      return E_FAIL;
    }

    // request the state change
    GlobalStateResult res = m_stateMachine->requestSwitchToState(test.m_target);

    // check the result
    if ( test.m_result != res )
    {
      error(L"\nUnexpected result on state change request!");
      return E_FAIL;
    }
  }

  log( L"<DARK_GREEN><b>success.</b></DARK_GREEN>" );

  log( L"\n  Try to request switch to an invalid state ID (expecting error)..." );
  GlobalStateResult res = m_stateMachine->requestSwitchToState( static_cast<StateIDType>( 123456 ) );
  // check the result
  if ( GlobalStateResult::ERR_INVALID_PARAMETER != res )
  {
    error(L"\nGot success when requesting switch to an invalid state ID!");
    return E_FAIL;
  }

  return S_OK;
}


HRESULT TestStatemachineGlobal::initTest( StateIDType startState )
{
  m_resultCallback = true;
  if ( FAILED(goToState(startState)) )
  {
    return E_FAIL;
  }

  m_enterCallbackCnt = 0;
  m_exitCallbackCnt = 0;
  m_resultCallback = true;

  return S_OK;
}

HRESULT TestStatemachineGlobal::testCallbacks()
{
  log( L"\n<BLUE><b>Test callbacks on transitions...</b></BLUE>" );

  // add some callbacks
  IStateMachine::CallbackExit exitFct = [self = this]( StateIDType /*srcStateID*/, StateIDType /*trgStateID*/ ) { self->m_exitCallbackCnt++; return self->m_resultCallback; };
  IStateMachine::CallbackEnter enterFct = [self = this]( StateIDType /*srcStateID*/, StateIDType /*trgStateID*/ ) { self->m_enterCallbackCnt++; };

  log( L"\n  Try to set an exit callback to an invalid state ID (expecting error)..." );
  bool res = m_stateMachine->addCallback( static_cast<StateIDType>( 123456 ), exitFct );
  if ( res )
  {
    error(L"\nGot success when setting an exit callback to an invalid state ID!");
  }
  log( L"\n  Try to set an enter callback to an invalid state ID (expecting error)..." );
  res = m_stateMachine->addCallback( static_cast<StateIDType>( 123456 ), enterFct );
  if ( res )
  {
    error(L"\nGot success when setting an enter callback to an invalid state ID!");
  }

  log( L"\n  Set exit callback to a valid state RUNNING (expecting success)..." );
  res = m_stateMachine->addCallback( StateIDType::RUNNING, exitFct );
  if ( !res )
  {
    error(L"\nCan't set callback!");
    return E_FAIL;
  }
  log( L"\n  Set enter callback to a valid state CONFIG (expecting success)..." );
  res = m_stateMachine->addCallback( StateIDType::CONFIG, enterFct );
  if ( !res )
  {
    error(L"\nCan't set callback!");
    return E_FAIL;
  }

  if ( FAILED(initTest(StateIDType::RUNNING)) )
  {
    error(L"\nInitialisation failed!");
    return E_FAIL;
  }
  m_resultCallback = false;
  log( L"\n  Try transition to PARAM with exit callback returning false (expecting error)..." );
  GlobalStateResult result = m_stateMachine->requestSwitchToState( StateIDType::CONFIG );
  if ( result != GlobalStateResult::ERR_CB_CANCEL )
  {
    error(L"\nGot success with exit callback returning false!");
  }
  log( L"\n  Check if we are in still state RUNNING..." );
  if ( StateIDType::RUNNING != m_stateMachine->getState() )
  {
    error(L"\nUnexpected state with exit callback returning false!");
  }
  log( L"\n  Check enter and exit counter (expecting 0 and 1)..." );
  if ( 1 != m_exitCallbackCnt )
  {
    error(L"\nUnexpected result in exit callback counter!");
  }
  if ( 0 != m_enterCallbackCnt )
  {
    error(L"\nUnexpected result in enter callback counter!");
  }

  if ( FAILED(initTest(StateIDType::RUNNING)) )
  {
    error(L"\nInitialisation failed!");
    return E_FAIL;
  }
  log( L"\n  Try transition to PARAM with exit callback returning true (expecting success)..." );
  result = m_stateMachine->requestSwitchToState( StateIDType::CONFIG );
  if ( GlobalStateResult::STS_OK != result )
  {
    error(L"\nGot error with exit callback returning true!");
  }
  log( L"\n  Check if we are now in state PARAM..." );
  if ( StateIDType::CONFIG != m_stateMachine->getState() )
  {
    error(L"\nUnexpected state with exit callback returning true!");
  }
  log( L"\n  Check enter and exit counter (expecting 1 and 1)..." );
  if ( 1 != m_exitCallbackCnt )
  {
    error(L"\nUnexpected result in exit callback counter!");
  }
  if ( 1 != m_enterCallbackCnt )
  {
    error(L"\nUnexpected result in enter callback counter!");
  }

  return S_OK;
}

HRESULT TestStatemachineGlobal::testRecursiveTransition( StateIDType targID, const wchar_t *expName, StateIDType expID, RecRequestResult expResult)
{
  log( L"\n<BLUE><b>Test recursive callbacks on transitions...</b></BLUE>" );

  // recreate the state machine, to get rid of all callbacks
  delete m_stateMachine;
  m_stateMachine = new StateMachine();

  if ( FAILED(initTest(StateIDType::RUNNING)) )
  {
    error(L"\nInitialisation failed!");
    return E_FAIL;
  }

  // now register an enter function, that requests a state change
  IStateMachine::CallbackEnter enterFct = [self = this, targID]( StateIDType /*srcStateID*/, StateIDType /*trgStateID*/ )
  {
    if ( GlobalStateResult::STS_OK == self->m_stateMachine->requestSwitchToState( targID ) )
    {
      self->m_requestResult = RecRequestResult::GOT_TRUE;
    }
    else
    {
      self->m_requestResult = RecRequestResult::GOT_FALSE;
    }
  };

  m_requestResult = UNSET;
  bool res = m_stateMachine->addCallback( StateIDType::CONFIG, enterFct );
  if ( !res )
  {
    error(L"\nCan't add enter callback!");
  }

  log( L"\n  Try transition to PARAM (expecting success)..." );
  GlobalStateResult result = m_stateMachine->requestSwitchToState( StateIDType::CONFIG );
  if ( GlobalStateResult::STS_OK != result )
  {
    error(L"\nGot error switching state!");
  }
  log( L"\n  Check if we are now in %ls...", expName );
  if ( m_stateMachine->getState() != expID )
  {
    error(L"\nUnexpected state with exit callback returning true!");
  }
  log( L"\n  Check if the callback request received false (for recursive state change request)..." );
  if ( m_requestResult != expResult )
  {
    error(L"\nUnexpected result in callback request!");
  }

  return S_OK;
}

HRESULT TestStatemachineGlobal::testEmergency()
{
  log( L"\n<BLUE><b>Test recursive callbacks on transitions...</b></BLUE>" );

  // recreate the state machine, to get rid of all callbacks
  delete m_stateMachine;
  m_stateMachine = new StateMachine();

  if ( FAILED(initTest(StateIDType::RUNNING)) )
  {
    error(L"\nInitialisation failed!");
    return E_FAIL;
  }

  // have two threads:
  //   thread 1: start a state change, but take a lot of time
  //   thread 2 (this): request a state change to emergency state
  // expected:
  //   thread 2 finishes and instantly switched to emergency state (although transition is thread 1 is still in progress)

  IStateMachine::CallbackExit exitFct = [self = this]( StateIDType /*srcStateID*/, StateIDType /*trgStateID*/ )
  {
    self->log( L"\n<DARK_GREEN><i>  I'm in the callback and take a lot of time to simulate a slow state transition...</i></DARK_GREEN>" );
    self->m_exitCallbackCnt++;
    // wait until the main thread set the variable back
    std::uint32_t timeOut = 100;
    while ( (self->m_exitCallbackCnt > 0) && (timeOut > 0) )
    {
      std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
      timeOut--;
    }
    self->log( L"\n<DARK_GREEN><i>  Exit the callback...</i></DARK_GREEN>" );
    return true;
  };
  if ( !m_stateMachine->addCallback(StateIDType::RUNNING, exitFct) )
  {
    error(L"\nCan't add enter callback!");
  }

  m_exitCallbackCnt = 0;
  std::thread thread1(&TestStatemachineGlobal::thread1Fct, this);

  log( L"\n  Wait, until we are in the exit callback of thread1..." );
  std::uint32_t timeOut = 100;
  while ( ( m_exitCallbackCnt == 0 ) && ( timeOut > 0 ) )
  {
    std::this_thread::sleep_for( std::chrono::milliseconds( 2 ) );
    timeOut--;
  }

  log( L"\n  Now try switch to emergency state..." );
  GlobalStateResult result = m_stateMachine->requestSwitchToState( StateIDType::ERROR_STATE );
  if ( GlobalStateResult::STS_OK != result )
  {
    error(L"\nGot error switching state!");
  }
  log( L"\n  Check if we are now in ERROR..." );
  if ( StateIDType::ERROR_STATE != m_stateMachine->getState() )
  {
    error(L"\nUnexpected state with exit callback returning true!");
  }

  log( L"\n  Signal thread 1 to finish..." );
  m_exitCallbackCnt = 0;
  thread1.join();

  return S_OK;
}

void TestStatemachineGlobal::thread1Fct()
{
  log( L"\n<GREY><i>  Thread1 running</i></GREY>" );
  log( L"\n<GREY><i>  Try transition to PARAM (expecting success)...</i></GREY>" );
  GlobalStateResult result = m_stateMachine->requestSwitchToState( StateIDType::CONFIG );
  if ( GlobalStateResult::STS_OK != result )
  {
    error(L"\nGot error switching state!");
  }

  log( L"\n<GREY><i>  Now expecting to be in state ERROR...</i></GREY>" );
  if ( StateIDType::ERROR_STATE != m_stateMachine->getState() )
  {
    error(L"\nUnexpected state");
  }

  log( L"\n<GREY><i>  Thread1 leaving</i></GREY>" );
}
