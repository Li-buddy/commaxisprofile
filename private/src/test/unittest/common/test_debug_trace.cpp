#include "test_debug_trace.h"

#include "common/debug_trace.h"
#include "mockup/external_bundles/mock_realtime_trace.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestDebugTrace);
REGISTER_TEST_TO_GROUP(TestDebugTrace, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestDebugTrace, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestDebugTrace, Common);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestDebugTrace::TestDebugTrace()
{
}


TestDebugTrace::~TestDebugTrace(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestDebugTrace::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the trace adapter in the axis profile bundle");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  DebugTrace testTrace;
  test::MockTraceLogRT testLog;
  common::log::trace::ILogRealTime3* expectedLogPointer = &testLog;
  test::MockTraceRegistrationRT testRegistration;
  testRegistration.setTestLog(expectedLogPointer);

  log(L"\n<b>Test the registration/deregistration of the trace.");

  log(L"\nCall addTrace() with trace registration mockup.");
  testTrace.addTrace(&testRegistration);
  const common::log::trace::ILogRealTime3* actualLogPointer = testTrace.getTrace();
  if ( actualLogPointer != ( expectedLogPointer ) )
  {
    error(L"addTracebundle did not set correct trace as active trace.");
    hResult = E_FAIL;
  }

  log(L"\nChange trace enabling states via registered callback.");
  testLog.setEnablingState({ true, false, true });

  log(L"\nCheck the enabling states.");
  const common::log::trace::EnablingState* enableSates = testTrace.getEnableStates();
  if ((enableSates->messages != true) || (enableSates->warnings != false) || (enableSates->errors != true))
  {
    error(L"Could not set the trace enabling states!");
    hResult = E_FAIL;
  }

  log(L"\nReset trace enabling states via registered callback.");
  testLog.setEnablingState({ true, true, true });

  log(L"\nCall removeTrace().");
  testTrace.removeTrace(&testRegistration);
  if ( testTrace.getTrace() != testTrace.getBufferedTrace() )
  {
    error(L"removeTrace did not set buffered trace as activ trace.");
    hResult = E_FAIL;
  }

  log(L"\nCall setTrace() of the adapter class to initialize the static variable.");
  testTrace.setTrace(expectedLogPointer);
  testRegistration.setReturnValue(common::log::trace::TraceResult::NOK);

  log(L"\nCall addTrace() and simulate an error in the unit registration.");
  testTrace.addTrace(&testRegistration);
  if ( testTrace.getTrace() != testTrace.getBufferedTrace() )
  {
    error(L"addTracebundle did not set buffered trace as active trace after error.");
    hResult = E_FAIL;
  }

  log(L"\nCall removeTrace().");
  testTrace.removeTrace(&testRegistration);
  if ( testTrace.getTrace() != testTrace.getBufferedTrace() )
  {
    error(L"removeTrace did not set buffered trace as activ trace.");
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}
